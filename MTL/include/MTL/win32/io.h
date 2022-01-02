#pragma once

#include "mtl/sdk.h"
#include "mtl/win32/mem.h"
#include <windows.h>
#include <time.h>


namespace mtl
{
    inline void time_t_to_file_time(time_t t, FILETIME& ft)
    {
        LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
        ft.dwLowDateTime = (DWORD)ll;
        ft.dwHighDateTime = ll >> 32;
    }

    inline time_t file_time_to_time_t(const FILETIME& ft )
    {
        std::time_t ret = 0;
        ULARGE_INTEGER ull;
        ull.LowPart = ft.dwLowDateTime;
        ull.HighPart = ft.dwHighDateTime;
        ret = (ull.QuadPart / 10000000ULL - 11644473600ULL);
        return ret;
    }


    namespace detail {

        class ReadOverLap : public OVERLAPPED
        {
        public:

            ReadOverLap(HANDLE handle, unsigned long long size)
            {
                ::ZeroMemory(this, sizeof(OVERLAPPED));

                this->hEvent = handle;
                fsize = size;

                buf.reset(new cbuff(fsize));
            }

            std::string str(int size)
            {
                return buf->toString(size);
            }

            unsigned long long fsize = 0;
            std::unique_ptr<cbuff> buf;

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

    class file
    {
    public:

        file(
            DWORD access = GENERIC_READ,
            DWORD share = FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
            DWORD flags = FILE_ATTRIBUTE_NORMAL
        )
            : access_flags(access),
            share_flags(share),
            create_flags(flags)
        {}

        file(HANDLE f)
        {
            handle = f;
            if (handle != INVALID_HANDLE_VALUE)
            {
                ::ZeroMemory(&fi_, sizeof(fi_));

                BOOL r = ::GetFileInformationByHandle(handle, &fi_);

                ULARGE_INTEGER uli;
                uli.HighPart = fi_.nFileSizeHigh;
                uli.LowPart = fi_.nFileIndexLow;

                fsize = uli.QuadPart;
            }
        }

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
                ::ZeroMemory(&fi_, sizeof(fi_));

                BOOL r = ::GetFileInformationByHandle(handle, &fi_);

                ULARGE_INTEGER uli;
                uli.HighPart = fi_.nFileSizeHigh;
                uli.LowPart = fi_.nFileIndexLow;

                fsize = uli.QuadPart;
                return ERROR_SUCCESS;
            }
            return ::GetLastError();
        }


        file(const file& rhs)
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

        file(file&& rhs)
            : handle(rhs.handle)
        {
            rhs.handle = INVALID_HANDLE_VALUE;
        }

        file& operator=(const file& rhs)
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

        file& operator=(file&& rhs)
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

        const file_info& info() const
        {
            return fi_;
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

            wbuff buf(2048);
            ::GetFinalPathNameByHandle(handle, buf, (DWORD)buf.size(), 0);
            return buf.toString();
        }

        ~file()
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
            cbuff buf(fsize);

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
        mtl::file_info fi_;
    };

    inline std::string slurp(
        const std::wstring& file_name,
        DWORD access = GENERIC_READ,
        DWORD share = FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
        DWORD flags = FILE_ATTRIBUTE_NORMAL)
    {
        file f(access, share, flags);

        if (f.open(file_name) != ERROR_SUCCESS)
        {
            return "";
        }

        return f.content();
    }


    inline bool spit(const std::wstring& file_name, const std::string data)
    {
        file f(GENERIC_WRITE);
        if (f.open(file_name, OPEN_ALWAYS | TRUNCATE_EXISTING) != ERROR_SUCCESS)
        {
            return false;
        }
        if (!f.content(data))
        {
            return false;
        }
        return true;
    }

    inline bool slurp(
        const std::wstring& file_name,
        std::function<void(DWORD,std::string)> cb,
        DWORD access = GENERIC_READ,
        DWORD share = FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
        DWORD flags = FILE_FLAG_OVERLAPPED)
    {
        file f(access, share, flags);
        if (f.open(file_name) != ERROR_SUCCESS)
        {
            return false;
        }

        return f.async_content(cb) == ERROR_SUCCESS;
    }


    inline bool spit(const std::wstring& file_name, const std::string data, std::function<void(DWORD)> cb)
    {
        file f(GENERIC_WRITE);
        if (f.open(file_name, OPEN_ALWAYS | TRUNCATE_EXISTING) != ERROR_SUCCESS)
        {
            return false;
        }
        if (f.async_content(data, cb) != ERROR_SUCCESS)
        {
            return false;
        }
        return true;
    }

    class Process
    {
    public:

        Process()
        {}

        ~Process()
        {
            dispose();
        }

        Process(Process&& rhs)
            : hChildStd_IN_Rd(rhs.hChildStd_IN_Rd),
            hChildStd_IN_Wr(rhs.hChildStd_IN_Wr),
            hChildStd_OUT_Rd(rhs.hChildStd_OUT_Rd),
            hChildStd_OUT_Wr(rhs.hChildStd_OUT_Wr)
        {
            rhs.hChildStd_IN_Rd = nullptr;
            rhs.hChildStd_IN_Wr = nullptr;
            rhs.hChildStd_OUT_Rd = nullptr;
            rhs.hChildStd_OUT_Wr = nullptr;
        }

        Process& operator=(Process&& rhs)
        {
            if (this == &rhs)
            {
                return *this;
            }

            hChildStd_IN_Rd = rhs.hChildStd_IN_Rd;
            hChildStd_IN_Wr = rhs.hChildStd_IN_Wr;
            hChildStd_OUT_Rd = rhs.hChildStd_OUT_Rd;
            hChildStd_OUT_Wr = rhs.hChildStd_OUT_Wr;

            rhs.hChildStd_IN_Rd = nullptr;
            rhs.hChildStd_IN_Wr = nullptr;
            rhs.hChildStd_OUT_Rd = nullptr;
            rhs.hChildStd_OUT_Wr = nullptr;

            return *this;
        }

        void dispose()
        {
            ::CloseHandle(hChildStd_IN_Rd);
            ::CloseHandle(hChildStd_IN_Wr);
            ::CloseHandle(hChildStd_OUT_Rd);
            ::CloseHandle(hChildStd_OUT_Wr);
        }

        void stdIn(const std::wstring& str)
        {
            stdIn_ = str;
        }

        static std::wstring exec(const std::wstring& cli)
        {
            Process p;
            return p.execute(cli);
        }

        static void exec(const std::wstring& cli, std::function<void(std::wstring)> cb)
        {
            Process p;
            p.execute(cli, cb);
        }

        static std::wstring exec(const std::wstring& cli, const std::wstring& stdIn)
        {
            Process p;
            p.stdIn(stdIn);
            return p.execute(cli);
        }

        static void exec(const std::wstring& cli, const std::wstring& stdIn, std::function<void(std::wstring)> cb)
        {
            Process p;
            p.stdIn(stdIn);
            p.execute(cli, cb);
        }

        std::wstring execute(const std::wstring& cli)
        {
            createChildProcess(cli.c_str());
            writeToPipe();
            return mtl::to_wstring(readFromPipe());
        }

        void execute(const std::wstring& cli, std::function<void(std::wstring)> cb)
        {
            createChildProcess(cli.c_str());
            writeToPipe();
            readFromPipeAsync(cb);
        }

    private:

        std::wstring stdIn_;

        Process(const Process& rhs) = delete;
        Process& operator=(const Process& rhs) = delete;

        void createChildProcess(const std::wstring& cli)
        {
            SECURITY_ATTRIBUTES saAttr;
            saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
            saAttr.bInheritHandle = TRUE;
            saAttr.lpSecurityDescriptor = NULL;

            if (!::CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0))
                throw std::runtime_error("StdoutRd CreatePipe");

            if (!::SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
                throw  std::runtime_error("Stdout SetHandleInformation");

            if (!::CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0))
                throw  std::runtime_error("Stdin CreatePipe");

            if (!::SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
                throw  std::runtime_error("Stdin SetHandleInformation");

            PROCESS_INFORMATION piProcInfo;
            ::ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

            STARTUPINFO siStartInfo;
            ::ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));

            siStartInfo.cb = sizeof(STARTUPINFO);
            siStartInfo.hStdError = hChildStd_OUT_Wr;
            siStartInfo.hStdOutput = hChildStd_OUT_Wr;
            siStartInfo.hStdInput = hChildStd_IN_Rd;
            siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

            std::wostringstream woss;
            wchar_t* env = ::GetEnvironmentStrings();
            while (*env)
            {
                woss << env;
                woss.write(L"\0", 1);
                env += wcslen(env) + 1;
            }

            woss << L"LC_ALL=C.UTF-8";
            woss.write(L"\0\0", 2);

            std::wstring envBlock = woss.str();

           // ::FreeEnvironmentStrings(env);

            BOOL bSuccess = FALSE;
            bSuccess = ::CreateProcess(NULL,
                (LPWSTR)cli.c_str(),     // command line
                NULL,          // process security attributes
                NULL,          // primary thread security attributes
                TRUE,          // handles are inherited
                CREATE_UNICODE_ENVIRONMENT,             // creation flags
                (LPVOID)envBlock.c_str(),          // use parent's environment
                NULL,          // use parent's current directory
                &siStartInfo,  // STARTUPINFO pointer
                &piProcInfo);  // receives PROCESS_INFORMATION

            if (!bSuccess)
            {
                throw  std::runtime_error("CreateProcess");
            }
            else
            {
                ::CloseHandle(piProcInfo.hProcess);
                ::CloseHandle(piProcInfo.hThread);
            }
        }

        void writeToPipe(void)
        {
            DWORD dwWritten = 0;
            BOOL bSuccess = FALSE;

            if (!stdIn_.empty())
            {
                std::string raw = mtl::to_string(stdIn_);

                for (;;)
                {
                    bSuccess = ::WriteFile(hChildStd_IN_Wr, raw.c_str(), raw.size(), &dwWritten, NULL);
                    if (!bSuccess) break;
                }
            }

            // Close the pipe handle so the child process stops reading.
            if (!::CloseHandle(hChildStd_IN_Wr))
                throw  std::runtime_error("StdInWr CloseHandle");

            hChildStd_IN_Wr = nullptr;
        }

        std::string  readFromPipe(void)
        {
            const int BUFSIZE = 4096;
            DWORD dwRead = 0;
            CHAR chBuf[BUFSIZE];
            BOOL bSuccess = FALSE;

            std::ostringstream oss;
            for (;;)
            {
                bSuccess = ::ReadFile(hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
                if (!bSuccess || dwRead == 0) break;

                oss.write(chBuf, dwRead);

                if (dwRead < BUFSIZE) break;
            }

            return oss.str();
        }

        void readFromPipeAsync(std::function<void(std::wstring)> cb)
        {
            mtl::file file(hChildStd_OUT_Rd);

            hChildStd_OUT_Rd = nullptr;

            file.async_content([cb](DWORD e, std::string s)
            {
                if (e != ERROR_SUCCESS)
                {
                    cb(L"");
                    return;
                }

                cb(mtl::to_wstring(s));
            });
        }

        HANDLE hChildStd_IN_Rd = nullptr;
        HANDLE hChildStd_IN_Wr = nullptr;
        HANDLE hChildStd_OUT_Rd = nullptr;
        HANDLE hChildStd_OUT_Wr = nullptr;

        std::ostringstream out;
    };
}

