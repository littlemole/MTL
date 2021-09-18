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


    template<class T>
    class event;


    template<class T, class ... Args>
    class event<T(Args...)>
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

        ~event()
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
                event<T(Args...)>* e = map()[id];
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
            static std::map<std::wstring, event<T(Args...)>*> theMap;
            return theMap;
        }
        handlers_t handlers_;
    };

    template<class ... Args>
    class event<void(Args...)>
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

        ~event()
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
                event<void(Args...)>* e = map()[id];
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
            static std::map<std::wstring, event<void(Args...)>*> theMap;
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
                event<T(Args...)>::unregister(id);
            }
        }

        sink<T(Args...)>& operator()(event<T(Args...)>& source)
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
        event<T(Args...)>* source_;
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
                event<void(Args...)>::unregister(id);
            }
        }

        sink<void(Args...)>& operator()(event<void(Args...)>& source)
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
        event<void(Args...)>* source_;
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
