// helloworld.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "helloworld.h"
#include "mtl/persist/json.h"
#include "mtl/win32/io.h"

class AboutDlg : public mtl::dialog
{
public:

	mtl::dlg_value< IDC_CHECK_LOVE, bool> hasLove;

	mtl::dlg_value< IDC_LIST_LOVE, std::vector<std::wstring>> kindValues;
	mtl::dlg_selection< IDC_LIST_LOVE, std::wstring> loveKind;

	mtl::dlg_value< IDC_COMBO_LOVE, std::vector<std::wstring>> artValues;
	mtl::dlg_selection< IDC_COMBO_LOVE, std::wstring> loveArt;

	AboutDlg()
	{
		hasLove = true;
		kindValues = { L"a bit", L"some what", L"yuh cool", L"ETERNAL LOVE" };
		artValues = { L"Red", L"Green", L"Blue" };

		binding(
			hasLove,
			kindValues,
			loveKind,
			artValues,
			loveArt
		);
	}

};

void load_resource_ids();



class MainWindow : public mtl::window<MainWindow>
{
public:

	mtl::menu	menu;
	mtl::button butt;
	mtl::list_view listview;
	AboutDlg aboutDlg;

	mtl::list_view::data_src data_source;

	MainWindow()
	{
		menu.create();
		mtl::menu_builder mb(menu);
		mb.add({{
			IDM_FILE, {{
				IDM_MENU_A,{
					{IDM_MENU_B },
					{IDM_MENU_C }
				}},
				{ IDM_MENU_REFRESH },
				{ IDM_EXIT }
			}},
			{ IDM_HELP, {{ IDM_ABOUT}} }
		});

		// set menu implicitly on window class
		//mtl::wc<MainWindow>().set_menu(IDC_HELLOWORLD);

		// preload jpeg image for display
		mtl::the_bitmap_cache().load(IDI_JPEG, CLSID_WICJpegDecoder, L"JPEG" );

		// create and show window
		create(L"Hello World", WS_OVERLAPPEDWINDOW,0,*menu);
		show();

	}
	
	void enum_clipboard()
	{
		auto formats = mtl::clipboard::enumerate();
		
		std::vector<std::vector<std::wstring>> rows;

		int i = 0;
		for (auto& f : formats) 
		{			
			std::wstring preview;
			switch (f.format_id)
			{
			case CF_TEXT :
			{
				preview = mtl::to_wstring(mtl::clipboard::as_string(f.format_id));
				break;
			}
			case CF_UNICODETEXT:
			{
				preview = mtl::clipboard::as_wstring(f.format_id);
				break;
			}
			case CF_OEMTEXT:
			{
				preview = mtl::to_wstring(mtl::clipboard::as_string(f.format_id));
				break;
			}
			}

			std::vector<std::wstring> row { std::to_wstring(f.format_id), f.description, preview };
			rows.push_back(row);
			i++;
		}
		
		data_source.set({ L"ID", L"Format", L"Preview" }, rows);

		data_source.attach(listview);
		
	}
	
	virtual LRESULT wm_create() override
	{
		// create chld button
		RECT r = client_rect();
		listview.create(IDC_BUTT, L"click me", handle, r, WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL| LVS_EDITLABELS, LVS_EX_TRANSPARENTBKGND|LVS_EX_JUSTIFYCOLUMNS|LVS_EX_CHECKBOXES| LVS_EX_AUTOSIZECOLUMNS | LVS_EX_FULLROWSELECT);// LVS_EX_AUTOSIZECOLUMNS);
		//LVS_EDITLABELS

		enum_clipboard();

		listview.onActivate([this](NMITEMACTIVATE* nmia) 
		{
			::MessageBox(handle, listview.get_item(nmia->iItem).c_str(), L"activate", 0);
		});

		listview.onClick([this](NMITEMACTIVATE* nmia)
		{
			set_text(listview.get_item(nmia->iItem));
		});

		return 0;
	}

	virtual LRESULT wm_command(int id) override
	{
		switch (id)
		{
			case IDM_ABOUT:
			{
				// show about dialog
				LRESULT r = aboutDlg.show_modal(IDD_ABOUTBOX, handle);
				if (r == IDOK)
				{
					if (aboutDlg.hasLove)
					{
						std::wstring love = aboutDlg.loveKind;
						std::wstring art = aboutDlg.loveArt;

						std::wstringstream woss;
						woss << "I love you " << art << L" you too";
						::MessageBox(handle, woss.str().c_str(), love.c_str(),0);
					}
				}
				break;
			}
			case IDM_MENU_REFRESH:
			{
				enum_clipboard();
				break;
			}
			case IDM_MENU_B:
			{
				data_source[0][0] = L"OVERRIDE";
				break;
			}
			case IDM_MENU_C:
			{
				::MessageBox(handle, data_source[0][0].value.c_str(), L"VALUE", 0);
				break;
			}
			case IDM_EXIT:
			{
				// kill ourselves
				destroy();
				break;
			}
		}

		return 0;
	}

	virtual LRESULT wm_draw(HDC hdc, RECT& bounds) override
	{
		HBITMAP bmp = mtl::gui().bitmap(IDI_JPEG);// , 500, 314);

		mtl::dc dc(hdc);
		dc.bit_blit(bmp, 0, 0);

		return 0;
	}

	virtual LRESULT wm_size(RECT& clientRect) override
	{
		RECT r = { 0,0,0,0 };
		listview.move(clientRect);
//		do nothing here
//		layout.do_layout(clientRect, r);
		return 0;
	}

	virtual LRESULT wm_destroy() override
	{
		// over and out
		::PostQuitMessage(0);

		return 0;
	}
};


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    mtl::STA enter;
	mtl::Win32CommonControls cc;

    mtl::application app(hInstance);

	MainWindow mainWnd;

	mtl::accelerators acc;
	acc.create({ { 'T', IDM_ABOUT, FCONTROL | FVIRTKEY } });
	return app.run( acc);

}


