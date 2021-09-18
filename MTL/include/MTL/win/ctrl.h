#pragma once

#include "mtl/win/wind.h"
#include "mtl/ole/img.h"
#include "mtl/ole/dataobj.h"

#include <commctrl.h>
#include <Uxtheme.h>

namespace MTL {

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
    class Ctrl : public Wnd
    {
    public:

        Event<int()> onCommand;
        Event<UINT(NMHDR*)> onNotify;

        ~Ctrl()
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

                    return this->wmCommand(wmId, code);
                }
                if (msg->message == WM_NOTIFY)
                {
                    NMHDR* nmhdr = (NMHDR*)(msg->lParam);

                    if (colorTheme && colorTheme->enabled() && (nmhdr->code == NM_CUSTOMDRAW))
                    {
                        NMCUSTOMDRAW* cd = (NMCUSTOMDRAW*)nmhdr;
                        return this->wmCustomDraw(cd);
                    }

                    return this->wmNotify((int)msg->wParam, nmhdr);
                }
                if (msg->message == WM_DRAWITEM)
                {
                    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)msg->lParam;
                    return this->wmDrawItem(lpDIS);
                }
                return 0;
            }

            switch (message)
            {
                case WM_SIZE:
                {
                    RECT cr = getClientRect();
                    wmSize(cr);
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
                    break;
                }
            }
            return ::CallWindowProc(oldProc_, hwnd, message, wParam, lParam);
        }
         
        //  subclassing the def wndproc
        void subClass()
        {
            ::SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)dynamic_cast<void*>((W*)this));
            WNDPROC proc = (WNDPROC)(::SetWindowLongPtr(handle, GWLP_WNDPROC, (LONG_PTR)(windowProcedure)));

            if (proc != windowProcedure)
            {
                oldProc_ = proc;
            }
        }

        void subClass(HWND hwnd)
        {
            handle = hwnd;
            subClass();
        }

        void deClass()
        {
            if (oldProc_)
            {
                ::SetWindowLongPtr(handle, GWLP_WNDPROC, (LONG_PTR)(oldProc_));
            }
        }

    protected:

        virtual LRESULT wmCommand(int id, int code)
        {
            onCommand.fire(code);
            return 0;
        }

        virtual LRESULT wmNotify(int id, NMHDR* nmhdr)
        {
            onNotify.fire(nmhdr->code, nmhdr);
            return 0;
        }

        virtual LRESULT wmCreate() override
        {
            return 0;
        }

        virtual LRESULT wmSize(RECT& rc) override
        {
            return 0;
        }

        virtual LRESULT wmCustomDraw(NMCUSTOMDRAW* cd)
        {
            return 0;
        }

        virtual LRESULT wmDrawItem(LPDRAWITEMSTRUCT dis)
        {
            return 0;
        }

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

            subClass();
            wmCreate();
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

    class Button;

    template<>
    class WindowClass<Button>
    {
    public:
        const wchar_t* name()
        {
            return L"Button";
        }
    };

    class Button : public Ctrl<Button>
    {
    public:

        void setBitmap(HBITMAP bmp)
        {
            sendMsg(BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp);
        }

        bool checked()
        {
            return BST_CHECKED == sendMsg(BM_GETCHECK, 0, 0);
        }

        void check(bool b = true)
        {
            sendMsg(BM_SETCHECK, b ? BST_CHECKED : BST_UNCHECKED, 0);
        }

    protected:

        virtual LRESULT wmCommand(int id, int code) override
        {
            onCommand.fire(id);
            return 0;
        }
    };


    class EditCtrl;

    template<>
    class WindowClass<EditCtrl>
    {
    public:
        const wchar_t* name()
        {
            return L"Edit";
        }
    };

    class EditCtrl : public Ctrl<EditCtrl>
    {};

    class StatusBar;

    template<>
    class WindowClass<StatusBar>
    {
    public:
        const wchar_t* name()
        {
            return STATUSCLASSNAME;
        }
    };

    class StatusBar : public Ctrl<StatusBar>
    {
    public:
    };


    class Tooltip;

    template<>
    class WindowClass<Tooltip>
    {
    public:
        const wchar_t* name()
        {
            return TOOLTIPS_CLASS;
        }
    };

    class Tooltip : public Ctrl<Tooltip> 
    {
    public:

        template<class T>
        LRESULT addTool(T& ctrl, const std::wstring& txt)
        {
            TOOLINFO ti;
            RECT r;
            ::GetClientRect(*ctrl, &r);

           ctrl.deClass();

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

            LRESULT res = sendMsg(TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
            ctrl.subClass();
            return res;
        }

        template<class T>
        LRESULT addTool(HWND notifyParent, T& ctrl)
        {
            TOOLINFO ti;
            RECT r;
            ::GetClientRect(*ctrl, &r);

            ctrl.deClass();

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

            LRESULT res = sendMsg(TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
            ctrl.subClass();
            return res;
        }

    protected:

        virtual HWND createWindow(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) override
        {
            auto& wc = windowClass<Tooltip>();

            handle = ::CreateWindowEx(
                exStyle,
                wc.name(),
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
            subClass();
            return handle;
        }
    };

    class ImageList
    {
    public:

        int width = 24;
        int height = 24;

        ImageList()
        {}

        ImageList(int w, int h, int style = ILC_COLOR32 | ILC_MASK, int n = 1, int max = 255)
            : width(w), height(h)
        {
            create(w, h, style, n, max);
        }

        ImageList& create(int w, int h, int style = ILC_COLOR32 | ILC_MASK, int n = 1, int max = 255)
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

        ~ImageList()
        {
            ::ImageList_Destroy(himl_);
            himl_ = nullptr;
        }

        int addBitmap(HBITMAP bmp, COLORREF col = RGB(0, 0, 0))
        {
            return ::ImageList_AddMasked(
                himl_,
                bmp,
                col
            );
        }

        int addIcon(HICON ico)
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

        ImageList(const ImageList& rhs) = delete;
        ImageList(ImageList&& rhs) = delete;

        ImageList& operator=(const ImageList& rhs) = delete;
        ImageList& operator=(ImageList&& rhs) = delete;

    private:

        HIMAGELIST himl_ = nullptr;
    };



    class TabControl;

    template<>
    class WindowClass<TabControl>
    {
    public:
        const wchar_t* name()
        {
            return WC_TABCONTROL;
        }
    };

    class TabControl : public Ctrl<TabControl>
    {
    public:

        class TabCtrlItem
        {
        public:
            TabCtrlItem()
                : title(L""), lparam(0)
            {}

            TabCtrlItem(const std::wstring& t)
                : title(t), lparam(0)
            {}

            TabCtrlItem(const std::wstring& t, const std::wstring& tt)
                : title(t), tooltip(tt), lparam(0)
            {}

            TabCtrlItem(const std::wstring& t, const std::wstring& tt, const std::wstring& i, int idx = -1, LPARAM p = 0)
                : title(t), tooltip(tt), id(i), index(idx),lparam(p)
            {}

            std::wstring title;
            std::wstring tooltip;
            std::wstring id;
            int index = -1;
            HWND hWnd = nullptr;
            LPARAM lparam = 0;
        };

        MTL::Event<void(TabCtrlItem&)>                                  onSelect;
        MTL::Event<void(MTL::TabControl::TabCtrlItem&, IDataObject*)>   onPopulateDataObj;
        MTL::Event<void(MTL::TabControl::TabCtrlItem&)>                 onDrag;
        MTL::Event<void(std::wstring)>                                  onDragOut;
        MTL::Event<void(IDataObject*, DWORD, DWORD&)>                   onDrop;
        MTL::Event<void(int, IDataObject*)>                             onDropExternal;
        MTL::Event<void(std::wstring)>                                  onCloseTab;

        HWND                                                            activeChild = nullptr;
        CLIPFORMAT			                                            dragTabFormat;
        MTL::punk<DefaultDropTarget>                                    dropTarget;
        MTL::Tooltip                                                    tooltip;
        MTL::ImageList			                                        imageList;

        TabControl()
        {
        }

        virtual ~TabControl() {}

        HWND setChild(HWND newChild)
        {
            HWND old = activeChild;
            if (::IsWindow(old))
            {
                ::ShowWindow(old, SW_HIDE);
            }
            activeChild = newChild;
            ::ShowWindow(activeChild, SW_SHOW);
            postMsg(WM_LAYOUT, 0, 0);
            return old;
        }

        void enableIcon(HICON icon, int w = 16, int h = -1)
        {
            if (h == -1) h = w;
            imageList.create(w, h);
            sendMsg(TCM_SETIMAGELIST, 0, (LPARAM)*imageList);
            iconIndex = imageList.addIcon(icon);

        }

        void enableDragDrop()
        {
            dragTabFormat = ::RegisterClipboardFormat(L"MTLTabDragDropCustomClipBoardFormat");
            dropTarget = MTL::dropTarget(dragTabFormat);

            dropTarget->onDrop([this](IDataObject* ido, DWORD keyState, DWORD& effect)
            {
                onDrop.fire(ido, keyState, effect);
            });

            sendMsg(TCM_SETEXTENDEDSTYLE, TCS_EX_REGISTERDROP, TCS_EX_REGISTERDROP);

            onNotify(TCN_GETOBJECT, [this](NMHDR* nmhdr)
            {
                wmObjectNotify((NMOBJECTNOTIFY*)nmhdr);
            });

            onPopulateDataObj([this](MTL::TabControl::TabCtrlItem& c, IDataObject* ido)
            {
            });

            onDrag([this](MTL::TabControl::TabCtrlItem& c)
            {
                MTL::format_etc fe(dragTabFormat);
                DWORD effect = 0;

                auto obj = dataObject(fe, c.id);

                onPopulateDataObj.fire(c, *obj);

                HRESULT hr = ::DoDragDrop(
                    *obj,
                    *MTL::dropSource(),
                    //DROPEFFECT_COPY | 
                    DROPEFFECT_MOVE,
                    &effect
                );

                if (effect == DROPEFFECT_NONE)
                {
                    POINT pt;
                    ::GetCursorPos(&pt);
                    RECT r;
                    ::GetWindowRect(getParent(), &r);
                    if (!::PtInRect(&r, pt))
                    {
                        onDragOut.fire(c.id);
                    }
                };
            });

            onDrop([this](IDataObject* ido, DWORD keyState, DWORD& effect)
            {
                MTL::dataobj_view dv(ido);
                std::wstring id = dv.wstring(dragTabFormat);

                int index_to = hitTest();
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
                int index_from = id2index(id);
                if (index_from == -1)
                {
                    onDropExternal.fire(index_to, ido);
                    effect = DROPEFFECT_MOVE;
                    return;
                }
                TabCtrlItem* tci = getTabCtrlItem(index_from);
                removeItem(index_from, false);
                insertItem(tci, index_to);

                clearHighlite();
                select(index_to);
                onSelect.fire(*tci);
                //highlite(cur);
            });
        }


        void add(TabCtrlItem&& item, int index = -1)
        {
            TabCtrlItem* tci = new TabCtrlItem(item);
            int idx = (int) insertItem(tci, index);
            select(idx);
            postMsg(WM_LAYOUT, 0, 0);
        }

        void add(TabCtrlItem&& item, HWND child, int index = -1)
        {
            TabCtrlItem* tci = new TabCtrlItem(item);
            tci->hWnd = child;
            int idx = (int)insertItem(tci, index);
            select(idx);
            postMsg(WM_LAYOUT, 0, 0);
        }

        TabCtrlItem& item(int index)
        {
            return *getTabCtrlItem(index);
        }

        TabCtrlItem& item(const std::wstring& id)
        {
            return *getTabCtrlItem(id2index(id));
        }

        void clearHighlite()
        {
            // clear highlight bits from tabctrl
            for (int i = 0; i < count(); i++)
            {
                postMsg(TCM_HIGHLIGHTITEM, i, FALSE);
            }
        }

        void highlite(int idx, bool bShow = true)
        {
            postMsg(TCM_HIGHLIGHTITEM, idx, bShow);
        }

        TabCtrlItem remove(int index = -1)
        {
            TabCtrlItem result(*getTabCtrlItem(index));
            removeItem(index, true);
            return result;
        }

        TabCtrlItem remove(const std::wstring& id)
        {
            int index = id2index(id);
            return remove(index);
        }

        int count()
        {
            return (int)sendMsg(TCM_GETITEMCOUNT, 0, 0);
        }

        LRESULT select(int i)
        {
            auto tci = getTabCtrlItem(i);
            if (tci)
            {
                HWND hWnd = tci->hWnd;
                if (::IsWindow(hWnd))
                {
                    setChild(hWnd);
                }
            }
            return sendMsg(TCM_SETCURSEL, (WPARAM)i, 0);
        }

        LRESULT select(const std::wstring& id)
        {
            int index = id2index(id);
            return select(index);
        }

        int selected()
        {
            return (int)TabCtrl_GetCurSel(handle);
        }

        RECT displayRect()
        {
            RECT r = getWindowRect();
            LRESULT res = sendMsg(TCM_ADJUSTRECT, FALSE, (LPARAM)&r);
            return r;
        }

        int hitTest()
        {
            TCHITTESTINFO tchit;
            ::GetCursorPos(&(tchit.pt));
            ::ScreenToClient(handle, &(tchit.pt));
            return TabCtrl_HitTest(handle, &tchit);
        }

        RECT getTabRect(int index)
        {
            RECT r;
            TabCtrl_GetItemRect(handle, index, &r);
            return r;
        }

        bool hitIconTest()
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

                RECT tr = getTabRect(selected());

                tr.right = tr.left + imageList.width;
                tr.bottom = tr.top + imageList.height;

                if (::PtInRect(&tr, tchit.pt))
                {
                    return true;
                }
            }
            return false;
        }

        int id2index(std::wstring id)
        {
            int c = count();
            for (int i = 0; i < c; i++)
            {
                TabCtrlItem* tci = getTabCtrlItem(i);
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
                    wmSize(bounds);
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
                        return wmPaintHandler();
                    }
                    break;
                }
                case WM_ERASEBKGND:
                {
                    return 1;
                }
            }
            return Ctrl<TabControl>::wndProc(hwnd, message, wParam, lParam);
        }

    protected:

        virtual LRESULT wmSize(RECT& clientRect) override
        {
            if (::IsWindow(activeChild))
            {
                RECT tr = displayRect();

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

        virtual LRESULT wmNotify(int id, NMHDR* nmhdr) override
        {
            if (nmhdr->code == TCN_SELCHANGE)
            {
                int curSel = selected();
                if (curSel == -1)
                {
                    return 0;
                }
                TabCtrlItem* tci = getTabCtrlItem(curSel);
                if (tci)
                {
                    onSelect.fire(*tci);
                    if (::IsWindow(tci->hWnd))
                    {
                        setChild(tci->hWnd);
                    }
                }
            }
            if (nmhdr->code == TCN_SELCHANGING)
            {
                return hitIconTest();
            }

            if (nmhdr->code == NM_CLICK)
            {
                if (hitIconTest())
                {
                    int idx = hitTest();
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

        virtual LRESULT wmDrawItem(LPDRAWITEMSTRUCT dis) override
        {
            if (!this->colorTheme) return 0;

            HBRUSH br = nullptr;
            BOOL bSelected = (dis->itemID == (UINT)selected());

            ::SetBkMode(dis->hDC, TRANSPARENT);
            if (!bSelected)
            {
                ::SetTextColor(dis->hDC, this->colorTheme->textColor());
                br = this->colorTheme->bkgBrush();
            }
            else
            {
                ::SetTextColor(dis->hDC, this->colorTheme->selectedTextColor());
                br = this->colorTheme->selectedBkgBrush();
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

        virtual void wmObjectNotify(NMOBJECTNOTIFY* notify)
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

        INT_PTR wmPaintHandler()
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

                ::FrameRect(hDC, &r, colorTheme->bkgBrush());
                r.bottom -= 2;
                if (i == current_item)
                {
                    RECT tmp = r;
                    //tmp.top -= 1;
                    tmp.top += 1;
                    tmp.bottom += 1;

                    ::FrameRect(hDC, &tmp, colorTheme->selectedBkgBrush());
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
 
            ::FillRgn(hDC, hFillRgn, colorTheme->bkgBrush());

            BOOL ok = DeleteObject(hFillRgn);
            ok = DeleteObject(hRgn);
            EndPaint(handle, &ps);
            return 0;
        }

        TabCtrlItem* getTabCtrlItem(int i)
        {
            TCITEM item;
            ZeroMemory(&item, sizeof(item));
            item.mask = TCIF_PARAM;
            sendMsg(TCM_GETITEM, (WPARAM)i, (LPARAM)(const LPTCITEM)(&item));
            return (TabCtrlItem*)(item.lParam);
        }

        LRESULT insertItem(TabCtrlItem* titem, int index = -1)
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

            LRESULT i = sendMsg(TCM_INSERTITEM, (WPARAM)index, (LPARAM)(const LPTCITEM)(&item));

            return i;
        }

        TabCtrlItem* removeItem(int index = -1, bool deleteItem = true)
        {
            TabCtrlItem* selectedItem = getTabCtrlItem(selected());
            TabCtrlItem* tci = (TabCtrlItem*)getTabCtrlItem(index);
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

            int i = hitTest();
            if (i != -1)
            {
                // wait a bit might be a drag-drop
                isMouseDown_ = true;
                timer_.timeout(250, [this]()
                {
                    if (isMouseDown_)
                    {
                        isMouseDown_ = false;
                        int i = hitTest();
                        if (i != -1)
                        {
                            // do a simple dragDrop
                            MTL::TabControl::TabCtrlItem* c = getTabCtrlItem(i);
                            if (c)
                            {
                                onDrag.fire(*c);
                            }
                        }
                    }
                });
            }
        }


        void forEach(std::function<void(TabCtrlItem& tci)> visitor)
        {
            int c = count();
            for (int i = 0; i < c; i++)
            {
                TabCtrlItem* ptci = getTabCtrlItem(i);
                if (ptci)
                {
                    visitor(*ptci);
                }
            }
        }

        virtual void onColorThemeChanged()
        {
            MTL::Wnd::onColorThemeChanged();
            tooltip.setColorTheme(colorTheme);

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

        virtual HWND createWindow(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu)
        {
            auto& wc = windowClass<TabControl>();

            handle = ::CreateWindowEx(
                exStyle,
                wc.name(),
                title,
                style,
                r.left, r.top, r.right - r.left, r.bottom - r.top,
                parent,
                menu,
                module_instance(),
                (LPVOID)this
            );

            subClass();

            RECT rp;
            ::GetClientRect(parent, &r);
            tooltip.create(1, parent, rp, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP);

            tooltip.addTool(parent, *this);

            tooltip.onNotify(TTN_GETDISPINFO, [this](NMHDR* nmhdr)
            {
                HWND hwndFrom = nmhdr->hwndFrom;
                UINT_PTR id = nmhdr->idFrom;
                NMTTDISPINFO* di = (NMTTDISPINFO*)nmhdr;
                di->lpszText = 0;
                di->hinst = 0;
                di->szText[0] = 0;

                int i = hitTest();
                if (i != -1)
                {
                    MTL::TabControl::TabCtrlItem* c = getTabCtrlItem(i);
                    if (c)
                    {
                        di->lpszText = (wchar_t*)(c->tooltip.c_str());
                        di->hinst = 0;
                    }
                }

            });

            return handle;
        }


        MTL::Timer                  timer_;
        std::vector<TabCtrlItem>    items_;
        bool				        isMouseDown_ = false;
        int                         iconIndex = -1;
    };

    class ComboBox;

    template<>
    class MTL::WindowClass<ComboBox>
    {
    public:
        const wchar_t* name()
        {
            return WC_COMBOBOX;
        }
    };

    class ComboBox : public Ctrl<ComboBox>
    {
    public:

        ComboBox& addString(const std::wstring& s, LPARAM param = 0 )
        {
            int r = (int) sendMsg(CB_ADDSTRING, 0, (LPARAM)s.c_str());
            if ( r != CB_ERR && param)
            {
                sendMsg(CB_SETITEMDATA, r, param);
            }
            return *this;
        }

        void width(int w)
        {
            sendMsg(CB_SETDROPPEDWIDTH, w, 0);
        }

        int count()
        {
            return (int)sendMsg(CB_GETCOUNT, 0, 0);
        }

        LRESULT getItemData(int id)
        {
            return sendMsg(CB_GETITEMDATA, 0, 0);
        }

        std::wstring getItemText(int id)
        {
            int n = (int) sendMsg(CB_GETLBTEXTLEN, id, 0);
            MTL::wbuff wbuf(n);
            n = (int) sendMsg(CB_GETLBTEXT, id, 0);
            return wbuf.toString(n);
        }

        LRESULT curSel()
        {
            return sendMsg(CB_GETCURSEL, 0, 0);
        }

        ComboBox& select(int idx)
        {
            sendMsg(CB_SETCURSEL, idx, 0);
            return *this;
        }
    };

    class ComboBoxEx;

    template<>
    class MTL::WindowClass<ComboBoxEx>
    {
    public:
        const wchar_t* name()
        {
            return WC_COMBOBOXEX;
        }
    };

    class ComboBoxEx : public Ctrl<ComboBoxEx>
    {
    public:

        std::map<std::wstring, int> path2index;
        ImageList imageList;
        int w = 0;
        int h = 0;

        ComboBoxEx()
        {}

        void setImgSize(int x, int y)
        {
            w = x;
            h = y;
        }

        int loadImage(const wchar_t* path)
        {
            if (path2index.count(path) != 0)
            {
                return path2index[path];
            }

            HBITMAP bmp = nullptr;
            if(w == 0)
            {
                bmp = bitmapCache().get(path);
                BITMAP bm;
                ::GetObject(bmp, sizeof(bm), &bm);
                w = bm.bmWidth;
                h = bm.bmHeight;
            }
            else
            {
                bmp = bitmapCache().get(path,w,h);
            }

            if (!*imageList)
            {
                imageList.create(w, h);
                sendMsg(CBEM_SETIMAGELIST, 0, (LPARAM)*imageList);
            }

            int idx = imageList.addBitmap(bmp);
            path2index[path] = idx;
            return idx;
        }

        void insertItem(int cmd, const std::wstring& label, const std::wstring& path, int index = -1)
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
            cbix.iImage = loadImage(path.c_str());
            cbix.iSelectedImage = loadImage(path.c_str());
            
            sendMsg(CBEM_INSERTITEM, 0,(LPARAM) &cbix);

            HWND cb = (HWND) sendMsg(CBEM_GETCOMBOCONTROL, 0, 0);
            
            RECT r;
            ::GetWindowRect(cb, &r);
            r.right = r.left + 120;
            r.bottom = r.top + 300;
            ::SetWindowPos(cb, 0, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
        }

        LRESULT curSel()
        {
            return sendMsg(CB_GETCURSEL, 0, 0);
        }

        ComboBoxEx& select(int idx)
        {
            sendMsg(CB_SETCURSEL, idx, 0);
            return *this;
        }

        int count()
        {
            return (int)sendMsg(CB_GETCOUNT, 0, 0);
        }

    };


    class ToolBar;

    template<>
    class MTL::WindowClass<ToolBar>
    {
    public:
        const wchar_t* name()
        {
            return TOOLBARCLASSNAME;
        }
    };

    class ToolBar : public Ctrl<ToolBar>
    {
    protected:
        MTL::ImageList imageList_;
        std::map<size_t, std::wstring> rindex_;
        std::map<std::wstring, size_t> index_;
        std::map<int, int> id2pos_;
        int wPadding = 5;
        int hPadding = 5;
        int w = 32;
        int h = 32;

    public:

        MTL::Event<void(int cmd)> onBar;
        MTL::Event<void(int id, NMTOOLBAR*)> onBarNotify;
        MTL::Event<int()> onRightClick;

        ToolBar& addButton(const std::wstring& path, int iCmd, const wchar_t* label = 0, BYTE style = 0, BYTE state = TBSTATE_ENABLED, DWORD_PTR data = 0)
        {
            size_t index = getBitmapIndex(path);
            TBBUTTON tbButton;
            tbButton.dwData = data;
            tbButton.fsState = state;
            tbButton.fsStyle = style;
            tbButton.iBitmap = (int)index;
            tbButton.idCommand = iCmd;
            tbButton.iString = (INT_PTR)label;

            sendMsg(TB_ADDBUTTONS, (WPARAM)1, (LPARAM)&tbButton);
            id2pos_[iCmd] = count() - 1;
            return *this;
        }


        ToolBar& addTextButton(int iCmd, const wchar_t* label = 0, BYTE style = 0, BYTE state = TBSTATE_ENABLED, DWORD_PTR data = 0)
        {
            TBBUTTON tbButton;
            tbButton.dwData = data;
            tbButton.fsState = state;
            tbButton.fsStyle = style;
            tbButton.iBitmap = 0;
            tbButton.idCommand = iCmd;
            tbButton.iString = (INT_PTR)label;

            sendMsg(TB_ADDBUTTONS, (WPARAM)1, (LPARAM)&tbButton);
            id2pos_[iCmd] = count() - 1;
            return *this;
        }

        ToolBar& addSeparator(int width, int iCmd = 0)
        {
            TBBUTTON tbButton;
            tbButton.dwData = 0;
            tbButton.fsState = 0;
            tbButton.fsStyle = TBSTYLE_SEP;
            tbButton.iBitmap = width;
            tbButton.idCommand = iCmd;
            tbButton.iString = (INT_PTR)0;

            sendMsg(TB_ADDBUTTONS, (WPARAM)1, (LPARAM)&tbButton);
            return *this;
        }

        ToolBar& addControl(HWND child, RECT& r, int iCmd = 0)
        {
            addSeparator(r.right - r.left, iCmd);

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
            id2pos_[iCmd] = count() - 1;
            return *this;
        }


        ToolBar& setPadding(int w, int h)
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

            sendMsg(TB_SETMETRICS, 0, (LPARAM)&tbm);
            return *this;
        }


        ToolBar& setSpacing(int w, int h)
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

            sendMsg(TB_SETMETRICS, 0, (LPARAM)&tbm);
            return *this;
        }

        ToolBar& setIndent(int w)
        {
            sendMsg(TB_SETINDENT, w, 0);
            return *this;
        }

        LRESULT state(int cmd)
        {
            return sendMsg(TB_GETSTATE, (WPARAM)cmd, 0);
        }

        LRESULT state(int cmd, int state)
        {
            return sendMsg(TB_SETSTATE, (WPARAM)cmd, (LPARAM)state);
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

        BYTE buttonStyle(int id)
        {
            TBBUTTONINFO tbbi;
            ::ZeroMemory(&tbbi, sizeof(tbbi));
            tbbi.cbSize = sizeof(tbbi);
            tbbi.dwMask = TBIF_STYLE;
            sendMsg(TB_GETBUTTONINFO, (WPARAM)id, (LPARAM)&tbbi);
            return tbbi.fsStyle;
        }

        BYTE buttonStyle(int id, BYTE style)
        {
            TBBUTTONINFO tbbi;
            ::ZeroMemory(&tbbi, sizeof(tbbi));
            tbbi.cbSize = sizeof(tbbi);
            tbbi.dwMask = TBIF_STYLE;
            tbbi.fsStyle = style;
            sendMsg(TB_SETBUTTONINFO, (WPARAM)id, (LPARAM)&tbbi);
            return tbbi.fsStyle;
        }

        int count()
        {
            return (int)sendMsg(TB_BUTTONCOUNT, 0, 0);
        }

        void clear()
        {
            while (count())
            {
                sendMsg(TB_DELETEBUTTON, 0, 0);
            }
        }

        int width()
        {
            int c = count();
            return (c * (wPadding + w));
        }

        RECT getButtonRect(int iCmd)
        {
            RECT r = { 0,0,0,0 };
            sendMsg(TB_GETRECT, iCmd, (LPARAM)&r);
            return r;
        }

        void getButton(int idx, TBBUTTON* tb)
        {
            sendMsg(TB_GETBUTTON, idx, (LPARAM)tb);
        }

        void getButtonByCmd(int iCmd, TBBUTTON* tb)
        {
            int pos = id2pos_[iCmd];
            sendMsg(TB_GETBUTTON, pos, (LPARAM)tb);
        }

        std::wstring bmp(int idx)
        {
            return rindex_[idx];
        }

    protected:

        virtual LRESULT wmCommand(int id, int code) override
        {
            onCommand.fire(id);
            onBar.fire(id);
            return 0;
        }

        virtual LRESULT wmNotify(int id, NMHDR* nmhdr) override
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


        virtual LRESULT wmCustomDraw(NMCUSTOMDRAW* cd) override
        {
            NMTBCUSTOMDRAW* tbcd = (NMTBCUSTOMDRAW*)cd;

            if (tbcd->nmcd.dwDrawStage == CDDS_PREPAINT)
            {
                HDC hdc = tbcd->nmcd.hdc;
                MTL::dc_view dcv(hdc);
                if(colorTheme->font())
                    dcv.select(colorTheme->font());
                dcv.fillRect(tbcd->nmcd.rc, colorTheme->bkgBrush());
                return TBCDRF_USECDCOLORS  //| TBCDRF_NOEDGES //TBCDRF_NOOFFSET | TBCDRF_NOBACKGROUND | TBCDRF_NOMARK
                    | CDRF_NEWFONT | CDRF_NOTIFYITEMDRAW; //| CDRF_NOTIFYSUBITEMDRAW
                    //| TBCDRF_HILITEHOTTRACK;
                    //|CDRF_NOTIFYPOSTPAINT;// | CDRF_DOERASE | CDRF_SKIPPOSTPAINT;
            }
            if ((tbcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT))
            {
                HDC hdc = tbcd->nmcd.hdc;
                MTL::dc_view dcv(hdc);
                if (colorTheme->font())
                    dcv.select(colorTheme->font());

                UINT state = tbcd->nmcd.uItemState;

                if (state & CDIS_CHECKED)
                {
                    dcv.fillRect(tbcd->nmcd.rc, colorTheme->selectedBkgBrush());
                }
                else
                {
                    dcv.fillRect(tbcd->nmcd.rc, colorTheme->bkgBrush());
                }
                dcv.setTextColor(colorTheme->textColor());
                tbcd->clrText = colorTheme->textColor();
                tbcd->clrHighlightHotTrack = colorTheme->bkgColor();
                tbcd->clrBtnHighlight = colorTheme->textColor();
                tbcd->clrMark = colorTheme->bkgColor();
                tbcd->clrBtnFace = colorTheme->bkgColor();
                tbcd->clrTextHighlight = colorTheme->bkgColor();
                tbcd->hbrMonoDither = colorTheme->bkgBrush();
                tbcd->hbrLines = colorTheme->bkgBrush();

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

        size_t getBitmapIndex(const std::wstring& s)
        {
            if (index_.count(s) == 0)
            {
                auto bmp = bitmapCache().get(s.c_str(), w, h);

                int idx = imageList_.addBitmap(bmp);

                index_[s] = index_.size();
                rindex_[idx] = s;
            }

            if (index_.count(s) == 0)
            {
                return -1;
            }
            return index_[s];
        }

        virtual HWND createWindow(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu)
        {
            auto& wc = MTL::windowClass<ToolBar>();

            handle = ::CreateWindowEx(
                0,
                wc.name(),
                title,
                WS_CHILD,
                r.left, r.top, r.right - r.left, r.bottom - r.top,
                parent,
                menu,
                MTL::module_instance(),
                (LPVOID)this
            );

            subClass();

            sendMsg(TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

            w = r.right - r.left;
            h = r.bottom - r.top;

            sendMsg(TB_SETBITMAPSIZE, 0, MAKELPARAM(w, h));

            imageList_.create(w, h);
            sendMsg(TB_SETIMAGELIST,0,(LPARAM)*imageList_);

            show(SW_SHOW);

            sendMsg(TB_SETSTYLE, 0, style);
            sendMsg(TB_SETEXTENDEDSTYLE, 0, exStyle);

            wmCreate();
            return handle;
        }

    };

    class ReBar;

    template<>
    class MTL::WindowClass<ReBar>
    {
    public:
        const wchar_t* name()
        {
            return REBARCLASSNAME;
        }
    };

    class ReBar : public MTL::Ctrl<ReBar>
    {
    public:

        int count()
        {
            return (int) sendMsg(RB_GETBANDCOUNT, 0, 0);
        }

        void remove(int idx)
        {
            sendMsg(RB_DELETEBAND, idx, 0);
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
            sendMsg(RB_SHOWBAND, idx, show);
        }

        void getBar(int idx, REBARBANDINFO* rbi)
        {
            sendMsg(RB_GETBANDINFO, idx, (LPARAM)rbi);
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

                MTL::wbuff buf(1024);
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

            sendMsg(RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
        }

        void setColors(COLORREF clrBkg, COLORREF clrFore)
        {
            sendMsg(RB_SETBKCOLOR, 0, (LPARAM)clrBkg);
            sendMsg(RB_SETTEXTCOLOR, 0, (LPARAM)clrFore);
        }

        void setShadowColors(COLORREF shadow, COLORREF highlite)
        {
            COLORSCHEME cs;
            ::ZeroMemory(&cs, sizeof(cs));
            cs.dwSize = sizeof(cs);
            cs.clrBtnHighlight = highlite;
            cs.clrBtnShadow = shadow;
            sendMsg(RB_SETCOLORSCHEME, 0, (LPARAM)&cs);
        }

        void setBarColors(int index, COLORREF clrBkg, COLORREF clrFore)
        {
            REBARBANDINFO rbBand = { sizeof(REBARBANDINFO) };

            rbBand.fMask = RBBIM_COLORS;

            rbBand.clrBack = clrBkg;
            rbBand.clrFore = clrFore;

            LRESULT result = sendMsg(RB_SETBANDINFO, (WPARAM)index, (LPARAM)&rbBand);
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
            setColors(colorTheme->bkgColor(), colorTheme->textColor());
            setShadowColors(colorTheme->bkgColor(), colorTheme->bkgColor());
        }

        virtual HWND createWindow(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) override
        {
            auto& wc = MTL::windowClass<ReBar>();

            handle = ::CreateWindowEx(
                exStyle,
                wc.name(),
                title,
                style,
                r.left, r.top, r.right - r.left, r.bottom - r.top,
                parent,
                menu,
                MTL::module_instance(),
                (LPVOID)this
            );

            // NO? 
            subClass();
            show(SW_SHOW);
            wmCreate();
            return handle;
        }
    };


}

