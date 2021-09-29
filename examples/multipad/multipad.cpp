
#include "multipad.h"

class EditWnd : public mtl::mdi_child
{
public:

	mtl::edit_ctrl editCtrl;
	mtl::default_layout layout;

	EditWnd(int menuId)
		: mtl::mdi_child(menuId)
	{
		fontDesc_ = mtl::font_desc(L"Lucida Console", 18);
		font_ = fontDesc_.create();
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

class ImgWnd : public mtl::mdi_child
{
public:

	ImgWnd(int menuId)
		: mtl::mdi_child(menuId)
	{
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


// main (frame) window

class MainWindow : public mtl::mdi_frame
{
public:

	mtl::menu			menu;

	mtl::status_bar		statusBar;
	mtl::splitter		splitter;
	mtl::explorer_tree  tree;

	mtl::event<int()>	onCmd;

	MainWindow()
	{}

	MainWindow(UINT viewMenu, UINT firstChildId)
		: mtl::mdi_frame(viewMenu, firstChildId)
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
		}
		else
		{
			auto& editWnd = mainWnd.create_mdi_child<EditWnd>(filename.c_str(), IDC_MULTIPAD_EDIT);
			editWnd.show();

			success = editWnd.load(filename);
			hWnd = *editWnd;
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
