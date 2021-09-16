#pragma once

#include "MTL/disp/bstr.h"

namespace MTL {

	namespace details {

		///////////////////////////////////////////////////////////////////////
		// these VARIANTs, oh girl
		///////////////////////////////////////////////////////////////////////


		template<VARTYPE vt>
		struct vt_traits
		{
		public:
			typedef void* type;
			static constexpr size_t    size() { return 0; }
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_BOOL>
		{
			typedef VARIANT_BOOL     type;
			static constexpr size_t			 size()
			{
				return sizeof(VARIANT_BOOL);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_BSTR>
		{
			typedef BSTR      type;
			static constexpr size_t     size()
			{
				return 0;
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_INT>
		{
			typedef INT       type;
			static constexpr size_t     size()
			{
				return sizeof(INT);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_UINT>
		{
			typedef UINT      type;
			static constexpr size_t     size()
			{
				return sizeof(UINT);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_I1>
		{
			typedef CHAR      type;
			static constexpr size_t     size()
			{
				return sizeof(CHAR);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_I2>
		{
			typedef SHORT     type;
			static constexpr size_t     size()
			{
				return sizeof(SHORT);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_I4>
		{
			typedef LONG      type;
			static constexpr size_t     size()
			{
				return sizeof(LONG);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_I8>
		{
			typedef LONGLONG  type;
			static constexpr size_t     size()
			{
				return sizeof(LONGLONG);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_UI1>
		{
			typedef BYTE      type;
			static constexpr size_t     size()
			{
				return sizeof(BYTE);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_UI2>
		{
			typedef USHORT     type;
			static constexpr size_t      size()
			{
				return sizeof(USHORT);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_UI4>
		{
			typedef ULONG      type;
			static constexpr size_t      size()
			{
				return sizeof(ULONG);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_UI8>
		{
			typedef ULONGLONG  type;
			static constexpr size_t      size()
			{
				return sizeof(ULONGLONG);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_R4>
		{
			typedef FLOAT      type;
			static constexpr size_t      size()
			{
				return sizeof(FLOAT);
			}
		};

		///////////////////////////////////////////////////////////////////////

		template<>
		struct vt_traits<VT_R8>
		{
			typedef DOUBLE     type;
			static constexpr size_t      size()
			{
				return sizeof(DOUBLE);
			}
		};

		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		template<class T>
		struct var_traits {
			static constexpr VARTYPE vt()		      { return VT_EMPTY; };
			static VARIANT* value(const VARIANT* v)   { return 0; }
		};

		template<>
		struct var_traits<VARIANT_BOOL> {
			static constexpr VARTYPE vt()				  { return VT_BOOL; }
			static VARIANT_BOOL value(const VARIANT* v)	  { return v->boolVal; }
		};

		template<>
		struct var_traits<VARIANT_BOOL*> {
			static constexpr VARTYPE vt()					{ return VT_BOOL | VT_BYREF; }
			static VARIANT_BOOL* value(const VARIANT* v)	{ return v->pboolVal; }
		};


		template<>
		struct var_traits<bool> {
			static constexpr VARTYPE vt()				{ return VT_BOOL; }
			static bool value(const VARIANT* v)			{ return v->boolVal == VARIANT_TRUE; }
		};

		template<>
		struct var_traits<bool*> {
			static constexpr VARTYPE vt()		{ return VT_BOOL | VT_BYREF; }
			static bool value(const VARIANT* v) { return *(v->pboolVal) == VARIANT_TRUE; }
		};

		template<>
		struct var_traits<BSTR> {
			static constexpr VARTYPE vt() { return VT_BSTR; }
			static BSTR value(const VARIANT* v) { return ::SysAllocStringLen(v->bstrVal, ::SysStringLen(v->bstrVal)); }
		};

		template<>
		struct var_traits<BSTR*> {
			static constexpr VARTYPE vt() { return VT_BSTR | VT_BYREF;; }
			static BSTR value(const VARIANT* v) { return ::SysAllocStringLen( *(v->pbstrVal), ::SysStringLen( *(v->pbstrVal)) ); }
		};

		template<>
		struct var_traits<b_copy> {
			static constexpr VARTYPE vt() { return VT_BSTR; }
		};

		template<>
		struct var_traits<b_move> {
			static constexpr VARTYPE vt() { return VT_BSTR; }
		};

		template<>
		struct var_traits<bstr> {
			static constexpr VARTYPE vt() { return VT_BSTR; }
			static bstr value(const VARIANT* v) { return bstr(b_copy(v->bstrVal)); }
		};

		template<>
		struct var_traits<bstr*> {
			static constexpr VARTYPE vt() { return VT_BSTR | VT_BYREF; }
			static bstr value(const VARIANT* v) { return bstr(b_copy(*(v->pbstrVal))); }
		};

		template<>
		struct var_traits<INT> {
			static constexpr VARTYPE vt() { return VT_I4; } // changed from VT_INT
			static INT value(const VARIANT* v)  { return v->lVal; }
		};

		template<>
		struct var_traits<INT*> {
			static constexpr VARTYPE vt() { return VT_I4 | VT_BYREF; } // changed from VT_INT
			static INT value(const VARIANT* v) { return *(v->plVal); }
		};

		template<>
		struct var_traits<UINT> {
			static constexpr VARTYPE vt() { return VT_UI4; } // changed from VT_UINT
			static UINT value(const VARIANT* v) { return v->ulVal; }
		};

		template<>
		struct var_traits<UINT*> {
			static constexpr VARTYPE vt() { return VT_UI4 | VT_BYREF; } // changed from VT_UINT
			static UINT value(const VARIANT* v) { return *(v->pulVal); }
		};

		template<>
		struct var_traits<CHAR> {
			static constexpr VARTYPE vt() { return VT_I1; }
			static CHAR value(const VARIANT* v) { return v->cVal; }
		};

		template<>
		struct var_traits<CHAR*> {
			static constexpr VARTYPE vt() { return VT_I1 | VT_BYREF; }
			static CHAR value(const VARIANT* v) { return *(v->pcVal); }
		};

		template<>
		struct var_traits<std::wstring> {
			static constexpr VARTYPE vt() { return VT_BSTR; }
			static std::wstring value(const VARIANT* v) { return std::wstring(v->bstrVal,::SysStringLen(v->bstrVal)); }
		};

		template<>
		struct var_traits<std::wstring*> {
			static constexpr VARTYPE vt() { return VT_BSTR | VT_BYREF; }
			static std::wstring value(const VARIANT* v) { return std::wstring(*(v->pbstrVal), ::SysStringLen( *(v->pbstrVal))); }
		};

		template<>
		struct var_traits<LONG> {
			static constexpr VARTYPE vt() { return VT_I4; }
			static LONG value(const VARIANT* v) { return v->lVal; }
		};

		template<>
		struct var_traits<LONG*> {
			static constexpr VARTYPE vt() { return VT_I4 | VT_BYREF; }
			static LONG value(const VARIANT* v) { return *(v->plVal); }
		};

		template<>
		struct var_traits<LONGLONG> {
			static constexpr VARTYPE vt()     { return VT_I8; }
			static LONGLONG value(const VARIANT* v) { return v->llVal; }
		};

		template<>
		struct var_traits<LONGLONG*> {
			static constexpr VARTYPE vt() { return VT_I8 | VT_BYREF; }
			static LONGLONG value(const VARIANT* v) { return *(v->pllVal); }
		};

		template<>
		struct var_traits<BYTE> {
			static constexpr VARTYPE vt() { return VT_UI1; }
			static BYTE value(const VARIANT* v) { return v->bVal; }
		};

		template<>
		struct var_traits<BYTE*> {
			static constexpr VARTYPE vt() { return VT_UI1 | VT_BYREF; }
			static BYTE value(const VARIANT* v) { return *(v->pbVal); }
		};


		template<>
		struct var_traits<USHORT> {
			static constexpr VARTYPE vt()	{ return VT_UI2; }
			static USHORT value(const VARIANT* v) { return v->uiVal; }
		};

		template<>
		struct var_traits<USHORT*> {
			static constexpr VARTYPE vt() { return VT_UI2 | VT_BYREF; }
			static USHORT value(const VARIANT* v) { return *(v->puiVal); }
		};

		template<>
		struct var_traits<ULONG> {
			static constexpr VARTYPE vt()  { return VT_UI4; }
			static ULONG value(const VARIANT* v) { return v->ulVal; }
		};

		template<>
		struct var_traits<ULONG*> {
			static constexpr VARTYPE vt()  { return VT_UI4 | VT_BYREF; }
			static ULONG value(const VARIANT* v) { return *(v->pulVal); }
		};

		template<>
		struct var_traits<ULONGLONG> {
			static constexpr VARTYPE vt()		{ return VT_UI8; }
			static ULONGLONG value(const VARIANT* v)  { return v->ullVal; }
		};

		template<>
		struct var_traits<ULONGLONG*> {
			static constexpr VARTYPE vt()		{ return VT_UI8 | VT_BYREF; }
			static ULONGLONG value(const VARIANT* v)	{ return *(v->pullVal); }
		};

		template<>
		struct var_traits<FLOAT> {
			static constexpr VARTYPE vt()  { return VT_R4; }
			static FLOAT value(const VARIANT* v) { return v->fltVal; }
		};


		template<>
		struct var_traits<FLOAT*> {
			static constexpr VARTYPE vt()  { return VT_R4 | VT_BYREF; }
			static FLOAT value(const VARIANT* v) { return *(v->pfltVal); }
		};

		template<>
		struct var_traits<DOUBLE> {
			static constexpr VARTYPE vt()   { return VT_R8; }
			static DOUBLE value(const VARIANT* v) { return v->dblVal; }
		};

		template<>
		struct var_traits<DOUBLE*> {
			static constexpr VARTYPE vt()   { return VT_R8 | VT_BYREF; }
			static DOUBLE value(const VARIANT* v) { return *(v->pdblVal); }
		};

		template<>
		struct var_traits<IUnknown*> {
			static constexpr VARTYPE vt()		{ return VT_UNKNOWN; }
			static IUnknown* value(const VARIANT* v)  { return v->punkVal; }
		};

		template<>
		struct var_traits<IUnknown**> {
			static constexpr VARTYPE vt()		{ return VT_UNKNOWN | VT_BYREF; }
			static IUnknown* value(const VARIANT* v)	{ return *(v->ppunkVal); }
		};

		template<>
		struct var_traits<IDispatch*> {
			static constexpr VARTYPE vt()		{ return VT_DISPATCH; }
			static IDispatch* value(const VARIANT* v) { return v->pdispVal; }
		};

		template<>
		struct var_traits<IDispatch**> {
			static constexpr VARTYPE vt()		{ return VT_DISPATCH | VT_BYREF; }
			static IDispatch* value(const VARIANT* v) { return *(v->ppdispVal); }
		};

	} // end namespace details

	///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	class variant;

	class variant_view
	{
	public:
		explicit variant_view(VARIANT& v)
			: v_(v)
		{
		}

		explicit variant_view(VARIANT* v)
			: v_(*v)
		{
		}

		VARTYPE vt() const
		{
			return v_.vt;
		}

		bool is_ref()
		{
			return v_.vt & VT_BYREF;
		}

		void copyTo(variant& v) const;

		void copyTo(VARIANT* v) const
		{
			if (v_.ullVal == v->ullVal)
				return;

			::VariantClear(v);
			::VariantCopy(v, &v_);
		}

		void copyTo(VARIANT& v) const
		{
			if (v_.ullVal == v.ullVal)
				return;

			::VariantClear(&v);
			::VariantCopy(&v, &v_);
		}

		bool isType(VARTYPE VT) const
		{
			return (v_.vt == VT);
		}

		void clear()
		{
			::VariantClear(&v_);
		}

		std::string to_string(int cp = CP_UTF8) const;
		std::wstring to_wstring() const;

		template<class T>
		T& valueOf() const;

	private:
		VARIANT& v_;
	};

	class variant : public VARIANT
	{
	public:
		variant() { ::VariantInit(this); }
		~variant() { ::VariantClear(this); }

		variant(const variant& v2)
		{
			::VariantInit(this);
			::VariantCopy(this, &v2);
		}

		variant(variant&& v2)
		{
			::VariantInit(this);
			this->vt = v2.vt;
			this->ullVal = v2.ullVal;
			v2.pbVal = 0;
			v2.vt = VT_EMPTY;
		}

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
			this->vt = v2.vt;
			this->ullVal = v2.ullVal;
			v2.pbVal = 0;
			v2.vt = VT_EMPTY;
		}

		explicit variant(VARIANT*&& v2)
		{
			::VariantInit(this);
			::VariantCopy(this, v2);
			this->vt = v2->vt;
			this->ullVal = v2->ullVal;
			v2->pbVal = 0;
			v2->vt = VT_EMPTY;
		}


		explicit variant(const ole_char& str)
		{
			::VariantInit(this);
			vt = VT_BSTR;
			bstrVal = ::SysAllocString(str.data());
		}

		explicit variant(const char* c, int cp = CP_UTF8)
		{
			::VariantInit(this);
			vt = VT_BSTR;
			bstrVal = ::SysAllocString(MTL::to_wstring(c,(int)strlen(c), cp).c_str());
		}

		template<size_t N>
		explicit variant(const char c[N], int cp = CP_UTF8)
		{
			::VariantInit(this);
			vt = VT_BSTR;
			std::wstring ws = MTL::to_wstring(c, N-1, cp);
			bstrVal = ::SysAllocStringLen(ws.c_str(), ws.size());
		}

		template<size_t N>
		explicit variant(const wchar_t (&wc)[N])
		{
			::VariantInit(this);
			vt = VT_BSTR;
			bstrVal = ::SysAllocStringLen(wc,N-1);
		}

		explicit variant(long l)			 { ::VariantInit(this); vt = VT_I4;	  lVal = l; }
		explicit variant(float f)			 { ::VariantInit(this); vt = VT_R4;	  fltVal = f; }
		explicit variant(double d)			 { ::VariantInit(this); vt = VT_R8;	  dblVal = d; }
		explicit variant(VARIANT_BOOL b)	 { ::VariantInit(this); vt = VT_BOOL; boolVal = b; }
		explicit variant(bool b)			 { ::VariantInit(this); vt = VT_BOOL; boolVal = b ? VARIANT_TRUE : VARIANT_FALSE; }
		explicit variant(CY cy)				 { ::VariantInit(this); vt = VT_CY;	  cyVal = cy; }
		explicit variant(const b_copy& b)	 { ::VariantInit(this); vt = VT_BSTR; bstrVal = ::SysAllocStringLen(b.data(),::SysStringLen(b.data())); }
		explicit variant(b_move&& b)		 { ::VariantInit(this); vt = VT_BSTR; bstrVal = b.data(); b.str_ = 0; }
		explicit variant(const bstr& b)		 { ::VariantInit(this); vt = VT_BSTR; bstrVal = ::SysAllocStringLen(*b, b.len()); }
		explicit variant(bstr&& b)			 { ::VariantInit(this); vt = VT_BSTR; bstrVal = *b; b.bstr_ = 0; }

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

		variant& operator=(VARIANT* v2)
		{
			::VariantClear(this);
			vt = v2->vt | VT_BYREF;
			this->pvarVal = v2;
			return *this;
		}

		variant& operator=( variant&& v2)
		{
			::VariantClear(this);
			this->vt = v2.vt;
			this->ullVal = v2.ullVal;
			v2.vt = VT_EMPTY;
			v2.ullVal = 0;
			return *this;
		}

		variant& operator=(const bstr& b)
		{
			if (this->bstrVal == b.bstr_)
				return *this;

			::VariantClear(this);
			this->vt = VT_BSTR;
			b.copy_to(&bstrVal);
			return *this;
		}

		variant& operator=( bstr&& b)
		{
			if (this->bstrVal == b.bstr_)
				return *this;

			::VariantClear(this);
			this->vt = VT_BSTR;
			this->bstrVal = b.bstr_;
			b.bstr_ = 0;
			return *this;
		}


		variant& operator=(const b_copy& b)
		{
			if (this->bstrVal == b.data() )
				return *this;

			::VariantClear(this);
			this->vt = VT_BSTR;
			this->bstrVal = ::SysAllocStringLen(b.data(), ::SysStringLen(b.data()));
			return *this;
		}


		variant& operator=(b_move&& b)
		{
			if (this->bstrVal == b.data())
				return *this;

			::VariantClear(this);
			this->vt = VT_BSTR;
			this->bstrVal = b.data();
			b.str_ = 0;
			return *this;
		}

		variant& operator=(BSTR* b)
		{
			if (this->pbstrVal == b)
				return *this;

			::VariantClear(this);
			this->vt = VT_BSTR | VT_BYREF;
			this->pbstrVal = b;
			return *this;
		}

		variant& operator=(const ole_char& str)
		{
			::VariantClear(this);
			this->vt = VT_BSTR;
			this->bstrVal = ::SysAllocString(str.data());
			return *this;
		}

		variant& operator=(IDispatch* disp)
		{
			::VariantClear(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
			disp->AddRef();
			return *this;
		}

		variant& operator=(IDispatch** disp)
		{
			::VariantClear(this);
			this->vt = VT_DISPATCH;
			this->ppdispVal = disp;
			return *this;
		}

		variant& operator=(IUnknown* disp)
		{
			::VariantClear(this);
			this->vt = VT_UNKNOWN;
			this->punkVal = disp;
			disp->AddRef();
			return *this;
		}

		variant& operator=(IUnknown** unk)
		{
			::VariantClear(this);
			this->vt = VT_UNKNOWN;
			this->ppunkVal = unk;
			return *this;
		}

		variant& operator=(bool b)
		{
			::VariantClear(this);
			this->vt = VT_BOOL;
			this->boolVal = b ? VARIANT_TRUE : VARIANT_FALSE;
			return *this;
		}

		variant& operator=(char c)
		{
			::VariantClear(this);
			this->vt = VT_I1 ;
			this->cVal = c;
			return *this;
		}

		variant& operator=(char* c)
		{
			::VariantClear(this);
			this->vt = VT_I1 | VT_BYREF;
			this->pcVal = c;
			return *this;
		}

		variant& operator=(BYTE b)
		{
			::VariantClear(this);
			this->vt = VT_UI1;
			this->bVal = b;
			return *this;
		}

		variant& operator=(BYTE* b)
		{
			::VariantClear(this);
			this->vt = VT_UI1 | VT_BYREF;
			this->pbVal = b;
			return *this;
		}

		variant& operator=(short c)
		{
			::VariantClear(this);
			this->vt = VT_I2;
			this->iVal = c;
			return *this;
		}

		variant& operator=(short* c)
		{
			::VariantClear(this);
			this->vt = VT_I2 | VT_BYREF;
			this->piVal = c;
			return *this;
		}

		variant& operator=(USHORT b)
		{
			::VariantClear(this);
			this->vt = VT_UI2;
			this->uiVal = b;
			return *this;
		}

		variant& operator=(USHORT* b)
		{
			::VariantClear(this);
			this->vt = VT_UI2 | VT_BYREF;
			this->puiVal = b;
			return *this;
		}

		variant& operator=(long c)
		{
			::VariantClear(this);
			this->vt = VT_I4;
			this->lVal = c;
			return *this;
		}

		variant& operator=(long* c)
		{
			::VariantClear(this);
			this->vt = VT_I4 | VT_BYREF;
			this->plVal = c;
			return *this;
		}

		variant& operator=(ULONG b)
		{
			::VariantClear(this);
			this->vt = VT_UI4;
			this->ulVal = b;
			return *this;
		}

		variant& operator=(ULONG* b)
		{
			::VariantClear(this);
			this->vt = VT_UI4 | VT_BYREF;
			this->pulVal = b;
			return *this;
		}

		variant& operator=(long long c)
		{
			::VariantClear(this);
			this->vt = VT_I8;
			this->llVal = c;
			return *this;
		}

		variant& operator=(long long* c)
		{
			::VariantClear(this);
			this->vt = VT_I8 | VT_BYREF;
			this->pllVal = c;
			return *this;
		}

		variant& operator=(ULONGLONG b)
		{
			::VariantClear(this);
			this->vt = VT_UI8;
			this->ullVal = b;
			return *this;
		}

		variant& operator=(ULONGLONG* b)
		{
			::VariantClear(this);
			this->vt = VT_UI8 | VT_BYREF;
			this->pullVal = b;
			return *this;
		}


		variant& operator=(float b)
		{
			::VariantClear(this);
			this->vt = VT_R4;
			this->fltVal = b;
			return *this;
		}

		variant& operator=(float* b)
		{
			::VariantClear(this);
			this->vt = VT_R4 | VT_BYREF;
			this->pfltVal = b;
			return *this;
		}

		variant& operator=(double b)
		{
			::VariantClear(this);
			this->vt = VT_R8;
			this->dblVal = b;
			return *this;
		}

		variant& operator=(double* b)
		{
			::VariantClear(this);
			this->vt = VT_R8 | VT_BYREF;
			this->pdblVal = b;
			return *this;
		}

		bool is_ref()
		{
			return vt & VT_BYREF;
		}


		void copy_to(variant& v) const
		{
			::VariantClear(&v);
			::VariantCopy(&v, this);
		}

		void copy_to(VARIANT* v) const
		{
			::VariantClear(v);
			::VariantCopy(v, this);
		}

		void copy_to(VARIANT& v) const
		{
			::VariantClear(&v);
			::VariantCopy(&v, this);
		}

		bool is_type(VARTYPE VT) const
		{
			return (vt == VT);
		}

		void clear()
		{
			::VariantClear(this);
		}

		HRESULT changeType(VARTYPE VT)
		{
			return ::VariantChangeType(this, this, VARIANT_NOUSEROVERRIDE, VT);
		}

		std::string to_string(int cp = CP_UTF8) const
		{
			if (vt == VT_BSTR)
			{
				return MTL::to_string(bstrVal,::SysStringLen(bstrVal),cp);
			}
			if (vt == (VT_BSTR | VT_BYREF))
			{
				return MTL::to_string(*pbstrVal, ::SysStringLen(bstrVal), cp);
			}

			variant tmp(*this);
			tmp.changeType(VT_BSTR);
			return tmp.to_string(cp);
		}

		std::wstring to_wstring() const
		{
			if (vt == VT_BSTR)
			{
				return std::wstring(bstrVal, ::SysStringLen(bstrVal));
			}
			if (vt == (VT_BSTR | VT_BYREF))
			{
				return std::wstring(*pbstrVal, ::SysStringLen(bstrVal));
			}
			variant tmp(*this);
			tmp.changeType(VT_BSTR);
			return tmp.to_wstring();
		}

		template<class T>
		T value_of() const
		{
			if (vt & VT_BYREF)
			{
				if (vt & details::var_traits<T>::vt())
				{
					return details::var_traits<T*>::value(this);
				}

				variant tmp(*this);
				tmp.changeType(details::var_traits<T>::vt());
				return details::var_traits<T*>::value(&tmp);
			}

			if (vt == details::var_traits<T>::vt())
			{
				return details::var_traits<T>::value(this);
			}

			variant tmp(*this);
			tmp.changeType(details::var_traits<T>::vt());
			return details::var_traits<T>::value(&tmp);
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


	inline void variant_view::copyTo(variant& v) const
	{
		if (v_.ullVal == v.ullVal)
			return;

		::VariantClear(&v);
		::VariantCopy(&v, &v_);
	}


	inline std::string variant_view::to_string(int cp) const
	{
		if (v_.vt == VT_BSTR)
		{
			return MTL::to_string(v_.bstrVal, ::SysStringLen(v_.bstrVal), cp);
		}
		if (v_.vt == (VT_BSTR | VT_BYREF))
		{
			return MTL::to_string(*(v_.pbstrVal), ::SysStringLen(v_.bstrVal), cp);
		}

		variant tmp(v_);
		tmp.changeType(VT_BSTR);
		return tmp.to_string(cp);
	}

	inline std::wstring variant_view::to_wstring() const
	{
		if (v_.vt == VT_BSTR)
		{
			return std::wstring(v_.bstrVal, ::SysStringLen(v_.bstrVal));
		}
		if (v_.vt == (VT_BSTR | VT_BYREF))
		{
			return std::wstring(*(v_.pbstrVal), ::SysStringLen(v_.bstrVal));
		}

		variant tmp(v_);
		tmp.changeType(VT_BSTR);
		return tmp.to_wstring();
	}

	template<class T>
	T& variant_view::valueOf() const
	{
		if (v_.vt & VT_BYREF)
		{
			if (v_.vt & details::var_traits<T>::vt())
			{
				return details::var_traits<T*>::value(&v_);
			}

			variant tmp(v_);
			tmp.changeType(details::var_traits<T>::vt());
			return details::var_traits<T*>::value(&tmp);
		}

		if ( v_.vt == details::var_traits<T>::vt())
		{
			return details::var_traits<T>::value(&v_);
		}

		variant tmp(v_);
		tmp.changeType(details::var_traits<T>::vt());
		return details::var_traits<T>::value(&tmp);
	}


} // end namespace 

