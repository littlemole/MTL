#pragma once

#include <MTL/win/wind.h>
#include <sstream>

namespace MTL {

    class Dialog;


    template<int ID, class T>
    class Bind
    {
    public:
        int id = ID;
        T val_;
        Dialog* dlg = nullptr;

        Bind()
        {}

        Bind(const T& t)
            : val_(t)
        {}

        void value(const T& t);
        T& value();

        T& operator*()
        {
            return value();
        }

        Bind& operator=(const T& rhs)
        {
            value(rhs);
            return *this;
        }
    };

    template<int ID, class T>
    class Selection
    {
    public:
        int id = ID;
        T val_;
        Dialog* dlg = nullptr;

        Selection()
        {}

        Selection(const T& t)
            : val_(t)
        {}

        void value(const T& t);
        T& value();

        T& operator*()
        {
            return value();
        }

        Selection& operator=(const int& rhs)
        {
            value(rhs);
            return *this;
        }
    };


    namespace details {

        template<int ID>
        void dlg_bind(Bind<ID, bool>& b)
        {
            if (!b.dlg) return;
            b.dlg->setDlgItemChecked(b.id, b.val_);
        }

        template<int ID>
        void dlg_bind(Bind<ID, int>& b)
        {
            if (!b.dlg) return;
            b.dlg->setDlgItemInt(b.id, b.val_);
        }

        template<int ID>
        void dlg_bind(Selection<ID, int>& s)
        {
            if (!s.dlg) return;
            s.dlg->setDlgItemIndex(s.id, s.val_);
        }

        template<int ID>
        void dlg_bind(Selection<ID, std::vector<int>>& s)
        {
            if (!s.dlg) return;
            s.dlg->setDlgItemMultiSelectIndex(s.id, s.val_);
        }

        template<int ID>
        void dlg_bind(Selection<ID, std::vector<std::wstring>>& s)
        {
            if (!s.dlg) return;
            s.dlg->setDlgItemMultiSelectIndex(s.id, s.val_);
        }

        template<int ID>
        void dlg_bind(Bind<ID, std::wstring>& b)
        {
            if (!b.dlg) return;
            b.dlg->setDlgItemText(b.id, b.val_);
        }

        template<int ID>
        void dlg_bind(Bind<ID, std::vector<std::wstring>>& b)
        {
            if (!b.dlg) return;
            auto v = b.val_;
            b.dlg->clearDlgItemList(b.id);
            for (auto& i : v)
            {
                b.dlg->addDlgItemText(b.id, i);
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
        void dlg_sync(Bind<ID, bool>& b)
        {
            if (!b.dlg) return;
            b.val_ = b.dlg->getDlgItemChecked(b.id);
        }

        template<int ID>
        void dlg_sync(Bind<ID, int>& b)
        {
            if (!b.dlg) return;
            b.val_ = b.dlg->getDlgItemInt(b.id);
        }

        template<int ID>
        void dlg_sync(Selection<ID, int>& s)
        {
            if (!s.dlg) return;
            s.val_ = s.dlg->getDlgItemIndex(s.id);
        }

        template<int ID>
        void dlg_sync(Selection<ID, std::vector<int>>& s)
        {
            if (!s.dlg) return;
            s.val_ = s.dlg->getDlgItemMultiSelectIndex(s.id);
        }

        template<int ID>
        void dlg_sync(Selection<ID, std::vector<std::wstring>>& s)
        {
            if (!s.dlg) return;
            s.val_ = s.dlg->getDlgItemMultiSelectText(s.id);
        }

        template<int ID>
        void dlg_sync(Bind<ID, std::wstring>& b)
        {
            if (!b.dlg) return;
            b.val_ = b.dlg->getDlgItemText(b.id);
        }

        template<int ID>
        void dlg_sync(Bind<ID, std::vector<std::wstring>>& b)
        {
            if (!b.dlg) return;
            b.val_.clear();

            int cnt = b.dlg->getDlgItemCount(b.id);
            for (int i = 0; i < cnt; i++)
            {
                std::wstring s = b.dlg->getDlgItemText(b.id, i);
                b.val_.push_back(s);
            }
        }
    }

    template<int I, class T>
    void Bind<I, T>::value(const T& t)
    {
        val_ = t;
        if (!dlg) return;
        dlg_bind(*this);
    }

    template<int I, class T>
    T& Bind<I, T>::value()
    {
        //if (!dlg) return val_;
        //dlg_sync(*this);
        return val_;
    }

    template<int I, class T>
    void Selection<I, T>::value(const T& t)
    {
        val_ = t;
        if (!dlg) return;
        dlg_bind(*this);
    }

    template<int I, class T>
    T& Selection<I, T>::value()
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

            virtual void bind(Dialog& dlg) = 0;
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

            void bind(Dialog& dlg) override
            {
                bind(dlg, bound);
            }

            void sync() override
            {
                sync(bound);
            }

        private:

            template<std::size_t I = 0, typename... Tp>
            typename std::enable_if<I == sizeof...(Tp), void>::type bind(Dialog& dlg, std::tuple<Tp...>& t)
            {}

            template<std::size_t I = 0, typename... Tp>
            typename std::enable_if < I < sizeof...(Tp), void>::type bind(Dialog& dlg, std::tuple<Tp...>& t)
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

            Binder(Dialog* dlg)
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

            Dialog& dlg_;
            std::unique_ptr<Binding> binding_;
        };

    } // end namespace details

    class Dialog : public Wnd
    {
    public:

        details::Binder binding;

        Dialog()
            : binding(this)
        {}

        std::wstring className(int id)
        {
            wchar_t buf[1024];
            ::GetClassName(getDlgItem(id), buf, 1024);
        }

        bool isClass(int id, const wchar_t* className)
        {
            wchar_t buf[1024];
            ::GetClassName(getDlgItem(id), buf, 1024);
            if (_wcsicmp(buf, className) == 0)
            {
                return true;
            }
            return false;
        }

        HWND showModeless(int lpTemplate, HWND hWndParent)
        {
            isModal_ = false;
            HWND hWnd = ::CreateDialogParam(module_instance(), MAKEINTRESOURCE(lpTemplate), hWndParent, &Dialog::dialogProcedure, (LPARAM)(this));

            modelessDialogs().add(hWnd);
            return hWnd;
        }

        LRESULT showModal(int lpTemplate, HWND hWndParent)
        {
            return ::DialogBoxParam(module_instance(), MAKEINTRESOURCE(lpTemplate), hWndParent, &Dialog::dialogProcedure, (LPARAM)(this));
        }

        LRESULT sendDlgItemMsg(int id, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            return (::SendMessage(dialogItem(id), msg, wParam, lParam));
        }

        LRESULT setDlgItemText(int id, const std::wstring& s)
        {
            if (isClass(id, L"LISTBOX"))
            {
                LRESULT r = sendDlgItemMsg(id, LB_FINDSTRINGEXACT, -1, (LPARAM)(s.c_str()));
                if (r != LB_ERR)
                {
                    return sendDlgItemMsg(id, LB_SETCURSEL, r, 0);
                }
            }
            else
            {
                return sendDlgItemMsg(id, WM_SETTEXT, 0, (LPARAM)(s.c_str()));
            }
            return 0;
        }

        LRESULT setDlgItemFont(int id, HFONT hfont)
        {
            return sendDlgItemMsg(id, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
        }

        LRESULT setDlgButtonIcon(int id, HICON hicon)
        {
            return sendDlgItemMsg(id, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)(hicon));
        }

        LRESULT setDlgButtonImg(int id, HBITMAP bmp)
        {
            return sendDlgItemMsg(id, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)(bmp));
        }

        LRESULT setDlgStaticIcon(int id, HICON hicon)
        {
            return sendDlgItemMsg(id, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)(hicon));
        }

        LRESULT setDlgItemInt(int id, int value)
        {
            std::wstring s = std::to_wstring(value);
            return setDlgItemText(id, s);
        }

        void  setDlgItemChecked(int id, int state = BST_CHECKED)
        {
            sendDlgItemMsg(id, BM_SETCHECK, state, 0);
        }

        void addDlgItemText(int id, const std::wstring& s)
        {
            if (isClass(id, L"COMBOBOX"))
            {
                sendDlgItemMsg(id, CB_ADDSTRING, 0, (LPARAM)(s.c_str()));
            }
            else
                if (isClass(id, L"LISTBOX"))
                {
                    sendDlgItemMsg(id, LB_ADDSTRING, 0, (LPARAM)(s.c_str()));
                }

        }

        void addDlgItemText(int id, const std::vector<std::wstring>& v)
        {
            for (auto& s : v)
            {
                addDlgItemText(id, s);
            }
        }

        void clearDlgItemList(int id)
        {
            LRESULT r = getDlgItemCount(id);

            if (isClass(id, L"COMBOBOX"))
            {
                while (r > 0)
                {
                    r = sendDlgItemMsg(id, CB_DELETESTRING, 0, 0);
                }
            }
            else
                if (isClass(id, L"LISTBOX"))
                {
                    while (r > 0)
                    {
                        r = sendDlgItemMsg(id, LB_DELETESTRING, 0, 0);
                    }
                }
        }

        void setDlgItemIndex(int id, int index)
        {
            if (isClass(id, L"COMBOBOX"))
            {
                sendDlgItemMsg(id, CB_SETCURSEL, (WPARAM)index, 0);
            }
            else
                if (isClass(id, L"LISTBOX"))
                {
                    sendDlgItemMsg(id, LB_SETCURSEL, (WPARAM)index, 0);
                }
        }

        void setDlgItemMultiSelectIndex(int id, const std::vector<int>& v)
        {
            if (isClass(id, L"LISTBOX"))
            {
                for (auto& i : v)
                {
                    sendDlgItemMsg(id, LB_SETSEL, TRUE, i);
                }
            }
        }

        void setDlgItemMultiSelectIndex(int id, const std::vector<std::wstring>& v)
        {
            if (isClass(id, L"LISTBOX"))
            {
                for (auto& i : v)
                {
                    LRESULT r = sendDlgItemMsg(id, LB_FINDSTRINGEXACT, 0, (LPARAM)i.c_str());
                    if (r != LB_ERR)
                    {
                        sendDlgItemMsg(id, LB_SETSEL, TRUE, r);
                    }
                }
            }
        }

        std::wstring getDlgItemText(int id)
        {
            wchar_t  buf[4096];
            if (isClass(id, L"LISTBOX"))
            {
                int r = getDlgItemIndex(id);
                if (r == LB_ERR) return L"";
                sendDlgItemMsg(id, LB_GETTEXT, r, (LPARAM)buf);
            }
            else
            {
                if (!(::GetDlgItemText(handle, id, buf, 4096)))
                    return L"";
            }
            return std::wstring(buf);
        }

        std::wstring getDlgItemText(int id, int index)
        {
            wchar_t  buf[4096];

            if (isClass(id, L"COMBOBOX"))
            {
                sendDlgItemMsg(id, CB_GETLBTEXT, index, (LPARAM)buf);
            }
            else
                if (isClass(id, L"LISTBOX"))
                {
                    sendDlgItemMsg(id, LB_GETTEXT, index, (LPARAM)buf);
                }

            return std::wstring(buf);
        }

        int getDlgItemInt(int id)
        {
            std::wstring s = getDlgItemText(id);
            std::wistringstream iss(s);
            int i = 0;
            iss >> i;
            return i;
        }

        int getDlgItemCount(int id)
        {
            if (isClass(id, L"COMBOBOX"))
            {
                return (int)sendDlgItemMsg(id, CB_GETCOUNT, 0, 0);
            }
            else
                if (isClass(id, L"LISTBOX"))
                {
                    return (int)sendDlgItemMsg(id, LB_GETCOUNT, 0, 0);
                }
            return 0;
        }

        HWND getDlgItem(int id)
        {
            return (::GetDlgItem(handle, id));
        }

        bool  getDlgItemChecked(int id)
        {
            return (BST_CHECKED == sendDlgItemMsg(id, BM_GETCHECK, 0, 0));
        }


        int getDlgItemIndex(int id)
        {
            if (isClass(id, L"COMBOBOX"))
            {
                return (int)sendDlgItemMsg(id, CB_GETCURSEL, 0, 0);
            }
            else
                if (isClass(id, L"LISTBOX"))
                {
                    return (int)sendDlgItemMsg(id, LB_GETCURSEL, 0, 0);
                }
            return 0;
        }

        std::vector<int> getDlgItemMultiSelectIndex(int id)
        {
            std::vector<int> result;
            if (isClass(id, L"LISTBOX"))
            {
                int n = (int)sendDlgItemMsg(id, LB_GETSELCOUNT, 0, 0);

                int* a = new int[n];
                sendDlgItemMsg(id, LB_GETSELITEMS, n, (LPARAM)a);
                for (int i = 0; i < n; i++)
                {
                    result.push_back(a[i]);
                }
                delete[] a;
            }
            return result;
        }

        std::vector<std::wstring> getDlgItemMultiSelectText(int id)
        {
            std::vector<std::wstring> result;
            if (isClass(id, L"LISTBOX"))
            {
                int n = (int)sendDlgItemMsg(id, LB_GETSELCOUNT, 0, 0);

                int* a = new int[n];
                sendDlgItemMsg(id, LB_GETSELITEMS, n, (LPARAM)a);
                for (int i = 0; i < n; i++)
                {
                    std::wstring s = getDlgItemText(id, a[i]);
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

        virtual void onInit()
        {

        }

        Event<int()> onClick;
        Event<int()> onEndDlg;

        LRESULT wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)
            {
            case WM_INITDIALOG:
            {
                handle = hwnd;
                binding.bind();
                onInit();
                return 0;
            }
            case WM_ERASEBKGND:
            {
                break;
            }
            case WM_COMMAND:
            {
                switch (LOWORD(wParam))
                {
                case IDOK:
                {
                    binding.sync();
                    // Fall through. 
                }
                case IDCANCEL:
                {
                    endDlg((int)wParam);
                    return TRUE;
                }
                default:
                {
                    onClick.fire(LOWORD(wParam));
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
                Dialog* that = (Dialog*)lParam;
                ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)that);
            }

            LONG_PTR l = ::GetWindowLongPtrW(hwnd, GWLP_USERDATA); 
            if (!l)
            {
                return FALSE;
            }

            Dialog* that = (Dialog*)l;
            return (LRESULT)(that->wndProc(hwnd, message, wParam, lParam));
        }
#else
        static BOOL CALLBACK dialogProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            if (message == WM_INITDIALOG)
            {
                Dialog* that = (Dialog*)lParam;
                ::SetWindowLongPtrW(hwnd, GWL_USERDATA, (LONG)that);
            }

            LONG_PTR l = ::GetWindowLongPtrW(hwnd, GWL_USERDATA);
            if (!l)
            {
                return FALSE;
            }

            Dialog* that = (Dialog*)l;
            return (BOOL)(that->wndProc(hwnd, message, wParam, lParam));
        }
#endif

        LRESULT endDlg(int n)
        {
            onEndDlg.fire(n);
            if (isModal_)
            {
                return ::EndDialog(handle, n);
            }
            modelessDialogs().remove(handle);
            ::DestroyWindow(handle);
            return n;
        }

    protected:

        bool isModal_ = true;

        virtual HWND createWindow(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu)
        {
            return 0;
        }

    private:

    };

}
