#pragma once

#define NOMINMAX
#include <SDKDDKVer.h>
#include <WinSock2.h>
#include <windows.h>
#include <wchar.h>
#include <objbase.h>

#include <functional>
#include <algorithm>
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "mtl/util/event.h"

namespace mtl {


    inline std::wstring guid_to_string(const GUID& guid)
    {
        wchar_t buf[256];
        size_t len = ::StringFromGUID2(guid, buf, 256);
        if (len == 0)
        {
            return L"";
        }
        return std::wstring(buf,len - 1);
    }

    inline GUID string_to_guid(const std::wstring& str)
    {
        GUID guid;
        ::CLSIDFromString(str.c_str(), &guid);
        return guid;
    }

    inline std::wstring new_guid()
    {
        GUID guid;
        ::CoCreateGuid(&guid);
        return guid_to_string(guid);
    }

    inline BOOL is_64bit_windows()
    {
#if defined(_WIN64)
        return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
        // 32-bit programs run on both 32-bit and 64-bit Windows
        // so must sniff
        BOOL f64 = FALSE;
        return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
        return FALSE; // Win64 does not support Win16
#endif
    }

	class library
	{
	public:
		virtual ~library() {}
	};

	template<class T>
	class library_impl : public library
	{
	public:
        library_impl(T&& t)
			: lib(std::move(t))
		{}

		~library_impl()
		{
			int x = 1;
		}
	private:
		T lib;
	};


	class uses
	{
	public:

		template<class ... Args>
		uses(Args&& ... args)
		{
			init(std::forward<Args>(args)...);
		}

	private:

		template<class T, class ... Args>
		void init(T&& t, Args&& ... args)
		{
			std::unique_ptr<library> impl(new library_impl(std::forward<T>(t)));
			libs_.push_back(std::move(impl));
			init(std::forward<Args>(args)...);
		}

		template<class T>
		void init(T&& t)
		{
			std::unique_ptr<library> impl(new library_impl(std::forward<T>(t)));
			libs_.push_back(std::move(impl));
			//libs_.emplace_back( impl );
		}

		std::vector<std::unique_ptr<library>> libs_;
	};



    inline std::wstring error_msg(DWORD e)
    {
        LPWSTR messageBuffer = nullptr;

        size_t size = ::FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, 
            e, 
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
            (LPWSTR)&messageBuffer, 
            0, 
            NULL
        );

        std::wstring message(messageBuffer, size);

        ::LocalFree(messageBuffer);
        return message;
    }
}
