#pragma once

#include "MTL/sdk.h"

namespace MTL {


	class Monitor
	{
	public:

		MTL::Event<void(std::wstring)> onFileChanged;

		std::wstring path;

		bool done = false;

		Monitor()
		{}

		Monitor(const std::wstring& p)
		{
			watch(p);
		}

		~Monitor()
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
				&Monitor::overlappedCompletionRoutine
			);
		}

	private:

		static void overlappedCompletionRoutine(
			DWORD dwErrorCode,
			DWORD dwNumberOfBytesTransfered,
			LPOVERLAPPED lpOverlapped
		)
		{
			Monitor* monitor = (Monitor*)(lpOverlapped->hEvent);
			if (monitor->done)
			{
				delete lpOverlapped;
				free(monitor->fni);
				::CloseHandle(monitor->fileHandle);
				return;
			}

			FILE_NOTIFY_INFORMATION* fni = monitor->fni;

			std::wstring fn(fni->FileName, fni->FileNameLength / sizeof(wchar_t));
			if (!fn.empty())
			{
				monitor->onFileChanged.fire(fn);
			}

			DWORD ret = 0;
			::ZeroMemory(lpOverlapped, sizeof(OVERLAPPED));
			lpOverlapped->hEvent = (HANDLE)monitor;

			fni->FileNameLength = MAX_PATH * 2;
			fni->Action = 0;
			fni->NextEntryOffset = 0;
			fni->FileName[0] = 0;


			::ReadDirectoryChangesW(
				monitor->fileHandle,
				fni,
				sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH * 2,
				FALSE,
				FILE_NOTIFY_CHANGE_LAST_WRITE,
				&ret,
				lpOverlapped,
				&Monitor::overlappedCompletionRoutine
			);

		}

	private:
		OVERLAPPED* overTheLap = nullptr;
		FILE_NOTIFY_INFORMATION* fni = nullptr;;
		HANDLE fileHandle = nullptr;
	};

}