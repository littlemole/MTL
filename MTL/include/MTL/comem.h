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
			data_ = (T*)::CoTaskMemAlloc(size_ * type_size);
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

inline int run_message_loop()
{
	MSG msg;

	while (true)
	{
		DWORD r = ::MsgWaitForMultipleObjectsEx(0, 0, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE | MWMO_ALERTABLE);
		if (r == WAIT_IO_COMPLETION)
		{
			continue;
		}

		if (!::GetMessage(&msg, 0, 0, 0))
		{
			break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}