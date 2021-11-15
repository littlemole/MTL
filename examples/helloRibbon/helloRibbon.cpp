// helloworld.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "helloRibbon.h"
#include "mtl/persist/json.h"
#include "mtl/win32/io.h"
#include "mtl/win/taskdlg.h"
#include "mtl/win/rib.h"

#include <uiribbon.h>
#include "ribbon.h"

class AboutDlg : public mtl::dialog
{
public:
	/*
	mtl::dlg_value< IDC_CHECK_LOVE, bool> hasLove;

	mtl::dlg_value< IDC_LIST_LOVE, std::vector<std::wstring>> kindValues;
	mtl::dlg_selection< IDC_LIST_LOVE, std::wstring> loveKind;

	mtl::dlg_value< IDC_COMBO_LOVE, std::vector<std::wstring>> artValues;
	mtl::dlg_selection< IDC_COMBO_LOVE, std::wstring> loveArt;
	*/
	AboutDlg()
	{
		/*
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
		*/
	}

};



class ContentWindow : public mtl::window<ContentWindow>
{
public:

	mtl::button butt;

	virtual LRESULT wm_create() override
	{
		// create chld button
		RECT r = { 0,0,120,40 };
		butt.create(IDC_BUTT, L"click me", handle, r, WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON);

		butt.onCommand(IDC_BUTT, [this]()
		{
			::MessageBox(handle, L"click", L"clack", 0);
		});

		return 0;
	}

	virtual LRESULT wm_draw(HDC hdc, RECT& bounds) override
	{
		HBITMAP bmp = mtl::gui().bitmap(IDI_JPEG);// , 500, 314);

		mtl::dc dc(hdc);
		dc.bit_blit(bmp, 0, 0);

		HBITMAP bmp2 = mtl::gui().bitmap(ID_CMD_EXIT);// , 500, 314);

		dc.alpha_blend(bmp2, 200, 200,64,64);

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
		return 0;
	}

};



class MainWindow : public mtl::window<MainWindow>
{
public:

	mtl::menu	menu;
	AboutDlg aboutDlg;
	ContentWindow contentWindow;

	mtl::default_layout	layout;
	mtl::ribbon ribbon;


	MainWindow()
	{
		mtl::the_bitmap_cache().load(IDI_JPEG, CLSID_WICJpegDecoder, L"JPEG");

		create(L"Hello World", WS_OVERLAPPEDWINDOW| WS_CLIPCHILDREN|WS_CLIPSIBLINGS,0,*menu);
		show();

	}

	virtual LRESULT wm_create() override
	{
		RECT cRect = client_rect();
		HWND hWnd = contentWindow.create(IDC_CONTENT, L"", handle, cRect, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS );

		layout = {							// Default Layout with a
			{										//  vector<Widget> of
/*				{									//   Widget holding a
					*statusBar,						//    statusbar
					mtl::layout::style::SOUTH		//    docked to SOUTH
				},									//  and
				*/
				{	
					*contentWindow,
					mtl::layout::style::FILL		//   filling remaining space
				}
			}
		};

		ribbon
			.init(handle)
			.value(ID_CMD_ZOOM, UI_PKEY_MinValue, mtl::decimal(1.0))//, 1))
			.value(ID_CMD_ZOOM, UI_PKEY_DecimalPlaces, 1)
			.layout(ID_CMD_SYNTAX, L"XXXXXXXXXXX")
			.load()
			.add(ID_CMD_SYNTAX, {
				{ 1, L"plain", ID_CMD_PLAIN },
				{ 2, L"html",0, L"img\\html-file.png"},
				{ 3, L"javascript" }
			})
			.select(ID_CMD_SYNTAX, 0);


		ribbon
			.on(ID_CMD_EXIT, [this] ()
			{
				::PostQuitMessage(0); 
			})
			.on(ID_CMD_NEW, [this] ()
			{
				if (ribbon.boolean(ID_CMD_NEW))
				{
					::MessageBox(handle, L"checked", L"toggle", 0);
				}
			})
			.on(ID_CMD_ZOOM, [this] () 
			{
				mtl::decimal d = ribbon.decimal_val(ID_CMD_ZOOM);

				double zoomValue = 0;
				::VarR8FromDec( &d, &zoomValue);
			
				std::wostringstream woss;
				woss << L"Zoom: " << zoomValue << L" " << d.Lo64;
				set_text(woss.str());
			})
			.on(ID_CMD_SYNTAX, [this] ()
			{
				UINT32 sel = ribbon.selected(ID_CMD_SYNTAX);
				if (sel != -1)
				{
					UINT32 cmd = ribbon.cmd(ID_CMD_SYNTAX);
					std::wostringstream woss;
					woss << L"Syntax: " << sel << " [" << cmd << "]";
					::MessageBox(handle, woss.str().c_str(), L"syntax", 0);
				}
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
					int result = IDCANCEL;

					mtl::task_dlg dlg;
					if (dlg
						.wnd(handle)
						.buttons(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON)
						.title(L"About")
						.instruction(L"My instruction")
						.content(L"CONTENT with a <a href=\"somewhere.com\">ref</a> to the void.")
						.add_radio_button({ 101, L"one"})
						.add_radio_button({ 102, L"two"})
						.add_button( {201, L"BUTT"})
						.verification(L"verify me")
						.expanded_title(L"expand details") 
						.expanded(L"o-hu")
						.icon(IDI_SMALL)
						.footer(L"footer")
						.footer_icon(IDI_SMALL)
						.onNavigate( [this] (std::wstring target)
						{
							::MessageBox(handle,target.c_str(), L"LINK", 0);
							return S_FALSE;
						})
						.onCreate( [&dlg] () 
						{
							dlg.disable(IDOK);
							dlg.disable(201);
						})
						.onChecked( [&dlg](bool checked) 
						{
							if (checked)
							{
								dlg.enable(IDOK);
								dlg.enable(201);
							}
						})
						.onClick([this](UINT id) 
						{
							::MessageBox(handle, L"BUTT", L"OK", 0);
							return S_OK;
						})
						.show() != IDCANCEL)
					{
						std::wostringstream woss;
						woss << L"button " << dlg.button << L" radio (" << dlg.radio << L") [" << dlg.checked << L"]";
						::MessageBox(handle, woss.str().c_str(), L"OK", 0);
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

	virtual LRESULT wm_size(RECT& clientRect) override
	{
		clientRect.top += ribbon.height();

		RECT r = { 0,0,0,0 };
		layout.do_layout(clientRect, r);
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

	mtl::accelerators acc(*mainWnd);
	acc.create({ { 'T', IDM_ABOUT, FCONTROL | FVIRTKEY } });
	mtl::accelerator() = acc;
	return app.run(mtl::accelerator());

}


