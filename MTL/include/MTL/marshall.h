#pragma once

#include <Objbase.h>
#include "MTL/punk.h"


namespace MTL {

	template<class I>
	class Proxy
	{
	public:

		Proxy(Proxy&& rhs)
			: riid_(rhs.riid_), stream_(rhs.stream_)
		{
			rhs.stream_ = nullptr;
		}

		Proxy(const Proxy& rhs)
			: riid_(rhs.riid_), stream_(rhs.stream_)
		{
		}

		explicit Proxy(I* i, DWORD flags = MSHLFLAGS_NORMAL, DWORD ctx = MSHCTX_LOCAL)
			: riid_(__uuidof(I))
		{
			HR hr = ::CoMarshalInterThreadInterfaceInStream(riid_, i, &stream_);
		}

		explicit Proxy( punk<I>& i, DWORD flags = MSHLFLAGS_NORMAL, DWORD ctx = MSHCTX_LOCAL)
			: riid_(__uuidof(I))
		{
			HR hr = ::CoMarshalInterThreadInterfaceInStream(riid_, *i, &stream_);
		}

		void unwrap(I** i)
		{
			HR hr = ::CoGetInterfaceAndReleaseStream(stream_, riid_, (void**)i);
		}

		punk<I> unwrap()
		{
			punk<I> p;
			HR hr = ::CoGetInterfaceAndReleaseStream(stream_, riid_, (void**)&p);
			return p;
		}

		punk<I> operator*()
		{
			return unwrap();
		}

	private:

		Proxy<I>& operator=(const Proxy& rhs) = delete;
		Proxy<I>& operator=(Proxy&& rhs) = delete;

		REFIID    riid_;
		IStream* stream_ = nullptr;
	};

}
