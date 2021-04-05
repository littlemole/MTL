#pragma once

#include <mtl/uni.h>

#include <oleauto.h>

namespace MTL {

	///////////////////////////////////////////////////////////////////////
	// these BSTRs, oh boy
	///////////////////////////////////////////////////////////////////////

	class bstr;
	class variant;

	class bstr_view
	{
	public:
		bstr_view()
			: str_(0)
		{}

		explicit bstr_view(BSTR b)
			: str_(b)
		{}

		int len() const
		{
			if (!str_) return 0;
			return ::SysStringLen(str_);
		}

		BSTR data() const
		{
			return str_;
		}

		BSTR operator*() const
		{
			return str_;
		}

		std::wstring str() const
		{
			if (!str_) return std::wstring(L"");
			return std::wstring(str_, len());
		}

		HRESULT copy_to(BSTR* dest) const
		{
			if (!dest) return E_INVALIDARG;
			if (*dest)
			{
				::SysFreeString(*dest);
				*dest = 0;
			}
			if (str_)
			{
				*dest = ::SysAllocStringLen(str_,len());
			}
			return S_OK;
		}

		bstr as_bstr();

	private:
		BSTR str_;
	};

	class ole_char
	{
	public:
		ole_char()
		{}

		explicit ole_char(const wchar_t* s)
			: str_(s)
		{}

		const wchar_t* data() const
		{
			return str_;
		}

	private:
		const wchar_t* str_ = nullptr;
	};

	class b_copy
	{
		friend class bstr;
		friend class variant;
	public:

		explicit b_copy(const BSTR& s)
			: str_(s)
		{}

		const BSTR data() const
		{
			return str_;
		}

	private:
		const BSTR& str_;
	};

	class b_move
	{
		friend class bstr;
		friend class variant;
	public:

		explicit b_move(BSTR& s)
			: str_(s)
		{}

		BSTR data() const
		{
			return str_;
		}

	private:
		BSTR& str_;
	};


	class bstr
	{
		friend class variant;
	public:

		bstr()
			: bstr_(0)
		{}

		bstr(const bstr& b)
			: bstr_(::SysAllocStringLen(b.bstr_,b.len()))
		{
		}

		bstr& operator=(const bstr& b)
		{
			if (this == b.address_of())
				return *this;

			if (bstr_) 
				::SysFreeString(bstr_);

			bstr_ = 0;
	
			if(*b)
				bstr_ = ::SysAllocStringLen(b.bstr_,b.len());

			return *this;
		}

		bstr(bstr&& b) noexcept
		{
			bstr_ = b.bstr_;
			b.bstr_ = 0;
		}


		bstr& operator=(bstr&& b)
		{
			if (this == b.address_of())
				return *this;

			if (bstr_) ::SysFreeString(bstr_);

			bstr_ = b.bstr_;
			b.bstr_ = 0;
			return *this;
		}


		template<size_t N>
		explicit bstr(const wchar_t (&b)[N])
			:bstr_(::SysAllocStringLen(b, N))
		{
		}

		template<size_t N>
		bstr& operator=(const wchar_t b[N])
		{
			if (bstr_ == b)
				return *this;

			if (bstr_) ::SysFreeString(bstr_);
			bstr_ = 0;

			if(N)
				bstr_ = ::SysAllocStringLen(b,N);
			return *this;
		}

		explicit bstr(const ole_char& b)
			: bstr_(::SysAllocString(b.data()))
		{
		}

		explicit bstr(const ole_char& b, size_t len)
			: bstr_(::SysAllocStringLen(b.data(), (UINT)len))
		{
		}

		bstr& operator=(const ole_char& b)
		{
			if (bstr_ == b.data())
				return *this;

			if (bstr_) ::SysFreeString(bstr_);
			bstr_ = 0;

			if(b.data())
				bstr_ = ::SysAllocString(b.data());
			return *this;
		}

		explicit bstr(const b_copy& b)
			: bstr_(0)
		{
			if(b.data())
				bstr_ = ::SysAllocStringLen(b.data(), ::SysStringLen(b.data()));
		}

		bstr& operator=(const b_copy& b)
		{
			if (bstr_ == b.data())
				return *this;

			if (bstr_) ::SysFreeString(bstr_);
			bstr_ = 0;

			if(b.data())
				bstr_ = ::SysAllocStringLen(b.data(),::SysStringLen(b.data()));
			return *this;
		}

		explicit bstr(b_move&& b)
		{
			bstr_ = b.data();
			b.str_ = 0;
		}

		bstr& operator=(b_move&& str)
		{
			if (bstr_ == str.data())
				return *this;

			if (bstr_) ::SysFreeString(bstr_);
			bstr_ = 0;
			if (str.data())
			{
				bstr_ = str.data();
				str.str_ = 0;
			}
			return *this;
		}


		explicit bstr(const std::string& str, int cp = CP_UTF8)
		{
			std::wstring ws = to_wstring(str, cp);
			bstr_ = ::SysAllocStringLen(ws.c_str(),(UINT)ws.size());
		}

		bstr& operator=(const std::string& str)
		{
			if (bstr_) ::SysFreeString(bstr_);
			bstr_ = 0;

			if (!str.empty())
			{
				std::wstring ws = to_wstring(str);
				bstr_ = ::SysAllocStringLen(ws.c_str(),(UINT)ws.size());
			}
			return *this;
		}


		/* that could be called on a BSTR :-( 
		explicit bstr(const std::wstring& str)
			: bstr_(::SysAllocStringLen(str.c_str(),(UINT)str.size()))
		{
			std::cout << "bstr(const std::wstring& str)" << std::endl;
		}

		*/

		~bstr()
		{
			::SysFreeString(bstr_);
		}

		int len() const
		{
			if (!bstr_) return 0;
			return ::SysStringLen(bstr_);
		}

		BSTR data() const
		{
			return bstr_;
		}

		BSTR operator*() const
		{
			return bstr_;
		}

		std::wstring str() const
		{
			if (!bstr_) return std::wstring(L"");
			return std::wstring(bstr_, len());
		}

		HRESULT copy_to(BSTR* dest) const
		{
			if (!dest) return E_INVALIDARG;
			if (*dest)
			{
				::SysFreeString(*dest);
				*dest = 0;
			}
			if (bstr_)
			{
				*dest = ::SysAllocStringLen(bstr_, len());
			}
			return S_OK;
		}

		bstr clone() const
		{
			return bstr(*this);
		}


		std::string to_string(int cp = CP_UTF8) const
		{
			if (!bstr_)
				return "";
			return MTL::to_string(bstr_,len(),cp);
		}


		BSTR* operator& ()
		{
			if (bstr_)
			{
				::SysFreeString(bstr_);
				bstr_ = 0;
			}
			return &bstr_;
		}

		const bstr* address_of() const
		{
			return this;
		}

		bstr* address_of()
		{
			return this;
		}

	private:
		BSTR	bstr_;
	};


	inline bstr bstr_view::as_bstr()
	{
		return bstr(b_copy(str_));
	}

	inline bstr from_olechar(wchar_t* str, size_t len = -1)
	{
		if (len == -1)
		{
			len = wcslen(str);
		}
		return bstr(ole_char(str),len);
	}

	inline bstr from_olechar(const std::wstring& str)
	{
		return bstr(ole_char(str.c_str()), str.size());
	}

} // end namespace 

