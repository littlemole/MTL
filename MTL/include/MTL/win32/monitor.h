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

		monitor(const monitor& rhs) = delete;
		monitor(monitor&& rhs)
		{
			onFileChanged = std::move(rhs.onFileChanged);
			path = std::move(rhs.path);
			done = rhs.done;

			overTheLap = rhs.overTheLap;
			rhs.overTheLap = nullptr;

			fni = rhs.fni;
			rhs.fni = nullptr;

			fileHandle = rhs.fileHandle;
			rhs.fileHandle = nullptr;
		}

		monitor& operator=(const monitor& rhs) = delete;

		monitor& operator=(monitor&& rhs)
		{
			if (this == &rhs)
			{
				return *this;
			}

			onFileChanged = std::move(rhs.onFileChanged);
			path = std::move(rhs.path);
			done = rhs.done;

			overTheLap = rhs.overTheLap;
			rhs.overTheLap = nullptr;

			fni = rhs.fni;
			rhs.fni = nullptr;

			fileHandle = rhs.fileHandle;
			rhs.fileHandle = nullptr;

			return *this;
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
				mon->onFileChanged.fire(mon->path + L"\\" + fn );
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


	class file_monitor
	{
	public:

		std::wstring watch(const std::wstring& filepath, std::function<void()> cb)
		{
			auto dir = mtl::path(filepath).parent_dir();
			if (dirMons_.count(dir) == 0)
			{
				dirMons_[dir].watch(dir);
			}
			
			std::wstring token = dirMons_[dir].onFileChanged([this,cb](std::wstring path) 
			{
				queue_[path] = cb;
				mtl::timer::set_timeout(500, [this]() 
				{
					auto q = queue_;
					queue_.clear();

					for (auto& it : q)
					{
						it.second();
					}
				});
			});
			return token;
		}

		void unwatch(const std::wstring& token, const std::wstring& filepath)
		{
			auto dir = mtl::path(filepath).parent_dir();

			if (dirMons_.count(dir))
			{
				size_t s = dirMons_[dir].onFileChanged.unregister(token);
				if (s == 0)
				{
					dirMons_[dir].stop();
					dirMons_.erase(dir);
				}
			}
		}

	private:

		std::map<std::wstring, std::function<void()>> queue_;
		std::map<std::wstring, monitor> dirMons_;
	};
}