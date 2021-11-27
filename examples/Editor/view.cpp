#include "view.h"

Encodings::Encodings()
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


Encodings::Iterator Encodings::begin()
{
	return codePages_.begin();
}

Encodings::Iterator Encodings::end()
{
	return codePages_.end();
}

Encodings::CodePage& Encodings::item(size_t index)
{
	return codePages_[index];
}

size_t Encodings::index(int cp)
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

Encodings& encodings()
{
	static Encodings enc;
	return enc;
}


ScriptDialog::ScriptDialog() 
{
	onInitDlg([this](mtl::dialog& dlg)
	{
		HWND hWnd = dialog_item(IDC_LIST_SCRIPTS);
		ListView_SetExtendedListViewStyle(hWnd, LVS_EX_CHECKBOXES);
		listview.subclass(hWnd);
		listview.add_column(0, L"Running Scripts");

		for (auto& it : scripts)
		{
			listview.add_item(it.second);
		}

		refresh();
	});

	onClick(IDC_BUTTON_KILL, [this]()
	{
		std::map<std::wstring, std::wstring> tmp = scripts;
		scripts.clear();

		int i = 0;
		for (auto& it : tmp)
		{
			if (listview.is_checked(i))
			{
				scriptService->kill(it.first);
			}
			else
			{
				scripts.insert(it);
			}
			i++;
		}
		send_msg(WM_COMMAND, IDC_BUTTON_REFRESH, 0);
	});

	onClick(IDC_BUTTON_REFRESH, [this]()
	{
		scriptService->getScripts([this](std::map<std::wstring, std::wstring> s)
		{
			scripts = s;
			listview.clear();
			for (auto& it : scripts)
			{
				listview.add_item(it.second);
			}
		});
	});
}

ScriptDialog::~ScriptDialog()
{
	if (timer)
	{
		mtl::timer::cancel(timer);
		timer = 0;
	}
}

void ScriptDialog::refresh()
{
	timer = 0;
	scriptService->getScripts([this](std::map<std::wstring, std::wstring> s)
	{
		if (scripts != s)
		{
			scripts = s;
			listview.clear();
			for (auto& it : scripts)
			{
				listview.add_item(it.second);
			}
		}
		timer = mtl::timer::set_timeout(250, [this](UINT_PTR)
		{
			refresh();
		});
	});

}


void MyStatusBar::set_text(const std::wstring& str)
{
	statusText = str;
	send_msg(SB_SETTEXT, SBT_NOBORDERS, (LPARAM)statusText.c_str());
}

LRESULT MyStatusBar::wm_create()
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

LRESULT MyStatusBar::wm_size(RECT& rc) 
{
	RECT cbr = comboBoxSyntax.client_rect();
	int cbw = cbr.right - cbr.left;
	int cbh = cbr.bottom - cbr.top;
	RECT dest = { rc.right - cbw - 2, rc.top + 1, rc.right - 2,rc.top + cbh + 1 };
	comboBoxSyntax.move(dest);
	invalidate();
	RECT r = rc;
	r.right -= (dest.right - dest.left);
	return mtl::status_bar::wm_size(r);
}



#define CONTROL_GROUP_ENC       2000
#define CONTROL_GROUP_EOL       2001
#define CONTROL_ENCODING        2003
#define CONTROL_EOL		        2004
#define CONTROL_SWITCH	        2005

#define OPENCHOICES					0
#define OPENCHOICES_OPEN			0
#define OPENCHOICES_OPEN_READONLY	1


MyFileDialog::MyFileDialog(int options)
	: mtl::file_open_dialog(options), readOnly_(false), encoding_(CP_ACP)
{
	customize([this](mtl::punk<IFileDialogCustomize> fdc)
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

long MyFileDialog::encoding()
{
	DWORD index = choice(CONTROL_ENCODING);
	if (index == -1) return index;
	return encodings().item(index).first;
}

bool MyFileDialog::readOnly()
{
	return readOnly_;
}

void MyFileDialog::encoding(long enc)
{
	encoding_ = (int)encodings().index(enc);
}


MySaveDialog::MySaveDialog(int options)
	: mtl::file_save_dialog(options)
{
	encoding(CP_UTF8);

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

		fdc->SetSelectedControlItem(CONTROL_SWITCH, eol_);

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

		fdc->SetSelectedControlItem(CONTROL_ENCODING, encoding_);

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

EOL_TYPE MySaveDialog::eol_type()
{
	int c = choice(CONTROL_EOL);
	if (c == 0) return EOL_UNIX;
	return EOL_DOS;
}

void MySaveDialog::eol_type(EOL_TYPE eol)
{
	eol_ = (long)eol;
}

long MySaveDialog::encoding()
{
	return encodings().item(choice(CONTROL_ENCODING)).first;
}

void MySaveDialog::encoding(long enc)
{
	encoding_ = (int)encodings().index(enc);
}


MainWindow::MainWindow() : taskbar(handle)
{
	mtl::font_desc fontDesc(L"Lucida Console", 14);
}

LRESULT MainWindow::wm_command(int id)
{
	onCmd.fire(id);
	return 0;
}

LRESULT MainWindow::wm_erase_background(WPARAM wParam)
{
	return 1;
}

LRESULT MainWindow::wm_size(RECT& clientRect)
{
	RECT rp = { 0,0,0,0 };
	layout.do_layout(clientRect, rp);
	return 0;
}

LRESULT MainWindow::wm_destroy()
{
	onCmd.fire(IDM_EXIT);
	//		::PostQuitMessage(0);
	return 0;
}

LRESULT MainWindow::wm_search(FINDREPLACE* fr)
{
	if (fr->Flags & FR_REPLACE)
	{
		onReplace.fire(fr);
		return 0;
	}
	if (fr->Flags & FR_REPLACEALL)
	{
		onReplace.fire(fr);
		return 0;
	}
	onFind.fire(fr);
	return 0;
}

LRESULT MainWindow::wm_create()
{
	//::SetWindowTheme(handle, L"", L"");
	return 0;
}

LRESULT MainWindow::on_alt_key()
{
	HMENU test = this->get_menu();
	if (::IsMenu(test))
	{
		this->set_menu(0);
	}
	else
	{
		this->set_menu(*menu);
	}
	return 0;
}




std::shared_ptr<mtl::scintilla_wnd> EditorView::createEditorWnd(std::wstring id, std::wstring path, std::string utf8)
{
	auto scintilla = std::make_shared<mtl::scintilla_wnd>();

	RECT tr = tabControl.display_rect();

	scintilla->create(IDC_SCINTILLA, *tabControl, tr, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	scintilla->load_xml(mtl::path_to_self_directory(L"\\styles.xml"));
	scintilla->set_code_page(CP_UTF8);
	scintilla->set_mode(SCLEX_CPP);
	scintilla->set_tab_width(4);
	scintilla->set_drop_target(*dropTarget);
	scintilla->notifications();

	scintilla->set_text(utf8);
	scintilla->show();
	scintilla->colorize();

	documentViews[id] = scintilla;
	activeDocument_ = id;

	std::wstring title = mtl::path(path).filename();
	tabControl.add({ title, path, id }, scintilla->handle);

	mainWnd.taskbar.add(id, scintilla->handle, path);


	return scintilla;
}


std::wstring EditorView::removeDocumentView(const std::wstring& id)
{
	if (documentViews.count(id) == 0) return L"";

	auto scintilla = documentViews[id];
	scintilla->destroy();

	documentViews.erase(id);

	tabControl.remove(id);
	mainWnd.taskbar.remove(id);

	std::wstring result = L"";

	if (documentViews.size() == 0)
	{
		HICON hIcon = mtl::shell::file_icon(L"C:\\test.txt");
		mainWnd.set_icon(hIcon);
		statusBar.set_status({ L"open a document ..." });
	}
	else
	{
		int index = tabControl.selected();
		index = index < 0 ? (int)documentViews.size() - 1 : index;
		std::wstring firstId = tabControl.item(index).id;
		result = firstId;
	}
	activeDocument_ = result;
	return result;
}

void EditorView::activate(EditorDocument& doc)
{
	if (documentViews.count(doc.id))
	{
		activeDocument_ = doc.id;

		updateStatus(doc);
		mainWnd.taskbar.activate(doc.id);
	}
}

void EditorView::updateStatus(EditorDocument& doc)
{
	auto sci = documentViews[doc.id];
	int pos = sci->pos();
	int line = sci->line_from_pos(pos);
	int line_pos = pos - sci->pos_from_line(line);

	wchar_t line_buf[100];
	swprintf_s(line_buf, 100, L"Line %4i", line);

	wchar_t pos_buf[100];
	swprintf_s(pos_buf, 100, L"Pos %3i", line_pos);

	statusBar.set_status({
		doc.textFile.filename, L"",
		encodings().item(encodings().index(doc.textFile.fileEncoding.code_page)).second,
		doc.textFile.fileEncoding.eol == EOL_UNIX ? L"UNIX" : L" DOS",
		line_buf,
		pos_buf
		});

	HICON icon = mtl::shell::file_icon(doc.textFile.filename);
	mainWnd.set_icon(icon);

	std::wstringstream woss;
	woss << L"Editor - " << mtl::path(doc.textFile.filename).filename();
	mainWnd.set_text(woss.str());
}



void EditorView::doFind(FINDREPLACE* fr)
{
	if (activeDocument().empty()) return;

	DWORD flags = fr->Flags;
	if (regexSearch == true)
	{
		flags |= SCFIND_REGEXP | SCFIND_CXX11REGEX;
	}

	documentViews[activeDocument()]->search(
		mtl::to_string(fr->lpstrFindWhat),
		flags
	);
};

void EditorView::doReplace(FINDREPLACE* fr)
{
	if (activeDocument().empty()) return;

	DWORD flags = fr->Flags;
	if (regexSearch == true)
	{
		flags |= SCFIND_REGEXP | SCFIND_CXX11REGEX;
	}

	if (flags & FR_REPLACEALL)
	{
		while (documentViews[activeDocument()]->replace(
			mtl::to_string(fr->lpstrFindWhat),
			mtl::to_string(fr->lpstrReplaceWith),
			flags
		));
	}
	else
	{
		documentViews[activeDocument()]->replace(
			mtl::to_string(fr->lpstrFindWhat),
			mtl::to_string(fr->lpstrReplaceWith),
			flags
		);
	}
};

void EditorView::doShowFindReplaceDialog(int id)
{
	if (activeDocument().empty()) return;
	documentViews[activeDocument()]->set_next_search_pos(0);

	regexSearch = false;

	HWND hWnd = searchDlg.replace(*mainWnd);
	switch (id)
	{
		case IDM_EDIT_FIND: {
			::SetWindowText(hWnd, L"Search");
			break;
		}
		case IDM_EDIT_REPLACE: {
			::SetWindowText(hWnd, L"Replace");
			break;
		}
		case IDM_EDIT_FIND_REGEX: {
			regexSearch = true;
			::SetWindowText(hWnd, L"RegExp Find");
			break;
		}
		case IDM_EDIT_REPLACE_REGEX: {
			regexSearch = true;
			::SetWindowText(hWnd, L"RegExp Replace");
			break;
		}
	}
};

EditorView::EditorView()
{
	mtl::the_bitmap_cache().img_path(L"\\img");

	urlFormat = ::RegisterClipboardFormat(CFSTR_INETURLW);

	dropTarget = mtl::drop_target(tabControl.dragTabFormat, DROPEFFECT_MOVE);

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
				{IDM_FILE_NEW},
				{IDM_FILE_OPEN},
				{IDM_EDIT_CUT, {
						{IDM_TOGGLE_OWNER_DRAWN, true},
						{IDM_EDIT_PASTE}
				}},
				{IDM_EXIT}
		}},
		{ ID_EDIT, {
			IDM_EDIT_FIND,
			IDM_EDIT_REPLACE,
			IDM_EDIT_FIND_REGEX,
			IDM_EDIT_REPLACE_REGEX
		}},
		{ ID_HELP, {
			{IDM_TOGGLE_OWNER_DRAWN},
			{ IDM_ABOUT}
		}}
		});

	//mainWnd.menu.item(ID_EDIT_COPY).checked = true;

	// create main window
	HWND hWnd = mainWnd.create(L"MTL Editor", WS_OVERLAPPEDWINDOW, 0, 0);// *mainWnd.menu);

	HICON hIcon = mtl::shell::file_icon(L"C:\\test.txt");
	mainWnd.set_icon(hIcon);

	// get main window client rect dimension
	RECT r1;
	::GetClientRect(hWnd, &r1);

	RECT tbr = { 0,0,48,48 };
	toolBar.create(IDC_TOOLBAR, *mainWnd, tbr,
		CCS_NODIVIDER | CCS_NOPARENTALIGN | WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_LIST, TBSTYLE_EX_MIXEDBUTTONS | TBSTYLE_EX_DRAWDDARROWS);
	toolBar.add_button(IDM_FILE_NEW);
	toolBar.add_button(IDM_FILE_OPEN);
	toolBar.add_button(IDM_SAVE, 0, BTNS_DROPDOWN);
	toolBar.add_button(IDM_EDIT_FIND, 0, BTNS_DROPDOWN);
	toolBar.add_button(IDM_RUN);
	toolBar.set_color_theme(colorTheme);


	RECT tabRect = r1;
	tabControl.create(IDC_TABCONTROL, L"", *mainWnd, tabRect, TCS_FLATBUTTONS | TCS_TABS | CCS_NODIVIDER | CCS_NOPARENTALIGN | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TCS_OWNERDRAWFIXED);
	tabControl.enable_dragdrop();
	tabControl.set_color_theme(colorTheme);

	mtl::icon closeIcon = mtl::shell::stock_icon(SIID_DELETE);
	tabControl.enable_icon(*closeIcon);

	statusBar.create(IDC_STATUSBAR, L"Status", hWnd, r1, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CCS_NOPARENTALIGN);// | CCS_NODIVIDER);
	statusBar.set_color_theme(colorTheme);
	statusBar.comboBoxSyntax.set_color_theme(colorTheme);
	statusBar.set_status({ L"STATUS", L"",L"INS", L"DOS", L"Line 221", L"Pos 18" });

	// set the layout
	mainWnd.layout = {							// Default Layout with a
		{										//  vector<Widget> of
			{									//   Widget holding 
				*toolBar,						//    toolbar
				mtl::layout::style::NORTH		//    docked to NORTH
			},
			{									//   Widget holding a
				*statusBar,						//    statusbar
				mtl::layout::style::SOUTH		//    docked to SOUTH
			},									//  and
			{									//   widget
				*tabControl,					//    tabcontrol
				mtl::layout::style::FILL		//    taking remaining space
			}
		}
	};

	mainWnd.show();
	mainWnd.update();

	tooltip.create(IDC_TOOLTIP, *mainWnd, r1, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP);


	//tooltip.add(*mainWnd, menuBar.eolButton);
	tooltip.add(statusBar, L"STATUS");

	tooltip.set_font(colorTheme->font());



	// events

	tabControl.onPopulateDataObj([this](mtl::tab_ctrl::tab& tab, IDataObject* dao)
	{
		mtl::format_etc furl(urlFormat);
		mtl::stg_medium stgm_url(std::wstring(L"mte://") + tab.id);
		dao->SetData(&furl, &stgm_url, FALSE);
	});

	tabControl.onDropExternal([this](int index_to, IDataObject* d)
	{
		DWORD effect;
		dropTarget->onDrop.fire(d, 0, effect);
		return;
	});

	toolBar.onBarNotify([this](int id, NMTOOLBAR* nmhdr)
	{
		if (id == IDM_SAVE)
		{
			mtl::menu m;
			m.create_popup();
			mtl::menu_builder mb(m, 32, 32);
			mb.add({ {IDM_SAVE}, {IDM_SAVE_AS} });

			m.popup(*mainWnd);
		}
		if (id == IDM_EDIT_FIND)
		{
			mtl::menu m;
			m.create_popup();
			mtl::menu_builder mb(m, 32, 32);
			mb.add({ {IDM_EDIT_FIND}, {IDM_EDIT_REPLACE}, {IDM_EDIT_FIND_REGEX}, {IDM_EDIT_REPLACE_REGEX} });

			m.popup(*mainWnd);
		}
	});

	toolBar.onCommand(IDM_SAVE, [this]()
	{
		mainWnd.onCmd.fire(IDM_SAVE);
	});


	statusBar.onLeftClick(MyStatusBar::EOL, [this]()
	{
		mtl::menu m;
		m.create_popup();
		mtl::menu_builder mb(m, 20, 20);
		mb.add({
			{IDM_EOL_WIN32}, {IDM_EOL_UNIX}
			});

		m.popup(*mainWnd);
	});

	mainWnd.onCmd(IDM_TOGGLE_OWNER_DRAWN, [this]()
	{
		bool checked = mainWnd.menu.item(IDM_TOGGLE_OWNER_DRAWN).checked;
		if (checked)
		{
			mainWnd.menu.item(IDM_TOGGLE_OWNER_DRAWN).check(false);
			colorTheme->enabled(false);
			colorTheme->update();
			mainWnd.update();

		}
		else
		{
			mainWnd.menu.item(IDM_TOGGLE_OWNER_DRAWN).check(true);
			colorTheme->enabled(true);
			colorTheme->update();
			mainWnd.update();
		}
	});

	mainWnd.onCmd(ID_EDIT_PASTE, [this]()
	{
		bool toggle = !mainWnd.menu.item(ID_EDIT_PASTE).checked;
		mainWnd.menu.item(ID_EDIT_PASTE).check(toggle);
	});

	mainWnd.onFind([this](FINDREPLACE* fr) { doFind(fr);  });
	mainWnd.onReplace([this](FINDREPLACE* fr) { doReplace(fr);  });
}

