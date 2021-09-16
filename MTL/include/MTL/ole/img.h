#pragma once

#include "MTL/punk.h"
#include "MTL/win/gdi.h"
#include "MTL/obj/impl.h"
#include "MTL/persist/stream.h"
#include <wincodec.h>

namespace MTL {

    namespace detail {
        inline MTL::punk<IWICBitmapSource> LoadBitmapSourceFromPath(const std::wstring& path)
        {
            punk<IWICBitmapSource> ipBitmap;

            punk<IWICImagingFactory> wic;
            HR hr = wic.createObject(CLSID_WICImagingFactory);

            punk<IWICBitmapDecoder> ipDecoder;
            hr = wic->CreateDecoderFromFilename(path.c_str(), 0, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &ipDecoder);

//            punk<IWICBitmapDecoder> ipDecoder;
            //HR hr = ::CoCreateInstance(CLSID_WICPngDecoder, NULL, CLSCTX_INPROC_SERVER, __uuidof(ipDecoder), reinterpret_cast<void**>(&ipDecoder));
 //           HR hr = ipDecoder.createObject(CLSID_WICJpegDecoder);

          //  MTL::Stream stream((MTL::Path(path)));
           // hr = ipDecoder->Initialize(*stream, WICDecodeMetadataCacheOnLoad);

            UINT nFrameCount = 0;
            hr = ipDecoder->GetFrameCount(&nFrameCount);

            if (nFrameCount != 1)
                throw E_UNEXPECTED;

            // load the first frame
            IWICBitmapFrameDecode* ipFrame = NULL;
            hr = ipDecoder->GetFrame(0, &ipFrame);

            hr = ::WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, ipFrame, &ipBitmap);
            return ipBitmap;
        }

        inline MTL::punk<IWICBitmapSource> LoadBitmapSourceFromStream(IStream* stream, CLSID decoder)
        {
            punk<IWICBitmapSource> ipBitmap;

            punk<IWICBitmapDecoder> ipDecoder;
            HR hr = ipDecoder.createObject(decoder);

            hr = ipDecoder->Initialize(stream, WICDecodeMetadataCacheOnLoad);

            UINT nFrameCount = 0;
            hr = ipDecoder->GetFrameCount(&nFrameCount);

            if (nFrameCount != 1)
                throw E_UNEXPECTED;

            // load the first frame
            IWICBitmapFrameDecode* ipFrame = NULL;
            hr = ipDecoder->GetFrame(0, &ipFrame);

            hr = ::WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, ipFrame, &ipBitmap);
            return ipBitmap;
        }


        inline HBITMAP CreateHBITMAPfromSource(IWICBitmapSource* ipBitmap);

        inline HBITMAP CreateHBITMAPfromSource(IWICBitmapSource* ipBitmap)
        {
            HBITMAP hbmp = NULL;

            UINT width = 0;
            UINT height = 0;
            HR hr = ipBitmap->GetSize(&width, &height);
            if (width == 0 || height == 0)
            {
                throw E_UNEXPECTED;
            }

            // prepare structure giving bitmap information (negative height indicates a top-down DIB)

            BITMAPINFO bminfo;
            ::ZeroMemory(&bminfo, sizeof(bminfo));
            bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bminfo.bmiHeader.biWidth = width;
            bminfo.bmiHeader.biHeight = -((LONG)height);
            bminfo.bmiHeader.biPlanes = 1;
            bminfo.bmiHeader.biBitCount = 32;
            bminfo.bmiHeader.biCompression = BI_RGB;

            // create a DIB section that can hold the image
            void* pvImageBits = NULL;
            //HDC hdcScreen = ::GetDC(NULL);
            MTL::DC hdcScreen(NULL);
            hbmp = ::CreateDIBSection(*hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, NULL, 0);
            // ::ReleaseDC(NULL, hdcScreen);
            if (hbmp == NULL)
            {
                throw E_FAIL;
            }

            // extract the image into the HBITMAP
            const UINT cbStride = width * 4;
            const UINT cbImage = cbStride * height;
            HRESULT r = ipBitmap->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE*>(pvImageBits));
            if (r != S_OK)
            {
                // couldn't extract image; delete HBITMAP
                ::DeleteObject(hbmp);
                throw E_FAIL;
                hbmp = NULL;
            }

            return hbmp;
        }

    } // end namespace detail

    class Bitmap
    {
    public:
        Bitmap()
        {}

        Bitmap(HBITMAP b)
            : hBitmap_(b)
        {}

        Bitmap(const Bitmap& rhs) = delete;
        Bitmap(Bitmap&& rhs)
            : hBitmap_(rhs.hBitmap_)
        {
            rhs.hBitmap_ = nullptr;
        }

        Bitmap& operator=(const Bitmap& rhs) = delete;

        Bitmap& operator=(Bitmap&& rhs)
        {
            if (this == rhs.addressOf())
            {
                return *this;
            }

            hBitmap_ = rhs.hBitmap_;
            rhs.hBitmap_ = nullptr;

            return *this;
        }

        ~Bitmap()
        {
            if (hBitmap_)
            {
                ::DeleteObject(hBitmap_);
            }
        }

        HBITMAP* operator&()
        {
            return &hBitmap_;
        }

        HBITMAP operator*()
        {
            return hBitmap_;
        }

        const Bitmap* addressOf() const
        {
            return this;
        }

        static Bitmap makeTransparentDIBSection(int w, int h, UINT32 col = 0x00000000)
        {
            DC desktopDC;
            CompatibleDC cdc_dest(*desktopDC);

            BITMAPINFO bmi;
            ::ZeroMemory(&bmi, sizeof(BITMAPINFO));

            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = w;
            bmi.bmiHeader.biHeight = h;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;         // four 8-bit components 
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biSizeImage = w * h * 4;

            VOID* pvBits = 0;
            HBITMAP dest = ::CreateDIBSection(*desktopDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);

            dc_view dcvd(*cdc_dest);
            dcvd.select(dest);

            int x, y;

            for (y = 0; y < h; y++)
                for (x = 0; x < w; x++)
                    ((UINT32*)pvBits)[x + y * w] = col;

            return dest;
        }

        HBITMAP detach()
        {
            HBITMAP result = hBitmap_;
            hBitmap_ = nullptr;
            return result;
        }

    private:

        HBITMAP hBitmap_ = nullptr;
    };

    inline Bitmap LoadPicture(const std::wstring& path)
    {
        HBITMAP hbmpSplash = NULL;

        punk<IWICBitmapSource> ipBitmap = MTL::detail::LoadBitmapSourceFromPath(path);
        if (!ipBitmap)
            throw E_FAIL;

        hbmpSplash = MTL::detail::CreateHBITMAPfromSource(*ipBitmap);

        return hbmpSplash;
    }

    inline Bitmap LoadPicture(IStream* stream, CLSID decoder = CLSID_WICPngDecoder)
    {
        HBITMAP hbmpSplash = NULL;

        punk<IWICBitmapSource> ipBitmap = MTL::detail::LoadBitmapSourceFromStream(stream,decoder);
        if (!ipBitmap)
            throw E_FAIL;

        hbmpSplash = MTL::detail::CreateHBITMAPfromSource(*ipBitmap);

        return hbmpSplash;
    }

    inline Bitmap LoadPictureFromResource(int resId, const wchar_t* type = L"PNG", CLSID decoder = CLSID_WICPngDecoder)
    {
        HBITMAP hbmpSplash = NULL;

        HRSRC hrsrc = ::FindResource(MTL::module_instance(), MAKEINTRESOURCE(resId), type);
        if (!hrsrc)
        {
            return hbmpSplash;
        }

        size_t s = s = ::SizeofResource(MTL::module_instance(), hrsrc);
        HGLOBAL hglob = ::LoadResource(MTL::module_instance(), hrsrc);
        void* pvoid = ::LockResource(hglob);

        std::string data((char*)pvoid, s);
        Stream stream(data);

        return LoadPicture(*stream, decoder);
    }

    class BitmapCache
    {
    public:

        BitmapCache()
        {
            imgPath_ = pathToSelfDirectory();
        }


        void imgPath(const std::wstring& path)
        {
            imgPath_ = path;
        }

        void load(int resourceId)
        {
            MTL::Bitmap bmp = MTL::LoadPictureFromResource(resourceId);

            if (!*bmp) return;

            BITMAP bm;
            ::GetObject(*bmp, sizeof(bm), &bm);

            id2bitmap_[resourceId] = *bmp;
            bitmaps_.push_back(std::move(bmp));
            idSize2bitmap_[resourceId][bm.bmWidth][bm.bmHeight] = *bmp;
        }

        void load(const wchar_t* path)
        {
            std::wostringstream woss;
            woss << imgPath_ << L"\\" << path;
            MTL::Bitmap bmp = MTL::LoadPicture(woss.str());
            if (!*bmp) return;

            BITMAP bm;
            ::GetObject(*bmp, sizeof(bm), &bm);
            path2bitmap_[path] = *bmp;
            bitmaps_.push_back(std::move(bmp));
            pathSize2bitmap_[path][bm.bmWidth][bm.bmHeight] = *bmp;
        }

        void load(int id, const wchar_t* path)
        {
            std::wostringstream woss;
            woss << imgPath_ << L"\\" << path;
            MTL::Bitmap bmp = MTL::LoadPicture(woss.str());
            if (!*bmp) return;

            BITMAP bm;
            ::GetObject(*bmp, sizeof(bm), &bm);
            path2bitmap_[path] = *bmp;
            id2bitmap_[id] = *bmp;
            bitmaps_.push_back(std::move(bmp));
            idSize2bitmap_[id][bm.bmWidth][bm.bmHeight] = *bmp;
            pathSize2bitmap_[path][bm.bmWidth][bm.bmHeight] = *bmp;
        }

        void load(const std::vector<std::tuple<int, const wchar_t*>>& bmps)
        {
            for (auto& bmp : bmps)
            {
                int id = std::get<0>(bmp);
                const wchar_t* path = std::get<1>(bmp);
                load(id, path);
            }
        }

        HBITMAP get(const wchar_t* path)
        {
            if (path2bitmap_.count(path) == 0)
            {
                load(path);
                if (path2bitmap_.count(path) == 0) return nullptr;
            }
            return path2bitmap_[path];
        }

        HBITMAP getById(int id)
        {
            if (id2bitmap_.count(id) == 0)
            {
                load(id);
                if (id2bitmap_.count(id) == 0) return nullptr;
            }
            return id2bitmap_[id];
        }

        HBITMAP get(const wchar_t* path, int w, int h)
        {
            if (path2bitmap_.count(path) == 0)
            {
                load(path);
                if (path2bitmap_.count(path) == 0) return nullptr;
            }

            HBITMAP bmp = pathSize2bitmap_[path][w][h];
            if (bmp)
            {
                return bmp;
            }

            bmp = path2bitmap_[path];
            if (!bmp)
            {
                return nullptr;
            }

            MTL::Bitmap dest = Bitmap::makeTransparentDIBSection(w, h);
            pathSize2bitmap_[path][w][h] = *dest;

            CompatibleDC cdc_dest;            
            dc_view dcvd(*cdc_dest);
            dcvd.select(*dest);
            dcvd.alphaBlend(bmp, 0, 0, w, h, 0xff);
            
            HBITMAP result = *dest;
            bitmaps_.push_back(std::move(dest));
            return result;
        }

        HBITMAP get(int id, int w, int h)
        {
            if (id2bitmap_.count(id) == 0)
            {
                load(id);
                if (id2bitmap_.count(id) == 0) return nullptr;
            }

            HBITMAP bmp = idSize2bitmap_[id][w][h];
            if (bmp)
            {
                return bmp;
            }

            bmp = id2bitmap_[id];
            if (!bmp)
            {
                return nullptr;
            }

            MTL::Bitmap dest = Bitmap::makeTransparentDIBSection(w, h);
            idSize2bitmap_[id][w][h] = *dest;

            CompatibleDC cdc_dest;
            dc_view dcvd(*cdc_dest);
            dcvd.select(*dest);
            dcvd.alphaBlend(bmp, 0, 0, w, h, 0xff);

            HBITMAP result = *dest;
            bitmaps_.push_back(std::move(dest));
            return result;
        }

    private:

        std::wstring imgPath_;
        std::vector<MTL::Bitmap> bitmaps_;
        std::map<std::wstring, HBITMAP> path2bitmap_;
        std::map<int, HBITMAP> id2bitmap_;
        std::map<int, std::map<int, std::map<int, HBITMAP>>> idSize2bitmap_;
        std::map<std::wstring, std::map<int, std::map<int, HBITMAP>>> pathSize2bitmap_;
    };



    inline MTL::BitmapCache& bitmapCache()
    {
        static MTL::BitmapCache bmp;
        return bmp;
    }



    class UI
    {
    public:

        struct Item
        {
            int id;
            std::string str; 
            std::wstring label;
            std::wstring img;
        };

        void add( const std::vector<Item>& items)
        {
            for (auto& item : items)
            {
                id2string[item.id] = MTL::to_wstring(item.str);
                string2id[MTL::to_wstring(item.str)] = item.id;

                if (!item.label.empty())
                {
                    id2label[item.id] = item.label;
                }

                if (item.img.empty())
                {
                    MTL::bitmapCache().load(item.id);
                }
                else
                {
                    MTL::bitmapCache().load(item.id, item.img.c_str());
                }
            }
        }

        void add(int id, const std::string& str)
        {
            id2string[id] = MTL::to_wstring(str);
            string2id[MTL::to_wstring(str)] = id;

            MTL::bitmapCache().load(id);
        }

        void add(int id, const std::string& str, const std::wstring& label, const std::wstring& img)
        {
            id2string[id] = MTL::to_wstring(str);
            id2label[id] = label;
            string2id[MTL::to_wstring(str)] = id;

            MTL::bitmapCache().load(id, img.c_str());
        }

        void add(int id, const std::string& str, const std::wstring& label)
        {
            id2string[id] = MTL::to_wstring(str);
            id2label[id] = label;
            string2id[MTL::to_wstring(str)] = id;

            MTL::bitmapCache().load(id);
        }

        const std::wstring& idString(int id) 
        {
            if (id2string.count(id) == 0) return L"";
            return id2string[id];
        }

        const std::wstring& label(int id)
        {
            static std::wstring empty = L"";
            if (id2label.count(id) == 0) return empty;
            return id2label[id];
        }

        int id(std::wstring id)
        {
            if (string2id.count(id) == 0) return 0;
            return string2id[id];
        }

        HBITMAP bitmap(int id)
        {
            return bitmapCache().getById(id);
        }

        HBITMAP bitmap(int id, int w, int h)
        {
            return bitmapCache().get(id,w,h);
        }

    private:

        std::map<int, std::wstring> id2string;
        std::map<int, std::wstring> id2label;
        std::map<std::wstring, int> string2id;
    };

    inline UI& ui()
    {
        static UI gui;
        return gui;
    }

    class MenuBuilder
    {
    public:

        std::shared_ptr<ColorTheme> colorTheme;

        struct Item
        {
            Item(int i)
                : id(i), label(ui().label(id).c_str())
            {} 

            Item(int i, bool check)
                : id(i), label(ui().label(id).c_str()),checked(check)
            {}

            Item(int i, const wchar_t* l)
                : id(i), label(l)
            {}

            Item(int i, bool check, const wchar_t* l)
                : id(i), label(l), checked(check)
            {}

            Item(int i, const wchar_t* l, const std::vector<Item>& subMenu)
                : id(i), label(l), items(subMenu)
            {}

            Item(int i, const std::vector<Item>& subMenu)
                : id(i), label(ui().label(id).c_str()), items(subMenu)
            {}

            int id;
            const wchar_t* label;
            std::vector<Item> items;
            bool enabled = true;
            bool checked = false;
        };

        MenuBuilder(Menu& m, int w = 32, int h = 32)
            : menu_(m), w_(w), h_(h)
        {
        }

        void add(const std::vector<Item>& items, bool toplevel = true)
        {
            if (!toplevel)
            {
                menu_.colorTheme_ = colorTheme;
            }
            else
            {
                menu_.hookColorTheme(colorTheme);
            }

            for (auto& item : items)
            {
                HBITMAP bmp = MTL::bitmapCache().get(item.id, w_, h_);

                if (item.items.empty())
                {
                    auto smi = std::make_shared<MenuItem>(item.id, item.label, true, false, nullptr, bmp);
                    menu_.add(smi);
                    if (item.checked) 
                    {
                        menu_.item(item.id).check(true);
                    }
                }
                else
                {
                    Menu popUp;
                    popUp.createPopUp();

                    MenuBuilder mb(popUp, w_, h_);
                    mb.colorTheme = colorTheme;
                    mb.add(item.items, false);

                    auto smi = std::make_shared<MenuItem>(item.id, item.label, true, false, *popUp, bmp);
                    menu_.add(smi);
                    popUp.detach();
                    menu_.subMenus[item.id] = std::move(popUp);
                }
            }
        }

        void addBitmaps()
        {
            bitmapify(*menu_, w_, h_);
        }

        static void bitmapify(HMENU hmenu, int w, int h)
        {
            MTL::Menu menu(hmenu, false);
            int c = menu.count();
            for (int i = 0; i < c; i++)
            {
                MTL::MenuItem item = menu.item(i);
                int id = item.id;

                if (item.subMenu)
                {
                    bitmapify(item.subMenu, w, h);
                }
                else
                {
                    HBITMAP bmp = MTL::bitmapCache().get(id, w, h);
                    if (bmp)
                    {
                        menu.addBitmap(i, bmp);
                    }
                }
            }
        }

    private:
        Menu& menu_;
        int w_ = 32;
        int h_ = 32;
    };



}

// eeek C helper macros

#define MTL_ID(id) id,#id

