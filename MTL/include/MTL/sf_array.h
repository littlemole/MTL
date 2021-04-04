#pragma once

#include "MTL/variant.h"

namespace MTL {



	///////////////////////////////////////////////////////////////////////
	// hate, hate, hate
	///////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////
	// encapsulate One-Dimensional SafeArrays
	//////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	// make SAFEARRAYBOUND more handsome
	//////////////////////////////////////////////////////////////////////

	struct sf_dim : public SAFEARRAYBOUND
	{
		sf_dim(long SZ, long LB = 0)
		{
			this->cElements = SZ;
			this->lLbound = LB;
		}
	};

	namespace details {

		//////////////////////////////////////////////////////////////////////
		// wrap low-level API
		//////////////////////////////////////////////////////////////////////

		class safearray_base
		{
		public:
			safearray_base(void)
				:sfArray_(0)
			{}

			virtual ~safearray_base()
			{}

			void create(VARTYPE vt, UINT cdims, SAFEARRAYBOUND* sfb)
			{
				sfArray_ = ::SafeArrayCreate(vt, cdims, sfb);
			}

			void create(VARTYPE vt, UINT cdims, const SAFEARRAYBOUND& sfb)
			{
				sfArray_ = ::SafeArrayCreate(vt, cdims, (SAFEARRAYBOUND*)&sfb);
			}

			void create(VARTYPE vt, int i)
			{
				sf_dim ab(i);
				sfArray_ = ::SafeArrayCreate(vt, 1, &ab);
			}

			void attach(SAFEARRAY* a)
			{
				sfArray_ = a;
			}

			void detach()
			{
				sfArray_ = 0;
			}

			void destroy()
			{
				if (sfArray_)
				{
					HRESULT hr = ::SafeArrayDestroy(sfArray_);
					sfArray_ = 0;
				}
			}

			HRESULT copy_to(SAFEARRAY** ppsaOut) const
			{
				sf_dim ab(size());
				if (*ppsaOut)
					::SafeArrayRedim(*ppsaOut, &ab);
				else
				{
					VARTYPE vt;
					this->getVartype(&vt);
					*ppsaOut = ::SafeArrayCreate(vt, 1, &ab);
				}
				return ::SafeArrayCopy(sfArray_, ppsaOut);
			}

			HRESULT copy_to(VARIANT& var) const
			{
				var.vt = var.vt | (VT_ARRAY | VT_VARIANT);
				if (var.vt & VT_BYREF)
					return copy_to(var.pparray);
				else
					return copy_to(&(var.parray));
			}

			UINT dimension() const
			{
				return ::SafeArrayGetDim(sfArray_);
			}

			template<class T>
			T item(long Index) const
			{
				T t;
				::SafeArrayGetElement(sfArray_, &Index, &t);
				return t;
			}

			HRESULT item(long Index, void* pv) const
			{
				return ::SafeArrayGetElement(sfArray_, &Index, pv);
			}

			HRESULT item(long* Index, void* pv) const
			{
				return ::SafeArrayGetElement(sfArray_, Index, pv);
			}

			UINT getElemsize() const
			{
				return ::SafeArrayGetElemsize(sfArray_);
			}

			HRESULT getIID(GUID* pguid) const
			{
				return ::SafeArrayGetIID(sfArray_, pguid);
			}

			HRESULT getLBound(long* pLbound, long dim = 1) const
			{
				return ::SafeArrayGetLBound(sfArray_, dim, pLbound);
			}

			HRESULT getUBound(long* pLbound, long dim = 1) const
			{
				return ::SafeArrayGetUBound(sfArray_, dim, pLbound);
			}

			HRESULT getVartype(VARTYPE* pvt) const
			{
				return ::SafeArrayGetVartype(sfArray_, pvt);
			}

			HRESULT put(long index, void* pv)
			{
				return ::SafeArrayPutElement(sfArray_, &index, pv);
			}

			HRESULT put(long* index, void* pv)
			{
				return ::SafeArrayPutElement(sfArray_, index, pv);
			}

			HRESULT reDim(SAFEARRAYBOUND* psaboundNew)
			{
				return ::SafeArrayRedim(sfArray_, psaboundNew);
			}

			HRESULT reDim(SAFEARRAYBOUND& psaboundNew)
			{
				return ::SafeArrayRedim(sfArray_, (SAFEARRAYBOUND*)&psaboundNew);
			}

			HRESULT reDim(int i)
			{
				sf_dim ab(i);
				return ::SafeArrayRedim(sfArray_, &ab);
			}

			HRESULT setIID(REFGUID guid)
			{
				return ::SafeArraySetIID(sfArray_, guid);
			}

			HRESULT accessData(void** ppData)
			{
				return ::SafeArrayAccessData(sfArray_, ppData);
			}

			HRESULT unaccessData()
			{
				return ::SafeArrayUnaccessData(sfArray_);
			}

			operator SAFEARRAY* ()
			{
				return sfArray_;
			}

			long size(int index = 0) const
			{
				return sfArray_->rgsabound[index].cElements;
			}

			long ubound(int index = 0) const
			{
				long ub = 0;
				::SafeArrayGetUBound(sfArray_, index, &ub);
				return ub;
			}

			long lbound(int index = 0) const
			{
				long lb = 0;
				::SafeArrayGetLBound(sfArray_, index, &lb);
				return lb;
			}


		protected:
			SAFEARRAY* sfArray_;
		};

	} // end namespace 

	//////////////////////////////////////////////////////////////////////
	// manage direct array access
	//////////////////////////////////////////////////////////////////////

	template<class T>
	class sf_array_view
	{
	public:

		sf_array_view(SAFEARRAY* s)
			: data_(0)
		{
			sfb_.attach(s);
			sfb_.accessData((void**)&data_);
		}

		~sf_array_view()
		{
			if (data_)
				sfb_.unaccessData();
			sfb_.detach();
		}

		T* operator()()
		{
			return data_;
		}

		T& operator[] (int index)
		{
			return ((data_[index]));
		}

		long size()
		{
			return sfb_.size();
		}

	private:
		details::safearray_base		sfb_;
		T* data_;
	};

	namespace details {

		//////////////////////////////////////////////////////////////////////
		// initialization helper
		//////////////////////////////////////////////////////////////////////

		template<VARTYPE VT>
		inline void sf_array_init(details::safearray_base& sf, const std::vector<std::wstring>& v);

		// spec for BSTR
		template<>
		inline void sf_array_init<VT_BSTR>(details::safearray_base& sf, const std::vector<std::wstring>& v)
		{
			sf_array_view<BSTR> sfa(sf);
			for (int i = 0; i < v.size(); i++)
			{
				sfa[i] = ::SysAllocString(v[i].c_str());
			}
		};

		template<>
		inline void sf_array_init<VT_VARIANT>(details::safearray_base& sf, const std::vector<std::wstring>& v)
		{
			sf_array_view<VARIANT> sfa(sf);
			for (int i = 0; i < v.size(); i++)
			{
				::VariantInit(&(sfa[i]));
				sfa[i].vt = VT_BSTR;
				sfa[i].bstrVal = ::SysAllocString(v[i].c_str());
			}
		};


	}

	//////////////////////////////////////////////////////////////////////
	// one dimensional SafeArray of VARTYPE VT
	//////////////////////////////////////////////////////////////////////

	template<VARTYPE VT>
	class sf_array : public details::safearray_base
	{
	public:
		sf_array()
		{}

		sf_array(int size)
		{
			sf_dim(size);
			this->create(sfb);
		}

		sf_array(const SAFEARRAYBOUND& sfb)
		{
			this->create(sfb);
		}

		explicit sf_array(SAFEARRAY* sf)
		{
			long ub, lb;
			::SafeArrayGetUBound(sf, 1, &ub);
			::SafeArrayGetLBound(sf, 1, &lb);
			sf_dim ab(ub - lb + 1, lb);
			this->create(ab);
			::SafeArrayCopy(sf, &(this->sfArray_));
		}

		explicit sf_array(SAFEARRAY*&& sf)
		{
			this->sfArray_ = sf;
			sf = 0;
		}


		explicit sf_array(const std::vector<std::wstring>& v)
		{
			sf_dim ab(v.size());
			this->create(ab);
			details::sf_array_init<VT>(*this, v);
		}

		~sf_array()
		{
			safearray_base::destroy();
		}

		sf_array& operator=(const SAFEARRAY* sf)
		{
			this->destroy();
			long ub, lb;
			::SafeArrayGetUBound((SAFEARRAY*)sf, 1, &ub);
			::SafeArrayGetLBound((SAFEARRAY*)sf, 1, &lb);
			sf_dim ab(ub - lb + 1, lb);
			this->create(ab);
			::SafeArrayCopy((SAFEARRAY*)sf, &sfArray_);
			return *this;
		}

		sf_array& operator=(const VARIANT& var)
		{
			if (var.vt & VT_BYREF)
				*this = *(var.pparray);
			else
				*this = (var.parray);

			return *this;
		}

		SAFEARRAY** operator&()
		{
			return &sfArray_;
		}

		operator SAFEARRAY* ()
		{
			return sfArray_;
		}

		void create(const SAFEARRAYBOUND& sfb, UINT cDim = 1)
		{
			if (sfArray_)
				destroy();
			sfArray_ = ::SafeArrayCreate(VT, cDim, (SAFEARRAYBOUND*)&sfb);
		}

	};



} // end namespace 

