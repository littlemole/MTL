#pragma once

#include "gtest/gtest.h"
#include "MTL/win32/uni.h"
#include "MTL/disp/bstr.h"
#include "MTL/disp/disp.h"
#include "MTL/disp/variant.h"
#include "MTL/disp/sf_array.h"
#include "MTL/obj/enum.h"
#include "MTL/punk.h"
#include "MTL/win32/box.h"
#include "MTL/util/path.h"
#include <sstream>
#include <deque>
#include <mutex>
#include <thread>
#include <ShlObj.h>

namespace MTL {


    template<class C, class T, class I, REFGUID LIBID = GUID_NULL, int MAJOR = 1, int MINOR = 0>
    class Collection : public Dispatch<T,I,LIBID,MAJOR,MINOR>
    {
    public:

        virtual  HRESULT __stdcall get_Count(long* cnt)
        {
            if (!cnt)
                return E_INVALIDARG;

            *cnt = (long)data_.size();
            return S_OK;
        }

        virtual  HRESULT __stdcall get_Length(long* cnt)
        {
            return get_Count(cnt);
        }

        virtual  HRESULT __stdcall get_Size(long* cnt)
        {
            return get_Count(cnt);
        }

        virtual  HRESULT __stdcall Empty(VARIANT_BOOL* vb)
        {
            if (!vb)
                return E_INVALIDARG;

            *vb = data_.empty() ? VARIANT_TRUE : VARIANT_FALSE;
            return S_OK;
        }

        virtual HRESULT __stdcall Clear()
        {
            data_.clear();
            return S_OK;
        }

        virtual HRESULT __stdcall _Enum(IEnumVARIANT** enu)
        {
            if (!enu)
                return E_INVALIDARG;

            *enu = new EnumVariant(data_);
            (*enu)->AddRef();
            return S_OK;
        }

    protected:

        std::vector<C> data_;
    };

    template<class T, class I, REFGUID LIBID = GUID_NULL, int MAJOR = 1, int MINOR = 0>
    class LongCollection : public Collection<long,T,I,LIBID,MAJOR,MINOR>
    {
    public:

        virtual HRESULT __stdcall Value(long index, long* value)
        {
            if (!value)
                return E_INVALIDARG;

            if (index >= this->data_.size())
                return E_INVALIDARG;

            *value = this->data_[index];
            return S_OK;
        }

        virtual HRESULT __stdcall Item(long index, long* value)
        {
            return Value(index, value);
        }

        virtual HRESULT __stdcall Put(long index, long value)
        {
            if (index >= this->data_.size())
                return E_INVALIDARG;

            this->data_[index] = value;
            return S_OK;
        }

        virtual HRESULT __stdcall Add(long value)
        {
            this->data_.push_back(value);
            return S_OK;
        }
    };

    namespace details
    {
        template<class T, class I, class ... Args>
        class interfaces<T(LongCollection<T, I>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                if (::IsEqualGUID(riid, __uuidof(I)))
                {
                    *ppvObject = (I*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, IID_IDispatch))
                {
                    *ppvObject = (IDispatch*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, __uuidof(I)))
                {
                    *ppvObject = (I*)(that);
                    that->AddRef();
                    return S_OK;
                }

                return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
            }
        };

        template<class T, class I, REFGUID LIBID, int MAJOR, int MINOR,class ... Args>
        class interfaces<T(LongCollection<T, I, LIBID, MAJOR,MINOR>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                return interfaces<T(LongCollection<T, I>)>::QueryInterface(that, riid, ppvObject);
            }
        };

    }


    template<class T, class I, REFGUID LIBID = GUID_NULL, int MAJOR = 1, int MINOR = 0>
    class BstrCollection: public Collection<bstr,T,I,LIBID,MAJOR,MINOR>
    {
    public:

        virtual HRESULT __stdcall Value(long index, BSTR* value)
        {
            if (!value)
                return E_INVALIDARG;

            if (index >= this->data_.size())
                return E_INVALIDARG;

            return this->data_[index].copy_to(value);
        }

        virtual HRESULT __stdcall Item(long index, BSTR* value)
        {
            return Value(index, value);
        }

        virtual HRESULT __stdcall Put(long index, BSTR value)
        {
            if (index >= this->data_.size())
                return E_INVALIDARG;

            this->data_[index] = bstr(b_copy(value));
            return S_OK;
        }

        virtual HRESULT __stdcall Add(BSTR value)
        {
            this->data_.push_back(bstr(b_copy(value)));
            return S_OK;
        }
    };

    namespace details
    {
        template<class T, class I, class ... Args>
        class interfaces<T(BstrCollection<T, I>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                if (::IsEqualGUID(riid, __uuidof(I)))
                {
                    *ppvObject = (I*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, IID_IDispatch))
                {
                    *ppvObject = (IDispatch*)(that);
                    that->AddRef();
                    return S_OK;
                }

                return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
            }
        };

        template<class T, class I, REFGUID LIBID, int MAJOR, int MINOR, class ... Args>
        class interfaces<T(BstrCollection<T, I, LIBID, MAJOR, MINOR>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                return interfaces<T(BstrCollection<T, I>)>::QueryInterface(that, riid, ppvObject);
            }
        };

    }

    template<class T, class I, REFGUID LIBID = GUID_NULL, int MAJOR = 1, int MINOR = 0>
    class VariantCollection : public Collection<variant,T,I,LIBID,MAJOR,MINOR>
    {
    public:

        virtual HRESULT __stdcall Value(long index, VARIANT* value)
        {
            if (!value)
                return E_INVALIDARG;

            if (index >= this->data_.size())
                return E_INVALIDARG;

            return this->data_[index].copy_to(value);
        }

        virtual HRESULT __stdcall Item(long index, VARIANT* value)
        {
            return Value(index, value);
        }

        virtual HRESULT __stdcall Put(long index, VARIANT value)
        {
            if (index >= this->data_.size())
                return E_INVALIDARG;

            this->data_[index] = variant(value);
            return S_OK;
        }

        virtual HRESULT __stdcall Add(VARIANT value)
        {
            this->data_.push_back(variant(value));
            return S_OK;
        }
    };

    namespace details
    {
        template<class T, class I, class ... Args>
        class interfaces<T(VariantCollection<T, I>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                if (::IsEqualGUID(riid, __uuidof(I)))
                {
                    *ppvObject = (I*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, IID_IDispatch))
                {
                    *ppvObject = (IDispatch*)(that);
                    that->AddRef();
                    return S_OK;
                }

                return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
            }
        };

        template<class T, class I, REFGUID LIBID, int MAJOR, int MINOR, class ... Args>
        class interfaces<T(VariantCollection<T, I, LIBID, MAJOR, MINOR>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                return interfaces<T(VariantCollection<T, I>)>::QueryInterface(that, riid, ppvObject);
            }
        };

    }

    template<class T, class I, REFGUID LIBID = GUID_NULL, int MAJOR = 1, int MINOR = 0>
    class DispCollection : public Collection<punk<IDispatch>,T,I,LIBID,MAJOR,MINOR >
    {
    public:

        virtual HRESULT __stdcall Value(long index, IDispatch** value)
        {
            if (!value)
                return E_INVALIDARG;

            if (index >= this->data_.size())
                return E_INVALIDARG;

            return this->data_[index].queryInterface(value);
        }

        virtual HRESULT __stdcall Item(long index, IDispatch** value)
        {
            return Value(index, value);
        }

        virtual HRESULT __stdcall Put(long index, IDispatch* value)
        {
            if (index >= this->data_.size())
                return E_INVALIDARG;

            this->data_[index] = punk<IDispatch>(value);
            return S_OK;
        }

        virtual HRESULT __stdcall Add(IDispatch* value)
        {
            this->data_.push_back(punk<IDispatch>(value));
            return S_OK;
        }
    };

    namespace details
    {
        template<class T, class I, class ... Args>
        class interfaces<T(DispCollection<T, I>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                if (::IsEqualGUID(riid, __uuidof(I)))
                {
                    *ppvObject = (I*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, IID_IDispatch))
                {
                    *ppvObject = (IDispatch*)(that);
                    that->AddRef();
                    return S_OK;
                }

                return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
            }
        };

        template<class T, class I, REFGUID LIBID, int MAJOR, int MINOR, class ... Args>
        class interfaces<T(DispCollection<T, I, LIBID, MAJOR, MINOR>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                return interfaces<T(DispCollection<T, I>)>::QueryInterface(that, riid, ppvObject);
            }
        };

    }

    template<class T, class I, REFGUID LIBID = GUID_NULL, int MAJOR = 1, int MINOR = 0>
    class UnknownCollection : public Collection<punk<IUnknown>, T, I, LIBID, MAJOR, MINOR >
    {
    public:

        virtual HRESULT __stdcall Value(long index, IUnknown** value)
        {
            if (!value)
                return E_INVALIDARG;

            if (index >= this->data_.size())
                return E_INVALIDARG;

            return this->data_[index].queryInterface(value);
        }

        virtual HRESULT __stdcall Item(long index, IUnknown** value)
        {
            return Value(index, value);
        }

        virtual HRESULT __stdcall Put(long index, IUnknown* value)
        {
            if (index >= this->data_.size())
                return E_INVALIDARG;

            this->data_[index] = punk<IUnknown>(value);
            return S_OK;
        }

        virtual HRESULT __stdcall Add(IUnknown* value)
        {
            this->data_.push_back(punk<IUnknown>(value));
            return S_OK;
        }
    };

    namespace details
    {
        template<class T, class I, class ... Args>
        class interfaces<T(UnknownCollection<T, I>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                if (::IsEqualGUID(riid, __uuidof(I)))
                {
                    *ppvObject = (I*)(that);
                    that->AddRef();
                    return S_OK;
                }
/*
                if (::IsEqualGUID(riid, IID_IDispatch))
                {
                    *ppvObject = (IDispatch*)(that);
                    that->AddRef();
                    return S_OK;
                }
*/
                return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
            }
        };

        template<class T, class I, REFGUID LIBID, int MAJOR, int MINOR, class ... Args>
        class interfaces<T(UnknownCollection<T, I, LIBID, MAJOR, MINOR>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                return interfaces<T(UnknownCollection<T, I>)>::QueryInterface(that, riid, ppvObject);
            }
        };

    }


}