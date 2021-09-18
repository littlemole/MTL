#pragma once

#include "mtl/fwd.h"
#include <sstream>

namespace mtl {

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

	inline HMODULE get_self_module()
	{
		HMODULE hm = NULL;
		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCWSTR)&get_self_module, &hm) == 0)
		{
			int ret = GetLastError();
			fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
			exit(1);
		}
		return hm;
	}

	inline std::wstring path_to_self()
	{
		HMODULE hm = get_self_module();

		wchar_t path[MAX_PATH];
		if (GetModuleFileName(hm, path, MAX_PATH) == 0)
		{
			int ret = GetLastError();
			fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
			exit(1);
		}
		return std::wstring(path);
	}



	inline std::wstring path_to_self_directory(const wchar_t* postfix = 0)
	{
		std::wstring self = path_to_self();

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

	class com_module
	{
	public:

		virtual ~com_module()
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

		bool can_unload()
		{
			return cnt_ == 0;
		}

		std::function<void()> onUnLoad;

	protected:
		std::atomic<long> cnt_;
	};

	inline com_module& the_com_module()
	{
		static com_module com;
		return com;
	}


} // end namespace MTL

