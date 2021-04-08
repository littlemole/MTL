#pragma once

namespace MTL {

    class STA
    {
    public:

        STA()
        {
            ::CoInitialize(0);
        }

        ~STA()
        {
            ::CoUninitialize();
        }
    };

    class MTA
    {
    public:

        MTA()
        {
            ::CoInitializeEx(0, COINIT_MULTITHREADED);
        }

        ~MTA()
        {
            ::CoUninitialize();
        }
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
            if (isError())
            {
                throw hr_;
            }
        }

        HR& operator=(const HRESULT rhs)
        {
            hr_ = rhs;
            if (isError())
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

        bool isError() const
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
    //////////////////////////////////////////////////////////////////////
    // punk : COM smart pointer 
    //////////////////////////////////////////////////////////////////////

    template < class I >
    class punk
    {
    public:
        punk() : interface_(0) { }


        explicit punk(I* i)
            :interface_(0)
        {
            if (i)
            {
                interface_ = (I*)i;
                interface_->AddRef();
            }
        }

        punk(const punk<I>& rhs)
        {
            interface_ = rhs.interface_;

            if (rhs.interface_) {

                interface_->AddRef();
            }
        }

        punk(punk<I>&& rhs)
        {
            interface_ = rhs.interface_;
            rhs.interface_ = 0;
        }

        template<class T>
        explicit punk(T* rhs)
        {
            interface_ = 0;
            if (rhs)
            {
                HRESULT hr = rhs->QueryInterface(__uuidof(I),(void**)&interface_);
                if (S_OK != hr)
                {
                    interface_ = 0;
                    throw hr;
                }
            }
        }

        template<class T>
        explicit punk(const punk<T>& rhs)
        {
            interface_ = 0;
            if (rhs.interface_)
            {
                HRESULT hr = rhs.queryInterface(&interface_);
                if (S_OK != hr)
                {
                    interface_ = 0;
                    throw hr;
                }
            }
        }

        template<class T>
        explicit punk(punk<T>&& rhs)
        {
            if (rhs.interface_)
            {
                interface_ = rhs.interface_;
                HRESULT hr = rhs.queryInterface(&interface_);
                if (S_OK != hr)
                {
                    interface_ = 0;
                    throw hr;
                }
                rhs.interface_->Release;
                rhs.interface_ = 0;
            }
            rhs.interface_ = 0;
        }

        virtual ~punk()
        {
            release();
        }

        HRESULT queryInterface(REFIID iid, void** iUnknown) const
        {
            return interface_->QueryInterface(iid, iUnknown);
        }

        template<class T>
        HRESULT queryInterface(T** Unknown) const
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

        HRESULT createObject(CLSID classId, int clsctx = CLSCTX_ALL)
        {
            return ::CoCreateInstance(classId, NULL, clsctx, __uuidof(I), (void**)&interface_);
        }


        HRESULT createObject(const std::wstring& progid, int clsctx = CLSCTX_ALL)
        {
            CLSID classId;
            HRESULT hr = ::CLSIDFromProgID(progid.c_str(), &classId);
            if (hr != S_OK)
                return hr;

            return createObject(classId, clsctx);
        }

        template<class T>
        HRESULT createObject( int clsctx = CLSCTX_ALL)
        {
            return ::CoCreateInstance( __uuidof(T), NULL, clsctx, __uuidof(I), (void**)&interface_);
        }

        const details::protect_refcnt<I>* operator->() const { return (const details::protect_refcnt<I>*) interface_; }
        details::protect_refcnt<I>* operator->()             { return (details::protect_refcnt<I>*) interface_; }
        I* operator*()                                       { return interface_; }
        I** operator&()                                      { return &interface_; }
        operator bool() const                                { return (interface_ != 0); }
        bool operator!() const                               { return (interface_ == 0); }

        punk& operator=(I* p)
        {
            if (p == interface_)
                return *this;

            release();
            interface_ = p;
            if (p)
            {
                interface_->AddRef();
            }
            return *this;
        }

        punk& operator=(const punk<I>& p)
        {
            if (p == *this)
                return *this;

            release();
            interface_ = p.interface_;
            if (p)
            {
                interface_->AddRef();
            }
            return *this;
        }

        punk& operator=(punk<I>&& p)
        {
            if (p == *this)
                return *this;

            release();
            interface_ = p.interface_;
            if (p)
            {
                p.interface_ = 0;
            }
            return *this;
        }

        template<class T>
        punk& operator=(const punk<T>& p)
        {
            release();
            p.queryInterface(__uuidof(I), (void**)&interface_);
            return *this;
        }

        void release()
        {
            if (interface_)
            {
                interface_->Release();
            }
            interface_ = 0;
        }

        I* addref()
        {
            if (interface_)
            {
                interface_->AddRef();
            }
            return interface_;
        }

        template<class I>
        void take_ownership(I* i)
        {
            if (interface_)
                interface_->Release();

            interface_ = i;
        }

        I* interface_;
    };

}
