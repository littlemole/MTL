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
			loveKind,
			artValues,
			loveArt
		);
	}

};

class MainWindow : public mtl::window<MainWindow>
{
public:

	mtl::button butt;
	AboutDlg aboutDlg;


	MainWindow()
	{
		// set menu implicitly on window class
		mtl::wc<MainWindow>().set_menu(IDC_HELLOWORLD);

		// peload jpeg image for display
		mtl::the_bitmap_cache().load(IDI_JPEG, CLSID_WICJpegDecoder, L"JPEG" );

		// create and show window
		create(L"Hello Worls", WS_OVERLAPPEDWINDOW);
		show();

	}

	virtual LRESULT wm_create() override
	{
		// create chld button
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
		HBITMAP bmp = mtl::the_bitmap_cache().get(IDI_JPEG, 500, 314);

		mtl::dc dc(hdc);
		dc.bit_blit(bmp, 0, 0);

		return 0;
	}

	virtual LRESULT wm_size(RECT& clientRect) override
	{
		RECT r = { 0,0,0,0 };
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

    mtl::application app(hInstance);

	MainWindow mainWnd;

	return app.run(*mainWnd, IDC_HELLOWORLD);

}


