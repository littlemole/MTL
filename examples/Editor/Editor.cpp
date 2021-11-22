#include "Editor.h"
#include "Service.h"
#include "Model.h"
#include "View.h"
#include "Controller.h"
#include "com.h"


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




/* -------------------------------- */

/* ----------------------------------------------- */

JsValueRef CALLBACK Echo(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, void* callbackState)
{
	if (argumentCount < 2)
	{
		return JS_INVALID_REFERENCE;
	}

	std::wstring msg;

	mtl::chakra::value val(arguments[1]);

	JsValueType jst = val.type();
	//	::JsGetValueType(arguments[1], &jst);
		/*
		if (jst == JsString)
		{
			const wchar_t* buf = nullptr;
			size_t len = 0;
			::JsStringToPointer(arguments[1], &buf, &len);

			msg = std::wstring(buf, len);
		}
		else
			*/
	{
		mtl::variant var = val.as_variant();
		//		JsErrorCode ec = ::JsValueToVariant(arguments[1], &var);
		//		if (ec != JsNoError)
		{
			//			::MessageBox(0, L"ERRO CREATE VARIANT", L"x", 0);
		}
		msg = var.to_wstring();
	}

	mtl::ui_thread().submit([msg]()
	{
		::MessageBox(0, msg.c_str(), L"ALERT", 0);
	});
	return JS_INVALID_REFERENCE;
}

/*

		view.mainWnd.onCmd(ID_EDIT_PASTE, [this]()
		{
			bool toggle = !view.mainWnd.menu.item(ID_EDIT_PASTE).checked;
			view.mainWnd.menu.item(ID_EDIT_PASTE).check(toggle);
		});
*/

// Controller handles User Input
// Controller owns Model and View


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
			controller->doRemoveDocument(id);
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

	documents[id]->fileWatchToken = token;

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

	fileService_.monitor.unwatch(doc->fileWatchToken, path);

	fileService_.write(textFile, [this,id,textFile,cb](IO_ERROR e)
	{
		mtl::timer::set_timeout(500, [this, id, textFile, cb, e](UINT_PTR)
		{
			std::wstring token = fileService_.monitor.watch(
				textFile.filename,
				[this, id]()
			{
				if (documents.count(id) == 0) return;

				std::wstring path = documents[id]->textFile.filename;
				this->onFileChanged.fire(id, path);
			}
			);

			documents[id]->fileWatchToken = token;

			cb(e);
		});
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


