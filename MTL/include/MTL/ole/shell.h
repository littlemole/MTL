#pragma once

#include <MTL/ole/dataobj.h>
#include "MTL/win/gdi.h"

namespace MTL {

	inline std::wstring appFilesPath(const std::wstring& dir)
	{
		wchar_t buff[MAX_PATH];
		::SHGetSpecialFolderPath(0, buff, CSIDL_APPDATA, TRUE);

		std::wstring path = buff;
		path += L"\\";
		path += dir;

		DWORD fa = GetFileAttributes(path.c_str());
		if (fa != FILE_ATTRIBUTE_DIRECTORY)
		{
			BOOL r = ::CreateDirectory(path.c_str(), NULL);
			// might "fail but succeed" thanks to Vista Virtualization!
			// if ( !::CreateDirectory(path.c_str(),NULL) )
			// // throw mol::X("could not create app dir"); 
		}
		return path;
	}

	namespace Shell {
		class Folder;
	}

	inline Shell::Folder desktop();

	namespace Shell {

		class Item
		{
			friend class Folder;
		public:
			Item(LPITEMIDLIST pidl, DWORD attributes)
				:pidl_(pidl), attributes_(attributes)
			{}

			~Item();

			bool isDir()
			{
				return (attributes_ & SFGAO_FOLDER) != 0;;
			}

			bool isFile()
			{
				return (attributes_ & SFGAO_STREAM) != 0;
			}

			bool isPartOfFileSystem()
			{
				return (attributes_ & SFGAO_FILESYSANCESTOR) != 0; //SFGAO_FILESYSTEM) != 0;
			}

			bool isBrowsable()
			{
				return (attributes_ & SFGAO_BROWSABLE) != 0;
			}

			//std::wstring displayName(Shell::Folder& parent, DWORD flags = SHGDN_FORPARSING);

			LPITEMIDLIST operator*() { return pidl_; }

			operator bool() 
			{
				return pidl_ != 0;
			}

		private:
			LPITEMIDLIST	pidl_;
			ULONG           attributes_;
		};

	}

	typedef std::shared_ptr<Shell::Item> Shit;

	namespace Shell {

		class Enumerator 
		{
		public:

			Enumerator()
			{}

			// while next
			Shit next(DWORD attributes = SFGAO_FOLDER | SFGAO_STREAM | SFGAO_FILESYSTEM | SFGAO_BROWSABLE | SFGAO_FILESYSANCESTOR);

			// restart enum
			void reset()
			{
				enumIdl_->Reset();
			}


			IEnumIDList** operator&()
			{
				return &enumIdl_;
			}

		private:
			punk<IEnumIDList>				enumIdl_;
		};

		class Folder
		{
			friend class Item;
		public:
			Folder()
			{
				::SHGetDesktopFolder(&folder);
			}

			Folder(LPITEMIDLIST pidl, Folder parent = desktop())
			{
				folder = parent.subFolder(pidl).folder;
			}

			Folder(IShellFolder* sf)
			{
				folder = sf;
			}

			Folder(const std::wstring& path, DWORD attributes = SFGAO_FOLDER | SFGAO_STREAM | SFGAO_FILESYSTEM | SFGAO_BROWSABLE | SFGAO_FILESYSANCESTOR)
			{
				//ULONG chEaten = 0;
				//LPITEMIDLIST pidl = 0;

				auto shit = parseDisplayName(path, attributes);
				auto desk = desktop();
				//HR hr = desk.folder->ParseDisplayName(0, 0, (wchar_t*)path.c_str(), &chEaten, &pidl, 0);
				HR hr = desk.folder->BindToObject(**shit, 0, IID_IShellFolder, (void**)&folder);
				//desk.release_pidl(pidl);
			}

			~Folder()
			{}

			std::wstring getDisplayName(DWORD flags = SHGDN_FORPARSING)
			{
				Shell::Folder desk = desktop();
				return getDisplayName(desk, flags);;
			}

			std::wstring getDisplayName(MTL::Shell::Item& shit, DWORD flags = SHGDN_FORPARSING)
			{
				return getDisplayName(*shit, flags);
			}

			std::wstring getDisplayName(Shell::Folder& parent, DWORD flags = SHGDN_FORPARSING)
			{
				punk<IPersistFolder2> pf2(folder);
				if (!pf2) return L"";

				LPITEMIDLIST pidl = 0;
				HR hr = pf2->GetCurFolder(&pidl);

				std::wstring result = parent.getDisplayName(pidl, flags);

				release_pidl(pidl);
				return result;
			}
			
			std::wstring getDisplayName(LPITEMIDLIST pidl, DWORD flags = SHGDN_FORPARSING)
			{
				if (S_OK == folder->GetDisplayNameOf(pidl, flags, &strret_))
				{
					wchar_t buf[MAX_PATH];
					StrRetToBuf(&strret_, pidl, buf, MAX_PATH);
					return std::wstring(buf);
				}
				return L"";
			}

			Shit shellItem()
			{
				punk<IPersistFolder2> pf2(folder);
				if (pf2)
				{
					LPITEMIDLIST pidl;
					if (S_OK == pf2->GetCurFolder(&pidl))
						return Shit(new Shell::Item(pidl, getAttributesOf(pidl)));
				}
				return Shit();
			}

			Shit getSpecialFolder(int csidl)
			{
				LPITEMIDLIST pidl = 0;
				::SHGetSpecialFolderLocation(0, csidl, &pidl);
				return Shit(new Shell::Item(pidl, getAttributesOf(pidl)));
			}

			Shit parseDisplayName(const std::wstring& path, DWORD attributes = SFGAO_FOLDER | SFGAO_STREAM | SFGAO_FILESYSTEM | SFGAO_BROWSABLE | SFGAO_FILESYSANCESTOR)
			{
				Shell::Folder	desk;
				LPITEMIDLIST pidl = 0;
				ULONG ulong = 0;
				// first param hwnd
				if (S_OK == desk.folder->ParseDisplayName(0, NULL, (LPOLESTR)(path.c_str()), &ulong, &pidl, &attributes))
				{
					Shell::Item* it = new Shell::Item(pidl, desk.getAttributesOf(pidl, attributes));
					return Shit(it);
				}
				return Shit();
			}

			// get Attributes of item
			ULONG getAttributesOf(LPITEMIDLIST pidl, ULONG attributes = SFGAO_FOLDER | SFGAO_STREAM | SFGAO_FILESYSTEM | SFGAO_BROWSABLE | SFGAO_FILESYSANCESTOR)
			{
				if (S_OK == folder->GetAttributesOf(1, (LPCITEMIDLIST*)&pidl, &attributes))
					return attributes;
				return 0;
			}

			// prepare enumeration
			Enumerator enumerate(HWND owner = NULL, SHCONTF flags = SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN)
			{
				Enumerator enumObj;
				HR hr = folder->EnumObjects(owner, flags, &enumObj);
				return enumObj;
			}

			// is valid
			operator bool() { return folder; }

			punk<IShellFolder>   folder;


			void release_pidl(LPITEMIDLIST pidl)
			{
				if (pidl)
				{
					::CoTaskMemFree(pidl);
				}
			}

			Shell::Folder subFolder(LPITEMIDLIST pidl)
			{
				IShellFolder* pFolder = nullptr;
				HR hr = folder->BindToObject(pidl, NULL, IID_IShellFolder, (LPVOID*)&pFolder);
				return Shell::Folder(pFolder);
			}

		protected:


			punk<IEnumIDList>				enumIdl_;
			punk<IShellFolder>				desk_;
			STRRET							strret_;
		};

		////////////////////////////////////////////////////////////////////////////////////

		inline Shit parseDisplayName(const std::wstring& path, DWORD attributes = SFGAO_FOLDER | SFGAO_STREAM | SFGAO_FILESYSTEM | SFGAO_BROWSABLE | SFGAO_FILESYSANCESTOR)
		{
			Shell::Folder	desk;
			LPITEMIDLIST pidl = 0;
			ULONG ulong = 0;
			// first param hwnd
			if (S_OK == desk.folder->ParseDisplayName(0, NULL, (LPOLESTR)(path.c_str()), &ulong, &pidl, &attributes))
			{
				Shell::Item* it = new Shell::Item(pidl, desk.getAttributesOf(pidl, attributes));
				return Shit(it);
			}
			return Shit();
		}

		namespace details {

			inline int  op(UINT  op, HWND hwnd, const std::wstring& from, const std::wstring& to, FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
			{
				SHFILEOPSTRUCT sfos;
				::ZeroMemory(&sfos, sizeof(sfos));

				wbuff f(from.size() + 2);
				wbuff t(to.size() + 2);

				memcpy(f, from.c_str(), (from.size() + 1) * sizeof(wchar_t));
				memcpy(t, to.c_str(), (to.size() + 1) * sizeof(wchar_t));

				f[from.size() + 1] = 0;
				t[to.size() + 1] = 0;

				sfos.hwnd = hwnd;
				sfos.wFunc = op;
				sfos.pFrom = f;
				sfos.pTo = t;
				sfos.fFlags = flags;
				sfos.lpszProgressTitle = L"";

				int  ret = SHFileOperation(&sfos);
				return ret;
			}


			inline int  multiOp(UINT  op, HWND hwnd, const std::vector<std::wstring>& from, const std::wstring& to, FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
			{
				SHFILEOPSTRUCT sfos;
				::ZeroMemory(&sfos, sizeof(sfos));

				size_t s = 0;
				for (size_t i = 0; i < from.size(); i++)
				{
					s += from[i].size() + 1;
				}

				wbuff f(s + 1);
				wbuff t(to.size() + 2);

				size_t c = 0;
				for (size_t i = 0; i < from.size(); i++)
				{
					memcpy(f + c, from[i].c_str(), (from[i].size() + 1) * sizeof(wchar_t));
					c += from[i].size() + 1;
				}
				memcpy(t, to.c_str(), (to.size() + 1) * sizeof(wchar_t));

				f[s] = 0;
				t[to.size() + 1] = 0;

				sfos.hwnd = hwnd;
				sfos.wFunc = op;
				sfos.pFrom = f;
				sfos.pTo = t;
				sfos.fFlags = flags;
				sfos.lpszProgressTitle = 0;

				int  ret = SHFileOperation(&sfos);
				return ret;
			}
		}


		inline int  Copy(
			HWND hwnd,
			const std::wstring& from,
			const std::wstring& to,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			return details::op(FO_COPY, hwnd, from, to, flags);
		}

		inline int  Copy(
			HWND hwnd,
			const std::vector<std::wstring>& from,
			const std::wstring& to,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			return details::multiOp(FO_COPY, hwnd, from, to, flags);
		}


		inline int  Remove(
			HWND hwnd,
			const std::wstring& from,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			std::wstring to(L"");
			return details::op(FO_DELETE, hwnd, from, to, flags);
		}

		inline int  Remove(
			HWND hwnd,
			const std::vector<std::wstring>& from,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			std::wstring to(L"");
			return details::multiOp(FO_DELETE, hwnd, from, to, flags);
		}

		inline int  Move(
			HWND hwnd,
			const std::wstring& from,
			const std::wstring& to,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			return details::op(FO_MOVE, hwnd, from, to, flags);
		}


		inline int  Move(
			HWND hwnd,
			const std::vector<std::wstring>& from,
			const std::wstring& to,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			return details::multiOp(FO_MOVE, hwnd, from, to, flags);
		}

		inline int  Rename(
			HWND hwnd,
			const std::wstring& from,
			const std::wstring& to,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			return details::op(FO_RENAME, hwnd, from, to, flags);
		}

		inline bool CreateDir(const std::wstring& dirname)
		{
			return ::CreateDirectoryW(Path(dirname).wpath().c_str(), 0) == TRUE;
		}

		//		SHFILEOPSTRUCT sfos_;

		inline BOOL Execute(const std::wstring& path, const std::wstring& verb = L"open", int nShow = SW_SHOWNORMAL, ULONG fMask = 0)
		{
			SHELLEXECUTEINFO sei;
			ZeroMemory(&sei, sizeof(sei));
			sei.cbSize = sizeof(sei);
			sei.lpVerb = verb.c_str();
			sei.lpFile = path.c_str();
			sei.nShow = nShow;
			sei.fMask = fMask;

			return ::ShellExecuteEx(&sei);
		}


		inline BOOL ExecuteArgs(
			const std::wstring& path,
			const std::wstring& args,
			const std::wstring& verb = L"open",
			int nShow = SW_SHOWNORMAL, ULONG fMask = SEE_MASK_FLAG_DDEWAIT | SEE_MASK_FLAG_NO_UI) //SEE_MASK_INVOKEIDLIST)
		{
			SHELLEXECUTEINFO sei;
			ZeroMemory(&sei, sizeof(sei));
			sei.cbSize = sizeof(sei);
			sei.lpVerb = verb.c_str();
			sei.lpParameters = args.c_str();
			sei.lpFile = path.c_str();
			sei.nShow = nShow;
			sei.fMask = fMask;

			return ::ShellExecuteEx(&sei);
		}


		//BOOL execute_shell_admin(const std::wstring& path, const std::wstring& args);

		////////////////////////////////////////////////////////////////////////////////////
		// execute a commandline w/o  cmd.exe
		////////////////////////////////////////////////////////////////////////////////////

		inline BOOL exec_cmdline(const std::wstring cl, bool showWindow = true);


	/*
	class UACPipe
	{
	public:

		UACPipe();
		~UACPipe();

		HANDLE create(const std::wstring& pipename);
		HANDLE open(const std::wstring& pipename);

		bool connect(int milisecs = 3000);
		void disconnect();
		void close();

		std::string read();
		bool write(const std::string& data);

		operator bool()
		{
			return pipe_ != INVALID_HANDLE_VALUE;
		}

	private:
		HANDLE pipe_;
		static const int BUFSIZE;
	};
	*/

	//std::wstring resolveShortcut(const std::wstring& link);
	//std::wstring resolveInternetShortcut(const std::wstring& link);


	}

	//! get the desktop singleton
	inline Shell::Folder desktop()
	{
		return Shell::Folder();
	}

	namespace Shell {

		inline HICON getStockIcon(SHSTOCKICONID id)
		{
			SHSTOCKICONINFO sii;
			::ZeroMemory(&sii, sizeof(sii));
			sii.cbSize = sizeof(sii);

			HRESULT hr = ::SHGetStockIconInfo(SIID_DELETE, SHGSI_ICON, &sii);
			if (hr != S_OK)
			{
				return 0;
			}
			return sii.hIcon;
		}

		inline HICON getFileIcon(const std::wstring& path)
		{
			static std::map<std::wstring, MTL::Icon> map;

			std::wstring ext = MTL::Path(path).ext();

			if (map.count(ext))
			{
				return *map[ext];
			}

			SHFILEINFO sfi;
			::ZeroMemory(&sfi, sizeof(sfi));
			
			bool b = ::SHGetFileInfo(path.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON);
			if (!b)
			{
				return 0;
			}
			map[ext] = sfi.hIcon;
			return sfi.hIcon;
		}

		inline Item::~Item()
		{
			desktop().release_pidl(pidl_);
			pidl_ = 0;
		}

		/*
		std::wstring Item::displayName(Shell::Folder& parent, DWORD flags)
		{
			return parent.getDisplayName(pidl_, flags);
		}
		*/
		inline Shit Enumerator::next(DWORD attributes )
		{
			ULONG fetched;
			LPITEMIDLIST pidl = 0;

			if (enumIdl_)
			{
				if (S_OK == enumIdl_->Next(1, &pidl, &fetched))
				{
					if (fetched == 1)
					{
						Shell::Item* it = new Shell::Item(pidl, desktop().getAttributesOf(pidl, attributes));
						return Shit(it);
					}
				}
			}
			return Shit();
		}

		class FolderView : public MTL::Window<FolderView>,
			public implements<stack_object<FolderView>(
				IShellBrowser, 
				IServiceProvider, 
				ICommDlgBrowser2, of<ICommDlgBrowser2, ICommDlgBrowser>)>//, IContextMenuSite)>
		{
		public:

			FolderView()
			{}

			~FolderView()
			{
				if (shellView_)
				{
					//if (layout_)
					//	layout_->remove(viewWnd_);

					HRESULT hr = shellView_->DestroyViewWindow();
					shellView_.release();
				}
			}

			// events

			Event<void(ULONG)> onChange;
			Event<void(std::wstring)> onPath;
			Event<void(std::wstring)> onOpen;

			// api

			bool displayFiles()					{ return displayFiles_; }
			void displayFiles(bool b)			{ displayFiles_ = b;  }
			void path(const std::wstring& p)	{ path_ = p; showPath(p); }
			std::wstring path()					{ return path_; }
			bool hasFocus()						{ return ::GetFocus() == viewWnd_; }
		

			std::vector<std::wstring> selected()
			{
				punk<IDataObject> dao;
				HRESULT hr = shellView_->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&dao);
				if (hr == S_OK)
				{
					return vectorFromDataObject(*dao);
				}
				return std::vector<std::wstring>();
			}

			void refresh()
			{
				if (!shellView_)
					return;

				shellView_->Refresh();
			}

			void cut()
			{
				if (!shellView_)
					return;

				HRESULT hr;

				punk<IDataObject> dao;
				hr = shellView_->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&dao);
				if (hr == S_OK)
				{
					std::vector<std::wstring> v = vectorFromDataObject(*dao);
					if (!v.empty())
					{
						punk<IDataObject> ido(new Shell::DataObject(v,true));

						hr = ::OleSetClipboard(*ido);

						if (hr == S_OK)
						{
							punk<IShellFolderView> sfw(shellView_);

							if (sfw)
							{
								hr = sfw->SetClipboard(TRUE);
							}
						}
					}
				}
			}

			void copy()
			{
				if (!shellView_)
					return;

				punk<IDataObject> dao;
				HRESULT hr = shellView_->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&dao);
				if (hr == S_OK)
				{
					std::vector<std::wstring> v = vectorFromDataObject(*dao);
					if (!v.empty())
					{
						punk<IDataObject> ido( new Shell::DataObject(v, false));
						HRESULT hr = ::OleSetClipboard(*ido);
						if (hr == S_OK)
						{
							punk<IShellFolderView> sfw(shellView_);

							if (sfw)
							{
								hr = sfw->SetClipboard(FALSE);
							}
						}
					}
				}
			}

			void paste()
			{
				if (!shellView_)
					return;

				punk<IDataObject> dao;
				HRESULT hr = shellView_->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&dao);
				std::vector<std::wstring> v;
				if (hr == S_OK)
				{
					v = vectorFromDataObject(*dao);
				}

				std::wstring path;
				if (v.size() == 1)
					path = v[0];
				else
					path = path_;

				format_etc_pref_dropeffect	fe;
				STGMEDIUM					sm;

				punk<IDataObject> ido;
				::OleGetClipboard(&ido);
				if (!ido)
					return;

				v.clear();
				v = vectorFromDataObject(*ido);

				if (v.size() < 1)
					return;

				DWORD  dw = DROPEFFECT_COPY;
				DWORD* d = &dw;

				if (S_OK == ido->GetData(&fe, &sm))
				{
					if (sm.tymed == TYMED_HGLOBAL)
					{
						Global glob(sm.hGlobal);

						Global::Lock<DWORD*> lock(*glob);

						*d = **lock;
						glob.detach();
					}
				}

				if (*d & DROPEFFECT_COPY)
				{
					Shell::Copy(handle, v, path, FOF_ALLOWUNDO);
				}
				else if (*d & DROPEFFECT_MOVE)
				{
					Shell::Move(handle, v, path, FOF_ALLOWUNDO);
				}
				format_etc_dropeffect fede;
				StgMedium stgm(*d, GHND | GMEM_SHARE);
				ido->SetData(&fede, &stgm, TRUE);
			}

			void rename()
			{
				if (!shellView_)
					return;

				punk<IDataObject> dao;
				HRESULT hr = shellView_->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&dao);
				if (hr == S_OK)
				{
					std::vector<std::wstring> v = vectorFromDataObject(*dao);
					if (!v.empty())
					{
						Shit it;
						Shell::Folder sf = desktop();
						it = sf.parseDisplayName(v[0]);
						if (!it)
							return;
						HRESULT hr = shellView_->SelectItem(**it, SVSI_EDIT);
					}
				}
			}

			void erase()
			{
				if (!shellView_)
					return;

				punk<IDataObject> dao;
				HRESULT hr = shellView_->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&dao);
				if (hr == S_OK)
				{
					std::vector<std::wstring> v = vectorFromDataObject(*dao);
					if (!v.empty())
					{
						::MTL::Shell::Remove( handle, v[0], FOF_ALLOWUNDO);
					}
				}
			}

			void execute()
			{
				if (!shellView_)
					return;

				punk<IDataObject> dao;
				HRESULT hr = shellView_->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&dao);
				if (hr == S_OK)
				{
					std::vector<std::wstring> v = vectorFromDataObject(*dao);
					if (!v.empty())
					{
						Shell::Execute(v[0]);
					}
				}
				if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
				{
					// no selection
					Shell::Execute(path_);
				}
			}

			void updir()
			{
				std::wstring p(path_);
				std::wstring parent = MTL::Path(path_).parentDir().addBackSlash().str();
				if (Path(parent).isDir())
				{
					path_ = parent;
					showPath(path_);
				}
			}

			void newdir()
			{
				if (!shellView_)
					return;

				punk<IDataObject> dao;
				HRESULT hr = shellView_->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&dao);
				if (hr == S_OK)
				{
					std::vector<std::wstring> v = vectorFromDataObject(*dao);
					if (!v.empty())
					{
						std::wstring tmp = L"newDir_";
						int i = 0;
						while (true)
						{
							std::wstring p(path_);
							std::wstringstream oss;
							oss << tmp << i;
							p = Path(p).append(oss.str()).str();
							if (!::CreateDirectory(p.c_str(), 0))
							{
								if (::GetLastError() == ERROR_ALREADY_EXISTS)
								{
									i++;
									continue;
								}
							}
							//this->update();
							this->refresh();
							return;
						}
					}
				}
				if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
				{
					// no selection
					std::wstring tmp = L"newDir_";
					int i = 0;
					while (true)
					{
						std::wstring p(path_);
						std::wstringstream oss;
						oss << tmp << i;
						p = Path(p).append(oss.str()).str();
						if (!::CreateDirectory(p.c_str(), 0))
						{
							if (::GetLastError() == ERROR_ALREADY_EXISTS)
							{
								i++;
								continue;
							}
						}
						this->refresh();
						return;
					}
				}
			}

			void properties()
			{
				if (!shellView_)
					return;

				punk<IDataObject> dao;
				HRESULT hr = shellView_->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&dao);
				if (hr == S_OK)
				{
					std::vector<std::wstring> v = vectorFromDataObject(*dao);
					if (!v.empty())
					{
						Shell::Execute(v[0], L"properties", 1, SEE_MASK_INVOKEIDLIST);
					}
				}
				if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
				{
					// no selection
					Shell::Execute(path_, L"properties", 1, SEE_MASK_INVOKEIDLIST);
				}
			}

			// wndproc
			LRESULT wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				if (message == WM_CREATE)
				{
					//path(_T("F:\\"));
				}
				if (message == WM_DESTROY)
				{
					//dispose();
				}
				if (message == WM_SIZE)
				{
					RECT clientRect_ = { 0, 0, 0, 0 };

					// get new width and height
					clientRect_.right = LOWORD(lParam);
					clientRect_.bottom = HIWORD(lParam);

					::SetWindowPos(viewWnd_, NULL, 0, 0, clientRect_.right, clientRect_.bottom, SWP_NOZORDER | SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOCOPYBITS);
					::RedrawWindow(viewWnd_, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_INTERNALPAINT);
				}
				return Window<FolderView>::wndProc(hwnd, message, wParam, lParam);
			}


			// COM section

			// Inherited via implements
			virtual HRESULT __stdcall GetWindow(HWND* phwnd) override
			{
				if (::IsWindow(handle))
				{
					*phwnd = handle;
					return S_OK;
				}
				return E_FAIL;
			}

			virtual HRESULT __stdcall ContextSensitiveHelp(BOOL fEnterMode) override
			{
				return S_OK;
			}

			virtual HRESULT __stdcall InsertMenusSB(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths) override
			{
				return S_OK;
			}

			virtual HRESULT __stdcall SetMenuSB(HMENU hmenuShared, HOLEMENU holemenuRes, HWND hwndActiveObject) override
			{
				return S_OK;
			}

			virtual HRESULT __stdcall RemoveMenusSB(HMENU hmenuShared) override
			{
				return S_OK;
			}

			virtual HRESULT __stdcall SetStatusTextSB(LPCWSTR pszStatusText) override
			{
				return S_OK;
			}

			virtual HRESULT __stdcall EnableModelessSB(BOOL fEnable) override
			{
				return S_OK;
			}

			virtual HRESULT __stdcall TranslateAcceleratorSB(MSG* pmsg, WORD wID) override
			{
				return S_OK;
			}

			virtual HRESULT __stdcall BrowseObject(PCUIDLIST_RELATIVE pidl, UINT wFlags) override
			{
				std::wstring s = desktop().getDisplayName((LPITEMIDLIST)pidl);
				onPath.fire(s);
				return S_OK;
			}

			virtual HRESULT __stdcall GetViewStateStream(DWORD grfMode, IStream** ppStrm) override
			{
				return E_NOTIMPL;
			}

			virtual HRESULT __stdcall GetControlWindow(UINT id, HWND* phwnd) override
			{
				*phwnd = NULL;
				return S_OK;
			}

			virtual HRESULT __stdcall SendControlMsg(UINT id, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pret) override
			{
				return E_NOTIMPL;
			}

			virtual HRESULT __stdcall QueryActiveShellView(IShellView** ppshv) override
			{
				return E_NOTIMPL;
			}

			virtual HRESULT __stdcall OnViewWindowActive(IShellView* pshv) override
			{
				return S_OK;
			}

			virtual HRESULT __stdcall SetToolbarItems(LPTBBUTTONSB lpButtons, UINT nButtons, UINT uFlags) override
			{
				return E_NOTIMPL;
			}


			virtual HRESULT __stdcall QueryService(REFGUID guidService, REFIID riid, void** ppv) override
			{
				if (::IsEqualIID(riid, IID_IShellBrowser))
				{
					punk<IShellBrowser> sb(this);
					return sb->QueryInterface(riid, ppv);
				}
				if (::IsEqualIID(riid, IID_ICommDlgBrowser))
				{
					punk<ICommDlgBrowser> db(this);
					return db->QueryInterface(riid, ppv);
				}
				if (::IsEqualIID(riid, IID_ICommDlgBrowser2))
				{
					punk<ICommDlgBrowser2> db(this);
					return db->QueryInterface(riid, ppv);
				}
				if (::IsEqualIID(riid, IID_IContextMenuSite))
				{
					//punk<IContextMenuSite> cms(this);
					//return cms->QueryInterface(riid, ppv);
				}

				return E_NOINTERFACE;
			}

			virtual HRESULT __stdcall OnDefaultCommand(IShellView* ppshv) override
			{
				punk<IDataObject> dao;
				HRESULT hr = ppshv->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&dao);
				if (hr == S_OK)
				{
					std::vector<std::wstring> v = vectorFromDataObject(*dao);
					if (!v.empty())
					{
						if (Path(v[0]).isDir())
						{
							path_ = v[0];
							this->showPath(path_);
							onPath.fire(path_);
						}
						else
						{
							onOpen.fire(v[0]);
						}
					}
				}

				return S_OK;
			}

			virtual HRESULT __stdcall OnStateChange(IShellView* ppshv, ULONG uChange) override
			{
				onChange.fire(uChange);
				return S_OK;
			}

			virtual HRESULT __stdcall IncludeObject(IShellView* ppshv, PCUITEMID_CHILD pidl) override
			{
				return S_OK;
			}

			virtual HRESULT __stdcall Notify(IShellView* ppshv, DWORD dwNotifyType) override
			{
				return S_OK;
			}

			virtual HRESULT __stdcall GetDefaultMenuText(IShellView* ppshv, LPWSTR pszText, int cchMax) override
			{
				//std::wstring s(L"open");
				//wcscpy_s(pszText,s.size(), s.c_str());
				return S_FALSE;
			}

			virtual HRESULT __stdcall GetViewFlags(DWORD* pdwFlags) override
			{
				*pdwFlags = 0;
				return S_OK;
			}

			/*
			virtual HRESULT __stdcall DoContextMenuPopup(IUnknown* punkContextMenu, UINT fFlags, POINT pt) override
			{
				return S_OK;
			}
			*/

		protected:

			bool showPath(const std::wstring& path)
			{
				Shit it;
				Shell::Folder sf = desktop();

				it = sf.parseDisplayName(path);
				if (!it)
					return false;

				Shell::Folder f(**it);
				if (!f)
					return false;

				HRESULT hr = E_FAIL;
				if (shellView_)
				{
					hr = shellView_->DestroyViewWindow();
					shellView_.release();
				}

				SFV_CREATE sc;
				::ZeroMemory(&sc, sizeof(sc));
				sc.cbSize = sizeof(sc);
				sc.psfvcb = NULL;
				sc.pshf = *(f.folder);
				sc.psvOuter = NULL;

				hr = f.folder->CreateViewObject( handle, IID_IShellView, (void**)&shellView_);
				if (hr != S_OK)
					return false;

				FOLDERSETTINGS fs;
				::ZeroMemory(&fs, sizeof(fs));
				fs.fFlags = 0;
				fs.ViewMode = FVM_DETAILS;

				punk<IShellBrowser> sb(this);
				if (!sb)
					return false;

				RECT r;
				::GetClientRect(handle,&r);
				hr = shellView_->CreateViewWindow(0, &fs, *sb, &r, &viewWnd_);
				if (hr != S_OK)
					return false;

				hr = shellView_->UIActivate(SVUIA_INPLACEACTIVATE);
				if (hr != S_OK)
					return false;

				onPath.fire(path_);
				return true;
			}

		protected:

			punk<IShellView> shellView_;
			std::wstring path_;
			bool displayFiles_ = true;
			HWND viewWnd_ = nullptr;
		};

	}
}