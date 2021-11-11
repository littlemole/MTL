
#include "multipad.h"

#include <dwmapi.h>

class EditWnd : public mtl::mdi_child
{
public:

	std::wstring id;
	mtl::edit_ctrl editCtrl;
	mtl::default_layout layout;

	mtl::event<void()> onDestroy;

	EditWnd(int menuId)
		: mtl::mdi_child(menuId)
	{
		fontDesc_ = mtl::font_desc(L"Lucida Console", 18);
		font_ = fontDesc_.create();
		id = mtl::new_guid();
	}

	virtual LRESULT wm_create() override
	{
		RECT r = client_rect();

		HWND hWnd = editCtrl.create(
			IDC_EDIT_CONTROL, 
			handle, 
			r, 
			WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_HSCROLL | WS_VSCROLL);

		editCtrl.set_font(*font_, true);

		layout = {
			{
				{ hWnd, mtl::layout::style::FILL }
			} 
		};
		return 0;
	}

	bool load(const std::wstring filename)
	{
		std::ifstream ifs;
		ifs.open( mtl::to_string(filename), std::ifstream::in|std::ios::binary);

		if (!ifs) return false;

		filename_ = filename;

		std::ostringstream oss;
		while (ifs)
		{
			std::string line;
			std::getline(ifs, line);
			oss << line << "\r\n";
		}
		ifs.close();

		editCtrl.set_text(mtl::to_wstring(oss.str()));
		set_text(filename);

		return true;
	}


	virtual LRESULT wm_size(RECT& clientRect) override
	{
		RECT r = { 0,0,0,0 };
		layout.do_layout(clientRect,r);
		return 0;
	}

	virtual LRESULT wm_erase_background(WPARAM wParam) override
	{
		// prevent bkgrnd erase
		return 1;
	}

	virtual LRESULT wm_dpi_changed(RECT* r) override
	{
		recalc();
		editCtrl.set_font( *font_, true);
		return 0;
	}

	virtual LRESULT wm_destroy() override
	{
		onDestroy.fire();
		return 0;
	}

private:

	// DPI awareness inside here
	void recalc()
	{
		int iDpi = ::GetDpiForWindow(this->handle);
		mtl::font_desc fd = fontDesc_.scale(iDpi);
		font_ = fd.create();
	}

	std::wstring	filename_;
	mtl::font_desc	fontDesc_;
	mtl::font		font_ = nullptr;
};

class taskbar_tab : public mtl::window<taskbar_tab>
{
public:

	std::wstring id;
	HWND owner = nullptr;
	HWND mainWnd = nullptr;

	mtl::event<void(std::wstring)> onClose;
	mtl::event<void(std::wstring)> onActivate;

	taskbar_tab(HWND o, HWND m) : owner(o), mainWnd(m) 
	{
		id = mtl::new_guid();
	}

	taskbar_tab(std::wstring key, HWND o, HWND m) : id(key), owner(o), mainWnd(m)
	{
	}

	virtual LRESULT wm_create() override
	{
		BOOL fForceIconic = FALSE;
		BOOL fHasIconicBitmap = TRUE;

		mtl::HR hr = ::DwmSetWindowAttribute(
			handle,
			DWMWA_FORCE_ICONIC_REPRESENTATION,
			&fForceIconic,
			sizeof(fForceIconic));

		hr = ::DwmSetWindowAttribute(
			handle,
			DWMWA_HAS_ICONIC_BITMAP,
			&fHasIconicBitmap,
			sizeof(fHasIconicBitmap));

		return 0;
	}

	LRESULT virtual wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_ACTIVE)
			{
				onActivate.fire(id);
				return 0;
			}
			break;
		}
		case WM_SYSCOMMAND:
		{
			if (wParam != SC_CLOSE)
			{
				LRESULT lResult = ::SendMessage(mainWnd, WM_SYSCOMMAND, wParam, lParam);
				return lResult;
			}
			else
			{
				onClose.fire(id);
				return 0;
			}
			break;
		}
		case WM_CLOSE:
		{
			onClose.fire(id);
			return 0;
			break;
		}
		case WM_DWMSENDICONICTHUMBNAIL:
		{
			int w = HIWORD(lParam);
			int h = LOWORD(lParam);

			RECT r;
			::GetClientRect(owner, &r);
			auto bmp = mtl::screenshot(owner,r.right,r.bottom, true);
			
			mtl::wnd_dc dc(owner);

			auto thumbnail = mtl::bitmap::make_transparent_dib_section(w, h);

			{
				mtl::compatible_dc cdc(*dc);
				cdc.select(*thumbnail);

				mtl::compatible_dc cdc_src(*dc);
				cdc_src.select(*bmp);

				BITMAP bm;
				::GetObject(*bmp, sizeof(bm), &bm);

				if (bm.bmWidth < w && bm.bmHeight < h)
				{
					::BitBlt(*cdc, 0, 0, bm.bmWidth, bm.bmHeight, *cdc_src, 0, 0, SRCCOPY);
				}
				else
				{
					::StretchBlt(*cdc, 0, 0, w, h, *cdc_src, 0, 0, w * 2, h * 2, SRCCOPY);
				}
			}
			
			::DwmSetIconicThumbnail(handle, *thumbnail, 0);
			return 0;
			break;
		}
		case WM_DWMSENDICONICLIVEPREVIEWBITMAP:
		{
			RECT r;
			::GetWindowRect(owner, &r);

			RECT rm;
			::GetWindowRect(mainWnd, &rm);

			int w = r.right - r.left;
			int h = r.bottom - r.top;
			auto bmp = mtl::screenshot(owner, w, h, false);

			POINT pt = { r.left,r.top };
			POINT pt2 = { rm.left, rm.top };

			pt.x -= pt2.x;
			pt.y -= pt2.y;
			
			::DwmSetIconicLivePreviewBitmap(handle, *bmp, &pt, 0);// DWM_SIT_DISPLAYFRAME);

			return 0;
			break;
		}
		}
		return mtl::window<taskbar_tab>::wndProc(hWnd, message, wParam, lParam);
	}
};

class ImgWnd : public mtl::mdi_child
{
public:

	std::wstring id;
	mtl::event<void()> onDestroy;

	ImgWnd(int menuId)
		: mtl::mdi_child(menuId)
	{
		id = mtl::new_guid();
	}

	virtual LRESULT wm_draw(HDC hdc, RECT& r) override
	{
		mtl::dc dcv(hdc);
		dcv.bit_blit(*bitmap_, 0, 0);
		return 0;
	}

	bool load(const std::wstring filename)
	{
		bitmap_ = mtl::load_picture(filename);

		BITMAP bmp;
		::GetObject(*bitmap_, sizeof(BITMAP), (LPSTR)&bmp);

		w = bmp.bmWidth;
		h = bmp.bmHeight;

		resize();
		return true;
	}

	virtual LRESULT wm_dpi_changed(RECT* r) override
	{
		resize();
		return 0;
	}

	virtual LRESULT wm_destroy() override
	{
		onDestroy.fire();
		return 0;
	}


private:

	void resize()
	{
		RECT r = window_rect();
		POINT pos = { r.left, r.top };
		RECT dest = { pos.x, pos.y, pos.x + w, pos.y + h };

		UINT styles = style();// ::GetWindowLong(handle, GWL_STYLE);

		int iDpi = ::GetDpiForWindow(this->handle);
		::AdjustWindowRectExForDpi(&dest, styles, FALSE, 0, iDpi);

		move(dest.left, dest.top, dest.right - dest.left, dest.bottom - dest.top );
	}

	int w = 0;
	int h = 0;

	mtl::bitmap bitmap_;
};


class Taskbar
{
public:

	mtl::punk<ITaskbarList> taskBar;
	mtl::punk<ITaskbarList3> taskBar3;

	std::map<std::wstring, std::unique_ptr<taskbar_tab>> tabs;

	mtl::event<void(std::wstring)> onClose;
	mtl::event<void(std::wstring)> onActivate;

	HWND& mainWnd;

	Taskbar(HWND& main)
		: mainWnd(main)
	{
		mtl::HR hr = taskBar.create_object(CLSID_TaskbarList);
		hr = taskBar->HrInit();
		hr = taskBar.query_interface(&taskBar3);
	}

	Taskbar& activate(std::wstring id)
	{
		if (tabs.count(id))
		{
			taskBar3->ActivateTab(tabs[id]->handle);
		}
		return *this;
	}

	Taskbar& remove(std::wstring id)
	{		
		taskBar3->UnregisterTab(tabs[id]->handle);
		tabs[id]->destroy();
		tabs.erase(id);
		return *this;
	}

	bool exists(const std::wstring& id)
	{
		return tabs.count(id);
	}

	taskbar_tab* tab(const std::wstring& id)
	{
		if (exists(id))
		{
			return tabs[id].get();
		}
		return nullptr;
	}

	HWND hWnd(const std::wstring& id)
	{
		if (exists(id))
		{
			return  tabs[id]->owner;
		}
		return nullptr;
	}

	void set_text(std::wstring id, const std::wstring& txt)
	{
		if (tabs.count(id))
		{
			::SetWindowText(tabs[id]->handle, txt.c_str());
		}
	}

	void add(std::wstring id, HWND tab, const std::wstring& txt)
	{
		add(id,tab);
		set_text(id, txt);
	}

	void add(std::wstring id, HWND tab)
	{
		mtl::wbuff buf(256);
		::GetWindowText(tab, buf, (int) buf.size());

		taskbar_tab* tw = new taskbar_tab(id,tab, mainWnd);

		RECT r = { -32000 ,-32000 ,10,10 };
		tw->create(0, buf, 0, r, WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);

		tw->onActivate([this, tw](std::wstring id)
		{
			taskBar3->ActivateTab(tw->handle);
			onActivate.fire(id);
		});
		tw->onClose([this](std::wstring id)
		{
			onClose.fire(id);
		});

		tabs[tw->id] = std::unique_ptr<taskbar_tab>(tw);
		taskBar3->RegisterTab(tw->handle, mainWnd);
		taskBar3->SetTabOrder(tw->handle, nullptr);
	}

};

// main (frame) window

class MainWindow : public mtl::mdi_frame
{
public:

	mtl::menu			menu;

	mtl::status_bar		statusBar;
	mtl::splitter		splitter;
	mtl::explorer_tree  tree;

	mtl::event<int()>	onCmd;

	Taskbar				taskbar;

	MainWindow() : taskbar(handle)
	{}

	MainWindow(UINT viewMenu, UINT firstChildId)
		: mtl::mdi_frame(viewMenu, firstChildId), taskbar(handle)
	{}

	virtual LRESULT wm_create() override
	{
		// get main window client rect dimension
		RECT r = client_rect();

		// create normal child windows, initially sized to full client rect
		// layout will resize them approp
		tree.create(IDC_TREE, handle, r, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
		statusBar.create(IDC_STATUS, L"", handle, r, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);

		// create splitter at given initial position and given width
		r.left = 200;
		r.right = 208;
		splitter.create(IDC_SPLITTER, handle, r, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);

		// set the layout
		layout = {							// Default Layout
			{										//  vector<Widget>
				{									//   Widget
					*statusBar,						//    holding statusbar
					mtl::layout::style::SOUTH		//    docked to SOUTH
				},
				{									//   Widget
					{								//    SplitterLayout
						*tree,						//     Left Splitter child window: tree
						*splitter,					//     Splitter Window Separator
						mdi_client()					//     Right Splitter MDI client area
					},
					mtl::layout::style::FILL		//   filling remaining space
				}
			}
		};

		return 0;
	}

	virtual LRESULT wm_command(int id) override
	{
		onCmd.fire(id);
		return 0;
	}

	virtual LRESULT wm_erase_background(WPARAM wParam) override
	{
		// prevent bkgrnd erase
		return 1;
	}

	virtual LRESULT wm_destroy() override
	{
		onCmd.fire(IDM_EXIT);
		return 0;
	}
};


// Controller initializes view and handles User Input

class MultipadController
{
public:

	mtl::menu		menu;
	MainWindow		mainWnd;

	MultipadController()
		: menu(IDC_MULTIPAD)
	{
		HWND hWnd = mainWnd.create(L"Multipad", WS_OVERLAPPEDWINDOW, 0, *menu);
		mainWnd.show().update();

		mainWnd.onCmd(ID_FILE_OPEN, [this]()
		{
			mtl::open_dlg openDlg( *mainWnd );
			if (openDlg.open(OFN_READONLY))
			{
				load(openDlg.filename());
			}
		});

		mainWnd.tree.onSelect([this](std::wstring path)
		{
			mtl::path p(path);
			if (!p.is_dir())
			{
				load(path);
			}
		});
	
		mainWnd.onCmd(ID_VIEW_CASCADE, [this]()
		{
			mainWnd.cascade();
		});

		mainWnd.onCmd(ID_VIEW_TILE, [this]()
		{
			mainWnd.tile_horizontal();
		});

		mainWnd.onCmd(ID_VIEW_ARRANGE, [this]()
		{
			mainWnd.icon_arrange();
		});

		mainWnd.onCmd(IDM_ABOUT, [this]()
		{
			mtl::dialog dlg;
			dlg.show_modal(IDD_ABOUTBOX, *mainWnd );
		});

		mainWnd.onCmd(IDM_EXIT, []()
		{
			::PostQuitMessage(0);
		});
		 
		mainWnd.taskbar.onClose([this](std::wstring id) 
		{
			LRESULT lr = ::SendMessage(mainWnd.taskbar.hWnd(id), WM_CLOSE, 0, 0);
		});

		mainWnd.taskbar.onActivate([this](std::wstring id)
		{
			::SendMessage(mainWnd.mdi_client(), WM_MDIACTIVATE, (WPARAM)mainWnd.taskbar.hWnd(id), 0);
			::SetActiveWindow(*mainWnd);
		});
	}

private :

	bool load(std::wstring filename)
	{
		HWND hWnd = nullptr;
		bool success = false;

		std::wstring ext = mtl::path(filename).ext();

		if (ext == L".bmp" || ext == L".jpeg" || ext == L".jpg" || ext == L".gif" || ext == L".png")
		{
			auto& imgWnd = mainWnd.create_mdi_child<ImgWnd>(filename.c_str(), IDC_MULTIPAD_EDIT);
			imgWnd.show();

			success = imgWnd.load(filename);
			hWnd = *imgWnd;

			imgWnd.onDestroy([this, id = imgWnd.id]()
			{
				mainWnd.taskbar.remove(id);
			});

			imgWnd.onActivate([this, id = imgWnd.id](mtl::mdi_child* mdi)
			{
				mainWnd.taskbar.activate(id);
			});

			mainWnd.taskbar.add(imgWnd .id, *imgWnd, mtl::path(filename).filename());
		}
		else
		{
			auto& editWnd = mainWnd.create_mdi_child<EditWnd>(filename.c_str(), IDC_MULTIPAD_EDIT);
			editWnd.show();

			success = editWnd.load(filename);
			hWnd = *editWnd;

			editWnd.onDestroy([this, id = editWnd.id]()
			{
				mainWnd.taskbar.remove(id);
			});

			editWnd.onActivate([this, id = editWnd.id](mtl::mdi_child* mdi)
			{
				mainWnd.taskbar.activate(id);
			});

			mainWnd.taskbar.add(editWnd .id,*editWnd, mtl::path(filename).filename());

		}
		if (success)
		{
			mainWnd.statusBar.set_text(filename);
		}

		::InvalidateRect(hWnd, 0, TRUE);
		::UpdateWindow(hWnd);
		return success;
	}

};


// go WinMain, go!

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	mtl::STA enter;

	mtl::application app(hInstance);

	MultipadController controller;

	return app.run();
}
