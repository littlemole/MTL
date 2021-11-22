#pragma once

#include <mtl/win/wnd.h>
#include <sstream>

namespace mtl {

    class dialog;


    template<int ID, class T>
    class dlg_value
    {
    public:
        int id = ID;
        T val_;
        dialog* dlg = nullptr;

        dlg_value()
        {}

        dlg_value(const T& t)
            : val_(t)
        {}

        void value(const T& t);
        T& value();

        T& operator*()
        {
            return value();
        }

        operator T()
        {
            return value();
        }

        dlg_value& operator=(const T& rhs)
        {
            value(rhs);
            return *this;
        }
    };

    template<int ID, class T>
    class dlg_selection
    {
    public:
        int id = ID;
        T val_;
        dialog* dlg = nullptr;

        dlg_selection()
        {}

        dlg_selection(const T& t)
            : val_(t)
        {}

        void value(const T& t);
        T& value();

        T& operator*()
        {
            return value();
        }

        operator T()
        {
            return value();
        }

        dlg_selection& operator=(const int& rhs)
        {
            value(rhs);
            return *this;
        }
    };


    namespace details {

        template<int ID>
        void dlg_bind(dlg_value<ID, bool>& b)
        {
            if (!b.dlg) return;
            b.dlg->set_dlg_item_checked(b.id, b.val_);
        }

        template<int ID>
        void dlg_bind(dlg_value<ID, int>& b)
        {
            if (!b.dlg) return;
            b.dlg->set_dlg_item_int(b.id, b.val_);
        }

        template<int ID>
        void dlg_bind(dlg_selection<ID, int>& s)
        {
            if (!s.dlg) return;
            s.dlg->set_dlg_item_index(s.id, s.val_);
        }

        template<int ID>
        void dlg_bind(dlg_selection<ID, std::wstring>& s)
        {
            if (!s.dlg) return;
            s.dlg->set_dlg_item_text(s.id, s.val_);
        }

        template<int ID>
        void dlg_bind(dlg_selection<ID, std::vector<int>>& s)
        {
            if (!s.dlg) return;
            s.dlg->set_dlg_item_multi_select_index(s.id, s.val_);
        }

        template<int ID>
        void dlg_bind(dlg_selection<ID, std::vector<std::wstring>>& s)
        {
            if (!s.dlg) return;
            s.dlg->set_dlg_item_multi_select_index(s.id, s.val_);
        }

        template<int ID>
        void dlg_bind(dlg_value<ID, std::wstring>& b)
        {
            if (!b.dlg) return;
            b.dlg->set_dlg_item_text(b.id, b.val_);
        }

        template<int ID>
        void dlg_bind(dlg_value<ID, std::vector<std::wstring>>& b)
        {
            if (!b.dlg) return;
            auto v = b.val_;
            b.dlg->clear_dlg_item_list(b.id);
            for (auto& i : v)
            {
                b.dlg->add_dlg_item_text(b.id, i);
            }
        }

        /*
        inline void dlg_bind( ) {}

        template<class T, class ... Args>
        void dlg_bind(  T t, Args ... args)
        {
            dlg_bind(t);
            dlg_bind(args...);
        }
        */
        template<int ID>
        void dlg_sync(dlg_value<ID, bool>& b)
        {
            if (!b.dlg) return;
            b.val_ = b.dlg->get_dlg_item_checked(b.id);
        }

        template<int ID>
        void dlg_sync(dlg_value<ID, int>& b)
        {
            if (!b.dlg) return;
            b.val_ = b.dlg->get_dlg_item_int(b.id);
        }

        template<int ID>
        void dlg_sync(dlg_selection<ID, int>& s)
        {
            if (!s.dlg) return;
            s.val_ = s.dlg->get_dlg_item_index(s.id);
        }

        template<int ID>
        void dlg_sync(dlg_selection<ID, std::wstring>& s)
        {
            if (!s.dlg) return;
            s.val_ = s.dlg->get_dlg_item_text(s.id);
        }

        template<int ID>
        void dlg_sync(dlg_selection<ID, std::vector<int>>& s)
        {
            if (!s.dlg) return;
            s.val_ = s.dlg->get_dlg_item_multi_select_index(s.id);
        }

        template<int ID>
        void dlg_sync(dlg_selection<ID, std::vector<std::wstring>>& s)
        {
            if (!s.dlg) return;
            s.val_ = s.dlg->get_dlg_item_multi_Select_text(s.id);
        }

        template<int ID>
        void dlg_sync(dlg_value<ID, std::wstring>& b)
        {
            if (!b.dlg) return;
            b.val_ = b.dlg->get_dlg_item_text(b.id);
        }

        template<int ID>
        void dlg_sync(dlg_value<ID, std::vector<std::wstring>>& b)
        {
            if (!b.dlg) return;
            b.val_.clear();

            int cnt = b.dlg->get_dlg_item_count(b.id);
            for (int i = 0; i < cnt; i++)
            {
                std::wstring s = b.dlg->get_dlg_item_text(b.id, i);
                b.val_.push_back(s);
            }
        }
    }

    template<int I, class T>
    void dlg_value<I, T>::value(const T& t)
    {
        val_ = t;
        if (!dlg) return;
        details::dlg_bind(*this);
    }

    template<int I, class T>
    T& dlg_value<I, T>::value()
    {
        //if (!dlg) return val_;
        //dlg_sync(*this);
        return val_;
    }

    template<int I, class T>
    void dlg_selection<I, T>::value(const T& t)
    {
        val_ = t;
        if (!dlg) return;
        dlg_bind(*this);
    }

    template<int I, class T>
    T& dlg_selection<I, T>::value()
    {
        //if (!dlg) return val_;
        //dlg_sync(*this);
        return val_;
    }

    namespace details {

        class Binding
        {
        public:
            virtual ~Binding() {}

            virtual void bind(dialog& dlg) = 0;
            virtual void sync() = 0;
        };

        template<class ... Args>
        class Bindings : public Binding
        {
        public:

            std::tuple<Args&...> bound;

            template<class ... Args>
            Bindings(Args& ... args)
                : bound(args ...)
            {}

            void bind(dialog& dlg) override
            {
                bind(dlg, bound);
            }

            void sync() override
            {
                sync(bound);
            }

        private:

            template<std::size_t I = 0, typename... Tp>
            typename std::enable_if<I == sizeof...(Tp), void>::type bind(dialog& dlg, std::tuple<Tp...>& t)
            {}

            template<std::size_t I = 0, typename... Tp>
            typename std::enable_if < I < sizeof...(Tp), void>::type bind(dialog& dlg, std::tuple<Tp...>& t)
            {
                auto& b = std::get<I>(t);
                b.dlg = &dlg;
                dlg_bind(b);

                bind<I + 1, Tp...>(dlg, t);
            }

            template<std::size_t I = 0, typename... Tp>
            typename std::enable_if<I == sizeof...(Tp), void>::type sync(std::tuple<Tp...>& t)
            {}

            template<std::size_t I = 0, typename... Tp>
            typename std::enable_if < I < sizeof...(Tp), void>::type sync(std::tuple<Tp...>& t)
            {
                auto& b = std::get<I>(t);
                dlg_sync(b);

                sync<I + 1, Tp...>(t);
            }

        };


        class Binder
        {
        public:

            Binder(dialog* dlg)
                : dlg_(*dlg)
            {}

            template<class ... Args>
            Binder& operator()(Args&... args)
            {
                binding_ = std::unique_ptr<Binding>(new Bindings<Args...>(args...));
                return *this;
            }

            void bind()
            {
                if (!binding_) return;
                binding_->bind(dlg_);
            }

            void sync()
            {
                if (!binding_) return;
                binding_->sync();
            }

        private:
            Binder(const Binder& rhs) = delete;
            Binder& operator=(const Binder& rhs) = delete;

            dialog& dlg_;
            std::unique_ptr<Binding> binding_;
        };

    } // end namespace details

    class dialog : public wnd
    {
    public:

        details::Binder binding;

        dialog()
            : binding(this)
        {}

        std::wstring class_name(int id)
        {
            wchar_t buf[1024];
            ::GetClassName(get_dlg_item(id), buf, 1024);
        }

        bool is_class(int id, const wchar_t* className)
        {
            wchar_t buf[1024];
            ::GetClassName(get_dlg_item(id), buf, 1024);
            if (_wcsicmp(buf, className) == 0)
            {
                return true;
            }
            return false;
        }

        HWND show_modeless(int lpTemplate, HWND hWndParent)
        {
            isModal_ = false;
            handle = ::CreateDialogParam(module_instance(), MAKEINTRESOURCE(lpTemplate), hWndParent, &dialog::dialogProcedure, (LPARAM)(this));

            modeless_dlg().add(handle);
            return handle;
        }

        LRESULT show_modal(int lpTemplate, HWND hWndParent)
        {
            return ::DialogBoxParam(module_instance(), MAKEINTRESOURCE(lpTemplate), hWndParent, &dialog::dialogProcedure, (LPARAM)(this));
        }

        LRESULT send_dlg_item_msg(int id, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            return (::SendMessage(dialog_item(id), msg, wParam, lParam));
        }

        LRESULT set_dlg_item_text(int id, const std::wstring& s)
        {
            if (is_class(id, L"LISTBOX"))
            {
                LRESULT r = send_dlg_item_msg(id, LB_FINDSTRINGEXACT, -1, (LPARAM)(s.c_str()));
                if (r != LB_ERR)
                {
                    return send_dlg_item_msg(id, LB_SETCURSEL, r, 0);
                }
            }
            else
            {
                return send_dlg_item_msg(id, WM_SETTEXT, 0, (LPARAM)(s.c_str()));
            }
            return 0;
        }

        LRESULT set_dlg_item_font(int id, HFONT hfont)
        {
            return send_dlg_item_msg(id, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
        }

        LRESULT set_dlg_button_icon(int id, HICON hicon)
        {
            return send_dlg_item_msg(id, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)(hicon));
        }

        LRESULT set_dlg_button_img(int id, HBITMAP bmp)
        {
            return send_dlg_item_msg(id, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(bmp));
        }

        LRESULT set_dlg_static_icon(int id, HICON hicon)
        {
            return send_dlg_item_msg(id, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)(hicon));
        }

        LRESULT set_dlg_item_int(int id, int value)
        {
            std::wstring s = std::to_wstring(value);
            return set_dlg_item_text(id, s);
        }

        void  set_dlg_item_checked(int id, int state = BST_CHECKED)
        {
            send_dlg_item_msg(id, BM_SETCHECK, state, 0);
        }

        void add_dlg_item_text(int id, const std::wstring& s)
        {
            if (is_class(id, L"COMBOBOX"))
            {
                send_dlg_item_msg(id, CB_ADDSTRING, 0, (LPARAM)(s.c_str()));
            }
            else
                if (is_class(id, L"LISTBOX"))
                {
                    send_dlg_item_msg(id, LB_ADDSTRING, 0, (LPARAM)(s.c_str()));
                }

        }

        void add_dlg_item_text(int id, const std::vector<std::wstring>& v)
        {
            for (auto& s : v)
            {
                add_dlg_item_text(id, s);
            }
        }

        void clear_dlg_item_list(int id)
        {
            LRESULT r = get_dlg_item_count(id);

            if (is_class(id, L"COMBOBOX"))
            {
                while (r > 0)
                {
                    r = send_dlg_item_msg(id, CB_DELETESTRING, 0, 0);
                }
            }
            else
                if (is_class(id, L"LISTBOX"))
                {
                    while (r > 0)
                    {
                        r = send_dlg_item_msg(id, LB_DELETESTRING, 0, 0);
                    }
                }
        }

        void set_dlg_item_index(int id, int index)
        {
            if (is_class(id, L"COMBOBOX"))
            {
                send_dlg_item_msg(id, CB_SETCURSEL, (WPARAM)index, 0);
            }
            else
                if (is_class(id, L"LISTBOX"))
                {
                    send_dlg_item_msg(id, LB_SETCURSEL, (WPARAM)index, 0);
                }
        }

        void set_dlg_item_multi_select_index(int id, const std::vector<int>& v)
        {
            if (is_class(id, L"LISTBOX"))
            {
                for (auto& i : v)
                {
                    send_dlg_item_msg(id, LB_SETSEL, TRUE, i);
                }
            }
        }

        void set_dlg_item_multi_select_index(int id, const std::vector<std::wstring>& v)
        {
            if (is_class(id, L"LISTBOX"))
            {
                for (auto& i : v)
                {
                    LRESULT r = send_dlg_item_msg(id, LB_FINDSTRINGEXACT, 0, (LPARAM)i.c_str());
                    if (r != LB_ERR)
                    {
                        send_dlg_item_msg(id, LB_SETSEL, TRUE, r);
                    }
                }
            }
        }

        std::wstring get_dlg_item_text(int id)
        {
            wchar_t  buf[4096];
            if (is_class(id, L"LISTBOX"))
            {
                int r = get_dlg_item_index(id);
                if (r == LB_ERR) return L"";
                send_dlg_item_msg(id, LB_GETTEXT, r, (LPARAM)buf);
            }
            else
            {
                if (!(::GetDlgItemText(handle, id, buf, 4096)))
                    return L"";
            }
            return std::wstring(buf);
        }

        std::wstring get_dlg_item_text(int id, int index)
        {
            wchar_t  buf[4096];

            if (is_class(id, L"COMBOBOX"))
            {
                send_dlg_item_msg(id, CB_GETLBTEXT, index, (LPARAM)buf);
            }
            else
                if (is_class(id, L"LISTBOX"))
                {
                    send_dlg_item_msg(id, LB_GETTEXT, index, (LPARAM)buf);
                }

            return std::wstring(buf);
        }

        int get_dlg_item_int(int id)
        {
            std::wstring s = get_dlg_item_text(id);
            std::wistringstream iss(s);
            int i = 0;
            iss >> i;
            return i;
        }

        int get_dlg_item_count(int id)
        {
            if (is_class(id, L"COMBOBOX"))
            {
                return (int)send_dlg_item_msg(id, CB_GETCOUNT, 0, 0);
            }
            else
                if (is_class(id, L"LISTBOX"))
                {
                    return (int)send_dlg_item_msg(id, LB_GETCOUNT, 0, 0);
                }
            return 0;
        }

        HWND get_dlg_item(int id)
        {
            return (::GetDlgItem(handle, id));
        }

        bool  get_dlg_item_checked(int id)
        {
            return (BST_CHECKED == send_dlg_item_msg(id, BM_GETCHECK, 0, 0));
        }


        int get_dlg_item_index(int id)
        {
            if (is_class(id, L"COMBOBOX"))
            {
                return (int)send_dlg_item_msg(id, CB_GETCURSEL, 0, 0);
            }
            else
                if (is_class(id, L"LISTBOX"))
                {
                    return (int)send_dlg_item_msg(id, LB_GETCURSEL, 0, 0);
                }
            return 0;
        }

        std::vector<int> get_dlg_item_multi_select_index(int id)
        {
            std::vector<int> result;
            if (is_class(id, L"LISTBOX"))
            {
                int n = (int)send_dlg_item_msg(id, LB_GETSELCOUNT, 0, 0);

                int* a = new int[n];
                send_dlg_item_msg(id, LB_GETSELITEMS, n, (LPARAM)a);
                for (int i = 0; i < n; i++)
                {
                    result.push_back(a[i]);
                }
                delete[] a;
            }
            return result;
        }

        std::vector<std::wstring> get_dlg_item_multi_select_text(int id)
        {
            std::vector<std::wstring> result;
            if (is_class(id, L"LISTBOX"))
            {
                int n = (int)send_dlg_item_msg(id, LB_GETSELCOUNT, 0, 0);

                int* a = new int[n];
                send_dlg_item_msg(id, LB_GETSELITEMS, n, (LPARAM)a);
                for (int i = 0; i < n; i++)
                {
                    std::wstring s = get_dlg_item_text(id, a[i]);
                    result.push_back(s);
                }
                delete[] a;
            }
            return result;
        }

        void center()
        {
            RECT r;
            ::GetWindowRect(handle, &r);

            RECT s;
            ::GetWindowRect(::GetDesktopWindow(), &s);

            int x = s.right / 2 - (r.right - r.left) / 2;
            int y = s.bottom / 2 - (r.bottom - r.top) / 2;
            int w = (r.right - r.left);
            int h = (r.bottom - r.top);
            move(x, y, w, h);
        }

        event<int()> onClick;
        event<int()> onEndDlg;
        event<void(dialog&)> onInitDlg;

        virtual void on_init()
        {
            onInitDlg.fire(*this);
        }

        LRESULT wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)
            {
            case WM_INITDIALOG:
            {
                handle = hwnd;
                binding.bind();
                on_init();
                return 0;
            }
            case WM_ERASEBKGND:
            {
                break;
            }
            case WM_COMMAND:
            {
                onClick.fire(LOWORD(wParam));
                switch (LOWORD(wParam))
                {
                case IDOK:
                {
                    binding.sync();
                    // Fall through. 
                }
                case IDCANCEL:
                {
                    end((int)wParam);
                    return TRUE;
                }
                default:
                {
                    return TRUE;
                }
                }
                break;
            }
            }
            return FALSE;
        }


#ifdef _WIN64
        static LRESULT CALLBACK dialogProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            if (message == WM_INITDIALOG)
            {
                dialog* that = (dialog*)lParam;
                ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)that);
            }

            LONG_PTR l = ::GetWindowLongPtrW(hwnd, GWLP_USERDATA); 
            if (!l)
            {
                return FALSE;
            }

            dialog* that = (dialog*)l;
            return (LRESULT)(that->wndProc(hwnd, message, wParam, lParam));
        }
#else
        static BOOL CALLBACK dialogProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            if (message == WM_INITDIALOG)
            {
                dialog* that = (dialog*)lParam;
                ::SetWindowLongPtrW(hwnd, GWL_USERDATA, (LONG)that);
            }

            LONG_PTR l = ::GetWindowLongPtrW(hwnd, GWL_USERDATA);
            if (!l)
            {
                return FALSE;
            }

            dialog* that = (dialog*)l;
            return (BOOL)(that->wndProc(hwnd, message, wParam, lParam));
        }
#endif

        LRESULT end(int n)
        {
            onEndDlg.fire(n);
            if (isModal_)
            {
                return ::EndDialog(handle, n);
            }
            modeless_dlg().remove(handle);
            ::DestroyWindow(handle);
            handle = nullptr;
            return n;
        }

    protected:

        bool isModal_ = true;

        virtual HWND create_window(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) override
        {
            return 0;
        }

    private:

    };

}
