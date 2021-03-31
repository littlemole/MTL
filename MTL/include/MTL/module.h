#pragma once

#include "MTL/fwd.h"

namespace MTL {

	//////////////////////////////////////////////////////////////////////////////
	// globals hidden behing meyers singletons
	//////////////////////////////////////////////////////////////////////////////


	inline HINSTANCE& moduleHinstance()
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
		if (GetModuleFileName(hm, path, sizeof(path)) == 0)
		{
			int ret = GetLastError();
			fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
			exit(1);
		}
		return std::wstring(path);
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

