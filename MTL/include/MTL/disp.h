#pragma once

#include "MTL/impl.h"

namespace MTL {


	//////////////////////////////////////////////////////////////////////////////
	// specialization with support for IDispatch
	//
	// depends on the typelib to be embedded as resource in the COM server 
	// .rc file
	//////////////////////////////////////////////////////////////////////////////

	template<class T, class I, class ... Args>
	class implements<dispatch_object<T>(I, Args...)> : public implements<T(of<I,IDispatch>,I,Args...)> // public details::derives<void(I, Args...)>
	{
	public:

		using classType = T;

		implements()
		{
			std::wstring selfPath = pathToSelf();
			HRESULT hr = ::LoadTypeLib(selfPath.c_str(), &typeLib_);
			if (hr != S_OK)
			{
				exit(1);
			}

			hr = typeLib_->GetTypeInfoOfGuid(__uuidof(I), &typeInfo_);
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

	protected:
		std::atomic<long> refCnt_;

		ITypeLib* typeLib_ = nullptr;
		ITypeInfo* typeInfo_ = nullptr;
	};


	//////////////////////////////////////////////////////////////////////////////
	// syntactic sugar to avoid typing
	//////////////////////////////////////////////////////////////////////////////

	template<class T>
	class dispatch;

	template<class T, class I, class ... Args>
	class dispatch < T(I, Args...)> : public implements<dispatch_object<T>(I, Args...)>
	{};

	template<class T>
	class stack_disp;

	template<class T, class I, class ... Args>
	class stack_disp<T(I,Args...)> : public implements<dispatch_object<stack_object<T>>(I, Args...)>
	{};



} // end namespace MTL

