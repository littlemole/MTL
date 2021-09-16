#pragma once

#define NOMINMAX
#include <SDKDDKVer.h>
//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <WinSock2.h>
//#define NOMINMAX  1
#include <windows.h>
#include <wchar.h>
#include <objbase.h>

// C RunTime Header Files

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <map>
#include <atomic>
#include <algorithm>
#include <set>

namespace MTL {


    inline std::wstring guid_to_string(const GUID& guid)
    {
        wchar_t buf[256];
        size_t len = ::StringFromGUID2(guid, buf, 256);
        if (len == 0)
        {
            return L"";
        }
        return std::wstring(buf,len - 1);

        /*
        wchar_t guidStr[39];
        swprintf_s(
            guidStr,
            L"{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

        std::wstring s(guidStr);
        return s;
        */
    }

    inline GUID string_to_guid(const std::wstring& str)
    {
        GUID guid;
        ::CLSIDFromString(str.c_str(), &guid);
        return guid;

        /*
    GUID guid;
    swscanf_s(str.c_str(),
        L"{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
        &guid.Data1, &guid.Data2, &guid.Data3,
        &guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3],
        &guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7] );

    return guid;
    */
    }

    inline std::wstring new_guid()
    {
        GUID guid;
        ::CoCreateGuid(&guid);
        return guid_to_string(guid);
    }

    inline BOOL Is64BitWindows()
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

	class Library
	{
	public:
		virtual ~Library() {}
	};

	template<class T>
	class LibraryImpl : public Library
	{
	public:
		LibraryImpl(T&& t)
			: lib(std::move(t))
		{}

		~LibraryImpl()
		{
			int x = 1;
		}
	private:
		T lib;
	};


	class Uses
	{
	public:

		template<class ... Args>
		Uses(Args&& ... args)
		{
			init(std::forward<Args>(args)...);
		}

	private:

		template<class T, class ... Args>
		void init(T&& t, Args&& ... args)
		{
			std::unique_ptr<Library> impl(new LibraryImpl(std::forward<T>(t)));
			libs_.push_back(std::move(impl));
			init(std::forward<Args>(args)...);
		}

		template<class T>
		void init(T&& t)
		{
			std::unique_ptr<Library> impl(new LibraryImpl(std::forward<T>(t)));
			libs_.push_back(std::move(impl));
			//libs_.emplace_back( impl );
		}

		std::vector<std::unique_ptr<Library>> libs_;
	};


    template<class T>
    class Event;


    template<class T, class ... Args>
    class Event<T(Args...)>
    {
    public:
        using handler_t = std::function<void(Args...)>;
        using handlers_t = std::map<std::wstring,std::pair<T, handler_t>>;

        void fire(T id, Args ... args)
        {
            for (auto& h : handlers_)
            {
                if (id == h.second.first)
                {
                    h.second.second(args...);
                }
            }
        }

        ~Event()
        {
            for (auto& h : handlers_)
            {
                if (map().count(h.first))
                {
                    map().erase(h.first);
                }
            }
        }

        std::wstring operator()(T code, const handler_t& h)
        {
            std::wstring id = new_guid();
            handlers_[id] = std::make_pair(code, h);
            map()[id] = this;
            return id;
        }

        static void unregister(const std::wstring& id)
        {
            if (map().count(id))
            {
                Event<T(Args...)>* e = map()[id];
                if (e->handlers_.count(id))
                {
                    e->handlers_.erase(id);
                }
                map().erase(id);
            }
        }

    private:

        inline static auto& map()
        {
            static std::map<std::wstring, Event<T(Args...)>*> theMap;
            return theMap;
        }
        handlers_t handlers_;
    };

    template<class ... Args>
    class Event<void(Args...)>
    {
    public:
        using handler_t = std::function<void(Args...)>;
        using handlers_t = std::map<std::wstring,handler_t>;

        void fire(Args ... args)
        {
            for (auto& h : handlers_)
            {
                h.second(args...);
            }
        }

        ~Event()
        {
            for (auto& h : handlers_)
            {
                if (map().count(h.first))
                {
                    map().erase(h.first);
                }
            }
        }

        std::wstring operator()(const handler_t& h)
        {
            std::wstring id = new_guid();
            handlers_[id] = h;
            map()[id] = this;
            return id;
        }

        static void unregister(const std::wstring& id)
        {
            if (map().count(id))
            {
                Event<void(Args...)>* e = map()[id];
                if (e->handlers_.count(id))
                {
                    e->handlers_.erase(id);
                }
                map().erase(id);
            }
        }

    private:
        handlers_t handlers_;

        inline static auto& map()
        {
            static std::map<std::wstring, Event<void(Args...)>*> theMap;
            return theMap;
        }
    };

    template<class T>
    class sink;

    template<class T, class ... Args>
    class sink<T(Args...)>
    {
    public:

        ~sink()
        {
            for (auto& id : ids_)
            {
                Event<T(Args...)>::unregister(id);
            }
        }

        sink<T(Args...)>& operator()(Event<T(Args...)>& source)
        {
            source_ = &source;
            return *this;
        }

        sink<T(Args...)>& when(T code)
        {
            code_ = code;
            return *this;
        }

        sink<T(Args...)>& then(std::function<void(Args...)> cb)
        {
            std::wstring id = source_->operator()(code_, cb);
            ids_.insert(id);
            return *this;
        }

    private:
        Event<T(Args...)>* source_;
        T code_;
        std::set<std::wstring> ids_;
    };


    template<class ... Args>
    class sink<void(Args...)>
    {
    public:

        ~sink()
        {
            for (auto& id : ids_)
            {
                Event<void(Args...)>::unregister(id);
            }
        }

        sink<void(Args...)>& operator()(Event<void(Args...)>& source)
        {
            source_ = &source;
            return *this;
        }

        sink<void(Args...)>& then(std::function<void(Args...)> cb)
        {
            std::wstring id = source_->operator()(cb);
            ids_.insert(id);
            return *this;
        }

    private:
        Event<void(Args...)>* source_;
        std::set<std::wstring> ids_;
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
