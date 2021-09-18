#pragma once

#include "mtl/win32/module.h"

namespace mtl {

	//////////////////////////////////////////////////////////////////////////////
	// marker templates
	//////////////////////////////////////////////////////////////////////////////

	template<class T, class I>
	class of
	{};

	template<class T>
	class dispatch_object
	{};

	template<class T>
	class stack_object
	{};

	template<class T>
	class aggregatable
	{};

	template<class T>
	class localserver
	{};

	template<class T, REFGUID LIBID = GUID_NULL, int MAJOR = 1, int MINOR = 0>
	class dual;

	namespace details {

		//////////////////////////////////////////////////////////////////////////////
		// derivation helper
		//
		// deriving from derives<Args..:> will derive from all Args... except
		// for those Args... wrapped in of<T>
		//
		// this helps to filter out Interfaces that should be exposed via
		// QueryInterface but should not derved from, as in
		// - Args contains both IDispatch and a Interface already erivinf from
		//   IDispatch
		// - Persist, PersistInit, PersistFile etc
		//////////////////////////////////////////////////////////////////////////////

		template< class T>
		class derives;

		template<class T>
		class derives<T()>
		{};


		template< class T, class I, class ... Args>
		class derives<T(I, Args...)> : public I, public derives<T(Args...)>
		{};

		template< class C, class T, class I, class ... Args>
		class derives<C(of<T, I>, Args...)> : public derives<C(Args...)>
		{};


		//////////////////////////////////////////////////////////////////////////////
		// template to build a list of Interfaces that are exposed via QueryInterface
		//////////////////////////////////////////////////////////////////////////////


		template<class T>
		class interfaces;

		template<class T>
		class interfaces<T()>
		{
		public:

			static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
			{
				return E_NOINTERFACE;
			}
		};

		template<class T, class I, class ... Args>
		class interfaces<T(I, Args...)>
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

				return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
			}
		};

		template<class T, class O, class I, class ... Args>
		class interfaces<T(of<O, I>, Args...)>
		{
		public:

			static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
			{
				if (::IsEqualGUID(riid, __uuidof(I)))
				{
					*ppvObject = (I*)(O*)(that);
					that->AddRef();
					return S_OK;
				}

				return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
			}
		};

	} // end namespace details


	//////////////////////////////////////////////////////////////////////////////
	// implements is the primary base class for COM classes
	// it will derive from all interfaces specified ( I, Args ...) except for those
	// wrapped in of<T>, and exposes all of them via QI
	//////////////////////////////////////////////////////////////////////////////


	template<class T>
	class implements;

	template<class T, class I, class ... Args>
	class implements<T(I, Args...)> : public details::derives<T(I, Args...)>
	{
	public:

		using classType = T;

		virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject)
		{
			T* that = (T*)this;

			if (!ppvObject)
				return E_INVALIDARG;

			*ppvObject = 0;

			if (::IsEqualGUID(riid, IID_IUnknown))
			{
				*ppvObject = (IUnknown*)(I*)(that);
				that->AddRef();
				return S_OK;
			}			
			return details::interfaces<T(I, Args...)>::QueryInterface(that, riid, ppvObject);
		}

		virtual ULONG __stdcall AddRef(void) override
		{
			this->refCnt_++;
			the_com_module().lock();

			return refCnt_;
		}

		virtual ULONG __stdcall Release(void) override
		{
			long cnt = --(this->refCnt_);

			if (cnt == 0)
			{
				T* that = (T*)this;
				delete that;
			}
			the_com_module().unlock();
			return cnt;
		}

	protected:
		std::atomic<long> refCnt_;
	};

	//////////////////////////////////////////////////////////////////////////////
	// specialization with support for COM aggregation (aggregatable)
	//////////////////////////////////////////////////////////////////////////////

	template<class T, class I, class ... Args>
	class implements<aggregatable<T>(I, Args...)> :public details::derives<T(I, Args...)>
	{
		friend class class_object<aggregatable<T>>;

	public:

		using classType = T;

		class InnerUnknown : public IUnknown
		{
		public:

			InnerUnknown(implements<aggregatable<T>(I, Args...)>* outerThis)
				:This(outerThis)
			{}

			virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override
			{
				T* that = (T*)This;

				if (!ppvObject)
					return E_INVALIDARG;

				*ppvObject = 0;

				if (::IsEqualGUID(riid, IID_IUnknown))
				{
					*ppvObject = (IUnknown*)(this);
					this->AddRef();
					return S_OK;
				}

				return details::interfaces<T(I, Args...)>::QueryInterface(that, riid, ppvObject);
			}

			virtual ULONG __stdcall AddRef(void) override
			{
				this->refCnt_++;
				the_com_module().lock();

				return refCnt_;
			}

			virtual ULONG __stdcall Release(void) override
			{
				long cnt = --(this->refCnt_);

				if (cnt == 0)
				{
					T* that = (T*)This;
					delete that;
				}
				the_com_module().unlock();
				return cnt;
			}

		private:
			implements<aggregatable<T>(I, Args...)>* This;
			std::atomic<long> refCnt_;
		};

		implements()
			:innerUnknown(this), pOuterUnknown(&innerUnknown)
		{
		}

		virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject)
		{
			return pOuterUnknown->QueryInterface(riid, ppvObject);
		}

		virtual ULONG __stdcall AddRef(void) override
		{
			return pOuterUnknown->AddRef();
		}

		virtual ULONG __stdcall Release(void) override
		{
			return pOuterUnknown->Release();
		}

	protected:
		InnerUnknown innerUnknown;
		IUnknown* pOuterUnknown = nullptr;
	};


	//////////////////////////////////////////////////////////////////////////////
	// specialization decorator for an aggregating object
	//
	// AGG: aggregation (outer) object
	// IMPL: implements<Args...>
	//////////////////////////////////////////////////////////////////////////////

	template<class AGG, class IMPL>
	class aggregate<AGG(IMPL)> : public IMPL
	{
	public:

		using classType = typename IMPL::classType;

		aggregate()
		{
			HRESULT hr = ::CoCreateInstance(__uuidof(AGG), (IUnknown*)this, CLSCTX_ALL, IID_IUnknown, (void**)&inner_);
			if (hr != S_OK)
				inner_ = nullptr;
		}

		~aggregate()
		{
			if (inner_)
			{
				inner_->Release();
				inner_ = nullptr;
			}
		}

		virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject)
		{
			classType* that = (classType*)this;

			if (!ppvObject)
				return E_INVALIDARG;

			*ppvObject = 0;

			if (::IsEqualGUID(riid, IID_IUnknown))
			{
				*ppvObject = (IUnknown*)(that);
				that->AddRef();
				return S_OK;
			}

			HRESULT hr = IMPL::QueryInterface(riid, ppvObject);
			if (hr != S_OK && inner_)
			{
				this->AddRef();

				hr = inner_->QueryInterface(riid, ppvObject);

				this->Release();
				return hr;
			}
			return hr;
		}

	private:
		IUnknown* inner_ = nullptr;
	};

	//////////////////////////////////////////////////////////////////////////////
	// specialization decorator for stack based object
	//
	// AddRef and Release will have a dummy implemenation, object lifetime
	// is supposed to implemented elsewhere
	//
	// mostly usable for COM callback interfaces like Eventsinks that get advised
	//////////////////////////////////////////////////////////////////////////////

	template<class T, class I, class ... Args>
	class implements<stack_object<T>(I, Args...)> : public implements<T(I, Args...)>
	{
	public:

		using classType = T;

		virtual ULONG __stdcall AddRef(void) override
		{
			return 2;
		}

		virtual ULONG __stdcall Release(void) override
		{
			return 1;
		}
	};

}
