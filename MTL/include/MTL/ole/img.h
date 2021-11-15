#pragma once

#include "mtl/sdk.h"
#include "mtl/punk.h"
#include "mtl/win/gdi.h"
//#include "mtl/obj/impl.h"
#include "mtl/persist/stream.h"
#include <wincodec.h>

namespace mtl {

    namespace detail {
        inline mtl::punk<IWICBitmapSource> load_bitmap_source_from_path(const std::wstring& path)
        {
            punk<IWICBitmapSource> ipBitmap;

            punk<IWICImagingFactory> wic;
            HR hr = wic.create_object(CLSID_WICImagingFactory);

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

        inline punk<IWICBitmapSource> load_bitmap_source_from_stream(IStream* stream, CLSID decoder)
        {
            punk<IWICBitmapSource> ipBitmap;

            punk<IWICBitmapDecoder> ipDecoder;
            HR hr = ipDecoder.create_object(decoder);

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


//        inline HBITMAP create_bitmap_from_source(IWICBitmapSource* ipBitmap);

        inline HBITMAP create_bitmap_from_source(IWICBitmapSource* ipBitmap)
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
            wnd_dc hdcScreen(NULL);
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

    class bitmap
    {
    public:
        bitmap()
        {}

        bitmap(HBITMAP b)
            : hBitmap_(b)
        {}

        bitmap(const bitmap& rhs) = delete;
        bitmap(bitmap&& rhs)
            : hBitmap_(rhs.hBitmap_)
        {
            rhs.hBitmap_ = nullptr;
        }

        bitmap& operator=(const bitmap& rhs) = delete;

        bitmap& operator=(bitmap&& rhs)
        {
            if (this == rhs.address_of())
            {
                return *this;
            }

            hBitmap_ = rhs.hBitmap_;
            rhs.hBitmap_ = nullptr;

            return *this;
        }

        ~bitmap()
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

        const bitmap* address_of() const
        {
            return this;
        }

        static bitmap make_transparent_dib_section(int w, int h, UINT32 col = 0x00000000)
        {
            wnd_dc desktopDC;
            compatible_dc cdc_dest(*desktopDC);

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

            dc dcvd(*cdc_dest);
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

    inline bitmap load_picture(const std::wstring& path)
    {
        HBITMAP hbmpSplash = NULL;

        punk<IWICBitmapSource> ipBitmap = mtl::detail::load_bitmap_source_from_path(path);
        if (!ipBitmap)
            throw E_FAIL;

        hbmpSplash = mtl::detail::create_bitmap_from_source(*ipBitmap);

        return hbmpSplash;
    }

    inline bitmap load_picture(IStream* stream, CLSID decoder = CLSID_WICPngDecoder)
    {
        HBITMAP hbmpSplash = NULL;

        punk<IWICBitmapSource> ipBitmap = mtl::detail::load_bitmap_source_from_stream(stream,decoder);
        if (!ipBitmap)
            throw E_FAIL;

        hbmpSplash = mtl::detail::create_bitmap_from_source(*ipBitmap);

        return hbmpSplash;
    }

    inline bitmap load_picture_from_resource(int resId, const wchar_t* type = L"PNG", CLSID decoder = CLSID_WICPngDecoder)
    {
        HBITMAP hbmpSplash = NULL;

        HRSRC hrsrc = ::FindResource(mtl::module_instance(), MAKEINTRESOURCE(resId), type);
        if (!hrsrc)
        {
            hrsrc = ::FindResource(mtl::module_instance(), MAKEINTRESOURCE(resId), L"IMAGE");
        }
        if (!hrsrc)
        {
            return hbmpSplash;
        }

        size_t s = s = ::SizeofResource(mtl::module_instance(), hrsrc);
        HGLOBAL hglob = ::LoadResource(mtl::module_instance(), hrsrc);
        void* pvoid = ::LockResource(hglob);

        std::string data((char*)pvoid, s);
        stream strm(data);

        return load_picture(*strm, decoder);
    }

    class bitmap_cache
    {
    public:

        bitmap_cache()
        {
            imgPath_ = path_to_self_directory();
        }


        void img_path(const std::wstring& path)
        {
            imgPath_ = path;
        }

        void load(int resourceId,  CLSID decoder = CLSID_WICPngDecoder, const wchar_t* type = L"PNG")
        {
            load(resourceId, resourceId, decoder, type);
        }

        void load(int image_id, int resourceId, CLSID decoder = CLSID_WICPngDecoder, const wchar_t* type = L"PNG")
        {
            mtl::bitmap bmp = mtl::load_picture_from_resource(resourceId, type, decoder);

            if (!*bmp) return;

            BITMAP bm;
            ::GetObject(*bmp, sizeof(bm), &bm);

            id2bitmap_[image_id] = *bmp;
            idSize2bitmap_[image_id][bm.bmWidth][bm.bmHeight] = *bmp;
            bitmaps_.push_back(std::move(bmp));
        }

        void load(const wchar_t* path)
        {
            std::wostringstream woss;
            woss << imgPath_ << L"\\" << path;
            mtl::bitmap bmp = mtl::load_picture(woss.str());
            if (!*bmp) return;

            BITMAP bm;
            ::GetObject(*bmp, sizeof(bm), &bm);

            path2bitmap_[path] = *bmp;
            pathSize2bitmap_[path][bm.bmWidth][bm.bmHeight] = *bmp;
            bitmaps_.push_back(std::move(bmp));
        }

        void load(int id, const wchar_t* path)
        {
            std::wostringstream woss;
            woss << imgPath_ << L"\\" << path;
            bitmap bmp = load_picture(woss.str());
            if (!*bmp) return;

            BITMAP bm;
            ::GetObject(*bmp, sizeof(bm), &bm);

            path2bitmap_[path] = *bmp;
            id2bitmap_[id] = *bmp;
            idSize2bitmap_[id][bm.bmWidth][bm.bmHeight] = *bmp;
            pathSize2bitmap_[path][bm.bmWidth][bm.bmHeight] = *bmp;
            bitmaps_.push_back(std::move(bmp));
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

        HBITMAP get(int index)
        {
            int id = index;
            if (aliases_.count(id))
            {
                if (id2bitmap_.count(id) == 0)
                {
                    load(id, aliases_[id]);
                }
            }
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

            bitmap dest = bitmap::make_transparent_dib_section(w, h);
            pathSize2bitmap_[path][w][h] = *dest;

            compatible_dc cdc_dest;            
            dc dcvd(*cdc_dest);
            dcvd.select(*dest);
            dcvd.alpha_blend(bmp, 0, 0, w, h, 0xff);
            
            HBITMAP result = *dest;
            bitmaps_.push_back(std::move(dest));
            return result;
        }

        HBITMAP get(int index, int w, int h)
        {
            int id = index;
            if (aliases_.count(id))
            {
                load(id,aliases_[id]);
            }

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

            bitmap dest = bitmap::make_transparent_dib_section(w, h);
            idSize2bitmap_[id][w][h] = *dest;

            compatible_dc cdc_dest;
            dc dcvd(*cdc_dest);
            dcvd.select(*dest);
            dcvd.alpha_blend(bmp, 0, 0, w, h, 0xff);

            HBITMAP result = *dest;
            bitmaps_.push_back(std::move(dest));
            return result;
        }

        void alias(int src, int dest)
        {
            aliases_[src] = dest;
        }

    private:

        std::wstring imgPath_;
        std::vector<bitmap> bitmaps_;
        std::map<std::wstring, HBITMAP> path2bitmap_;
        std::map<int, HBITMAP> id2bitmap_;
        std::map<int, std::map<int, std::map<int, HBITMAP>>> idSize2bitmap_;
        std::map<std::wstring, std::map<int, std::map<int, HBITMAP>>> pathSize2bitmap_;
        std::map<int, int> aliases_;
    };



    inline bitmap_cache& the_bitmap_cache()
    {
        static bitmap_cache bmp;
        return bmp;
    }



    class ui
    {
    public:

        struct item
        {
            int id;
            std::string str; 
            std::wstring label;
            std::wstring img;
        };

        void add( std::vector<item> new_items)
        {
            for (auto& it : new_items)
            {
                id2string[it.id] = to_wstring(it.str);
                string2id[to_wstring(it.str)] = it.id;

                if(it.label.empty())
                {
                    mtl::wbuff buf(1024);
                    int r = ::LoadString(mtl::module_instance(),it.id,buf, (int)buf.size());
                    if(r)
                    {
                        it.label = buf.toString();
                    }
                }

                if (!it.label.empty())
                {
                    id2label[it.id] = it.label;
                }

                if (it.img.empty())
                {
                    //the_bitmap_cache().load(it.id);
                }
                else
                {
                    the_bitmap_cache().load(it.id, it.img.c_str());
                }
            }
        }

        void add(int id, const std::string& str)
        {
            id2string[id] = to_wstring(str);
            string2id[to_wstring(str)] = id;

            mtl::wbuff buf(1024);
            int r = ::LoadString(mtl::module_instance(), id, buf, (int)buf.size());
            if (r)
            {
                id2label[id] = buf.toString();
            }
        }

        void add_ribbon(int id, const std::wstring& str, int label_id, int image_id = 0)
        {
            id2string[id] = str;
            string2id[str] = id;

            if (label_id)
            {
                mtl::wbuff buf(1024);
                int r = ::LoadString(mtl::module_instance(), label_id, buf, (int)buf.size());
                if (r)
                {
                    id2label[id] = buf.toString();
                }
            }
            if (image_id)
            {
                //the_bitmap_cache().load(id,image_id, CLSID_WICPngDecoder, L"IMAGE");
                the_bitmap_cache().alias(id, image_id);
            }
        }
        /* dosn't do anything with img ?
        void add(int id, const std::string& str, const std::wstring& label, const std::wstring& img)
        {
            id2string[id] = to_wstring(str);
            id2label[id] = label;
            string2id[to_wstring(str)] = id;
        }
        */
        void add(int id, const std::string& str, const std::wstring& label)
        {
            id2string[id] = to_wstring(str);
            id2label[id] = label;
            string2id[to_wstring(str)] = id;
        }

        const std::wstring& id_string(int id) 
        {
            static std::wstring empty = L"";
            if (id2string.count(id) == 0) return empty;
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
            return the_bitmap_cache().get(id);
        }

        HBITMAP bitmap(int id, int w, int h)
        {
            return the_bitmap_cache().get(id,w,h);
        }

    private:

        std::map<int, std::wstring> id2string;
        std::map<int, std::wstring> id2label;
        std::map<std::wstring, int> string2id;
    };

    inline ui& gui()
    {
        static ui the_gui;
        return the_gui;
    }

    class menu_builder
    {
    public:

        std::shared_ptr<color_theme> theme;

        struct item
        {
            item(int i)
                : id(i), label(gui().label(id).c_str())
            {} 

            item(int i, bool check)
                : id(i), label(gui().label(id).c_str()),checked(check)
            {}

            item(int i, const wchar_t* l)
                : id(i), label(l)
            {}

            item(int i, bool check, const wchar_t* l)
                : id(i), label(l), checked(check)
            {}

            item(int i, const wchar_t* l, const std::vector<item>& subMenu)
                : id(i), label(l), items(subMenu)
            {}

            item(int i, const std::vector<item>& subMenu)
                : id(i), label(gui().label(id).c_str()), items(subMenu)
            {}

            int id;
            const wchar_t* label;
            std::vector<item> items;
            bool enabled = true;
            bool checked = false;
        };

        menu_builder(menu& m, int w = 32, int h = 32)
            : menu_(m), w_(w), h_(h)
        {
        }

        void add(const std::vector<item>& items, bool toplevel = true)
        {
            if (!toplevel)
            {
                menu_.colorTheme_ = theme;
            }
            else
            {
                menu_.hook_color_theme(theme);
            }

            for (auto& it : items)
            {
                HBITMAP bmp = gui().bitmap(it.id, w_, h_);

                if (it.items.empty())
                {
                    auto smi = std::make_shared<menu_item>(it.id, it.label, true, false, nullptr, bmp);
                    menu_.add(smi);
                    if (it.checked)
                    {
                        menu_.item(it.id).check(true);
                    }
                }
                else
                {
                    menu popUp;
                    popUp.create_popup();

                    menu_builder mb(popUp, w_, h_);
                    mb.theme = theme;
                    mb.add(it.items, false);

                    auto smi = std::make_shared<menu_item>(it.id, it.label, true, false, *popUp, bmp);
                    menu_.add(smi);
                    popUp.detach();
                    menu_.subMenus[it.id] = std::move(popUp);
                }
            }
        }

        void add_bitmaps()
        {
            bitmapify(*menu_, w_, h_);
        }

        static void bitmapify(HMENU hmenu, int w, int h)
        {
            menu m(hmenu, false);
            int c = m.count();
            for (int i = 0; i < c; i++)
            {
                menu_item it = m.item(i);
                int id = it.id;

                if (it.subMenu)
                {
                    bitmapify(it.subMenu, w, h);
                }
                else
                {
                    HBITMAP bmp = gui().bitmap(id, w, h);
                    if (bmp)
                    {
                        m.add_bitmap(i, bmp);
                    }
                }
            }
        }

    private:
        menu& menu_;
        int w_ = 32;
        int h_ = 32;
    };


    inline mtl::bitmap screenshot(HWND hWnd, int width, int height, bool clientOnly = true)
    {
        mtl::wnd_dc dc(hWnd);
        RECT r;
        if (clientOnly)
        {
            ::GetClientRect(hWnd, &r);
        }
        else
        {
            ::GetWindowRect(hWnd, &r);
        }
        int w = r.right - r.left;
        int h = r.bottom - r.top;
        auto bmp = mtl::bitmap::make_transparent_dib_section(w, h);
        //mtl::bitmap bmp = ::CreateCompatibleBitmap(*dc, w, h);

        {
            mtl::compatible_dc cdc(*dc);
            cdc.select(*bmp);

            BOOL b = ::PrintWindow(hWnd, *cdc, clientOnly);
        }

        double dw = w <= width ? w : width;
        double ratio = (double) w / (double)h;
        double dh = dw / ratio;

        if (dh > height)
        {
            dh = h <= height ? h: height;
            dw =  dh*ratio;
        }

        auto result = mtl::bitmap::make_transparent_dib_section(width, height);
        {
            mtl::compatible_dc cdc(*dc);
            cdc.select(*result);

            cdc.stretch_blit(*bmp, 0, 0, (int) dw, (int) dh);
        }
        return result;
    }
}

// eeek C helper macros

#define MTL_ID(id) id,#id

