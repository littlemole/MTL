#pragma once

#include "mtl/sdk.h"

namespace mtl {

    class STA
    {
    public:

        STA()
        {
            hr = ::CoInitialize(0);
        }

        STA(const STA& rhs) = delete;

        STA(STA&& rhs)
        {
            hr = rhs.hr;
            rhs.hr = S_FALSE;
        }

        ~STA()
        {
            if (hr == S_OK)
            {
                ::CoUninitialize();
            }
        }

        STA& operator=(const STA& rhs) = delete;
        STA& operator=(STA&& rhs) = delete;

    private:
        HRESULT hr = S_FALSE;
    };

    class MTA
    {
    public:

        MTA()
        {
            hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
        }

        MTA(const MTA& rhs) = delete;

        MTA(MTA&& rhs)
        {
            hr = rhs.hr;
            rhs.hr = S_FALSE;
        }

        ~MTA()
        {
            if (hr == S_OK)
            {
                ::CoUninitialize();
            }
        }

        MTA& operator=(const MTA& rhs) = delete;
        MTA& operator=(MTA&& rhs) = delete;

    private:
        HRESULT hr = S_FALSE;
    };

    class OLE
    {
    public:

        OLE()
        {
            hr = ::OleInitialize(0);
        }

        OLE(const OLE& rhs) = delete;

        OLE(OLE&& rhs)
        {
            hr = rhs.hr;
            rhs.hr = S_FALSE;
        }

        ~OLE()
        {
            if (hr == S_OK)
            {
                ::OleUninitialize();
            }
        }

        OLE& operator=(const OLE& rhs) = delete;
        OLE& operator=(OLE&& rhs) = delete;

    private:
        HRESULT hr = S_FALSE;
    };

    template < class I >
    class punk;

    class HR
    {
    public:
        HR()
        {}

        HR(const HRESULT hr)
            : hr_(hr)
        {
            if (is_error())
            {
                throw hr_;
            }
        }

        HR& operator=(const HRESULT rhs)
        {
            hr_ = rhs;
            if (is_error())
            {
                throw hr_;
            }
            return *this;
        }

        operator bool() const
        {
            if (hr_ == S_OK)
                return true;
            return false;
        }

        bool is_error() const
        {
            if (hr_ != S_OK && hr_ != S_FALSE)
                return true;
            return false;
        }

        HRESULT result() const
        {
            return hr_;
        }

        HRESULT operator*() const
        {
            return hr_;
        }

        enum {
            WCODE_HRESULT_FIRST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x200),
            WCODE_HRESULT_LAST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF + 1, 0) - 1
        };

        static WORD HRESULT2WCode(HRESULT hr)
        {
            return (hr >= WCODE_HRESULT_FIRST && hr <= WCODE_HRESULT_LAST)
                ? WORD(hr - WCODE_HRESULT_FIRST)
                : 0;
        }

        static std::wstring msg(HRESULT hr)
        {
            wchar_t* buffer = nullptr;

            ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                hr,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&buffer,
                0,
                NULL);
                
            if (buffer != NULL)
            {
                size_t const nLen = wcslen(buffer);
                if (nLen > 1 && buffer[nLen - 1] == '\n')
                {
                    buffer[nLen - 1] = 0;
                    if (buffer[nLen - 2] == '\r')
                    {
                        buffer[nLen - 2] = 0;
                    }
                }
            }
            else 
            {
                long size = 32 * sizeof(wchar_t);
                buffer = (LPWSTR)LocalAlloc(0, size);
                if (msg != NULL) 
                {
                    WORD wCode = HRESULT2WCode(hr);
                    if (wCode != 0) 
                    {
                        _snwprintf_s(buffer,size, _TRUNCATE, L"IDispatch error #%d", (int)wCode);
                    }
                    else 
                    {
                        _snwprintf_s(buffer, size, _TRUNCATE, L"Unknown error 0x%0lX", hr);
                    }
                }
            }

            std::wstring result(buffer);
            ::LocalFree(buffer);
            return result;
        }

    private:
        HRESULT hr_ = S_OK;
    };

    namespace details {

        template<class I>
        class protect_refcnt : public I
        {
            friend class punk<I>;
        private:
            
            virtual ULONG __stdcall AddRef(void) override
            {
                return I::AddRef();
            }

            virtual ULONG __stdcall Release(void) override
            {
                return I::Release();
            }
        };
    }

    template<class T>
    struct from_object
    {
        int clsctx = CLSCTX_ALL;

        from_object(int ctx = CLSCTX_ALL)
            : clsctx(ctx)
        {}
    };

    struct from_progid
    {
        int clsctx = CLSCTX_ALL;
        std::string prog_id;

        from_progid(const std::string& progid, int ctx = CLSCTX_ALL)
            : clsctx(ctx), prog_id(progid)
        {}
    };

    struct from_clsid
    {
        int clsctx = CLSCTX_ALL;
        CLSID clsid;

        from_clsid(const CLSID& cls, int ctx = CLSCTX_ALL)
            : clsctx(ctx), clsid(cls)
        {}
    };

    //////////////////////////////////////////////////////////////////////
    // punk : COM smart pointer 
    //////////////////////////////////////////////////////////////////////

    template < class I >
    class punk
    {
    public:

        using interface_type = I;

        punk() : interface_(0) { }


        template<class T>
        punk(from_object<T>&& rhs)
            : interface_(0)
        {
            HR hr = create_object<T>(rhs.clsctx);
        }

        punk(from_progid&& rhs)
            : interface_(0)
        {
            HR hr = create_object(rhs.prog_id, rhs.clsctx);
        }

        punk(from_clsid&& rhs)
            : interface_(0)
        {
            HR hr = create_object(rhs.clsid, rhs.clsctx);
        }
         
        explicit punk(I* i)  // explicit for reason
            :interface_(0)
        {
            if (i)
            {
                interface_ = (I*)i;
                interface_->AddRef();
            }
        }

        punk(const punk<I>& rhs)
            :interface_(rhs.interface_)
        {
            if (rhs.interface_) {

                interface_->AddRef();
            }
        }

        punk(punk<I>&& rhs)
            :interface_(rhs.interface_)
        {
            rhs.interface_ = 0;
        }

        template<class T>
        explicit punk(T* rhs)
            : interface_(0)
        {
            if (rhs)
            {
                HRESULT hr = rhs->QueryInterface(__uuidof(I),(void**)&interface_);
                if (S_OK != hr)
                {
                    interface_ = 0;
                }
            }
        }

        template<class T>
         punk(const punk<T>& rhs)
             : interface_(0)
        {
            if (rhs.interface_)
            {
                HRESULT hr = rhs.query_interface(&interface_);
                if (S_OK != hr)
                {
                    interface_ = 0;
                }
            }
        }

        template<class T>
         punk(punk<T>&& rhs)
             : interface_(0)
        {
            if (rhs.interface_)
            {
                HRESULT hr = rhs.query_interface(&interface_);
                if (S_OK != hr)
                {
                    interface_ = 0;
                    return;
                }
                rhs.release();
            }
        }

        virtual ~punk()
        {
            release();
        }

        HRESULT query_interface(REFIID iid, void** iUnknown) const
        {
            return interface_->QueryInterface(iid, iUnknown);
        }

        template<class T>
        HRESULT query_interface(T** Unknown) const
        {
            return interface_->QueryInterface(__uuidof(T), (void**)Unknown);
        }

        template<class T>
        punk<T> as() const
        {
            punk<T> result;
            HR hr = interface_->QueryInterface(__uuidof(T), (void**)&result);
            return result;
        }

        HRESULT create_object(CLSID classId, int clsctx = CLSCTX_ALL)
        {
            return ::CoCreateInstance(classId, NULL, clsctx, __uuidof(I), (void**)&interface_);
        }


        HRESULT create_object(const std::wstring& progid, int clsctx = CLSCTX_ALL)
        {
            CLSID classId;
            HRESULT hr = ::CLSIDFromProgID(progid.c_str(), &classId);
            if (hr != S_OK)
                return hr;

            return create_object(classId, clsctx);
        }

        template<class T>
        HRESULT create_object( int clsctx = CLSCTX_ALL)
        {
            return ::CoCreateInstance( __uuidof(T), NULL, clsctx, __uuidof(I), (void**)&interface_);
        }

        const details::protect_refcnt<I>* operator->() const { return (const details::protect_refcnt<I>*) interface_; }
        details::protect_refcnt<I>* operator->()             { return (details::protect_refcnt<I>*) interface_; }
        I* operator*()                                 const { return interface_; }
        I** operator&()                                      { return &interface_; }
        operator bool() const                                { return (interface_ != 0); }
        bool operator!() const                               { return (interface_ == 0); }

        punk<I>* address_of()
        {
            return this;
        }

        punk& operator=(I* p)
        {
            if (p == interface_)
                return *this;

            if (p)
            {
                p->AddRef();
            }
            release();
            interface_ = p;
            return *this;
        }

        punk& operator=(const punk<I>& p)
        {
            if (p.interface_ == interface_)
                return *this;

            if (p)
            {
                p.interface_->AddRef();
            }
            release();
            interface_ = p.interface_;
            return *this;
        }

        punk& operator=(punk<I>&& p)
        {
            if (p.interface_ == interface_)
                return *this;

            I* tmp = p.interface_;
            release();
            interface_ = tmp;
            if (p)
            {
                p.interface_ = 0;
            }
            return *this;
        }

        template<class T>
        punk& operator=(const punk<T>& p)
        {
            I* tmp = nullptr;
            p.query_interface(__uuidof(I), (void**)&tmp);
            release();
            interface_ = tmp;
            return *this;
        }

        void release()
        {
            I* tmp = interface_;
            interface_ = 0;
            if (tmp)
            {
                tmp->Release();
            }
        }

        I* addref()
        {
            if (interface_)
            {
                interface_->AddRef();
            }
            return interface_;
        }

        void take_ownership(I* i)
        {
            release();
            interface_ = i;
        }

        I* interface_;

        template<class T>
        punk<I>& operator=( from_object<T>&& rhs)
        {
            release();
            HR hr = create_object<T>(rhs.clsctx);
            return *this;
        }

        punk<I>& operator=(from_progid&& rhs)
        {
            release();
            HR hr = create_object(rhs.prog_id, rhs.clsctx);
            return *this;
        }

        punk<I>& operator=(from_clsid&& rhs)
        {
            release();
            HR hr = create_object(rhs.clsid, rhs.clsctx);
            return *this;
        }

    };



}
