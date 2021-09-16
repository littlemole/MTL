#pragma once

#include "MTL/obj/impl.h"

namespace MTL {


	//////////////////////////////////////////////////////////////////////////////
	// primary class factory impl
	//////////////////////////////////////////////////////////////////////////////

	template<class T>
	class ClassObject : public implements<ClassObject<T>(IClassFactory)>
	{
	public:

		virtual ULONG __stdcall AddRef(void) override
		{
			comModule().lock();
			return 2;
		}

		virtual ULONG __stdcall Release(void) override
		{
			comModule().unlock();
			return 1;
		}

		virtual HRESULT __stdcall CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override
		{
			if (!ppvObject)
				return E_INVALIDARG;

			*ppvObject = 0;

			T* t = new T();
			HRESULT hr = t->QueryInterface(riid, ppvObject);
			if (hr == S_OK)
				return hr;

			delete t;
			return E_NOINTERFACE;
		}

		virtual HRESULT __stdcall LockServer(BOOL fLock) override
		{
			if (fLock)
			{
				comModule().lock();
			}
			else
			{
				comModule().unlock();
			}
			return S_OK;
		}

	};

	//////////////////////////////////////////////////////////////////////////////
	// class factory with support for COM aggregation
	//////////////////////////////////////////////////////////////////////////////

	template<class T>
	class ClassObject<aggregatable<T>> : public ClassObject<T> // public implements<ClassObject<aggregatable<T>>(IClassFactory)>
	{
	public:
		/*

		virtual ULONG __stdcall AddRef(void) override
		{
			comModule().lock();
			return 2;
		}

		virtual ULONG __stdcall Release(void) override
		{
			comModule().unlock();
			return 1;
		}

		*/

		virtual HRESULT __stdcall CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override
		{
			if (!ppvObject)
				return E_INVALIDARG;

			*ppvObject = 0;

			if (pUnkOuter)
			{
				if (!::IsEqualGUID(IID_IUnknown, riid))
				{
					return E_INVALIDARG;
				}

				T* t = new T();
				t->pOuterUnknown = pUnkOuter;

				HRESULT hr = t->innerUnknown.QueryInterface(riid, ppvObject);
				if (hr == S_OK)
					return hr;

				delete t;
				return E_NOINTERFACE;
			}

			T* t = new T();
			HRESULT hr = t->QueryInterface(riid, ppvObject);
			if (hr == S_OK)
				return hr;

			delete t;
			return E_NOINTERFACE;
		}

		/*
		virtual HRESULT __stdcall LockServer(BOOL fLock) override
		{
			if (fLock)
			{
				comModule().lock();
			}
			else
			{
				comModule().unlock();
			}
			return S_OK;
		}
		*/
	};

	template<class T>
	class ClassObject<localserver<T>> : public ClassObject<T>
	{
	public:

		virtual ULONG __stdcall AddRef(void) override
		{
			return 2;
		}

		virtual ULONG __stdcall Release(void) override
		{
			return 1;
		}
	};

} // end namespace MTL

