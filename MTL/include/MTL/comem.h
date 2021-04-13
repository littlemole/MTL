#pragma once

#include <Objbase.h>

template<class T>
class co_mem
{
public:
	using type = T;
	using type_size = sizeof(T);

	co_mem()
		: data_(nullptr), size_(0)
	{}

	explicit co_mem(size_t nElements)
		: data_(nullptr), size_(nElements)
	{
		if(size_)
			data_ = (T*)::CoTaskMemAlloc(size_ * type_size);
	}

	co_mem(const co_mem<T>&& rhs)
		: data_(nullptr), size_(rhs.size_)
	{
        if (size_)
        {
            data_ = (T*)::CoTaskMemAlloc(size_ * type_size);
            memcpy(data, rhs.data_, size_ * type_size);
        }
	}

	co_mem(co_mem<T>&& rhs)
		: data_(rhs.data_), size_(rhs.size_)
	{
		rhs.size_ = 0;
		rhs.data_ = 0;
	}

	~co_mem()
	{
		if (data_)
		{
			::CoTaskMemFree(data_);
		}
	}

	T& operator[](size_t index)
	{
		return data_[index];
	}

	T* data()
	{
		return data_;
	}

	T* operator*()
	{
		return data_;
	}

	T** operator&()
	{
		return &data_;
	}

	co_mem<t>* address_of()
	{
		return this;
	}

	size_t size() const
	{
		return size_;
	}

private:

	co_mem<T>& operator(const co_mem<T>& rhs) = delete;
	co_mem<T>& operator(co_mem<T>&& rhs) = delete;
	T* data_;
	size_t size_;
};

// null terminated wide string allocated with the com memory allocator
// to pass around OLECHARS

class co_str
{
public:
    co_str()
    {}

    co_str(const co_str& rhs)
    {
        int len = rhs.size() + 1;
        str_ = (wchar_t*)::CoTaskMemAlloc(len * sizeof(wchar_t));
        memcpy(str_, rhs.str_, len);
    }

    co_str(co_str&& rhs)
        : str_(rhs.str_)
    {
        rhs.str_ = nullptr;
    }

    explicit co_str(const wchar_t* s)
    {
        int len = _wcslen(s) + 1;
        str_ = (wchar_t*)::CoTaskMemAlloc( len * sizeof(wchar_t) );
        memcpy(str_, s, len);
    }

    explicit co_str(const std::wstring& s)
    {
        int len = s.size() + 1;
        str_ = (wchar_t*)::CoTaskMemAlloc(len * sizeof(wchar_t));
        memcpy(str_, s.c_str(), len);
    }

    ~co_str()
    {
        if (str_)
        {
            ::CoTaskMemFree(str_);
            str_ = 0;
        }
    }

    co_str& operator=(const co_str& rhs)
    {
        if (str_ == rhs.str_)
        {
            return *this;
        }

        if (str_)
        {
            ::CoTaskMemFree(str_);
            str_ = 0;
        }

        int len = rhs.size() + 1;
        str_ = (wchar_t*)::CoTaskMemAlloc(len * sizeof(wchar_t));
        memcpy(str_, rhs.str_, len);

        return *this;
    }

    co_str& operator=(co_str&& rhs)
    {
        if (str_ == rhs.str_)
        {
            return *this;
        }

        if (str_)
        {
            ::CoTaskMemFree(str_);
            str_ = 0;
        }

        str_ = rhs.str_;
        rhs.str_ = nullptr;

        return *this;
    }

    int size()
    {
        if (!str_) return 0;
        return _wcslen(str_);
    }

    std::wstring str()
    {
        if (!str_) return L"";
        return std::wstring(str_, size());
    }

    wchar_t* operator*()
    {
        return str_;
    }

    wchar_t** operator&()
    {
        if (str_)
        {
            ::CoTaskMemFree(str_);
            str_ = nullptr;
        }
        return &str_;
    }

private:
    wchar_t* str_ = nullptr;
};

class CoStr
{
public:

    CoStr()
    {}

    CoStr(wchar_t* s)
        : str_(s)
    {}

    CoStr(const CoStr& rhs)
    {
        if (rhs.str_)
        {
            int len = rhs.size();
            wchar_t* str_ = (wchar_t*) ::CoTaskMemAlloc(len * sizeof(wchar_t));
            memcpy(str_, rhs.str_, len);
        }
    }

    CoStr(CoStr&& rhs)
    {
        if (rhs.str_)
        {
            wchar_t* str_ = rhs.str_;
            rhs.str_ = 0;
        }
    }

    CoStr& operator=(const CoStr& rhs)
    {
        if (this == &rhs)
            return *this;

        if (str_)
        {
            ::CoTaskMemFree(str_);
            str_ = 0;
        }
        if (rhs.str_)
        {
            int len = rhs.size();
            wchar_t* str_ = (wchar_t*) ::CoTaskMemAlloc(len * sizeof(wchar_t));
            memcpy(str_, rhs.str_, len);
        }

        return *this;
    }

    CoStr& operator=(CoStr&& rhs)
    {
        if (this == &rhs)
            return *this;

        if (str_)
        {
            ::CoTaskMemFree(str_);
            str_ = 0;
        }
        if (rhs.str_)
        {
            str_ = rhs.str_;
            rhs.str_ = 0;
        }

        return *this;
    }

    CoStr& operator=(wchar_t* rhs)
    {

        if (str_)
        {
            ::CoTaskMemFree(str_);
            str_ = 0;
        }
        if (rhs)
        {
            str_ = rhs;
        }

        return *this;
    }
    size_t size() const
    {
        if (!str_) return 0;
        return wcslen(str_);
    }


    wchar_t* operator*()
    {
        return str_;
    }

    ~CoStr()
    {
        if (str_)
        {
            ::CoTaskMemFree(str_);
            str_ = 0;
        }
    }

    wchar_t* str_ = nullptr;
};
