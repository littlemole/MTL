#pragma once

#include <MTL/win/wind.h>


namespace MTL {

    class Icon
    {
    public:
        Icon()
        {}

        ~Icon()
        {
            if (hIcon)
            {
                ::DestroyIcon(hIcon);
            }
        }

        Icon(HICON icon)
            : hIcon(icon)
        {}

        Icon(const Icon& rhs) = delete;
        Icon& operator=(const Icon& rhs) = delete;

        Icon(Icon&& rhs)
            : hIcon(rhs.hIcon)
        {
            rhs.hIcon = nullptr;
        }

        Icon& operator=(Icon&& rhs)
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

        Icon& operator=(HICON icon)
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

    class Brush
    {
    public:
        Brush()
        {
            hbr_ = ::CreateSolidBrush(RGB(0,0,0));
        }

        Brush(COLORREF col)
        {
            hbr_ = ::CreateSolidBrush(col);
        }

        Brush(const Brush& rhs) = delete;
        Brush(Brush&& rhs)
            : hbr_(rhs.hbr_)
        {
            rhs.hbr_ = nullptr;
        }

        Brush& operator=(const Brush& rhs) = delete;

        Brush& operator=(Brush&& rhs)
        {
            if (hbr_ == rhs.hbr_)
            {
                return *this;
            }
            hbr_ = rhs.hbr_;
            rhs.hbr_ = nullptr;
            return *this;
        }

        ~Brush()
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

    class dc_view
    {
    public:

        dc_view() : hdc_(0) {};
        dc_view(HDC hdc) : hdc_(hdc) {};
        virtual ~dc_view()
        {
            for (std::list<HGDIOBJ>::iterator it = obList_.begin(); it != obList_.end(); it++)
            {
                ::SelectObject(hdc_, *it);
                //::DeleteObject(_old); note:object should delete itself!
            }
            obList_.clear();
        }

        int setBkMode(int i)                       { return ::SetBkMode(hdc_, i); }
        int fillRect(const RECT& r, HBRUSH hbr)     { return ::FillRect(hdc_, &r, hbr); }
        int frameRect(const RECT& r, HBRUSH hbr)    { return ::FrameRect(hdc_, &r, hbr); }
        COLORREF setBkColor(COLORREF crColor)       { return ::SetBkColor(hdc_, crColor); }
        COLORREF setTextColor(COLORREF crColor)     { return ::SetTextColor(hdc_, crColor); }
        BOOL textOut(int x, int y, std::wstring t)  { return ::TextOut(hdc_, x, y, t.c_str(), (int)t.size()); }
        BOOL extTextOut(int x, int y, std::wstring t, RECT& r, UINT options = ETO_CLIPPED | ETO_OPAQUE)
        {
            return ::ExtTextOut(hdc_, x, y, options, &r, t.c_str(), (UINT)t.size(), 0);
        }

        void save()                                 { ::SaveDC(hdc_); }
        void setWindowOrgEx(int x = 0, int y = 0)   { ::SetWindowOrgEx(hdc_, x, y, NULL); }
        void setWindowExtEx(int x = 0, int y = 0)   { ::SetWindowExtEx(hdc_, x, y, NULL); }

        void restore(int i = -1)                    { ::RestoreDC(hdc_, i); }

        void drawText( const wchar_t* txt, RECT& r, int flags = DT_LEFT)
        {
            ::DrawText(hdc_, txt, -1, &r, flags);
        }

        void bitBlit(HBITMAP bitmap, int x, int y, int flags = SRCCOPY);
        void stretchBlit(HBITMAP bitmap, int x, int y, int w, int h, int flags = SRCCOPY);
        void transparentBlit(HBITMAP bmp, int x, int y, int w, int h, COLORREF transparent = RGB(0, 0, 0));
        void alphaBlend(HBITMAP bmp, int x, int y, int w, int h, int alpha = 0xff );


        HGDIOBJ select(HGDIOBJ obj)
        {
            HGDIOBJ old = ::SelectObject(hdc_, obj);
            obList_.push_front(old);
            return old;
        }

        HDC operator *() { return hdc_; }

        void detach() { hdc_ = nullptr; }

    protected:
        std::list<HGDIOBJ>	obList_;
        HDC		hdc_;
    };


    class PaintDC 
    {
    public:
        PaintDC(HWND hwnd)
            : hWnd_(hwnd)
        {
            hdc_ = ::BeginPaint(hWnd_, &ps_);
        }

        ~PaintDC()
        {
            ::EndPaint(hWnd_, &ps_);
        }

        HDC operator*()
        {
            return hdc_;
        }

    protected:
        HWND			hWnd_;
        HDC             hdc_;
        PAINTSTRUCT		ps_;
    };


    class DC 
    {
    public:
        DC() : hWnd_(::GetDesktopWindow()) 
        { 
            hdc_ = ::GetDC(hWnd_); 
        }

        DC(HWND hwnd) :hWnd_(hwnd) 
        { 
            hdc_ = ::GetDC(hWnd_); 
        }

        ~DC() 
        {  
            ::ReleaseDC(hWnd_, hdc_); 
        }

        HDC operator*()
        {
            return hdc_;
        }

    private:
        HWND			hWnd_;
        HDC             hdc_;
    };

    class CompatibleDC
    {
    public:

        CompatibleDC(HDC hdc = nullptr) 
        { 
            if (hdc)
            {
                hdc_ = ::CreateCompatibleDC(hdc);
            }
            else
            {
                DC desktopDC;
                hdc_ = ::CreateCompatibleDC(*desktopDC);
            }
        }

        ~CompatibleDC() 
        { 
            ::DeleteDC(hdc_); 
        }

        HDC operator*()
        {
            return hdc_;
        }

    private:
        HDC             hdc_;
    };

    class MetaDC
    {
    public:
        MetaDC() 
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

    private:
        HDC             hdc_ = nullptr;
    };

    inline void dc_view::bitBlit(HBITMAP bitmap, int x, int y, int flags )
    {
        MTL::CompatibleDC cdc;
        MTL::dc_view dcv(*cdc);
        dcv.select(bitmap);

        BITMAP bm;
        ::GetObject(bitmap, sizeof(bm), &bm);
        ::BitBlt(hdc_, x, y, bm.bmWidth, bm.bmHeight, *cdc, 0, 0, flags);
    }

    inline void dc_view::stretchBlit(HBITMAP bitmap, int x, int y, int w, int h, int flags)
    {
        MTL::CompatibleDC cdc;
        MTL::dc_view dcv(*cdc);
        dcv.select(bitmap);

        BITMAP bm;
        ::GetObject(bitmap, sizeof(bm), &bm);
        ::StretchBlt(hdc_, x, y, w, h, *cdc, 0, 0, bm.bmWidth, bm.bmHeight, flags);
    }

    inline void dc_view::transparentBlit(HBITMAP bitmap, int x, int y, int w, int h, COLORREF transparent)
    {
        MTL::CompatibleDC cdc;
        MTL::dc_view dcv(*cdc);
        dcv.setBkMode(TRANSPARENT);
        dcv.select(bitmap);

        BITMAP bm;
        ::GetObject(bitmap, sizeof(bm), &bm);

        ::TransparentBlt( hdc_, x, y, w, h, *cdc, 0, 0, bm.bmWidth, bm.bmHeight, transparent);
    }

    inline void dc_view::alphaBlend(HBITMAP bmp, int x, int y, int w, int h, int alpha)
    {
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = alpha; 
        bf.AlphaFormat = 1;

        BITMAP bm;
        ::GetObject(bmp, sizeof(bm), &bm);

        DC desktopDC;
        CompatibleDC cdcs(*desktopDC);
        MTL::dc_view dcvs(*cdcs);
        dcvs.select(bmp);

        ::AlphaBlend(hdc_, x, y, w, h, *dcvs, 0, 0, bm.bmWidth, bm.bmHeight, bf);
    }

    class FontDesc
    {
    public:

        FontDesc()
        {
            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
        }

        FontDesc(const std::wstring& font, int size)
        {
            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            wcsncpy_s(logfont_.lfFaceName, font.c_str(), font.size()+1);
            logfont_.lfHeight = size;
        }

        FontDesc(HFONT hfont)
        {
            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            ::GetObject(hfont, sizeof(LOGFONT), (void*)&logfont_);
        }

        FontDesc(LOGFONT* font)
        {
            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            memcpy(&logfont_, font, sizeof(LOGFONT));
        }

        FontDesc(const FontDesc& rhs)
        {
            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            memcpy(&logfont_, &rhs.logfont_, sizeof(LOGFONT));
        }

        FontDesc(FontDesc&& rhs)
        {
            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            memcpy(&logfont_, &rhs.logfont_, sizeof(LOGFONT));
            ::ZeroMemory((void*)&rhs.logfont_, sizeof(LOGFONT));
        }

        FontDesc& operator=(const FontDesc& rhs)
        {
            if (this == &rhs)
            {
                return *this;
            }

            ::ZeroMemory((void*)&logfont_, sizeof(LOGFONT));
            memcpy(&logfont_, &rhs.logfont_, sizeof(LOGFONT));

            return *this;
        }

        FontDesc& operator=(FontDesc&& rhs)
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

        MTL::FontDesc scale(int iDpi)
        {
            MTL::FontDesc fd(*this);
            fd.font()->lfHeight = -::MulDiv(logfont_.lfHeight, iDpi, 96);
            return fd;
        }

    private:
        LOGFONT	logfont_;
    };


    class Font
    {
    public:
        Font()
        {}

        Font(HFONT font)
            : font_(font)
        {}

        ~Font()
        {
            if (font_)
            {
                ::DeleteObject(font_);
            }
        }

        Font(Font&& rhs)
            : font_(rhs.font_)
        {
            rhs.font_ = nullptr;
        }

        Font& operator=(Font&& rhs)
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

        Font& operator=(HFONT rhs)
        {
            if (font_)
            {
                ::DeleteObject(font_);
            }
            font_ = rhs;
            return *this;
        }

    private:

        Font(const Font& rhs) = delete;
        Font& operator=(const Font& rhs) = delete;

        HFONT font_ = nullptr;
    };

class MenuItem : public OwnerDrawn
{
public:
    int id = -1;
    std::wstring label;
    bool enabled = true;
    bool checked = false;
    HMENU subMenu = nullptr;
    HBITMAP bitmap = nullptr;
    std::shared_ptr<ColorTheme> theme;
    HMENU parent = nullptr;

    MenuItem() {}

    MenuItem( int i, const std::wstring& l, bool e, bool c, HMENU m, HBITMAP b)
        : id(i), label(l), enabled(e), checked(c), subMenu(m), bitmap(b)
    {}

    virtual LRESULT wmDrawItem(LPDRAWITEMSTRUCT dis) override;
    virtual LRESULT wmMeasureItem(MEASUREITEMSTRUCT* mis) override;

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

    class Menu
    {
    friend class MenuBuilder;
    public:

        std::vector<std::shared_ptr<MenuItem>> items;
        std::map<int, Menu> subMenus;

        Menu()
        {}

        Menu(HMENU hmenu, bool attached = false)
            : hMenu_(hmenu), attached_(attached)
        {}

        Menu(int id)
        { 
            hMenu_ = ::LoadMenu(MTL::module_instance(), MAKEINTRESOURCE(id));
        }

        Menu(const Menu& rhs)
            : attached_(false), 
              hMenu_(rhs.hMenu_),
              colorTheme_(rhs.colorTheme_),
              items(rhs.items),
              subMenus(rhs.subMenus)
        {}

        Menu(Menu&& rhs)
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

        void createPopUp()
        {
            if (hMenu_ && attached_)
            {
                ::DestroyMenu(hMenu_);
                hMenu_ = nullptr;
            }
            hMenu_ = ::CreatePopupMenu();
        }

        void hookColorTheme(std::shared_ptr<ColorTheme>& ct)
        {
            colorTheme_.reset();
            colorThemeToken_ = ct->onUpdate([this]() { this->update(); });
        }

        Menu submenu(int pos)
        {
            HMENU m = ::GetSubMenu(hMenu_, pos);
            return Menu(m, false);
        }

        MenuItem& item(int id)
        {
            static MenuItem empty;

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
                MenuItem& mi = sm.second.item(id);
                if (mi.id != -1) return mi;
            }

            return empty;
        }

        void forEach(std::function<void(MenuItem&)> visitor)
        {
            for (auto& it : items)
            {
                visitor(*it);
            }
        }

        void update(MenuItem& item)
        {
            if (item.subMenu)
            {
                Menu subMenu = subMenus[item.id];

                Menu menu;
                menu.subMenus = subMenu.subMenus;
                menu.items = subMenu.items;
                menu.createPopUp();
                menu.colorTheme_ = subMenu.colorTheme_;

                for (auto& smItem : subMenu.items)
                {
                    menu.update(*smItem);
                }

                item.subMenu = *menu;
                menu.detach();
                subMenus[item.id] = std::move(menu);
            }
            auto smi = std::make_shared<MenuItem>(item.id, item.label, item.enabled, item.checked, item.subMenu, item.bitmap);
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

        std::shared_ptr<ColorTheme> colorTheme()
        {
            return colorTheme_;
        }

        Menu subMenu(int id)
        {
            MENUITEMINFO mii;
            ::ZeroMemory(&mii, sizeof(mii));
            mii.cbSize = sizeof(mii);
            mii.fMask = MIIM_SUBMENU;
            ::GetMenuItemInfo(hMenu_, id, FALSE, &mii);
            return Menu(mii.hSubMenu, false);
        }

        int id(int pos)
        {
            return ::GetMenuItemID(hMenu_, pos);
        }

        void add(std::shared_ptr<MenuItem> item)
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
            std::shared_ptr<MenuItem> it = items[cnt];
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

        void addBitmap(int pos, HBITMAP bmp)
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

        ~Menu()
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

        int trackPopUp(HWND wnd, int flags = TPM_LEFTALIGN | TPM_RETURNCMD)
        {
            flags |= TPM_RETURNCMD;
            POINT pt;
            ::GetCursorPos(&pt);
            int cmd = ::TrackPopupMenu(hMenu_, flags, pt.x, pt.y, 0, wnd, 0);
            return cmd;
        }

        void popUp(HWND wnd, int flags = TPM_LEFTALIGN)
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


        Menu& operator=(const Menu& rhs)
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

        Menu& operator=(Menu&& rhs)
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
        std::shared_ptr<ColorTheme> colorTheme_;
        HMENU hMenu_ = nullptr;
        bool attached_ = true;
        std::wstring colorThemeToken_;
    };

    inline LRESULT MenuItem::wmDrawItem(LPDRAWITEMSTRUCT dis)
    {
        MTL::dc_view dcv(dis->hDC);
        dcv.select(theme->font());

        MTL::MenuItem* item = (MTL::MenuItem*)(dis->itemData);
        auto theme = item->theme;

        if ((dis->itemAction & ODA_SELECT) && (dis->itemState & ODS_SELECTED))
        {
            dcv.fillRect(dis->rcItem, theme->selectedBkgBrush());
        }
        else
        {
            dcv.fillRect(dis->rcItem, theme->bkgBrush());
        }

        int x = dis->rcItem.left + theme->padding();
        int y = dis->rcItem.top + theme->padding();

        COLORREF bkg = theme->bkgColor();
        COLORREF txt = theme->textColor();
        if ((dis->itemAction & ODA_SELECT) && (dis->itemState & ODS_SELECTED))
        {
            bkg = theme->selectedBkgColor();
            txt = theme->selectedTextColor();
        }

        COLORREF oldBkg = dcv.setBkColor(bkg);
        COLORREF oldTxt = dcv.setTextColor(txt);

        int bmw = 0;
        int bmh = 0;
        if (item->bitmap)
        {
            BITMAP bmp;
            ::GetObject(item->bitmap, sizeof(bmp), &bmp);
            bmw = bmp.bmWidth;
            bmh = bmp.bmHeight;
            dcv.setBkMode(TRANSPARENT);
            dcv.alphaBlend(item->bitmap, x, y, bmp.bmWidth, bmp.bmHeight, 0xff);
            x += bmp.bmWidth;
        }

        x += theme->padding();

        SIZE size = { 0,0 };
        ::GetTextExtentPoint32(*dcv, item->label.c_str(), (int)item->label.size(), &size);

        RECT r = dis->rcItem;
        r.left += x;

        y = r.top + (((r.bottom - r.top) - size.cy) / 2);
        dcv.extTextOut(x, y, item->label.c_str(), r, ETO_OPAQUE);

        if (item->checked)
        {
            RECT r = dis->rcItem;
            r.left += item->theme->padding();
            r.top += item->theme->padding();
            r.right = r.left + bmw +2;
            r.bottom = r.top + bmh +2;
            dcv.frameRect(r, item->theme->selectedBkgBrush());
        }

        dcv.setBkColor(oldBkg);
        dcv.setTextColor(oldTxt);

        return TRUE;
    }

    inline LRESULT MenuItem::wmMeasureItem(MEASUREITEMSTRUCT* mis)
    {
        MTL::DC dc(::GetDesktopWindow());
        MTL::dc_view dcv(*dc);

        dcv.select(theme->font());

        MTL::MenuItem* item = (MTL::MenuItem*)(mis->itemData);
        auto theme = item->theme;

        SIZE size = { 0,0 };
        ::GetTextExtentPoint32(*dcv, item->label.c_str(), (int)item->label.size(), &size);
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