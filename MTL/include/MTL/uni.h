#pragma once

#include <windows.h>
#include <wchar.h>
#include <memory.h> 

#include <vector>
#include <string>
#include <type_traits>

namespace MTL {

	template<class T>
	class Buffer
	{
	public:
		Buffer(size_t n)
			: buf(n + 1, 0)
		{
			buf[n] = 0;
		}

		Buffer(size_t n, int value)
			: buf(n + 1, 0)
		{
			::memset(&buf[0], value, (n) * sizeof(T));
			buf[n] = 0;
		}

		Buffer(const Buffer& rhs)
			: buf(rhs.buf)
		{}

		Buffer(Buffer&& rhs)
			: buf(std::move(rhs.buf))
		{
			rhs.buf.clear();
		}

		Buffer& operator=(const Buffer& rhs)
		{
			if (&rhs == this)
				return *this;
			buf = rhs;
			return *this;
		}

		Buffer& operator=(Buffer&& rhs)
		{
			if (&rhs == this)
				return *this;
			buf = std::move(rhs.buf);
			rhs.buf.clear();
			return *this;
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

	typedef Buffer<char> cbuff;
	typedef Buffer<wchar_t> wbuff;

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

}

