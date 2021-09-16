#pragma once

#include "MTL/win/wc.h"
#include "MTL/win32/uni.h"
#include <set>
#include <commdlg.h>
#include <commctrl.h>

namespace MTL {

    inline UINT WmLayout()
    {
        static UINT wmLayout = ::RegisterWindowMessage(L"mwin_WM_LAYOUT");
        return wmLayout;
    }

#define WM_LAYOUT ::MTL::WmLayout()

    inline UINT WmReflect()
    {
        static UINT wmReflect = ::RegisterWindowMessage(L"mwin_WM_REFLECT");
        return wmReflect;
    }

#define WM_REFLECT ::MTL::WmReflect()

    inline UINT WmSearch()
    {
        static UINT wmSearch = ::RegisterWindowMessage(FINDMSGSTRING);
        return wmSearch;
    }

#define WM_SEARCH  ::MTL::WmSearch()

    class ColorTheme
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

        MTL::Event<void()> onUpdate;

        ColorTheme(HFONT hFont, COLORREF text, COLORREF bkg, COLORREF selText, COLORREF selBkg)
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
        COLORREF textColor() { return textColor_; }
        COLORREF bkgColor() { return bkgColor_; }
        COLORREF selectedTextColor() { return selectedTextColor_; }
        COLORREF selectedBkgColor() { return selectedBkgColor_; }
        int padding() { return padding_; }

        ColorTheme& enabled(bool e)
        {
            enabled_ = e;
            return *this;
        }

        ColorTheme& font(HFONT f)
        {
            font_ = f;
            return *this;
        }

        ColorTheme& textColor(COLORREF c)
        {
            textColor_ = c;
            ::DeleteObject(textBrush_);
            textBrush_ = ::CreateSolidBrush(textColor_);
            return *this;
        }

        ColorTheme& bkgColor( COLORREF c)
        {
            bkgColor_ = c;
            ::DeleteObject(bkgBrush_);
            bkgBrush_ = ::CreateSolidBrush(bkgColor_);
            return *this;
        }

        ColorTheme& selectedTextColor(COLORREF c)
        {
            selectedTextColor_ = c;
            ::DeleteObject(selectedTextBrush_);
            selectedTextBrush_ = ::CreateSolidBrush(selectedTextColor_);
            return *this;
        }

        ColorTheme& selectedBkgColor(COLORREF c)
        {
            selectedBkgColor_ = c;
            ::DeleteObject(selectedBkgBrush_);
            selectedBkgBrush_ = ::CreateSolidBrush(selectedBkgColor_);
            return *this;
        }

        ColorTheme& padding(int p)
        {
            padding_ = p;
            return *this;
        }

        ~ColorTheme()
        {
            free();
        }

        void update()
        {
            onUpdate.fire();
        }

        HBRUSH textBrush() { return textBrush_; }
        HBRUSH bkgBrush() { return bkgBrush_; }
        HBRUSH selectedTextBrush() { return selectedTextBrush_; }
        HBRUSH selectedBkgBrush() { return selectedBkgBrush_; }

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

    class ModelessDialogs
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

        BOOL isDialogMessage(MSG& msg)
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

    inline ModelessDialogs& modelessDialogs()
    {
        static ModelessDialogs md;
        return md;
    }

    class Wnd
    {
    public:

        HWND handle = nullptr;

        virtual ~Wnd()
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

        void setColorTheme(std::shared_ptr<ColorTheme>& ct)
        {
            if (ct && (ct.get() != colorTheme.get()))
            {
                if (!colorThemeToken_.empty() && colorTheme)
                {
                    colorTheme->onUpdate.unregister(colorThemeToken_);
                    colorThemeToken_ = L"";
                }
                colorThemeToken_ = ct->onUpdate([this]() { this->onColorThemeChanged(); });
            }

            colorTheme = ct;
            onColorThemeChanged();
        }

        void noColorTheme()
        {
            colorTheme.reset();
            onColorThemeChanged();
        }

        HWND operator *() const
        {
            return handle;
        }

        RECT getClientRect() const
        {
            RECT r;
            ::GetClientRect(handle, &r);
            return r;
        }

        RECT getWindowRect() const
        {
            RECT r;
            ::GetWindowRect(handle, &r);
            return r;
        }

        virtual Wnd& update() 
        {
            ::UpdateWindow(handle);

            RECT wr = getWindowRect();
            wr.left += 1;
            move(wr);
            wr.left -= 1;
            move(wr);

            sendMsg(WM_LAYOUT, 0, 0);

            return *this;
        }

        virtual Wnd& invalidate(RECT* r = 0, bool redraw = false) 
        {
            ::InvalidateRect(handle, r, redraw);
            return *this;
        }

        virtual Wnd& show(UINT how = SW_SHOW) 
        {
            ::ShowWindow(handle, how);
            return *this;
        }

        virtual HWND getParent() const
        {
            return ::GetParent(handle);
        }

        virtual Wnd& setMenu(HMENU menu) 
        {
            ::SetMenu(handle,menu);
            return *this;
        }

        virtual HMENU getMenu() const
        {
            return ::GetMenu(handle);
        }

        virtual Wnd& setFont(HFONT font, bool repaint = false)
        {
            sendMsg(WM_SETFONT, (WPARAM)font, MAKELPARAM(repaint, 0));
            return *this;
        }

        virtual LONG style() const
        {
            return ::GetWindowLong(handle, GWL_STYLE);
        }

        virtual LONG exStyle() const
        {
            return ::GetWindowLong(handle, GWL_EXSTYLE);
        }

        virtual Wnd& relayout()
        {
            sendMsg(WM_LAYOUT, 0, 0);
            return *this;
        }

        Wnd& move(int x, int y)
        {
            ::SetWindowPos(handle, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
            return *this;
        }

        Wnd& move(int x, int y, int w, int h)
        {
            ::SetWindowPos(handle, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
            return *this;
        }

        Wnd& move(const RECT& dest)
        {
            ::SetWindowPos(handle, NULL, dest.left, dest.top, dest.right - dest.left, dest.bottom - dest.top, SWP_NOZORDER | SWP_NOACTIVATE);
            return *this;
        }

        LRESULT sendMsg(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0)
        {
            return ::SendMessage(handle, msg, wParam, lParam);
        }

        LRESULT postMsg(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0)
        {
            return ::PostMessage(handle, msg, wParam, lParam);
        }

        Wnd& setText(const std::wstring& title)
        {
            ::SetWindowText(handle, title.c_str());
            return *this;
        }

        std::wstring getText()
        {
            int n = GetWindowTextLength(handle);
            wchar_t* buf = new wchar_t[n];
            n = ::GetWindowText(handle, buf, n);

            std::wstring result(buf, n);
            delete[] buf;
            return result;
        }

        HICON getIcon(int type = 0)
        {
            return (HICON)sendMsg(WM_GETICON, type, 96);
        }

        HICON setIcon(HICON icon, int type = 0)
        {
            return (HICON)sendMsg(WM_SETICON, type, (LPARAM)icon);
        }

        HWND dialogItem(int id)
        {
            return ::GetDlgItem(handle, id);
        }

        int dialogItemInt(int id, BOOL signedInt = true)
        {
            return ::GetDlgItemInt(handle, id, NULL, signedInt);
        }

        std::wstring dialogItemText(int id)
        {
            wchar_t buf[2048];
            UINT n = ::GetDlgItemText(handle, id, buf, 2048);
            return std::wstring(buf, n);
        }

        virtual HWND create(const wchar_t* title, int style = WS_OVERLAPPEDWINDOW, int exStyle = 0, HMENU menu = nullptr, HWND parent = nullptr)
        {
            RECT r = { CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 };
            return createWindow(title, parent, r, style, exStyle, menu);
        }

        virtual HWND create(size_t id, HWND parent, RECT& r, int style = WS_CHILD | WS_VISIBLE  | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, int exStyle = 0)
        {
            return createWindow(L"", parent, r, style, exStyle, (HMENU)id);
        }

        virtual HWND create(size_t id, const wchar_t* title, HWND parent, RECT& r, int style = WS_CHILD | WS_VISIBLE  | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, int exStyle = 0)
        {
            return createWindow(title, parent, r, style, exStyle, (HMENU)id);
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
                LRESULT r = wmDestroy();
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
                    return wmCtlColor((HDC)wParam, (HWND)lParam);
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

        virtual void onColorThemeChanged()
        {
            if (colorTheme && colorTheme->enabled() && colorTheme->font())
            {
                setFont(colorTheme->font());
            }
        }

        virtual LRESULT wmCreate()
        {
            return 0;
        }

        virtual LRESULT wmDestroy()
        {
            return 0;
        }

        virtual LRESULT wmNcDestroy()
        {
            return 0;
        }

        virtual LRESULT wmSize(RECT& clientRect)
        {
            return 0;
        }


        virtual LRESULT wmCtlColor(HDC hdc, HWND ctrl) 
        {
            ::SetBkColor(hdc,colorTheme->bkgColor());
            ::SetTextColor(hdc, colorTheme->textColor());
            //if (colorTheme->font) ::SelectObject(hdc, colorTheme->font);
            return (LRESULT)colorTheme->bkgBrush();
        }


        virtual HWND createWindow(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) = 0;

        std::shared_ptr<ColorTheme> colorTheme;
        std::wstring colorThemeToken_;
    };



    class OwnerDrawn
    {
    public:

        virtual ~OwnerDrawn() {}

        virtual LRESULT wmDrawItem(LPDRAWITEMSTRUCT dis) = 0;
        virtual LRESULT wmMeasureItem(MEASUREITEMSTRUCT * mis) = 0;
    };


    template<class W>
    class Window : public Wnd
    {
    public:

        HWND operator*() const
        {
            return handle;
        }

        Window()
        {}

        virtual LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            if(message == WM_LAYOUT)
            {
                RECT r = this->getClientRect();
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
                 this->wmCreate();
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
                    OwnerDrawn* ownerDrawn = (OwnerDrawn*)(lpDIS->itemData);
                    return ownerDrawn->wmDrawItem(lpDIS);
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
                    OwnerDrawn* ownerDrawn = (OwnerDrawn*)(mis->itemData);
                    return ownerDrawn->wmMeasureItem(mis);
                }
                return Wnd::wndProc(hWnd, message, wParam, lParam);
                break;
            }
            case WM_ACTIVATE :
            {
                RECT bounds = this->getClientRect();
                this->wmSize(bounds);
                return 0;
            }
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);

                RECT bounds = this->getClientRect();
                return this->wmSize(bounds);
                break;
            }
            case WM_PAINT:
            {
                return this->wmPaint();
                break;
            }
            case WM_ERASEBKGND:
            {
                return this->wmEraseBackground(wParam);
            }
            case WM_CLOSE:
            {
                this->wmClose();
                return 0;
            }
            case WM_DESTROY:
            {
                return this->wmDestroy();
                break;
            }
            case WM_NCDESTROY:
            {
                return this->wmNcDestroy();
                break;
            }
            case WM_DPICHANGED:
            {
                RECT* r = (RECT*)lParam;
                if (r)
                {
                    ::SetWindowPos(this->handle, NULL, r->left, r->top, r->right - r->left, r->bottom - r->top, 0);
                }
                LRESULT result = this->wmDpiChanged(r);
                ::InvalidateRect(this->handle, 0, TRUE);
                ::UpdateWindow(this->handle);
                return result;
                break;
            }
            case WM_DPICHANGED_BEFOREPARENT:
            {
                LRESULT result = this->wmDpiChanged(0);
                ::InvalidateRect(this->handle, 0, TRUE);
                ::UpdateWindow(this->handle);
                return result;
                break;
            }

            default:
            {
                return Wnd::wndProc(hWnd, message, wParam, lParam);
            }
            }
            return 0;
        }

    protected:

        virtual HWND createWindow(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu)
        {
            auto& wc = windowClass<W>();

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

        virtual LRESULT wmCommand(int id)
        {
            return -1;
        }

        virtual LRESULT wmAccellerator(int id)
        {
            return wmCommand(id);
        }

        virtual LRESULT wmControl(UINT message, WPARAM wParam, LPARAM lParam)
        {
            MSG msg;
            msg.hwnd = (HWND)lParam;
            msg.message = message;
            msg.wParam = wParam;
            msg.lParam = lParam;
            return ::SendMessage((HWND)lParam, WM_REFLECT, (WPARAM)&msg, 0);
        }

        virtual LRESULT wmSize(RECT& clientRect)
        {
            return 0;
        }

        virtual LRESULT wmPaint()
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
            LRESULT res = wmDraw(hdcMem, r);

            ::BitBlt(hdc, 0, 0, r.right - r.left, r.bottom - r.top, hdcMem, 0, 0, SRCCOPY);

            // Free-up the off-screen DC
            ::SelectObject(hdcMem, hOld);
            ::DeleteObject(hbmMem);
            ::DeleteDC(hdcMem);
            ::EndPaint(handle, &ps);
            return S_OK;
        }

        virtual LRESULT wmDraw(HDC hdc, RECT& bounds)
        {
            return 0;
        }

        virtual LRESULT wmClose()
        {
            destroy();
            return 0;
        }

        virtual LRESULT wmCreate() override
        {
            return 0;
        }


        virtual LRESULT wmDestroy() override
        {
            return 0;
        }

        virtual LRESULT wmNcDestroy() override
        {
            return 0;
        }

        virtual LRESULT wmLayout(RECT& r)
        {
            return wmSize(r);
        }

        virtual LRESULT wmEraseBackground(WPARAM wParam)
        {
            return ::DefWindowProc(handle, WM_ERASEBKGND, wParam, NULL);
        }

        virtual LRESULT wmDpiChanged(RECT* r)
        {
            return 0;
        }

        virtual LRESULT wmSearch(FINDREPLACE* fr)
        {
            return 0;
        }


    };

    template<class T>
    class ScrollWnd : public Window<T>
    {
    public:

        ScrollWnd()
        {
            ::ZeroMemory(&horizontalScrollInfo_, sizeof(SCROLLINFO));
            horizontalScrollInfo_.cbSize = sizeof(horizontalScrollInfo_);
            horizontalScrollInfo_.fMask = SIF_ALL;

            ::ZeroMemory(&verticalScrollInfo_, sizeof(SCROLLINFO));
            verticalScrollInfo_.cbSize = sizeof(verticalScrollInfo_);
            verticalScrollInfo_.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
        }

        void setVScroll(int rowHeight, unsigned int nRows)
        {
            scrollRowHeight_ = rowHeight;
            scrollRows_ = nRows;

            RECT r = this->getClientRect();
            scrollV(r.bottom);
        }

        void setHScroll(int colWidth, unsigned int nCols)
        {
            scrollColWidth_ = colWidth;
            scrollCols_ = nCols;

            RECT r = this->getClientRect();

            scrollH(r.right);
        }

        unsigned int  getScrollPos(int sb = SB_VERT)
        {
            if (sb == SB_HORZ)
            {
                ::GetScrollInfo(this->handle, sb, &horizontalScrollInfo_);
                return horizontalScrollInfo_.nPos;
            }

            ::GetScrollInfo(this->handle, sb, &verticalScrollInfo_);
            return verticalScrollInfo_.nPos;
        }

        unsigned int  getScrollMax(int sb = SB_VERT)
        {
            if (sb == SB_HORZ)
            {
                ::GetScrollInfo(this->handle, sb, &horizontalScrollInfo_);
                return horizontalScrollInfo_.nMax;
            }

            ::GetScrollInfo(this->handle, sb, &verticalScrollInfo_);
            return verticalScrollInfo_.nMax;
        }

        void setScrollPos(unsigned int p, int sb = SB_VERT)
        {
            RECT r = this->getClientRect();

            if (sb == SB_HORZ)
            {
                int xPos = getScrollPos(SB_HORZ);
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
                int yPos = getScrollPos();
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
                int yPos = getScrollPos(SB_VERT);
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
                    RECT r = this->getClientRect();
                    int height = r.bottom / verticalScrollInfo_.nPage;

                    ::ScrollWindow(this->handle, 0, height * (yPos - verticalScrollInfo_.nPos), NULL, NULL);
                    ::UpdateWindow(this->handle);
                    this->wmVScroll(verticalScrollInfo_.nPos);
                }
                return 0;
            }
            case WM_HSCROLL:
            {
                int xPos = getScrollPos(SB_HORZ);
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
                    RECT r = this->getClientRect();
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

                    setScrollPos(getScrollPos() + cLineScroll);
                    //invalidateRect(0,TRUE);
                }
                break;
            }
            case WM_SIZE:
            {
                scrollV(HIWORD(lParam));
                scrollH(LOWORD(lParam));
                ::InvalidateRect(this->handle,0, FALSE);
                return Window<T>::wndProc(hWnd, message, wParam, lParam);
            }
            } // end switch
            return Window<T>::wndProc(hWnd, message, wParam, lParam);
        }

        void virtual wmVScroll(unsigned int pos) {};
        void virtual wmHScroll(unsigned int pos) {};

    protected:

        SCROLLINFO verticalScrollInfo_;
        SCROLLINFO horizontalScrollInfo_;
        int scrollRowHeight_;
        int scrollColWidth_;
        unsigned int scrollRows_;
        unsigned int scrollCols_;
        short wheelDelta_;

        void scrollV(unsigned int h)
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

        void scrollH(unsigned int w)
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

    class Timer
    {
    public:

        UINT_PTR id = 0;

        Timer()
        {}

        ~Timer()
        {
            cancel();
        }

        Timer(const Timer& rhs) = delete;
        Timer& operator=(const Timer& rhs) = delete;

        Timer(Timer&& rhs) noexcept
            : id(rhs.id)
        {
            rhs.id = 0;
        }

        Timer& operator = (Timer&& rhs) noexcept
        {
            if (id == rhs.id)
            {
                return *this;
            }
            id = rhs.id;
            rhs.id = 0;
            return *this;
        }

        Timer(int milisecs, std::function<void()> cb)
        {
            timeout(milisecs, cb);
        }

        UINT_PTR timeout(int milisecs, std::function<void()> cb)
        {
            cancel();
            id = ::SetTimer(nullptr, 0, milisecs, &Timer::timerProc);
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
            ::OutputDebugString(L"TIMER\r\n");
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

    // Chrome_WidgetWin_0
    // typedef BOOL (CALLBACK* WNDENUMPROC)(HWND, LPARAM);
    inline HWND findChild(HWND parent, const std::wstring& s)
    {
        struct Enumerator 
        {
            HWND result = nullptr;
            std::wstring className;

            static BOOL __stdcall proc(HWND hWnd, LPARAM lParam)
            {
                Enumerator* enumerator = (Enumerator*)lParam;
                MTL::wbuff buf(1024);
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


    inline RECT workArea(HWND wnd)
    {
        HMONITOR monitor = ::MonitorFromWindow(wnd, 0);
        MONITORINFO moni;
        moni.cbSize = sizeof(moni);
        GetMonitorInfo(monitor, &moni);
        return moni.rcWork;
    }
}
