#pragma once

#include <Objbase.h>
#include "MTL/punk.h"
#include "MTL/win32/mem.h"


namespace MTL {



	template<class I>
	class Proxy
	{
	public:

		Proxy(Proxy&& rhs)
			:  stream_(rhs.stream_),cookie_(rhs.cookie_)
		{
			rhs.stream_ = nullptr;
			rhs.cookie_ = 0;
		}

		Proxy(const Proxy& rhs)
			: stream_(rhs.stream_), cookie_(rhs.cookie_)
		{
		}

		explicit Proxy(I* i, DWORD flags = MSHLFLAGS_NORMAL, DWORD ctx = MSHCTX_LOCAL)
		{
			HR hr = ::CoMarshalInterThreadInterfaceInStream(__uuidof(I), i, &stream_);
		}

		explicit Proxy( punk<I>& i, DWORD flags = MSHLFLAGS_NORMAL, DWORD ctx = MSHCTX_LOCAL)
		{
			HR hr = ::CoMarshalInterThreadInterfaceInStream(__uuidof(I), *i, &stream_);
		}

		explicit Proxy(DWORD cookie)
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

		Proxy<I>& operator=(const Proxy& rhs) = delete;
		Proxy<I>& operator=(Proxy&& rhs) = delete;

		IStream* stream_ = nullptr;
		DWORD cookie_ = 0;
	};

	class GIT
	{
	public:
		GIT()
		{
			HRESULT hr = git_.createObject(CLSID_StdGlobalInterfaceTable);
		}

		~GIT()
		{

		}

		template<class I>
		Proxy<I> proxy(I* p)
		{
			DWORD cookie = registerInterface(p);
			return Proxy<I>(cookie);
		}


		template<class T>
		DWORD registerInterface(T* t)
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

		HRESULT revokeInterface(DWORD cookie)
		{
			if (!git_)
				return E_FAIL;

			return git_->RevokeInterfaceFromGlobal(cookie);
		}

		template<class I>
		HRESULT revokeInterface(Proxy<I>& p)
		{
			if (!git_)
				return E_FAIL;

			if(!p.cookie())
				return E_FAIL;

			return git_->RevokeInterfaceFromGlobal(p.cookie());
		}

		template<class T>
		HRESULT getInterface(DWORD cookie, T** t)
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
	void Proxy<I>::unwrap(I** i)
	{
		if (cookie_)
		{
			GIT git;
			HR hr = git.getInterface<I>(cookie_, i);
			return;
		}
		HR hr = ::CoGetInterfaceAndReleaseStream(stream_, __uuidof(I), (void**)i);
	}

	template<class I>
	punk<I> Proxy<I>::unwrap()
	{
		punk<I> p;
		unwrap(&p);
		return p;
	}

	template<class I>
	void Proxy<I>::revoke()
	{
		if (!cookie_)
		{
			return;
		}
		GIT git;
		git.revokeInterface(*this);
		cookie_ = 0;
	}

	class ROT
	{
	public:
		
		static DWORD registerObject( const std::wstring& id, IUnknown* unk, DWORD flags = ROTFLAGS_REGISTRATIONKEEPSALIVE)
		{
			MTL::punk<IRunningObjectTable> rot;
			HRESULT hr = ::GetRunningObjectTable(0, &rot);
			if (hr != S_OK)
			{
				return 0;
			}

			MTL::punk<IMoniker> moniker;
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

		static DWORD registerObject(const CLSID& clsid, const std::wstring& id, IUnknown* unk, DWORD flags = ROTFLAGS_REGISTRATIONKEEPSALIVE)
		{
			MTL::punk<IRunningObjectTable> rot;
			HRESULT hr = ::GetRunningObjectTable(0, &rot);
			if (hr != S_OK)
			{
				return 0;
			}

			MTL::punk<IMoniker> classMoniker;
			::CreateClassMoniker(clsid, &classMoniker);
			if (!classMoniker) return 0;

			MTL::punk<IMoniker> itemMoniker;
			::CreateItemMoniker(L"!", id.c_str(), &itemMoniker);
			if (!itemMoniker) return 0;

			MTL::punk<IMoniker> moniker;
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

		static MTL::punk<IUnknown> getObject(const std::wstring& id)
		{
			MTL::punk<IUnknown> result;
			MTL::punk<IRunningObjectTable> rot;
			HRESULT hr = ::GetRunningObjectTable(0, &rot);
			if (hr != S_OK)
			{
				return result;
			}

			MTL::punk<IMoniker> moniker;
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


		static MTL::punk<IUnknown> getObject(const CLSID& clsid, const std::wstring& id)
		{
			MTL::punk<IUnknown> result;
			MTL::punk<IRunningObjectTable> rot;
			HRESULT hr = ::GetRunningObjectTable(0, &rot);
			if (hr != S_OK)
			{
				return result;
			}

			MTL::punk<IMoniker> classMoniker;
			::CreateClassMoniker(clsid, &classMoniker);
			if (!classMoniker) return result;

			MTL::punk<IMoniker> itemMoniker;
			::CreateItemMoniker(L"!", id.c_str(), &itemMoniker);
			if (!itemMoniker) return result;

			MTL::punk<IMoniker> moniker;
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
		static MTL::punk<T> object(const std::wstring& id)
		{
			MTL::punk<IUnknown> obj = getObject(id);
			if (!obj) return obj;
			MTL::punk<T> result(obj);
			return result;
		}


		template<class T>
		static MTL::punk<T> object(const CLSID& clsid, const std::wstring& id)
		{
			MTL::punk<IUnknown> obj = getObject(clsid,id);
			if (!obj) return obj;
			MTL::punk<T> result(obj);
			return result;
		}

		static void revokeObject(DWORD cookie)
		{
			if (!cookie) return;

			MTL::punk<IRunningObjectTable> rot;
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

			MTL::punk<IRunningObjectTable> rot;
			HRESULT hr = ::GetRunningObjectTable(0, &rot);
			if (hr != S_OK)
			{
				return result;
			}

			MTL::punk<IEnumMoniker> enumMoniker;
			hr = rot->EnumRunning(&enumMoniker);
			if (hr != S_OK)
			{
				return result;
			}

			unsigned long fetched = 1;

			while (true)
			{
				MTL::punk<IMoniker> moniker;
				hr = enumMoniker->Next(1, &moniker, &fetched);
				if (hr != S_OK) break;

				MTL::punk<IBindCtx> ctx;
				::CreateBindCtx(0, &ctx);
				MTL::co_str str;
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
			std::wstring guid = MTL::guid_to_string(clsid);
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
	class Rotten
	{
	public:

		DWORD flags = ROTFLAGS_REGISTRATIONKEEPSALIVE;

		Rotten()
		{
			uid = MTL::new_guid();
		}

		Rotten(const std::wstring& id)
			: uid(id)
		{}

		Rotten(const CLSID& c, const std::wstring& id)
			: uid(id), clsid(c)
		{}

		Rotten(const CLSID& c)
			: clsid(c)
		{
			uid = MTL::new_guid();
		}

		Rotten<T>& operator=(MTL::punk<T>& rhs)
		{
			obj = rhs;
			if (cookie)
			{
				ROT::revokeObject(cookie);
				cookie = 0;
			}
			MTL::punk<IUnknown> unk(obj);
			if (::IsEqualCLSID(clsid, CLSID_NULL))
			{
				cookie = ROT::registerObject(uid, *unk, flags);
			}
			else
			{
				cookie = ROT::registerObject(clsid, uid, *unk, flags);
			}
			return *this;
		}

		~Rotten()
		{
			ROT::revokeObject(cookie);
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
		MTL::punk<T> obj;
		DWORD cookie = 0;
		const CLSID& clsid = CLSID_NULL;
	};
}

