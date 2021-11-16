#include "Editor.h"
#include "MTL/scintilla/sci.h"
#include "MTL/win32/monitor.h"
#include "MTL/win32/io.h"
#include "MTL/ole/dataobj.h"
#include "MTL/win32/arg.h"

#include "MTL/obj/impl.h"
#include "MTL/disp/disp.h"
#include "MTL/disp/variant.h"
#include "MTL/disp/bstr.h"
#include "MTL/obj/localserver.h"
#include "MTL/obj/marshall.h"
#include "MTL/util/str.h"
#include "MTL/script/script.h"
#include "Editor_h.h"
#include <Uxtheme.h>
#include <queue>

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

	CodePage& item(size_t index)
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

enum EOL_TYPE {
	EOL_UNIX,
	EOL_DOS
};

enum IO_ERROR {
	IO_ERROR_SUCCESS,
	IO_ERROR_NOT_FOUND,
	IO_ERROR_ACCESS_DENIED
};

class TextFile
{
public:
	bool readonly = false;
	std::wstring filename;
	unsigned long long size;
	time_t last_written;
	mtl::file_encoding fileEncoding;
	std::string utf8;
};

class EditorDocument
{
public:
	std::wstring id;
	TextFile textFile;
	std::wstring fileWatchToken;
};


class FileService
{
public:

	mtl::file_monitor monitor;

	mtl::path find(const std::wstring& relative_path)
	{
		mtl::path p(relative_path);
		if (p.exists())
		{
			return p;
		}
		
		mtl::path tmp(mtl::path_to_self_directory() + L"\\" + relative_path);
		if(tmp.exists())
		{
			return tmp;
		}

		mtl::path tmp2(mtl::current_working_directory() + L"\\" + relative_path);
		if (tmp.exists())
		{
			return tmp;
		}
		return mtl::path();
	}

	mtl::path find_in(const std::wstring& relative_path, const std::wstring& root)
	{
		mtl::path p(relative_path);
		if (p.exists())
		{
			return p;
		}

		mtl::path tmp(root + L"\\" + relative_path);
		if (tmp.exists())
		{
			return tmp;
		}
		return find(relative_path);
	}

	IO_ERROR read(const std::wstring& path, long encoding, bool readonly, std::function<void(IO_ERROR,TextFile)> cb)
	{
		mtl::path p = find(path);
		if (!p)
		{
			return IO_ERROR_NOT_FOUND;
		}

		mtl::file file(GENERIC_READ);
		DWORD r = file.open(p,OPEN_EXISTING);
		if (r != ERROR_SUCCESS)
		{
			return read_admin(path, encoding, readonly, cb);
		}

		TextFile result;
		result.filename = p.str();
		result.readonly = readonly;
		result.size = file.info().size();
		result.last_written = mtl::file_time_to_time_t(file.info().last_access_time());

		r = file.async_content([this, encoding, result = std::move(result), cb](DWORD e, std::string content) mutable
		{
			if (e != ERROR_SUCCESS)
			{
				cb(IO_ERROR_ACCESS_DENIED, result);
				return;
			}

			auto sniffed_encoding = mtl::sniff(content.data(), content.size() > 1024 ? 1024 : content.size());
			result.fileEncoding = sniffed_encoding;

			if (encoding != -1)
			{
				result.fileEncoding.code_page = encoding;
			}

			result.utf8 = mtl::raw_bytes_as_utf8(result.fileEncoding, content.data(), content.size());
			result.utf8 = mtl::unix2dos(result.utf8);
			cb(IO_ERROR_SUCCESS, std::move(result));
		});

		if (r != ERROR_SUCCESS)
		{
			return IO_ERROR_ACCESS_DENIED;
		}

		return IO_ERROR_SUCCESS;
	}

	IO_ERROR write(TextFile textFile, std::function<void(IO_ERROR)> cb)
	{
		mtl::file file(GENERIC_READ|GENERIC_WRITE);

		DWORD r = file.open(textFile.filename,OPEN_ALWAYS| TRUNCATE_EXISTING);
		if (r != ERROR_SUCCESS)
		{
			return write_admin(textFile, cb);
		}

		std::string raw_bytes = mtl::encode_utf8(textFile.utf8,textFile.fileEncoding);

		r = file.async_content(raw_bytes, [this,cb](DWORD e) mutable
		{
			if (e != ERROR_SUCCESS)
			{
				cb(IO_ERROR_ACCESS_DENIED);
			}
			cb(IO_ERROR_SUCCESS);
		});

		if (r != ERROR_SUCCESS)
		{
			return IO_ERROR_ACCESS_DENIED;
		}
		return IO_ERROR_SUCCESS;
	}

	IO_ERROR read_admin(const std::wstring& path, long encoding, bool readonly, std::function<void(IO_ERROR,TextFile)> cb)
	{
		return IO_ERROR_ACCESS_DENIED;
	}


	IO_ERROR write_admin(TextFile textFile, std::function<void(IO_ERROR)> cb)
	{
		return IO_ERROR_ACCESS_DENIED;
	}
};

class RotService
{
public:

	EditorDocument transferTab(std::wstring instance, const std::wstring& from)
	{
		EditorDocument result;

		auto info = mtl::split(from, ':');
		if (info.size() != 2)
		{
			return result;
		}

		std::wstring instanceId = info[0];
		std::wstring documentId = info[1];

		mtl::punk<IMTLEditor> remoteEditor = mtl::rot::object<IMTLEditor>(__uuidof(MTLEditor), instanceId);
		if (!remoteEditor) return result;

		mtl::punk<IMTLEditorDocuments> docs;
		HRESULT hr = remoteEditor->get_documents(&docs);
		if (!docs) return result;

		mtl::punk<IMTLEditorDocument> doc;
		mtl::variant vId{ mtl::ole_char(from.c_str()) };
		docs->item(vId, &doc);
		if (!doc) return result;

		mtl::bstr fn;
		doc->get_filename(&fn);
		mtl::bstr content;
		doc->get_content(&content);

		std::wostringstream woss;
		woss << instance << L":" << documentId;

		std::wstring newId = woss.str();

		std::string utf8 = content.to_string();
		TextFile textFile;
		textFile.fileEncoding.code_page = CP_UTF8;
		textFile.fileEncoding.eol = mtl::file_encoding::UNIX;
		textFile.fileEncoding.has_bom = false;
		textFile.fileEncoding.is_binary = false;
		textFile.filename = fn.str();
		textFile.last_written = 0;
		textFile.readonly = false;
		textFile.size = utf8.size();
		textFile.utf8 = utf8;

		result.id = newId;
		result.textFile = std::move(textFile);

		docs->remove(vId);
		return result;
	}

};



class ScriptService;

class Script
{
public:


	Script(ScriptService& service, FileService& fs, const std::wstring& id, const std::wstring& s, const std::wstring& fn);

	~Script()
	{
		if (scripting_ && scripting_->activeScript)
		{
			close();
		}
	}


	bool run(mtl::punk<IUnknown> obj)
	{
		scripting_ = new mtl::active_script(L"JScript");
		scripting_->add_named_object( *obj, L"mte");
		scripting_->add_named_object(*hostObj_, L"", SCRIPTITEM_GLOBALMEMBERS| SCRIPTITEM_ISVISIBLE);
		scripting_->onError = [this](long line, long pos, std::wstring err, std::wstring src)
		{
			if (onError_)
			{
				onError_(line, pos, err, src);
			}
		};
		scripting_->run_script(source_);

		if (!wait_)
		{
			close();
			return true;
		}

		return false;
	}

	unsigned int set_timeout(unsigned int ms, IDispatch* cb)
	{
		mtl::punk<IDispatch> disp(cb);

		unsigned int id = mtl::timer::set_timeout(ms, [disp]()
		{
			DISPPARAMS dispParams = { 0, 0, 0, 0 };
			IDispatch* d = *disp;
			if (d)
			{
				d->Invoke(DISPID_VALUE, IID_NULL, 0, DISPATCH_METHOD, &dispParams, 0, 0, 0);
			}
		});

		return id;
	}


	void close();

	void importSource(std::wstring file)
	{
		std::wstring root = mtl::path( filename_ ).parent_dir();

		mtl::path p = fileService_.find_in(file, root);

		std::string content = mtl::slurp(p.str());

		mtl::punk<IActiveScriptParse> asp(*scripting_->activeScript);
		mtl::variant varResult;
		if (asp)
		{
			EXCEPINFO ei;
			::ZeroMemory(&ei, sizeof(ei));

			HRESULT hr = asp->ParseScriptText(
				*mtl::bstr(content),
				NULL, 0, 0, 1, 0,
				SCRIPTTEXT_ISPERSISTENT | SCRIPTTEXT_ISVISIBLE,
				&varResult,
				&ei
			);
		}
	}

	std::wstring filename() { return filename_;  }
	std::wstring id() { return id_; }
	bool wait() { return wait_;  }
	void wait(bool b) { wait_ = b; }

	void onError(std::function<void(long, long, std::wstring, std::wstring)> cb)
	{
		onError_ = cb;
	}

private:
	ScriptService& scriptService_;
	FileService& fileService_;

	bool wait_ = false;

	std::wstring id_;
	std::wstring source_;
	std::wstring filename_;

	mtl::punk<IUnknown> hostObj_;
	mtl::punk<mtl::active_script> scripting_;

	std::function<void(long, long, std::wstring, std::wstring)> onError_;

};


class ScriptService
{
public:

	FileService& fileService;

	ScriptService(FileService& service)
		: fileService(service)
	{
	}

	~ScriptService()
	{
		if (worker_.joinable())
		{
			worker_.join();
		}
	}


	void start(mtl::punk<IUnknown> unk)
	{
		worker_ = std::thread(&ScriptService::threadfun, this, mtl::proxy<IUnknown>(unk) );
	}

	void stop()
	{
		if (worker_.joinable())
		{
			box_.stop();
		}
	}

	void run(std::wstring scriptSource, std::wstring filename, std::function<void(long,long,std::wstring,std::wstring)> onError)
	{
		box_.submit( [this, scriptSource, filename, onError] ()
		{
			std::wstring id = mtl::new_guid();
			Script* script = new Script(*this, fileService, id, scriptSource, filename);
			script->onError(onError);

			scripts[id] = std::unique_ptr<Script>(script);
			bool done = scripts[id]->run(unk_);
			if (done)
			{
				scripts.erase(id);
			}
		});
	}

	void erase(std::wstring id)
	{
		scripts.erase(id);
	}

	void getScripts( std::function<void(std::map<std::wstring, std::wstring>)> cb)
	{
		box_.submit([this,cb]() 
		{
			std::map<std::wstring, std::wstring> result;

			for (auto& it : scripts)
			{
				result[it.first] = it.second->filename();
			}

			mtl::ui_thread().submit([cb,result]() 
			{
				cb(result);
			});
		});
	}

private:

	void threadfun( mtl::proxy<IUnknown> p)
	{
		mtl::STA enter;

		unk_ = *p;
		box_.run();
		unk_.release();
	}

	mtl::punk<IUnknown> unk_;
	std::thread worker_;
	mtl::thread_box<void()> box_;

	std::map<std::wstring, std::unique_ptr<Script>> scripts;
};



class EditorView;

class EditorModel
{
public:

	mtl::event<void()> onUpdate;
	mtl::event<void(std::wstring, std::wstring)> onFileChanged;

	EditorModel( FileService& fileService, RotService rotService, ScriptService& scriptService)
		: fileService_(fileService),
		  rotService_(rotService),
		  scriptService_(scriptService)
	{
		instanceId = mtl::new_guid();


	}

	std::wstring instanceId;
	std::wstring activeDocument;
	bool regexSearch = false;
	std::map<std::wstring, std::shared_ptr<EditorDocument>> documents;

	EditorDocument openNew();
	EditorDocument transferDocument(const std::wstring& from);
	IO_ERROR openFile(const std::wstring& path, bool readOnly, long enc, std::function<void(EditorDocument)> cb);
	void insertDocument(const std::wstring& id, TextFile& textFile);
	void saveDocument(std::wstring id, std::wstring path, int enc, EOL_TYPE eol, std::string utf8, std::function<void(IO_ERROR)> cb);
	void removeDocument(const std::wstring& id, const std::wstring& active);
	void reloadFile(const std::wstring& id, std::function<void(std::string)> cb);
private:
	FileService& fileService_;
	RotService& rotService_;
	ScriptService& scriptService_;
};


/* -------------------------------- */

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
		DWORD index = choice(CONTROL_ENCODING);
		if (index == -1) return index;
		return encodings().item(index).first;
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

	EOL_TYPE eol_type()
	{
		int c = choice(CONTROL_EOL);
		if (c == 0) return EOL_UNIX;
		return EOL_DOS;
	}

	void eol_type(EOL_TYPE eol)
	{
		eol_ = (long)eol;
	}

	long encoding()
	{
		return encodings().item(choice(CONTROL_ENCODING)).first;
	}

	void encoding(long enc)
	{
		encoding_ = (int)encodings().index(enc);
	}

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

	mtl::menu						menu;

	int								padding = 6;


	MainWindow()
	{
		mtl::font_desc fontDesc(L"Lucida Console", 14);
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
//		::PostQuitMessage(0);
		return 0;
	}

	virtual LRESULT wm_search(FINDREPLACE* fr) override
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


	virtual LRESULT wm_create() override
	{
		//::SetWindowTheme(handle, L"", L"");
		return 0;
	}

	virtual LRESULT on_alt_key() override
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

};



class EditorView
{
public:

	MainWindow				mainWnd;
	MyStatusBar				statusBar;

	mtl::tab_ctrl			tabControl;
	mtl::tool_tip			tooltip;
	mtl::search_dlg			searchDlg;
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


	std::shared_ptr<mtl::scintilla_wnd> createEditorWnd(std::wstring id, std::wstring path, std::string utf8)
	{
		auto scintilla = std::make_shared<mtl::scintilla_wnd>();

		RECT tr = tabControl.display_rect();

		scintilla->create(IDC_SCINTILLA, *tabControl, tr, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		scintilla->load_xml(mtl::path_to_self_directory(L"\\styles.xml"));
		scintilla->set_code_page(CP_UTF8);
		scintilla->set_mode(SCLEX_CPP);
		scintilla->set_drop_target(*dropTarget);
		scintilla->notifications();

		scintilla->set_text(utf8);
		scintilla->show();
		scintilla->colorize();

		documentViews[id] = scintilla;

		std::wstring title = mtl::path(path).filename();
		tabControl.add({ title, path, id }, scintilla->handle);

		return scintilla;
	}


	std::wstring removeDocumentView(const std::wstring& id)
	{
		if (documentViews.count(id) == 0) return L"";

		auto scintilla = documentViews[id];
		scintilla->destroy();

		documentViews.erase(id);

		tabControl.remove(id);
		if (documentViews.size() == 0)
		{
			HICON hIcon = mtl::shell::file_icon(L"C:\\test.txt");
			mainWnd.set_icon(hIcon);
			statusBar.set_status({ L"open a document ..." });
			return L"";
		}
		else
		{
			int index = tabControl.selected();
			index = index < 0 ? (int)documentViews.size() - 1 : index;
			std::wstring firstId = tabControl.item(index).id;
			return firstId;
		}

	}

	void updateStatus(EditorDocument& doc)
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
	
	EditorView()
	{
		mtl::the_bitmap_cache().img_path(L"\\img");

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
				{ IDM_ABOUT}
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
			CCS_NODIVIDER | CCS_NOPARENTALIGN | WS_CHILD  | TBSTYLE_TOOLTIPS| TBSTYLE_LIST, TBSTYLE_EX_MIXEDBUTTONS| TBSTYLE_EX_DRAWDDARROWS);
		toolBar.add_button(IDM_FILE_NEW);
		toolBar.add_button(IDM_FILE_OPEN);
		toolBar.add_button(IDM_SAVE,0, BTNS_DROPDOWN);
		toolBar.add_button(IDM_EDIT_FIND,0, BTNS_DROPDOWN);
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

	}

};


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

class MTLScriptHostObject :
	public mtl::implements<MTLScriptHostObject(mtl::dual<IMTLScriptHostObject>)>
{
public:

	MTLScriptHostObject(Script* script)
		: script_(script)
	{}

	virtual HRESULT __stdcall Import(BSTR value) override
	{
		script_->importSource(mtl::bstr_view(value).str());
		return S_OK;
	}

	virtual HRESULT __stdcall setTimeout( long ms, IDispatch* cb, long* id) override
	{
		unsigned int i = script_->set_timeout(ms, cb);
		if (id) *id = i;

		return S_OK;
	}

	virtual HRESULT __stdcall Wait() override
	{
		script_->wait(true);;
		return S_OK;
	}

	virtual HRESULT __stdcall Quit() override
	{
		script_->close();
		return S_OK;
	}

private:
	Script* script_ = nullptr;
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

/* ----------------------------------------------- */

// Controller handles User Input
// Controller owns Model and View

class EditorController 
{
public:

	FileService& fileService;
	RotService& rotService;
	ScriptService& scriptService;

	EditorModel model;
	EditorView view;

	mtl::punk<IMTLEditor> editor;
	//mtl::punk<mtl::active_script> scripting;

	// sinks for demo purpose
	// real value of sink is for
	// subscribers with shorter lifetime
	// than emitters - handles event
	// notification deregistration

	mtl::sink<int()> with;
	mtl::sink<void(FINDREPLACE*)> whenFind;
	mtl::sink<void(FINDREPLACE*)> whenReplace;
	mtl::sink<UINT(NMTOOLBAR*)> onToolbar;

	void removeDocument(const std::wstring& id)
	{
		if (model.documents.count(id) == 0) return;

		std::wstring active = view.removeDocumentView(id);

		model.removeDocument(id,active);

		if (!model.activeDocument.empty())
		{
			view.updateStatus(*model.documents[active]);
		}

	}

	void transferDocument(const std::wstring& from, int to = -1)
	{
		EditorDocument document = model.transferDocument(from);

		if (document.id.empty()) return;

		insertDocument(document);
	}

	void openFile(const std::wstring& path, bool readOnly, long enc)
	{
		model.openFile(path, readOnly, enc, [this](EditorDocument doc)
		{
			insertDocument(doc);
		});
	}


	void insertDocument(EditorDocument doc)
	{
		model.insertDocument(doc.id, doc.textFile);

		auto scintilla = view.createEditorWnd(doc.id, doc.textFile.filename, doc.textFile.utf8);

		scintilla->onNotify(SCN_MODIFIED, [this, id = doc.id](NMHDR* nmhdr)
		{
			view.updateStatus(*model.documents[id]);
		});

		scintilla->onNotify(SCN_UPDATEUI, [this, id = doc.id](NMHDR* nmhdr)
		{
			view.updateStatus(*model.documents[id]);
		});

		view.updateStatus(*model.documents[doc.id]);
	}

	void openNew()
	{
		EditorDocument doc = model.openNew();
		insertDocument(doc);
	}

	void saveDocument(std::wstring id, std::wstring path, long enc, EOL_TYPE eol)
	{
		std::string utf8 = view.documentViews[id]->get_text();
		model.saveDocument(id, path, enc, eol, utf8, [this,path](IO_ERROR io) 
		{
			view.statusBar.set_text( std::wstring(L"file saved to ") + path);
		});
	}

	void reloadFile(const std::wstring& id)
	{
		model.reloadFile(id, [this, id](std::string utf8)
		{
			if (model.documents.count(id) == 0) return;

			view.documentViews[id]->set_text(utf8);
			view.documentViews[id]->colorize();

		});
	}


	EditorController(mtl::options& opt,FileService& fs, RotService& rs, ScriptService& sc)
		: fileService(fs), 
		  rotService(rs), 
		  scriptService(sc), 
		  model(fs, rs, sc)
		  //editor(__uuidof(MTLEditor),model.instanceId)
	{
		//scripting = new mtl::active_script(L"JScript");

		//mtl::punk<IMTLEditor> mtlEditor(new MTLEditor(this));
		//editor = mtlEditor;


		model.onFileChanged([this](std::wstring id, std::wstring path) 
		{
			mtl::dialog	dlgFileChanged;
			dlgFileChanged.onInitDlg([this,path](mtl::dialog& dlg) 
			{
				std::wostringstream woss;
				woss << L"File " << path << " changed on Disk.\r\n\r\nReload File?";
				dlg.set_dlg_item_text(IDC_FILE_CHANGED_LABEL, woss.str());
				//::SetWindowText(dlg.get_dlg_item(IDC_FILE_CHANGED_LABEL), woss.str().c_str());
				//::SetWindowText(::GetDlgItem(dlg, IDC_FILE_CHANGED_LABEL), woss.str().c_str());

				std::wostringstream woss2;
				woss2 << L"File " << mtl::path(path).filename() << " changed on Disk!";
				//::SetWindowText(dlg, woss2.str().c_str());
				dlg.set_text(woss2.str());

			});

			if (IDOK == dlgFileChanged.show_modal(IDD_FILE_CHANGED, *view.mainWnd))
			{
				reloadFile(id);
			}
			
		});

		/* example for dynamic tooltip ! */
		/*
		view.tooltip.onNotify(TTN_GETDISPINFO, [this](NMHDR* nmhdr)
		{
			HWND hwndFrom = nmhdr->hwndFrom;
			UINT_PTR id = nmhdr->idFrom;
			NMTTDISPINFO* di = (NMTTDISPINFO*)nmhdr;
			di->lpszText = 0;
			di->hinst = 0;
			di->szText[0] = 0;
			di->lpszText = ((wchar_t*)L"STATIC CB");

			int i = view.tabControl.hit_test();
			if (i != -1)
			{
				mtl::tab_ctrl::tab& c = view.tabControl.item(i);
				di->lpszText = (wchar_t*)(c.tooltip.c_str());
				di->hinst = 0;
			}

		});
		*/

		view.mainWnd.onCmd(IDM_ABOUT, [this]() 
		{
			mtl::dialog dlg;
			dlg.show_modal(IDD_ABOUTBOX, *view.mainWnd);

		});

		view.statusBar.onLeftClick( MyStatusBar::EOL, [this]() 
		{
			mtl::menu m;
			m.create_popup();
			mtl::menu_builder mb(m, 20, 20);
			mb.add({
				{IDM_EOL_WIN32}, {IDM_EOL_UNIX}
				});

			m.popup(*view.mainWnd);
		});
		/*
		view.statusBar.onNotify(NM_CLICK, [this](NMHDR* nmhdr) 
		{
			NMMOUSE* nm = (NMMOUSE*)nmhdr;
			if (nm->dwItemSpec == 3)
			{
				//::MessageBox(*view.mainWnd, L"EOL", L"LOL", 0);
				mtl::menu m;
				m.create_popup();
				mtl::menu_builder mb(m, 20, 20);
				mb.add({
					{IDM_EOL_WIN32}, {IDM_EOL_UNIX}
				});

				m.popup(*view.mainWnd);
			}
		});
		*/

		with(view.mainWnd.onCmd)
			.when(IDM_SAVE)
			.then([this]()
		{

			MySaveDialog fd(0);

			fd.filter({ { L"all files (*.*)", L"*.*"} });
			fd.encoding(CP_UTF8);
			fd.eol_type(EOL_UNIX);

			HRESULT hr = fd.save(*view.mainWnd);
			if (hr == S_OK)
			{
				std::wstring path = fd.path();
				long enc = fd.encoding();
				EOL_TYPE eol = fd.eol_type();
				saveDocument(model.activeDocument, path, enc, eol);
			}

		});

		with(view.mainWnd.onCmd)
			.when(IDM_EDIT_FIND)
			.then([this]()
		{
			if (model.activeDocument.empty()) return;

			model.regexSearch = false;			
			view.documentViews[model.activeDocument]->set_next_search_pos_(0);
			HWND hWnd = view.searchDlg.find(*view.mainWnd);
			::SetWindowText(hWnd, L"Search");
		});

		with(view.mainWnd.onCmd)
			.when(IDM_EDIT_FIND_REGEX)
			.then([this]()
		{
			if (model.activeDocument.empty()) return;

			model.regexSearch = true;
			view.documentViews[model.activeDocument]->set_next_search_pos_(0);
			HWND hWnd = view.searchDlg.find(*view.mainWnd);
			::SetWindowText(hWnd, L"RegExp Find");
		});

		with(view.mainWnd.onCmd)
			.when(IDM_EDIT_REPLACE)
			.then([this]()
		{
			if (model.activeDocument.empty()) return;

			model.regexSearch = false;
			view.documentViews[model.activeDocument]->set_next_search_pos_(0);
			HWND hWnd = view.searchDlg.replace(*view.mainWnd);
			::SetWindowText(hWnd, L"Replace");
		});

		with(view.mainWnd.onCmd)
			.when(IDM_EDIT_REPLACE_REGEX)
			.then([this]()
		{
			if (model.activeDocument.empty()) return;

			model.regexSearch = true;
			view.documentViews[model.activeDocument]->set_next_search_pos_(0);
			HWND hWnd = view.searchDlg.replace(*view.mainWnd);
			::SetWindowText(hWnd, L"RegExp Replace");
		});


		view.toolBar.onCommand(IDM_RUN, [this]()
		{
			if (model.activeDocument.empty()) return;

			//mtl::punk<IMTLEditor> editor(*this->editor);
			//scripting->add_named_object(*editor, L"moe");
			std::string utf8 = view.documentViews[model.activeDocument]->get_text();
			std::wstring fn = model.documents[model.activeDocument]->textFile.filename;
			scriptService.run(mtl::to_wstring(utf8), fn.c_str(), [this](long line, long pos, std::wstring err, std::wstring src)
			{
				std::wostringstream oss;
				oss << err << L" line " << line << "\r\n";
				oss << src;
				::MessageBoxW( *view.mainWnd, oss.str().c_str(), src.c_str(), MB_ICONERROR);
			});

			//scripting->run_script(mtl::to_wstring(utf8));

		});

		whenFind(view.mainWnd.onFind)
			.then([this](FINDREPLACE* fr)
		{
			if (model.activeDocument.empty()) return;

			DWORD flags = fr->Flags;
			if (model.regexSearch == true)
			{
				flags |= SCFIND_REGEXP | SCFIND_CXX11REGEX;
			}

			view.documentViews[model.activeDocument]->search(
				mtl::to_string(fr->lpstrFindWhat),
				flags
			);
		});

		whenReplace(view.mainWnd.onReplace)
			.then([this](FINDREPLACE* fr)
		{
			if (model.activeDocument.empty()) return;

			DWORD flags = fr->Flags;
			if (model.regexSearch == true)
			{
				flags |= SCFIND_REGEXP | SCFIND_CXX11REGEX;
			}
			
			if (flags & FR_REPLACEALL)
			{
				while(view.documentViews[model.activeDocument]->replace(
					mtl::to_string(fr->lpstrFindWhat),
					mtl::to_string(fr->lpstrReplaceWith),
					flags
				));
			}
			else
			{
				view.documentViews[model.activeDocument]->replace(
					mtl::to_string(fr->lpstrFindWhat),
					mtl::to_string(fr->lpstrReplaceWith),
					flags
				);
			}
		});

		view.mainWnd.onCmd(IDM_TOGGLE_OWNER_DRAWN, [this]()
		{
			bool checked = view.mainWnd.menu.item(IDM_TOGGLE_OWNER_DRAWN).checked;
			if (checked)
			{
				view.mainWnd.menu.item(IDM_TOGGLE_OWNER_DRAWN).check(false);
				view.colorTheme->enabled(false);
				view.colorTheme->update();
				view.mainWnd.update();

			}
			else
			{
				view.mainWnd.menu.item(IDM_TOGGLE_OWNER_DRAWN).check(true);
				view.colorTheme->enabled(true);
				view.colorTheme->update();
				view.mainWnd.update();
			}
		});

		view.mainWnd.onCmd(ID_EDIT_PASTE, [this]()
		{
			bool toggle = !view.mainWnd.menu.item(ID_EDIT_PASTE).checked;
			view.mainWnd.menu.item(ID_EDIT_PASTE).check(toggle);
		});

		view.mainWnd.onCmd(IDM_FILE_OPEN, [this]()
		{
			MyFileDialog fd(0);

			fd.filter({ { L"all files (*.*)", L"*.*"} });
			fd.encoding(CP_UTF8);

			HRESULT hr = fd.open(*view.mainWnd);
			if (hr == S_OK)
			{
				std::wstring path = fd.path();
				bool ro = fd.readOnly();
				long enc = fd.encoding();
				openFile(path, ro, enc);
			}
		});

		view.toolBar.onCommand(IDM_FILE_OPEN, [this]()
		{
			MyFileDialog fd(0);

			fd.filter({ { L"all files (*.*)", L"*.*"} });
			fd.encoding(CP_UTF8);

			HRESULT hr = fd.open(*view.mainWnd);
			if (hr == S_OK)
			{
				std::wstring path = fd.path();
				bool ro = fd.readOnly();
				long enc = fd.encoding();
				openFile(path,ro,enc);
			}

		});

		view.toolBar.onCommand(IDM_FILE_NEW, [this]()
		{
			openNew();
		});


		view.toolBar.onCommand(IDM_SAVE, [this]()
		{
			view.mainWnd.onCmd.fire(IDM_SAVE);
		});


		view.mainWnd.onCmd(IDM_SAVE_AS, [this]()
		{
			::MessageBox(*view.mainWnd, L"SAVE AS", L"x", 0);
		});

		view.mainWnd.onCmd(IDM_FILE_NEW, [this]()
		{
			scriptService.getScripts([this](std::map<std::wstring, std::wstring> scripts) 
			{
				std::wostringstream woss;
				for (auto& it : scripts)
				{
					woss << it.first << ":" << it.second << std::endl;
				}
				::MessageBox(*view.mainWnd, woss.str().c_str(), L"Scripts", 0);
			});


			//::MessageBox(*view.mainWnd, L"NEW", L"x", 0);
		});

		view.toolBar.onBarNotify([this](int id, NMTOOLBAR* nmhdr)
		{
			if (id == IDM_SAVE)
			{
				mtl::menu m;
				m.create_popup();
				mtl::menu_builder mb(m, 32, 32);
				mb.add({ {IDM_SAVE}, {IDM_SAVE_AS} });

				m.popup(*view.mainWnd);
			}
			if (id == IDM_EDIT_FIND)
			{
				mtl::menu m;
				m.create_popup();
				mtl::menu_builder mb(m, 32, 32);
				mb.add({ {IDM_EDIT_FIND}, {IDM_EDIT_REPLACE}, {IDM_EDIT_FIND_REGEX}, {IDM_EDIT_REPLACE_REGEX} });

				m.popup(*view.mainWnd);
			}
		});

		view.mainWnd.onCmd(IDM_EXIT, [this]()
		{
//			view.mainWnd.destroy();
			scriptService.stop();
			::PostQuitMessage(0);

		});

		view.tabControl.onDragOut([this](std::wstring id)
		{
			//::MessageBox(*view.mainWnd, id.c_str(), L"DRAG OUT", 0);
			std::wostringstream woss;
			woss << L"/open " << id;
			mtl::shell::execute_args(mtl::path_to_self(), woss.str());
		});


		view.tabControl.onCloseTab( [this](std::wstring id) 
		{
			removeDocument(id);
		});

		view.tabControl.onNotify(NM_CLICK, [this](NMHDR* hmhdr) 
		{
			//::MessageBox(*view.mainWnd, L"NM_CLICK", L"tabControl.onNotify", 0);
		});

		view.tabControl.onSelect([this](mtl::tab_ctrl::tab& item)
		{
			
			if (view.tabControl.hit_icon_test())
			{
				::OutputDebugString(L"CLOSE: view.tabControl.onSelect\r\n");
				return;
			}
			
			::OutputDebugString(L"SELECT: view.tabControl.onSelect\r\n");

			//HICON icon = mtl::shell::file_icon(model.documents[item.id]->textFile.filename);
			//view.mainWnd.set_icon(icon);

			view.updateStatus(*model.documents[item.id]);
		});

		view.tabControl.onDropExternal([this](int index_to, IDataObject* d)
		{
			DWORD effect;
			view.dropTarget->onDrop.fire(d, 0, effect);
			return;
			mtl::dataobj_view dov(d);
			std::wstring from = dov.wstring(view.tabControl.dragTabFormat);

			//::MessageBox(0, id.c_str(), id.c_str(), 0);

			transferDocument(from, index_to);
		});

		static UINT chromeFormat = ::RegisterClipboardFormat(L"Chromium Web Custom MIME Data Format");
//static UINT tabTextFormat = ::RegisterClipboardFormat(L"text/xml");
		static UINT taintFormat = ::RegisterClipboardFormat(L"chromium/x-renderer-taint");

		static UINT urlFormat = ::RegisterClipboardFormat(CFSTR_INETURLW);

		view.tabControl.onPopulateDataObj([this](mtl::tab_ctrl::tab& tab, IDataObject* dao)
		{
			//static UINT chromeFormat = ::RegisterClipboardFormat(L"Chromium Web Custom MIME Data Format");
			//static UINT tabTextFormat = ::RegisterClipboardFormat(L"text/xml");
			//static UINT taintFormat = ::RegisterClipboardFormat(L"chromium/x-renderer-taint");

			//mtl::format_etc fe(tabTextFormat);
			/*
			mtl::stg_medium stgm(tab.id);
			mtl::format_etc_unicodetext etcu;
			dao->SetData(&etcu, &stgm, FALSE);

			mtl::format_etc_text etct;
			std::string utf8 = mtl::to_string(tab.id);
			mtl::stg_medium stgm_utf8(utf8);
			dao->SetData(&etct, &stgm_utf8, FALSE);
			*/
			//return;

			std::wstring json = tab.id;// L"{ \"id\" : \"42\" }";

			mtl::format_etc fetaint(taintFormat);
			char b = 0;
			mtl::stg_medium stgm_taint( (void*)&b,1,  GMEM_MOVEABLE | GMEM_NODISCARD);
			dao->SetData(&fetaint, &stgm_taint, FALSE);

			mtl::format_etc furl(urlFormat);
			mtl::stg_medium stgm_url( std::wstring(L"mte://") + tab.id);
			dao->SetData(&furl, &stgm_url, FALSE);


			/*
			mtl::format_etc fec(chromeFormat);
			std::wstringstream woss;
			DWORD s = (json.size()+0) *1;
			//			woss.write((wchar_t*)&s,sizeof(DWORD)/2);
						//woss << L'\0';
			DWORD c = 1;
			woss.write((wchar_t*)&c, sizeof(DWORD) / 2);
			//woss << L'\0';
			std::wstring type(L"text-some");
			DWORD ts = (type.size()+0) * 1;
			woss.write((wchar_t*)&ts, sizeof(DWORD) / 2);
			woss << type << L'\0';
//			woss << L'\t' << L'\0' << L"text/json" << L'\0';// << L'&' << L'\0' << tab.id;
			woss.write((wchar_t*)&s, sizeof(DWORD) / 2);
			woss << json << L'\0';

			std::wstring payload = woss.str();
			DWORD ps = (payload.size()+0)*1;
			std::wostringstream woss2;
			woss2.write( (wchar_t*)&ps, sizeof(DWORD) / 2);
			woss2.write(payload.c_str(), payload.size());

			std::wstring ws = woss2.str();

			char* a = (char*)(ws.data());
			mtl::stg_medium stgm_chrome(ws);
			dao->SetData(&fec, &stgm_chrome, FALSE);
			*/
		});

		view.dropTarget->onDrop([this](IDataObject* ido, DWORD keyState, DWORD& effect)
		{
			effect = DROPEFFECT_COPY;

			mtl::dataobj_view dov(ido);
			if (dov.has(view.tabControl.dragTabFormat))
			{
				std::wstring id = dov.wstring(view.tabControl.dragTabFormat);

				if (model.documents.count(id))
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
					transferDocument(id, -1);
				}
				return;
			}
			mtl::format_etc_dropfile fdd;
			if (dov.has(fdd))
			{
				auto v = dov.dropFiles();
				for (auto& i : v)
				{
					openFile(i,false,-1);
				}
				return;
			}
			mtl::format_etc_unicodetext fdu;
			if (dov.has(fdu))
			{
				auto s = dov.wstring(fdu);
				std::string utf8 = mtl::to_string(s);

				if (model.activeDocument.empty()) return;
				view.documentViews[model.activeDocument]->insert_text(utf8);

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

				if (model.activeDocument.empty()) return;
				view.documentViews[model.activeDocument]->insert_text(utf8);

				if ((keyState & MK_SHIFT) || (keyState & MK_CONTROL))
				{
					effect = DROPEFFECT_MOVE;
				}
				return;
			}
		});

		if (opt.has(L"open"))
		{
			for (auto uid : opt.args())
			{
				uid = mtl::url_unescape(uid);
				if (!uid.empty() && uid.back() == '/')
				{
					uid = uid.substr(0, uid.size() - 1);
				}
				//::MessageBox(*view.mainWnd, uid.c_str(), L"/open", 0);
				if (uid.starts_with(L"mte://"))
				{
					//uid = mtl::url_unescape(uid);
					uid = uid.substr(6);
				}
				if (uid.starts_with(L"{"))
				{
					transferDocument(uid, -1);
					return;
				}
				openFile(uid, false, -1);
			}
		}

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
				transferDocument(uid, -1);
			}
		}

		if (opt.has(L"file"))
		{
			for (auto& p : opt.args())
			{
				openFile(p, false, -1);
			}
		}
	}
};


/* ------------------------------------------------- */


HRESULT __stdcall MTLEditorDocument::get_documentId(BSTR* docId) 
{
	if (!docId) return E_INVALIDARG;

	*docId = ::SysAllocString(id.c_str());
	return S_OK;
}

HRESULT __stdcall MTLEditorDocument::get_filename(BSTR* fn) 
{
	if (!fn) return E_INVALIDARG;
	*fn = 0;

	if (controller->model.documents.count(id) == 0)
	{
		return S_OK;
	}

	std::wstring filename = controller->model.documents[id]->textFile.filename;

	*fn = ::SysAllocString(filename.c_str());
	return S_OK;
}

HRESULT __stdcall MTLEditorDocument::get_content(BSTR* cnt)
{
	if (!cnt) return E_INVALIDARG;

	*cnt = 0;

	std::string bytes;
	if (controller->model.documents.count(id) != 0)
	{
		bytes = controller->view.documentViews[id]->get_text();
	}

	*cnt = ::SysAllocString(mtl::to_wstring(bytes).c_str());
	return S_OK;
}

HRESULT __stdcall MTLEditorDocuments::get_count(long* cnt)
{
	*cnt = (long)controller->model.documents.size();
	return S_OK;
}

HRESULT __stdcall MTLEditorDocuments::item(VARIANT idx, IMTLEditorDocument** doc)
{
	if (idx.vt == VT_BSTR)
	{
		mtl::variant_view vv(idx);
		auto id = vv.to_wstring();
		if (controller->model.documents.count(id))
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
		if (controller->model.documents.count(id))
		{
			controller->removeDocument(id);
		}
	}
	return S_OK;
}


/* ------------------------------------ */


void EditorModel::removeDocument(const std::wstring& id,  const std::wstring& active)
{
	if (documents.count(id) == 0) return;

	activeDocument = active;

	fileService_.monitor.unwatch(documents[id]->fileWatchToken, documents[id]->textFile.filename);

	documents.erase(id);
}

EditorDocument EditorModel::transferDocument(const std::wstring& from)
{
	return rotService_.transferTab(this->instanceId, from);
	/*EditorDocument result;

	auto info = mtl::split(from, ':');
	if (info.size() != 2)
	{
		return result;
	}

	std::wstring instanceId = info[0];
	std::wstring documentId = info[1];

	mtl::punk<IMTLEditor> remoteEditor = mtl::rot::object<IMTLEditor>(__uuidof(MTLEditor), instanceId);
	if (!remoteEditor) return result;

	mtl::punk<IMTLEditorDocuments> docs;
	HRESULT hr = remoteEditor->get_documents(&docs);
	if (!docs) return result;

	mtl::punk<IMTLEditorDocument> doc;
	mtl::variant vId{ mtl::ole_char(from.c_str()) };
	docs->item(vId, &doc);
	if (!doc) return result;

	mtl::bstr fn;
	doc->get_filename(&fn);
	mtl::bstr content;
	doc->get_content(&content);

	std::wostringstream woss;
	woss << this->instanceId << L":" << documentId;

	std::wstring newId = woss.str();

	std::string utf8 = content.to_string();
	TextFile textFile;
	textFile.fileEncoding.code_page = CP_UTF8;
	textFile.fileEncoding.eol = mtl::file_encoding::UNIX;
	textFile.fileEncoding.has_bom = false;
	textFile.fileEncoding.is_binary = false;
	textFile.filename = fn.str();
	textFile.last_written = 0;
	textFile.readonly = false;
	textFile.size = utf8.size();
	textFile.utf8 = utf8;

	result.id = newId;
	result.textFile = std::move(textFile);

	docs->remove(vId);
	*/
	//return result;
}

/*
void EditorModel::updateStatus(std::wstring id)
{
	if (documents.count(id) == 0) return;

	//std::wstring path = documents[id]->textFile.filename;
	//int cp = documents[id]->textFile.fileEncoding.code_page;
	//EOL_TYPE eol = (EOL_TYPE)documents[id]->textFile.fileEncoding.eol;

	//view_.updateStatus( *documents[id] );
}
*/

void EditorModel::insertDocument(const std::wstring& id, TextFile& textFile)
{
	activeDocument = id;
	documents[id] = std::make_shared<EditorDocument>(id, textFile);

	std::wstring token = fileService_.monitor.watch(
		textFile.filename,
		[this,id]() 
		{
			if (documents.count(id) == 0) return;

			std::wstring path = documents[id]->textFile.filename;
			this->onFileChanged.fire(id, path);
		}
	);

	/*
	auto scintilla = view_.createEditorWnd(id, path, textFile.utf8);


//	scintilla->set_text(textFile.utf8);

	scintilla->onNotify(SCN_MODIFIED, [this, id](NMHDR* nmhdr)
	{
		SCNotification* notify = (SCNotification*)nmhdr;
		if (notify->nmhdr.code == SCN_MODIFIED)
		{
			updateStatus(id);
		}
	});

	scintilla->onNotify(SCN_UPDATEUI, [this, id](NMHDR* nmhdr)
	{
		updateStatus(id);
	});
	
	updateStatus(id);


	HICON icon = mtl::shell::file_icon(documents[id]->textFile.filename);
	view_.mainWnd.set_icon(icon);

	std::wstring title = mtl::path(path).filename();
	view_.tabControl.add({ title, path, id }, scintilla->handle);
	*/
}

IO_ERROR EditorModel::openFile(const std::wstring& path, bool readOnly, long enc, std::function<void(EditorDocument)> cb)
{
	IO_ERROR ie = fileService_.read(path, enc, readOnly, [this, cb](IO_ERROR e, TextFile textFile)
	{
		if (e == IO_ERROR_SUCCESS)
		{
			std::wostringstream woss;
			woss << instanceId << ":" << mtl::new_guid();
			std::wstring id = woss.str();

			EditorDocument result;
			result.id = id;
			result.textFile = std::move(textFile);

			cb(result);
		}
	});
	return ie;
}


EditorDocument EditorModel::openNew()
{
	std::wostringstream woss;
	woss <<instanceId << ":" << mtl::new_guid();
	std::wstring id = woss.str();

	std::wstring fn = L"New File.txt";

	TextFile textFile;
	textFile.filename = fn;
	textFile.last_written = time(0);
	textFile.readonly = false;
	textFile.size = 0;
	textFile.fileEncoding.code_page = CP_UTF8;
	textFile.fileEncoding.eol = mtl::file_encoding::UNIX;
	textFile.fileEncoding.has_bom = false;
	textFile.fileEncoding.is_binary = false;

	EditorDocument doc;
	doc.id = id;
	doc.textFile = std::move(textFile);
	return doc;
}

void EditorModel::saveDocument(std::wstring id, std::wstring path, int enc, EOL_TYPE eol, std::string utf8, std::function<void(IO_ERROR)> cb)
{
	if (documents.count(id) == 0)
	{
		cb(IO_ERROR_NOT_FOUND);
		return;
	}

	auto doc = documents[id];
	TextFile textFile = doc->textFile;

	textFile.utf8 = utf8;
	textFile.size = utf8.size();
	textFile.filename = path;
	textFile.last_written = time(0);

	if (enc != -1)
	{
		textFile.fileEncoding.code_page = enc;
	}

	textFile.fileEncoding.eol = (mtl::file_encoding::eol_mode)eol;

	fileService_.write(textFile, [this,cb](IO_ERROR e)
	{
		cb(e);
	});
}

void EditorModel::reloadFile(const std::wstring& id, std::function<void(std::string)> cb)
{
	if (documents.count(id) == 0)
	{
		return;
	}

	std::wstring path = documents[id]->textFile.filename;
	bool readOnly = documents[id]->textFile.readonly;
	long enc = documents[id]->textFile.fileEncoding.code_page;
	openFile(path, readOnly, enc, [this,id,cb](EditorDocument doc) 
	{
		if (documents.count(id) == 0)
		{
			return;
		}

		cb(doc.textFile.utf8);
	});
}

/* ------------------------------------------- */

// go WinMain, go!

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

	int r = 0;

	{
		mtl::application app(hInstance);


		FileService fileService;
		RotService rotService;
		ScriptService scriptService(fileService);

		EditorController controller(opt, fileService, rotService, scriptService);

		mtl::rotten<IMTLEditor> editor(__uuidof(MTLEditor), controller.model.instanceId);
		mtl::punk<IMTLEditor> mtlEditor(new MTLEditor(&controller));
		editor = mtlEditor;
		controller.editor = *editor;

		scriptService.start(controller.editor);


		mtl::accelerator() = mtl::accelerators(*controller.view.mainWnd, IDC_EDITOR);

		r = app.run(mtl::accelerator());
	}
	return r;
}


Script::Script(ScriptService& service, FileService& fs,  const std::wstring& i, const std::wstring& s, const std::wstring& fn)
	: scriptService_(service), fileService_(fs), id_(i), source_(s), filename_(fn)
{
	mtl::punk<MTLScriptHostObject> host(new MTLScriptHostObject(this) );
	host.query_interface( &hostObj_ );
}


void Script::close()
{
	wait_ = false;
	scripting_->close();
	scriptService_.erase(id_);
}