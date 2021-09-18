#pragma once

#include "mtl/sdk.h"

#include <wchar.h>
#include <memory.h> 
#include <type_traits>

namespace mtl {

	template<class T>
	class buffer
	{
	public:

		buffer()
		{}

		buffer(size_t n)
			: buf(n + 1, 0)
		{
			buf[n] = 0;
		}

		buffer(size_t n, int value)
			: buf(n + 1, 0)
		{
			::memset(&buf[0], value, (n) * sizeof(T));
			buf[n] = 0;
		}

		buffer(const buffer& rhs)
			: buf(rhs.buf)
		{}

		buffer(buffer&& rhs)
			: buf(std::move(rhs.buf))
		{
			rhs.buf.clear();
		}

		buffer& operator=(const buffer& rhs)
		{
			if (&rhs == this)
				return *this;
			buf = rhs;
			return *this;
		}

		buffer& operator=(buffer&& rhs)
		{
			if (&rhs == this)
				return *this;
			buf = std::move(rhs.buf);
			rhs.buf.clear();
			return *this;
		}

		void alloc(size_t n)
		{
			buf = std::vector<T>(n + 1);
			buf[n] = 0;
		}

		T* operator&()
		{
			return &buf[0];
		}
		
		operator const T* () const
		{
			return &buf[0];
		}

		operator T* () 
		{
			return &buf[0];
		}
		

		size_t size() const
		{
			return buf.size() - 1;
		}

		std::basic_string<T> toString(size_t len = std::basic_string<T>::npos) const
		{
			if (len != std::basic_string<T>::npos)
			{
				return std::basic_string<T>(&buf[0], len);
			}
			return std::basic_string<T>(&buf[0]);
		}

	private:
		std::vector<T> buf;
	};

	typedef buffer<char> cbuff;
	typedef buffer<wchar_t> wbuff;

	inline std::string to_string(const wchar_t* str, int nchars = -1, long cp = CP_UTF8)
	{
		int len = ::WideCharToMultiByte(cp, 0, str, nchars, 0, 0, 0, 0);
		cbuff buf(len);
		
		int n = ::WideCharToMultiByte(cp, 0, str, nchars, buf, len, 0, 0);
		return buf.toString(len);
	}

	inline std::string to_string(const std::wstring& in, long cp = CP_UTF8)
	{
		return to_string(in.c_str(), (int)in.size(), cp);
	}

	inline std::wstring to_wstring(const char* str, int nchars = -1, long cp = CP_UTF8)
	{
		int len = ::MultiByteToWideChar(cp, 0, str, nchars, 0, 0);
		wbuff buf(len);
		
		::MultiByteToWideChar(cp, 0, str, nchars, buf, len);
		return buf.toString(len);
	}

	inline std::wstring to_wstring(const std::string& in, long cp = CP_UTF8)
	{
		return to_wstring(in.c_str(), (int)in.size(), cp);
	}

	inline std::string trim(const std::string& in)
	{
		size_t first = in.find_first_not_of(" \r\n\t");
		size_t last = in.find_last_not_of(" \r\n\t");
		if ((first == std::string::npos) && (last == std::string::npos))
			return "";
		size_t from = (first != std::string::npos) ? first : 0;
		size_t to = (last != std::string::npos) ? last - from + 1 : in.size() - from;
		return in.substr(from, to);
	}


	inline std::wstring trim(const std::wstring& in)
	{
		size_t first = in.find_first_not_of(L" \r\n\t");
		size_t last = in.find_last_not_of(L" \r\n\t");
		if ((first == std::string::npos) && (last == std::string::npos))
			return L"";
		size_t from = (first != std::string::npos) ? first : 0;
		size_t to = (last != std::string::npos) ? last - from + 1 : in.size() - from;
		return in.substr(from, to);
	}
}

