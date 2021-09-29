#pragma once

#include "mtl/win/wnd.h"
#include "mtl/win/layout.h"

namespace mtl {

    class mdi_child;

    namespace details {

        HMENU get_view_submenu(HMENU menu, int pos = -1)
        {
            HMENU subMenu = nullptr;
            if (pos == -1)
            {
                int c = ::GetMenuItemCount(menu);
                switch (c)
                {
                    case 0 : 
                    {
                        return 0;
                        break;
                    }
                    case 1:
                    {
                        pos = 0;
                        break;
                    }
                    case 2:
                    {
                        pos = 2;
                        break;
                    }
                    default:
                    {
                        pos = c - 2;
                        break;
                    }
                }
            }
            return ::GetSubMenu(menu,pos);
        }
    }

	class mdi_frame : public window<mdi_frame>
	{
	public:

        HMENU menu = nullptr;

        default_layout layout;

        mdi_frame(int windowMenuIndex = -1, UINT idFirstMdiChild = 50000)
        {
            windowMenuIndex_ = windowMenuIndex;
            ccs_.idFirstChild = idFirstMdiChild;
        }

        void maximize()
        {
            ::PostMessage(mdiClient_, WM_MDIMAXIMIZE, (WPARAM)active(), 0);
        }

        void minimize()
        {
            ::ShowWindow(active(), SW_MINIMIZE);
            ::UpdateWindow(active());
            next();
        }

        void restore()
        {
            ::PostMessage(mdiClient_, WM_MDIRESTORE, (WPARAM)active(), 0);
        }

        void next()
        {
            ::PostMessage(mdiClient_, WM_MDINEXT, 0, 0);
        }

        void cascade()
        {
            ::PostMessage(mdiClient_, WM_MDICASCADE, 0, 0);
        }

        void tile_horizontal()
        {
            ::PostMessage(mdiClient_, WM_MDITILE, 0, 0);
        }

        void tile_vertical()
        {
            ::PostMessage(mdiClient_, WM_MDITILE, MDITILE_VERTICAL, 0);
        }

        void icon_arrange()
        {
            ::PostMessage(mdiClient_, WM_MDIICONARRANGE, 0, 0);
        }

        HWND active()
        {
            return (HWND)(::SendMessage(mdiClient_, WM_MDIGETACTIVE, 0, 0));
        }

        void destroy_child(HWND wnd)
        {
            ::PostMessage(mdiClient_, WM_MDIDESTROY, (WPARAM)(wnd), 0);
        }

        void reset_menu()
        {
            HWND client = mdi_client();
            if (client && menu)
            {
                HMENU viewMenu = details::get_view_submenu(menu, windowMenuIndex_);
                ::SendMessage(client, WM_MDISETMENU, (WPARAM)menu, (LPARAM)viewMenu);
                ::DrawMenuBar(handle);
            }
        }

        virtual LRESULT wm_size(RECT& clientRect) override
        {
            RECT r = { 0,0,0,0 };
            ::InvalidateRect(mdi_client(), 0, FALSE);
            layout.do_layout(clientRect, r);

            // do not do that here
            //::InvalidateRect(this->handle, 0, TRUE);
            return 0;
        }

        virtual LRESULT wm_draw(HDC dc, RECT& r) override
        {
            ::InvalidateRect(mdi_client(), 0, TRUE);
            ::UpdateWindow(mdi_client());
            return 0;
        }

        virtual LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            if (message == WM_LAYOUT)
            {
                RECT r;
                ::GetClientRect(hWnd, &r);
                this->wm_layout(r);
                return 0;
            }

            if (message == WM_SEARCH)
            {
                FINDREPLACE* fp = (FINDREPLACE*)lParam;
                this->wm_search(fp);
                return 0;
            }

            switch (message)
            {
            case WM_CREATE:
            {
                LPCREATESTRUCT lpcs = ((LPCREATESTRUCT)lParam);

                RECT bounds;
                ::GetClientRect(hWnd, &bounds);

                menu = lpcs->hMenu;

                mdiClient_ = ::CreateWindowEx(
                    WS_EX_WINDOWEDGE,
                    L"MDICLIENT",
                    L"",
                    WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
                    bounds.left, bounds.top,
                    bounds.right - bounds.left, bounds.bottom - bounds.top,
                    hWnd,
                    (HMENU)1,
                    module_instance(),
                    &ccs_
                );

                LRESULT r = this->wm_create();

                if (r) return r;
                return 0;
                break;
            }
            case WM_DESTROY:
            {
                LRESULT r = this->wm_destroy();
                if (r) return r;
                break;
            }
            case WM_COMMAND:
            {
                int code = HIWORD(wParam);
                int wmId = LOWORD(wParam);

                LRESULT r = 0;
                if (code == 0 && lParam == 0)
                {
                    r = this->wm_command(wmId);
                }
                else if (code == 1)
                {
                    r = this->wm_accellerator(wmId);
                }
                else
                {
                    r = this->wm_control(message, wParam, lParam);
                }

                HWND active = this->active();
                if (::IsWindow(active))
                    ::SendMessage(active, WM_COMMAND, wParam, lParam);
                break;
            }
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);

                RECT bounds;
                ::GetClientRect(hWnd, &bounds);
                return this->wm_size(bounds);
                break;
            }
            case WM_PAINT:
            {
                this->wm_paint();
                break;
            }
            case WM_ERASEBKGND:
            {
                return this->wm_erase_background(wParam);
            }
            case WM_DPICHANGED:
            {
                ::InvalidateRect(hWnd, 0, TRUE);
                RECT* r = (RECT*)lParam;
                if (r)
                {
                    ::SetWindowPos(hWnd, NULL, r->left, r->top, r->right - r->left, r->bottom - r->top, 0);
                }
                LRESULT result = this->wm_dpi_changed(r);
                ::UpdateWindow(hWnd);
                break;
            }
            case WM_DPICHANGED_BEFOREPARENT:
            {
                LRESULT result = this->wm_dpi_changed(0);
                ::InvalidateRect(hWnd, 0, TRUE);
                ::UpdateWindow(hWnd);
                break;
            }
            default:
            {
                break;
            }
            }

            //if (::IsWindow(mdiClient_))
            {
                return ::DefFrameProc(hWnd, mdiClient_, message, wParam, lParam);
            }

            return ::DefWindowProc(hWnd, message, wParam, lParam);
        }

        HWND mdi_client()
        {
            return mdiClient_;
        }
        
        template<class C, class ... Args>
        C& create_mdi_child(std::wstring title, Args ... args)
        {
            C* c = new C(args...);
            HWND hWnd = create_document(title, (mdi_child*)c);
            return *c;
        }
        
        HWND create_document(std::wstring title, mdi_child* wnd, UINT styles = WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS |
            WS_CLIPCHILDREN | WS_THICKFRAME |
            WS_SYSMENU | WS_CAPTION | MDIS_ALLCHILDSTYLES|
            WS_MAXIMIZEBOX | WS_MINIMIZEBOX, UINT exStyles = WS_EX_MDICHILD);

	private:
        CLIENTCREATESTRUCT ccs_;
        HWND mdiClient_ = nullptr;
        int windowMenuIndex_ = -1;
	};


    class mdi_child : public window<mdi_child>
    {
    public:

        mtl::menu menu;

        mdi_child()
        {
            auto& clazz = wc<mdi_child>();
            clazz.lpfnWndProc = &mdi_child::windowProc;
        }

        mdi_child(int menuId, int viewMenuIndex = -1)
            : menu(menuId), viewMenuIndex_(viewMenuIndex)
        {
            auto& clazz = wc<mdi_child>();
            clazz.lpfnWndProc = &mdi_child::windowProc;
        }

        virtual LRESULT wm_size(RECT& clientRect) override
        {
            return 0;
        }

        virtual void destroy() override
        {
            if (handle)
            {
                mdi_frame* frame = unwrap<mdi_frame>(::GetParent(::GetParent(handle)));
                if (frame)
                {
                    frame->destroy_child(handle);
                }
            }
        }

        virtual LRESULT wm_nc_destroy() override
        {
            return 0;
        }

        virtual HWND create(const wchar_t* title, int style = WS_OVERLAPPEDWINDOW, int exStyle = 0, HMENU menu = nullptr, HWND parent = nullptr) override;

        virtual LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override
        {
            if (message == WM_LAYOUT)
            {
                RECT r;
                ::GetClientRect(hWnd, &r);
                this->wm_layout(r);
                return 0;
            }

            if (message == WM_SEARCH)
            {
                FINDREPLACE* fp = (FINDREPLACE*)lParam;
                this->wm_search(fp);
                return 0;
            }

            switch (message)
            {
            case WM_CREATE:
            {
                LRESULT r = this->wm_create();
                break;
            }
            case WM_DESTROY:
            {
                LRESULT r = this->wm_destroy();
                if (r) return r;
                break;
            }
            case WM_NCDESTROY:
            {
                LRESULT r = this->wm_nc_destroy();
                if (r) return r;
                delete this;
                break;
            }
            case WM_COMMAND:
            {
                int code = HIWORD(wParam);
                int wmId = LOWORD(wParam);

                LRESULT r = 0;
                if (code == 0 && lParam == 0)
                {
                    r = this->wm_command(wmId);
                }
                else if (code == 1)
                {
                    r = this->wm_accellerator(wmId);
                }
                else
                {
                    r = this->wm_control(message, wParam, lParam);
                }

                break;
            }
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);

                RECT bounds;
                ::GetClientRect(hWnd, &bounds);
                this->wm_size(bounds);
                break;
            }
            case WM_PAINT:
            {
                this->wm_paint();
                break;
            }
            case WM_ERASEBKGND:
            {
                return this->wm_erase_background(wParam);
            }
            case WM_DPICHANGED:
            {
                ::InvalidateRect(hWnd, 0, TRUE);
                RECT* r = (RECT*)lParam;
                if (r)
                {
                    ::SetWindowPos(hWnd, NULL, r->left, r->top, r->right - r->left, r->bottom - r->top, 0);
                }
                LRESULT result = this->wm_dpi_changed(r);
                ::UpdateWindow(hWnd);
                break;
            }
            case WM_DPICHANGED_BEFOREPARENT:
            {
                LRESULT result = this->wm_dpi_changed(0);
                ::InvalidateRect(hWnd, 0, TRUE);
                ::UpdateWindow(hWnd);
                break;
            }
            case WM_MDIACTIVATE:
            {
                HWND frameWnd = ::GetParent(::GetParent(hWnd));

                // if this wnd got activated
                if (lParam == (LPARAM)hWnd)
                {
                    if (menu)
                    {
                        HMENU viewMenu = details::get_view_submenu(*menu, viewMenuIndex_);
                        ::SendMessage(::GetParent(hWnd), WM_MDISETMENU, (WPARAM)*menu, (LPARAM)viewMenu);
                        ::DrawMenuBar(frameWnd);
                    }
                }
                else // deactivated
                {
                    mdi_frame* frame = unwrap<mdi_frame>(frameWnd);
                    if (frame)
                    {
                        frame->reset_menu();
                    }
                }
                break;
            }
            default:
            {
                break;
            }
            }

            return ::DefMDIChildProc(hWnd, message, wParam, lParam);
        }

        static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)
            {
            case WM_NCCREATE:
            {
                CREATESTRUCTW* cs       = (CREATESTRUCTW*)lParam;
                MDICREATESTRUCT* mcs    = (MDICREATESTRUCT*)(cs->lpCreateParams);
                mdi_child* that          = (mdi_child*)(mcs->lParam);

                that->handle = hWnd;
                ::SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)that);
                // fallthru
            }
            default:
            {
                LONG_PTR l = ::GetWindowLongPtrW(hWnd, GWLP_USERDATA);
                if (!l)
                {
                    return DefMDIChildProc(hWnd, message, wParam, lParam);
                }

                mdi_child* that = (mdi_child*)l;
                return that->wndProc(hWnd, message, wParam, lParam);
            }
            }
            return 0;
        }

    protected:
        int viewMenuIndex_ = -1;
        
    private:

        virtual HWND create(size_t id, HWND parent, RECT& r, int style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, int exStyle = 0) override
        {
            return nullptr;
        }

        virtual HWND create(size_t id, const wchar_t* title, HWND parent, RECT& r, int style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, int exStyle = 0) override
        {
            return nullptr;
        }

    };

    HWND mdi_frame::create_document(std::wstring title, mdi_child* child, UINT styles , UINT exStyles )
    {
        RECT bounds = { CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 };

        auto& clazz = wc<mdi_child>();

        HWND wnd = ::CreateMDIWindow(
            clazz.name(), 
            title.c_str(), 
            styles, 
            bounds.left, bounds.top,
            bounds.right - bounds.left, bounds.bottom - bounds.top, 
            mdiClient_, 
            module_instance(), 
            (LPARAM)child
        );
        
        ::DrawMenuBar(this->handle);
        return wnd;

    }

    HWND mdi_child::create(
        const wchar_t* title, 
        int style,
        int exStyle, 
        HMENU menu, 
        HWND parent
    )
    {
        RECT bounds = { CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 };

        auto& clazz = wc<mdi_child>();

        HWND wnd = ::CreateMDIWindow(
            clazz.name(),
            title,
            style,
            bounds.left, bounds.top,
            bounds.right - bounds.left, bounds.bottom - bounds.top,
            parent,
            module_instance(),
            (LPARAM)this
        );

        ::DrawMenuBar(this->handle);
        return wnd;
    }

}
