#pragma once

#include <mtl/uni.h>

#include <oleauto.h>

namespace MTL {

	///////////////////////////////////////////////////////////////////////
	// these BSTRs, oh boy
	///////////////////////////////////////////////////////////////////////

	class bstr
	{
	public:
		bstr()
			: bstr_(0)
		{}

		bstr(const bstr& b)
			: bstr_(::SysAllocString(b.bstr_))
		{}

		bstr(bstr&& b)
			: bstr_(b.bstr_)
		{
			b.bstr_ = 0;
		}

		explicit bstr(BSTR b)
			: bstr_(::SysAllocString(b))
		{}

		explicit bstr(BSTR&& b)
			: bstr_(b)
		{
			b = 0;
		}

		explicit bstr(const std::string& str)
		{
			bstr_ = ::SysAllocString(MTL::to_wstring(str).c_str());
		}

		explicit bstr(const std::wstring& str)
			: bstr_(::SysAllocString(str.c_str()))
		{}

		~bstr()
		{
			::SysFreeString(bstr_);
		}

		int len() const
		{
			return ::SysStringLen(bstr_);
		}

		std::string to_string(int cp = CP_UTF8) const
		{
			if (!bstr_)
				return "";
			return MTL::to_string(bstr_,len(),cp);
		}

		std::wstring to_wstring() const
		{
			if (!bstr_)
				return L"";
			return std::wstring(bstr_,len());
		}

		bstr& operator=(const bstr& b)
		{
			if (bstr_) ::SysFreeString(bstr_);
			bstr_ = ::SysAllocString(b.bstr_);
			return *this;
		}

		bstr& operator=(bstr&& b)
		{
			if (bstr_) ::SysFreeString(bstr_);
			bstr_ = b.bstr_;
			b.bstr_ = 0;
			return *this;
		}

		bstr& operator=(const BSTR& b)
		{
			if (bstr_) ::SysFreeString(bstr_);
			bstr_ = ::SysAllocString(b);
			return *this;
		}

		bstr& operator=(const std::string& str)
		{
			if (bstr_) ::SysFreeString(bstr_);
			bstr_ = ::SysAllocString(MTL::to_wstring(str).c_str());
			return *this;
		}

		bstr& operator=(const std::wstring wstr)
		{
			if (bstr_) ::SysFreeString(bstr_);
			bstr_ = ::SysAllocString(wstr.c_str());
			return *this;
		}

		const BSTR* operator &() const
		{
			return &bstr_;
		}

		BSTR* operator &()
		{
			return &bstr_;
		}

		const BSTR operator *() const
		{
			return bstr_;
		}

		BSTR operator *() 
		{
			return bstr_;
		}

	private:
		BSTR	bstr_;
	};

} // end namespace 

