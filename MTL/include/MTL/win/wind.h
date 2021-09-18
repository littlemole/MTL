#pragma once

#include "mtl/win/wc.h"
#include "mtl/win32/uni.h"

#include <commdlg.h>
#include <commctrl.h>

#include <set>

namespace mtl {

    inline UINT WmLayout()
    {
        static UINT wmLayout = ::RegisterWindowMessage(L"mwin_WM_LAYOUT");
        return wmLayout;
    }

#define WM_LAYOUT ::mtl::WmLayout()

    inline UINT WmReflect()
    {
        static UINT wmReflect = ::RegisterWindowMessage(L"mwin_WM_REFLECT");
        return wmReflect;
    }

#define WM_REFLECT ::mtl::WmReflect()

    inline UINT WmSearch()
    {
        static UINT wmSearch = ::RegisterWindowMessage(FINDMSGSTRING);
        return wmSearch;
    }

#define WM_SEARCH  ::mtl::WmSearch()

    class color_theme
    {
    protected:

        bool                            enabled_ = true;
        HFONT   						font_;
        COLORREF						textColor_ = RGB(0x35, 0x39, 0x44);
        COLORREF						bkgColor_ = RGB(0x55, 0x59, 0x64);
        COLORREF						selectedTextColor_ = RGB(0xEA, 0xEA, 0xEA);
        COLORREF						selectedBkgColor_ = RGB(0xEA, 0xEA, 0xEA);
        int                             padding_ = 6;

    public:

        event<void()> onUpdate;

        color_theme(HFONT hFont, COLORREF text, COLORREF bkg, COLORREF selText, COLORREF selBkg)
            : font_(hFont),
            textColor_(text),
            bkgColor_(bkg),
            selectedTextColor_(selText),
            selectedBkgColor_(selBkg)
        {
            textBrush_ = ::CreateSolidBrush(textColor_);
            bkgBrush_ = ::CreateSolidBrush(bkgColor_);
            selectedTextBrush_ = ::CreateSolidBrush(selectedTextColor_);
            selectedBkgBrush_ = ::CreateSolidBrush(selectedBkgColor_);
        }

        bool enabled() { return enabled_; }
        HFONT font() { return font_; }
        COLORREF text_color() { return textColor_; }
        COLORREF bkg_color() { return bkgColor_; }
        COLORREF selected_text_color() { return selectedTextColor_; }
        COLORREF selected_bkg_color() { return selectedBkgColor_; }
        int padding() { return padding_; }

        color_theme& enabled(bool e)
        {
            enabled_ = e;
            return *this;
        }

        color_theme& font(HFONT f)
        {
            font_ = f;
            return *this;
        }

        color_theme& text_color(COLORREF c)
        {
            textColor_ = c;
            ::DeleteObject(textBrush_);
            textBrush_ = ::CreateSolidBrush(textColor_);
            return *this;
        }

        color_theme& bkg_color( COLORREF c)
        {
            bkgColor_ = c;
            ::DeleteObject(bkgBrush_);
            bkgBrush_ = ::CreateSolidBrush(bkgColor_);
            return *this;
        }

        color_theme& selected_text_color(COLORREF c)
        {
            selectedTextColor_ = c;
            ::DeleteObject(selectedTextBrush_);
            selectedTextBrush_ = ::CreateSolidBrush(selectedTextColor_);
            return *this;
        }

        color_theme& selected_bkg_color(COLORREF c)
        {
            selectedBkgColor_ = c;
            ::DeleteObject(selectedBkgBrush_);
            selectedBkgBrush_ = ::CreateSolidBrush(selectedBkgColor_);
            return *this;
        }

        color_theme& padding(int p)
        {
            padding_ = p;
            return *this;
        }

        ~color_theme()
        {
            free();
        }

        void update()
        {
            onUpdate.fire();
        }

        HBRUSH text_brush() { return textBrush_; }
        HBRUSH bkg_brush() { return bkgBrush_; }
        HBRUSH selected_text_brush() { return selectedTextBrush_; }
        HBRUSH selected_bkg_brush() { return selectedBkgBrush_; }

    private:

        HBRUSH textBrush_;
        HBRUSH bkgBrush_;
        HBRUSH selectedTextBrush_;
        HBRUSH selectedBkgBrush_;

        void free()
        {
            ::DeleteObject(textBrush_);
            ::DeleteObject(bkgBrush_);
            ::DeleteObject(selectedTextBrush_);
            ::DeleteObject(selectedBkgBrush_);
        }
    };

    class modeless_dialogs
    {
    public:

        void add(HWND hWnd)
        {
            dialogs_.insert(hWnd);
        }

        void remove(HWND hWnd)
        {
            dialogs_.erase(hWnd);
        }

        BOOL is_dialog_message(MSG& msg)
        {
            for (HWND hWnd : dialogs_)
            {
                BOOL r = ::IsDialogMessage(hWnd, &msg);
                if (r)
                {
                    return TRUE;
                }
            }
            return FALSE;
        }
    private:
        std::set<HWND> dialogs_;
    };

    inline modeless_dialogs& modeless_dlg()
    {
        static modeless_dialogs md;
        return md;
    }

    class wnd
    {
    public:

        HWND handle = nullptr;

        virtual ~wnd()
        {
            if (handle && ::IsWindow(handle))
            {
                ::SetWindowLongPtrW(handle, GWLP_USERDATA, (LONG_PTR)0);
            }
            if (!colorThemeToken_.empty() && colorTheme)
            {
                colorTheme->onUpdate.unregister(colorThemeToken_);
                colorThemeToken_ = L"";
            }
        }

        void set_color_theme(std::shared_ptr<color_theme>& ct)
        {
            if (ct && (ct.get() != colorTheme.get()))
            {
                if (!colorThemeToken_.empty() && colorTheme)
                {
                    colorTheme->onUpdate.unregister(colorThemeToken_);
                    colorThemeToken_ = L"";
                }
                colorThemeToken_ = ct->onUpdate([this]() { this->on_color_theme_changed(); });
            }

            colorTheme = ct;
            on_color_theme_changed();
        }

        void no_color_theme()
        {
            colorTheme.reset();
            on_color_theme_changed();
        }

        HWND operator *() const
        {
            return handle;
        }

        RECT client_rect() const
        {
            RECT r;
            ::GetClientRect(handle, &r);
            return r;
        }

        RECT window_rect() const
        {
            RECT r;
            ::GetWindowRect(handle, &r);
            return r;
        }

        virtual wnd& update() 
        {
            ::UpdateWindow(handle);

            RECT wr = window_rect();
            wr.left += 1;
            move(wr);
            wr.left -= 1;
            move(wr);

            send_msg(WM_LAYOUT, 0, 0);

            return *this;
        }

        virtual wnd& invalidate(RECT* r = 0, bool redraw = false) 
        {
            ::InvalidateRect(handle, r, redraw);
            return *this;
        }

        virtual wnd& show(UINT how = SW_SHOW) 
        {
            ::ShowWindow(handle, how);
            return *this;
        }

        virtual HWND parent() const
        {
            return ::GetParent(handle);
        }

        virtual wnd& set_menu(HMENU menu) 
        {
            ::SetMenu(handle,menu);
            return *this;
        }

        virtual HMENU get_menu() const
        {
            return ::GetMenu(handle);
        }

        virtual wnd& set_font(HFONT font, bool repaint = false)
        {
            send_msg(WM_SETFONT, (WPARAM)font, MAKELPARAM(repaint, 0));
            return *this;
        }

        virtual LONG style() const
        {
            return ::GetWindowLong(handle, GWL_STYLE);
        }

        virtual LONG ex_style() const
        {
            return ::GetWindowLong(handle, GWL_EXSTYLE);
        }

        virtual wnd& relayout()
        {
            send_msg(WM_LAYOUT, 0, 0);
            return *this;
        }

        wnd& move(int x, int y)
        {
            ::SetWindowPos(handle, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
            return *this;
        }

        wnd& move(int x, int y, int w, int h)
        {
            ::SetWindowPos(handle, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
            return *this;
        }

        wnd& move(const RECT& dest)
        {
            ::SetWindowPos(handle, NULL, dest.left, dest.top, dest.right - dest.left, dest.bottom - dest.top, SWP_NOZORDER | SWP_NOACTIVATE);
            return *this;
        }

        LRESULT send_msg(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0)
        {
            return ::SendMessage(handle, msg, wParam, lParam);
        }

        LRESULT post_msg(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0)
        {
            return ::PostMessage(handle, msg, wParam, lParam);
        }

        wnd& set_text(const std::wstring& title)
        {
            ::SetWindowText(handle, title.c_str());
            return *this;
        }

        std::wstring get_text()
        {
            int n = GetWindowTextLength(handle);
            wchar_t* buf = new wchar_t[n];
            n = ::GetWindowText(handle, buf, n);

            std::wstring result(buf, n);
            delete[] buf;
            return result;
        }

        HICON get_icon(int type = 0)
        {
            return (HICON)send_msg(WM_GETICON, type, 96);
        }

        HICON set_icon(HICON icon, int type = 0)
        {
            return (HICON)send_msg(WM_SETICON, type, (LPARAM)icon);
        }

        HWND dialog_item(int id)
        {
            return ::GetDlgItem(handle, id);
        }

        int dialog_item_int(int id, BOOL signedInt = true)
        {
            return ::GetDlgItemInt(handle, id, NULL, signedInt);
        }

        std::wstring dialog_item_text(int id)
        {
            wchar_t buf[2048];
            UINT n = ::GetDlgItemText(handle, id, buf, 2048);
            return std::wstring(buf, n);
        }

        virtual HWND create(const wchar_t* title, int style = WS_OVERLAPPEDWINDOW, int exStyle = 0, HMENU menu = nullptr, HWND parent = nullptr)
        {
            RECT r = { CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 };
            return create_window(title, parent, r, style, exStyle, menu);
        }

        virtual HWND create(size_t id, HWND parent, RECT& r, int style = WS_CHILD | WS_VISIBLE  | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, int exStyle = 0)
        {
            return create_window(L"", parent, r, style, exStyle, (HMENU)id);
        }

        virtual HWND create(size_t id, const wchar_t* title, HWND parent, RECT& r, int style = WS_CHILD | WS_VISIBLE  | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, int exStyle = 0)
        {
            return create_window(title, parent, r, style, exStyle, (HMENU)id);
        }

        virtual void destroy()
        {
            if (handle)
            {
                ::DestroyWindow(handle);
                handle = nullptr;
            }
        }

        virtual LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)
            {
            case WM_DESTROY:
            {
                LRESULT r = wm_destroy();
                if (!colorThemeToken_.empty() && colorTheme)
                {
                    colorTheme->onUpdate.unregister(colorThemeToken_);
                    colorThemeToken_ = L"";
                }
                return r;
                break;
            }
            case WM_CTLCOLORLISTBOX:
            case WM_CTLCOLORSTATIC:
            case WM_CTLCOLORBTN:
            case WM_CTLCOLORDLG:
            case WM_CTLCOLORMSGBOX:
            case WM_CTLCOLOREDIT:
            {
                if (colorTheme && colorTheme->enabled())
                {
                    return wm_ctl_color((HDC)wParam, (HWND)lParam);
                }
                return ::DefWindowProc(hWnd, message, wParam, lParam);
                break;
            }
            default:
            {
                return ::DefWindowProc(hWnd, message, wParam, lParam);
            }
            }
            return 0;
        }

    protected:

        virtual void on_color_theme_changed()
        {
            if (colorTheme && colorTheme->enabled() && colorTheme->font())
            {
                set_font(colorTheme->font());
            }
        }

        virtual LRESULT wm_create()
        {
            return 0;
        }

        virtual LRESULT wm_destroy()
        {
            return 0;
        }

        virtual LRESULT wm_nc_destroy()
        {
            return 0;
        }

        virtual LRESULT wm_size(RECT& clientRect)
        {
            return 0;
        }


        virtual LRESULT wm_ctl_color(HDC hdc, HWND ctrl) 
        {
            ::SetBkColor(hdc,colorTheme->bkg_color());
            ::SetTextColor(hdc, colorTheme->text_color());
            //if (colorTheme->font) ::SelectObject(hdc, colorTheme->font);
            return (LRESULT)colorTheme->bkg_brush();
        }


        virtual HWND create_window(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) = 0;

        std::shared_ptr<color_theme> colorTheme;
        std::wstring colorThemeToken_;
    };



    class owner_drawn
    {
    public:

        virtual ~owner_drawn() {}

        virtual LRESULT wm_draw_item(LPDRAWITEMSTRUCT dis) = 0;
        virtual LRESULT wm_measure_item(MEASUREITEMSTRUCT * mis) = 0;
    };


    template<class W>
    class window : public wnd
    {
    public:

        HWND operator*() const
        {
            return handle;
        }

        window()
        {}

        virtual LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            if(message == WM_LAYOUT)
            {
                RECT r = this->client_rect();
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
                 this->wm_create();
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

                if (r == 0) return 0;
                return ::DefWindowProc(hWnd, message, wParam, lParam);
                break;
            }
            case WM_NOTIFY:
            {
                int id = (int) wParam;
                NMHDR* nmhdr = (NMHDR*)(lParam);
                unsigned int code = nmhdr->code;
                int wmId = (int) nmhdr->idFrom;

                if (code == RBN_HEIGHTCHANGE)
                {
                    ::PostMessage(hWnd, WM_LAYOUT, 0, 0);
                    return 0;
                }

                MSG msg;
                msg.hwnd = nmhdr->hwndFrom;
                msg.message = message;
                msg.wParam = wParam;
                msg.lParam = lParam;
                return ::SendMessage(nmhdr->hwndFrom, WM_REFLECT, (WPARAM)&msg, 0);
                break;

            }
            case WM_DRAWITEM:
            {
                LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)lParam;
                
                if ((wParam == 0) && (lpDIS->CtlType == ODT_MENU)) // menu
                {
                    owner_drawn* ownerDrawn = (owner_drawn*)(lpDIS->itemData);
                    return ownerDrawn->wm_draw_item(lpDIS);
                }
                
                MSG msg;
                msg.hwnd = lpDIS->hwndItem;
                msg.message = message;
                msg.wParam = wParam;
                msg.lParam = lParam;
                return ::SendMessage(msg.hwnd, WM_REFLECT, (WPARAM)&msg, 0);
            }
            case WM_MEASUREITEM:
            {
                MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;

                if ((wParam == 0) && (mis->CtlType == ODT_MENU)) // menu
                {
                    owner_drawn* ownerDrawn = (owner_drawn*)(mis->itemData);
                    return ownerDrawn->wm_measure_item(mis);
                }
                return wnd::wndProc(hWnd, message, wParam, lParam);
                break;
            }
            case WM_ACTIVATE :
            {
                RECT bounds = this->client_rect();
                this->wm_size(bounds);
                return 0;
            }
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);

                RECT bounds = this->client_rect();
                return this->wm_size(bounds);
                break;
            }
            case WM_PAINT:
            {
                return this->wm_paint();
                break;
            }
            case WM_ERASEBKGND:
            {
                return this->wm_erase_background(wParam);
            }
            case WM_CLOSE:
            {
                this->wm_close();
                return 0;
            }
            case WM_DESTROY:
            {
                return this->wm_destroy();
                break;
            }
            case WM_NCDESTROY:
            {
                return this->wm_nc_destroy();
                break;
            }
            case WM_DPICHANGED:
            {
                RECT* r = (RECT*)lParam;
                if (r)
                {
                    ::SetWindowPos(this->handle, NULL, r->left, r->top, r->right - r->left, r->bottom - r->top, 0);
                }
                LRESULT result = this->wm_dpi_changed(r);
                ::InvalidateRect(this->handle, 0, TRUE);
                ::UpdateWindow(this->handle);
                return result;
                break;
            }
            case WM_DPICHANGED_BEFOREPARENT:
            {
                LRESULT result = this->wm_dpi_changed(0);
                ::InvalidateRect(this->handle, 0, TRUE);
                ::UpdateWindow(this->handle);
                return result;
                break;
            }

            default:
            {
                return wnd::wndProc(hWnd, message, wParam, lParam);
            }
            }
            return 0;
        }

    protected:

        virtual HWND create_window(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) override
        {
            auto& wc = windowclass<W>();

            handle = ::CreateWindowEx(
                exStyle,
                wc.name(),
                title,
                style,
                r.left, r.top, r.right - r.left, r.bottom - r.top,
                parent,
                menu,
                module_instance(),
                (LPVOID)(W*)this
            );
            return handle;
        }

        virtual LRESULT wm_command(int id)
        {
            return -1;
        }

        virtual LRESULT wm_accellerator(int id)
        {
            return wm_command(id);
        }

        virtual LRESULT wm_control(UINT message, WPARAM wParam, LPARAM lParam)
        {
            MSG msg;
            msg.hwnd = (HWND)lParam;
            msg.message = message;
            msg.wParam = wParam;
            msg.lParam = lParam;
            return ::SendMessage((HWND)lParam, WM_REFLECT, (WPARAM)&msg, 0);
        }

        virtual LRESULT wm_size(RECT& clientRect)
        {
            return 0;
        }

        virtual LRESULT wm_paint()
        {
            PAINTSTRUCT ps;
            HDC hdc = ::BeginPaint(handle, &ps);
            RECT r;
            ::GetClientRect(this->handle, &r);

            HDC hdcMem = ::CreateCompatibleDC(hdc);
            HBITMAP hbmMem = (HBITMAP)::CreateCompatibleBitmap(hdc, r.right - r.left, r.bottom - r.top);
            HBITMAP hOld = (HBITMAP)::SelectObject(hdcMem, hbmMem);

            HBRUSH br = (HBRUSH)::GetStockObject(WHITE_BRUSH);
            ::FillRect(hdcMem, &r, br);
            LRESULT res = wm_draw(hdcMem, r);

            ::BitBlt(hdc, 0, 0, r.right - r.left, r.bottom - r.top, hdcMem, 0, 0, SRCCOPY);

            // Free-up the off-screen DC
            ::SelectObject(hdcMem, hOld);
            ::DeleteObject(hbmMem);
            ::DeleteDC(hdcMem);
            ::EndPaint(handle, &ps);
            return S_OK;
        }

        virtual LRESULT wm_draw(HDC hdc, RECT& bounds)
        {
            return 0;
        }

        virtual LRESULT wm_close()
        {
            destroy();
            return 0;
        }

        virtual LRESULT wm_create() override
        {
            return 0;
        }


        virtual LRESULT wm_destroy() override
        {
            return 0;
        }

        virtual LRESULT wm_nc_destroy() override
        {
            return 0;
        }

        virtual LRESULT wm_layout(RECT& r)
        {
            return wm_size(r);
        }

        virtual LRESULT wm_erase_background(WPARAM wParam)
        {
            return ::DefWindowProc(handle, WM_ERASEBKGND, wParam, NULL);
        }

        virtual LRESULT wm_dpi_changed(RECT* r)
        {
            return 0;
        }

        virtual LRESULT wm_search(FINDREPLACE* fr)
        {
            return 0;
        }


    };

    template<class T>
    class scroll_wnd : public window<T>
    {
    public:

        scroll_wnd()
        {
            ::ZeroMemory(&horizontalScrollInfo_, sizeof(SCROLLINFO));
            horizontalScrollInfo_.cbSize = sizeof(horizontalScrollInfo_);
            horizontalScrollInfo_.fMask = SIF_ALL;

            ::ZeroMemory(&verticalScrollInfo_, sizeof(SCROLLINFO));
            verticalScrollInfo_.cbSize = sizeof(verticalScrollInfo_);
            verticalScrollInfo_.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
        }

        void set_v_scroll(int rowHeight, unsigned int nRows)
        {
            scrollRowHeight_ = rowHeight;
            scrollRows_ = nRows;

            RECT r = this->client_rect();
            scroll_v(r.bottom);
        }

        void set_h_scroll(int colWidth, unsigned int nCols)
        {
            scrollColWidth_ = colWidth;
            scrollCols_ = nCols;

            RECT r = this->client_rect();

            scroll_h(r.right);
        }

        unsigned int  get_scroll_pos(int sb = SB_VERT)
        {
            if (sb == SB_HORZ)
            {
                ::GetScrollInfo(this->handle, sb, &horizontalScrollInfo_);
                return horizontalScrollInfo_.nPos;
            }

            ::GetScrollInfo(this->handle, sb, &verticalScrollInfo_);
            return verticalScrollInfo_.nPos;
        }

        unsigned int  get_scroll_max(int sb = SB_VERT)
        {
            if (sb == SB_HORZ)
            {
                ::GetScrollInfo(this->handle, sb, &horizontalScrollInfo_);
                return horizontalScrollInfo_.nMax;
            }

            ::GetScrollInfo(this->handle, sb, &verticalScrollInfo_);
            return verticalScrollInfo_.nMax;
        }

        void set_scroll_pos(unsigned int p, int sb = SB_VERT)
        {
            RECT r = this->client_rect();

            if (sb == SB_HORZ)
            {
                int xPos = get_scroll_pos(SB_HORZ);
                if (xPos == p)
                    return;

                horizontalScrollInfo_.nPos = p;
                ::SetScrollInfo(this->handle, sb, &horizontalScrollInfo_, TRUE);
                ::GetScrollInfo(this->handle, sb, &horizontalScrollInfo_);

                int width = r.right / horizontalScrollInfo_.nPage;
                ::ScrollWindow(this->handle, 0, width * (xPos - horizontalScrollInfo_.nPos), NULL, NULL);
            }
            else
            {
                int yPos = get_scroll_pos();
                if (yPos == p)
                    return;

                //siV_.fMask  = SIF_ALL|SIF_DISABLENOSCROLL;
                verticalScrollInfo_.nPos = p;
                ::SetScrollInfo(this->handle, sb, &verticalScrollInfo_, TRUE);
                ::GetScrollInfo(this->handle, sb, &verticalScrollInfo_);

                int height = r.bottom / verticalScrollInfo_.nPage;
                ::ScrollWindow(this->handle, 0, height * (yPos - verticalScrollInfo_.nPos), NULL, NULL);
            }

            ::UpdateWindow(this->handle);
        }

        LRESULT virtual wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            static UINT wheelScrollLines = 0;//number of lines to scroll 
            static BOOL b = ::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &wheelScrollLines, 0);

            switch (message)
            {
            case WM_VSCROLL:
            {
                // Save the posi_tion for comparison later on
                int yPos = get_scroll_pos(SB_VERT);
                switch (LOWORD(wParam))
                {
                    // user clicked the HOME keyboard key
                case SB_TOP:
                    verticalScrollInfo_.nPos = verticalScrollInfo_.nMin;
                    break;

                    // user clicked the END keyboard key
                case SB_BOTTOM:
                    verticalScrollInfo_.nPos = verticalScrollInfo_.nMax;
                    break;

                    // user clicked the top arrow
                case SB_LINEUP:
                {
                    verticalScrollInfo_.nPos -= 1;
                    break;
                }

                // user clicked the bottom arrow
                case SB_LINEDOWN:
                {
                    verticalScrollInfo_.nPos += 1;
                    break;
                }

                // user clicked the scroll bar shaft above the scroll box
                case SB_PAGEUP:
                    verticalScrollInfo_.nPos -= verticalScrollInfo_.nPage;
                    break;

                    // user clicked the scroll bar shaft below the scroll box
                case SB_PAGEDOWN:
                    verticalScrollInfo_.nPos += verticalScrollInfo_.nPage;
                    break;

                    // user dragged the scroll box
                case SB_THUMBTRACK:
                    verticalScrollInfo_.nPos = verticalScrollInfo_.nTrackPos;
                    break;

                default:
                    break;
                }
                // Set the posi_tion and then retrieve it.  Due to adjustments
                //   by Windows it may not be the same as the value set.
                //si_.fMask = SIF_ALL ;
                //siV_.fMask = SIF_ALL;
                ::SetScrollInfo(this->handle, SB_VERT, &verticalScrollInfo_, TRUE);
                ::GetScrollInfo(this->handle, SB_VERT, &verticalScrollInfo_);

                // If the position has changed, scroll window and update it
                if (verticalScrollInfo_.nPos != yPos)
                {
                    RECT r = this->client_rect();
                    int height = r.bottom / verticalScrollInfo_.nPage;

                    ::ScrollWindow(this->handle, 0, height * (yPos - verticalScrollInfo_.nPos), NULL, NULL);
                    ::UpdateWindow(this->handle);
                    this->wmVScroll(verticalScrollInfo_.nPos);
                }
                return 0;
            }
            case WM_HSCROLL:
            {
                int xPos = get_scroll_pos(SB_HORZ);
                switch (LOWORD(wParam))
                {
                    // user clicked the HOME keyboard key
                case SB_TOP:
                    horizontalScrollInfo_.nPos = horizontalScrollInfo_.nMin;
                    break;

                    // user clicked the END keyboard key
                case SB_BOTTOM:
                    horizontalScrollInfo_.nPos = horizontalScrollInfo_.nMax;
                    break;

                    // user clicked the top arrow
                case SB_LINEUP:
                    horizontalScrollInfo_.nPos -= 1;
                    break;

                    // user clicked the bottom arrow
                case SB_LINEDOWN:
                    horizontalScrollInfo_.nPos += 1;
                    break;

                    // user clicked the scroll bar shaft above the scroll box
                case SB_PAGEUP:
                    horizontalScrollInfo_.nPos -= horizontalScrollInfo_.nPage;
                    break;

                    // user clicked the scroll bar shaft below the scroll box
                case SB_PAGEDOWN:
                    horizontalScrollInfo_.nPos += horizontalScrollInfo_.nPage;
                    break;

                    // user dragged the scroll box
                case SB_THUMBTRACK:
                    horizontalScrollInfo_.nPos = horizontalScrollInfo_.nTrackPos;
                    break;

                default:
                    break;
                }
                // Set the posi_tion and then retrieve it.  Due to adjustments
                //   by Windows it may not be the same as the value set.
                //si_.fMask = SIF_POS;
                //siH_.fMask = SIF_ALL;
                //siH_.fMask  = SIF_ALL|SIF_DISABLENOSCROLL;
                ::SetScrollInfo(this->handle, SB_HORZ, &horizontalScrollInfo_, TRUE);
                ::GetScrollInfo(this->handle, SB_HORZ, &horizontalScrollInfo_);
                // If the position has changed, scroll window and update it
                if (horizontalScrollInfo_.nPos != xPos)
                {
                    RECT r = this->client_rect();
                    int width = r.right / horizontalScrollInfo_.nPage;

                    ::ScrollWindow(this->handle, width * (xPos - horizontalScrollInfo_.nPos), 0, NULL, NULL);
                    ::UpdateWindow(this->handle);
                    this->wmHScroll(horizontalScrollInfo_.nPos);
                }
                return 0;
            }
            case WM_MOUSEWHEEL:
            {
                if (wParam & (MK_SHIFT | MK_CONTROL))
                {
                    break;
                }
                wheelDelta_ -= (short)HIWORD(wParam);
                if (abs(wheelDelta_) >= WHEEL_DELTA && wheelScrollLines > 0)
                {
                    int cLineScroll = (int)std::min((UINT)scrollRows_ - 1, wheelScrollLines);
                    if (cLineScroll == 0)
                        cLineScroll++;

                    cLineScroll *= (wheelDelta_ / WHEEL_DELTA);

                    wheelDelta_ = wheelDelta_ % WHEEL_DELTA;

                    set_scroll_pos(get_scroll_pos() + cLineScroll);
                    //invalidateRect(0,TRUE);
                }
                break;
            }
            case WM_SIZE:
            {
                scroll_v(HIWORD(lParam));
                scroll_h(LOWORD(lParam));
                ::InvalidateRect(this->handle,0, FALSE);
                return window<T>::wndProc(hWnd, message, wParam, lParam);
            }
            } // end switch
            return window<T>::wndProc(hWnd, message, wParam, lParam);
        }

        void virtual wm_v_scroll(unsigned int pos) {};
        void virtual wm_h_scroll(unsigned int pos) {};

    protected:

        SCROLLINFO verticalScrollInfo_;
        SCROLLINFO horizontalScrollInfo_;
        int scrollRowHeight_;
        int scrollColWidth_;
        unsigned int scrollRows_;
        unsigned int scrollCols_;
        short wheelDelta_;

        void scroll_v(unsigned int h)
        {
            verticalScrollInfo_.nMax = scrollRows_;//max
            verticalScrollInfo_.nMin = 0;
            verticalScrollInfo_.nPage = 1;
            if (scrollRowHeight_ > 0)
                verticalScrollInfo_.nPage = h / scrollRowHeight_;//pagesize
            verticalScrollInfo_.nPos = 0;

            ::SetScrollInfo(this->handle, SB_VERT, &verticalScrollInfo_, TRUE);
            ::GetScrollInfo(this->handle, SB_VERT, &verticalScrollInfo_);
        }

        void scroll_h(unsigned int w)
        {
            horizontalScrollInfo_.nMax = scrollCols_;//max
            horizontalScrollInfo_.nMin = 0;
            horizontalScrollInfo_.nPage = 1;
            if (scrollColWidth_ > 0)
                horizontalScrollInfo_.nPage = w / scrollColWidth_;//pagesize
            horizontalScrollInfo_.nPos = 0;

            ::SetScrollInfo(this->handle, SB_HORZ, &horizontalScrollInfo_, TRUE);
            ::GetScrollInfo(this->handle, SB_HORZ, &horizontalScrollInfo_);
        }

    };

    template<class W>
    W* unwrap(HWND hWnd)
    {
        LONG_PTR l = ::GetWindowLongPtrW(hWnd, GWLP_USERDATA);
        if (!l) return nullptr;

        return reinterpret_cast<W*>(l);
    }

    class timer
    {
    public:

        UINT_PTR id = 0;

        timer()
        {}

        ~timer()
        {
            cancel();
        }

        timer(const timer& rhs) = delete;
        timer& operator=(const timer& rhs) = delete;

        timer(timer&& rhs) noexcept
            : id(rhs.id)
        {
            rhs.id = 0;
        }

        timer& operator = (timer&& rhs) noexcept
        {
            if (id == rhs.id)
            {
                return *this;
            }
            id = rhs.id;
            rhs.id = 0;
            return *this;
        }

        timer(int milisecs, std::function<void()> cb)
        {
            timeout(milisecs, cb);
        }

        UINT_PTR timeout(int milisecs, std::function<void()> cb)
        {
            cancel();
            id = ::SetTimer(nullptr, 0, milisecs, &timer::timerProc);
            timers()[id] = cb;
            return id;
        }

        void cancel()
        {
            if (id)
            {
                if (timers().count(id))
                {
                    timers().erase(id);
                }
                ::KillTimer(nullptr, id);
                id = 0;
            }
        }

    private:

        static void timerProc(
            HWND,
            UINT,
            UINT_PTR id,
            DWORD
        )
        {
            ::KillTimer(nullptr, id);
            if (timers().count(id) == 0)
            {
                return;
            }
            timers()[id]();
            timers().erase(id);
        }


        static std::map<UINT_PTR, std::function<void()>>& timers()
        {
            static std::map<UINT_PTR, std::function<void()>> timermap;
            return timermap;
        }
    };

    inline HWND find_child(HWND parent, const std::wstring& s)
    {
        struct Enumerator 
        {
            HWND result = nullptr;
            std::wstring className;

            static BOOL __stdcall proc(HWND hWnd, LPARAM lParam)
            {
                Enumerator* enumerator = (Enumerator*)lParam;
                mtl::wbuff buf(1024);
                ::GetClassName(hWnd, buf, (int) buf.size());
                if (enumerator->className == buf.toString())
                {
                    enumerator->result = hWnd;
                    return FALSE;
                }
                return TRUE;
            }
        };
        Enumerator enumerator;
        enumerator.className = s;
        ::EnumChildWindows(parent, &Enumerator::proc, (LPARAM)&enumerator);
        return enumerator.result;
    }


    inline RECT work_area(HWND wnd)
    {
        HMONITOR monitor = ::MonitorFromWindow(wnd, 0);
        MONITORINFO moni;
        moni.cbSize = sizeof(moni);
        ::GetMonitorInfo(monitor, &moni);
        return moni.rcWork;
    }
}
