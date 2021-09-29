
#include "mtlExplorer.h"


// main (frame) window

class MainWindow : public mtl::window<MainWindow>
{
public:

	mtl::event<int()>				onCmd;
	mtl::default_layout				layout;

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

    virtual LRESULT wm_size(RECT& clientRect) override
    {
        RECT r = { 0,0,0,0 };
        layout.do_layout(clientRect, r);
        return 0;
    }

    virtual LRESULT wm_destroy() override
    {
		onCmd.fire(IDM_EXIT);
        return 0;
    }
};

// View constructs Widget tree and Layout
// View owns all Win32 widgets

class EditorView
{
public:

	MainWindow				mainWnd;
	mtl::shell::folder_view	folderWnd;

	mtl::menu				menu;
	mtl::status_bar			statusBar;
	mtl::splitter			splitter;
	mtl::explorer_tree		tree;

	EditorView()
		: menu(IDC_MTLEXPLORER)
	{
		// create main window
		HWND hWnd = mainWnd.create(L"MTL Explorer", WS_OVERLAPPEDWINDOW,0,*menu);

		// get main window client rect dimension
		RECT r1;
		::GetClientRect(hWnd, &r1);

		// create normal child windows, initially sized to full client rect
		// layout will resize them approp
		tree.create(IDC_FOLDER_TREE, hWnd, r1, WS_CHILD | WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
		folderWnd.create(IDC_FOLDER_VIEW, L"", hWnd, r1, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		statusBar.create(IDC_STATUS, L"", hWnd, r1);

		// create splitter at given initial position and given width
		r1.left = 200;
		r1.right = 208;
		splitter.create(IDC_SPLITTER, hWnd, r1, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

		// set the layout
		mainWnd.layout = {							// Default Layout with a
			{										//  vector<Widget> of
				{									//   Widget holding a
					*statusBar,						//    statusbar
					mtl::layout::style::SOUTH		//    docked to SOUTH
				},									//  and
				{									//   Widget holding a
					{								//    SplitterLayout with
						*tree,						//     Left Splitter child window: tree,
						*splitter,					//     Splitter Window Separator and
						*folderWnd					//     Right Splitter child wind,
					},  
					mtl::layout::style::FILL		//   filling remaining space
				}
			}
		};

		::ShowWindow(*mainWnd, SW_SHOW);
		::UpdateWindow(*mainWnd);
	}
};

// Controller handles User Input
// Controller owns Model and View

class EditorController
{
public:

	EditorView view;

	EditorController()
	{
		view.mainWnd.onCmd(ID_FILE_OPEN, [this]()
		{
			mtl::pick_folder folderPicker;
			std::wstring path = folderPicker.choose(view.mainWnd.handle);
			if (!path.empty())
			{
				view.folderWnd.path(path);
			}
		});

		view.mainWnd.onCmd(ID_EDIT_CUT, [this]()
		{
			view.folderWnd.cut();
		});

		view.mainWnd.onCmd(ID_EDIT_COPY, [this]()
		{
			view.folderWnd.copy();
		});

		view.mainWnd.onCmd(ID_EDIT_PASTE, [this]()
		{
			view.folderWnd.paste();
		});

		view.mainWnd.onCmd(ID_EDIT_PROPERTIES, [this]()
		{
			view.folderWnd.properties();
		});

		view.mainWnd.onCmd(ID_EDIT_EXECUTE, [this]()
		{
			view.folderWnd.execute();
		});

		view.mainWnd.onCmd(ID_VIEW_PARENTDIRECTORY, [this]()
		{
			view.folderWnd.up_dir();
		});

		view.mainWnd.onCmd(ID_VIEW_SHOW_DIRVIEW, [this]()
		{
			view.tree.show(SW_SHOW);
			view.mainWnd.relayout();
		});

		view.mainWnd.onCmd(ID_VIEW_HIDE_DIRVIEW, [this]()
		{
			view.tree.show(SW_HIDE);
			view.mainWnd.relayout();
		});

		view.mainWnd.onCmd(ID_VIEW_SHOW_TREE_ROOT, [this]()
		{
			view.tree.set_root();// L"shell:::{B4BFCC3A-DB2C-424C-B029-7FE99A87C641}");// L"C:\\");
		});

		view.mainWnd.onCmd(ID_VIEW_SHOW_TREE_DIR, [this]()
		{
			std::wstring path = view.folderWnd.path();
			view.tree.set_root(path);
		});
		view.tree.onSelect([this](std::wstring path)
		{
			mtl::path p(path);
			if (!p.is_dir())
			{
				view.mainWnd.set_text(mtl::path(path).filename());
			}
			else
			{
				view.folderWnd.path(path);
			}
		});

		view.folderWnd.onPath([this](std::wstring path) 
		{
			if (mtl::path(path).is_dir())
			{
				view.statusBar.set_text(path);
				view.tree.show_item(path);
			}
		});

		view.folderWnd.onOpen([this](std::wstring path)
		{
			bool isShift = ::GetAsyncKeyState(VK_SHIFT);
			bool isControl = ::GetAsyncKeyState(VK_CONTROL);

			if (isShift || isControl)
			{
				/*
				mtl::OpenDlg dlg(*view.mainWnd);
				dlg.fileName(path);
				bool b = dlg.dlgOpen();
				if (b)
				{
					::MessageBox(*view.mainWnd, path.c_str(), L"OPEN", 0);
				}
				*/
				mtl::file_open_dialog dlg;
				dlg.path(path);
				if (S_OK == dlg.open(*view.mainWnd))
				{
					::MessageBox(*view.mainWnd, path.c_str(), L"OPEN", 0);
				}
			}
			mtl::shell::execute(path);
		});

		view.mainWnd.onCmd(IDM_ABOUT, [this]()
		{
			mtl::dialog dlg;
			dlg.show_modal(IDD_ABOUTBOX, view.mainWnd.handle);
		});

		view.mainWnd.onCmd(IDM_EXIT, []()
		{
			::PostQuitMessage(0);
		});
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

	mtl::OLE enter;

    mtl::application app(hInstance);

	EditorController controller;

    return app.run();
}
