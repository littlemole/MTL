#pragma once

#include "mvc.h"
#include "service.h"

class Encodings
{
public:

	typedef std::pair<int, std::wstring> CodePage;
	typedef std::vector<CodePage>::iterator Iterator;

	Encodings();

	Iterator begin();
	Iterator end();

	CodePage& item(size_t index);
	size_t index(int cp);

private:
	std::vector<CodePage> codePages_;
};

Encodings& encodings();


class ScriptDialog : public mtl::dialog
{
public:

	mtl::list_view listview;

	UINT_PTR timer = 0;

	ScriptDialog();
	~ScriptDialog();

	void refresh();

	ScriptService* scriptService = nullptr;
	std::map<std::wstring, std::wstring> scripts;
};

class MyStatusBar : public mtl::status_bar
{
public:

	enum PARTS {
		FILENAME, SEP, ENCODING, EOL, LINE, POS
	};

	mtl::font font;
	mtl::font fontSmall;
	mtl::combo_box comboBoxSyntax;
	std::wstring statusText;

	mtl::brush brush;

	void set_text(const std::wstring& str);

private:

	virtual LRESULT wm_create() override;
	virtual LRESULT wm_size(RECT& rc) override;
};


class MyFileDialog : public mtl::file_open_dialog
{
public:
	MyFileDialog(int options);

	long encoding();
	bool readOnly();

	void encoding(long enc);

private:
	long encoding_;
	bool readOnly_ = false;

};

class MySaveDialog : public mtl::file_save_dialog
{
public:
	MySaveDialog(int options);

	EOL_TYPE eol_type();
	void eol_type(EOL_TYPE eol);

	long encoding();
	void encoding(long enc);

private:
	long encoding_ = 0;
	long eol_ = 0;
};


class MainWindow : public mtl::window<MainWindow>
{
public:

	mtl::event<int()>				onCmd;
	mtl::event<void(FINDREPLACE*)>	onFind;
	mtl::event<void(FINDREPLACE*)>	onReplace;

	mtl::default_layout				layout;
	mtl::taskbar					taskbar;
	mtl::menu						menu;

	int								padding = 6;

	MainWindow();

	virtual LRESULT wm_create() override;
	virtual LRESULT wm_size(RECT& clientRect) override;
	virtual LRESULT wm_destroy() override;
	virtual LRESULT wm_erase_background(WPARAM wParam) override;
	virtual LRESULT wm_command(int id) override;
	virtual LRESULT wm_search(FINDREPLACE* fr) override;
	virtual LRESULT on_alt_key() override;
};

enum DocumentViewType {
	DOCVIEW_TXT,
	DOCVIEW_IMG
};

class DocumentView
{
public:

	virtual ~DocumentView() {}

	virtual std::wstring id() = 0;
	virtual HWND handle() = 0;
	virtual DocumentViewType type() = 0;
private:
};

class ScintillaDocumentView : public DocumentView
{
public:

	mtl::scintilla_wnd scintilla;

	ScintillaDocumentView(const std::wstring& id)
		: id_(id)
	{

	}

	virtual ~ScintillaDocumentView() {}

	virtual std::wstring id() override
	{
		return id_;
	}

	virtual HWND handle() override
	{
		return scintilla.handle;
	}

	virtual DocumentViewType type() override
	{
		return DOCVIEW_TXT;
	}

private:
	std::wstring id_;
};

class DocumentViews
{
public:

	void insert(DocumentView* doc)
	{
		std::shared_ptr<DocumentView> d(doc);
		views[doc->id()] = d;
		lookup[doc->handle()] = doc->id();
	}

	std::wstring id(HWND hWnd)
	{
		if (!lookup.count(hWnd)) return L"";
		return lookup[hWnd];
	}

	void erase(std::wstring id)
	{
		if (!views.count(id)) return;

		HWND hWnd = views[id]->handle();
		lookup.erase(hWnd);
		views.erase(id);
	}

	DocumentView& get(std::wstring id)
	{
		static ScintillaDocumentView empty(L"");
		if (!views.count(id)) return empty;

		return *views[id];
	}

	DocumentView& get(HWND hWnd)
	{
		return get(id(hWnd));
	}

	DocumentView& operator[](std::wstring id)
	{
		return get(id);
	}

	DocumentView& operator[](HWND hWnd)
	{
		return get(id(hWnd));
	}

	size_t size()
	{
		return views.size();
	}

private:

	std::map<std::wstring, std::shared_ptr<DocumentView>> views;
	std::map<HWND,std::wstring> lookup;
};

class EditorView
{
public:

	bool regexSearch		= false;
	UINT urlFormat			= 0;

	MainWindow				mainWnd;
	MyStatusBar				statusBar;

	ScriptDialog			scriptDialog;
	mtl::search_dlg			searchDlg;

	mtl::tab_ctrl			tabControl;
	mtl::tool_tip			tooltip;
	mtl::tool_bar			toolBar;

	mtl::font				font;
	mtl::font				smallFont;

	COLORREF				textColor = RGB(0xEA, 0xEA, 0xEA);
	COLORREF				bkgColor = RGB(0x55, 0x59, 0x64);
	COLORREF				selectedColor = RGB(0x75, 0x79, 0x84);
	COLORREF				selectedBkgColor = RGB(0xEA, 0xEA, 0xEA);

	std::shared_ptr<mtl::color_theme>	colorTheme;

	mtl::punk<mtl::default_drop_target> dropTarget;

	std::map<std::wstring, std::shared_ptr<mtl::scintilla_wnd>> documentViews;


	EditorView();

	std::shared_ptr<mtl::scintilla_wnd> createEditorWnd(std::wstring id, std::wstring path, std::string utf8);

	std::wstring removeDocumentView(const std::wstring& id);

	void updateStatus(Document& doc);

	std::wstring activeDocument() { return activeDocument_; }

	void activate(Document& doc);

	void doShowFindReplaceDialog(int id);

	void doFind(FINDREPLACE* fr);
	void doReplace(FINDREPLACE* fr);

private:

	std::wstring activeDocument_;
};

