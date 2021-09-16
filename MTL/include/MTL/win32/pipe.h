#pragma once

#include "MTL/ole/shell.h"

namespace MTL {


    class NamedPipe
    {
    public:

        static const int BUFSIZE = 4096;

        NamedPipe()
            :pipe_(INVALID_HANDLE_VALUE)
        {}

        NamedPipe(HANDLE h)
            :pipe_(h)
        {}

        ~NamedPipe()
        {
            close();
        }

        NamedPipe(const NamedPipe& rhs) = delete;
        NamedPipe& operator=(const NamedPipe& rhs) = delete;

        NamedPipe(NamedPipe&& rhs) noexcept
        {
            pipe_ = rhs.pipe_;
            rhs.pipe_ = INVALID_HANDLE_VALUE;
        }

        NamedPipe& operator=(NamedPipe&& rhs) noexcept
        {
            if (this == &rhs)
            {
                return *this;
            }

            close();
            pipe_ = rhs.pipe_;
            rhs.pipe_ = INVALID_HANDLE_VALUE;

            return *this;
        }

        operator bool()
        {
            return pipe_ != INVALID_HANDLE_VALUE;
        }

        HANDLE detach()
        {
            HANDLE result = pipe_;
            pipe_ = INVALID_HANDLE_VALUE;
            return result;
        }

        HANDLE operator*() const
        {
            return pipe_;
        }

        static NamedPipe create(const std::wstring& pipename)
        {
            HANDLE handle = CreateNamedPipe(
                pipename.c_str(),             // pipe name 
                PIPE_ACCESS_DUPLEX,			// read/write access 
                PIPE_TYPE_BYTE |				// message type pipe 
                PIPE_READMODE_BYTE |			// message-read mode 
                PIPE_WAIT,					// blocking mode 
                PIPE_UNLIMITED_INSTANCES,		// max. instances  
                BUFSIZE,						// output buffer size 
                BUFSIZE,						// input buffer size 
                0,							// client time-out 
                NULL);						// default security attribute 

            return NamedPipe(handle);
        }

        static NamedPipe open(const std::wstring& pipename)
        {

            if (!::WaitNamedPipe(pipename.c_str(), 20000))
            {
                return INVALID_HANDLE_VALUE;
            }

            // Try to open named pipe; wait for it, if necessary. 

            while (1)
            {
                HANDLE handle = ::CreateFile(
                    pipename.c_str(),   // pipe name 
                    GENERIC_READ |  // read and write access 
                    GENERIC_WRITE,
                    0,              // no sharing 
                    NULL,           // default security attributes
                    OPEN_EXISTING,  // opens existing pipe 
                    0,              // default attributes 
                    NULL);          // no template file 

                // all OK ? 
                if (handle != INVALID_HANDLE_VALUE)
                    return NamedPipe(handle);

                // Exit if an error other than ERROR_PIPE_BUSY occurs. 

                if (::GetLastError() != ERROR_PIPE_BUSY)
                {
                    return NamedPipe(INVALID_HANDLE_VALUE);
                }

                // All pipe instances are busy, so wait for 20 seconds. 	 
                if (!::WaitNamedPipe(pipename.c_str(), 20000))
                {
                    return NamedPipe(INVALID_HANDLE_VALUE);
                }
            }

            // never get here
            return NamedPipe(INVALID_HANDLE_VALUE);
        }

        bool connect(int milisecs = 3000)
        {
            return  ::ConnectNamedPipe(pipe_, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        }

        void disconnect()
        {
            ::DisconnectNamedPipe(pipe_);
        }

        void close()
        {
            if (pipe_ != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(pipe_);
            }
            pipe_ = INVALID_HANDLE_VALUE;
        }


        std::string read()
        {

            // Read client requests from the pipe. 
            char buf[BUFSIZE];
            DWORD cbBytesRead = 0;

            std::ostringstream oss;
            while (1)
            {
                BOOL b = ::ReadFile(
                    pipe_,        // handle to pipe 
                    buf,    // buffer to receive data 
                    BUFSIZE, // size of buffer 
                    &cbBytesRead, // number of bytes read 
                    NULL);        // not overlapped I/O 

                if (!b)
                {
                    //cry();
                    break;
                }

                if (cbBytesRead == 0)
                    break;

                oss.write(buf, cbBytesRead);
                if (cbBytesRead < BUFSIZE)
                    break;
            }
            return oss.str();
        }

        bool write(const std::string& data)
        {

            DWORD cbWritten = 0;
            size_t cnt = 0;

            while ((data.size() - cnt) > BUFSIZE)
            {
                BOOL b = ::WriteFile(
                    pipe_,                  // pipe handle 
                    data.c_str() + cnt,       // message 
                    BUFSIZE,			    // message length 
                    &cbWritten,             // bytes written 
                    NULL);                  // not overlapped 
                if (!b)
                {
                    //cry();
                    return false;
                }
                cnt += cbWritten;
            }

            if ((data.size() - cnt + cbWritten) > 0)
            {
                BOOL b = ::WriteFile(
                    pipe_,                 // pipe handle 
                    data.c_str() + cnt,       // message 
                    (DWORD)(data.size() - cnt),	   // message length 
                    &cbWritten,            // bytes written 
                    NULL);                 // not overlapped 
                if (!b)
                {
                    //cry();
                    return false;
                }
            }
            cnt += cbWritten;
            return true;
        }


    private:
        HANDLE pipe_;
    };

    namespace detail {

        class PipeReadOverlap : public OVERLAPPED
        {
        public:
            static const int BUFSIZE = 4096;

            PipeReadOverlap()
                : buf(4096)
            {
                ::ZeroMemory(this, sizeof(OVERLAPPED));
            }

            HANDLE pipe = INVALID_HANDLE_VALUE;

            std::function<void(std::string)> onRead;
            bool readAll = false;
            MTL::cbuff buf;
            std::ostringstream data;

            static void overlappedCompletionRoutine(
                DWORD dwErrorCode,
                DWORD dwNumberOfBytesTransfered,
                LPOVERLAPPED lpOverlapped
            )
            {
                PipeReadOverlap* overlap = (PipeReadOverlap*)lpOverlapped;

                if (dwErrorCode == ERROR_SUCCESS)
                {
                    overlap->data.write(overlap->buf, dwNumberOfBytesTransfered);

                    auto cb = overlap->onRead;
                    if (overlap->readAll)
                    {

                        char* buf = overlap->buf;
                        BOOL b = ::ReadFileEx(
                            overlap->pipe,        // handle to pipe 
                            buf,    // buffer to receive data 
                            overlap->BUFSIZE, // size of buffer 
                            overlap,
                            PipeReadOverlap::overlappedCompletionRoutine
                        );

                        if (!b)
                        {
                            DWORD e = ::GetLastError();
                            std::cout << "read fail " << e << std::endl;

                            cb(overlap->data.str());
                            delete overlap;
                        }
                        return;
                    }
                    else
                    {
                        if (cb)
                        {
                            cb(overlap->data.str());
                        }
                    }
                }

                delete overlap;
            };

        };

        class PipeWriteOverlap : public OVERLAPPED
        {
        public:

            static const int BUFSIZE = 4096;

            PipeWriteOverlap()
            {
                ::ZeroMemory(this, sizeof(OVERLAPPED));
            }

            HANDLE pipe = INVALID_HANDLE_VALUE;
            std::string data;
            size_t written = 0;

            std::function<void(bool)> onWritten;

            static void overlappedCompletionRoutine(
                DWORD dwErrorCode,
                DWORD dwNumberOfBytesTransfered,
                LPOVERLAPPED lpOverlapped
            )
            {
                PipeWriteOverlap* overlap = (PipeWriteOverlap*)lpOverlapped;

                if (dwErrorCode == ERROR_SUCCESS)
                {
                    overlap->written += dwNumberOfBytesTransfered;

                    if (overlap->written < overlap->data.size())
                    {
                        DWORD toWrite = (DWORD)overlap->data.size() - (DWORD)overlap->written;
                        if (toWrite > BUFSIZE)
                        {
                            toWrite = BUFSIZE;
                        }

                        BOOL b = ::WriteFileEx(
                            overlap->pipe,                  // pipe handle 
                            overlap->data.data() + overlap->written,       // message 
                            toWrite,			    // message length 
                            overlap,
                            &PipeWriteOverlap::overlappedCompletionRoutine
                        );

                        if (!b)
                        {
                            if (overlap->onWritten)
                            {
                                overlap->onWritten(false);
                            }
                        }
                    }
                    else
                    {
                        if (overlap->onWritten)
                        {
                            overlap->onWritten(true);
                        }
                    }
                }
                delete overlap;
            };
        };
    }

    class AsyncNamedPipe
    {
    public:

        AsyncNamedPipe()
            :pipe_(INVALID_HANDLE_VALUE)
        {}

        AsyncNamedPipe(HANDLE h)
            :pipe_(h)
        {
        }

        ~AsyncNamedPipe()
        {
            close();
        }

        AsyncNamedPipe(const AsyncNamedPipe& rhs) = delete;
        AsyncNamedPipe& operator=(const AsyncNamedPipe& rhs) = delete;

        AsyncNamedPipe(AsyncNamedPipe&& rhs) noexcept
        {
            pipe_ = rhs.pipe_;
            rhs.pipe_ = INVALID_HANDLE_VALUE;
        }

        AsyncNamedPipe& operator=(AsyncNamedPipe&& rhs) noexcept
        {
            if (this == &rhs)
            {
                return *this;
            }

            close();
            pipe_ = rhs.pipe_;
            rhs.pipe_ = INVALID_HANDLE_VALUE;

            return *this;
        }

        operator bool()
        {
            return pipe_ != INVALID_HANDLE_VALUE;
        }

        HANDLE detach()
        {
            HANDLE result = pipe_;
            pipe_ = INVALID_HANDLE_VALUE;
            return result;
        }

        HANDLE operator*() const
        {
            return pipe_;
        }

        static std::shared_ptr<AsyncNamedPipe> create(const std::wstring& pipename)
        {
            HANDLE handle = CreateNamedPipe(
                pipename.c_str(),             // pipe name 
                PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,	// read/write access 
                PIPE_TYPE_BYTE |				// message type pipe 
                PIPE_READMODE_BYTE |			// message-read mode 
                PIPE_WAIT,					// blocking mode 
                PIPE_UNLIMITED_INSTANCES,		// max. instances  
                detail::PipeWriteOverlap::BUFSIZE,						// output buffer size 
                detail::PipeReadOverlap::BUFSIZE,						// input buffer size 
                0,							// client time-out 
                NULL);						// default security attribute 


            return std::make_shared<AsyncNamedPipe>(handle);
        }

        static std::shared_ptr<AsyncNamedPipe> open(const std::wstring& pipename)
        {

            if (!::WaitNamedPipe(pipename.c_str(), 20000))
            {
                return std::make_shared<AsyncNamedPipe>(INVALID_HANDLE_VALUE);
            }

            // Try to open named pipe; wait for it, if necessary. 

            while (1)
            {
                HANDLE handle = ::CreateFile(
                    pipename.c_str(),   // pipe name 
                    GENERIC_READ |  // read and write access 
                    GENERIC_WRITE,
                    0,              // no sharing 
                    NULL,           // default security attributes
                    OPEN_EXISTING,  // opens existing pipe 
                    FILE_FLAG_OVERLAPPED,  // default attributes 
                    NULL);          // no template file 

                // all OK ? 
                if (handle != INVALID_HANDLE_VALUE)
                    return std::make_shared<AsyncNamedPipe>(handle);

                // Exit if an error other than ERROR_PIPE_BUSY occurs. 

                if (::GetLastError() != ERROR_PIPE_BUSY)
                {
                    return std::make_shared<AsyncNamedPipe>(INVALID_HANDLE_VALUE);
                }

                // All pipe instances are busy, so wait for 20 seconds. 	 
                if (!::WaitNamedPipe(pipename.c_str(), 20000))
                {
                    return std::make_shared<AsyncNamedPipe>(INVALID_HANDLE_VALUE);
                }
            }

            // never get here
            return std::make_shared<AsyncNamedPipe>(INVALID_HANDLE_VALUE);
        }

        bool connect(int milisecs = 3000)
        {
            return  ::ConnectNamedPipe(pipe_, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        }

        void disconnect()
        {
            ::DisconnectNamedPipe(pipe_);
        }

        void close()
        {
            if (pipe_ != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(pipe_);
            }
            pipe_ = INVALID_HANDLE_VALUE;
        }


        void read(std::function<void(std::string)> cb)
        {
            detail::PipeReadOverlap* overlap = new detail::PipeReadOverlap();
            overlap->pipe = pipe_;
            overlap->onRead = cb;

            char* buf = overlap->buf;
            BOOL b = ::ReadFileEx(
                pipe_,        // handle to pipe 
                buf,    // buffer to receive data 
                overlap->BUFSIZE, // size of buffer 
                overlap,
                &detail::PipeReadOverlap::overlappedCompletionRoutine
            );

            if (!b)
            {
                cb("");
                delete overlap;
            }
        }

        void readAll(std::function<void(std::string)> cb)
        {
            detail::PipeReadOverlap* overlap = new detail::PipeReadOverlap();
            overlap->pipe = pipe_;
            overlap->onRead = cb;
            overlap->readAll = true;

            char* buf = overlap->buf;
            BOOL b = ::ReadFileEx(
                pipe_,        // handle to pipe 
                buf,    // buffer to receive data 
                overlap->BUFSIZE, // size of buffer 
                overlap,
                &detail::PipeReadOverlap::overlappedCompletionRoutine
            );

            if (!b)
            {
                cb("");
                delete overlap;
            }
        }

        bool write(const std::string& data, std::function<void(bool)> cb)
        {
            detail::PipeWriteOverlap* overlap = new detail::PipeWriteOverlap;
            overlap->pipe = pipe_;
            overlap->onWritten = cb;
            overlap->data = data;

            DWORD toWrite = (DWORD)data.size();
            if (toWrite > overlap->BUFSIZE)
            {
                toWrite = overlap->BUFSIZE;
            }

            BOOL b = ::WriteFileEx(
                pipe_,                  // pipe handle 
                data.data(),            // message 
                toWrite,			    // message length 
                overlap,
                &detail::PipeWriteOverlap::overlappedCompletionRoutine
            );
            if (!b)
            {
                cb(false);
                delete overlap;
            }
            return true;
        }

    private:
        HANDLE pipe_;
    };



    class Elevator
    {
    public:

        Elevator(
            const std::wstring& executor, 
            const std::wstring& pipename
        )
            : executor_(executor), 
              pipe_name_(pipename)
        {
        }

        std::string read(const std::wstring& file)
        {
            NamedPipe pipe = NamedPipe::create(pipe_name_);

            std::wostringstream oss_args;
            oss_args
                << L"read "
                << pipe_name_
                << L" \""
                << file
                << L"\"";

            BOOL r = MTL::Shell::ExecuteArgs(
                executor_, 
                oss_args.str(), 
                L"runas", 
                SW_HIDE
            );

            if (!r)
            {
                pipe.disconnect();
                pipe.close();
                return "";
            }

            if (!pipe.connect())
            {
                pipe.disconnect();
                pipe.close();
                return "";
            }

            std::string data = pipe.read();

            pipe.disconnect();
            pipe.close();

            return data;
        }

        void read(const std::wstring& file, std::function<void(std::string)> cb)
        {
            std::shared_ptr<AsyncNamedPipe> pipe = AsyncNamedPipe::create(pipe_name_);

            std::wostringstream oss_args;
            oss_args
                << L"read "
                << pipe_name_
                << L" \""
                << file
                << L"\"";

            BOOL r = MTL::Shell::ExecuteArgs(
                executor_, 
                oss_args.str(), 
                L"runas", 
                SW_HIDE
            );

            if (!r)
            {
                pipe->disconnect();
                pipe->close();
                cb("");
                return;
            }

            if (!pipe->connect())
            {
                pipe->disconnect();
                pipe->close();
                cb("");
                return;
            }

            pipe->readAll([pipe, cb](std::string data)
            {
                cb(data);
                pipe->disconnect();
                pipe->close();
            });
        }


        bool write(const std::wstring& file, const std::string& data)
        {
            NamedPipe pipe = NamedPipe::create(pipe_name_);

            std::wostringstream oss_args;
            oss_args
                << L"write "
                << pipe_name_
                << L" \""
                << file
                << "\"";

            BOOL r = MTL::Shell::ExecuteArgs(
                executor_, 
                oss_args.str(), 
                L"runas", 
                SW_HIDE
            );

            if (!r)
            {
                pipe.disconnect();
                pipe.close();
                return false;
            }

            if (!pipe.connect())
            {
                pipe.disconnect();
                pipe.close();
                return false;
            }

            if (!pipe.write(data))
            {
                pipe.disconnect();
                pipe.close();
                return false;
            }

            std::string s = pipe.read();

            pipe.disconnect();
            pipe.close();
            return true;
        }

        void write(const std::wstring& file, const std::string& data, std::function<void(bool)> cb)
        {
            auto pipe = AsyncNamedPipe::create(pipe_name_);

            std::wostringstream oss_args;
            oss_args
                << L"write "
                << pipe_name_
                << L" \""
                << file
                << "\"";

            BOOL r = MTL::Shell::ExecuteArgs(
                executor_, 
                oss_args.str(), 
                L"runas", 
                SW_HIDE
            );

            if (!r)
            {
                pipe->disconnect();
                pipe->close();
                cb(false);
            }

            if (!pipe->connect())
            {
                pipe->disconnect();
                pipe->close();
                cb(false);
            }

            pipe->write(data, [cb, pipe](bool success)
            {
                cb(success);
                pipe->disconnect();
                pipe->close();
            });
        }


    private:
        std::wstring executor_;
        std::wstring pipe_name_;
    };

}

