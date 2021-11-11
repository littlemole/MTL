#pragma once

#include "mtl/sdk.h"
#include "mtl/punk.h"
#include "mtl/win32/uni.h"
#include "mtl/win32/mem.h"
#include "mtl/util/path.h"
#include "mtl/obj/impl.h"

#include <objidl.h>
#include <ShlObj.h>
#include <Shlwapi.h>

#include <sstream>

namespace mtl {

    class stream_view
    {
    public:

        stream_view()
        {}

        stream_view(IStream* s)
            : stream_(s)
        {}

        ~stream_view()
        {
        }

        size_t size()
        {
            STATSTG stat;
            HRESULT hr = stream_->Stat(&stat, STATFLAG_NONAME);
            if (hr != S_OK)
            {
                return 0;
            }

            return (size_t)stat.cbSize.QuadPart;
        }

        void size(size_t s)
        {
            ULARGE_INTEGER uli;
            uli.QuadPart = s;
            HRESULT hr = stream_->SetSize(uli);
        }

        void reset()
        {
            seek(0);
        }

        void seek(size_t pos)
        {
            LARGE_INTEGER li;
            li.QuadPart = pos;
            HRESULT hr = stream_->Seek(li, 0, 0);
        }

        std::string read()
        {
            cbuff buf(size());
            ULONG fetched = 0;
            HRESULT hr = stream_->Read(buf, (ULONG)buf.size(), &fetched);

            return buf.toString(fetched);
        }

        void write(const std::string& str)
        {
            HRESULT hr = stream_->Write(str.data(), (ULONG)str.size(), 0);
        }

        IStream* operator->()
        {
            return stream_;
        }

        IStream* operator*()
        {
            return stream_;
        }

    private:
        IStream* stream_ = nullptr;
    };


    class stream
    {
    public:

        stream()
        {
            ::CreateStreamOnHGlobal(0, TRUE, &stream_);
        }

        stream(const std::string& data)
        {
            HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, data.size());
            void* dest = ::GlobalLock(hGlobal);
            memcpy(dest, data.data(), data.size());
            ::GlobalUnlock(hGlobal);
            ::CreateStreamOnHGlobal(hGlobal, TRUE, &stream_);
        }

        stream(mtl::path path, DWORD mode = STGM_READWRITE )
        {
            HR hr = ::SHCreateStreamOnFileW(path.str().c_str(), mode, &stream_);

        }

        stream(int id, const std::wstring& type)
        {
            HRSRC hrsrc = ::FindResource(NULL, MAKEINTRESOURCE(id), type.c_str());
            if (hrsrc == NULL)
                throw E_FAIL;

            DWORD dwResourceSize = ::SizeofResource(NULL, hrsrc);
            HGLOBAL hglbImage = ::LoadResource(NULL, hrsrc);
            if (hglbImage == NULL)
                throw E_FAIL;

            global::lock<void*> srcLock(hglbImage);

            global glob(*srcLock, dwResourceSize);
            global::lock<void*> destLock(*glob);
    
            ::CopyMemory(*destLock, *srcLock, dwResourceSize);
            glob.detach();
            HR hr = ::CreateStreamOnHGlobal(*destLock, TRUE, &stream_);
        }

        ~stream()
        {
        }

        operator bool()
        {
            return stream_.interface_ != nullptr;
        }


        stream* address_of()
        {
            return this;
        }

        IStream* operator*()
        {
            return *stream_;
        }

        /*
        IStream** operator&()
        {
            return stream_.operator&();
        }
        */

        size_t size()
        {
            STATSTG stat;
            HRESULT hr = stream_->Stat(&stat, STATFLAG_NONAME);
            if (hr != S_OK)
            {
                return 0;
            }

            return (size_t)stat.cbSize.QuadPart;
        }

        void size(size_t s)
        {
            ULARGE_INTEGER uli;
            uli.QuadPart = s;
            HRESULT hr = stream_->SetSize(uli);
        }

        void reset()
        {
            seek(0);
        }

        void seek(size_t pos)
        {
            LARGE_INTEGER li;
            li.QuadPart = pos;
            HRESULT hr = stream_->Seek(li, 0, 0);
        }

        std::string read()
        {
            cbuff buf(size());
            ULONG fetched = 0;
            HRESULT hr = stream_->Read(buf, (ULONG)buf.size(), &fetched);

            return buf.toString(fetched);
        }

        void write(const std::string& str)
        {
            HRESULT hr = stream_->Write(str.data(), (ULONG)str.size(), 0);
        }

        IStream* operator->()
        {
            return *stream_;
        }


    private:
        punk<IStream> stream_;
    };


    class storage_view
    {
    public:

        storage_view(IStorage* s)
            : storage_(s)
        {}

        ~storage_view()
        {}

        punk<IStorage> create_stg(const std::wstring& name, DWORD mode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE)
        {
            punk<IStorage> result;
            HRESULT hr = storage_->CreateStorage(name.c_str(), mode, 0, 0, &result);
            return result;
        }

        punk<IStream> create_stream(const std::wstring& name, DWORD mode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE)
        {
            punk<IStream> result;
            HRESULT hr = storage_->CreateStream(name.c_str(), mode, 0, 0, &result);
            return result;
        }

        void commit()
        {
            storage_->Commit(0);
        }

        punk<IStorage> open_stg(const std::wstring& name, DWORD mode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE)
        {
            punk<IStorage> result;
            HRESULT hr = storage_->OpenStorage(name.c_str(), 0, mode, 0, 0, &result);
            return result;
        }

        punk<IStream> open_stream(const std::wstring& name, DWORD mode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE)
        {
            punk<IStream> result;
            HRESULT hr = storage_->OpenStream(name.c_str(), 0, mode, 0, &result);
            return result;
        }

        IStorage* operator*()
        {
            return storage_;
        }

        IStorage* operator->()
        {
            return storage_;
        }

    private:
        IStorage* storage_;
    };

    class storage
    {
    public:

        storage()
        {}

        bool create(DWORD mode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE)
        {
            HRESULT hr = ::StgCreateStorageEx(
                0,
                mode,
                STGFMT_DOCFILE,
                0, 0, 0,
                IID_IStorage,
                (void**)&storage_
            );
            return hr == S_OK;
        }

        bool create(const std::wstring& filename, DWORD mode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE)
        {
            HRESULT hr = ::StgCreateStorageEx(
                filename.c_str(),
                mode,
                STGFMT_DOCFILE,
                0, 0, 0,
                IID_IStorage,
                (void**)&storage_
            );
            return hr == S_OK;
        }

        bool open(const std::wstring& filename, DWORD mode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE)
        {
            HRESULT hr = ::StgOpenStorage(filename.c_str(), 0, STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, 0, &storage_);
            return hr == S_OK;
        }

        punk<IStorage> create_stg(const std::wstring& name, DWORD mode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE)
        {
            punk<IStorage> result;
            HRESULT hr = storage_->CreateStorage(name.c_str(), mode, 0, 0, &result);
            return result;
        }

        punk<IStream> create_stream(const std::wstring& name, DWORD mode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE)
        {
            punk<IStream> result;
            HRESULT hr = storage_->CreateStream(name.c_str(), mode, 0, 0, &result);
            return result;
        }

        void commit()
        {
            storage_->Commit(0);
        }

        punk<IStorage> open_stg(const std::wstring& name, DWORD mode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE)
        {
            punk<IStorage> result;
            HRESULT hr = storage_->OpenStorage(name.c_str(), 0, mode, 0, 0, &result);
            return result;
        }

        punk<IStream> open_stream(const std::wstring& name, DWORD mode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE)
        {
            punk<IStream> result;
            HRESULT hr = storage_->OpenStream(name.c_str(), 0, mode, 0, &result);
            return result;
        }

        operator bool()
        {
            return storage_.interface_ != 0;
        }

        IStorage* operator*()
        {
            return *storage_;
        }

        IStorage* operator->()
        {
            return *storage_;
        }

        void release()
        {
            storage_.release();
        }

    private:
        punk<IStorage> storage_;
    };


    class dummy_storage : public implements<stack_object<dummy_storage>(IStorage)>
    {
    public:

        virtual HRESULT __stdcall CreateStream(const OLECHAR* pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream** ppstm) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall OpenStream(const OLECHAR* pwcsName, void* reserved1, DWORD grfMode, DWORD reserved2, IStream** ppstm) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall CreateStorage(const OLECHAR* pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage** ppstg) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall OpenStorage(const OLECHAR* pwcsName, IStorage* pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage** ppstg) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall CopyTo(DWORD ciidExclude, const IID* rgiidExclude, SNB snbExclude, IStorage* pstgDest) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall MoveElementTo(const OLECHAR* pwcsName, IStorage* pstgDest, const OLECHAR* pwcsNewName, DWORD grfFlags) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall Commit(DWORD grfCommitFlags) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall Revert(void) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall EnumElements(DWORD reserved1, void* reserved2, DWORD reserved3, IEnumSTATSTG** ppenum) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall DestroyElement(const OLECHAR* pwcsName) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall RenameElement(const OLECHAR* pwcsOldName, const OLECHAR* pwcsNewName) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall SetElementTimes(const OLECHAR* pwcsName, const FILETIME* pctime, const FILETIME* patime, const FILETIME* pmtime) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall SetClass(REFCLSID clsid) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall SetStateBits(DWORD grfStateBits, DWORD grfMask) override
        {
            return E_NOTIMPL;
        }

        virtual HRESULT __stdcall Stat(STATSTG* pstatstg, DWORD grfStatFlag) override
        {
            return E_NOTIMPL;
        }

    };
}