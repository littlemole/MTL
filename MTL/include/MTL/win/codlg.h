#pragma once

#include <mtl/punk.h>
#include "mtl/win/wnd.h"
#include "mtl/win/enc.h"
#include <mtl/win32/uni.h>
#include <mtl/win32/mem.h>
#include <mtl/util/path.h>

#include <commdlg.h>
#include "Shobjidl.h"
#include "KnownFolders.h"

#define WM_SEARCH_MSG ::MTL::SearchDlg::getSearchMsg()

namespace mtl {


    class search_dlg 
    {
    public:
        search_dlg()
        {
            what_.alloc(1024);
            with_.alloc(1024);
            ::ZeroMemory(&frp_, sizeof(frp_));
            frp_.lStructSize = sizeof(FINDREPLACE);
            frp_.lpstrFindWhat = &what_;
            frp_.wFindWhatLen = (WORD)what_.size();
            frp_.lpstrReplaceWith = &with_;
            frp_.wReplaceWithLen = (WORD)with_.size();
            WmSearch();
        }

        HWND find(HWND parent, DWORD flags = FR_DOWN, const wchar_t* what = 0)
        {
            if (what)
            {
                wcscpy_s( &what_, wcslen(what)+1,what);
            }
            frp_.lpstrReplaceWith = 0;
            frp_.Flags = flags;
            frp_.wReplaceWithLen = 0;
            frp_.hwndOwner = parent;
            HWND hWnd = ::FindText(&frp_);
            if (hWnd == 0)
            {
                DWORD err = CommDlgExtendedError();
                std::wostringstream woss;
                woss << L"e:" << err << std::endl;
                ::OutputDebugString(woss.str().c_str());
            }
            modeless_dlg().add(hWnd);
            return hWnd;
        }

        HWND replace(HWND parent, DWORD flags = FR_DOWN, const wchar_t* what = 0, const wchar_t* with = 0)
        {
            if (what)
            {
                wcscpy_s(&what_, wcslen(what)+1, what);
            }
            if (with)
            {
                wcscpy_s(&with_, wcslen(with)+1, with);
            }
            frp_.Flags = flags;
            frp_.hwndOwner = parent;
            HWND hWnd = ::ReplaceText(&frp_);
            modeless_dlg().add(hWnd);
            return hWnd;
        }

    private:
        FINDREPLACE frp_;
        wbuff what_;
        wbuff with_;
    };

    class open_dlg
    {
    public:

        static const wchar_t* filter()
        {
            static const wchar_t f[] = L"All Files (*.*)\0*.*\0\0";
            return f;
        }


        open_dlg(HWND parent)
        {
            ::ZeroMemory(&of_, sizeof(of_));
            of_.lStructSize = sizeof(OPENFILENAME);
            of_.hwndOwner = parent;
            of_.lpstrFilter = filter();
            dlg_ = 0;
        }

        void set_filter(const wchar_t* filter)
        {
            of_.lpstrFilter = filter;
        }

        const std::wstring filename(int i = 0)
        {
            if (of_.Flags & OFN_ALLOWMULTISELECT)
            {
                return filenames_[i];
            }
            return filename_;
        }

        void  filename(const std::wstring& s)
        {
            filename_ = s;
        }

        int nSelections()
        {
            if (of_.Flags & OFN_ALLOWMULTISELECT)
            {
                return (int)filenames_.size();
            }
            return 1;
        }

        int  index()
        {
            return of_.nFilterIndex;
        }

        void index(int i)
        {
            of_.nFilterIndex = i;
        }

        BOOL open(int flags = OFN_HIDEREADONLY)
        {
            reset();
            wchar_t buf[1024];
            wchar_t buf2[1024];
            buf[0] = 0;
            buf2[0] = 0;

            of_.lpstrFile = buf;
            of_.lpstrFileTitle = buf2;
            of_.nMaxFile = 1024;
            of_.nMaxFileTitle = 1024;
            of_.Flags = flags;
            of_.lCustData = (LPARAM)this;

            if (filename_.size() > 0)
                ::memcpy(buf, filename_.c_str(), (filename_.size() + 1) * sizeof(wchar_t));

            BOOL ret = ::GetOpenFileName(&of_);

            if (!ret)
                return ret;

            if (flags & OFN_ALLOWMULTISELECT)
            {
                std::wstring p = std::wstring(of_.lpstrFile, of_.nFileOffset - 1);
                wchar_t* c = of_.lpstrFile + of_.nFileOffset;
                wchar_t* b = c;
                if (*(c - 1) != 0)
                {
                    filenames_.push_back(p + L"\\" + std::wstring(buf + of_.nFileOffset));
                }
                else
                    while (!((*c == 0) && (*(c - 1) == 0)) && c <= of_.lpstrFile + (int)(of_.nMaxFile * sizeof(wchar_t)))
                    {
                        if (*c == 0)
                        {
                            std::wstring s(p);
                            s.append(L"\\");
                            if (c - b > 0 && *b)
                            {
                                std::wstring f(b, int(c - b));
                                s.append(f);
                                filenames_.push_back(s);
                            }
                            b = c + 1;
                        }
                        c++;
                    }
                return ret;
            }

            filename_ = buf;
            filetitle_ = buf2;
            if (of_.nFileExtension != 0)
                extension_ = (wchar_t*)(of_.lpstrFile + of_.nFileExtension);
            else
                extension_ = L"";

            return ret;
        }

        BOOL  save(int flags = OFN_HIDEREADONLY)
        {
            reset();
            wchar_t buf[1024];
            wchar_t buf2[1024];
            buf[0] = 0;
            buf2[0] = 0;

            of_.lpstrFile = buf;
            of_.lpstrFileTitle = buf2;
            of_.nMaxFile = 1024;
            of_.nMaxFileTitle = 1024;
            of_.Flags = flags;
            of_.lCustData = (LPARAM)this;

            if (filename_.size() > 0)
                ::memcpy(buf, filename_.c_str(), (filename_.size() + 1) * sizeof(wchar_t));

            BOOL ret = ::GetSaveFileName(&of_);

            if (!ret)
                return ret;

            filename_ = buf;
            filetitle_ = buf2;
            if (of_.nFileExtension != 0)
                extension_ = (wchar_t*)(of_.lpstrFile + of_.nFileExtension);
            else
                extension_ = L"";

            return ret;
        }


        bool read_only()
        {
            return (of_.Flags & OFN_READONLY) != 0;
        }

    protected:

        void reset()
        {
           // filename_ = L"";
            extension_ = L"";
            filetitle_ = L"";
            of_.nFilterIndex = 0;
        }

        HWND dlg_;
        std::vector<std::wstring> filenames_;

        std::wstring			filename_;
        std::wstring			extension_;
        std::wstring			filetitle_;
        OPENFILENAME		    of_;
    };

    class pick_font
    {
    public:

        pick_font(DWORD styles = 0)
        {
            ::ZeroMemory(&lf_, sizeof(LOGFONT));
            
            std::wstring ws(L"Modern");
            wcsncpy_s(lf_.lfFaceName, ws.c_str(), ws.size() + 1);
            
            ::ZeroMemory(&cf_, sizeof(CHOOSEFONT));
            cf_.lStructSize = sizeof(cf_);
            cf_.Flags = styles;// CF_INITTOLOGFONTSTRUCT | CF_NOSTYLESEL
            cf_.hInstance = module_instance();
            cf_.lpLogFont = &lf_;
           // cf_.nFontType
        }

        LOGFONT* font()
        {
            return &lf_;
        }

        bool choose(HWND owner)
        {
            cf_.hwndOwner = owner;
            return ::ChooseFont(&cf_);
        }

    private:
        LOGFONT lf_;
        CHOOSEFONT cf_;
    };

    class pick_folder
    {
    public:

        pick_folder(
            const GUID& startFolder = FOLDERID_Desktop, 
            DWORD options = FOS_ALLNONSTORAGEITEMS | FOS_NOVALIDATE /*| mol::v7::FOS_FORCEFILESYSTEM*/ | FOS_PICKFOLDERS | FOS_ALLOWMULTISELECT
        )
            : startFolder_(startFolder), options_(options)
        {}

        std::wstring choose(HWND hwnd)
        {
            punk<IFileDialog> fd;

            HR hr = fd.create_object(CLSID_FileOpenDialog);

            DWORD dwOptions;
            hr = fd->GetOptions(&dwOptions);
            hr = fd->SetOptions(dwOptions | options_);

            punk<IShellItem> desktop;
            hr = ::SHCreateItemInKnownFolder(startFolder_, 0, 0, IID_IShellItem, (void**)&desktop);
            hr = fd->SetDefaultFolder(*desktop);

            // Show the dialog
            HRESULT r = fd->Show(hwnd);

            if (r == S_OK)
            {
                // Obtain the result of the user's interaction with the dialog.
                punk<IShellItem> siResult;
                r = fd->GetResult(&siResult);
                if (r == S_OK)
                {
                    // Do something with the result.

                    co_str buf;
                    r = siResult->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &buf);
                    if (r == S_OK)
                    {
                        return buf.str();
                    }
                }
            }
            return L"";
        }

    private:
        const GUID& startFolder_;
        DWORD options_;
    };


    template<class T>
    class file_dialog
    {
    public:
        file_dialog(int options = 0)
            : options_(options)
        {}

        T& filter(const std::vector<std::pair<std::wstring,std::wstring>>& filter)
        {
            filter_ = filter;
            return *(T*)this;
        }

        template<class F>
        T&  customize(F fun)
        {
            punk<IFileDialogCustomize> c(fd_);
            fun(c);
            return *(T*)this;
        }

        DWORD choice(DWORD what)
        {
            DWORD result = 0;
            punk<IFileDialogCustomize> fdc(fd_);
            HR hr = fdc->GetSelectedControlItem(what, &result);
            return result;
        }

        UINT type()
        {
            UINT result = 0;
            HR hr = fd_->GetFileTypeIndex(&result);
            return result;
        }

        const std::vector<std::wstring>& paths()
        {
            return paths_;
        }

        const std::wstring& path()
        {
            if (paths_.empty())
            {
                static std::wstring empty = L"";
                return empty;
            }
            return paths_[0];
        }

        HRESULT path(const std::wstring& path)
        {
            HRESULT hr = S_OK;
            if (!path.empty())
            {
                hr = fd_->SetFileName(path.c_str());
                if (hr != S_OK)
                    return hr;

                punk<IShellItem> shit;
                hr = ::SHCreateItemFromParsingName(
                    mtl::path(path).parentDir().str().c_str(),
                    NULL,
                    IID_IShellItem,
                    (void**)&shit
                );

                if (hr != S_OK)
                    return hr;

                hr = fd_->SetFolder(*shit);
                if (hr != S_OK)
                    return hr;

            }

            return hr;
        }

    protected:

        HRESULT init(int options, REFCLSID clsid)
        {
            fd_.release();
            paths_.clear();

            HRESULT hr = fd_.create_object(clsid);
            if (hr != S_OK)
                return hr;

            DWORD dwFlags;
            hr = fd_->GetOptions(&dwFlags);
            if (hr != S_OK)
                return hr;

            hr = fd_->SetOptions(/*dwFlags |*/ options);
            if (hr != S_OK)
                return hr;

            return hr;
        }



        punk<IFileDialog> fd_;
        int options_ = 0;
        std::vector<std::wstring> paths_;
        std::vector<std::pair<std::wstring, std::wstring>> filter_;
    };


    class file_open_dialog : public file_dialog< file_open_dialog>
    {
    public:
        file_open_dialog(int options = 0)
            : file_dialog<file_open_dialog>(options)
        {
            HR hr = init(options_, CLSID_FileOpenDialog);
        }

        HRESULT open(HWND parent)
        {
            std::vector< COMDLG_FILTERSPEC> spec;

            for (auto& f : filter_)
            {
                spec.push_back({ f.first.c_str(), f.second.c_str() });
            }
            if (!spec.empty())
            {
                HRESULT hr = fd_->SetFileTypes( (UINT) spec.size(), &spec[0]);
                if (hr != S_OK)
                    return hr;
            }

            HRESULT hr = fd_->Show(parent);
            if (hr != S_OK)
                return hr;

            punk<IFileOpenDialog> fod(fd_);
            if (!fod)
                return hr;

            punk<IShellItemArray> psiResult;
            hr = fod->GetResults(&psiResult);
            if (hr != S_OK)
                return hr;

            DWORD numItems = 0;
            psiResult->GetCount(&numItems);
            for (DWORD i = 0; i < numItems; i++)
            {
                punk<IShellItem> shit;
                hr = psiResult->GetItemAt(i, &shit);
                if (hr != S_OK)
                    return hr;

                co_str pszFilePath;
                hr = shit->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (hr == S_OK)
                {
                    paths_.push_back(std::wstring(*pszFilePath));
                }
                else
                {
                    hr = shit->GetDisplayName(SIGDN_URL, &pszFilePath);
                    if (hr == S_OK)
                    {
                        paths_.push_back(std::wstring(*pszFilePath));
                    }
                }
            }

            return S_OK;
        }
    };

    class file_save_dialog : public file_dialog<file_save_dialog>
    {
    public:
        file_save_dialog(int options = 0)
            : file_dialog<file_save_dialog>(options)
        {
            HR hr = init(options_, CLSID_FileSaveDialog);
        }

        HRESULT save(HWND parent)
        {
            std::vector< COMDLG_FILTERSPEC> spec;

            for (auto& f : filter_)
            {
                spec.push_back({ f.first.c_str(), f.second.c_str() });
            }
            if (!spec.empty())
            {
                HRESULT hr = fd_->SetFileTypes( (UINT) spec.size(), &spec[0]);
                if (hr != S_OK)
                    return hr;
            }

            HRESULT hr = fd_->Show(parent);
            if (hr != S_OK)
                return hr;

            punk<IShellItem> psiResult;
            hr = fd_->GetResult(&psiResult);
            if (hr != S_OK)
                return hr;

            co_str pszFilePath;
            hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
            paths_.push_back(std::wstring(*pszFilePath));

            return S_OK;
        }

    };

}

