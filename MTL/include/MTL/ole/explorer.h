#pragma once

#include <mtl/punk.h>
#include <mtl/win/wnd.h>
#include <mtl/win32/mem.h>
#include <mtl/obj/impl.h>

#include <olectl.h>
#include <shobjidl_core.h>
#include <Shlwapi.h>
#include <shobjidl.h>
#include <Windowsx.h.>
#include <KnownFolders.h>

namespace mtl {

	class explorer_tree : public window<explorer_tree>, public implements<stack_object<explorer_tree>(INameSpaceTreeControlEvents, IServiceProvider)>
	{
	public:

		event<void(bool, std::wstring)> onClick;
		event<void(std::wstring)> onSelect;

		explorer_tree()
		{
			adviseCookie_ = -1;
			lastClickTime_ = 0;

			doubleClickTimeout_ = ::GetDoubleClickTime() * 2;

			//useContext_ = true;

			foreCol_ = ::GetSysColor(COLOR_MENUTEXT);
			bgCol_   = ::GetSysColor(COLOR_WINDOW);
			bgBrush_ = ::CreateSolidBrush(bgCol_);
		}

		~explorer_tree()
		{
			::DeleteObject(bgBrush_);
		}

		virtual LRESULT wm_erase_background(WPARAM wParam) override
		{
			// prevent bkgrnd erase
			return 1;
		}

		virtual LRESULT wm_destroy() override
		{
			if (tree_)
			{
				if (adviseCookie_ != -1)
				{
					tree_->TreeUnadvise(adviseCookie_);
					adviseCookie_ = (DWORD)-1;
				}

				IUnknown_SetSite( *tree_, NULL);
				tree_.release();
			}
			currentItem_.release();
			return 0;
		}

		virtual LRESULT wm_create() override
		{
			//treeMenu_.load(IDM_TREE_DIR);
			::GetClientRect(handle,&clientRect_);

			RECT rc;
			::GetWindowRect(handle,&rc);
			::MapWindowRect(HWND_DESKTOP, handle, &rc);

			HRESULT hr = tree_.create_object(CLSID_NamespaceTreeControl, CLSCTX_INPROC);
			if (SUCCEEDED(hr))
			{
				const NSTCSTYLE nsctsFlags = //NSTCS_HASEXPANDOS |            // Show expandos
											   // NSTCS_ROOTHASEXPANDO |         // Root nodes have expandos
											   // NSTCS_FADEINOUTEXPANDOS |      // Fade-in-out based on focus
											   // NSTCS_NOINFOTIP |              // Don't show infotips
												//NSTCS_ALLOWJUNCTIONS |         // Show folders such as zip folders and libraries
					NSTCS_SHOWSELECTIONALWAYS |    // Show selection when NSC doesn't have focus
					//NSTCS_TABSTOP|
					//NSTCS_SINGLECLICKEXPAND|
					NSTCS_SHOWTABSBUTTON |
					NSTCS_FULLROWSELECT;           // Select full width of item

				hr = tree_->Initialize( handle, &rc, nsctsFlags);
				if (SUCCEEDED(hr))
				{
					// New Windows 7 features
					punk<INameSpaceTreeControl2> tree2(tree_);
					if (tree2)
					{
						NSTCSTYLE2 nscts2Flags = //NSTCS2_DISPLAYPADDING |            // Padding between top-level nodes
							NTSCS2_NOSINGLETONAUTOEXPAND |     // Don't auto-expand nodes with a single child node
							NSTCS2_INTERRUPTNOTIFICATIONS |    // Register for interrupt notifications on a per-node basis
							NSTCS2_DISPLAYPINNEDONLY |         // Filter on pinned property
							NTSCS2_NEVERINSERTNONENUMERATED;   // Don't insert items with property SFGAO_NONENUMERATED
						hr = tree2->SetControlStyle2(nscts2Flags, nscts2Flags);
					}
					if (SUCCEEDED(hr))
					{
						
						tree_->TreeAdvise(static_cast<INameSpaceTreeControlEvents*>(this), &adviseCookie_);
						IUnknown_SetSite( *tree_, static_cast<IServiceProvider*>(this));

						tree_->RemoveAllRoots();


						punk<IShellItem> desktop;
						hr = ::SHCreateItemInKnownFolder(FOLDERID_Desktop, 0, NULL, IID_PPV_ARGS(&desktop));
						if (SUCCEEDED(hr))
						{
							// Add hidden root
							tree_->AppendRoot( *desktop, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, NSTCRS_HIDDEN | NSTCRS_EXPANDED, NULL); // ignore result
						}
						
					}
				}
			}

			return 0;
		}

		virtual void show_item(const std::wstring& path)
		{
			std::wstring p = path + L"\\";
			punk<IShellItem> shit;
			HRESULT hr = ::SHCreateItemFromParsingName(path.c_str(), nullptr, IID_IShellItem, (void**)&shit);
			if (hr == S_OK && shit)
			{
				punk<INameSpaceTreeControl2> tree2(tree_);
				hr = tree2->EnsureItemVisible(*shit);
				if (hr != S_OK)
				{
					::OutputDebugString(L"show item failed");
				}
			};
		}

		virtual void set_root()
		{

			punk<IShellItem> shit;
			HRESULT hr = ::SHCreateItemInKnownFolder(FOLDERID_Desktop, 0, NULL, IID_PPV_ARGS(&shit));
			if (hr == S_OK && shit)
			{
				tree_->RemoveAllRoots();
				tree_->AppendRoot(*shit, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, NSTCRS_HIDDEN | NSTCRS_EXPANDED, NULL);
			}
		}

		virtual void set_root(const std::wstring& path)
		{

			punk<IShellItem> shit;
			HRESULT hr = ::SHCreateItemFromParsingName(path.c_str(), nullptr, IID_IShellItem, (void**)&shit);
			if (hr == S_OK && shit)
			{
				tree_->RemoveAllRoots();
				tree_->AppendRoot( *shit, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, NSTCRS_HIDDEN | NSTCRS_EXPANDED, NULL);
			}
		}

		virtual LRESULT wm_size(RECT& clientRect) override
		{
			clientRect_ = clientRect;

			HWND hwndTree;
			IUnknown_GetWindow( *tree_, &hwndTree);

			RECT rc;
			::GetWindowRect(hwndTree,&rc);
			::MapWindowRect(HWND_DESKTOP, handle, &rc);
			::SetWindowPos(hwndTree, NULL, rc.left, rc.top, rc.right, rc.bottom, SWP_NOZORDER);
			return 0;
		}

		// QueryService

		HRESULT __stdcall QueryService(REFGUID /*guidService*/, REFIID riid, void** ppv) override
		{
			if (riid == IID_INameSpaceTreeControlCustomDraw)
			{
				return this->QueryInterface(riid, ppv);
			}

			HRESULT hr = E_NOINTERFACE;
			*ppv = NULL;
			return hr;
		}


		// INameSpaceTreeControlEvents

		HRESULT virtual __stdcall  OnItemClick(IShellItem* psi, NSTCEHITTEST nstceHitTest, NSTCECLICKTYPE nstceClickType) override
		{
			HRESULT ret = S_OK;

			if ((nstceClickType & NSTCECT_LBUTTON) && psi && (nstceHitTest & NSTCEHT_ONITEMTABBUTTON))
			{
				co_str psz;
				HRESULT hr = psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &psz);
				if (hr == S_OK )
				{
					SFGAOF sfgaof;
					hr = psi->GetAttributes(SFGAO_FOLDER, &sfgaof);
					if (SUCCEEDED(hr))
					{
						bool b = sfgaof & SFGAO_FOLDER ? true : false;
						onClick.fire(b, psz.str());
						ret = S_FALSE;
					}
				}
			}
			if ((nstceClickType & NSTCECT_LBUTTON) && psi && (nstceHitTest & NSTCEHT_ONITEM))
			{
				co_str psz;
				HRESULT hr = psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &psz);
				if (hr == S_OK)
				{
					DWORD now = ::GetTickCount();
					std::wstring ws = psz.str();
					if ((ws == currentPath_) && (now - lastClickTime_ < doubleClickTimeout_))
					{
						SFGAOF sfgaof;
						hr = psi->GetAttributes(SFGAO_FOLDER, &sfgaof);
						if (SUCCEEDED(hr))
						{
							bool b = sfgaof & SFGAO_FOLDER ? true : false;
							if (b == false)
							{
								onClick.fire(b, ws);
								ret = S_FALSE;
							}
							ret = S_FALSE;
						}
					}
					lastClickTime_ = ::GetTickCount();;
				}
			}
			return ret;
		}

		HRESULT __stdcall OnSelectionChanged(IShellItemArray* psiaSelection) override
		{
			IShellItem* psi;
			HRESULT hr = psiaSelection->GetItemAt(0, &psi);
			if (SUCCEEDED(hr))
			{
				IShellItem2* psi2;
				hr = psi->QueryInterface(IID_PPV_ARGS(&psi2));
				if (SUCCEEDED(hr))
				{
					co_str psz;
					HRESULT hr = psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &psz);
					if (hr == S_OK)
					{
						currentPath_ = psz.str();
						currentItem_ = psi;
						onSelect.fire(currentPath_);
					}
					psi2->Release();
				}
				psi->Release();
			}
			return S_OK;
		}


		// all these have no or default impl
		HRESULT virtual __stdcall  OnPropertyItemCommit(IShellItem* /*psi*/) { return S_FALSE; }
		HRESULT virtual __stdcall  OnItemStateChanging(IShellItem* /*psi*/, NSTCITEMSTATE /*nstcisMask*/, NSTCITEMSTATE /*nstcisState*/) { return S_OK; }
		HRESULT virtual __stdcall  OnItemStateChanged(IShellItem* /*psi*/, NSTCITEMSTATE /*nstcisMask*/, NSTCITEMSTATE /*nstcisState*/) { return S_OK; }
		HRESULT virtual __stdcall  OnKeyboardInput(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/) { return S_FALSE; }
		HRESULT virtual __stdcall  OnBeforeExpand(IShellItem* /*psi*/) { return S_OK; }
		HRESULT virtual __stdcall  OnAfterExpand(IShellItem* /*psi*/) { return S_OK; }
		HRESULT virtual __stdcall  OnBeginLabelEdit(IShellItem* /*psi*/) { return S_OK; }
		HRESULT virtual __stdcall  OnEndLabelEdit(IShellItem* /*psi*/) { return S_OK; }
		HRESULT virtual __stdcall  OnGetToolTip(IShellItem* /*psi*/, LPWSTR /*pszTip*/, int /*cchTip*/) { return E_NOTIMPL; }
		HRESULT virtual __stdcall  OnBeforeItemDelete(IShellItem* /*psi*/) { return E_NOTIMPL; }
		HRESULT virtual __stdcall  OnItemAdded(IShellItem* /*psi*/, BOOL /*fIsRoot*/) { return E_NOTIMPL; }
		HRESULT virtual __stdcall  OnItemDeleted(IShellItem* /*psi*/, BOOL /*fIsRoot*/) { return E_NOTIMPL; }
		HRESULT virtual __stdcall  OnBeforeContextMenu(IShellItem* /*psi*/, REFIID /*riid*/, void** ppv) { *ppv = NULL; return E_NOTIMPL; }
		HRESULT virtual __stdcall  OnAfterContextMenu(IShellItem* /*psi*/, IContextMenu* /*pcmIn*/, REFIID /*riid*/, void** ppv) { *ppv = NULL; return E_NOTIMPL; }
		HRESULT virtual __stdcall  OnBeforeStateImageChange(IShellItem* /*psi*/) { return S_OK; }
		HRESULT virtual __stdcall  OnGetDefaultIconIndex(IShellItem* /*psi*/, int* /*piDefaultIcon*/, int* /*piOpenIcon*/) { return E_NOTIMPL; }

		std::wstring getSelection()
		{
			return currentPath_;
		}

	private:

		void InvokeVerb(const std::string& verb);

		punk<INameSpaceTreeControl>					tree_;
		punk<IShellItem>							currentItem_;

		std::wstring								currentPath_;
		bool										displayFiles_;
		//bool										useContext_;
		RECT										clientRect_;
		//mol::Menu									treeMenu_;
		DWORD										adviseCookie_;

		OLE_COLOR									bgCol_;
		OLE_COLOR									foreCol_;
		HBRUSH										bgBrush_;

		DWORD										lastClickTime_;
		DWORD										doubleClickTimeout_;
	};
}