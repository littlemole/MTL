#pragma once

#include "mtl/punk.h"
#include "mtl/win32/mem.h"
#include <Objbase.h>


namespace mtl {



	template<class I>
	class proxy
	{
	public:

		proxy(proxy&& rhs)
			:  stream_(rhs.stream_),cookie_(rhs.cookie_)
		{
			rhs.stream_ = nullptr;
			rhs.cookie_ = 0;
		}

		proxy(const proxy& rhs)
			: stream_(rhs.stream_), cookie_(rhs.cookie_)
		{
		}

		explicit proxy(I* i, DWORD flags = MSHLFLAGS_NORMAL, DWORD ctx = MSHCTX_LOCAL)
		{
			HR hr = ::CoMarshalInterThreadInterfaceInStream(__uuidof(I), i, &stream_);
		}

		explicit proxy( punk<I>& i, DWORD flags = MSHLFLAGS_NORMAL, DWORD ctx = MSHCTX_LOCAL)
		{
			HR hr = ::CoMarshalInterThreadInterfaceInStream(__uuidof(I), *i, &stream_);
		}

		explicit proxy(DWORD cookie)
			: cookie_(cookie)
		{}

		void unwrap(I** i);
		punk<I> unwrap();

		void revoke();

		punk<I> operator*()
		{
			return unwrap();
		}

		DWORD cookie()
		{
			return cookie_;
		}

	private:

		proxy<I>& operator=(const proxy& rhs) = delete;
		proxy<I>& operator=(proxy&& rhs) = delete;

		IStream* stream_ = nullptr;
		DWORD cookie_ = 0;
	};

	class git
	{
	public:
		git()
		{
			HRESULT hr = git_.create_object(CLSID_StdGlobalInterfaceTable);
		}

		~git()
		{

		}

		template<class I>
		proxy<I> proxy(I* p)
		{
			DWORD cookie = registerInterface(p);
			return proxy<I>(cookie);
		}


		template<class T>
		DWORD register_interface(T* t)
		{
			if (!git_)
				return E_FAIL;

			punk<IUnknown> pnk(t);
			if (!pnk)
				return E_NOINTERFACE;

			DWORD cookie = 0;
			HR hr = git_->RegisterInterfaceInGlobal(pnk, __uuidof(T), &cookie);
			return cookie;
		}

		HRESULT revoke_interface(DWORD cookie)
		{
			if (!git_)
				return E_FAIL;

			return git_->RevokeInterfaceFromGlobal(cookie);
		}

		template<class I>
		HRESULT revoke_interface( mtl::proxy<I>& p)
		{
			if (!git_)
				return E_FAIL;

			if(!p.cookie())
				return E_FAIL;

			return git_->RevokeInterfaceFromGlobal(p.cookie());
		}

		template<class T>
		HRESULT get_interface(DWORD cookie, T** t)
		{
			if (!git_)
				return E_FAIL;

			return git_->GetInterfaceFromGlobal(cookie, __uuidof(T), (void**)t);
		}

		template<class T>
		punk<T> operator[](DWORD cookie)
		{
			if (!git_)
				return E_FAIL;

			punk<T> t;
			HR hr = git_->GetInterfaceFromGlobal(cookie, __uuidof(T), (void**)&t);
			return t;
		}

	private:

		punk<IGlobalInterfaceTable> git_;
	};

	template<class I>
	void proxy<I>::unwrap(I** i)
	{
		if (cookie_)
		{
			git gt;
			HR hr = gt.get_interface<I>(cookie_, i);
			return;
		}
		HR hr = ::CoGetInterfaceAndReleaseStream(stream_, __uuidof(I), (void**)i);
	}

	template<class I>
	punk<I> proxy<I>::unwrap()
	{
		punk<I> p;
		unwrap(&p);
		return p;
	}

	template<class I>
	void proxy<I>::revoke()
	{
		if (!cookie_)
		{
			return;
		}
		git gt;
		gt.revoke_interface(*this);
		cookie_ = 0;
	}

	class rot
	{
	public:
		
		static DWORD register_object( const std::wstring& id, IUnknown* unk, DWORD flags = ROTFLAGS_REGISTRATIONKEEPSALIVE)
		{
			mtl::punk<IRunningObjectTable> rot;
			HRESULT hr = ::GetRunningObjectTable(0, &rot);
			if (hr != S_OK)
			{
				return 0;
			}

			mtl::punk<IMoniker> moniker;
			::CreateItemMoniker(L"!", id.c_str(), &moniker);
			if (!moniker) return 0;

			DWORD cookie;
			hr = rot->Register( flags, unk, *moniker, &cookie);
			if (hr != S_OK)
			{
				return 0;
			}
			return cookie;
		}

		static DWORD register_object(const CLSID& clsid, const std::wstring& id, IUnknown* unk, DWORD flags = ROTFLAGS_REGISTRATIONKEEPSALIVE)
		{
			mtl::punk<IRunningObjectTable> rot;
			HRESULT hr = ::GetRunningObjectTable(0, &rot);
			if (hr != S_OK)
			{
				return 0;
			}

			mtl::punk<IMoniker> classMoniker;
			::CreateClassMoniker(clsid, &classMoniker);
			if (!classMoniker) return 0;

			mtl::punk<IMoniker> itemMoniker;
			::CreateItemMoniker(L"!", id.c_str(), &itemMoniker);
			if (!itemMoniker) return 0;

			mtl::punk<IMoniker> moniker;
			classMoniker->ComposeWith(*itemMoniker, FALSE, &moniker);
			if (!moniker) return 0;

			DWORD cookie;
			hr = rot->Register(flags, unk, *moniker, &cookie);
			if (hr != S_OK)
			{
				return 0;
			}
			return cookie;
		}

		static mtl::punk<IUnknown> getObject(const std::wstring& id)
		{
			mtl::punk<IUnknown> result;
			mtl::punk<IRunningObjectTable> rot;
			HRESULT hr = ::GetRunningObjectTable(0, &rot);
			if (hr != S_OK)
			{
				return result;
			}

			mtl::punk<IMoniker> moniker;
			::CreateItemMoniker(L"!", id.c_str(), &moniker);
			if (hr != S_OK)
			{
				return result;
			}

			rot->GetObjectW(*moniker, &result);
			if (hr != S_OK)
			{
				return result;
			}
			return result;
		}


		static mtl::punk<IUnknown> getObject(const CLSID& clsid, const std::wstring& id)
		{
			mtl::punk<IUnknown> result;
			mtl::punk<IRunningObjectTable> rot;
			HRESULT hr = ::GetRunningObjectTable(0, &rot);
			if (hr != S_OK)
			{
				return result;
			}

			mtl::punk<IMoniker> classMoniker;
			::CreateClassMoniker(clsid, &classMoniker);
			if (!classMoniker) return result;

			mtl::punk<IMoniker> itemMoniker;
			::CreateItemMoniker(L"!", id.c_str(), &itemMoniker);
			if (!itemMoniker) return result;

			mtl::punk<IMoniker> moniker;
			classMoniker->ComposeWith(*itemMoniker, FALSE, &moniker);
			if (!moniker) return result;

			hr = rot->GetObjectW(*moniker, &result);
			if (hr != S_OK)
			{
				return result;
			}
			return result;
		}

		template<class T>
		static mtl::punk<T> object(const std::wstring& id)
		{
			mtl::punk<IUnknown> obj = getObject(id);
			if (!obj) return obj;
			mtl::punk<T> result(obj);
			return result;
		}


		template<class T>
		static mtl::punk<T> object(const CLSID& clsid, const std::wstring& id)
		{
			mtl::punk<IUnknown> obj = getObject(clsid,id);
			if (!obj) return obj;
			mtl::punk<T> result(obj);
			return result;
		}

		static void revoke_object(DWORD cookie)
		{
			if (!cookie) return;

			mtl::punk<IRunningObjectTable> rot;
			HRESULT hr = ::GetRunningObjectTable(0, &rot);
			if (hr != S_OK)
			{
				return;
			}

			rot->Revoke(cookie);
		}

		std::vector<std::wstring> enumerate()
		{
			std::vector<std::wstring> result;

			mtl::punk<IRunningObjectTable> rot;
			HRESULT hr = ::GetRunningObjectTable(0, &rot);
			if (hr != S_OK)
			{
				return result;
			}

			mtl::punk<IEnumMoniker> enumMoniker;
			hr = rot->EnumRunning(&enumMoniker);
			if (hr != S_OK)
			{
				return result;
			}

			unsigned long fetched = 1;

			while (true)
			{
				mtl::punk<IMoniker> moniker;
				hr = enumMoniker->Next(1, &moniker, &fetched);
				if (hr != S_OK) break;

				mtl::punk<IBindCtx> ctx;
				::CreateBindCtx(0, &ctx);
				mtl::co_str str;
				hr = moniker->GetDisplayName( *ctx, NULL, &str);
				if (hr == S_OK)
				{
					result.push_back(str.str());
				}
			}

			return result;
		}

		std::vector<std::wstring> enumerate(const CLSID& clsid)
		{
			std::vector<std::wstring> result;
			std::vector<std::wstring> running = enumerate();
			
			std::wostringstream woss;
			std::wstring guid = mtl::guid_to_string(clsid);
			guid = guid.substr(1, guid.size() - 2);
			woss << L"clsid:" << guid << L":";
			std::wstring prefix = woss.str();

			for (auto& displayname : running)
			{
				std::wstring n = displayname.substr(0, prefix.size());
				if (_wcsnicmp(prefix.data(), n.data(), prefix.size()) == 0)
				{
					result.push_back(displayname.substr(prefix.size() + 1));
				}
			}
			return result;
		}
	};

	template<class T>
	class rotten
	{
	public:

		DWORD flags = ROTFLAGS_REGISTRATIONKEEPSALIVE;

		rotten()
		{
			uid = new_guid();
		}

		rotten(const std::wstring& id)
			: uid(id)
		{}

		rotten(const CLSID& c, const std::wstring& id)
			: uid(id), clsid(c)
		{}

		rotten(const CLSID& c)
			: clsid(c)
		{
			uid = new_guid();
		}

		rotten<T>& operator=(punk<T>& rhs)
		{
			obj = rhs;
			if (cookie)
			{
				rot::revokeObject(cookie);
				cookie = 0;
			}
			punk<IUnknown> unk(obj);
			if (::IsEqualCLSID(clsid, CLSID_NULL))
			{
				cookie = rot::registerObject(uid, *unk, flags);
			}
			else
			{
				cookie = rot::registerObject(clsid, uid, *unk, flags);
			}
			return *this;
		}

		~rotten()
		{
			rot::revokeObject(cookie);
		}

		std::wstring id()
		{
			return uid;
		}

		T* operator*()
		{
			return *obj;
		}

	private:
		std::wstring uid;
		mtl::punk<T> obj;
		DWORD cookie = 0;
		const CLSID& clsid = CLSID_NULL;
	};
}

