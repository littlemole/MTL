// helloworld.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "helloworld.h"

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
			artValues,
			loveKind,
			loveArt
		);
	}

};

class MainWindow : public mtl::window<MainWindow>
{
public:

	mtl::button butt;

	MainWindow()
	{
		mtl::the_bitmap_cache().load(IDI_JPEG, CLSID_WICJpegDecoder, L"JPEG" );
	}

	virtual LRESULT wm_create() override
	{
		RECT r = { 0,0,120,40 };
		butt.create(IDC_BUTT,L"click me", handle, r,WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON);

		butt.onCommand(IDC_BUTT, [this]()
		{
			::MessageBox(handle, L"click", L"clack", 0);
		});
		return 0;
	}

	virtual LRESULT wm_command(int id) override
	{
		switch (id)
		{
			case IDM_ABOUT:
			{
				AboutDlg dlg;
				LRESULT r = dlg.show_modal(IDD_ABOUTBOX, handle);
				if (r == IDOK)
				{
					if (dlg.hasLove)
					{
						std::wstring love = dlg.loveKind;
						std::wstring art = dlg.loveArt;

						std::wstringstream woss;
						woss << "I love you " << art << L" you too";
						::MessageBox(handle, woss.str().c_str(), love.c_str(),0);
					}
				}
				break;
			}
			case IDM_EXIT:
			{
				destroy();
				break;
			}
		}
		return 0;
	}

	virtual LRESULT wm_draw(HDC hdc, RECT& bounds) override
	{
		mtl::dc_view dcv(hdc);

		HBITMAP bmp = mtl::the_bitmap_cache().get(IDI_JPEG, 500, 314);
		dcv.bit_blit(bmp, 0, 0);
		return 0;
	}

	virtual LRESULT wm_size(RECT& clientRect) override
	{
		RECT r = { 0,0,0,0 };
//		layout.do_layout(clientRect, r);
		return 0;
	}

	virtual LRESULT wm_destroy() override
	{
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

    mtl::application app(hInstance);


	MainWindow mainWnd;

	// set menu implicitly on window class
	mtl::wc<MainWindow>().set_menu(IDC_HELLOWORLD);

	// specify menu explcitly:
	// mtl::menu menu(IDC_HELLOWORLD);
	
	mainWnd.create(L"Hello Worls", WS_OVERLAPPEDWINDOW, 0, 0);// *menu);
	mainWnd.show();

	return app.run(*mainWnd, IDC_HELLOWORLD);

}


