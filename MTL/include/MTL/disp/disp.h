#pragma once

#include "mtl/obj/impl.h"
#include "mtl/punk.h"
#include "MTL/win32/uni.h"
#include "ocidl.h"
#include <map>
#include <iostream>

namespace mtl {

	template<class T, class I, REFGUID LIBID= GUID_NULL, int MAJOR = 1, int MINOR = 0>
	class dispatch : public I
	{
	public:

		dispatch()
		{
			load_typelib();
		}

		void load_typelib()
		{
			if (!::IsEqualGUID(LIBID, GUID_NULL))
			{
				load_typelib(LIBID, MAJOR, MINOR);
				return;
			}

			std::wstring selfPath = path_to_self();
			HRESULT hr = ::LoadTypeLibEx(selfPath.c_str(), REGKIND_NONE, &typeLib_);
			if (hr == S_OK)
			{
				hr = typeLib_->GetTypeInfoOfGuid(__uuidof(I), &typeInfo_);
			}
			if (hr != S_OK)
			{
				exit(1);
			}
		}

		void load_typelib(const CLSID& libid, int major = 1, int minor = 0)
		{
			HRESULT hr = ::LoadRegTypeLib(libid, major, minor, LOCALE_SYSTEM_DEFAULT, &typeLib_);
			if (hr == S_OK)
			{
				hr = typeLib_->GetTypeInfoOfGuid(__uuidof(I), &typeInfo_);
			}
			if (hr != S_OK)
			{
				exit(1);
			}
		}

		~dispatch()
		{
			if (typeInfo_)
			{
				typeInfo_->Release();
			}
			if (typeLib_)
			{
				typeLib_->Release();
			}
		}

		virtual HRESULT __stdcall GetTypeInfoCount(UINT* pctinfo) override
		{
			if (!pctinfo)
				return E_INVALIDARG;
			*pctinfo = 1;
			return S_OK;
		}
		virtual HRESULT __stdcall GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override
		{
			if (!ppTInfo)
				return E_INVALIDARG;

			*ppTInfo = 0;

			return typeInfo_->QueryInterface(IID_ITypeInfo, (void**)ppTInfo);
		}
		virtual HRESULT __stdcall GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override
		{
			return ::DispGetIDsOfNames(typeInfo_, rgszNames, cNames, rgDispId);
		}
		virtual HRESULT __stdcall Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override
		{
			HRESULT hr = ::DispInvoke(this, typeInfo_, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
			return hr;
		}

		std::atomic<long> refCnt_;

		ITypeLib* typeLib_ = nullptr;
		ITypeInfo* typeInfo_ = nullptr;

};

template<class T>
class support_error_info;

template<class ... Args>
class support_error_info<void(Args...)> : public ISupportErrorInfo
{
public:

	virtual HRESULT __stdcall InterfaceSupportsErrorInfo(REFIID riid) override
	{
		return supports_error_info<Args...>(riid);
	}

private:

	template<class T, class ... VArgs>
	HRESULT supports_error_info(REFIID riid)
	{
		if (::IsEqualIID(riid, __uuidof(T)))
		{
			return S_OK;
		}
		if constexpr (sizeof...(VArgs) != 0)
		{
			return supports_error_info<VArgs...>(riid);
		}
		return S_FALSE;
	}
};

template<class T>
class provide_class_info : public IProvideClassInfo2
{
public:

	virtual HRESULT __stdcall GetClassInfo(ITypeInfo** ppTI) override
	{
		if (!ppTI)
		{
			return E_INVALIDARG;
		}
		*ppTI = 0;

		T* that = (T*)this;

		HRESULT hr = that->typeLib_->GetTypeInfoOfGuid(__uuidof(T), ppTI);
		return hr;
	}

	virtual HRESULT __stdcall GetGUID(DWORD dwGuidKind, GUID* pGUID) override
	{
		if (!pGUID)
			return E_INVALIDARG;

		T* that = (T*)this;

		punk<ITypeInfo> ti;
		HRESULT hr = GetClassInfo(&ti);
		if (hr != S_OK)
		{
			return hr;
		}

		TYPEATTR* ta = 0;
		ti->GetTypeAttr(&ta);
		int c = ta->cImplTypes;
		for (int i = 0; i < c; i++)
		{
			HREFTYPE hRefType = NULL;
			ti->GetRefTypeOfImplType(i, &hRefType);

			punk<ITypeInfo> sti;
			ti->GetRefTypeInfo(hRefType, &sti);

			int iImplTypeFlags = 0;
			
			ti->GetImplTypeFlags(i, &iImplTypeFlags);
			if (iImplTypeFlags & IMPLTYPEFLAG_FDEFAULT)
			{
				if (iImplTypeFlags & IMPLTYPEFLAG_FSOURCE)
				{
					TYPEATTR* sta = 0;
					sti->GetTypeAttr(&sta);
					*pGUID = sta->guid;
					sti->ReleaseTypeAttr(sta);
					ti->ReleaseTypeAttr(ta);
					return S_OK;
				}
			}

		}
		ti->ReleaseTypeAttr(ta);
		
		return E_UNEXPECTED;

	}
};

	class class_info {};

	template<class T>
	class error_info {};

	namespace details
	{
		template<class T, class I, class ... Args>
		class interfaces<T(dispatch<T,I>, Args...)>
		{
		public:

			static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
			{
				if (::IsEqualGUID(riid, IID_IDispatch))
				{
					*ppvObject = (IDispatch*)(I*)(that);
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

		template<class T, class I, REFGUID LIBID, int MAJOR, int MINOR, class ... Args>
		class interfaces<T(dispatch<T, I, LIBID, MAJOR, MINOR>, Args...)>
		{
		public:

			static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
			{
				if (::IsEqualGUID(riid, IID_IDispatch))
				{
					*ppvObject = (IDispatch*)(I*)(that);
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


		template< class T, class ... Args>
		class derives<T(class_info, Args...)> : public provide_class_info<T>, public derives<T(Args...)>
		{};

		template<class T, class ... Args>
		class interfaces<T(class_info, Args...)> : public interfaces<T(provide_class_info<T>, Args...)>
		{};

		template<class T,  class ... Args>
		class interfaces<T(provide_class_info<T>, Args...)>
		{
		public:

			static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
			{

				if (::IsEqualGUID(riid, IID_IProvideClassInfo2))
				{
					*ppvObject = (IProvideClassInfo2*)(that);
					that->AddRef();
					return S_OK;
				}


				if (::IsEqualGUID(riid, IID_IProvideClassInfo))
				{
					*ppvObject = (IProvideClassInfo*)(IProvideClassInfo2*)(that);
					that->AddRef();
					return S_OK;
				}
				return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
			}
		};

		template< class T, class I, class ... Args>
		class derives<T(error_info<I>, Args...)> : public support_error_info<I>, public derives<T(Args...)>
		{};

		template<class T, class I, class ... Args>
		class interfaces<T(error_info<I>, Args...)> : public interfaces<T(support_error_info<I>, Args...)>
		{};

		template<class T, class I, class ... Args>
		class interfaces<T(support_error_info<I>, Args...)>
		{
		public:

			static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
			{

				if (::IsEqualGUID(riid, IID_ISupportErrorInfo))
				{
					*ppvObject = (ISupportErrorInfo*)(that);
					that->AddRef();
					return S_OK;
				}

				return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
			}
		};


	}

	template<class T, REFGUID LIBID, int MAJOR , int MINOR >
	class dual
	{
	public:
		using type = T;
	};

	namespace details
	{

		template< class T, class I, class ... Args>
		class derives<T(dual<I>, Args...)> : public dispatch<T,I>, public derives<T(Args...)>
		{};

		template< class T, class I, REFGUID LIBID, int MAJOR, int MINOR, class ... Args>
		class derives<T(dual<I,LIBID,MAJOR,MINOR>, Args...)> : public dispatch<T, I, LIBID, MAJOR, MINOR>, public derives<T(Args...)>
		{};

		template<class T, class I, class ... Args>
		class interfaces<T(dual<I>, Args...)> : public interfaces<T(dispatch<T,I>, Args...)>
		{};

		template<class T, class I, REFGUID LIBID, int MAJOR, int MINOR, class ... Args>
		class interfaces<T(dual<I,LIBID,MAJOR,MINOR>, Args...)> : public interfaces<T(dispatch<T, I, LIBID, MAJOR, MINOR>, Args...)>
		{};

	}

} // end namespace MTL

