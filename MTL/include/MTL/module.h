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

	inline std::wstring pathToSelfDirectory()
	{
		std::wstring self = pathToSelf();

		std::size_t pos = self.find_last_of(L"\\");
		if (pos == std::wstring::npos)
		{
			return L"";
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

	/*
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
	*/
} // end namespace MTL

