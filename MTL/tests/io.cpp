#include "gtest/gtest.h"
#include "MTL/win32/uni.h"
#include "MTL/disp/bstr.h"
#include "MTL/disp/disp.h"
#include "MTL/disp/variant.h"
#include "MTL/disp/sf_array.h"
#include "MTL/punk.h"
#include "MTL/win32/box.h"
#include "MTL/win32/pipe.h"
#include "MTL/win32/io.h"
#include "MTL/util/path.h"
#include "MTL/obj/collection.h"
#include "MTL/obj/marshall.h"
#include "MTL/ole/control.h"
#include "MTL/ole/shell.h"
#include "MTL/win/enc.h"
#include <string>
#include <sstream>
#include <fstream>
#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ShlObj.h>

using namespace mtl;

class IOTest : public ::testing::Test {
protected:

    IOTest() {
        // You can do set-up work for each test here.

        ::CoInitialize(0);

        empty_msg_queue();
    }

    ~IOTest() override {
        // You can do clean-up work that doesn't throw exceptions here.
        ::CoUninitialize();

    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    void SetUp() override {
        // Code here will be called immediately after the constructor (right
        // before each test).

    }

    void TearDown() override {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    // Class members declared here can be used by all tests in the test suite
    // for Foo.

    void empty_msg_queue()
    {
        MSG msg = { 0,0,0,0 };
        while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0);
    }

    void run_msg_loop()
    {
        MSG msg = { 0,0,0,0 };

        while (true)
        {
            DWORD r = ::MsgWaitForMultipleObjectsEx(0, 0, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE | MWMO_ALERTABLE);
            if (r == WAIT_IO_COMPLETION)
            {
                continue;
            }

            if (!::GetMessage(&msg, 0, 0, 0))
            {
                break;
            }
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
};


TEST_F(IOTest, testSimpleFile) 
{
    auto file_name = L"C:\\Windows\\System32\\drivers\\etc\\hosts";

    std::fstream fs;
    std::ostringstream oss;
    fs.open( mtl::to_string(file_name), std::ios::binary | std::ios::in);
    if (fs)
    {
        while (fs)
        {
            mtl::cbuff buf(1024);
            fs.read(buf, buf.size());
            oss.write(buf, fs.gcount());
        }
        fs.close();
    }

    std::string bytes = oss.str();

    std::string header = "# Copyright (c) 1993-2009 Microsoft Corp.";

    std::cout << bytes << std::endl;

    EXPECT_EQ(header, bytes.substr(0,header.size()));
}


TEST_F(IOTest, testSimpleFileWin32)
{
    auto file_name = L"C:\\Windows\\System32\\drivers\\etc\\hosts";

    HANDLE handle = nullptr;

    handle = ::CreateFile(
        file_name,
        GENERIC_READ ,
        FILE_SHARE_WRITE| FILE_SHARE_READ | FILE_SHARE_DELETE,
        nullptr,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, //FILE_FLAG_OVERLAPPED
        nullptr
    );

    EXPECT_NE(INVALID_HANDLE_VALUE, handle);

    BY_HANDLE_FILE_INFORMATION fi;
    ::ZeroMemory(&fi, sizeof(fi));
    
    BOOL r = ::GetFileInformationByHandle(handle, &fi);

    EXPECT_NE(0, r);

    ULARGE_INTEGER uli;
    uli.HighPart = fi.nFileSizeHigh;
    uli.LowPart = fi.nFileIndexLow;

    unsigned long long fsize = uli.QuadPart;

    mtl::cbuff buf(fsize);

    DWORD nRead = 0;
    r = ::ReadFile(
        handle,
        buf,
        (DWORD)buf.size(),
        &nRead,
        nullptr
    );

    EXPECT_NE(0, r);

    std::string bytes(buf, nRead);

    std::string header = "# Copyright (c) 1993-2009 Microsoft Corp.";

    std::cout << bytes << std::endl;

    ::CloseHandle(handle);

    EXPECT_EQ(header, bytes.substr(0, header.size()));
}

TEST_F(IOTest, testSimpleFileWin32Async)
{
    auto file_name = L"C:\\Windows\\System32\\drivers\\etc\\hosts";

    HANDLE handle = nullptr;

    handle = ::CreateFile(
        file_name,
        GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
        nullptr,
        OPEN_ALWAYS,
        FILE_FLAG_OVERLAPPED,
        nullptr
    );

    EXPECT_NE(INVALID_HANDLE_VALUE, handle);

    auto overlappedCompletionRoutine = [](
        DWORD dwErrorCode,
        DWORD dwNumberOfBytesTransfered,
        LPOVERLAPPED lpOverlapped
    ) 
    {
        mtl::detail::ReadOverLap* overlap = (mtl::detail::ReadOverLap*)lpOverlapped;

        if (dwErrorCode == ERROR_SUCCESS)
        {
            std::string bytes = overlap->str(dwNumberOfBytesTransfered);

            std::string header = "# Copyright (c) 1993-2009 Microsoft Corp.";

            std::cout << bytes << std::endl;
            EXPECT_EQ(header, bytes.substr(0, header.size()));
            ::CloseHandle(overlap->hEvent);
            delete overlap;
            ::PostQuitMessage(0);
        }
        else
        {
            ::CloseHandle(overlap->hEvent);
            delete overlap;
        }
    };


    BY_HANDLE_FILE_INFORMATION fi;
    ::ZeroMemory(&fi, sizeof(fi));

    BOOL r = ::GetFileInformationByHandle(handle, &fi);

    EXPECT_NE(0, r);

    ULARGE_INTEGER uli;
    uli.HighPart = fi.nFileSizeHigh;
    uli.LowPart = fi.nFileIndexLow;

    mtl::detail::ReadOverLap* overlap = new mtl::detail::ReadOverLap(handle, uli.QuadPart);

    char* buf = *overlap->buf;
    DWORD nRead = 0;
    r = ::ReadFileEx(
        handle,
        buf,
        (DWORD)overlap->buf.get()->size(),
        overlap,
        overlappedCompletionRoutine
    );

    EXPECT_NE(0, r);

    run_msg_loop();
}


TEST_F(IOTest, testSlurpFileWin32)
{
    auto file_name = L"C:\\Windows\\System32\\drivers\\etc\\hosts";

    std::string content = slurp(file_name);

    std::string header = "# Copyright (c) 1993-2009 Microsoft Corp.";

    std::cout << content << std::endl;

    EXPECT_EQ(header, content.substr(0, header.size()));
}


TEST_F(IOTest, testSpitFileWin32)
{
    auto file_name = L"C:\\moe\\test2.txt";

    std::string content = "test\r\ndata\r\nfile\r\n\r\n";

    bool b = spit(file_name, content);
    EXPECT_EQ(true, b);

    std::string file_content = slurp(file_name);
    EXPECT_EQ(content, file_content);
}



TEST_F(IOTest, testSlurpFileWin32Async)
{
    auto file_name = L"C:\\Windows\\System32\\drivers\\etc\\hosts";

    bool b = slurp(file_name, [](DWORD e, std::string bytes) 
    {
        std::string header = "# Copyright (c) 1993-2009 Microsoft Corp.";

        std::cout << bytes << std::endl;

        EXPECT_EQ(header, bytes.substr(0, header.size()));

        ::PostQuitMessage(0);
    });

    EXPECT_NE(false, b);

    run_msg_loop();
}


TEST_F(IOTest, testSlurpFileWin32Async2)
{
    auto file_name = L"C:\\Windows\\System32\\drivers\\etc\\hosts";

    bool b = slurp(file_name, [](DWORD e, std::string bytes)
    {
        std::string header = "# Copyright (c) 1993-2009 Microsoft Corp.";

        std::cout << bytes << std::endl;

        EXPECT_EQ(header, bytes.substr(0, header.size()));

        ::PostQuitMessage(0);
    });

    EXPECT_NE(false, b);

    run_msg_loop();
}


TEST_F(IOTest, testSpitFileWin32Async)
{
    auto file_name = L"C:\\moe\\test2.txt";

    std::string old_content = "test\r\ndata\r\nfile\r\n\r\n";
    std::string new_content = "test\r\ndata\r\nfile\r\n\r\nNEW\r\n";

    spit(file_name, new_content, [file_name, old_content, new_content](DWORD e)
    {
        EXPECT_FALSE(e);

        std::string data = slurp(file_name);
        EXPECT_EQ(new_content, data);

        spit(file_name, old_content, [file_name, old_content](DWORD e)
        {
            EXPECT_FALSE(e);

            std::string data = slurp(file_name);
            EXPECT_EQ(old_content, data);
            ::PostQuitMessage(0);
        });
    });

    run_msg_loop();
}

void pipeServer(std::wstring pipe_name)
{
    named_pipe pipe = named_pipe::open(pipe_name);
    EXPECT_TRUE( (bool)pipe );

    std::string data = pipe.read();
    std::cout << "recvd: " << data << std::endl;

    pipe.write(data);
}

TEST_F(IOTest, testNamedPipe)
{
    MSG msg = { 0,0,0,0 };
    while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0);

    auto pipe_name = L"\\\\.\\pipe\\mtl-test";

    named_pipe pipe = named_pipe::create(pipe_name);
    EXPECT_TRUE((bool)pipe);

    if (!pipe)
    {
        DWORD e = ::GetLastError();
        std::cout << "e: " << e << std::endl;
    }

    std::thread t1(pipeServer,pipe_name);

    bool b = pipe.connect();
    EXPECT_TRUE(b);

    std::string data = "some pipe data send over";
    pipe.write(data);

    std::string rcvd = pipe.read();

    EXPECT_EQ(data, rcvd);

    t1.join();
}

void pipeAsyncServer(std::wstring pipe_name)
{
    named_pipe pipe = named_pipe::open(pipe_name);
    EXPECT_TRUE((bool)pipe);

    std::string data = pipe.read();
    std::cout << "recvd: " << data << std::endl;

    pipe.write(data);

    data = pipe.read();
    std::cout << "recvd: " << data << std::endl;

    pipe.write(data);

}

TEST_F(IOTest, testNamedPipeAsync)
{
    auto pipe_name = L"\\\\.\\pipe\\mtl-test";

    auto pipe = async_named_pipe::create(pipe_name);
    EXPECT_TRUE((bool)*pipe);

    if (!*pipe)
    {
        DWORD e = ::GetLastError();
        std::cout << "e: " << e << std::endl;
    }

    std::thread t1(pipeAsyncServer, pipe_name);

    bool b = pipe->connect();
    EXPECT_TRUE(b);

    std::string data = "some pipe data send over";
    pipe->write(data, [](bool success) 
    {
        EXPECT_TRUE(success);
        std::cout << "WRITTEN" << std::endl;
    });

    pipe->read( [&pipe,&data](std::string rcvd) 
    {
        std::cout << "RECEIVD: " << rcvd << std::endl;
        EXPECT_EQ(data, rcvd);

        pipe->write(data, [](bool success)
        {
            EXPECT_TRUE(success);
            std::cout << "WRITTEN2" << std::endl;
        });

        pipe->read([&pipe, &data](std::string rcvd)
        {
            std::cout << "RECEIVD2: " << rcvd << std::endl;
            EXPECT_EQ(data, rcvd);

            pipe->read([&pipe, &data](std::string rcvd)
            {
                std::cout << "RECEIVD2: " << rcvd << std::endl;
                EXPECT_EQ("", rcvd);
                ::PostQuitMessage(0);
            });
        });
    });

    run_msg_loop();

    t1.join();
}




TEST_F(IOTest, testNamedPipeAsyncReadAll)
{
    auto pipe_name = L"\\\\.\\pipe\\mtl-test";

    auto pipe = async_named_pipe::create(pipe_name);
    EXPECT_TRUE((bool)*pipe);

    if (!*pipe)
    {
        DWORD e = ::GetLastError();
        std::cout << "e: " << e << std::endl;
    }

    std::thread t1(pipeAsyncServer, pipe_name);

    bool b = pipe->connect();
    EXPECT_TRUE(b);

    std::string data = "some pipe data send over";
    pipe->write(data, [](bool success)
    {
        EXPECT_TRUE(success);
        std::cout << "WRITTEN" << std::endl;
    });

    pipe->read_all([&pipe, &data](std::string rcvd)
    {
        std::cout << "RECEIVD: " << rcvd << std::endl;
        EXPECT_EQ("some pipe data send oversome pipe data send over", rcvd);
        ::PostQuitMessage(0);
    });

    pipe->write(data, [](bool success)
    {
        EXPECT_TRUE(success);
        std::cout << "WRITTEN2" << std::endl;
    });

    run_msg_loop();

    t1.join();
}


TEST_F(IOTest, testPipedProcess)
{
    std::wstring result = mtl::Process::exec(L"git.exe status .");
    std::wcout << result << std::endl;
}



TEST_F(IOTest, testPipedProcessAsync)
{
    //mtl::Process::exec(L"git.exe -C \"C:\\moe\" status .", [](std::wstring result) 

    std::wstring p = mtl::path(mtl::path_to_self_directory()).parent_dir().parent_dir();

    std::wostringstream woss;
    woss << L"git.exe -C \"" << p << "\" status .";

    {
        mtl::Process::exec(woss.str(), [](std::wstring result)
        {
            std::wcout << result << std::endl;
            ::PostQuitMessage(0);
        });
    }
    run_msg_loop();
}

/*

TEST_F(IOTest, testElevatorRead)
{
    auto pipe_name = L"\\\\.\\pipe\\mtl-prw";
    std::wstring executor = MTL::pathToSelfDirectory() + L"\\rwp.exe";
    std::wstring file_name = L"C:\\moe\\admin1.txt";

    File f;
    bool b = f.open(file_name);
    EXPECT_EQ(false, b);

    Elevator eleve(executor, pipe_name);
    std::string data = eleve.read(file_name);
    EXPECT_EQ("hidden text\r\n", data);
}


TEST_F(IOTest, testElevatorWrite)
{
    auto pipe_name = L"\\\\.\\pipe\\mtl-prw";
    std::wstring executor = MTL::pathToSelfDirectory() + L"\\rwp.exe";
    std::wstring file_name = L"C:\\moe\\admin1.txt";

    File f;
    bool b = f.open(file_name);
    EXPECT_EQ(false, b);

    std::string payload = "something else";

    Elevator eleve(executor, pipe_name);
    eleve.write(file_name, payload);
    std::string data = eleve.read(file_name);
    EXPECT_EQ(payload, data);

    eleve.write(file_name, "hidden text\r\n");

}


TEST_F(IOTest, testElevatorAsyncRead)
{
    auto pipe_name = L"\\\\.\\pipe\\mtl-prw";
    std::wstring executor = MTL::pathToSelfDirectory() + L"\\rwp.exe";
    std::wstring file_name = L"C:\\moe\\admin1.txt";

    File f;
    bool b = f.open(file_name);
    EXPECT_EQ(false, b);

    Elevator eleve(executor, pipe_name);
    eleve.read(file_name, [](std::string data) 
    {
        EXPECT_EQ("hidden text\r\n", data);
        ::PostQuitMessage(0);
    });

    run_msg_loop();
}

TEST_F(IOTest, testElevatorAsyncWrite)
{
    auto pipe_name = L"\\\\.\\pipe\\mtl-prw";
    std::wstring executor = MTL::pathToSelfDirectory() + L"\\rwp.exe";
    std::wstring file_name = L"C:\\moe\\admin1.txt";

    File f;
    bool b = f.open(file_name);
    EXPECT_EQ(false, b);

    std::string payload = "something else";

    Elevator eleve(executor, pipe_name);
    eleve.write(file_name,payload, [executor, pipe_name, file_name, payload](bool success)
    {
        EXPECT_TRUE(success);
        {
            Elevator eleve(executor, pipe_name);
            std::string data = eleve.read(file_name);
            EXPECT_EQ(payload, data);
        }
        {
            Elevator eleve(executor, pipe_name);
            eleve.write(file_name,"hidden text\r\n");
        }
        ::PostQuitMessage(0);
    });

    run_msg_loop();
}


*/
