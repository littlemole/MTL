#pragma once

#include "mtl/win/wnd.h"
#include "mtl/ole/img.h"
#include "mtl/ole/dataobj.h"

#include <commctrl.h>
#include <Uxtheme.h>

namespace mtl {

    class Win32CommonControls
    {
    public:
        Win32CommonControls()
        {
            INITCOMMONCONTROLSEX iccx;
            iccx.dwSize = sizeof(iccx);
            iccx.dwICC = ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES |
                ICC_BAR_CLASSES | ICC_COOL_CLASSES |
                ICC_DATE_CLASSES | ICC_HOTKEY_CLASS |
                ICC_INTERNET_CLASSES | ICC_LINK_CLASS |
                ICC_NATIVEFNTCTL_CLASS | ICC_PAGESCROLLER_CLASS |
                ICC_TREEVIEW_CLASSES| ICC_LISTVIEW_CLASSES|
                ICC_USEREX_CLASSES;
            ::InitCommonControlsEx(&iccx);
        }
    };


    template<class W>
    class ctrl : public wnd
    {
    public:

        event<int()> onCommand;
        event<UINT(NMHDR*)> onNotify;

        ~ctrl()
        {}

        //default ctrl handling: pass back to windows
        virtual LRESULT wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            if(message == WM_REFLECT)
            {
                MSG* msg = (MSG*)wParam;
                if (msg->message == WM_COMMAND)
                {
                    int code = HIWORD(msg->wParam);
                    int wmId = LOWORD(msg->wParam);

                    return this->wm_command(wmId, code);
                }
                if (msg->message == WM_NOTIFY)
                {
                    NMHDR* nmhdr = (NMHDR*)(msg->lParam);

                    if (colorTheme && colorTheme->enabled() && (nmhdr->code == NM_CUSTOMDRAW))
                    {
                        NMCUSTOMDRAW* cd = (NMCUSTOMDRAW*)nmhdr;
                        return this->wm_custom_draw(cd);
                    }

                    return this->wm_notify((int)msg->wParam, nmhdr);
                }
                if (msg->message == WM_DRAWITEM)
                {
                    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)msg->lParam;
                    return this->wm_draw_item(lpDIS);
                }
                return 0;
            }

            switch (message)
            {
                case WM_SIZE:
                {
                    RECT cr = client_rect();
                    wm_size(cr);
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
                    break;
                }
            }
            return ::CallWindowProc(oldProc_, hwnd, message, wParam, lParam);
        }
         
        //  subclassing the def wndproc
        void subclass()
        {
            ::SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)dynamic_cast<void*>((W*)this));
            WNDPROC proc = (WNDPROC)(::SetWindowLongPtr(handle, GWLP_WNDPROC, (LONG_PTR)(windowProcedure)));

            if (proc != windowProcedure)
            {
                oldProc_ = proc;
            }
        }

        void subclass(HWND hwnd)
        {
            handle = hwnd;
            subclass();
        }

        void declass()
        {
            if (oldProc_)
            {
                ::SetWindowLongPtr(handle, GWLP_WNDPROC, (LONG_PTR)(oldProc_));
            }
        }

    protected:

        virtual LRESULT wm_command(int id, int code)
        {
            onCommand.fire(code);
            return 0;
        }

        virtual LRESULT wm_notify(int id, NMHDR* nmhdr)
        {
            onNotify.fire(nmhdr->code, nmhdr);
            return 0;
        }

        virtual LRESULT wm_create() override
        {
            return 0;
        }

        virtual LRESULT wm_size(RECT& rc) override
        {
            return 0;
        }

        virtual LRESULT wm_custom_draw(NMCUSTOMDRAW* cd)
        {
            return 0;
        }

        virtual LRESULT wm_draw_item(LPDRAWITEMSTRUCT dis)
        {
            return 0;
        }

        virtual HWND create_window(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) override
        {
            auto& wndClass = wc<W>();

            handle = ::CreateWindowEx(
                exStyle,
                wndClass.name(),
                title,
                style,
                r.left, r.top, r.right - r.left, r.bottom - r.top,
                parent,
                menu,
                module_instance(),
                (LPVOID)(W*)this
            );

            subclass();
            wm_create();
            return handle;
        }

        static LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
        {
            LONG_PTR l = ::GetWindowLongPtrW(hWnd, GWLP_USERDATA);
            if (!l)
            {
                return ::DefWindowProc(hWnd, message, wParam, lParam);
            }

            W* that = (W*)l;

            return that->wndProc(hWnd, message, wParam, lParam);
        }

        WNDPROC	oldProc_ = nullptr;
    };

    class button;

    template<>
    class window_class<button>
    {
    public:
        const wchar_t* name()
        {
            return L"Button";
        }
    };

    class button : public ctrl<button>
    {
    public:

        void set_bitmap(HBITMAP bmp)
        {
            send_msg(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp);
        }

        bool checked()
        {
            return BST_CHECKED == send_msg(BM_GETCHECK, 0, 0);
        }

        void check(bool b = true)
        {
            send_msg(BM_SETCHECK, b ? BST_CHECKED : BST_UNCHECKED, 0);
        }

    protected:

        virtual LRESULT wm_command(int id, int code) override
        {
            onCommand.fire(id);
            return 0;
        }
    };


    class edit_ctrl;

    template<>
    class window_class<edit_ctrl>
    {
    public:
        const wchar_t* name()
        {
            return L"Edit";
        }
    };

    class edit_ctrl : public ctrl<edit_ctrl>
    {};

    class status_bar;

    template<>
    class window_class<status_bar>
    {
    public:
        const wchar_t* name()
        {
            return STATUSCLASSNAME;
        }
    };

    class status_bar : public ctrl<status_bar>
    {
    public:

        mtl::event<int()> onLeftClick;
        mtl::event<int()> onRightClick;

        status_bar()
        {
            onNotify(NM_CLICK, [this](NMHDR* nmhdr) 
            {
                NMMOUSE* nm = (NMMOUSE*)nmhdr;
                onLeftClick.fire((int)nm->dwItemSpec);
            });

            onNotify(NM_RCLICK, [this](NMHDR* nmhdr)
            {
                NMMOUSE* nm = (NMMOUSE*)nmhdr;
                onRightClick.fire((int)nm->dwItemSpec);
            });
        }

        void set_status(std::vector<std::wstring> parts)
        {
            status_ = parts;
            set_status_txt(statusRect_);
        }

    protected:

        void set_status_txt( RECT& r)
        {
            int n = (int)status_.size();

            if (n == 0) return;

            mtl::wnd_dc dc(handle);

            HFONT font = (HFONT)send_msg(WM_GETFONT, 0, 0);
            dc.select(font);

            std::vector<int> widths;
            for (auto& part : status_)
            {
                int w = dc.text_width(part)+10;
                widths.push_back(w);
            }

            std::vector<int> rights;
            rights.resize(n);

            int right = r.right;
            for (int i = n - 1; i > 0; i--)
            {
                rights[i] = right;
                right -= widths[i];
            }
            rights[0] = widths[0];

            send_msg(SB_SETPARTS, (WPARAM)n, (LPARAM)&rights[0]);

            for (int i = 0; i < n; i++)
            {
                send_msg(SB_SETTEXT, (WPARAM)i, (LPARAM)status_[i].c_str());
            }
        }

        virtual LRESULT wm_size(RECT& rc) override
        {
            statusRect_ = rc;
            set_status_txt(rc);
            return 0;
        }

        RECT statusRect_;
        std::vector<std::wstring> status_;
        int parts_ = 0;
    };


    class tool_tip;

    template<>
    class window_class<tool_tip>
    {
    public:
        const wchar_t* name()
        {
            return TOOLTIPS_CLASS;
        }
    };

    class tool_tip : public ctrl<tool_tip>
    {
    public:

        template<class T>
        LRESULT add(T& ctrl, const std::wstring& txt)
        {
            TOOLINFO ti;
            RECT r;
            ::GetClientRect(*ctrl, &r);

           ctrl.declass();

            ti.cbSize = sizeof(TOOLINFO);
            ti.uFlags = TTF_SUBCLASS|TTF_IDISHWND;
            ti.hwnd = ::GetParent(*ctrl);
            ti.hinst = 0;
            ti.uId = (UINT_PTR)(HWND)(*ctrl);
            ti.lpszText = (wchar_t*)(txt.c_str());
            ti.rect.left = r.left;
            ti.rect.top = r.top;
            ti.rect.right = r.right;
            ti.rect.bottom = r.bottom;
            ti.lpReserved = 0;
            ti.lParam = 0;

            LRESULT res = send_msg(TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
            ctrl.subclass();
            return res;
        }

        template<class T>
        LRESULT add(HWND notifyParent, T& ctrl)
        {
            TOOLINFO ti;
            RECT r;
            ::GetClientRect(*ctrl, &r);

            ctrl.declass();

            ti.cbSize = sizeof(TOOLINFO);
            ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
            ti.hwnd = notifyParent;
            ti.hinst = 0;
            ti.uId = (UINT_PTR)(HWND)(*ctrl);
            ti.lpszText = LPSTR_TEXTCALLBACK;
            ti.rect.left = r.left;
            ti.rect.top = r.top;
            ti.rect.right = r.right;
            ti.rect.bottom = r.bottom;
            ti.lpReserved = 0;
            ti.lParam = 0;

            LRESULT res = send_msg(TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
            ctrl.subclass();
            return res;
        }

    protected:

        virtual HWND create_window(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) override
        {
            auto& wndClass = wc<tool_tip>();

            handle = ::CreateWindowEx(
                exStyle,
                wndClass.name(),
                0,
                style,
                r.left, r.top, r.right - r.left, r.bottom - r.top,
                parent,
                0,
                module_instance(),
                (LPVOID)this
            );

            ::SetWindowPos(handle, HWND_TOPMOST, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);


            DWORD e = ::GetLastError();
            subclass();
            return handle;
        }
    };

    class image_list
    {
    public:

        int width = 24;
        int height = 24;

        image_list()
        {}

        image_list(int w, int h, int style = ILC_COLOR32 | ILC_MASK, int n = 1, int max = 255)
            : width(w), height(h)
        {
            create(w, h, style, n, max);
        }

        image_list& create(int w, int h, int style = ILC_COLOR32 | ILC_MASK, int n = 1, int max = 255)
        {
            if (himl_)
            {
                ::ImageList_Destroy(himl_);
            }
            width = w;
            height = h;
            himl_ = ::ImageList_Create(w, h, ILC_COLOR32 | ILC_MASK, n, max);
            return *this;
        }

        ~image_list()
        {
            ::ImageList_Destroy(himl_);
            himl_ = nullptr;
        }

        int add_bitmap(HBITMAP bmp, COLORREF col = RGB(0, 0, 0))
        {
            return ::ImageList_AddMasked(
                himl_,
                bmp,
                col
            );
        }

        int add_icon(HICON ico)
        {
            return ::ImageList_AddIcon(himl_, ico);
        }

        void draw(HDC hdc, int index, int x, int y, int options = ILD_TRANSPARENT)
        {
            ::ImageList_Draw(himl_, index, hdc,x, y, options);
        }

        HIMAGELIST operator*()
        {
            return himl_;
        }

        image_list(const image_list& rhs) = delete;
        image_list(image_list&& rhs) = delete;

        image_list& operator=(const image_list& rhs) = delete;
        image_list& operator=(image_list&& rhs) = delete;

    private:

        HIMAGELIST himl_ = nullptr;
    };



    class tab_ctrl;

    template<>
    class window_class<tab_ctrl>
    {
    public:
        const wchar_t* name()
        {
            return WC_TABCONTROL;
        }
    };

    class tab_ctrl : public ctrl<tab_ctrl>
    {
    public:

        class tab
        {
        public:
            tab()
                : title(L""), lparam(0)
            {}

            tab(const std::wstring& t)
                : title(t), lparam(0)
            {}

            tab(const std::wstring& t, const std::wstring& tt)
                : title(t), tooltip(tt), lparam(0)
            {}

            tab(const std::wstring& t, const std::wstring& tt, const std::wstring& i, int idx = -1, LPARAM p = 0)
                : title(t), tooltip(tt), id(i), index(idx),lparam(p)
            {}

            std::wstring title;
            std::wstring tooltip;
            std::wstring id;
            int index = -1;
            HWND hWnd = nullptr;
            LPARAM lparam = 0;
        };

        event<void(tab&)>                                          onSelect;
        event<void(tab&, IDataObject*)>                            onPopulateDataObj;
        event<void(tab&)>                                          onDrag;
        event<void(std::wstring)>                                  onDragOut;
        event<void(IDataObject*, DWORD, DWORD&)>                   onDrop;
        event<void(int, IDataObject*)>                             onDropExternal;
        event<void(std::wstring)>                                  onCloseTab;

        HWND                                                       activeChild = nullptr;
        CLIPFORMAT			                                       dragTabFormat;
        punk<default_drop_target>                                  dropTarget;
        tool_tip                                                   tooltip;
        image_list			                                       imageList;

        tab_ctrl()
        {
        }

        virtual ~tab_ctrl() {}

        HWND set_child(HWND newChild)
        {
            HWND old = activeChild;
            if (::IsWindow(old))
            {
                ::ShowWindow(old, SW_HIDE);
            }
            activeChild = newChild;
            ::ShowWindow(activeChild, SW_SHOW);
            post_msg(WM_LAYOUT, 0, 0);
            return old;
        }

        void enable_icon(HICON icon, int w = 16, int h = -1)
        {
            if (h == -1) h = w;
            imageList.create(w, h);
            send_msg(TCM_SETIMAGELIST, 0, (LPARAM)*imageList);
            iconIndex = imageList.add_icon(icon);

        }

        void enable_dragdrop()
        {
            dragTabFormat = ::RegisterClipboardFormat(L"MTLTabDragDropCustomClipBoardFormat");
            dropTarget = drop_target(dragTabFormat, DROPEFFECT_MOVE);

            dropTarget->onDrop([this](IDataObject* ido, DWORD keyState, DWORD& effect)
            {
                onDrop.fire(ido, keyState, effect);
            });

            send_msg(TCM_SETEXTENDEDSTYLE, TCS_EX_REGISTERDROP, TCS_EX_REGISTERDROP);

            onNotify(TCN_GETOBJECT, [this](NMHDR* nmhdr)
            {
                wm_object_notify((NMOBJECTNOTIFY*)nmhdr);
            });

            onPopulateDataObj([this](tab& c, IDataObject* ido)
            {
            });

            onDrag([this](tab& c)
            {
                format_etc fe(dragTabFormat);
                DWORD effect = 0;

                auto obj = data_obj(fe, c.id);

                onPopulateDataObj.fire(c, *obj);


                HRESULT hr = ::DoDragDrop(
                    *obj,
                    *drop_source(),
                    DROPEFFECT_COPY | 
                    DROPEFFECT_MOVE,
                    &effect
                );

                if (effect == DROPEFFECT_NONE)
                {
                    POINT pt;
                    ::GetCursorPos(&pt);
                    RECT r;
                    ::GetWindowRect(parent(), &r);
                    if (!::PtInRect(&r, pt))
                    {
                        onDragOut.fire(c.id);
                    }
                };
            });

            onDrop([this](IDataObject* ido, DWORD keyState, DWORD& effect)
            {
                dataobj_view dv(ido);
                std::wstring id = dv.wstring(dragTabFormat);

                int index_to = hit_test();
                int cur = (int)selected();

                if (index_to == -1)
                    index_to = (int)count();
                if (index_to == -1)
                    index_to = 0;

                if (count())
                {
                    auto tci = getTabCtrlItem(index_to);
                    if (tci)
                    {
                        std::wstring to = tci->id;

                        if (id == to)
                        {
                            effect = DROPEFFECT_NONE;
                            return;
                        }
                    }
                }
                int index_from = id_to_index(id);
                if (index_from == -1)
                {
                    onDropExternal.fire(index_to, ido);
                    effect = DROPEFFECT_MOVE;
                    return;
                }
                tab* tci = getTabCtrlItem(index_from);
                removeItem(index_from, false);
                insertItem(tci, index_to);

                clear_highlite();
                select(index_to);
                onSelect.fire(*tci);
                //highlite(cur);
            });
        }


        void add(tab&& item, int index = -1)
        {
            tab* tci = new tab(item);
            int idx = (int) insertItem(tci, index);
            select(idx);
            post_msg(WM_LAYOUT, 0, 0);
        }

        void add(tab&& item, HWND child, int index = -1)
        {
            tab* tci = new tab(item);
            tci->hWnd = child;
            int idx = (int)insertItem(tci, index);
            select(idx);
            post_msg(WM_LAYOUT, 0, 0);
        }

        tab& item(int index)
        {
            return *getTabCtrlItem(index);
        }

        tab& item(const std::wstring& id)
        {
            return *getTabCtrlItem(id_to_index(id));
        }

        void clear_highlite()
        {
            // clear highlight bits from tabctrl
            for (int i = 0; i < count(); i++)
            {
                post_msg(TCM_HIGHLIGHTITEM, i, FALSE);
            }
        }

        void highlite(int idx, bool bShow = true)
        {
            post_msg(TCM_HIGHLIGHTITEM, idx, bShow);
        }

        tab remove(int index = -1)
        {
            tab result(*getTabCtrlItem(index));
            removeItem(index, true);
            return result;
        }

        tab remove(const std::wstring& id)
        {
            int index = id_to_index(id);
            return remove(index);
        }

        int count()
        {
            return (int)send_msg(TCM_GETITEMCOUNT, 0, 0);
        }

        LRESULT select(int i)
        {
            auto tci = getTabCtrlItem(i);
            if (tci)
            {
                HWND hWnd = tci->hWnd;
                if (::IsWindow(hWnd))
                {
                    set_child(hWnd);
                }
            }
            return send_msg(TCM_SETCURSEL, (WPARAM)i, 0);
        }

        LRESULT select(const std::wstring& id)
        {
            int index = id_to_index(id);
            return select(index);
        }

        int selected()
        {
            return (int)TabCtrl_GetCurSel(handle);
        }

        RECT display_rect()
        {
            RECT r = window_rect();
            LRESULT res = send_msg(TCM_ADJUSTRECT, FALSE, (LPARAM)&r);
            //r.right -= 2;
            //r.bottom -= 2;
            return r;
        }

        int hit_test()
        {
            TCHITTESTINFO tchit;
            ::GetCursorPos(&(tchit.pt));
            ::ScreenToClient(handle, &(tchit.pt));
            return TabCtrl_HitTest(handle, &tchit);
        }

        RECT tab_rect(int index)
        {
            RECT r;
            TabCtrl_GetItemRect(handle, index, &r);
            return r;
        }

        bool hit_icon_test()
        {
            TCHITTESTINFO tchit;
            ::GetCursorPos(&(tchit.pt));
            ::ScreenToClient(handle, &(tchit.pt));
            int r = TabCtrl_HitTest(handle, &tchit);
            if (r !=-1)
            {
                if (tchit.flags == TCHT_ONITEMICON)
                {
                    return true;
                }

                if (iconIndex == -1)
                {
                    return false;
                }

                RECT tr = tab_rect(selected());

                tr.right = tr.left + imageList.width;
                tr.bottom = tr.top + imageList.height;

                if (::PtInRect(&tr, tchit.pt))
                {
                    return true;
                }
            }
            return false;
        }

        int id_to_index(std::wstring id)
        {
            int c = count();
            for (int i = 0; i < c; i++)
            {
                tab* tci = getTabCtrlItem(i);
                if (tci)
                {
                    if (tci->id == id)
                    {
                        return i;
                    }
                }
            }
            return -1;
        }


        virtual LRESULT wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
        {
            if (message == WM_LAYOUT) message = WM_SIZE;
            switch (message)
            {
                case WM_CREATE:
                {
                    break;
                }
                case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);

                    RECT bounds;
                    ::GetClientRect(handle, &bounds);
                    wm_size(bounds);
                    break;
                }
                case WM_LBUTTONDOWN:
                {
                    handleOnMouseDown();
                    break;
                }
                case WM_LBUTTONUP:
                {
                    handleOnMouseUp();
                    break;
                }
                case WM_PAINT:
                {
                    if (colorTheme && colorTheme->enabled())
                    {
                        return wm_paint_handler();
                    }
                    break;
                }
                case WM_ERASEBKGND:
                {
                    if (colorTheme && colorTheme->enabled())
                    {
                        return 1;
                    }
                    return 0;
                }
            }
            return ctrl<tab_ctrl>::wndProc(hwnd, message, wParam, lParam);
        }

    protected:

        virtual LRESULT wm_size(RECT& clientRect) override
        {
            if (::IsWindow(activeChild))
            {
                RECT tr = display_rect();

                POINT p1{ tr.left, tr.top };
                ::ScreenToClient(handle, &p1);
                POINT p2{ tr.right, tr.bottom };
                ::ScreenToClient(handle, &p2);
                RECT r = {
                    p1.x,p1.y,
                    p2.x,p2.y
                };

                ::SetWindowPos(
                    activeChild, nullptr,
                    r.left, r.top,
                    r.right, r.bottom,
                    SWP_NOACTIVATE | SWP_NOZORDER
                );
            }
            return 0;
        }

        virtual LRESULT wm_notify(int id, NMHDR* nmhdr) override
        {
            if (nmhdr->code == TCN_SELCHANGE)
            {
                int curSel = selected();
                if (curSel == -1)
                {
                    return 0;
                }
                tab* tci = getTabCtrlItem(curSel);
                if (tci)
                {
                    onSelect.fire(*tci);
                    if (::IsWindow(tci->hWnd))
                    {
                        set_child(tci->hWnd);
                    }
                }
            }
            if (nmhdr->code == TCN_SELCHANGING)
            {
                return hit_icon_test();
            }

            if (nmhdr->code == NM_CLICK)
            {
                if (hit_icon_test())
                {
                    int idx = hit_test();
                    if (idx != -1)
                    {
                        auto& it = item(idx);
                        onCloseTab.fire(it.id);
                        return 0;
                    }
                }
            }
            onNotify.fire(nmhdr->code, nmhdr);
            return 0;
        }

        virtual LRESULT wm_draw_item(LPDRAWITEMSTRUCT dis) override
        {
            if (!this->colorTheme || !this->colorTheme->enabled()) return 0;

            HBRUSH br = nullptr;
            BOOL bSelected = (dis->itemID == (UINT)selected());

            ::SetBkMode(dis->hDC, TRANSPARENT);
            if (!bSelected)
            {
                ::SetTextColor(dis->hDC, this->colorTheme->text_color());
                br = this->colorTheme->bkg_brush();
            }
            else
            {
                ::SetTextColor(dis->hDC, this->colorTheme->selected_text_color());
                br = this->colorTheme->selected_bkg_brush();
            }
            ::FillRect(dis->hDC, &dis->rcItem, br);

            TCITEM tabitem;
            ::ZeroMemory(&tabitem, sizeof(TCITEM));
            TCHAR  buff[30] = { 0 };

            tabitem.mask = TCIF_TEXT;
            tabitem.pszText = buff;
            tabitem.cchTextMax = 30;
            ::SendMessage(handle, TCM_GETITEM, static_cast<WPARAM>(dis->itemID), reinterpret_cast<LPARAM>(&tabitem));

            ::ImageList_Draw(*imageList, 0, dis->hDC, dis->rcItem.left, dis->rcItem.top, ILD_TRANSPARENT);

            ::TextOut(dis->hDC, (dis->rcItem.left + 6 + imageList.width), (dis->rcItem.top + 2), tabitem.pszText, lstrlen(tabitem.pszText));
            return 0;
        }

        virtual void wm_object_notify(NMOBJECTNOTIFY* notify)
        {
            notify->hResult = E_FAIL;
            IID iid = *(notify->piid);
            if (dropTarget && ::IsEqualIID(IID_IDropTarget, iid))
            {
                IUnknown* unk = *dropTarget;
                unk->AddRef();
                notify->pObject = unk;
                notify->hResult = S_OK;
            }
        }

        INT_PTR wm_paint_handler()
        {
            PAINTSTRUCT ps;
            HDC hDC = ::BeginPaint(handle, &ps);

            ::CallWindowProc(oldProc_, handle, WM_PRINTCLIENT, (WPARAM)hDC, PRF_CLIENT);

            HRGN hRgn = ::CreateRectRgn(0, 0, 0, 0);
            int n_items = TabCtrl_GetItemCount(handle);
            int current_item = TabCtrl_GetCurSel(handle);

            RECT r;
            RECT lh_corner = { 0 }, rh_corner = { 0 };
            bool gIsVista = true;
            bool xp_themed = true;
            
            for (int i = 0; i < n_items; ++i)
            {
                TabCtrl_GetItemRect(handle, i, &r);
                if (i == current_item)
                {
                    r.left -= 1;
                    r.right += 1;
                    r.top -= 2;
                    if (i == 0)
                    {
                        r.left -= 1;
                        if (!xp_themed)
                            r.right += 1;
                    }
                    if (i == n_items - 1)
                        r.right += 1;
                }
                else
                {
                    r.right -= 1;
                    if ((xp_themed || gIsVista) && i == n_items - 1)
                        r.right -= 1;
                }

                ::FrameRect(hDC, &r, colorTheme->bkg_brush());
                r.bottom -= 2;
                if (i == current_item)
                {
                    RECT tmp = r;
                    //tmp.top -= 1;
                    tmp.top += 1;
                    tmp.bottom += 1;

                    ::FrameRect(hDC, &tmp, colorTheme->selected_bkg_brush());
                }

                if (xp_themed)
                {
                    if (i != current_item + 1)
                    {
                        lh_corner = r;
                        lh_corner.bottom = lh_corner.top + 1;
                        lh_corner.right = lh_corner.left + 1;
                    }

                    rh_corner = r;
                    rh_corner.bottom = rh_corner.top + 1;
                    rh_corner.left = rh_corner.right - 1;
                }

                HRGN hTabRgn = ::CreateRectRgn(r.left, r.top, r.right, r.bottom);
                ::CombineRgn(hRgn, hRgn, hTabRgn, RGN_OR);
                BOOL ok = ::DeleteObject(hTabRgn);

                if (lh_corner.right > lh_corner.left)
                {
                    HRGN hRoundedCorner = ::CreateRectRgn(
                        lh_corner.left, 
                        lh_corner.top, 
                        lh_corner.right, 
                        lh_corner.bottom
                    );
                    ::CombineRgn(hRgn, hRgn, hRoundedCorner, RGN_DIFF);
                    ok = ::DeleteObject(hRoundedCorner);
                }

                if (rh_corner.right > rh_corner.left)
                {
                    HRGN hRoundedCorner = ::CreateRectRgn(
                        rh_corner.left, 
                        rh_corner.top, 
                        rh_corner.right, 
                        rh_corner.bottom
                    );
                    ::CombineRgn(hRgn, hRgn, hRoundedCorner, RGN_DIFF);
                    ok = ::DeleteObject(hRoundedCorner);
                }
            }

            ::GetClientRect(handle, &r);
            HRGN hFillRgn = ::CreateRectRgn(r.left, r.top, r.right, r.bottom);
            ::CombineRgn(hFillRgn, hFillRgn, hRgn, RGN_DIFF);
            ::SelectClipRgn(hDC, hFillRgn);
 
            ::FillRgn(hDC, hFillRgn, colorTheme->bkg_brush());

            BOOL ok = DeleteObject(hFillRgn);
            ok = DeleteObject(hRgn);
            EndPaint(handle, &ps);
            return 0;
        }

        tab* getTabCtrlItem(int i)
        {
            TCITEM item;
            ZeroMemory(&item, sizeof(item));
            item.mask = TCIF_PARAM;
            send_msg(TCM_GETITEM, (WPARAM)i, (LPARAM)(const LPTCITEM)(&item));
            return (tab*)(item.lParam);
        }

        LRESULT insertItem(tab* titem, int index = -1)
        {
            int c = (int)count();
            //default: add to back
            if (index == -1)
                index = c;

            TCITEM item;
            item.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
            item.pszText = (wchar_t*)(titem->title.c_str());
            item.cchTextMax = (int)titem->title.size() + sizeof(wchar_t);

            int iconIdx = titem->index;
            if (iconIdx == -1 && iconIndex != -1)
            {
                iconIdx = iconIndex;
            }
            item.iImage = iconIdx;
            item.lParam = (LPARAM)(titem);

            LRESULT i = send_msg(TCM_INSERTITEM, (WPARAM)index, (LPARAM)(const LPTCITEM)(&item));

            return i;
        }

        tab* removeItem(int index = -1, bool deleteItem = true)
        {
            tab* selectedItem = getTabCtrlItem(selected());
            tab* tci = (tab*)getTabCtrlItem(index);
            LRESULT lr = (LRESULT)TabCtrl_DeleteItem(handle, index);

            int c = count();

            if ( c > 0 )
            {
                if (tci && selectedItem && (selectedItem->id == tci->id))
                {
                    int newIndex2Select = index ;
                    if (newIndex2Select < 0) newIndex2Select = 0;
                    if (newIndex2Select > c - 1) newIndex2Select = c - 1;
                    select(newIndex2Select);
                }
                else
                {
                    select(selectedItem->id);
                }
            }

            if (tci && deleteItem)
            {
                delete tci;
                return 0;
            }
            return tci;
        }

        virtual void handleOnMouseUp()
        {
            isMouseDown_ = false;
        }

        virtual void handleOnMouseDown()
        {
            isMouseDown_ = false;

            int i = hit_test();
            if (i != -1)
            {
                // wait a bit might be a drag-drop
                isMouseDown_ = true;
                timer_.timeout(250, [this](UINT_PTR)
                {
                    if (isMouseDown_)
                    {
                        isMouseDown_ = false;
                        int i = hit_test();
                        if (i != -1)
                        {
                            // do a simple dragDrop
                            tab* c = getTabCtrlItem(i);
                            if (c)
                            {
                                onDrag.fire(*c);
                            }
                        }
                    }
                });
            }
        }


        void forEach(std::function<void(tab& tci)> visitor)
        {
            int c = count();
            for (int i = 0; i < c; i++)
            {
                tab* ptci = getTabCtrlItem(i);
                if (ptci)
                {
                    visitor(*ptci);
                }
            }
        }

        virtual void on_color_theme_changed()
        {
            wnd::on_color_theme_changed();
            tooltip.set_color_theme(colorTheme);

            LONG style = ::GetWindowLong(handle, GWL_STYLE);

            if (!colorTheme->enabled())
            {
                style &= ~TCS_OWNERDRAWFIXED;
            }
            else
            {
                style |= TCS_OWNERDRAWFIXED;
            }
            ::SetWindowLong(handle, GWL_STYLE, style);
        }

        virtual HWND create_window(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) override
        {
            auto& wndClass = wc<tab_ctrl>();

            handle = ::CreateWindowEx(
                exStyle,
                wndClass.name(),
                title,
                style,
                r.left, r.top, r.right - r.left, r.bottom - r.top,
                parent,
                menu,
                module_instance(),
                (LPVOID)this
            );

            subclass();

            RECT rp;
            ::GetClientRect(parent, &rp);
            tooltip.create(1, parent, rp, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP);

            tooltip.add(parent, *this);

            tooltip.onNotify(TTN_GETDISPINFO, [this](NMHDR* nmhdr)
            {
                HWND hwndFrom = nmhdr->hwndFrom;
                UINT_PTR id = nmhdr->idFrom;
                NMTTDISPINFO* di = (NMTTDISPINFO*)nmhdr;
                di->lpszText = 0;
                di->hinst = 0;
                di->szText[0] = 0;

                int i = hit_test();
                if (i != -1)
                {
                    tab* c = getTabCtrlItem(i);
                    if (c)
                    {
                        di->lpszText = (wchar_t*)(c->tooltip.c_str());
                        di->hinst = 0;
                    }
                }

            });

            return handle;
        }


        timer                       timer_;
        std::vector<tab>            items_;
        bool				        isMouseDown_ = false;
        int                         iconIndex = -1;
    };

    class combo_box;

    template<>
    class window_class<combo_box>
    {
    public:
        const wchar_t* name()
        {
            return WC_COMBOBOX;
        }
    };

    class combo_box : public ctrl<combo_box>
    {
    public:

        combo_box& add(const std::wstring& s, LPARAM param = 0 )
        {
            int r = (int) send_msg(CB_ADDSTRING, 0, (LPARAM)s.c_str());
            if ( r != CB_ERR && param)
            {
                send_msg(CB_SETITEMDATA, r, param);
            }
            return *this;
        }

        void width(int w)
        {
            send_msg(CB_SETDROPPEDWIDTH, w, 0);
        }

        int count()
        {
            return (int)send_msg(CB_GETCOUNT, 0, 0);
        }

        LRESULT item_data(int id)
        {
            return send_msg(CB_GETITEMDATA, 0, 0);
        }

        std::wstring item_txt(int id)
        {
            int n = (int)send_msg(CB_GETLBTEXTLEN, id, 0);
            wbuff wbuf(n);
            n = (int)send_msg(CB_GETLBTEXT, id, 0);
            return wbuf.toString(n);
        }

        LRESULT selected()
        {
            return send_msg(CB_GETCURSEL, 0, 0);
        }

        combo_box& select(int idx)
        {
            send_msg(CB_SETCURSEL, idx, 0);
            return *this;
        }
    };

    class combo_box_ex;

    template<>
    class window_class<combo_box_ex>
    {
    public:
        const wchar_t* name()
        {
            return WC_COMBOBOXEX;
        }
    };

    class combo_box_ex : public ctrl<combo_box_ex>
    {
    public:

        std::map<std::wstring, int> path2index;
        image_list imageList;
        int w = 0;
        int h = 0;

        combo_box_ex()
        {}

        void img_size(int x, int y)
        {
            w = x;
            h = y;
        }

        int load_image(const wchar_t* path)
        {
            if (path2index.count(path) != 0)
            {
                return path2index[path];
            }

            HBITMAP bmp = nullptr;
            if(w == 0)
            {
                bmp = the_bitmap_cache().get(path);
                BITMAP bm;
                ::GetObject(bmp, sizeof(bm), &bm);
                w = bm.bmWidth;
                h = bm.bmHeight;
            }
            else
            {
                bmp = the_bitmap_cache().get(path,w,h);
            }

            if (!*imageList)
            {
                imageList.create(w, h);
                send_msg(CBEM_SETIMAGELIST, 0, (LPARAM)*imageList);
            }

            int idx = imageList.add_bitmap(bmp);
            path2index[path] = idx;
            return idx;
        }

        void add(int cmd, const std::wstring& label, const std::wstring& path, int index = -1)
        {
            COMBOBOXEXITEM cbix;
            ::ZeroMemory(&cbix, sizeof(cbix));
            cbix.mask = CBEIF_IMAGE | CBEIF_LPARAM| CBEIF_SELECTEDIMAGE;
            if (!label.empty())
            {
                cbix.mask |= CBEIF_TEXT;
                cbix.pszText = (wchar_t*)label.c_str();
            }
            cbix.iItem = index;
            cbix.lParam = cmd;
            cbix.iImage = load_image(path.c_str());
            cbix.iSelectedImage = load_image(path.c_str());
            
            send_msg(CBEM_INSERTITEM, 0,(LPARAM) &cbix);

            HWND cb = (HWND)send_msg(CBEM_GETCOMBOCONTROL, 0, 0);
            
            RECT r;
            ::GetWindowRect(cb, &r);
            r.right = r.left + 120;
            r.bottom = r.top + 300;
            ::SetWindowPos(cb, 0, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
        }

        LRESULT selected()
        {
            return send_msg(CB_GETCURSEL, 0, 0);
        }

        combo_box_ex& select(int idx)
        {
            send_msg(CB_SETCURSEL, idx, 0);
            return *this;
        }

        int count()
        {
            return (int)send_msg(CB_GETCOUNT, 0, 0);
        }

    };


    class tool_bar;

    template<>
    class window_class<tool_bar>
    {
    public:
        const wchar_t* name()
        {
            return TOOLBARCLASSNAME;
        }
    };

    class tool_bar : public ctrl<tool_bar>
    {
    protected:
        image_list imageList_;
        std::map<size_t, size_t> cmd2img_;
//        std::map<size_t, size_t> rindex_;
 //       std::map<size_t, size_t> index_;
        std::map<int, int> cmd2pos_;
        int wPadding = 5;
        int hPadding = 5;
        int w = 32;
        int h = 32;

    public:

        event<void(int cmd)> onBar;
        event<void(int id, NMTOOLBAR*)> onBarNotify;
        event<int()> onRightClick;

        tool_bar& add_button(int iCmd, const wchar_t* label = 0, BYTE style = 0, BYTE state = TBSTATE_ENABLED, DWORD_PTR data = 0)
        {
            if (!label)
            {
                label = gui().label(iCmd).c_str();
            }
            size_t index = getBitmapIndex(iCmd);
            TBBUTTON tbButton;
            tbButton.dwData = data;
            tbButton.fsState = state;
            tbButton.fsStyle = style;
            tbButton.iBitmap = (int)index;
            tbButton.idCommand = iCmd;
            tbButton.iString = (INT_PTR)label;

            send_msg(TB_ADDBUTTONS, (WPARAM)1, (LPARAM)&tbButton);
            cmd2pos_[iCmd] = count() - 1;
            return *this;
        }


        tool_bar& add_text_button(int iCmd, const wchar_t* label = 0, BYTE style = 0, BYTE state = TBSTATE_ENABLED, DWORD_PTR data = 0)
        {
            TBBUTTON tbButton;
            tbButton.dwData = data;
            tbButton.fsState = state;
            tbButton.fsStyle = style;
            tbButton.iBitmap = 0;
            tbButton.idCommand = iCmd;
            tbButton.iString = (INT_PTR)label;

            send_msg(TB_ADDBUTTONS, (WPARAM)1, (LPARAM)&tbButton);
            cmd2pos_[iCmd] = count() - 1;
            return *this;
        }

        tool_bar& add_separator(int width, int iCmd = 0)
        {
            TBBUTTON tbButton;
            tbButton.dwData = 0;
            tbButton.fsState = 0;
            tbButton.fsStyle = TBSTYLE_SEP;
            tbButton.iBitmap = width;
            tbButton.idCommand = iCmd;
            tbButton.iString = (INT_PTR)0;

            send_msg(TB_ADDBUTTONS, (WPARAM)1, (LPARAM)&tbButton);
            return *this;
        }

        tool_bar& add_control(HWND child, RECT& r, int iCmd = 0)
        {
            add_separator(r.right - r.left, iCmd);

            ::SetParent(child, handle);
            ::SetWindowPos(
                child,
                0,
                r.left + wPadding,
                r.top + hPadding,
                r.right - r.left - wPadding,
                r.bottom - r.top - hPadding,
                SWP_NOZORDER | SWP_NOACTIVATE
            );
            cmd2pos_[iCmd] = count() - 1;
            return *this;
        }


        tool_bar& padding(int w, int h)
        {
            wPadding = w;
            hPadding = h;

            TBMETRICS tbm;
            tbm.cbSize = sizeof(tbm);
            tbm.cxBarPad = 0;
            tbm.cxButtonSpacing = 0;
            tbm.cxPad = w;
            tbm.cyBarPad = 0;
            tbm.cyButtonSpacing = 0;
            tbm.cyPad = h;
            tbm.dwMask = TBMF_PAD;

            send_msg(TB_SETMETRICS, 0, (LPARAM)&tbm);
            return *this;
        }


        tool_bar& spacing(int w, int h)
        {
            TBMETRICS tbm;
            tbm.cbSize = sizeof(tbm);
            tbm.cxBarPad = 0;
            tbm.cxButtonSpacing = w;
            tbm.cxPad = 0;
            tbm.cyBarPad = 0;
            tbm.cyButtonSpacing = h;
            tbm.cyPad = 0;
            tbm.dwMask = TBMF_BUTTONSPACING;

            send_msg(TB_SETMETRICS, 0, (LPARAM)&tbm);
            return *this;
        }

        tool_bar& indent(int w)
        {
            send_msg(TB_SETINDENT, w, 0);
            return *this;
        }

        LRESULT state(int cmd)
        {
            return send_msg(TB_GETSTATE, (WPARAM)cmd, 0);
        }

        LRESULT state(int cmd, int state)
        {
            return send_msg(TB_SETSTATE, (WPARAM)cmd, (LPARAM)state);
        }


        LRESULT check(int cmd, bool set = true)
        {
            LRESULT old = state(cmd);
            LRESULT s = old;
            if (set)
            {
                s = s | TBSTATE_CHECKED;
            }
            else
            {
                s = s & ~TBSTATE_CHECKED;
            }
            state(cmd, (int)s);
            return old;
        }

        BYTE button_style(int id)
        {
            TBBUTTONINFO tbbi;
            ::ZeroMemory(&tbbi, sizeof(tbbi));
            tbbi.cbSize = sizeof(tbbi);
            tbbi.dwMask = TBIF_STYLE;
            send_msg(TB_GETBUTTONINFO, (WPARAM)id, (LPARAM)&tbbi);
            return tbbi.fsStyle;
        }

        BYTE button_style(int id, BYTE style)
        {
            TBBUTTONINFO tbbi;
            ::ZeroMemory(&tbbi, sizeof(tbbi));
            tbbi.cbSize = sizeof(tbbi);
            tbbi.dwMask = TBIF_STYLE;
            tbbi.fsStyle = style;
            send_msg(TB_SETBUTTONINFO, (WPARAM)id, (LPARAM)&tbbi);
            return tbbi.fsStyle;
        }

        int count()
        {
            return (int)send_msg(TB_BUTTONCOUNT, 0, 0);
        }

        void clear()
        {
            while (count())
            {
                send_msg(TB_DELETEBUTTON, 0, 0);
            }
        }

        int width()
        {
            int c = count();
            return (c * (wPadding + w));
        }

        RECT button_rect(int iCmd)
        {
            RECT r = { 0,0,0,0 };
            send_msg(TB_GETRECT, iCmd, (LPARAM)&r);
            return r;
        }

        void get_button(int idx, TBBUTTON* tb)
        {
            send_msg(TB_GETBUTTON, idx, (LPARAM)tb);
        }

        void get_button_by_cmd(int iCmd, TBBUTTON* tb)
        {
            int pos = cmd2pos_[iCmd];
            send_msg(TB_GETBUTTON, pos, (LPARAM)tb);
        }


    protected:

        virtual LRESULT wm_command(int id, int code) override
        {
            onCommand.fire(id);
            onBar.fire(id);
            return 0;
        }

        virtual LRESULT wm_notify(int id, NMHDR* nmhdr) override
        {
            if (nmhdr->code == TBN_DROPDOWN)
            {
                NMTOOLBAR* nmtb = (NMTOOLBAR*)nmhdr;
                onBarNotify.fire(nmtb->iItem, nmtb);
            }
            else if (nmhdr->code == NM_RCLICK)
            {
                NMMOUSE* mmouse = (LPNMMOUSE)nmhdr;
                int id = (int) mmouse->dwItemSpec;
                if (id != -1)
                {
                    onRightClick.fire(id);
                }
            }
            else
            {
                onNotify.fire(nmhdr->code, nmhdr);
            }
            return 0;
        }


        virtual LRESULT wm_custom_draw(NMCUSTOMDRAW* cd) override
        {
            NMTBCUSTOMDRAW* tbcd = (NMTBCUSTOMDRAW*)cd;

            if (tbcd->nmcd.dwDrawStage == CDDS_PREPAINT)
            {
                HDC hdc = tbcd->nmcd.hdc;
                dc dcv(hdc);
                if(colorTheme->font())
                    dcv.select(colorTheme->font());
                dcv.fill_rect(tbcd->nmcd.rc, colorTheme->bkg_brush());
                return TBCDRF_USECDCOLORS  //| TBCDRF_NOEDGES //TBCDRF_NOOFFSET | TBCDRF_NOBACKGROUND | TBCDRF_NOMARK
                    | CDRF_NEWFONT | CDRF_NOTIFYITEMDRAW; //| CDRF_NOTIFYSUBITEMDRAW
                    //| TBCDRF_HILITEHOTTRACK;
                    //|CDRF_NOTIFYPOSTPAINT;// | CDRF_DOERASE | CDRF_SKIPPOSTPAINT;
            }
            if ((tbcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT))
            {
                HDC hdc = tbcd->nmcd.hdc;
                dc dcv(hdc);
                if (colorTheme->font())
                    dcv.select(colorTheme->font());

                UINT state = tbcd->nmcd.uItemState;

                if (state & CDIS_CHECKED)
                {
                    dcv.fill_rect(tbcd->nmcd.rc, colorTheme->selected_bkg_brush());
                }
                else
                {
                    dcv.fill_rect(tbcd->nmcd.rc, colorTheme->bkg_brush());
                }
                dcv.set_text_color(colorTheme->text_color());
                tbcd->clrText = colorTheme->text_color();
                tbcd->clrHighlightHotTrack = colorTheme->bkg_color();
                tbcd->clrBtnHighlight = colorTheme->text_color();
                tbcd->clrMark = colorTheme->bkg_color();
                tbcd->clrBtnFace = colorTheme->bkg_color();
                tbcd->clrTextHighlight = colorTheme->bkg_color();
                tbcd->hbrMonoDither = colorTheme->bkg_brush();
                tbcd->hbrLines = colorTheme->bkg_brush();

                LRESULT result = //TBCDRF_NOOFFSET | TBCDRF_NOEDGES | TBCDRF_NOMARK 
                    CDRF_NEWFONT | TBCDRF_USECDCOLORS | TBCDRF_HILITEHOTTRACK;

                if (state & CDIS_CHECKED)
                {
                    result |= TBCDRF_BLENDICON;
                }
                return result;
            }
            return CDRF_DODEFAULT;
        }

        size_t getBitmapIndex(int id)
        {
            if (cmd2img_.count(id) == 0)
            {
                auto bmp = the_bitmap_cache().get(id, w, h);

                if (bmp)
                {
                    int idx = imageList_.add_bitmap(bmp);
                    cmd2img_[id] = idx;
                }
            }

            if (cmd2img_.count(id) == 0)
            {
                return -1;
            }
            return cmd2img_[id];
        }

        virtual HWND create_window(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) override
        {
            auto& wndClass = wc<tool_bar>();

            handle = ::CreateWindowEx(
                0,
                wndClass.name(),
                title,
                WS_CHILD,
                r.left, r.top, r.right - r.left, r.bottom - r.top,
                parent,
                menu,
                module_instance(),
                (LPVOID)this
            );

            subclass();

            send_msg(TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

            w = r.right - r.left;
            h = r.bottom - r.top;

            send_msg(TB_SETBITMAPSIZE, 0, MAKELPARAM(w, h));

            imageList_.create(w, h);
            send_msg(TB_SETIMAGELIST,0,(LPARAM)*imageList_);

            show(SW_SHOW);

            send_msg(TB_SETSTYLE, 0, style);
            send_msg(TB_SETEXTENDEDSTYLE, 0, exStyle);

            wm_create();
            return handle;
        }

    };

    class rebar;

    template<>
    class window_class<rebar>
    {
    public:
        const wchar_t* name()
        {
            return REBARCLASSNAME;
        }
    };

    class rebar : public ctrl<rebar>
    {
    public:

        int count()
        {
            return (int) send_msg(RB_GETBANDCOUNT, 0, 0);
        }

        void remove(int idx)
        {
            send_msg(RB_DELETEBAND, idx, 0);
        }

        int id(int pos)
        {
            REBARBANDINFO  rbi;
            rbi.cbSize = sizeof(REBARBANDINFO);
            rbi.fMask = RBBIM_ID;
            getBar(pos, &rbi);
            return rbi.wID;
        }

        int index(int cmd)
        {
            int cnt = count();
            for (int i = 0; i < cnt; i++)
            {
                if (id(i) == cmd)
                {
                    return i;
                }
            }
            return -1;
        }

        void show(int idx, bool show = true)
        {
            send_msg(RB_SHOWBAND, idx, show);
        }

        void getBar(int idx, REBARBANDINFO* rbi)
        {
            send_msg(RB_GETBANDINFO, idx, (LPARAM)rbi);
        }

        void addBar(const wchar_t* label, int id, HWND child, int height = -1, int style = RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS | RBBS_BREAK)
        {
            REBARBANDINFO rbBand = { sizeof(REBARBANDINFO) };
            rbBand.fMask =
                RBBIM_STYLE         // fStyle is valid.
                | RBBIM_TEXT        // lpText is valid.
                | RBBIM_CHILD       // hwndChild is valid.
                | RBBIM_CHILDSIZE   // child size members are valid.
                | RBBIM_ID
                | RBBIM_SIZE;       // cx is valid
            rbBand.fStyle = style;
            // RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS;
            // RBBS_NOGRIPPER;//|RBBS_CHILDEDGE;//|RBS_BANDBORDERS

            rbBand.lpText = (wchar_t*)label;
            rbBand.wID = id;

            RECT r;
            ::GetWindowRect(child, &r);

            int width = r.right - r.left;

            if (height == -1)
            {
                height = r.bottom - r.top;

                wbuff buf(1024);
                size_t n = ::GetClassName(child, buf, (int) buf.size());
                std::wstring wcn = buf.toString(n);
                if (wcn == TOOLBARCLASSNAME)
                {
                    RECT r = { 0,0,0,0 };
                    SIZE s = { 0,0 };
                    ::SendMessage(child, TB_GETITEMRECT, 0, (LPARAM)&r);
                    height = r.bottom - r.top;
                }
            }

            rbBand.hwndChild = child;
            rbBand.cxMinChild = 120;// width;// r.right - r.left;
            rbBand.cyMinChild = height;// r.bottom - r.top;
            rbBand.cx = r.right - r.left;
            rbBand.cxIdeal = r.right - r.left;

            send_msg(RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
        }

        void setColors(COLORREF clrBkg, COLORREF clrFore)
        {
            send_msg(RB_SETBKCOLOR, 0, (LPARAM)clrBkg);
            send_msg(RB_SETTEXTCOLOR, 0, (LPARAM)clrFore);
        }

        void setShadowColors(COLORREF shadow, COLORREF highlite)
        {
            COLORSCHEME cs;
            ::ZeroMemory(&cs, sizeof(cs));
            cs.dwSize = sizeof(cs);
            cs.clrBtnHighlight = highlite;
            cs.clrBtnShadow = shadow;
            send_msg(RB_SETCOLORSCHEME, 0, (LPARAM)&cs);
        }

        void setBarColors(int index, COLORREF clrBkg, COLORREF clrFore)
        {
            REBARBANDINFO rbBand = { sizeof(REBARBANDINFO) };

            rbBand.fMask = RBBIM_COLORS;

            rbBand.clrBack = clrBkg;
            rbBand.clrFore = clrFore;

            LRESULT result = send_msg(RB_SETBANDINFO, (WPARAM)index, (LPARAM)&rbBand);
        }

    protected:

        virtual void onColorThemeChanged()
        {
            if (!colorTheme || !colorTheme->enabled())
            {
                ::SetWindowTheme(handle, NULL, NULL);
                return;
            }

            ::SetWindowTheme(handle, L"", L"");
            setColors(colorTheme->bkg_color(), colorTheme->text_color());
            setShadowColors(colorTheme->bkg_color(), colorTheme->bkg_color());
        }

        virtual HWND create_window(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) override
        {
            auto& wndClass = wc<rebar>();

            handle = ::CreateWindowEx(
                exStyle,
                wndClass.name(),
                title,
                style,
                r.left, r.top, r.right - r.left, r.bottom - r.top,
                parent,
                menu,
                module_instance(),
                (LPVOID)this
            );

            // NO? 
            subclass();
            show(SW_SHOW);
            wm_create();
            return handle;
        }
    };


    class list_view;

    template<>
    class window_class<list_view>
    {
    public:
        const wchar_t* name()
        {
            return WC_LISTVIEW;
        }
    };

    class list_view : public ctrl<list_view>
    {
    public:

        mtl::event<void(NMITEMACTIVATE*)> onActivate;
        mtl::event<void(NMITEMACTIVATE*)> onClick;
        mtl::event<void(int id, std::wstring, bool&)> onLabelEdited;

        mtl::sink<void(int, int, std::wstring)> with;

        class data_src
        {
        public:

            class data_cell
            {
            public:
                int id = 0;
                int subitem = 0;
                std::wstring value;

                data_cell& operator=(const std::wstring& str)
                {
                    value = str;
                    onChange.fire(id, subitem, value);
                    return *this;
                }

                mtl::event<void(int, int, std::wstring)> onChange;
            };

            class data_row 
            {
            public:
                int id = 0;
                std::vector<data_cell> cells;

                data_cell& operator[](int id)
                {
                    return cells[id];
                }
            };

            std::vector<std::wstring> headers;
            std::vector<data_row> rows;

            mtl::event<void(int, int, std::wstring)> onChange;

            mtl::sink<void(int id, std::wstring, bool&)> with;

            data_row& operator[](int id)
            {
                return rows[id];
            }

            data_src()
            {}

            data_src(const std::vector<std::wstring>& h)
                : headers(h)
            {}

            data_src(const std::vector<std::wstring>& h, const std::vector<std::vector<std::wstring>>& data)
                : headers(h)
            {
                for (size_t i = 0; i < data.size(); i++)
                {
                    std::vector<data_cell> items;
                    for (size_t j = 0; j < data[i].size(); j++)
                    {
                        data_cell d((int)i, (int)j, data[i][j]);
                        d.onChange([this](int id, int subitem, std::wstring value) 
                        {
                            onChange.fire(id, subitem, value);
                        });
                        items.push_back(d);
                    }
                    rows.push_back(data_row( (int)i,items));
                }
            }

            data_src& set(const std::vector<std::wstring>& h, const std::vector<std::vector<std::wstring>>& data)
            {
                headers = h;
                rows.clear();
                for (size_t i = 0; i < data.size(); i++)
                {
                    std::vector<data_cell> items;

                    for (size_t j = 0; j < data[i].size(); j++)
                    {
                        data_cell d{ (int)i, (int)j, data[i][j] };
                        d.onChange([this](int id, int subitem, std::wstring value)
                        {
                            onChange.fire(id, subitem, value);
                        });                       
                        items.push_back(d);
                    }
                    data_row dr{ (int)i, items };
                    rows.push_back(dr);
                }
                return *this;
            }

            void attach(list_view& listview)
            {
                with.clear();
                listview.with.clear();
                listview.clear();

                listview.add_columns(headers);
                for (auto& row : rows)
                {
                    listview.add_row(row);
                }

                listview
                    .with(onChange)
                    .then( [this,&listview] (int id, int subitem, std::wstring value) 
                {
                    listview.set_item(id, value, subitem);
                });

                with(listview.onLabelEdited)
                    .then( [this] (int id, std::wstring txt, bool& accept)
                {
                    rows[id].cells[0].value = txt;
                    accept = true;
                });
            }
        };

        list_view& add_column(int index, const std::wstring& txt)
        {
            LVCOLUMN lvc;
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; //

            lvc.iOrder = 0;
            lvc.iSubItem = 0;
            lvc.pszText = (wchar_t*)txt.c_str();
            lvc.cx = 100;
            lvc.fmt = LVCFMT_LEFT;

            int r = ListView_InsertColumn(handle, index, &lvc);

            column_count_++;
            return *this;
        }

        list_view& add_columns(const std::vector<std::wstring>& columns)
        {
            if (column_count_) return *this;

            int i = 0;
            for (auto& col : columns)
            {
                add_column(i,col);
                i++;
            }
            return *this;
        }

        int count()
        {
            return (int)ListView_GetItemCount(handle);
        }

        int selected(int i = -1, int f = LVNI_SELECTED)
        {
            return (int)ListView_GetNextItem(handle, i, f);
        }

        int selected_count()
        {
            return (int)ListView_GetSelectedCount(handle);
        }

        bool is_checked(int id)
        {
            return ListView_GetCheckState(handle,id);
        }

        void set_checked(int id, bool status)
        {
            ListView_SetCheckState(handle,id,status);
        }

        std::wstring get_item(int i, int subItem = 0)
        {
            mtl::wbuff buf(1024);

            LVITEM lvI;
            ::ZeroMemory(&lvI, sizeof(lvI));
            lvI.mask = LVIF_TEXT;
            lvI.pszText = buf;
            lvI.cchTextMax = (int) buf.size();
            lvI.iSubItem = subItem;
            lvI.iItem = i;

            ListView_GetItem(handle, &lvI);
            return buf.toString();
        }

        list_view& add_item(const std::wstring& item, int index = -1)
        {
            LVITEM lvI;

            if (index == -1) index = count();

            lvI.pszText = (wchar_t*)item.c_str();// LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
            lvI.mask = LVIF_TEXT | LVIF_STATE; //LVIF_IMAGE
            lvI.stateMask = 0;
            lvI.iSubItem = 0;
            lvI.state = 0;

            lvI.iItem = index;
            lvI.iImage = 0;

            int r = ListView_InsertItem(handle, &lvI);
            return *this;
        }


        list_view& set_item(int index ,const std::wstring& item, int subitem = 0)
        {
            LVITEM lvI;

            if (index == -1) index = count()-1;

            lvI.pszText = (wchar_t*)item.c_str();// LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
            lvI.mask = LVIF_TEXT | LVIF_STATE; //LVIF_IMAGE
            lvI.stateMask = 0;
            lvI.iSubItem = subitem;
            lvI.state = 0;

            lvI.iItem = index;
            lvI.iImage = 0;

            BOOL b = ListView_SetItem(handle, &lvI);
            return *this;
        }



        list_view& set_param(int index, LPARAM lParam)
        {
            LVITEM lvI;

            if (index == -1) index = count() - 1;

            lvI.pszText = 0;// (wchar_t*)item.c_str();// LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
            lvI.mask = LVIF_PARAM; //LVIF_IMAGE
            lvI.stateMask = 0;
            lvI.iSubItem = 0;
            lvI.state = 0;

            lvI.iItem = index;
            lvI.iImage = 0;
            lvI.lParam = lParam;

            ListView_SetItem(handle, &lvI);
            return *this;
        }


        LPARAM get_param(int index)
        {        
            LVITEM lvI;

            if (index == -1) index = count() - 1;

            lvI.pszText = 0;// (wchar_t*)item.c_str();// LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
            lvI.mask = LVIF_PARAM; //LVIF_IMAGE
            lvI.stateMask = 0;
            lvI.iSubItem = 0;
            lvI.state = 0;

            lvI.iItem = index;
            lvI.iImage = 0;
            lvI.lParam = 0;

            ListView_GetItem(handle, &lvI);
            return lvI.lParam;
        }

        list_view& add_row(const std::vector<std::wstring>& items)
        {
            if (items.empty()) return *this;

            add_item(items[0]);

            for (size_t i = 1; i < items.size(); i++)
            {
                set_item(-1, items[i], (int) i);
            }

            return *this;
        }

        list_view& add_row(const data_src::data_row& row)
        {
            if (row.cells.empty()) return *this;

            add_item(row.cells[0].value);// , row.cells[0].id);

            for (size_t i = 1; i < row.cells.size(); i++)
            {
                set_item(-1, row.cells[i].value, (int)i);
            }

            return *this;
        }

        list_view& clear()
        {
            BOOL b = ListView_DeleteAllItems(handle);
            return *this;
        }


    protected:

        int column_count_ = 0;

        virtual LRESULT wm_notify(int id, NMHDR* nmhdr) override
        {
            onNotify.fire(nmhdr->code, nmhdr);

            if (nmhdr->code == LVN_BEGINLABELEDIT)
            {
                return FALSE;
            }
            if (nmhdr->code == LVN_ENDLABELEDIT)
            {
                NMLVDISPINFO* nmdisp = (NMLVDISPINFO*)nmhdr;
                std::wstring ws = nmdisp->item.pszText;

                bool accept = true;
                onLabelEdited.fire(nmdisp->item.iItem, ws, accept);

                if (accept) return TRUE;
                return FALSE;
            }
            if (nmhdr->code == LVN_ITEMACTIVATE)
            {
                NMITEMACTIVATE* nma = (NMITEMACTIVATE*)nmhdr;
                onActivate.fire(nma);
            }
            if (nmhdr->code == NM_CLICK)
            {
                NMITEMACTIVATE* nma = (NMITEMACTIVATE*)nmhdr;
                onClick.fire(nma);
            }
            return 0;
        }
        
        virtual HWND create_window(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) override
        {
            auto& wndClass = wc<list_view>();

            handle = ::CreateWindowEx(
                0,
                wndClass.name(),
                title,
                style,
                r.left, r.top, r.right - r.left, r.bottom - r.top,
                parent,
                menu,
                module_instance(),
                (LPVOID)this
            );

            // NO? 
            subclass();
            show(SW_SHOW);
            wm_create();

            ListView_SetExtendedListViewStyle(handle, exStyle);
                // LVS_EX_AUTOSIZECOLUMNS | LVS_EX_FULLROWSELECT);// | LVS_EX_GRIDLINES);

            return handle;
        }
        
    };
}

