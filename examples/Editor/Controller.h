#pragma once

#include "mvc.h"
#include "service.h"
#include "model.h"
#include "view.h"

class connector
{
public:

	template<class T>
	class connect;

	template<class T, class ... Args>
	class connect<T(Args...)>
	{
	public:

		connect(void* t, mtl::event<T(Args...)>& e)
			: host_(t), e_(e)
		{}

		connect<T(Args...)>& when(int id)
		{
			id_ = id;
			return *this;
		}

		connect<T(Args...)>& then(std::function<void(Args...)> fun)
		{
			e_(id_, [fun](Args ... args)
			{
				fun(args...);
			});
			return *this;
		}

		template<class C>
		connect<T(Args...)>& then(void (C::* fun)(Args...))
		{
			C* c = (C*)host_;
			e_(id_, [fun, c](Args ... args)
			{
				(c->*fun)(args...);
			});

			return *this;
		}

		connect<T(Args...)>& operator=(std::function<void(Args...)> fun)
		{
			then(fun);
			return *this;
		}

		template<class C>
		connect<T(Args...)>& operator=(void (C::* fun)(Args...))
		{
			then(fun);
			return *this;
		}

	private:
		void* host_ = nullptr;
		mtl::event<T(Args...)>& e_;
		int id_ = 0;
	};

	template<class ... Args>
	class connect<void(Args...)>
	{
	public:

		connect(void* t, mtl::event<void(Args...)>& e)
			: host_(t), e_(e)
		{}

		void then(std::function<void(Args...)> fun)
		{
			e_([fun](Args ... args)
			{
				fun(args...);
			});
		}

		template<class C>
		void then( void (C::*fun)(Args...) )
		{
			C* c = (C*)host_;
			e_([fun,c](Args ... args)
			{
				(c->*fun)(args...);
			});
		}

		connect<void(Args...)>& operator=(std::function<void(Args...)> fun)
		{
			then(fun);
			return *this;
		}

		template<class C>
		connect<void(Args...)>& operator=(void (C::* fun)(Args...))
		{
			then(fun);
			return *this;
		}

	private:
		void* host_ = nullptr;
		mtl::event<void(Args...)>& e_;
	};


	connector()
	{}


	connector(void* t)
		: host_(t)
	{}

	template<class T, class ... Args>
	auto with(mtl::event<T(Args...)>& e)
	{
		return connect<T(Args...)>{ host_, e };
	}


	template<class T, class ... Args>
	auto operator()(mtl::event<T(Args...)>& e)
	{
		return with(e);
	}

private:
	void* host_ = nullptr;
};

class EditorController
{
public:

	connector on;

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

/*	mtl::sink<int()> with;
	mtl::sink<void(FINDREPLACE*)> whenFind;
	mtl::sink<void(FINDREPLACE*)> whenReplace;
	mtl::sink<UINT(NMTOOLBAR*)> onToolbar;
	*/
	void doRemoveDocument(std::wstring id)
	{
		if (model.documents.count(id) == 0) return;

		std::wstring active = view.removeDocumentView(id);

		view.mainWnd.taskbar.remove(id);

		model.removeDocument(id, active);

		if (!model.activeDocument.empty())
		{
			view.updateStatus(*model.documents[active]);
		}

	}

	void doTransferDocument(const std::wstring& from, int to = -1)
	{
		EditorDocument document = model.transferDocument(from);

		if (document.id.empty()) return;

		doInsertDocument(document);
	}

	void doOpenFile(const std::wstring& path, bool readOnly, long enc)
	{
		model.openFile(path, readOnly, enc, [this](EditorDocument doc)
		{
			doInsertDocument(doc);
		});
	}


	void doInsertDocument(EditorDocument doc)
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

		view.mainWnd.taskbar.add(doc.id, scintilla->handle, doc.textFile.filename);

		view.updateStatus(*model.documents[doc.id]);
	}

	void doCreateNew()
	{
		EditorDocument doc = model.openNew();
		doInsertDocument(doc);
	}

	void doSaveDocument(std::wstring id, std::wstring path, long enc, EOL_TYPE eol)
	{
		std::string utf8 = view.documentViews[id]->get_text();
		model.saveDocument(id, path, enc, eol, utf8, [this, path](IO_ERROR io)
		{
			view.statusBar.set_text(std::wstring(L"file saved to ") + path);
		});
	}

	void doReloadFile(const std::wstring& id)
	{
		model.reloadFile(id, [this, id](std::string utf8)
		{
			if (model.documents.count(id) == 0) return;

			view.documentViews[id]->set_text(utf8);
			view.documentViews[id]->colorize();

		});
	}

	void doShowHelp()
	{
		mtl::dialog dlg;
		dlg.show_modal(IDD_ABOUTBOX, *view.mainWnd);
	}

	void doShowFileChangedDialog(std::wstring id, std::wstring path)
	{
		mtl::dialog	dlgFileChanged;
		dlgFileChanged.onInitDlg([this, path](mtl::dialog& dlg)
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
			doReloadFile(id);
		}
	}

	EditorController(mtl::options& opt, FileService& fs, RotService& rs, ScriptService& sc)
		: on(this),
		fileService(fs),
		rotService(rs),
		scriptService(sc),
		model(fs, rs, sc)
		//editor(__uuidof(MTLEditor),model.instanceId)
	{
		//scripting = new mtl::active_script(L"JScript");

		//mtl::punk<IMTLEditor> mtlEditor(new MTLEditor(this));
		//editor = mtlEditor;


		on(model.onFileChanged).then([this](std::wstring id, std::wstring path)
		{
			doShowFileChangedDialog(id, path);
		});

		on(view.mainWnd.taskbar.onClose).then([this](std::wstring id)
		{
			doRemoveDocument(id);
		});


		on(view.mainWnd.taskbar.onActivate).then([this](std::wstring id)
		{
			::SetForegroundWindow(*view.mainWnd);
			view.tabControl.select(id);
		});

		/*
		view.mainWnd.taskbar.onActivate([this](std::wstring id)
		{
			::SetForegroundWindow(*view.mainWnd);
			view.tabControl.select(id);
		});
		*/
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

		on(view.mainWnd.onCmd).when(IDM_ABOUT) = &EditorController::doShowHelp;
			
			/*.then([this]()
		{
			mtl::dialog dlg;
			dlg.show_modal(IDD_ABOUTBOX, *view.mainWnd);

		});
		*/

		view.statusBar.onLeftClick(MyStatusBar::EOL, [this]()
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

		on(view.mainWnd.onCmd)
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
				doSaveDocument(model.activeDocument, path, enc, eol);
			}

		});

		on(view.mainWnd.onCmd)
			.when(IDM_EDIT_FIND)
			.then([this]()
		{
			if (model.activeDocument.empty()) return;

			model.regexSearch = false;
			view.documentViews[model.activeDocument]->set_next_search_pos_(0);
			HWND hWnd = view.searchDlg.find(*view.mainWnd);
			::SetWindowText(hWnd, L"Search");
		});

		on(view.mainWnd.onCmd)
			.when(IDM_EDIT_FIND_REGEX)
			.then([this]()
		{
			if (model.activeDocument.empty()) return;

			model.regexSearch = true;
			view.documentViews[model.activeDocument]->set_next_search_pos_(0);
			HWND hWnd = view.searchDlg.find(*view.mainWnd);
			::SetWindowText(hWnd, L"RegExp Find");
		});

		on(view.mainWnd.onCmd)
			.when(IDM_EDIT_REPLACE)
			.then([this]()
		{
			if (model.activeDocument.empty()) return;

			model.regexSearch = false;
			view.documentViews[model.activeDocument]->set_next_search_pos_(0);
			HWND hWnd = view.searchDlg.replace(*view.mainWnd);
			::SetWindowText(hWnd, L"Replace");
		});

		on(view.mainWnd.onCmd)
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
				::MessageBoxW(*view.mainWnd, oss.str().c_str(), src.c_str(), MB_ICONERROR);
			});

			//scripting->run_script(mtl::to_wstring(utf8));

		});

		on(view.mainWnd.onFind)
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

		on(view.mainWnd.onReplace)
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
				while (view.documentViews[model.activeDocument]->replace(
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
			mtl::chakra::runtime runtime;
			//			JsRuntimeHandle runtime;
					//	JsContextRef context;
						/*if (JsNoError != ::JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime))
						{
							exit(0);
						}
			*/
			//	::JsCreateContext(*runtime, &context);

			mtl::chakra::active_ctx ctx(runtime.make_context());

			//	::JsSetCurrentContext(context);

			mtl::punk<IDispatch> disp(editor);
			mtl::variant var(*disp);

			//			::JsVariantToValue( &var, &hostObject);

			::JsValueRef hostObject = mtl::chakra::value::from_variant(&var);


			//			::JsCreateObject(&hostObject);

			mtl::chakra::value globalObject(ctx.global());

			//			::JsValueRef globalObject;
			//			::JsGetGlobalObject(&globalObject);


			globalObject[L"mte"] = hostObject;
			globalObject[L"MsgBox"] = ctx.make_fun(&Echo);
			globalObject[L"HelloWorld"] = mtl::chakra::value::from_string(L"Wonderful World");

			::JsProjectWinRTNamespace(L"Windows.Foundation");
			::JsProjectWinRTNamespace(L"Windows.Web");
			::JsProjectWinRTNamespace(L"Windows.UI.Popups");

			//::JsSetProjectionEnqueueCallback(&jsProjectionEnqueueCallback,this);

			std::wstring tmp = mtl::chakra::value(globalObject[L"HelloWorld"]).to_string();

			/*
						::JsPropertyIdRef hostPropertyId;
						::JsGetPropertyIdFromName(L"mte", &hostPropertyId);

						::JsSetProperty(*globalObject, hostPropertyId, hostObject, true);
			*/
			/*
						::JsPropertyIdRef echoPropertyId;
						::JsGetPropertyIdFromName(L"MsgBox", &echoPropertyId);


						JsNativeFunction echo = &Echo;

						JsValueRef function;
						JsErrorCode ec = ::JsCreateFunction( echo, nullptr, &function);
						if (ec != JsNoError)
						{
							::MessageBox(*view.mainWnd, L"ERRO CREATE FUN", L"x", 0);
						}
			*/
			//	::JsSetProperty(*globalObject, echoPropertyId, function, true);


				//::JsSetCurrentContext(JS_INVALID_REFERENCE);

			std::string utf8 = view.documentViews[model.activeDocument]->get_text();
			std::wstring fn = model.documents[model.activeDocument]->textFile.filename;

			unsigned currentSourceContext = 0;

			::JsValueRef result = ctx.run(L"function to_hell(s) { mte.SayHello(s); }", fn);

			result = ctx.run(mtl::to_wstring(utf8), fn);
			/*
						::JsValueRef result;
						if (JsNoError != ::JsRunScript(L"function to_hell(s) { mte.SayHello(s); }", currentSourceContext, fn.c_str(), &result))
						{
							::MessageBox(*view.mainWnd, L"JS ERROR", L"ERR 1", MB_ICONERROR);
						}

						::JsValueRef result2;
						if (JsNoError != ::JsRunScript(mtl::to_wstring(utf8).c_str(), currentSourceContext, fn.c_str(), &result2))
						{
							::MessageBox(*view.mainWnd, L"JS ERROR", L"ERR 2", MB_ICONERROR);
						}
			*/
			//	::JsSetCurrentContext(JS_INVALID_REFERENCE);

				//
				// Clean up the runtime.
				//

				//::JsDisposeRuntime(runtime);
				/*
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
				*/
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
				doOpenFile(path, ro, enc);
			}

		});

		view.toolBar.onCommand(IDM_FILE_NEW, [this]()
		{
			doCreateNew();
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
				//scriptDialog dlg(scriptService,scripts);
				view.scriptDialog.scripts = scripts;
				view.scriptDialog.scriptService = &scriptService;

				HWND hWnd = view.scriptDialog.show_modeless(IDD_DIALOG_SCRIPTS, *view.mainWnd);
				::ShowWindow(hWnd, SW_SHOW);

				/*
				std::wostringstream woss;
				for (auto& it : scripts)
				{
					woss << it.first << ":" << it.second << std::endl;
				}
				*/
				//::MessageBox(*view.mainWnd, woss.str().c_str(), L"Scripts", 0);
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


		view.tabControl.onCloseTab([this](std::wstring id)
		{
			doRemoveDocument(id);
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
			model.activeDocument = item.id;
			view.updateStatus(*model.documents[item.id]);
			view.mainWnd.taskbar.activate(item.id);
		});

		view.tabControl.onDropExternal([this](int index_to, IDataObject* d)
		{
			DWORD effect;
			view.dropTarget->onDrop.fire(d, 0, effect);
			return;
			mtl::dataobj_view dov(d);
			std::wstring from = dov.wstring(view.tabControl.dragTabFormat);

			//::MessageBox(0, id.c_str(), id.c_str(), 0);

			doTransferDocument(from, index_to);
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
			mtl::stg_medium stgm_taint((void*)&b, 1, GMEM_MOVEABLE | GMEM_NODISCARD);
			dao->SetData(&fetaint, &stgm_taint, FALSE);

			mtl::format_etc furl(urlFormat);
			mtl::stg_medium stgm_url(std::wstring(L"mte://") + tab.id);
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
					doTransferDocument(id, -1);
				}
				return;
			}
			mtl::format_etc_dropfile fdd;
			if (dov.has(fdd))
			{
				auto v = dov.dropFiles();
				for (auto& i : v)
				{
					doOpenFile(i, false, -1);
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
					doTransferDocument(uid, -1);
					return;
				}
				doOpenFile(uid, false, -1);
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
				doTransferDocument(uid, -1);
			}
		}

		if (opt.has(L"file"))
		{
			for (auto& p : opt.args())
			{
				doOpenFile(p, false, -1);
			}
		}
	}
};

