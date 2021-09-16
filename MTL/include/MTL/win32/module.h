#pragma once

#include "MTL/fwd.h"
#include <sstream>

namespace MTL {

	//////////////////////////////////////////////////////////////////////////////
	// globals hidden behing meyers singletons
	//////////////////////////////////////////////////////////////////////////////


	inline HINSTANCE& module_instance()
	{
		static HINSTANCE inst = 0;
		return inst;
	}


	//////////////////////////////////////////////////////////////////////////////
	// helpers
	//////////////////////////////////////////////////////////////////////////////

	inline HMODULE getSelfModule()
	{
		HMODULE hm = NULL;
		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCWSTR)&getSelfModule, &hm) == 0)
		{
			int ret = GetLastError();
			fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
			exit(1);
		}
		return hm;
	}

	inline std::wstring pathToSelf()
	{
		HMODULE hm = getSelfModule();

		wchar_t path[MAX_PATH];
		if (GetModuleFileName(hm, path, MAX_PATH) == 0)
		{
			int ret = GetLastError();
			fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
			exit(1);
		}
		return std::wstring(path);
	}



	inline std::wstring pathToSelfDirectory(const wchar_t* postfix = 0)
	{
		std::wstring self = pathToSelf();

		std::size_t pos = self.find_last_of(L"\\");
		if (pos == std::wstring::npos)
		{
			return L"";
		}

		if (postfix)
		{
			std::wostringstream woss;
			woss << self.substr(0, pos) << postfix;
			return woss.str();
		}

		return self.substr(0, pos);
	}

	class ComModule
	{
	public:

		virtual ~ComModule()
		{}

		virtual long lock()
		{
			cnt_++;
			return cnt_;
		}

		virtual long unlock()
		{
			cnt_--;
			if (cnt_ == 0)
			{
				if (onUnLoad)
				{
					onUnLoad();
				}
			}
			return cnt_;
		}

		bool canUnload()
		{
			return cnt_ == 0;
		}

		std::function<void()> onUnLoad;

	protected:
		std::atomic<long> cnt_;
	};

	inline ComModule& comModule()
	{
		static ComModule com;
		return com;
	}


} // end namespace MTL

