#pragma once

#include "MTL/impl.h"
#include "ocidl.h"
#include <map>
#include <string>

namespace MTL {


	inline std::wstring guid_to_string(const GUID& guid)
	{
		wchar_t guidStr[39];
		swprintf_s(
			guidStr,
			L"{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

		std::wstring s(guidStr);
		return s;
	}

	inline GUID string_to_guid(const std::wstring& str)
	{
		GUID guid;
		wscanf_s(str.c_str(),
			L"{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
			&guid.Data1, &guid.Data2, &guid.Data3,
			&guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3],
			&guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7] );

		return guid;
	}

	inline BOOL Is64BitWindows()
	{
#if defined(_WIN64)
		return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
		// 32-bit programs run on both 32-bit and 64-bit Windows
		// so must sniff
		BOOL f64 = FALSE;
		return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
		return FALSE; // Win64 does not support Win16
#endif
	}

	template<class T>
	class Dispatch;

	template<class T, class I>
	class Dispatch<T(I)> : public I
	{
	public:
		void load_typelib()
		{
			std::wstring selfPath = pathToSelf();
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

		~Dispatch()
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

		virtual HRESULT __stdcall GetClassInfo(ITypeInfo** ppTI)
		{
			if (typeLib_)
			{
				return typeLib_->GetTypeInfoOfGuid(__uuidof(T), ppTI);
			}
			return E_FAIL;
		}

	protected:
		std::atomic<long> refCnt_;

		ITypeLib* typeLib_ = nullptr;
		ITypeInfo* typeInfo_ = nullptr;

	};

	namespace details
	{
		template<class T, class I, class ... Args>
		class interfaces<T(Dispatch<T(I)>, Args...)>
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
	}

	//////////////////////////////////////////////////////////////////////////////
	// specialization with support for IDispatch
	//
	// depends on the typelib to be embedded as resource in the COM server 
	// .rc file
	//////////////////////////////////////////////////////////////////////////////

	/*
	template<class T, class I, class ... Args>
	class implements<dispatch_object<T>(I, Args...)> : public implements<T(of<I,IDispatch>,I, IProvideClassInfo,Args...)> // public details::derives<void(I, Args...)>
	{
	public:

		using classType = T;

		implements()
		{
			std::wstring selfPath = pathToSelf();
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

		implements(const CLSID& libid, int major = 1, int minor = 0)
		{
			HRESULT hr = ::LoadRegTypeLib(libid,major,minor,LOCALE_SYSTEM_DEFAULT,&typeLib_);
			if (hr == S_OK)
			{
				hr = typeLib_->GetTypeInfoOfGuid(__uuidof(I), &typeInfo_);
			}
			if (hr != S_OK)
			{
				exit(1);
			}						
		}

		~implements()
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

		virtual HRESULT __stdcall GetClassInfo(ITypeInfo** ppTI)
		{
			if (typeLib_)
			{
				return typeLib_->GetTypeInfoOfGuid(__uuidof(T), ppTI);
			}
			return E_FAIL;
		}

	protected:
		std::atomic<long> refCnt_;

		ITypeLib* typeLib_ = nullptr;
		ITypeInfo* typeInfo_ = nullptr;
	};

	*/
	//////////////////////////////////////////////////////////////////////////////
	// syntactic sugar to avoid typing
	//////////////////////////////////////////////////////////////////////////////

	template<class T>
	class dispatch;

	template<class T, class I, class ... Args>
	class dispatch < T(I, Args...)> : public implements<T(Dispatch<T(I)>, Args...)>
	{
	public:
		dispatch() {}
		/*
		dispatch(const CLSID& libid, int major = 1, int minor = 0)
			: implements<T(Dispatch<T(I)>, Args...)>(libid, major, minor)
		{}
		*/
	};

	template<class T>
	class stack_disp;

	template<class T, class I, class ... Args>
	class stack_disp<T(I, Args...)> : public implements<stack_object<T>(Dispatch<T(I)>, Args...)>
	{
	public:
		stack_disp() {}
		/*
		stack_disp(const CLSID& libid, int major = 1, int minor = 0)
			: implements<dispatch_object<stack_object<T>>(I, Args...)>(libid, major, minor)
		{}
		*/
	};

	/*
	template<class T, class I, class ... Args>
	class dispatch < T(I, Args...)> : public implements<dispatch_object<T>(I, Args...)>
	{
	public:
		dispatch() {}
		dispatch(const CLSID& libid, int major = 1, int minor = 0)
			: implements<dispatch_object<T>(I, Args...)>(libid,major,minor)
		{}
	};

	template<class T>
	class stack_disp;

	template<class T, class I, class ... Args>
	class stack_disp<T(I,Args...)> : public implements<dispatch_object<stack_object<T>>(I, Args...)>
	{
	public:
		stack_disp() {}
		stack_disp(const CLSID& libid, int major = 1, int minor = 0)
			: implements<dispatch_object<stack_object<T>>(I, Args...)>(libid,major,minor)
		{}
	};
	*/


} // end namespace MTL

