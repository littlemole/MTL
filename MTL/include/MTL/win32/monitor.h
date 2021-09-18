#pragma once

#include "mtl/sdk.h"

namespace mtl {


	class monitor
	{
	public:

		event<void(std::wstring)> onFileChanged;

		std::wstring path;

		bool done = false;

		monitor()
		{}

		monitor(const std::wstring& p)
		{
			watch(p);
		}

		~monitor()
		{
			stop();
		}

		void stop()
		{
			done = true;

			::CancelIoEx(fileHandle, overTheLap);
		}

		void watch(const std::wstring& p)
		{
			path = p;

			overTheLap = new OVERLAPPED;
			::ZeroMemory(overTheLap, sizeof(OVERLAPPED));

			fni = (FILE_NOTIFY_INFORMATION*)malloc(sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH * 2);
			fni->FileNameLength = MAX_PATH * 2;
			fni->Action = 0;
			fni->NextEntryOffset = 0;
			fni->FileName[0] = 0;

			fileHandle = ::CreateFile(
				path.c_str(),
				FILE_LIST_DIRECTORY,
				FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
				NULL
			);

			overTheLap->hEvent = (HANDLE)this;

			DWORD ret = 0;
			::ReadDirectoryChangesW(
				fileHandle,
				fni,
				sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH * 2,
				FALSE,
				FILE_NOTIFY_CHANGE_LAST_WRITE,
				&ret,
				overTheLap,
				&monitor::overlappedCompletionRoutine
			);
		}

	private:

		static void overlappedCompletionRoutine(
			DWORD dwErrorCode,
			DWORD dwNumberOfBytesTransfered,
			LPOVERLAPPED lpOverlapped
		)
		{
			monitor* mon = (monitor*)(lpOverlapped->hEvent);
			if (mon->done)
			{
				delete lpOverlapped;
				free(mon->fni);
				::CloseHandle(mon->fileHandle);
				return;
			}

			FILE_NOTIFY_INFORMATION* fni = mon->fni;

			std::wstring fn(fni->FileName, fni->FileNameLength / sizeof(wchar_t));
			if (!fn.empty())
			{
				mon->onFileChanged.fire(fn);
			}

			DWORD ret = 0;
			::ZeroMemory(lpOverlapped, sizeof(OVERLAPPED));
			lpOverlapped->hEvent = (HANDLE)mon;

			fni->FileNameLength = MAX_PATH * 2;
			fni->Action = 0;
			fni->NextEntryOffset = 0;
			fni->FileName[0] = 0;


			::ReadDirectoryChangesW(
				mon->fileHandle,
				fni,
				sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH * 2,
				FALSE,
				FILE_NOTIFY_CHANGE_LAST_WRITE,
				&ret,
				lpOverlapped,
				&monitor::overlappedCompletionRoutine
			);

		}

	private:
		OVERLAPPED* overTheLap = nullptr;
		FILE_NOTIFY_INFORMATION* fni = nullptr;;
		HANDLE fileHandle = nullptr;
	};

}