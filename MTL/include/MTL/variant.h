#pragma once

#include "MTL/bstr.h"

namespace MTL {

	namespace details {

		///////////////////////////////////////////////////////////////////////
		// these VARIANTs, oh girl
		///////////////////////////////////////////////////////////////////////


		template<VARTYPE vt>
		struct VariantTraits
		{
		public:
			typedef void* type;
			static size_t    size() { return 0; }
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_BOOL>
		{
			typedef VARIANT_BOOL     type;
			static size_t			 size() 
			{
				return sizeof(VARIANT_BOOL);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_BSTR>
		{
			typedef BSTR      type;
			static size_t     size() 
			{
				return 0;
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_INT>
		{
			typedef INT       type;
			static size_t     size() 
			{
				return sizeof(INT);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_UINT>
		{
			typedef UINT      type;
			static size_t     size() 
			{
				return sizeof(UINT);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_I1>
		{
			typedef CHAR      type;
			static size_t     size() 
			{
				return sizeof(CHAR);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_I2>
		{
			typedef SHORT     type;
			static size_t     size() 
			{
				return sizeof(SHORT);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_I4>
		{
			typedef LONG      type;
			static size_t     size() 
			{
				return sizeof(LONG);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_I8>
		{
			typedef LONGLONG  type;
			static size_t     size() 
			{
				return sizeof(LONGLONG);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_UI1>
		{
			typedef BYTE      type;
			static size_t     size() 
			{
				return sizeof(BYTE);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_UI2>
		{
			typedef USHORT     type;
			static size_t      size() 
			{
				return sizeof(USHORT);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_UI4>
		{
			typedef ULONG      type;
			static size_t      size() 
			{
				return sizeof(ULONG);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_UI8>
		{
			typedef ULONGLONG  type;
			static size_t      size() 
			{
				return sizeof(ULONGLONG);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_R4>
		{
			typedef FLOAT      type;
			static size_t      size() 
			{
				return sizeof(FLOAT);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct VariantTraits<VT_R8>
		{
			typedef DOUBLE     type;
			static size_t      size() 
			{
				return sizeof(DOUBLE);
			}
		};

		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		template<class T>
		struct VarTraits {
			static VARTYPE vt() { return VT_EMPTY; };
		};

		template<>
		struct VarTraits<VARIANT_BOOL> {
			static VARTYPE vt() { return VT_BOOL; }
		};

		template<>
		struct VarTraits<VARIANT_BOOL*> {
			static VARTYPE vt() { return VT_BOOL | VT_BYREF; }
		};


		template<>
		struct VarTraits<bool> {
			static VARTYPE vt() { return VT_BOOL; }
		};

		template<>
		struct VarTraits<BSTR> {
			static VARTYPE vt() { return VT_BSTR; }
		};

		template<>
		struct VarTraits<bstr> {
			static VARTYPE vt() { return VT_BSTR; }
		};

		template<>
		struct VarTraits<BSTR*> {
			static VARTYPE vt() { return VT_BSTR | VT_BYREF; }
		};


		template<>
		struct VarTraits<INT> {
			static VARTYPE vt() { return VT_I4; } // changed from VT_INT
		};

		template<>
		struct VarTraits<INT*> {
			static VARTYPE vt() { return VT_I4 | VT_BYREF; } // changed from VT_INT
		};

		template<>
		struct VarTraits<UINT> {
			static VARTYPE vt() { return VT_UI4; } // changed from VT_UINT
		};

		template<>
		struct VarTraits<UINT*> {
			static VARTYPE vt() { return VT_UI4 | VT_BYREF; } // changed from VT_UINT
		};

		template<>
		struct VarTraits<CHAR> {
			static VARTYPE vt() { return VT_I1; }
		};

		template<>
		struct VarTraits<CHAR*> {
			static VARTYPE vt() { return VT_I1 | VT_BYREF; }
		};

		template<>
		struct VarTraits<LONG> {
			static VARTYPE vt() { return VT_I4; }
		};

		template<>
		struct VarTraits<LONG*> {
			static VARTYPE vt() { return VT_I4 | VT_BYREF; }
		};


		template<>
		struct VarTraits<LONGLONG> {
			static VARTYPE vt() { return VT_I8; }
		};

		template<>
		struct VarTraits<LONGLONG*> {
			static VARTYPE vt() { return VT_I8 | VT_BYREF; }
		};

		template<>
		struct VarTraits<BYTE> {
			static VARTYPE vt() { return VT_UI1; }
		};

		template<>
		struct VarTraits<BYTE*> {
			static VARTYPE vt() { return VT_UI1 | VT_BYREF; }
		};


		template<>
		struct VarTraits<USHORT> {
			static VARTYPE vt() { return VT_UI2; }
		};

		template<>
		struct VarTraits<USHORT*> {
			static VARTYPE vt() { return VT_UI2 | VT_BYREF; }
		};

		template<>
		struct VarTraits<ULONG> {
			static VARTYPE vt() { return VT_UI4; }
		};

		template<>
		struct VarTraits<ULONG*> {
			static VARTYPE vt() { return VT_UI4 | VT_BYREF; }
		};

		template<>
		struct VarTraits<ULONGLONG> {
			static VARTYPE vt() { return VT_UI8; }
		};

		template<>
		struct VarTraits<ULONGLONG*> {
			static VARTYPE vt() { return VT_UI8 | VT_BYREF; }
		};

		template<>
		struct VarTraits<FLOAT> {
			static VARTYPE vt() { return VT_R4; }
		};


		template<>
		struct VarTraits<FLOAT*> {
			static VARTYPE vt() { return VT_R4 | VT_BYREF; }
		};

		template<>
		struct VarTraits<DOUBLE> {
			static VARTYPE vt() { return VT_R8; }
		};

		template<>
		struct VarTraits<DOUBLE*> {
			static VARTYPE vt() { return VT_R8 | VT_BYREF; }
		};

		template<>
		struct VarTraits<IUnknown*> {
			static VARTYPE vt() { return VT_UNKNOWN; }
		};

		template<>
		struct VarTraits<IUnknown**> {
			static VARTYPE vt() { return VT_UNKNOWN | VT_BYREF; }
		};

		template<>
		struct VarTraits<IDispatch*> {
			static VARTYPE vt() { return VT_DISPATCH; }
		};

		template<>
		struct VarTraits<IDispatch**> {
			static VARTYPE vt() { return VT_DISPATCH | VT_BYREF; }
		};

	} // end namespace details

	///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	class variant : public VARIANT
	{
	public:
		variant()  { ::VariantInit(this); }
		~variant() { ::VariantClear(this); }

		explicit variant(VARENUM vt)
		{
			::VariantInit(this);
			this->vt = vt;
		}

		explicit variant(const VARIANT& v2)
		{
			::VariantInit(this);
			::VariantCopy(this, &v2);
		}

		explicit variant(const VARIANT* v2)
		{
			::VariantInit(this);
			::VariantCopy(this, v2);
		}

		explicit variant(VARIANT&& v2)
		{
			::VariantInit(this);
			::VariantCopy(this, &v2);
			v2.pbVal = 0;
			v2.vt = VT_EMPTY;
		}

		explicit variant(VARIANT*&& v2)
		{
			::VariantInit(this);
			::VariantCopy(this, v2);
			v2->pbVal = 0;
			v2->vt = VT_EMPTY;
		}

		variant(const variant& v2)
		{
			::VariantInit(this);
			::VariantCopy(this, &v2);
		}

		explicit variant(const std::string& str)
		{
			::VariantInit(this);
			vt = VT_BSTR;
			bstrVal = ::SysAllocString(MTL::to_wstring(str).c_str());
		}

		explicit variant(const std::wstring& str)
		{
			::VariantInit(this);
			vt = VT_BSTR;
			bstrVal = ::SysAllocString(str.c_str());
		}

		explicit variant(const char* c)
		{
			::VariantInit(this);
			vt = VT_BSTR;
			bstrVal = ::SysAllocString(MTL::to_wstring(c).c_str());
		}

		explicit variant(const wchar_t* wc)
		{
			::VariantInit(this);
			vt = VT_BSTR;
			bstrVal = ::SysAllocString(wc);
		}

		explicit variant(long l)			 { ::VariantInit(this); vt = VT_I4;	  lVal = l; }
		explicit variant(float f)			 { ::VariantInit(this); vt = VT_R4;	  fltVal = f; }
		explicit variant(double d)			 { ::VariantInit(this); vt = VT_R8;	  dblVal = d; }
		explicit variant(VARIANT_BOOL b)	 { ::VariantInit(this); vt = VT_BOOL; boolVal = b; }
		explicit variant(bool b)			 { ::VariantInit(this); vt = VT_BOOL; boolVal = b ? VARIANT_TRUE : VARIANT_FALSE; }
		explicit variant(CY cy)				 { ::VariantInit(this); vt = VT_CY;	  cyVal = cy; }
		explicit variant(BSTR b)			 { ::VariantInit(this); vt = VT_BSTR; bstrVal = ::SysAllocString(b); }
		explicit variant(const MTL::bstr& b) { ::VariantInit(this); vt = VT_BSTR; bstrVal = *b; }

		explicit variant(IUnknown* iu)
		{
			::VariantInit(this);
			vt = VT_UNKNOWN;
			punkVal = iu;
			if (iu)
				punkVal->AddRef();
		}

		explicit variant(IDispatch* di)
		{
			::VariantInit(this);
			vt = VT_DISPATCH;
			pdispVal = di;
			if (di)
				pdispVal->AddRef();
		}

		explicit variant(long* l)			{ ::VariantInit(this); vt = VT_BYREF | VT_I4;	plVal = l; }
		explicit variant(float* f)			{ ::VariantInit(this); vt = VT_BYREF | VT_R4;	pfltVal = f; }
		explicit variant(double* d)			{ ::VariantInit(this); vt = VT_BYREF | VT_R8;	pdblVal = d; }
		explicit variant(VARIANT_BOOL* b)	{ ::VariantInit(this); vt = VT_BYREF | VT_BOOL; pboolVal = b; }
		explicit variant(CY* cy)			{ ::VariantInit(this); vt = VT_BYREF | VT_CY;	pcyVal = cy; }
		explicit variant(BSTR* b)			{ ::VariantInit(this); vt = VT_BYREF | VT_BSTR; pbstrVal = b; }

		explicit variant(IUnknown** iu)
		{
			::VariantInit(this);
			vt = VT_BYREF | VT_UNKNOWN;
			ppunkVal = iu;
			if (iu && *iu)
				(*ppunkVal)->AddRef();
		}

		explicit variant(IDispatch** di)
		{
			::VariantInit(this);
			vt = VT_BYREF | VT_DISPATCH;
			ppdispVal = di;
			if (di)
				(*ppdispVal)->AddRef();
		}

		explicit variant(VARIANT* vr)		{ ::VariantInit(this); vt = VT_BYREF | VT_VARIANT;  pvarVal = vr; }
		explicit variant(void* vo)			{ ::VariantInit(this); vt = VT_BYREF;				byref = vo; }
		explicit variant(char c)			{ ::VariantInit(this); vt = VT_I1;					cVal = c; }
		explicit variant(unsigned long l)	{ ::VariantInit(this); vt = VT_UI4;					ulVal = l; }
		explicit variant(int i)				{ ::VariantInit(this); vt = VT_I4;					intVal = i; }
		explicit variant(unsigned int i)	{ ::VariantInit(this); vt = VT_UI4;					uintVal = i; }
		explicit variant(unsigned long* l)	{ ::VariantInit(this); vt = VT_BYREF | VT_UI4;		pulVal = l; }
		explicit variant(int* i)			{ ::VariantInit(this); vt = VT_BYREF | VT_I4;		pintVal = i; }
		explicit variant(unsigned int* i)	{ ::VariantInit(this); vt = VT_BYREF | VT_UI4;		puintVal = i; }

		template<VARTYPE VT>
		explicit variant(SAFEARRAY** sfa)	{ ::VariantInit(this); vt = VT_ARRAY | VT | VT_BYREF; pparray = sfa; }

		template<VARTYPE VT>
		explicit variant(SAFEARRAY* sfa)	{ ::VariantInit(this); vt = VT_ARRAY | VT; parray = sfa; }

		variant& operator=(const VARIANT& v2)
		{
			::VariantClear(this);
			VARIANT& tmp = (VARIANT&)v2;
			::VariantCopy(this, &v2);
			return *this;
		}

		variant& operator=(const variant& v2)
		{
			::VariantClear(this);
			::VariantCopy(this, &v2);
			return *this;
		}

		void Copy(variant& v) const
		{
			::VariantClear(&v);
			::VariantCopy(&v, this);
		}

		bool isType(VARTYPE VT) const
		{
			return (vt == VT);
		}

		void changeType(VARTYPE VT)
		{
			::VariantChangeType(this, this, VARIANT_NOUSEROVERRIDE, VT);
		}

		std::string to_string(int cp = CP_UTF8) const
		{
			if (vt == VT_BSTR)
			{
				return MTL::to_string(bstrVal,-1,cp);
			}
			if (vt == (VT_BSTR | VT_BYREF))
			{
				return MTL::to_string(*pbstrVal, -1, cp);
			}

			variant tmp(*this);
			tmp.changeType(VT_BSTR);
			return tmp.to_string(cp);
		}

		std::wstring to_wstring() const
		{
			if (vt == VT_BSTR)
			{
				return std::wstring(bstrVal);
			}
			if (vt == (VT_BSTR | VT_BYREF))
			{
				return std::wstring(*pbstrVal);
			}
			variant tmp(*this);
			tmp.changeType(VT_BSTR);
			return tmp.to_wstring();
		}

		template<class T>
		T& valueOf() const
		{
			if (vt == details::VarTraits<T>::vt())
			{
				return (T&)(*pintVal);
			}
			variant tmp(*this);
			tmp.changeType(details::VarTraits<T>::vt());
			return (T&)(*tmp.pintVal);
		}
	};


	namespace details {
		template<VARENUM V>
		class concreteVariant : public variant
		{
		public:
			concreteVariant()
			{
				vt = V;
				pdispVal = 0;
			}
		};
	}

	class vNull  : public details::concreteVariant<VT_NULL>  {};
	class vEmpty : public details::concreteVariant<VT_EMPTY> {};
	class vError : public details::concreteVariant<VT_ERROR> {};


	///////////////////////////////////////////////////////////////////////
	// hate, hate, hate
	///////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////
	// encapsulate One-Dimensional SafeArrays
	//////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	// make SAFEARRAYBOUND more handsome
	//////////////////////////////////////////////////////////////////////

	struct ArrayBound : public SAFEARRAYBOUND
	{
		ArrayBound(long SZ, long LB = 0)
		{
			this->cElements = SZ;
			this->lLbound = LB;
		}
	};

	namespace details {

		//////////////////////////////////////////////////////////////////////
		// wrap low-level API
		//////////////////////////////////////////////////////////////////////

		class SafeArrayBase
		{
		public:
			SafeArrayBase(void)
				:sfArray_(0)
			{}

			virtual ~SafeArrayBase()
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
				ArrayBound ab(i);
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

			HRESULT copy(SAFEARRAY** ppsaOut) const
			{
				ArrayBound ab(size());
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

			HRESULT copy(VARIANT& var) const
			{
				var.vt = var.vt | (VT_ARRAY | VT_VARIANT);
				if (var.vt & VT_BYREF)
					return copy(var.pparray);
				else
					return copy(&(var.parray));
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
				ArrayBound ab(i);
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
	class SFAccess
	{
	public:

		SFAccess(SAFEARRAY* s)
			: data_(0)
		{
			sfb_.attach(s);
			sfb_.accessData((void**)&data_);
		}

		~SFAccess()
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
		details::SafeArrayBase		sfb_;
		T* data_;
	};

	namespace details {

		//////////////////////////////////////////////////////////////////////
		// initialization helper
		//////////////////////////////////////////////////////////////////////

		template<VARTYPE VT>
		inline void SafeArrayInit(details::SafeArrayBase& sf, const std::vector<std::wstring>& v);

		// spec for BSTR
		template<>
		inline void SafeArrayInit<VT_BSTR>(details::SafeArrayBase& sf, const std::vector<std::wstring>& v)
		{
			SFAccess<BSTR> sfa(sf);
			for (int i = 0; i < v.size(); i++)
			{
				sfa[i] = ::SysAllocString(v[i].c_str());
			}
		};

		template<>
		inline void SafeArrayInit<VT_VARIANT>(details::SafeArrayBase& sf, const std::vector<std::wstring>& v)
		{
			SFAccess<VARIANT> sfa(sf);
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
	class SafeArray : public details::SafeArrayBase
	{
	public:
		SafeArray()
		{}

		SafeArray(const SAFEARRAYBOUND& sfb)
		{
			this->create(sfb);
		}

		SafeArray(SAFEARRAY* sf)
		{
			long ub, lb;
			::SafeArrayGetUBound(sf, 1, &ub);
			::SafeArrayGetLBound(sf, 1, &lb);
			ArrayBound ab(ub - lb + 1, lb);
			this->create(ab);
			::SafeArrayCopy(sf, &(this->sfArray_));
		}

		SafeArray(SAFEARRAY*&& sf)
		{
			this->sfArray_ = sf;
			sf = 0;
		}


		explicit SafeArray(const std::vector<std::wstring>& v)
		{
			ArrayBound ab(v.size());
			this->create(ab);
			details::SafeArrayInit<VT>(*this, v);
		}

		~SafeArray(void)
		{
			SafeArrayBase::destroy();
		}

		SafeArray& operator=(const SAFEARRAY* sf)
		{
			this->destroy();
			long ub, lb;
			::SafeArrayGetUBound((SAFEARRAY*)sf, 1, &ub);
			::SafeArrayGetLBound((SAFEARRAY*)sf, 1, &lb);
			ArrayBound ab(ub - lb + 1, lb);
			this->create(ab);
			::SafeArrayCopy((SAFEARRAY*)sf, &sfArray_);
			return *this;
		}

		SafeArray& operator=(const VARIANT& var)
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

		void create(const SAFEARRAYBOUND& sfb)
		{
			if (sfArray_)
				destroy();
			sfArray_ = ::SafeArrayCreate(VT, 1, (SAFEARRAYBOUND*)&sfb);
		}

	};



} // end namespace 

