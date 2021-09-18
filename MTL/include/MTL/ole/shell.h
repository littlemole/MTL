#pragma once

#include <mtl/ole/dataobj.h>
#include "mtl/win/gdi.h"

namespace mtl {

	inline std::wstring app_files_path(const std::wstring& dir)
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

	namespace shell {
		class folder;
	}

	inline shell::folder desktop();

	namespace shell {

		class item
		{
			friend class folder;
		public:
			item(LPITEMIDLIST pidl, DWORD attributes)
				:pidl_(pidl), attributes_(attributes)
			{}

			~item();

			bool is_dir()
			{
				return (attributes_ & SFGAO_FOLDER) != 0;;
			}

			bool is_file()
			{
				return (attributes_ & SFGAO_STREAM) != 0;
			}

			bool is_part_of_filesystem()
			{
				return (attributes_ & SFGAO_FILESYSANCESTOR) != 0; //SFGAO_FILESYSTEM) != 0;
			}

			bool is_browsable()
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

	typedef std::shared_ptr<shell::item> shit;

	namespace shell {

		class enumerator 
		{
		public:

			enumerator()
			{}

			// while next
			shit next(DWORD attributes = SFGAO_FOLDER | SFGAO_STREAM | SFGAO_FILESYSTEM | SFGAO_BROWSABLE | SFGAO_FILESYSANCESTOR);

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

		class folder
		{
			friend class item;
		public:
			folder()
			{
				::SHGetDesktopFolder(&shell_folder);
			}

			folder(LPITEMIDLIST pidl, folder parent = desktop())
			{
				shell_folder = parent.sub_folder(pidl).shell_folder;
			}

			folder(IShellFolder* sf)
			{
				shell_folder = sf;
			}

			folder(const std::wstring& path, DWORD attributes = SFGAO_FOLDER | SFGAO_STREAM | SFGAO_FILESYSTEM | SFGAO_BROWSABLE | SFGAO_FILESYSANCESTOR)
			{
				//ULONG chEaten = 0;
				//LPITEMIDLIST pidl = 0;

				auto shit = parse_display_name(path, attributes);
				auto desk = desktop();
				//HR hr = desk.folder->ParseDisplayName(0, 0, (wchar_t*)path.c_str(), &chEaten, &pidl, 0);
				HR hr = desk.shell_folder->BindToObject(**shit, 0, IID_IShellFolder, (void**)&shell_folder);
				//desk.release_pidl(pidl);
			}

			~folder()
			{}

			std::wstring display_name(DWORD flags = SHGDN_FORPARSING)
			{
				shell::folder desk = desktop();
				return display_name(desk, flags);
			}

			std::wstring display_name(shell::item& shit, DWORD flags = SHGDN_FORPARSING)
			{
				return display_name(*shit, flags);
			}

			std::wstring display_name(shell::folder& parent, DWORD flags = SHGDN_FORPARSING)
			{
				punk<IPersistFolder2> pf2(shell_folder);
				if (!pf2) return L"";

				LPITEMIDLIST pidl = 0;
				HR hr = pf2->GetCurFolder(&pidl);

				std::wstring result = parent.display_name(pidl, flags);

				release_pidl(pidl);
				return result;
			}
			
			std::wstring display_name(LPITEMIDLIST pidl, DWORD flags = SHGDN_FORPARSING)
			{
				if (S_OK == shell_folder->GetDisplayNameOf(pidl, flags, &strret_))
				{
					wchar_t buf[MAX_PATH];
					StrRetToBuf(&strret_, pidl, buf, MAX_PATH);
					return std::wstring(buf);
				}
				return L"";
			}

			shit shell_item()
			{
				punk<IPersistFolder2> pf2(shell_folder);
				if (pf2)
				{
					LPITEMIDLIST pidl;
					if (S_OK == pf2->GetCurFolder(&pidl))
						return shit(new shell::item(pidl, attributes_of(pidl)));
				}
				return shit();
			}

			shit special_folder(int csidl)
			{
				LPITEMIDLIST pidl = 0;
				::SHGetSpecialFolderLocation(0, csidl, &pidl);
				return shit(new shell::item(pidl, attributes_of(pidl)));
			}

			shit parse_display_name(const std::wstring& path, DWORD attributes = SFGAO_FOLDER | SFGAO_STREAM | SFGAO_FILESYSTEM | SFGAO_BROWSABLE | SFGAO_FILESYSANCESTOR)
			{
				shell::folder	desk;
				LPITEMIDLIST pidl = 0;
				ULONG ulong = 0;
				// first param hwnd
				if (S_OK == desk.shell_folder->ParseDisplayName(0, NULL, (LPOLESTR)(path.c_str()), &ulong, &pidl, &attributes))
				{
					shell::item* it = new shell::item(pidl, desk.attributes_of(pidl, attributes));
					return shit(it);
				}
				return shit();
			}

			// get Attributes of item
			ULONG attributes_of(LPITEMIDLIST pidl, ULONG attributes = SFGAO_FOLDER | SFGAO_STREAM | SFGAO_FILESYSTEM | SFGAO_BROWSABLE | SFGAO_FILESYSANCESTOR)
			{
				if (S_OK == shell_folder->GetAttributesOf(1, (LPCITEMIDLIST*)&pidl, &attributes))
					return attributes;
				return 0;
			}

			// prepare enumeration
			enumerator enumerate(HWND owner = NULL, SHCONTF flags = SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN)
			{
				enumerator enumObj;
				HR hr = shell_folder->EnumObjects(owner, flags, &enumObj);
				return enumObj;
			}

			// is valid
			operator bool() { return shell_folder; }

			punk<IShellFolder>   shell_folder;


			void release_pidl(LPITEMIDLIST pidl)
			{
				if (pidl)
				{
					::CoTaskMemFree(pidl);
				}
			}

			shell::folder sub_folder(LPITEMIDLIST pidl)
			{
				IShellFolder* pFolder = nullptr;
				HR hr = shell_folder->BindToObject(pidl, NULL, IID_IShellFolder, (LPVOID*)&pFolder);
				return shell::folder(pFolder);
			}

		protected:

			punk<IEnumIDList>				enumIdl_;
			punk<IShellFolder>				desk_;
			STRRET							strret_;
		};

		////////////////////////////////////////////////////////////////////////////////////

		inline shit parse_display_name(const std::wstring& path, DWORD attributes = SFGAO_FOLDER | SFGAO_STREAM | SFGAO_FILESYSTEM | SFGAO_BROWSABLE | SFGAO_FILESYSANCESTOR)
		{
			shell::folder	desk;
			LPITEMIDLIST pidl = 0;
			ULONG ulong = 0;
			// first param hwnd
			if (S_OK == desk.shell_folder->ParseDisplayName(0, NULL, (LPOLESTR)(path.c_str()), &ulong, &pidl, &attributes))
			{
				shell::item* it = new shell::item(pidl, desk.attributes_of(pidl, attributes));
				return shit(it);
			}
			return shit();
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


		inline int  copy(
			HWND hwnd,
			const std::wstring& from,
			const std::wstring& to,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			return details::op(FO_COPY, hwnd, from, to, flags);
		}

		inline int  copy(
			HWND hwnd,
			const std::vector<std::wstring>& from,
			const std::wstring& to,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			return details::multiOp(FO_COPY, hwnd, from, to, flags);
		}


		inline int  remove(
			HWND hwnd,
			const std::wstring& from,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			std::wstring to(L"");
			return details::op(FO_DELETE, hwnd, from, to, flags);
		}

		inline int  remove(
			HWND hwnd,
			const std::vector<std::wstring>& from,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			std::wstring to(L"");
			return details::multiOp(FO_DELETE, hwnd, from, to, flags);
		}

		inline int  move(
			HWND hwnd,
			const std::wstring& from,
			const std::wstring& to,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			return details::op(FO_MOVE, hwnd, from, to, flags);
		}


		inline int  move(
			HWND hwnd,
			const std::vector<std::wstring>& from,
			const std::wstring& to,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			return details::multiOp(FO_MOVE, hwnd, from, to, flags);
		}

		inline int  rename(
			HWND hwnd,
			const std::wstring& from,
			const std::wstring& to,
			FILEOP_FLAGS flags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI)
		{
			return details::op(FO_RENAME, hwnd, from, to, flags);
		}

		inline bool create_dir(const std::wstring& dirname)
		{
			return ::CreateDirectoryW(mtl::path(dirname).wpath().c_str(), 0) == TRUE;
		}

		//		SHFILEOPSTRUCT sfos_;

		inline BOOL execute(const std::wstring& path, const std::wstring& verb = L"open", int nShow = SW_SHOWNORMAL, ULONG fMask = 0)
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


		inline BOOL execute_args(
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
	inline shell::folder desktop()
	{
		return shell::folder();
	}

	namespace shell {

		inline HICON stock_icon(SHSTOCKICONID id)
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

		inline HICON file_icon(const std::wstring& filepath)
		{
			static std::map<std::wstring, icon> map;

			std::wstring ext = mtl::path(filepath).ext();

			if (map.count(ext))
			{
				return *map[ext];
			}

			SHFILEINFO sfi;
			::ZeroMemory(&sfi, sizeof(sfi));
			
			bool b = ::SHGetFileInfo(filepath.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON);
			if (!b)
			{
				return 0;
			}
			map[ext] = sfi.hIcon;
			return sfi.hIcon;
		}

		inline item::~item()
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
		inline shit enumerator::next(DWORD attributes )
		{
			ULONG fetched;
			LPITEMIDLIST pidl = 0;

			if (enumIdl_)
			{
				if (S_OK == enumIdl_->Next(1, &pidl, &fetched))
				{
					if (fetched == 1)
					{
						shell::item* it = new shell::item(pidl, desktop().attributes_of(pidl, attributes));
						return shit(it);
					}
				}
			}
			return shit();
		}

		class folder_view : public window<folder_view>,
			public implements<stack_object<folder_view>(
				IShellBrowser, 
				IServiceProvider, 
				ICommDlgBrowser2, of<ICommDlgBrowser2, ICommDlgBrowser>)>//, IContextMenuSite)>
		{
		public:

			folder_view()
			{}

			~folder_view()
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

			event<void(ULONG)> onChange;
			event<void(std::wstring)> onPath;
			event<void(std::wstring)> onOpen;

			// api

			bool display_files()				{ return displayFiles_; }
			void display_files(bool b)			{ displayFiles_ = b;  }
			void path(const std::wstring& p)	{ path_ = p; show_path(p); }
			std::wstring path()					{ return path_; }
			bool has_focus()					{ return ::GetFocus() == viewWnd_; }
		

			std::vector<std::wstring> selected()
			{
				punk<IDataObject> dao;
				HRESULT hr = shellView_->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&dao);
				if (hr == S_OK)
				{
					return vector_from_data_object(*dao);
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
					std::vector<std::wstring> v = vector_from_data_object(*dao);
					if (!v.empty())
					{
						punk<IDataObject> ido(new shell::data_object(v,true));

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
					std::vector<std::wstring> v = vector_from_data_object(*dao);
					if (!v.empty())
					{
						punk<IDataObject> ido( new shell::data_object(v, false));
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
					v = vector_from_data_object(*dao);
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
				v = vector_from_data_object(*ido);

				if (v.size() < 1)
					return;

				DWORD  dw = DROPEFFECT_COPY;
				DWORD* d = &dw;

				if (S_OK == ido->GetData(&fe, &sm))
				{
					if (sm.tymed == TYMED_HGLOBAL)
					{
						global glob(sm.hGlobal);

						global::lock<DWORD*> lock(*glob);

						*d = **lock;
						glob.detach();
					}
				}

				if (*d & DROPEFFECT_COPY)
				{
					shell::copy(handle, v, path, FOF_ALLOWUNDO);
				}
				else if (*d & DROPEFFECT_MOVE)
				{
					shell::move(handle, v, path, FOF_ALLOWUNDO);
				}
				format_etc_dropeffect fede;
				stg_medium stgm(*d, GHND | GMEM_SHARE);
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
					std::vector<std::wstring> v = vector_from_data_object(*dao);
					if (!v.empty())
					{
						shit it;
						shell::folder sf = desktop();
						it = sf.parse_display_name(v[0]);
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
					std::vector<std::wstring> v = vector_from_data_object(*dao);
					if (!v.empty())
					{
						::mtl::shell::remove( handle, v[0], FOF_ALLOWUNDO);
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
					std::vector<std::wstring> v = vector_from_data_object(*dao);
					if (!v.empty())
					{
						shell::execute(v[0]);
					}
				}
				if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
				{
					// no selection
					shell::execute(path_);
				}
			}

			void up_dir()
			{
				std::wstring p(path_);
				std::wstring parent = ::mtl::path(path_).parent_dir().add_backslash().str();
				if (mtl::path(parent).is_dir())
				{
					path_ = parent;
					show_path(path_);
				}
			}

			void new_dir()
			{
				if (!shellView_)
					return;

				punk<IDataObject> dao;
				HRESULT hr = shellView_->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&dao);
				if (hr == S_OK)
				{
					std::vector<std::wstring> v = vector_from_data_object(*dao);
					if (!v.empty())
					{
						std::wstring tmp = L"newDir_";
						int i = 0;
						while (true)
						{
							std::wstring p(path_);
							std::wstringstream oss;
							oss << tmp << i;
							p = mtl::path(p).append(oss.str()).str();
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
						p = mtl::path(p).append(oss.str()).str();
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
					std::vector<std::wstring> v = vector_from_data_object(*dao);
					if (!v.empty())
					{
						shell::execute(v[0], L"properties", 1, SEE_MASK_INVOKEIDLIST);
					}
				}
				if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
				{
					// no selection
					shell::execute(path_, L"properties", 1, SEE_MASK_INVOKEIDLIST);
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
				return window<folder_view>::wndProc(hwnd, message, wParam, lParam);
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
				std::wstring s = desktop().display_name((LPITEMIDLIST)pidl);
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
					std::vector<std::wstring> v = vector_from_data_object(*dao);
					if (!v.empty())
					{
						if (mtl::path(v[0]).is_dir())
						{
							path_ = v[0];
							this->show_path(path_);
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

			bool show_path(const std::wstring& path)
			{
				shit it;
				shell::folder sf = desktop();

				it = sf.parse_display_name(path);
				if (!it)
					return false;

				shell::folder f(**it);
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
				sc.pshf = *(f.shell_folder);
				sc.psvOuter = NULL;

				hr = f.shell_folder->CreateViewObject( handle, IID_IShellView, (void**)&shellView_);
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