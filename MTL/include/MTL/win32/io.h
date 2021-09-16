#pragma once

#include "MTL/sdk.h"
#include "MTL/win32/mem.h"

namespace MTL
{
    namespace detail {

        class ReadOverLap : public OVERLAPPED
        {
        public:

            ReadOverLap(HANDLE handle, unsigned long long size)
            {
                ::ZeroMemory(this, sizeof(OVERLAPPED));

                this->hEvent = handle;
                fsize = size;

                buf.reset(new MTL::cbuff(fsize));
            }

            std::string str(int size)
            {
                return buf->toString(size);
            }

            unsigned long long fsize = 0;
            std::unique_ptr<MTL::cbuff> buf;

            std::function<void(DWORD e, std::string)> onRead;

            static void overlappedCompletionRoutine(
                DWORD dwErrorCode,
                DWORD dwNumberOfBytesTransfered,
                LPOVERLAPPED lpOverlapped
            )
            {
                ReadOverLap* overlap = (ReadOverLap*)lpOverlapped;
                auto cb = overlap->onRead;

                if (dwErrorCode == ERROR_SUCCESS)
                {
                    std::string bytes = overlap->str(dwNumberOfBytesTransfered);
                    ::CloseHandle(overlap->hEvent);
                    delete overlap;

                    if (cb)
                    {
                        cb(ERROR_SUCCESS,bytes);
                    }
                }
                else
                {
                    cb(dwErrorCode, "");
                    ::CloseHandle(overlap->hEvent);
                    delete overlap;
                }
            };
        };


        class WriteOverLap : public OVERLAPPED
        {
        public:

            WriteOverLap(HANDLE handle, const std::string& d)
            {
                ::ZeroMemory(this, sizeof(OVERLAPPED));

                this->hEvent = handle;
                data = d;
            }

            static const size_t BUFSIZE = 4096;

            std::string data;
            size_t written = 0;

            std::function<void(DWORD)> onWritten;

            static void overlappedCompletionRoutine(
                DWORD dwErrorCode,
                DWORD dwNumberOfBytesTransfered,
                LPOVERLAPPED lpOverlapped
            )
            {
                WriteOverLap* overlap = (WriteOverLap*)lpOverlapped;

                if (dwErrorCode == ERROR_SUCCESS)
                {
                    overlap->written += dwNumberOfBytesTransfered;
                    auto cb = overlap->onWritten;

                    DWORD toWrite = (DWORD)overlap->data.size() - (DWORD)overlap->written;
                    if (toWrite > WriteOverLap::BUFSIZE)
                    {
                        toWrite = WriteOverLap::BUFSIZE;
                    }

                    if (overlap->written < overlap->data.size())
                    {
                        BOOL r = ::WriteFileEx(
                            overlap->hEvent,
                            overlap->data.data() + overlap->written,
                            toWrite,
                            overlap,
                            &WriteOverLap::overlappedCompletionRoutine
                        );
                        if (!r)
                        {
                            if (cb)
                            {
                                DWORD e = ::GetLastError();
                                cb(e);
                            }

                            ::CloseHandle(overlap->hEvent);
                            delete overlap;
                        }
                    }
                    else
                    {
                        auto cb = overlap->onWritten;
                        ::CloseHandle(overlap->hEvent);
                        delete overlap;

                        if (cb)
                        {
                            cb(ERROR_SUCCESS);
                        }
                    }
                }
                else
                {
                    auto cb = overlap->onWritten;
                    cb(dwErrorCode);
                    ::CloseHandle(overlap->hEvent);
                    delete overlap;
                }
            };
        };
    }

    class File
    {
    public:

        File(
            DWORD access = GENERIC_READ,
            DWORD share = FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
            DWORD flags = FILE_ATTRIBUTE_NORMAL
        )
            : access_flags(access),
            share_flags(share),
            create_flags(flags)
        {}

        DWORD open(const std::wstring& file_path, DWORD mode = OPEN_ALWAYS)
        {
            handle = ::CreateFile(
                file_path.c_str(),
                access_flags,
                share_flags,
                nullptr,
                mode,
                create_flags, //FILE_FLAG_OVERLAPPED
                nullptr
            );

            if (handle != INVALID_HANDLE_VALUE)
            {
                BY_HANDLE_FILE_INFORMATION fi;
                ::ZeroMemory(&fi, sizeof(fi));

                BOOL r = ::GetFileInformationByHandle(handle, &fi);

                ULARGE_INTEGER uli;
                uli.HighPart = fi.nFileSizeHigh;
                uli.LowPart = fi.nFileIndexLow;

                fsize = uli.QuadPart;
                return ERROR_SUCCESS;
            }
            return ::GetLastError();
        }


        File(const File& rhs)
        {
            if (rhs.handle != INVALID_HANDLE_VALUE)
            {
                access_flags = rhs.access_flags;
                share_flags = rhs.share_flags;
                create_flags = rhs.create_flags;

                std::wstring p = rhs.path();

                open(p);
            }
        }

        File(File&& rhs)
            : handle(rhs.handle)
        {
            rhs.handle = INVALID_HANDLE_VALUE;
        }

        File& operator=(const File& rhs)
        {
            if (this == &rhs)
            {
                return *this;
            }

            close();
            if (rhs.handle != INVALID_HANDLE_VALUE)
            {
                access_flags = rhs.access_flags;
                share_flags = rhs.share_flags;
                create_flags = rhs.create_flags;

                std::wstring p = rhs.path();

                open(p);
            }

            return *this;
        }

        File& operator=(File&& rhs)
        {
            if (this == &rhs)
            {
                return *this;
            }

            close();
            if (rhs.handle != INVALID_HANDLE_VALUE)
            {
                handle = rhs.handle;
                rhs.handle = INVALID_HANDLE_VALUE;
            }

            return *this;
        }

        unsigned long long pos()
        {
            LARGE_INTEGER distance;
            LARGE_INTEGER result;
            distance.QuadPart = 0;
            result.QuadPart = 0;

            ::SetFilePointerEx(handle, distance, &result, FILE_CURRENT);
            return result.QuadPart;
        }

        void pos(unsigned long long p)
        {
            LARGE_INTEGER distance;
            distance.QuadPart = p;
            ::SetFilePointerEx(handle, distance, nullptr, FILE_BEGIN);
        }

        void advance(unsigned long long p)
        {
            LARGE_INTEGER distance;
            distance.QuadPart = p;
            ::SetFilePointerEx(handle, distance, nullptr, FILE_CURRENT);
        }

        std::wstring path() const
        {
            if (handle == INVALID_HANDLE_VALUE)
            {
                return L"";
            }

            MTL::wbuff buf(2048);
            ::GetFinalPathNameByHandle(handle, buf, (DWORD)buf.size(), 0);
            return buf.toString();
        }

        ~File()
        {
            close();
        }

        operator bool()
        {
            return handle != INVALID_HANDLE_VALUE;
        }

        HANDLE operator*()
        {
            return handle;
        }

        void detach()
        {
            handle = INVALID_HANDLE_VALUE;
        }

        void close()
        {
            if (handle != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(handle);
                handle = INVALID_HANDLE_VALUE;
            }
        }

        unsigned long long size() const
        {
            return fsize;
        }

        DWORD access() const
        {
            return access_flags;
        }

        DWORD share() const
        {
            return share_flags;
        }

        DWORD flags() const
        {
            return create_flags;
        }

        std::string content()
        {
            MTL::cbuff buf(fsize);

            DWORD nRead = 0;
            BOOL r = ::ReadFile(
                handle,
                buf,
                (DWORD)buf.size(),
                &nRead,
                nullptr
            );

            if (!r)
            {
                return "";
            }

            return buf.toString(nRead);
        }

        bool content(const std::string& data)
        {
            DWORD written = 0;
            BOOL r = ::WriteFile(
                handle,
                data.data(),
                (DWORD)data.size(),
                &written,
                nullptr
            );
            if (!r)
            {
                DWORD e = ::GetLastError();
                return false;
            }
            if (written != data.size()) return false;
            return true;
        }

        DWORD async_content(std::function<void(DWORD,std::string)> cb)
        {
            detail::ReadOverLap* overlap = new detail::ReadOverLap(handle, fsize);
            overlap->onRead = cb;

            char* buf = *overlap->buf;
            DWORD nRead = 0;
            BOOL r = ::ReadFileEx(
                handle,
                buf,
                (DWORD)overlap->buf.get()->size(),
                overlap,
                &detail::ReadOverLap::overlappedCompletionRoutine
            );

            if (!r)
            {
                DWORD e = ::GetLastError();
                cb(e, "");
                return e;
            }

            detach();

            return ERROR_SUCCESS;
        }

        DWORD async_content(const std::string& data, std::function<void(DWORD)> cb)
        {
            detail::WriteOverLap* overlap = new detail::WriteOverLap(handle, data);
            overlap->onWritten = cb;

            DWORD toWrite = (DWORD)data.size();
            if (toWrite > detail::WriteOverLap::BUFSIZE)
            {
                toWrite = detail::WriteOverLap::BUFSIZE;
            }

            DWORD nRead = 0;
            BOOL r = ::WriteFileEx(
                handle,
                overlap->data.data(),
                toWrite,
                overlap,
                &detail::WriteOverLap::overlappedCompletionRoutine
            );

            if (!r)
            {
                DWORD e = ::GetLastError();
                cb(e);
                delete overlap;
                return e;
            }

            detach();

            return ERROR_SUCCESS;
        }

    private:
        DWORD access_flags = GENERIC_READ;
        DWORD share_flags = FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE;
        DWORD create_flags = FILE_ATTRIBUTE_NORMAL;
        HANDLE handle = INVALID_HANDLE_VALUE;
        unsigned long long fsize = 0;

    };

    std::string slurp(
        const std::wstring& file_name,
        DWORD access = GENERIC_READ,
        DWORD share = FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
        DWORD flags = FILE_ATTRIBUTE_NORMAL)
    {
        File file(access, share, flags);

        if (file.open(file_name) != ERROR_SUCCESS)
        {
            return "";
        }

        return file.content();
    }


    bool spit(const std::wstring& file_name, const std::string data)
    {
        File file(GENERIC_WRITE);
        if (file.open(file_name, OPEN_ALWAYS | TRUNCATE_EXISTING) != ERROR_SUCCESS)
        {
            return false;
        }
        if (!file.content(data))
        {
            return false;
        }
        return true;
    }

    bool slurp(
        const std::wstring& file_name,
        std::function<void(DWORD,std::string)> cb,
        DWORD access = GENERIC_READ,
        DWORD share = FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
        DWORD flags = FILE_FLAG_OVERLAPPED)
    {
        File file(access, share, flags);
        if (file.open(file_name) != ERROR_SUCCESS)
        {
            return false;
        }

        return file.async_content(cb) == ERROR_SUCCESS;
    }


    bool spit(const std::wstring& file_name, const std::string data, std::function<void(DWORD)> cb)
    {
        File file(GENERIC_WRITE);
        if (file.open(file_name, OPEN_ALWAYS | TRUNCATE_EXISTING) != ERROR_SUCCESS)
        {
            return false;
        }
        if (file.async_content(data, cb) != ERROR_SUCCESS)
        {
            return false;
        }
        return true;
    }


}

