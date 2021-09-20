#pragma once

#include <mtl/win/wnd.h>


namespace mtl {

    class icon
    {
    public:
        icon()
        {}

        ~icon()
        {
            if (hIcon)
            {
                ::DestroyIcon(hIcon);
            }
        }

        icon(HICON icon)
            : hIcon(icon)
        {}

        icon(const icon& rhs) = delete;
        icon& operator=(const icon& rhs) = delete;

        icon(icon&& rhs)
            : hIcon(rhs.hIcon)
        {
            rhs.hIcon = nullptr;
        }

        icon& operator=(icon&& rhs)
        {
            if (hIcon == rhs.hIcon) return *this;
            if (hIcon)
            {
                ::DestroyIcon(hIcon);
            }
            hIcon = rhs.hIcon;
            rhs.hIcon = nullptr;
            return *this;
        }

        icon& operator=(HICON icon)
        {
            if (hIcon == icon) return *this;
            if (hIcon)
            {
                ::DestroyIcon(hIcon);
            }
            hIcon = icon;
            return *this;
        }

        HICON operator*()
        {
            return hIcon;
        }

    private:

        HICON hIcon = nullptr;
    };

    class brush
    {
    public:
        brush()
        {
            hbr_ = ::CreateSolidBrush(RGB(0,0,0));
        }

        brush(COLORREF col)
        {
            hbr_ = ::CreateSolidBrush(col);
        }

        brush(const brush& rhs) = delete;
        brush(brush&& rhs)
            : hbr_(rhs.hbr_)
        {
            rhs.hbr_ = nullptr;
        }

        brush& operator=(const brush& rhs) = delete;

        brush& operator=(brush&& rhs)
        {
            if (hbr_ == rhs.hbr_)
            {
                return *this;
            }
            hbr_ = rhs.hbr_;
            rhs.hbr_ = nullptr;
            return *this;
        }

        ~brush()
        {
            ::DeleteObject(hbr_);
        }

        HBRUSH operator* ()
        {
            return hbr_;
        }

    private:
        HBRUSH hbr_ = nullptr;
    };

    class dc
    {
    public:

        dc() : hdc_(0) 
        {};

        dc(HDC hdc) : hdc_(hdc) 
        {};

        virtual ~dc()
        {
            for (std::list<HGDIOBJ>::iterator it = obList_.begin(); it != obList_.end(); it++)
            {
                ::SelectObject(hdc_, *it);
                //::DeleteObject(_old); note:object should delete itself!
            }
            obList_.clear();
        }

        HDC operator *()    { return hdc_; }
        void detach()       { hdc_ = nullptr; }

        HGDIOBJ select(HGDIOBJ obj)
        {
            HGDIOBJ old = ::SelectObject(hdc_, obj);
            obList_.push_front(old);
            return old;
        }

        int fill_rect(const RECT& r, HBRUSH hbr)     { return ::FillRect(hdc_, &r, hbr); }
        int frame_rect(const RECT& r, HBRUSH hbr)    { return ::FrameRect(hdc_, &r, hbr); }

        int set_bk_mode(int i)                       { return ::SetBkMode(hdc_, i); }
        COLORREF set_bk_color(COLORREF crColor)      { return ::SetBkColor(hdc_, crColor); }
        COLORREF set_text_color(COLORREF crColor)    { return ::SetTextColor(hdc_, crColor); }

        BOOL text_out(int x, int y, std::wstring t)  { return ::TextOut(hdc_, x, y, t.c_str(), (int)t.size()); }

        BOOL ext_text_out(int x, int y, std::wstring t, RECT& r, UINT options = ETO_CLIPPED | ETO_OPAQUE)
        {
            return ::ExtTextOut(hdc_, x, y, options, &r, t.c_str(), (UINT)t.size(), 0);
        }

        void draw_text(const wchar_t* txt, RECT& r, int flags = DT_LEFT)
        {
            ::DrawText(hdc_, txt, -1, &r, flags);
        }

        void bit_blit(HBITMAP bitmap, int x, int y, int flags = SRCCOPY);
        void stretch_blit(HBITMAP bitmap, int x, int y, int w, int h, int flags = SRCCOPY);
        void transparent_blit(HBITMAP bmp, int x, int y, int w, int h, COLORREF transparent = RGB(0, 0, 0));
        void alpha_blend(HBITMAP bmp, int x, int y, int w, int h, int alpha = 0xff );

        void save() { ::SaveDC(hdc_); }
        void set_window_org(int x = 0, int y = 0) { ::SetWindowOrgEx(hdc_, x, y, NULL); }
        void set_window_ext(int x = 0, int y = 0) { ::SetWindowExtEx(hdc_, x, y, NULL); }

        void restore(int i = -1) { ::RestoreDC(hdc_, i); }

    protected:
        std::list<HGDIOBJ>	obList_;
        HDC		hdc_;
    };


    class paint_dc : public dc
    {
    public:
        paint_dc(HWND hwnd)
            : hWnd_(hwnd)
        {
            hdc_ = ::BeginPaint(hWnd_, &ps_);
        }

        ~paint_dc()
        {
            ::EndPaint(hWnd_, &ps_);
        }

        HDC operator*()
        {
            return hdc_;
        }

    protected:
        HWND			hWnd_;
        PAINTSTRUCT		ps_;
    };


    class wnd_dc : public dc
    {
    public:
        wnd_dc() : hWnd_(::GetDesktopWindow())
        { 
            hdc_ = ::GetDC(hWnd_); 
        }

        wnd_dc(HWND hwnd) :hWnd_(hwnd)
        { 
            hdc_ = ::GetDC(hWnd_); 
        }

        ~wnd_dc()
        {  
            ::ReleaseDC(hWnd_, hdc_); 
        }

        HDC operator*()
        {
            return hdc_;
        }

    private:
        HWND			hWnd_;
    };

    class compatible_dc : public dc
    {
    public:

        compatible_dc(HDC hdc = nullptr)
        { 
            if (hdc)
            {
                hdc_ = ::CreateCompatibleDC(hdc);
            }
            else
            {
                wnd_dc desktopDC;
                hdc_ = ::CreateCompatibleDC(*desktopDC);
            }
        }

        ~compatible_dc()
        { 
            ::DeleteDC(hdc_); 
        }

        HDC operator*()
        {
            return hdc_;
        }

    };

    class meta_dc : public dc
    {
    public:
        meta_dc()
        { 
            create();
        }

        void create()
        {
            if (hdc_)
            {
                ::CloseMetaFile(hdc_);
            }
            hdc_ = ::CreateMetaFile(NULL);
        }

        HDC operator*()
        {
            return hdc_;
        }

        HMETAFILE close() 
        { 
            HMETAFILE hmf = ::CloseMetaFile(hdc_);
            hdc_ = 0;
            return hmf;
        }
    };

    inline void dc::bit_blit(HBITMAP bitmap, int x, int y, int flags )
    {
        compatible_dc cdc;
        cdc.select(bitmap);

        BITMAP bm;
        ::GetObject(bitmap, sizeof(bm), &bm);
        ::BitBlt(hdc_, x, y, bm.bmWidth, bm.bmHeight, *cdc, 0, 0, flags);
    }

    inline void dc::stretch_blit(HBITMAP bitmap, int x, int y, int w, int h, int flags)
    {
        compatible_dc cdc;
        cdc.select(bitmap);

        BITMAP bm;
        ::GetObject(bitmap, sizeof(bm), &bm);
        ::StretchBlt(hdc_, x, y, w, h, *cdc, 0, 0, bm.bmWidth, bm.bmHeight, flags);
    }

    inline void dc::transparent_blit(HBITMAP bitmap, int x, int y, int w, int h, COLORREF transparent)
    {
        compatible_dc cdc;
        cdc.set_bk_mode(TRANSPARENT);
        cdc.select(bitmap);

        BITMAP bm;
        ::GetObject(bitmap, sizeof(bm), &bm);

        ::TransparentBlt( hdc_, x, y, w, h, *cdc, 0, 0, bm.bmWidth, bm.bmHeight, transparent);
    }

    inline void dc::alpha_blend(HBITMAP bmp, int x, int y, int w, int h, int alpha)
    {
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = alpha; 
        bf.AlphaFormat = 1;

        BITMAP bm;
        ::GetObject(bmp, sizeof(bm), &bm);

        wnd_dc desktopDC;
        compatible_dc cdc(*desktopDC);
        cdc.select(bmp);

        ::AlphaBlend(hdc_, x, y, w, h, *cdc, 0, 0, bm.bmWidth, bm.bmHeight, bf);
    }

    class font_desc
    {
    public:

        font_desc()
        {
            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
        }

        font_desc(const std::wstring& font, int size)
        {
            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            wcsncpy_s(logfont_.lfFaceName, font.c_str(), font.size()+1);
            logfont_.lfHeight = size;
        }

        font_desc(HFONT hfont)
        {
            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            ::GetObject(hfont, sizeof(LOGFONT), (void*)&logfont_);
        }

        font_desc(LOGFONT* font)
        {
            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            memcpy(&logfont_, font, sizeof(LOGFONT));
        }

        font_desc(const font_desc& rhs)
        {
            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            memcpy(&logfont_, &rhs.logfont_, sizeof(LOGFONT));
        }

        font_desc(font_desc&& rhs)
        {
            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            memcpy(&logfont_, &rhs.logfont_, sizeof(LOGFONT));
            ::ZeroMemory((void*)&rhs.logfont_, sizeof(LOGFONT));
        }

        font_desc& operator=(const font_desc& rhs)
        {
            if (this == &rhs)
            {
                return *this;
            }

            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            memcpy(&logfont_, &rhs.logfont_, sizeof(LOGFONT));

            return *this;
        }

        font_desc& operator=(font_desc&& rhs)
        {
            if (this == &rhs)
            {
                return *this;
            }

            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            memcpy(&logfont_, &rhs.logfont_, sizeof(LOGFONT));
            ::ZeroMemory((void*)&rhs.logfont_, sizeof(LOGFONT));

            return *this;
        }

        const wchar_t* name()
        {
            return logfont_.lfFaceName;
        }

        std::wstring size()
        {
            int h = -1 * (logfont_.lfHeight);

            std::wostringstream woss;
            woss << h;
            return woss.str();
        }

       std::wstring weight()
        {
            std::wostringstream woss;
            woss << logfont_.lfWeight;
            return woss.str();
        }

        std::wstring style()
        {
            if (logfont_.lfItalic == TRUE)
                return L"italic";
            return L"normal";
        }

        std::wstring decoration()
        {
            if (logfont_.lfUnderline == TRUE)
                return L"underline";
            if (logfont_.lfStrikeOut == TRUE)
                return L"line-through";

            return L"normal";
        }

        LOGFONT* font()
        {
            return &logfont_;
        }

        HFONT create()
        {
            return ::CreateFontIndirect(font());
        }

        font_desc scale(int iDpi)
        {
            font_desc fd(*this);
            fd.font()->lfHeight = -::MulDiv(logfont_.lfHeight, iDpi, 96);
            return fd;
        }

    private:
        LOGFONT	logfont_;
    };


    class font
    {
    public:

        font()
        {}

        font(HFONT font)
            : font_(font)
        {}

        ~font()
        {
            if (font_)
            {
                ::DeleteObject(font_);
            }
        }

        font(font&& rhs)
            : font_(rhs.font_)
        {
            rhs.font_ = nullptr;
        }

        font& operator=(font&& rhs)
        {
            if (font_ == rhs.font_)
            {
                return *this;
            }
            if (font_)
            {
                ::DeleteObject(font_);
            }
            font_ = rhs.font_;
            rhs.font_ = nullptr;
            return *this;
        }

        HFONT operator*()
        {
            return font_;
        }

        font& operator=(HFONT rhs)
        {
            if (font_)
            {
                ::DeleteObject(font_);
            }
            font_ = rhs;
            return *this;
        }

    private:

        font(const font& rhs) = delete;
        font& operator=(const font& rhs) = delete;

        HFONT font_ = nullptr;
    };

    class menu_item : public owner_drawn
    {
    public:

        int id = -1;
        std::wstring label;
        bool enabled = true;
        bool checked = false;
        HMENU subMenu = nullptr;
        HBITMAP bitmap = nullptr;
        std::shared_ptr<color_theme> theme;
        HMENU parent = nullptr;

        menu_item() 
        {}

        menu_item( int i, const std::wstring& l, bool e, bool c, HMENU m, HBITMAP b)
            : id(i), label(l), enabled(e), checked(c), subMenu(m), bitmap(b)
        {}

        virtual LRESULT wm_draw_item(LPDRAWITEMSTRUCT dis) override;
        virtual LRESULT wm_measure_item(MEASUREITEMSTRUCT* mis) override;

        void check(bool status)
        {
            checked = status;

            if (theme && theme->enabled()) return;

            MENUITEMINFO mii;
            ::ZeroMemory(&mii, sizeof(mii));
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_STATE| MIIM_CHECKMARKS| MIIM_BITMAP;

            BOOL b = ::GetMenuItemInfo(parent, id, FALSE, &mii);

            if (status)
            {
                mii.fState |= MFS_CHECKED;
                mii.hbmpChecked = NULL;
                mii.hbmpItem = bitmap;
            
            }
            else
            {
                mii.fState &= ~MFS_CHECKED;
                mii.hbmpChecked = NULL;
                mii.hbmpItem = bitmap;
            }
             b = ::SetMenuItemInfo(parent, id, FALSE, &mii);
            b = false;
        }

        void status(bool status)
        {
            enabled = status;

            if (theme && theme->enabled()) return;

            MENUITEMINFO mii;
            ::ZeroMemory(&mii, sizeof(mii));
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_STATE;

            ::GetMenuItemInfo(parent, id, FALSE, &mii);

            if (status)
            {
                mii.fState |= MFS_ENABLED;
            }
            else
            {
                mii.fState &= ~MFS_ENABLED;
            }
            ::SetMenuItemInfo(parent, id, FALSE, &mii);
        }
    };

    class menu
    {
    friend class menu_builder;
    public:

        std::vector<std::shared_ptr<menu_item>> items;
        std::map<int, menu> subMenus;

        menu()
        {}

        menu(HMENU hmenu, bool attached = false)
            : hMenu_(hmenu), attached_(attached)
        {}

        menu(int id)
        { 
            hMenu_ = ::LoadMenu(mtl::module_instance(), MAKEINTRESOURCE(id));
        }

        menu(const menu& rhs)
            : attached_(false), 
              hMenu_(rhs.hMenu_),
              colorTheme_(rhs.colorTheme_),
              items(rhs.items),
              subMenus(rhs.subMenus)
        {}

        menu(menu&& rhs)
            : attached_(rhs.attached_), 
              hMenu_(rhs.hMenu_),
              colorTheme_(rhs.colorTheme_),
              items(std::move(rhs.items)),
              subMenus(std::move(rhs.subMenus))
        {
            rhs.hMenu_ = nullptr;
        }

        void create()
        {
            if (hMenu_ && attached_)
            {
                ::DestroyMenu(hMenu_);
                hMenu_ = nullptr;
            }
            hMenu_ = ::CreateMenu();
        }

        void create_popup()
        {
            if (hMenu_ && attached_)
            {
                ::DestroyMenu(hMenu_);
                hMenu_ = nullptr;
            }
            hMenu_ = ::CreatePopupMenu();
        }

        void hook_color_theme(std::shared_ptr<color_theme>& ct)
        {
            colorTheme_.reset();
            colorThemeToken_ = ct->onUpdate([this]() { this->update(); });
        }

        menu submenu(int pos)
        {
            HMENU m = ::GetSubMenu(hMenu_, pos);
            return menu(m, false);
        }

        menu_item& item(int id)
        {
            static menu_item empty;

            for (auto& it : items)
            {
                if (it->id == id)
                {
                    return *it;
                }
            }

            for (auto& sm : subMenus)
            {
                if (sm.first == id)
                {
                    return empty;
                }
                menu_item& mi = sm.second.item(id);
                if (mi.id != -1) return mi;
            }

            return empty;
        }

        void forEach(std::function<void(menu_item&)> visitor)
        {
            for (auto& it : items)
            {
                visitor(*it);
            }
        }

        void update(menu_item& item)
        {
            if (item.subMenu)
            {
                menu subMenu = subMenus[item.id];

                menu men;
                men.subMenus = subMenu.subMenus;
                men.items = subMenu.items;
                men.create_popup();
                men.colorTheme_ = subMenu.colorTheme_;

                for (auto& smItem : subMenu.items)
                {
                    men.update(*smItem);
                }

                item.subMenu = *men;
                men.detach();
                subMenus[item.id] = std::move(men);
            }
            auto smi = std::make_shared<menu_item>(item.id, item.label, item.enabled, item.checked, item.subMenu, item.bitmap);
            add(smi);
        }

        void update()
        {
            int c = count();
            for (int i = c - 1; c >= 0; c--)
            {
                ::RemoveMenu(hMenu_, c, MF_BYPOSITION);
            }

            for (auto& item : items)
            {
                update(*item);
            }
        }

        std::shared_ptr<color_theme> theme()
        {
            return colorTheme_;
        }

        menu sub_menu(int id)
        {
            MENUITEMINFO mii;
            ::ZeroMemory(&mii, sizeof(mii));
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_SUBMENU;
            ::GetMenuItemInfo(hMenu_, id, FALSE, &mii);
            return menu(mii.hSubMenu, false);
        }

        int id(int pos)
        {
            return ::GetMenuItemID(hMenu_, pos);
        }

        void add(std::shared_ptr<menu_item> item)
        {
            int cnt = ::GetMenuItemCount(hMenu_);

            if (items.size() < cnt )
            {
                exit(0);
            }

            if (items.size() == cnt)
            {
                items.push_back(item);
            }
            else
            {
                items[cnt] = item;
            }
            items[cnt]->parent = hMenu_;

            MENUITEMINFO mii;
            ::ZeroMemory(&mii, sizeof(mii));
            mii.cbSize = sizeof(mii);
            mii.wID = item->id;
            mii.fState = MFS_ENABLED;
            std::shared_ptr<menu_item> it = items[cnt];
            mii.dwItemData = (ULONG_PTR) it.get();

            if (colorTheme_ && colorTheme_->enabled())
            {
                mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_ID | MIIM_STATE;
                mii.fType = MFT_OWNERDRAW;
                items[cnt]->theme = colorTheme_;
            }
            else
            {
                mii.fMask = MIIM_DATA | MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE;
                mii.fType = MFT_STRING;
                mii.dwTypeData = (LPWSTR)(items[cnt]->label.c_str());
                mii.cch = (UINT)items[cnt]->label.size();

                if (item->bitmap)
                {
                    mii.fMask |= MIIM_BITMAP;
                    HBITMAP bmp = item->bitmap;
                    mii.hbmpItem = bmp;
                    mii.hbmpChecked = bmp;
                    mii.hbmpUnchecked = bmp;
                }

                if (item->checked)
                {
                    mii.fState |= MFS_CHECKED;
                }
            }

            if (item->subMenu)
            {
                mii.fMask |= MIIM_SUBMENU;
                mii.hSubMenu = item->subMenu;
            }
            BOOL b = ::InsertMenuItem(hMenu_, cnt, TRUE, &mii);
            if (!b)
            {
                int x = b ? 1 : 0;
            }
        }

        void add_bitmap(int pos, HBITMAP bmp)
        {
            MENUITEMINFO mii;
            ::ZeroMemory(&mii, sizeof(mii));
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_BITMAP;
            mii.hbmpItem = bmp;
            mii.hbmpChecked = bmp;
            mii.hbmpUnchecked = bmp;
            ::SetMenuItemInfo(hMenu_, pos, TRUE, &mii);
        }

        int count()
        {
            return (int)items.size();
            // return ::GetMenuItemCount(hMenu_);
        }

        ~menu()
        {
            if (hMenu_ && attached_)
            {
                ::DestroyMenu(hMenu_);
                hMenu_ = nullptr;
            }

            if (!colorThemeToken_.empty())
            {
                if (colorTheme_)
                {
                    colorTheme_->onUpdate.unregister(colorThemeToken_);
                    colorThemeToken_ = L"";
                }
            }
        }

        int track_popup(HWND wnd, int flags = TPM_LEFTALIGN | TPM_RETURNCMD)
        {
            flags |= TPM_RETURNCMD;
            POINT pt;
            ::GetCursorPos(&pt);
            int cmd = ::TrackPopupMenu(hMenu_, flags, pt.x, pt.y, 0, wnd, 0);
            return cmd;
        }

        void popup(HWND wnd, int flags = TPM_LEFTALIGN)
        {
            POINT pt;
            ::GetCursorPos(&pt);
            ::TrackPopupMenu(hMenu_, flags, pt.x, pt.y, 0, wnd, 0);
        }

        HMENU operator* ()
        {
            return hMenu_;
        }

        void detach()
        {
            attached_ = false;
        }

        operator bool()
        {
            return ::IsMenu(hMenu_);
        }


        menu& operator=(const menu& rhs)
        {
            if (this == &rhs)
            {
                return *this;
            }
            
            if (hMenu_ && attached_)
            {
                ::DestroyMenu(hMenu_);
                hMenu_ = nullptr;
            }
            attached_ = false;
            hMenu_ = rhs.hMenu_;
            colorTheme_ = rhs.colorTheme_;
            items = rhs.items;
            subMenus = rhs.subMenus;
            return *this;
        }

        menu& operator=(menu&& rhs)
        {
            if (this == &rhs)
            {
                return *this;
            }

            if (hMenu_ && attached_)
            {
                ::DestroyMenu(hMenu_);
                hMenu_ = nullptr;
            }
            attached_ = rhs.attached_;
            hMenu_ = rhs.hMenu_;
            rhs.hMenu_ = nullptr;
            colorTheme_ = rhs.colorTheme_;
            items = std::move(rhs.items);
            subMenus = std::move(rhs.subMenus);
            return *this;
        }

    private:
        std::shared_ptr<color_theme> colorTheme_;
        HMENU hMenu_ = nullptr;
        bool attached_ = true;
        std::wstring colorThemeToken_;
    };

    inline LRESULT menu_item::wm_draw_item(LPDRAWITEMSTRUCT dis)
    {
        dc dcv(dis->hDC);
        dcv.select(theme->font());

        menu_item* item = (menu_item*)(dis->itemData);
        auto theme = item->theme;

        if ((dis->itemAction & ODA_SELECT) && (dis->itemState & ODS_SELECTED))
        {
            dcv.fill_rect(dis->rcItem, theme->selected_bkg_brush());
        }
        else
        {
            dcv.fill_rect(dis->rcItem, theme->bkg_brush());
        }

        int x = dis->rcItem.left + theme->padding();
        int y = dis->rcItem.top + theme->padding();

        COLORREF bkg = theme->bkg_color();
        COLORREF txt = theme->text_color();
        if ((dis->itemAction & ODA_SELECT) && (dis->itemState & ODS_SELECTED))
        {
            bkg = theme->selected_bkg_color();
            txt = theme->selected_text_color();
        }

        COLORREF oldBkg = dcv.set_bk_color(bkg);
        COLORREF oldTxt = dcv.set_text_color(txt);

        int bmw = 0;
        int bmh = 0;
        if (item->bitmap)
        {
            BITMAP bmp;
            ::GetObject(item->bitmap, sizeof(bmp), &bmp);
            bmw = bmp.bmWidth;
            bmh = bmp.bmHeight;
            dcv.set_bk_mode(TRANSPARENT);
            dcv.alpha_blend(item->bitmap, x, y, bmp.bmWidth, bmp.bmHeight, 0xff);
            x += bmp.bmWidth;
        }

        x += theme->padding();

        SIZE size = { 0,0 };
        ::GetTextExtentPoint32(*dcv, item->label.c_str(), (int)item->label.size(), &size);

        RECT r = dis->rcItem;
        r.left += x;

        y = r.top + (((r.bottom - r.top) - size.cy) / 2);
        dcv.ext_text_out(x, y, item->label.c_str(), r, ETO_OPAQUE);

        if (item->checked)
        {
            RECT r = dis->rcItem;
            r.left += item->theme->padding();
            r.top += item->theme->padding();
            r.right = r.left + bmw +2;
            r.bottom = r.top + bmh +2;
            dcv.frame_rect(r, item->theme->selected_bkg_brush());
        }

        dcv.set_bk_color(oldBkg);
        dcv.set_text_color(oldTxt);

        return TRUE;
    }

    inline LRESULT menu_item::wm_measure_item(MEASUREITEMSTRUCT* mis)
    {
        mtl::wnd_dc dc(::GetDesktopWindow());
        dc.select(theme->font());

        menu_item* item = (menu_item*)(mis->itemData);
        auto theme = item->theme;

        SIZE size = { 0,0 };
        ::GetTextExtentPoint32(*dc, item->label.c_str(), (int)item->label.size(), &size);
        mis->itemWidth = size.cx + 2 * theme->padding();
        mis->itemHeight = 2 * theme->padding();

        if (item->bitmap)
        {
            BITMAP bmp;
            ::GetObject(item->bitmap, sizeof(bmp), &bmp);

            mis->itemWidth  += 2 * theme->padding() + bmp.bmWidth;
            mis->itemHeight += bmp.bmHeight;
        }

        return TRUE;
    }

}