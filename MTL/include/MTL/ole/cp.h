#pragma once

#include "mtl/punk.h"
#include "mtl/obj/impl.h"
#include "mtl/disp/aut.h"
#include <ocidl.h>
#include <OleCtl.h>

namespace mtl {


	class cookies
	{
	public:
		DWORD get()
		{
			if (!free_cookies_.empty())
			{
				DWORD c = free_cookies_.front();
				free_cookies_.pop_front();
				return c;
			}

			DWORD c = (DWORD)cookies_.size() + 1;
			cookies_.push_back(c);
			return c;
		}

		void release(DWORD c)
		{
			for (std::list<DWORD>::iterator it = cookies_.begin(); it != cookies_.end(); it++)
			{
				if ((*it) == c)
				{
					free_cookies_.push_back(c);
					cookies_.erase(it);
					return;
				}
			}
		}

	protected:
		std::list<DWORD> cookies_;
		std::list<DWORD> free_cookies_;

	};

	class connection_point: public implements< connection_point(IConnectionPoint)>
	{
	public:

		connection_point(IConnectionPointContainer* cpc)
			: cpc_(cpc)
		{}
		
		template<class ... Args>
		void fire(DISPID id, Args ... args)
		{
			for (auto& it : sinks_)
			{
				punk<IDispatch> disp(it.second);
				if (disp)
				{
					automation(*disp).member(id).invoke(args...);
				}
			}
		}

		virtual HRESULT __stdcall GetConnectionInterface(IID* pIID)
		{
			return E_NOTIMPL;
		}


		virtual HRESULT __stdcall Advise(IUnknown* pUnkSink, DWORD* pdwCookie) override
		{
			DWORD cookie = cookies_.get();
			sinks_[cookie] = pUnkSink;
			*pdwCookie = cookie;
			return S_OK;
		}

		virtual HRESULT __stdcall Unadvise(DWORD dwCookie) override
		{
			sinks_.erase(dwCookie);
			cookies_.release(dwCookie);
			return S_OK;
		}

		virtual HRESULT __stdcall EnumConnections(IEnumConnections** ppEnum) override
		{
			return E_NOTIMPL;
		}

		virtual HRESULT __stdcall GetConnectionPointContainer(IConnectionPointContainer** ppCPC) override
		{
			if (!ppCPC) return E_INVALIDARG;
			return cpc_->QueryInterface(IID_IConnectionPointContainer, (void**)ppCPC);
		}

		void container(IConnectionPointContainer* c)
		{
			cpc_ = c;
		}

	protected:
		IConnectionPointContainer* cpc_;
		std::map<DWORD, punk<IUnknown>> sinks_;
		cookies cookies_;
	};


	template<class I>
	class connection_point_impl : public connection_point
	{
	public:

		connection_point_impl()
		{}

		connection_point_impl(IConnectionPointContainer* cpc)
			: connection_point(cpc)
		{}

		~connection_point_impl()
		{}

		virtual HRESULT __stdcall GetConnectionInterface(IID* pIID) override
		{
			if (!pIID) return E_INVALIDARG;
			*pIID = __uuidof(I);
			return S_OK;
		}
	};


	class enum_connection_point : public implements<enum_connection_point(IEnumConnectionPoints)>
	{
	public:

		enum_connection_point(const std::vector<punk<IConnectionPoint>>& v)
			: unks_(v)
		{}

		virtual HRESULT __stdcall Next(ULONG celt, LPCONNECTIONPOINT* rgelt, ULONG* pceltFetched) override
		{
			if (celt != 1)
				return E_INVALIDARG;

			if (rgelt == 0)
				return E_INVALIDARG;

			if (pceltFetched)
				*pceltFetched = 0;

			for (ULONG i = 0; i < celt; i++)
			{
				rgelt[i] = 0;
			}

			if (pos_ >= unks_.size())
			{
				return S_FALSE;
			}

			HRESULT hr = unks_[pos_].query_interface(&(rgelt[0]));
			if (hr != S_OK)
				return hr;

			if (pceltFetched)
				*pceltFetched = 1;
			pos_++;
			return hr;
		}

		virtual HRESULT __stdcall Skip(ULONG celt) override
		{
			pos_ += celt;
			if (pos_ >= unks_.size())
				return S_FALSE;

			return S_OK;
		}

		virtual HRESULT __stdcall Reset(void) override
		{
			pos_ = 0;
			return S_OK;
		}

		virtual HRESULT __stdcall Clone(IEnumConnectionPoints** ppenum) override
		{
			punk<IEnumConnectionPoints> unk(new enum_connection_point(unks_));
			return unk.query_interface(ppenum);
		}

	private:
		std::vector<punk<IConnectionPoint>> unks_;
		int pos_ = 0;
	};

	template<class T>
	class connection_point_container;

	template<class T, class ... Args>
	class connection_point_container<T(Args...)> : public IConnectionPointContainer
	{
	public:

		connection_point_container()
		{
			connection_points<Args...>();
		}

		template<class I>
		void fire(DISPID id, Args ... args)
		{
			for (auto& cp : cps_)
			{
				IID iid;
				cp->GetConnectionInterface(&iid);
				if (::IsEqualIID(__uuidof(I), iid))
				{
					cp->fire(id, args...);
				}
			}
		}


		virtual HRESULT __stdcall EnumConnectionPoints(IEnumConnectionPoints** ppEnum) override
		{
			punk<enum_connection_point> cps(new enum_connection_point(cps_));
			return cps.query_interface(ppEnum);
		}

		virtual HRESULT __stdcall FindConnectionPoint(REFIID riid, IConnectionPoint** ppCP) override
		{
			for (auto& it : cps_)
			{
				IID iid;
				it->GetConnectionInterface(&iid);
				if (::IsEqualIID(riid, iid))
				{
					return it.query_interface(ppCP);
				}
			}
			return CONNECT_E_NOCONNECTION;
		}

	private:

		std::vector<punk<IConnectionPoint>> cps_;

		template<class I, class ...Args>
		void connection_points()
		{
			make_connection_point<I>();
			if constexpr (sizeof...(Args) != 0)
			{
				connection_points<Args...>();
			}
		}

		template<class I>
		void make_connection_point()
		{
			IConnectionPointContainer* cpc = (IConnectionPointContainer*)this;
			punk<IConnectionPoint> cp(new connection_point_impl<I>(cpc));
			cps_.push_back(cp);
		}

	};
	

	namespace details
	{
		template<class T, class I, class ... Args>
		class interfaces<T(connection_point_container<I>, Args...)>
		{
		public:

			static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
			{
				if (::IsEqualGUID(riid, IID_IConnectionPointContainer))
				{
					*ppvObject = (IConnectionPointContainer*)(that);
					that->AddRef();
					return S_OK;
				}

				return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
			}
		};

	}
}