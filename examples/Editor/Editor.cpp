// Editor.cpp : Defines the entry point for the application.
//

#include "Editor.h"
#include "MTL/scintilla/sci.h"
#include "MTL/win32/monitor.h"
#include "MTL/win32/io.h"
#include "MTL/ole/dataobj.h"
#include "MTL/win32/arg.h"

#include "MTL/obj/impl.h"
#include "MTL/disp/disp.h"
#include "MTL/disp/variant.h"
#include "MTL/obj/localserver.h"
#include "MTL/obj/marshall.h"
#include "MTL/util/str.h"
#include "MTL/script/script.h"
#include "Editor_h.h"
#include <Uxtheme.h>

/*

App
 Model
  Config
   AppSettings
   EditorSettings
    EditorStyles
   MenuBarSettings
   Encodings
  Documents
  Services
   PersistService (XML serialization)
   FileService
   ScriptingService
   FileWatchMonitorService
 View
  MainWnd
   MenuBar
   Explorer
   Editor
   StatusBar
  Dialogs
  ContextMenu
  FileMenu
 Controller
  MenuCommands
  RunScript
  SciNotifications
 API

 WinMain
  - inits libs
  - runs App passing args

# base dependencies on shared_ptr if lifetime < exe

 App
  - pre-loads resources, if any
  - parses cli args
  - launches controller
  - owns API (COM Layer)
  - runs msg loop

 Controller owns View and Model
  View has Controller ref to call event handler to update model
  Model has View ref to update view
  Model does not know Controller
  Model or Model parts are passed to view for updaate
  View does not hold ref to Model

Model owns config and loads it at init
Model owns API to trigger event callbacks
View owns all visual win32 User/GDI objects, controls and dialogs

MainWnd owns all child widget windows

API (COM Layer) knows Controller and hence also has access to Model and View
- only weak references to Model or view
- requires deps to be shared_ptr based

dependencies
- metacpp -> msxml6
- rwp.exe (UAC)
- WebView2

only regfree com allowed.


*/

class Resources;
class ComponentModel;
class IO;
class Persistence;
class Configuration;
class Monitor;

class Services
{
public:


};


struct XmlReBarItem
{
	std::string id;
	std::string label;
	std::string style;
	std::string img;

};

template<>
struct meta::Data<XmlReBarItem>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("button"),
			member("id",&XmlReBarItem::id,meta::attribute()),
			"label", &XmlReBarItem::label,
			"style", &XmlReBarItem::style,
			"img", &XmlReBarItem::img
		);
	}
};


struct XmlBar
{
	std::string id;
	std::string label;
	std::string mode;
	int width;
	std::vector<XmlReBarItem> items;
};

template<>
struct meta::Data<XmlBar>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("bar"),
			member("id", &XmlBar::id, meta::attribute()),
			member("label", &XmlBar::label, meta::attribute()),
			member("mode", &XmlBar::mode, meta::attribute()),
			member("width", &XmlBar::width, meta::attribute()),
			"items", &XmlBar::items
		);
	}
};

struct XmlMenu
{
	std::string id;
	std::string label;
};


template<>
struct meta::Data<XmlMenu>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("menu"),
			member("id", &XmlMenu::id, meta::attribute()),
			member("label", &XmlMenu::label)
		);
	}
};

struct XmlReBar
{
	std::vector<XmlMenu> menu;
	std::vector<XmlBar> bar;
};

template<>
struct meta::Data<XmlReBar>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("rebar"),
			member("menu", &XmlReBar::menu),
			member("bar", &XmlReBar::bar)
		);
	}
};

struct XmlDocumentInfo
{
	std::string id;
	std::string title;
	std::string tooltip;
};

template<>
struct meta::Data<XmlDocumentInfo>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("documentInfo"),
			member("id", &XmlDocumentInfo::id),
			member("title", &XmlDocumentInfo::title),
			member("tooltip", &XmlDocumentInfo::tooltip)
		);
	}
};

struct XmlDocument
{
	XmlDocumentInfo info;
	std::string body;
};


template<>
struct meta::Data<XmlDocument>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("documentInfo"),
			member("info", &XmlDocument::info),
			member("body", &XmlDocument::body)
		);
	}
};


class Encodings
{
public:

	typedef std::pair<int, std::wstring> CodePage;
	typedef std::vector<CodePage>::iterator Iterator;

	Encodings()
	{
		//capture supported codepages. we add three pretty well-known to the list
		codePages_.push_back(CodePage(std::make_pair(CP_ACP, L"ANSI Default")));
		codePages_.push_back(CodePage(std::make_pair(CP_UTF8, L"UTF-8")));
		codePages_.push_back(CodePage(std::make_pair(CP_WINUNICODE, L"Unicode (UTF-16)")));

		const mtl::codepages& cps = mtl::code_pages();
		for (auto it = cps.begin(); it != cps.end(); it++)
		{
			codePages_.push_back(CodePage(std::make_pair((*it).first, (*it).second.second)));
		}
	}


	Iterator begin()
	{
		return codePages_.begin();
	}

	Iterator end()
	{
		return codePages_.end();
	}

	CodePage& item(int index)
	{
		return codePages_[index];
	}

	size_t index(int cp)
	{
		for (size_t i = 0; i < codePages_.size(); i++)
		{
			if (codePages_[i].first == cp)
			{
				return i;
			}
		}
		return -1;
	}

private:
	std::vector<CodePage> codePages_;
};

Encodings& encodings()
{
	static Encodings enc;
	return enc;
}

class MyStatusBar : public mtl::status_bar
{
public:

	mtl::font font;
	mtl::font fontSmall;
	mtl::combo_box comboBoxSyntax;
	std::wstring statusText;

	mtl::brush brush;

	void set_text(const std::wstring& str)
	{
		statusText = str;//SBT_OWNERDRAW
		send_msg(SB_SETTEXT, SBT_NOBORDERS, (LPARAM)statusText.c_str());
	}

	virtual LRESULT wm_create() override
	{
		//::SetWindowTheme(handle, L"", L"");
		mtl::font_desc fontDesc(L"Lucida Console", 15);
		font = fontDesc.create();

		mtl::font_desc fontDescSmall(L"Lucida Console", 12);
		fontSmall = fontDescSmall.create();

		RECT rc = client_rect();
		RECT rcb = rc;
		rcb.top = rc.top + 1;
		rcb.bottom = rc.bottom + 1;
		rcb.left = rc.right - 152;
		rcb.right = rc.right - 2;
		comboBoxSyntax.create(IDC_SYNTAX, handle, rcb, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER, WS_EX_RIGHT);
		::SetWindowTheme(*comboBoxSyntax, L"", L"");
		comboBoxSyntax.add(L"Plain", IDM_SYNTAX_PLAIN);
		comboBoxSyntax.add(L"Perl", IDM_SYNTAX_CPP);
		comboBoxSyntax.add(L"Java", IDM_SYNTAX_JAVA);
		comboBoxSyntax.add(L"JavaScript", IDM_SYNTAX_JS);
		comboBoxSyntax.select(1);
		comboBoxSyntax.set_font(*fontSmall);

		wm_size(rc);
		return 0;
	}

	virtual LRESULT wm_size(RECT& rc) override
	{
		RECT cbr = comboBoxSyntax.client_rect();
		int cbw = cbr.right - cbr.left;
		int cbh = cbr.bottom - cbr.top;
		RECT dest = { rc.right - cbw -2, rc.top +1, rc.right-2,rc.top + cbh+1 };
		comboBoxSyntax.move(dest);
		invalidate();
		RECT r = rc;
		r.right -= (dest.right - dest.left);
		return mtl::status_bar::wm_size(r);
	}
};


#define CONTROL_GROUP_ENC       2000
#define CONTROL_GROUP_EOL       2001
#define CONTROL_ENCODING        2003
#define CONTROL_EOL		        2004
#define CONTROL_SWITCH	        2005

#define OPENCHOICES					0
#define OPENCHOICES_OPEN			0
#define OPENCHOICES_OPEN_READONLY	1


class MyFileDialog : public mtl::file_open_dialog
{
public:
	MyFileDialog(int options)
		: mtl::file_open_dialog(options), readOnly_(false), encoding_(CP_ACP)
	{
		customize( [this] ( mtl::punk<IFileDialogCustomize> fdc) 
		{
			HRESULT hr = fdc->EnableOpenDropDown(OPENCHOICES);
			if (hr != S_OK)
				return;

			hr = fdc->AddControlItem(OPENCHOICES, OPENCHOICES_OPEN, L"&Open");
			if (hr != S_OK)
				return;

			hr = fdc->AddControlItem(OPENCHOICES, OPENCHOICES_OPEN_READONLY, L"Open &read-only");
			if (hr != S_OK)
				return;

			// Create a Visual Group.
			hr = fdc->StartVisualGroup(CONTROL_GROUP_ENC, L"Encoding");
			if (hr != S_OK)
				return;

			// Add a combo box
			hr = fdc->AddComboBox(CONTROL_ENCODING);
			if (hr != S_OK)
				return;

			hr = fdc->SetControlState(CONTROL_ENCODING, CDCS_VISIBLE | CDCS_ENABLED);
			if (hr != S_OK)
				return;

			// populate box
			DWORD i = 0;
			for (Encodings::Iterator it = encodings().begin(); it != encodings().end(); it++)
			{
				const Encodings::CodePage codePage = *it;
				hr = fdc->AddControlItem(CONTROL_ENCODING, i, codePage.second.c_str());
				if (hr != S_OK)
					return;
				i++;
			}

			// End the visual group.
			hr = fdc->EndVisualGroup();
		});
	}

	long encoding()
	{
		return encodings().item(encoding_).first;
	}

	bool readOnly()
	{
		return readOnly_;
	}

	void encoding(long enc)
	{
		encoding_ = (int)encodings().index(enc);
	}

private:
	long encoding_;
	bool readOnly_ = false;

};

class MySaveDialog : public mtl::file_save_dialog
{
public:
	MySaveDialog(int options)
		: mtl::file_save_dialog(options),  encoding_(CP_ACP)
	{
		customize([this](mtl::punk<IFileDialogCustomize> fdc)
		{

			HRESULT hr = fdc->AddRadioButtonList(CONTROL_SWITCH);
			if (hr != S_OK)
				return;

			hr = fdc->SetControlState(CONTROL_SWITCH, CDCS_VISIBLE | CDCS_ENABLED);
			if (hr != S_OK)
				return;

			fdc->AddControlItem(CONTROL_SWITCH, 0, L"UNIX");
			fdc->AddControlItem(CONTROL_SWITCH, 1, L"DOS");

			fdc->SetSelectedControlItem(CONTROL_SWITCH, 1);

			// Create a Visual Group.
			hr = fdc->StartVisualGroup(CONTROL_GROUP_ENC, L"Encoding");
			if (hr != S_OK)
				return;

			// Add a combo box
			hr = fdc->AddComboBox(CONTROL_ENCODING);
			if (hr != S_OK)
				return;

			hr = fdc->SetControlState(CONTROL_ENCODING, CDCS_VISIBLE | CDCS_ENABLED);
			if (hr != S_OK)
				return;

			// populate box
			DWORD i = 0;
			for (Encodings::Iterator it = encodings().begin(); it != encodings().end(); it++)
			{
				const Encodings::CodePage codePage = *it;
				hr = fdc->AddControlItem(CONTROL_ENCODING, i, codePage.second.c_str());
				if (hr != S_OK)
					return;
				i++;
			}

			fdc->SetSelectedControlItem(CONTROL_ENCODING, 1);
			
			/*
			// End the visual group.
			hr = fdc->EndVisualGroup();
			
			// Create another Visual Group.
			hr = fdc->StartVisualGroup(CONTROL_GROUP_EOL, L"EOL Mode");
			if (hr != S_OK)
				return;
			*/
			

			// End the visual group.
			hr = fdc->EndVisualGroup();
			
		});
	}

	long encoding()
	{
		return encodings().item(encoding_).first;
	}

	void encoding(long enc)
	{
		encoding_ = (int)encodings().index(enc);
	}

private:
	long encoding_;

};


class MenuBar : public mtl::window<MenuBar>
{
public:

	HBRUSH brush;
	mtl::font				font;
	mtl::font				smallFont;
	mtl::tool_bar			mainBar;
	mtl::combo_box			comboBoxSyntax;
	mtl::combo_box			comboBoxEncoding;
	mtl::button				eolButton;

	mtl::default_layout		layout;

	MenuBar()
	{
		COLORREF col = ::GetSysColor(COLOR_BTNFACE);
		brush = ::CreateSolidBrush(col);
		//mtl::windowClass<MenuBar>().hbrBackground = brush;
	}


	virtual LRESULT wm_erase_background(WPARAM wParam) override
	{
		if (!colorTheme || !colorTheme->enabled()) return 0;

		RECT cr = client_rect();
		mtl::dc dcv((HDC)wParam);
		dcv.fill_rect(cr, colorTheme->bkg_brush());
		return 1;// ::DefWindowProc(handle, WM_ERASEBKGND, wParam, 0);
	}

	virtual LRESULT wm_paint() override
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(handle, &ps);
		::EndPaint(handle, &ps);
		return 0;
	}

	virtual LRESULT wm_create() override
	{
		// get dpi
		int iDpi = ::GetDpiForWindow(this->handle);
		float scale = (float)( iDpi / 96.0);

		mtl::font_desc fontDesc(L"Lucida Console", 14);
		font = fontDesc.create();

		mtl::font_desc fontDescSmall(L"Lucida Console", 12);
		smallFont = fontDescSmall.create();

		RECT r = client_rect();


		RECT r2 = r;
		r2.left = 0;
		r2.right = int(22 * scale);
		//r2.right = r2.left + 60;
		r2.bottom = int(28 * scale);
		mainBar.create(IDC_TOOLBAR, handle, r2, WS_VISIBLE|CCS_NODIVIDER | CCS_NOPARENTALIGN | TBSTYLE_FLAT | WS_CHILD , TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS);
		mainBar.set_font(*font);
		mainBar.set_color_theme(colorTheme);
		mainBar
			.padding(1, 1)
			.spacing(10, 1)
			.indent(10)
			.add_text_button(IDC_REBAR, L"File", BTNS_CHECK);



		RECT rSum = { r.left,r.top,r.right,28 };

		RECT rcb = { 0,0,150,32 };
		comboBoxSyntax.create(IDC_SYNTAX, handle, rcb, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER, WS_EX_RIGHT);
		comboBoxSyntax.add(L"Plain", IDM_SYNTAX_PLAIN);
		comboBoxSyntax.add(L"Perl", IDM_SYNTAX_CPP);
		comboBoxSyntax.add(L"Java", IDM_SYNTAX_JAVA);
		comboBoxSyntax.add(L"JavaScript", IDM_SYNTAX_JS);
		comboBoxSyntax.select(1);
		comboBoxSyntax.set_color_theme(colorTheme);
		comboBoxSyntax.set_font(*font);

		RECT rcb2 = { 0,0,200,32 };
		comboBoxEncoding.create(IDC_ENCODING, handle, rcb2, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER);
		Encodings& enc = encodings();
		auto it = enc.begin();
		for (++it; it != enc.end(); it++)
		{
			comboBoxEncoding.add((*it).second, (*it).first);
		}
		comboBoxEncoding.width(400);
		comboBoxEncoding.select(0);
		comboBoxEncoding.set_font(*font);

		RECT rcbb = { 0,0,60,10 };
		eolButton.create(IDC_EOL, handle, rcbb, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON);
		eolButton.set_text(L"DOS");
		eolButton.set_bitmap(mtl::the_bitmap_cache().get(L"msdos.png", 12,12));
		eolButton.set_font(*smallFont);

		layout = {
			{
				{ *eolButton, mtl::layout::style::EAST },
				{ *comboBoxSyntax,  mtl::layout::style::EAST},
				{ *comboBoxEncoding,  mtl::layout::style::EAST},
				{ *mainBar, mtl::layout::style::FILL}
			}
		};

		move(rSum);
		return 0;
	}

	virtual LRESULT wm_size(RECT& clientRect) override
	{
		RECT r = { 0,0,0,0 };
		layout.do_layout(clientRect, r);
		return 0;
	}

private:

};


class MainWindow : public mtl::window<MainWindow>
{
public:

	mtl::event<int()>				onCmd;
	mtl::event<void(FINDREPLACE*)>	onFind;

	mtl::default_layout				layout;
	//MenuBar& menuBar;

	mtl::menu						menu;
	mtl::font						menuFont;
	COLORREF						menuTextColor = RGB(0x35, 0x39, 0x44);
	COLORREF						menuBkgColor = RGB(0x55, 0x59, 0x64);
	COLORREF						menuSelectedColor = RGB(0xEA, 0xEA, 0xEA); 
	mtl::brush						bkgBrush;
	mtl::brush						txtBrush;
	mtl::brush						selectBrush;

	int								padding = 6;

	MainWindow()
	{
		mtl::font_desc fontDesc(L"Lucida Console", 14);
		menuFont = fontDesc.create();

		bkgBrush = mtl::brush(menuBkgColor);
		txtBrush = mtl::brush(menuTextColor);
		selectBrush = mtl::brush(menuSelectedColor);
	}

	virtual LRESULT wm_command(int id) override
	{
		onCmd.fire(id);
		return 0;
	}

	virtual LRESULT wm_erase_background(WPARAM wParam) override
	{
		return 1;
	}

	virtual LRESULT wm_size(RECT& clientRect) override
	{
		RECT rp = { 0,0,0,0 };
		layout.do_layout(clientRect, rp);
		return 0;
	}

	virtual LRESULT wm_destroy() override
	{
		onCmd.fire(IDM_EXIT);
		return 0;
	}

	virtual LRESULT wm_search(FINDREPLACE* fr) override
	{
		onFind.fire(fr);
		return 0;
	}

	virtual LRESULT wm_create() override
	{
		//::SetWindowTheme(handle, L"", L"");
		return 0;
	}

};



class EditorView
{
public:


	MainWindow				mainWnd;
	//MenuBar					menuBar;
	mtl::tab_ctrl			tabControl;
	MyStatusBar				statusBar;
	mtl::splitter			splitter;
	//mtl::ExplorerTree		tree;
	//std::map<int,mtl::tool_bar*> toolBars;
	mtl::bitmap				bitmap;
	mtl::image_list			imageList;
	mtl::font				font;
	mtl::font				smallFont;
	//mtl::rebar				reBar;
	mtl::monitor			monitor;
	mtl::tool_tip			tooltip;
	mtl::search_dlg			searchDlg;
	//mtl::button				theButton;
	mtl::tool_bar			toolBar;

	// events
	mtl::event<int()>			 onBarCmd;
	mtl::event<UINT(NMTOOLBAR*)> onBarNotify;

	mtl::punk<mtl::default_drop_target> dropTarget;

	std::shared_ptr<mtl::color_theme>	colorTheme;

	COLORREF						textColor = RGB(0xEA, 0xEA, 0xEA);
	COLORREF						bkgColor = RGB(0x35, 0x39, 0x44);
	COLORREF						selectedColor = RGB(0x55, 0x59, 0x64);
	COLORREF						selectedBkgColor = RGB(0xEA, 0xEA, 0xEA);


	std::shared_ptr<mtl::scintilla_wnd> createEditorWnd()
	{
		auto scintilla = std::make_shared<mtl::scintilla_wnd>();

		RECT tr = tabControl.display_rect();

		scintilla->create(IDC_SCINTILLA, *tabControl, tr, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		scintilla->set_code_page(CP_UTF8);
		scintilla->load_xml(mtl::path_to_self_directory(L"\\styles.xml"));
		scintilla->set_mode(SCLEX_CPP);
		scintilla->set_drop_target(*dropTarget);

		scintilla->onNotify(SCN_MODIFIED, [this](NMHDR* hdr)
		{
			//::MessageBox(*mainWnd, L"sci mod", L"sci", 0);
		});

		return scintilla;
	}

	void setUpAssets(const wchar_t* path)
	{
		mtl::the_bitmap_cache().img_path(path);

		// prepare menu command labels, ids and images
		mtl::gui().add({
			{ MTL_ID(ID_FILE), L"File", L"document.png" },
			{ MTL_ID(ID_EDIT), L"Edit", L"edit-copy.png" },
			{ MTL_ID(ID_HELP), L"Help", L"info.png" },
		//	{ MTL_ID(IDM_EXIT), L"Exit", L"application-exit.png" },
			{ MTL_ID(IDM_ABOUT), L"About", L"Help-browser.png" },
			{ MTL_ID(ID_FILE_OPEN), L"Open File", L"document-open.png" },
			{ MTL_ID(ID_FILE_OPEN_DIR), L"Open File", L"document-open.png" },
			{ MTL_ID(ID_FILE_NEW), L"Save File", L"document-new.png" },
			{ MTL_ID(ID_FILE_SAVE), L"Save File As", L"document-save.png" },
			{ MTL_ID(ID_FILE_SAVE_AS), L"New File", L"document-save-as.png" },
			{ MTL_ID(ID_FILE_PRINT), L"Print File", L"document-print.png" },
			{ MTL_ID(ID_EDIT_CUT), L"Cut", L"edit-cut.png" },
			{ MTL_ID(ID_EDIT_COPY), L"Copy", L"edit-copy.png" },
			{ MTL_ID(ID_EDIT_PASTE), L"Paste", L"edit-paste.png" },
			{ MTL_ID(ID_EDIT_UNDO), L"Undo", L"edit-cut.png" },
			{ MTL_ID(ID_EDIT_REDO), L"Redo", L"edit-redo.png" },
			{ MTL_ID(ID_EDIT_COLOR), L"Insert Color", L"alien.png" },
			{ MTL_ID(ID_EDIT_FIND), L"Find", L"edit-undo.png" },
			{ MTL_ID(ID_EDIT_REPLACE), L"Replace", L"edit-find-replace.png" },
			{ MTL_ID(ID_EDIT_FIND_REGEX), L"Find Regex", L"edit-find.png" },
			{ MTL_ID(ID_EDIT_REPLACE_REGEX), L"Replace Régex", L"edit-find-replace.png" },
			{ MTL_ID(ID_EDIT_RUN), L"Run", L"alien.png" },
			{ MTL_ID(ID_VIEW_SHOW_DIR), L"Show Explorer Tree", L"folder.png" }

		});
	}

	EditorView()
		: /* mainWnd(menuBar), / *menu(IDC_EDITOR), */ imageList(16, 16)
	{
		setUpAssets(mtl::path_to_self_directory(L"\\img").c_str());

		dropTarget = mtl::drop_target(tabControl.dragTabFormat);

		monitor.watch(mtl::path_to_self_directory(L"\\img"));
		monitor.onFileChanged( [this](std::wstring file)
		{
			::MessageBox( *mainWnd, file.c_str(), L"Monitor", 0);
			monitor.stop();
		});

		mtl::font_desc fontDesc(L"Lucida Console", 14);
		font = fontDesc.create();

		mtl::font_desc fontDescSmall(L"Lucida Console", 12);
		smallFont = fontDescSmall.create();

		colorTheme = std::make_shared<mtl::color_theme>(
			*font, 
			textColor,
			bkgColor,
			selectedColor,
			selectedBkgColor
		);

		mainWnd.menu.create();
		mtl::menu_builder mb(mainWnd.menu, 32, 32);
		mb.theme = colorTheme;
		mb.add({
			{ ID_FILE, {
					{ID_FILE_NEW},
					{ID_FILE_OPEN},
					{ID_EDIT_CUT, {
							{ID_EDIT_COPY, true},
							{ID_EDIT_PASTE} 
					}},
					{IDM_EXIT}
			}}
		});
		//mainWnd.menu.item(ID_EDIT_COPY).checked = true;

		// create main window
		HWND hWnd = mainWnd.create(L"MTL Editor", WS_OVERLAPPEDWINDOW, 0, *mainWnd.menu);

		HICON hIcon = mtl::shell::file_icon(L"C:\\test.txt");
		mainWnd.set_icon(hIcon);

		// get main window client rect dimension
		RECT r1;
		::GetClientRect(hWnd, &r1);

		RECT tbr = { 0,0,48,48 };
		toolBar.create(IDC_TOOLBAR, *mainWnd, tbr, 
			CCS_NODIVIDER | CCS_NOPARENTALIGN | WS_CHILD  | TBSTYLE_TOOLTIPS);
		toolBar.add_button(IDM_NEW);
		toolBar.add_button(IDM_OPEN);
		toolBar.add_button(IDM_SAVE);
		toolBar.add_button(IDM_SAVE_AS);
		toolBar.set_color_theme(colorTheme);

		//RECT tbr = r1;
		//tbr.right = tbr.left + 32;
		//tbr.bottom = tbr.top + 32;

		/*
		reBar.create(IDC_REBAR, hWnd, r1,
			CCS_NOPARENTALIGN|// RBS_AUTOSIZE| //RBS_FIXEDORDER
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |
			WS_CLIPCHILDREN | //RBS_VARHEIGHT |
			CCS_NODIVIDER | RBS_BANDBORDERS
		);
		reBar.set_color_theme(colorTheme);
		
		menuBar.set_color_theme(colorTheme);
		menuBar.create(IDC_SELECTOR, *mainWnd, r1, WS_CHILD  | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

		reBar.addBar(L"", IDC_REBAR_EOL, *menuBar, -1, RBBS_GRIPPERALWAYS);// RBBS_NOGRIPPER | RBBS_BREAK);// RBBS_TOPALIGN);// | RBBS_NOGRIPPER); //RBBS_GRIPPERALWAYS); //RBBS_HIDETITLE
		

		RECT rb{ 0,0,96,48 };
		theButton.create(ID_EDIT_RUN, *reBar, rb);// , WS_CHILD | WS_BORDER | BS_NOTIFY | BS_FLAT | BS_TEXT | BS_OWNERDRAW);
		::SetWindowTheme(*theButton, L"", L"");
		theButton.set_color_theme(colorTheme);
		theButton.set_text(L"BUTT");
		theButton.set_font(*font);

		reBar.addBar(L"", IDC_REBAR_ENCODING, *theButton, -1,  RBBS_GRIPPERALWAYS);// RBBS_NOGRIPPER | RBBS_BREAK);// RBBS_TOPALIGN);// | RBBS_NOGRIPPER); //RBBS_GRIPPERALWAYS); //RBBS_HIDETITLE

		load_xml(ID_MODE_SIMPLE);
		*/
		RECT tabRect = r1;
		tabControl.create(IDC_TABCONTROL, L"", *mainWnd, tabRect, TCS_FLATBUTTONS | TCS_TABS | CCS_NODIVIDER | CCS_NOPARENTALIGN | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TCS_OWNERDRAWFIXED);

		//tabControl.add({ L"dummy",L"id",L"tt" });
		tabControl.enable_dragdrop();
		//tabControl.setFont(*font);
		tabControl.set_color_theme(colorTheme);
		//tabControl.tooltip.setFont(colorTheme->font);

		mtl::icon closeIcon = mtl::shell::stock_icon(SIID_DELETE);
		tabControl.enable_icon(*closeIcon);

		//RECT tr = tabControl.displayRect();

		statusBar.create(IDC_STATUSBAR, L"Status", hWnd, r1, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CCS_NOPARENTALIGN);// | CCS_NODIVIDER);
		statusBar.set_color_theme(colorTheme);
		statusBar.comboBoxSyntax.set_color_theme(colorTheme);

		// create splitter at given initial position and given width
		r1.left = 200;
		r1.right = 208;

		// set the layout
		mainWnd.layout = {							// Default Layout with a
			{										//  vector<Widget> of
				{
					*toolBar,
					mtl::layout::style::NORTH
				},
				/*
				{
					*toolBar,
					mtl::Layout::Style::NORTH
				},
				*/
				{									//   Widget holding a
					*statusBar,						//    statusbar
					mtl::layout::style::SOUTH		//    docked to SOUTH
				},									//  and
				{
					*tabControl,
					mtl::layout::style::FILL
				}
			}
		};

		mainWnd.show();
		mainWnd.update();
		
		tooltip.create(IDC_TOOLTIP, *mainWnd, r1, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP);
		
		tooltip.onNotify(TTN_GETDISPINFO, [this](NMHDR* nmhdr) 
		{
			HWND hwndFrom = nmhdr->hwndFrom;
			UINT_PTR id = nmhdr->idFrom;
			NMTTDISPINFO* di = (NMTTDISPINFO*)nmhdr;
			di->lpszText = 0;
			di->hinst = 0;
			di->szText[0] = 0;
			di->lpszText = ((wchar_t*)L"STATIC CB");

			int i = tabControl.hit_test();
			if (i != -1)
			{
				mtl::tab_ctrl::tab& c = tabControl.item(i);
				di->lpszText = (wchar_t*)(c.tooltip.c_str());
				di->hinst = 0;
			}

		});
		
		//tooltip.add(*mainWnd, menuBar.eolButton);
		tooltip.add(statusBar, L"STATUS");

		tooltip.set_font(colorTheme->font());

		//statusBar.set_text(L"STATUS");
		statusBar.set_status({ L"STATUS", L"",L"INS", L"DOS", L"Line 221", L"Pos 18", L"XXXXXXXXXXXXXXXXXXXXXXXXXXXX"});
	}

};

// Controller handles User Input
// Controller owns Model and View

class EditorController;

class MTLEditorDocument :
	public mtl::implements<MTLEditorDocument(mtl::dual<IMTLEditorDocument>)>
{
public:

	std::wstring id;
	EditorController* controller = nullptr;

	MTLEditorDocument(const std::wstring& docId, EditorController* ctrl)
		: id(docId),controller(ctrl)
	{}

	virtual HRESULT __stdcall get_documentId(BSTR* docId) override;
	virtual HRESULT __stdcall get_filename(BSTR* fn) override;
	virtual HRESULT __stdcall get_content(BSTR* cnt) override;
};


class MTLEditorDocuments :
	public mtl::implements<MTLEditorDocuments(mtl::dual<IMTLEditorDocuments>)>
{
public:

	EditorController* controller = nullptr;

	MTLEditorDocuments(EditorController* ctrl)
		: controller(ctrl)
	{}

	virtual HRESULT __stdcall get_count(long* cnt) override;
	virtual HRESULT __stdcall item(VARIANT idx, IMTLEditorDocument** doc) override;
	virtual HRESULT __stdcall remove(VARIANT idx) override;
};


class MTLEditor :
	public mtl::implements<MTLEditor(mtl::dual<IMTLEditor>)>
{
public:

	mtl::variant value_;
	mtl::punk<IMTLEditorDocuments> documents;

	EditorController* controller;

	MTLEditor(EditorController* ctrl)
		: controller(ctrl)
	{
		documents = new MTLEditorDocuments(controller);
	}

	~MTLEditor()
	{
		::OutputDebugString(L"\r\nMTLEditor DEAD\r\n\r\n");
	}

	virtual HRESULT __stdcall SayHello(BSTR message) override
	{
		::MessageBox(0, message, L"Say Hello (EXE):", MB_ICONEXCLAMATION);

		return S_OK;
	}

	virtual HRESULT __stdcall put_Value(VARIANT value) override
	{
		value_ = value;
		return S_OK;
	}

	virtual HRESULT __stdcall get_Value(VARIANT* value) override
	{
		value_.copy_to(value);
		return S_OK;
	}

	virtual HRESULT __stdcall get_documents(IMTLEditorDocuments** docs) override
	{
		return documents.query_interface(docs);
	}
};

template<class ... Args>
class sinks : public mtl::sink<Args> ...
{
public:

};

class EditorController //: public sinks<int(),void(FINDREPLACE*),int(NMTOOLBAR*)> // mtl::sink<int()>
{
public:

//	mtl::punk<IMTLEditor> editor;

	std::wstring activeDocument;

	EditorView view;

	CLIPFORMAT doc_format;

//	std::set<DWORD> rotCookies;

	mtl::rotten<IMTLEditor> editor;

	std::map<std::wstring, std::shared_ptr<mtl::scintilla_wnd>> documents;
	std::map<std::wstring, std::wstring> documentPaths;
//	std::map<std::wstring, std::string> buffers;

	mtl::punk<mtl::active_script> scripting;

	mtl::sink<int()> onCmd;
	mtl::sink<void(FINDREPLACE*)> onFind;
	mtl::sink<UINT(NMTOOLBAR*)> onToolbar;


	~EditorController()
	{
	}

	void removeDoc(const std::wstring & id)
	{
		if (documents.count(id) == 0) return;

		auto scintilla = documents[id];
		scintilla->destroy();

		documents.erase(id);
		documentPaths.erase(id);

		view.tabControl.remove(id);
		if (documents.size() == 0)
		{
			activeDocument = L"";
			HICON hIcon = mtl::shell::file_icon(L"C:\\test.txt");
			view.mainWnd.set_icon(hIcon);
			//view.load_xml(ID_MODE_SIMPLE);
		}
		else
		{
			int index = view.tabControl.selected();
			index = index < 0 ? (int)documents.size()-1 : index;
			std::wstring firstId = view.tabControl.item(index).id;
			activeDocument = firstId;
			HICON icon = mtl::shell::file_icon(documentPaths[firstId]);
			view.mainWnd.set_icon(icon);
			//view.load_xml(ID_MODE_EDIT);
		}
	}

	void transferTab(const std::wstring& from, int to = -1)
	{
		auto info = mtl::split(from, ':');
		if (info.size() != 2)
		{
			return;
		}

		std::wstring instanceId = info[0];
		std::wstring documentId = info[1];

		mtl::punk<IMTLEditor> remoteEditor = mtl::rot::object<IMTLEditor>(__uuidof(MTLEditor), instanceId);
		if (!remoteEditor) return;

		mtl::punk<IMTLEditorDocuments> docs;
		HRESULT hr = remoteEditor->get_documents(&docs);
		if (!docs) return;

		mtl::punk<IMTLEditorDocument> doc;
		mtl::variant vId{ mtl::ole_char(from.c_str()) };
		docs->item(vId, &doc);
		if (!doc) return;

		mtl::bstr fn;
		doc->get_filename(&fn);
		mtl::bstr content;
		doc->get_content(&content);
		//::MessageBox(*view.mainWnd, fn.str().c_str(), content.str().c_str() , 0);

		std::wostringstream woss;
		woss << editor.id() << L":" << documentId;

		std::wstring newId = woss.str();

		std::string utf8 = content.to_string();
		openBytes(newId, fn.str(), utf8);

		docs->remove(vId);
	}

	void openBytes(const std::wstring& id, const std::wstring& path, const std::string& bytes)
	{
		activeDocument = id;
		documentPaths[id] = path;

		auto scintilla = view.createEditorWnd();
		documents[id] = scintilla;

		view.statusBar.set_text(path);
		scintilla->set_text(bytes);
		scintilla->show();

		scintilla->load_xml(mtl::path_to_self_directory(L"\\styles.xml"));
		scintilla->set_lexer(SCLEX_CPP);
		scintilla->set_keywords(0, scintilla->xmlStyleSets.styleSet[8].keywords[0]);
		scintilla->colorize();

		HICON icon = mtl::shell::file_icon(documentPaths[id]);
		view.mainWnd.set_icon(icon);

		std::wstring title = mtl::path(path).filename();
		view.tabControl.add( { title, path, id }, scintilla->handle );

		//view.mainWnd.postMsg(WM_LAYOUT, 0, 0);
		//view.load_xml(ID_MODE_EDIT);

		/*
		auto doc = view.scintilla.createDocument();
		activeDocument = id;
		documents[id] = doc;
		documentPaths[id] = path;
		view.scintilla.setDocument(*doc);
		buffers[id] = bytes;

		view.scintilla.setText(bytes);
		view.scintilla.show(SW_SHOW);

		//view.scintilla.load_xml(mtl::pathToSelfDirectory(L"\\styles.xml"));
		view.scintilla.setLexer(SCLEX_CPP);
		view.scintilla.setKeywords(0, view.scintilla.xmlStyleSets.styleSet[8].keywords[0]);

		view.scintilla.sendMsg(SCI_COLOURISE, (WPARAM)0, (LPARAM)-1);

		HICON icon = mtl::Shell::getFileIcon(documentPaths[id]);
		view.mainWnd.setIcon(icon);

		std::wstring title = mtl::Path(path).filename();
		view.tabControl.add({ title, path, id });

		//view.mainWnd.postMsg(WM_LAYOUT, 0, 0);
		view.load_xml(ID_MODE_EDIT);
		*/
	}

	void openPath(const std::wstring& path)
	{
		mtl::file file;
		if (file.open(path) == ERROR_SUCCESS)
		{
			if (file.async_content([this, path](DWORD e,std::string bytes)
			{
				std::wostringstream woss;
				woss << editor.id() << ":" << mtl::new_guid();
				std::wstring id = woss.str();

				openBytes(id, path, bytes);
			}) == ERROR_SUCCESS)
			{

			}
		}
	}

	EditorController(mtl::options& opt) : editor(__uuidof(MTLEditor))
	{
		doc_format = ::RegisterClipboardFormat(L"MTLEditorDocumentFormat");

		scripting = new mtl::active_script(L"JScript");

		mtl::punk<IMTLEditor> mtlEditor(new MTLEditor(this));
		editor = mtlEditor;

		if (opt.has(L"open"))
		{
			for (auto& uid : opt.args())
			{
				//::MessageBox(*view.mainWnd, uid.c_str(), L"open", 0);

				transferTab(uid, -1);
			}
		}


		//createTab(L"one", L"tooltip one");
		//createTab(L"two", L"tooltip two");
		//createTab(L"three", L"tooltip three");

		if (opt.has(L"split"))
		{
			for (auto& uid : opt.args())
			{
				RECT wa = mtl::work_area(*view.mainWnd);

				int w = wa.right - wa.left;
				int nw = w / 2;
				wa.left = wa.left + nw;
				wa.right = wa.left + nw;
				view.mainWnd.move(wa);
				transferTab(uid, -1);
			}
		}

		if (opt.has(L"file"))
		{
			for (auto& p : opt.args())
			{
				openPath(p);
			}
		}

		view.onBarCmd(ID_FILE_NEW, [this]()
		{
			mtl::pick_folder folderPicker;
			std::wstring path = folderPicker.choose(view.mainWnd.handle);
			if (!path.empty())
			{
				//view.folderWnd.path(path);
			}
		});

		onCmd(view.mainWnd.onCmd)
			.when(ID_FILE_NEW)
			.then([this]()
		{

			MySaveDialog fd(0);

			fd.filter({ { L"all files (*.*)", L"*.*"} });
			fd.encoding(CP_UTF8);

			HRESULT hr = fd.save(*view.mainWnd);
			if (hr == S_OK)
			{
				std::wstring path = fd.path();
				int i = fd.choice(CONTROL_SWITCH);
				std::wstring eol = L"UNIX";
				if (i == 1) eol = L"DOS";
				::MessageBox(0, path.c_str(), eol.c_str(), 0);
			}

		});

		view.mainWnd.onCmd(ID_FILE_NEW, [this]()
		{

			MyFileDialog fd(FOS_ALLOWMULTISELECT);

			fd.filter({ { L"all files (*.*)", L"*.*"} });
			fd.encoding(CP_UTF8);

			HRESULT hr = fd.open(*view.mainWnd);
			if (hr == S_OK)
			{
				std::wstring path = fd.path();
				::MessageBox(0, path.c_str(), L"yo", 0);
			}

		});

		view.onBarCmd(ID_EDIT_RUN, [this]()
		{
			if (activeDocument.empty()) return;

			mtl::punk<IMTLEditor> editor(*this->editor);
			scripting->add_named_object(*editor, L"moe");
			std::string utf8 = documents[activeDocument]->get_text();
			scripting->run_script(mtl::to_wstring(utf8));

		});

		view.onBarCmd(ID_EDIT_FIND, [this]()
		{
			HWND hWnd = view.searchDlg.find(*view.mainWnd);
			::SetWindowText(hWnd, L"RegExp Find");
		});

		onFind(view.mainWnd.onFind)
			.then([this](FINDREPLACE* fr)
		{
			if (activeDocument.empty()) return;
			//view.scintilla.search( mtl::to_string(fr->lpstrFindWhat), fr->Flags | SCFIND_REGEXP| SCFIND_CXX11REGEX);
			documents[activeDocument]->search(mtl::to_string(fr->lpstrFindWhat), fr->Flags | SCFIND_REGEXP | SCFIND_CXX11REGEX);
		});
		/*
		view.mainWnd.onFind( [this](FINDREPLACE* fr)
		{
			if (activeDocument.empty()) return;
			//view.scintilla.search( mtl::to_string(fr->lpstrFindWhat), fr->Flags | SCFIND_REGEXP| SCFIND_CXX11REGEX);
			documents[activeDocument]->search(mtl::to_string(fr->lpstrFindWhat), fr->Flags | SCFIND_REGEXP | SCFIND_CXX11REGEX);
		});
		*/
		view.mainWnd.onCmd(ID_EDIT_COPY, [this]()
		{
			bool checked = view.mainWnd.menu.item(ID_EDIT_COPY).checked;
			if (checked)
			{
				view.mainWnd.menu.item(ID_EDIT_COPY).check(false);
				/*
				COLORREF txtClr = ::GetSysColor(COLOR_WINDOWTEXT);
				COLORREF bkgClr = ::GetSysColor(COLOR_WINDOW);
				COLORREF selTxtClr = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
				COLORREF selBkgClr = ::GetSysColor(COLOR_HIGHLIGHT);
				view.colorTheme->textColor = txtClr;
				view.colorTheme->bkgColor = bkgClr;
				view.colorTheme->selectedTextColor = selTxtClr;
				view.colorTheme->selectedBkgColor = selBkgClr;
				view.colorTheme->font = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
				*/
				view.colorTheme->enabled(false);
				view.colorTheme->update();

				//std::shared_ptr<mtl::ColorTheme> tmp;
				//view.reBar.setColorTheme(tmp);// view.colorTheme);
				//view.tabControl.setColorTheme(view.colorTheme);

				//view.mainWnd.menu.update();
				//view.mainWnd.setMenu(*view.mainWnd.menu);
				view.mainWnd.update();

			}
			else
			{
				view.mainWnd.menu.item(ID_EDIT_COPY).check(true);

				view.colorTheme->enabled(true);
				/*
				view.colorTheme->textColor = view.textColor;
				view.colorTheme->bkgColor = view.bkgColor;
				view.colorTheme->selectedTextColor = view.selectedColor;
				view.colorTheme->selectedBkgColor = view.selectedBkgColor;
				view.colorTheme->font = *view.font;

				*/
				view.colorTheme->update();
				//view.reBar.setColorTheme(view.colorTheme);
				//view.tabControl.setColorTheme(view.colorTheme);
				//view.mainWnd.menu.update();
				//view.mainWnd.setMenu(*view.mainWnd.menu);
				view.mainWnd.update();
			}
			/*
			mtl::ROT rot;
			auto objects = rot.enumerate(__uuidof(MTLEditor));
			std::wostringstream woss;
			for (auto& obj : objects)
			{
				woss << obj << "\r\n";
			}
			std::wstring txt = woss.str();
			::MessageBox(*view.mainWnd, txt.c_str(), L"COPY", 0);
			*/
			//			view.folderWnd.copy();
		});

		view.mainWnd.onCmd(ID_EDIT_PASTE, [this]()
		{
			bool toggle = !view.mainWnd.menu.item(ID_EDIT_PASTE).checked;
			view.mainWnd.menu.item(ID_EDIT_PASTE).check(toggle);
		});


		view.toolBar.onCommand(IDM_OPEN, [this]()
		{
			//mtl::Dialog dlg;
			//dlg.showModal(IDD_ABOUTBOX, view.mainWnd.handle);

			MyFileDialog fd(0);

			fd.filter({ { L"all files (*.*)", L"*.*"} });
			fd.encoding(CP_UTF8);

			HRESULT hr = fd.open(*view.mainWnd);
			if (hr == S_OK)
			{
				std::wstring path = fd.path();
				//::MessageBox(0, path.c_str(), L"yo", 0);

				openPath(path);
				/*
								mtl::File file;
								if (file.open(path))
								{
									auto data = file.content();
									view.scintilla.setText(data);
									view.load_xml(ID_MODE_EDIT);
								}
								*/
			}

		});
		/*
		view.toolBar.onBar([this](int cmd)
		{
			switch (cmd)
			{
				case ID_FILE_NEW:
				{
					::MessageBox(*view.mainWnd, L"ID_FILE_NEW", L"click", 0);
					break;
				}
				case ID_FILE_OPEN:
				{
					MyFileDialog fd(0);

					fd.filter({ { L"all files (*.*)", L"*.*"} });
					fd.encoding(CP_UTF8);

					HRESULT hr = fd.open(*view.mainWnd);
					if (hr == S_OK)
					{
						std::wstring path = fd.path();
						//::MessageBox(0, path.c_str(), L"yo", 0);

						openPath(path);
						break;
					}
				}
			}
		});


		view.toolBar.onBarNotify([this](int id, NMTOOLBAR* nmtool)
		{
			switch (id)
			{
			case ID_FILE_NEW: {
				::MessageBox(*view.mainWnd, L"ID_FILE_NEW", L"click dropdown", 0);
				break;
			}
			case ID_FILE_OPEN: {
				::MessageBox(*view.mainWnd, L"ID_FILE_OPEN", L"click dropdown", 0);
				break;
			}
			}
		});
		*/
		view.mainWnd.onCmd(IDM_EXIT, []()
		{
			::PostQuitMessage(0);
		});

		/*
		view.onBarNotify(ID_FILE_OPEN, [this](NMTOOLBAR* hdr)
		{
			view.mainWnd.menu.sub_menu(ID_EDIT_CUT).popup(*view.mainWnd, TPM_CENTERALIGN);

		});
		*/

		onToolbar(view.onBarNotify)
			.when(ID_FILE_OPEN)
			.then([this](NMTOOLBAR* hdr)
		{
			//mtl::Menu menu(IDC_EDITOR);
			//mtl::MenuBuilder mb(*menu, 40, 40);
			//mb.addBitmaps();

			//int id  = view.menu.submenu(0).submenu(2).trackPopUp(*view.mainWnd, TPM_CENTERALIGN);

			view.mainWnd.menu.sub_menu(ID_EDIT_CUT).popup(*view.mainWnd, TPM_CENTERALIGN);
			/*			int id = view.mainWnd.menu.menu(ID_EDIT_CUT).trackPopUp(*view.mainWnd, TPM_CENTERALIGN);

						if (id == ID_FILE_NEW)
						{
							MyFileDialog fd(FOS_ALLOWMULTISELECT);

							fd.filter({ { L"all files (*.*)", L"*.*"} });
							fd.encoding(CP_UTF8);

							HRESULT hr = fd.open(*view.mainWnd);
							if (hr == S_OK)
							{
								std::wstring path = fd.path();
								::MessageBox(0, path.c_str(), L"yo", 0);
							}
						}
						*/
		});

		view.tabControl.onDragOut([this](std::wstring id)
		{
			::MessageBox(*view.mainWnd, id.c_str(), L"DRAG OUT", 0);
			std::wostringstream woss;
			woss << L"/open " << id;
			mtl::shell::execute_args(mtl::path_to_self(), woss.str());
		});


		view.tabControl.onCloseTab( [this](std::wstring id) 
		{
			removeDoc(id);
		});

		view.tabControl.onNotify(NM_CLICK, [this](NMHDR* hmhdr) 
		{
			::MessageBox(*view.mainWnd, L"NM_CLICK", L"tabControl.onNotify", 0);
		});

		view.tabControl.onSelect([this](mtl::tab_ctrl::tab& item)
		{
			
			if (view.tabControl.hit_icon_test())
			{
				::OutputDebugString(L"CLOSE: view.tabControl.onSelect\r\n");
				return;
			}
			
			::OutputDebugString(L"SELECT: view.tabControl.onSelect\r\n");

			HICON icon = mtl::shell::file_icon(documentPaths[item.id]);
			view.mainWnd.set_icon(icon);
		});

		view.tabControl.onPopulateDataObj([this](mtl::tab_ctrl::tab& tci, IDataObject* d)
		{
			XmlDocumentInfo xmlDocInfo { 
				mtl::to_string(tci.id), 
				mtl::to_string(tci.title), 
				mtl::to_string(tci.tooltip) 
			};

			XmlDocument xmlDoc{
				xmlDocInfo,
				"BODY"
			};

			auto doc = mtl::toXml(xmlDoc);
			mtl::bstr xml;
			doc->get_xml(&xml);

			std::string utf8 = xml.to_string();

		//	::MessageBox(0, xml.str().c_str(), L"onPopulateDataObj", 0);

			mtl::format_etc fe(doc_format);
			mtl::stg_medium stgm(utf8);
			d->SetData(&fe, &stgm, FALSE);

		});

		view.tabControl.onDropExternal([this](int index_to, IDataObject* d)
		{
			DWORD effect;
			view.dropTarget->onDrop.fire(d, 0, effect);
			return;
			mtl::dataobj_view dov(d);
			std::wstring from = dov.wstring(view.tabControl.dragTabFormat);

			//::MessageBox(0, id.c_str(), id.c_str(), 0);

			//int to = view.tabControl.id2index(id);
			transferTab(from, index_to);

		});

		view.dropTarget->onDrop([this](IDataObject* ido, DWORD keyState, DWORD& effect)
		{
			effect = DROPEFFECT_COPY;

			mtl::dataobj_view dov(ido);
			if (dov.has(view.tabControl.dragTabFormat))
			{
				std::wstring id = dov.wstring(view.tabControl.dragTabFormat);

				if (documents.count(id))
				{
					std::wostringstream woss;
					woss << L"/split " << id;
					mtl::shell::execute_args(mtl::path_to_self(), woss.str());

					RECT wa = mtl::work_area(*view.mainWnd);
					int w = wa.right - wa.left;
					int nw = w / 2;
					wa.right = wa.left + nw;
					view.mainWnd.move(wa);
					effect = DROPEFFECT_MOVE;
				}
				else
				{
					transferTab(id, -1);
				}
				return;
			}
			mtl::format_etc_dropfile fdd;
			if (dov.has(fdd))
			{
				auto v = dov.dropFiles();
				for (auto& i : v)
				{
					openPath(i);
				}
				return;
			}
			mtl::format_etc_unicodetext fdu;
			if (dov.has(fdu))
			{
				auto s = dov.wstring(fdu);
				std::string utf8 = mtl::to_string(s);

				if (activeDocument.empty()) return;
				documents[activeDocument]->insert_text(utf8);

				if ((keyState & MK_SHIFT) || (keyState & MK_CONTROL))
				{
					effect = DROPEFFECT_MOVE;
				}
				return;
			}
			mtl::format_etc_text fdt;
			if (dov.has(fdt))
			{
				auto s = dov.string(fdt);
				std::wstring ws = mtl::to_wstring(s, CP_WINANSI);
				std::string utf8 = mtl::to_string(ws);

				if (activeDocument.empty()) return;
				documents[activeDocument]->insert_text(utf8);

				if ((keyState & MK_SHIFT) || (keyState & MK_CONTROL))
				{
					effect = DROPEFFECT_MOVE;
				}
				return;
			}
		});
	}
};


HRESULT __stdcall MTLEditorDocument::get_documentId(BSTR* docId) 
{
	if (!docId) return E_INVALIDARG;

	*docId = ::SysAllocString(id.c_str());
	return S_OK;
}

HRESULT __stdcall MTLEditorDocument::get_filename(BSTR* fn) 
{
	if (!fn) return E_INVALIDARG;
	std::wstring filename = controller->documentPaths[id];

	*fn = ::SysAllocString(filename.c_str());
	return S_OK;
}

HRESULT __stdcall MTLEditorDocument::get_content(BSTR* cnt)
{
	if (!cnt) return E_INVALIDARG;

	*cnt = 0;

	std::string bytes;
	if (controller->documents.count(id) != 0)
	{
		bytes = controller->documents[id]->get_text();
	}

	*cnt = ::SysAllocString(mtl::to_wstring(bytes).c_str());
	return S_OK;
}

HRESULT __stdcall MTLEditorDocuments::get_count(long* cnt)
{
	*cnt = (long)controller->documents.size();
	return S_OK;
}

HRESULT __stdcall MTLEditorDocuments::item(VARIANT idx, IMTLEditorDocument** doc)
{
	if (idx.vt == VT_BSTR)
	{
		mtl::variant_view vv(idx);
		auto id = vv.to_wstring();
		if (controller->documents.count(id))
		{
			mtl::punk<IMTLEditorDocument> d(new MTLEditorDocument(id, controller));
			return d.query_interface(doc);
		}
	}
	return S_OK;
}

HRESULT __stdcall MTLEditorDocuments::remove(VARIANT idx)
{
	if (idx.vt == VT_BSTR)
	{
		mtl::variant_view vv(idx);
		auto id = vv.to_wstring();
		if (controller->documents.count(id))
		{
			controller->removeDoc(id);
		}
	}
	return S_OK;
}
// go WinMain, go!


void load_resource_ids();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	mtl::uses libraries{
		mtl::OLE(),
		mtl::Win32CommonControls(),
		mtl::sci_lexer()
	};

	mtl::options opt(
		{ L"open", L"split"}
	);

	//load_resource_ids();

	mtl::application app(hInstance);

	EditorController controller(opt);

	return app.run();
}
