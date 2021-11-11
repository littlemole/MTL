#pragma once

#include <string>
#include <set>
#include <map>
#include <functional>


namespace mtl {

    inline std::wstring new_guid();

    template<class T>
    class event;


    template<class T, class ... Args>
    class event<T(Args...)>
    {
    public:
        using handler_t = std::function<void(Args...)>;
        using handlers_t = std::map<std::wstring, std::pair<T, handler_t>>;

        void fire(T id, Args ... args)
        {
            auto handlers = handlers_;
            for (auto& h : handlers)
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

        static size_t unregister(const std::wstring& id)
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
            return map().size();
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
        using handlers_t = std::map<std::wstring, handler_t>;

        void fire(Args ... args)
        {
            auto handlers = handlers_;
            for (auto& h : handlers)
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

        static size_t unregister(const std::wstring& id)
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
            return map().size();
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
            clear();
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

        void clear()
        {
            for (auto& id : ids_)
            {
                event<T(Args...)>::unregister(id);
            }
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
            clear();
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

        void clear()
        {
            for (auto& id : ids_)
            {
                event<void(Args...)>::unregister(id);
            }
        }

    private:

        event<void(Args...)>* source_;
        std::set<std::wstring> ids_;
    };

}
