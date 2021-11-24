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
/*
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
		/ *
		if (jst == JsString)
		{
			const wchar_t* buf = nullptr;
			size_t len = 0;
			::JsStringToPointer(arguments[1], &buf, &len);

			msg = std::wstring(buf, len);
		}
		else
			* /
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
*/

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

	if (controller->model.documents.exists(id) == 0)
	{
		return S_OK;
	}

	std::wstring filename = controller->model.documents[id].textFile.filename;

	*fn = ::SysAllocString(filename.c_str());
	return S_OK;
}

HRESULT __stdcall MTLEditorDocument::get_content(BSTR* cnt)
{
	if (!cnt) return E_INVALIDARG;

	*cnt = 0;

	std::string bytes;
	if (controller->model.documents.exists(id) != 0)
	{
		bytes = controller->view.documentViews[id]->get_text();
	}

	*cnt = ::SysAllocString(mtl::to_wstring(bytes).c_str());
	return S_OK;
}

HRESULT __stdcall MTLEditorDocuments::get_count(long* cnt)
{
	*cnt = (long)controller->model.documents.count();
	return S_OK;
}

HRESULT __stdcall MTLEditorDocuments::item(VARIANT idx, IMTLEditorDocument** doc)
{
	if (idx.vt == VT_BSTR)
	{
		mtl::variant_view vv(idx);
		auto id = vv.to_wstring();
		if (controller->model.documents.exists(id))
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
		if (controller->model.documents.exists(id))
		{
			controller->doRemoveDocument(id);
		}
	}
	return S_OK;
}


/* ------------------------------------ */


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




		mtl::accelerator() = mtl::accelerators(*controller.view.mainWnd, IDC_EDITOR);

		r = app.run(mtl::accelerator());
	}
	return r;
}


