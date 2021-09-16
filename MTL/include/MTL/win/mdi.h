#pragma once

#include "MTL/win/wind.h"
#include "MTL/win/layout.h"

namespace MTL {

    class MdiChild;

    namespace details {

        HMENU getViewSubMenu(HMENU menu, int pos = -1)
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

//	template<class W>
	class MdiFrame : public Window<MdiFrame>
	{
	public:

        HMENU menu = nullptr;

        DefaultLayout layout;

        MdiFrame(int windowMenuIndex = -1, UINT idFirstMdiChild = 50000)
        {
            windowMenuIndex_ = windowMenuIndex;
            ccs_.idFirstChild = idFirstMdiChild;
        }

        void maximize()
        {
            ::PostMessage(mdiClient_, WM_MDIMAXIMIZE, (WPARAM)getActive(), 0);
        }

        void minimize()
        {
            ::ShowWindow(getActive(), SW_MINIMIZE);
            ::UpdateWindow(getActive());
            next();
        }

        void restore()
        {
            ::PostMessage(mdiClient_, WM_MDIRESTORE, (WPARAM)getActive(), 0);
        }

        void next()
        {
            ::PostMessage(mdiClient_, WM_MDINEXT, 0, 0);
        }

        void cascade()
        {
            ::PostMessage(mdiClient_, WM_MDICASCADE, 0, 0);
        }

        void tileHorizontal()
        {
            ::PostMessage(mdiClient_, WM_MDITILE, 0, 0);
        }

        void tileVertical()
        {
            ::PostMessage(mdiClient_, WM_MDITILE, MDITILE_VERTICAL, 0);
        }

        void iconArrange()
        {
            ::PostMessage(mdiClient_, WM_MDIICONARRANGE, 0, 0);
        }

        HWND getActive()
        {
            return (HWND)(::SendMessage(mdiClient_, WM_MDIGETACTIVE, 0, 0));
        }

        void destroyChild(HWND wnd)
        {
            ::PostMessage(mdiClient_, WM_MDIDESTROY, (WPARAM)(wnd), 0);
        }

        void resetMenu()
        {
            HWND client = mdiClient();
            if (client && menu)
            {
                HMENU viewMenu = details::getViewSubMenu(menu, windowMenuIndex_);
                ::SendMessage(client, WM_MDISETMENU, (WPARAM)menu, (LPARAM)viewMenu);
                ::DrawMenuBar(handle);
            }
        }

        virtual LRESULT wmSize(RECT& clientRect) override
        {
            RECT r = { 0,0,0,0 };
            ::InvalidateRect(mdiClient(), 0, FALSE);
            layout.do_layout(clientRect, r);

            // do not do that here
            //::InvalidateRect(this->handle, 0, TRUE);
            return 0;
        }

        virtual LRESULT wmDraw(HDC dc, RECT& r) override
        {
            ::InvalidateRect(mdiClient(), 0, TRUE);
            ::UpdateWindow(mdiClient());
            return 0;
        }

        virtual LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            if (message == WM_LAYOUT)
            {
                RECT r;
                ::GetClientRect(hWnd, &r);
                this->wmLayout(r);
                return 0;
            }

            if (message == WM_SEARCH)
            {
                FINDREPLACE* fp = (FINDREPLACE*)lParam;
                this->wmSearch(fp);
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

                LRESULT r = this->wmCreate();

                if (r) return r;
                return 0;
                break;
            }
            case WM_DESTROY:
            {
                LRESULT r = this->wmDestroy();
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
                    r = this->wmCommand(wmId);
                }
                else if (code == 1)
                {
                    r = this->wmAccellerator(wmId);
                }
                else
                {
                    r = this->wmControl(message, wParam, lParam);
                }

                HWND active = this->getActive();
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
                return this->wmSize(bounds);
                break;
            }
            case WM_PAINT:
            {
                this->wmPaint();
                break;
            }
            case WM_ERASEBKGND:
            {
                return this->wmEraseBackground(wParam);
            }
            case WM_DPICHANGED:
            {
                ::InvalidateRect(hWnd, 0, TRUE);
                RECT* r = (RECT*)lParam;
                if (r)
                {
                    ::SetWindowPos(hWnd, NULL, r->left, r->top, r->right - r->left, r->bottom - r->top, 0);
                }
                LRESULT result = this->wmDpiChanged(r);
                ::UpdateWindow(hWnd);
                break;
            }
            case WM_DPICHANGED_BEFOREPARENT:
            {
                LRESULT result = this->wmDpiChanged(0);
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

        HWND mdiClient()
        {
            return mdiClient_;
        }
        
        template<class C, class ... Args>
        C& createMDIChild(std::wstring title, Args ... args)
        {
            C* c = new C(args...);
            HWND hWnd = createDocument(title, (MdiChild*)c);
            return *c;
        }
        
        HWND createDocument(std::wstring title, MdiChild* wnd, UINT styles = WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS |
            WS_CLIPCHILDREN | WS_THICKFRAME |
            WS_SYSMENU | WS_CAPTION | MDIS_ALLCHILDSTYLES|
            WS_MAXIMIZEBOX | WS_MINIMIZEBOX, UINT exStyles = WS_EX_MDICHILD);

	private:
        CLIENTCREATESTRUCT ccs_;
        HWND mdiClient_ = nullptr;
        int windowMenuIndex_ = -1;
	};


    class MdiChild : public Window<MdiChild>
    {
    public:

        Menu menu;

        MdiChild()
        {
            auto& wc = windowClass<MdiChild>();
            wc.lpfnWndProc = &MdiChild::WndProc;
        }

        MdiChild(int menuId, int viewMenuIndex = -1)
            : menu(menuId), viewMenuIndex_(viewMenuIndex)
        {
            auto& wc = windowClass<MdiChild>();
            wc.lpfnWndProc = &MdiChild::WndProc;
        }

        virtual LRESULT wmSize(RECT& clientRect) override
        {
            return 0;
        }

        virtual void destroy() override
        {
            if (handle)
            {
                MdiFrame* frame = unwrap<MdiFrame>(::GetParent(::GetParent(handle)));
                if (frame)
                {
                    frame->destroyChild(handle);
                }
            }
        }

        virtual LRESULT wmNcDestroy()
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
                this->wmLayout(r);
                return 0;
            }

            if (message == WM_SEARCH)
            {
                FINDREPLACE* fp = (FINDREPLACE*)lParam;
                this->wmSearch(fp);
                return 0;
            }

            switch (message)
            {
            case WM_CREATE:
            {
                LRESULT r = this->wmCreate();
                break;
            }
            case WM_DESTROY:
            {
                LRESULT r = this->wmDestroy();
                if (r) return r;
                break;
            }
            case WM_NCDESTROY:
            {
                LRESULT r = this->wmNcDestroy();
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
                    r = this->wmCommand(wmId);
                }
                else if (code == 1)
                {
                    r = this->wmAccellerator(wmId);
                }
                else
                {
                    r = this->wmControl(message, wParam, lParam);
                }

                break;
            }
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);

                RECT bounds;
                ::GetClientRect(hWnd, &bounds);
                this->wmSize(bounds);
                break;
            }
            case WM_PAINT:
            {
                this->wmPaint();
                break;
            }
            case WM_ERASEBKGND:
            {
                return this->wmEraseBackground(wParam);
            }
            case WM_DPICHANGED:
            {
                ::InvalidateRect(hWnd, 0, TRUE);
                RECT* r = (RECT*)lParam;
                if (r)
                {
                    ::SetWindowPos(hWnd, NULL, r->left, r->top, r->right - r->left, r->bottom - r->top, 0);
                }
                LRESULT result = this->wmDpiChanged(r);
                ::UpdateWindow(hWnd);
                break;
            }
            case WM_DPICHANGED_BEFOREPARENT:
            {
                LRESULT result = this->wmDpiChanged(0);
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
                        HMENU viewMenu = details::getViewSubMenu(*menu, viewMenuIndex_);
                        ::SendMessage(::GetParent(hWnd), WM_MDISETMENU, (WPARAM)*menu, (LPARAM)viewMenu);
                        ::DrawMenuBar(frameWnd);
                    }
                }
                else // deactivated
                {
                    MdiFrame* frame = unwrap<MdiFrame>(frameWnd);
                    if (frame)
                    {
                        frame->resetMenu();
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

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)
            {
            case WM_NCCREATE:
            {
                CREATESTRUCTW* cs       = (CREATESTRUCTW*)lParam;
                MDICREATESTRUCT* mcs    = (MDICREATESTRUCT*)(cs->lpCreateParams);
                MdiChild* that          = (MdiChild*)(mcs->lParam);

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

                MdiChild* that = (MdiChild*)l;
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

    HWND MdiFrame::createDocument(std::wstring title, MdiChild* child, UINT styles , UINT exStyles )
    {
        RECT bounds = { CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 };

        auto& wc = windowClass<MdiChild>();

        HWND wnd = ::CreateMDIWindow(
            wc.name(), 
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

    HWND MdiChild::create(
        const wchar_t* title, 
        int style,
        int exStyle, 
        HMENU menu, 
        HWND parent
    )
    {
        RECT bounds = { CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 };

        auto& wc = windowClass<MdiChild>();

        HWND wnd = ::CreateMDIWindow(
            wc.name(),
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
