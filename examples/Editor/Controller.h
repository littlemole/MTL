#pragma once

#include "mvc.h"
#include "service.h"
#include "model.h"
#include "view.h"
#include "com.h"

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

	mtl::rotten<IMTLEditor> editor;


	void doRemoveDocument(std::wstring id)
	{
		if (!model.documents.exists(id)) return;

		std::wstring active = view.removeDocumentView(id);

		model.removeDocument(id);

		if (!active.empty())
		{
			view.updateStatus(model.documents[active]);
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
			view.updateStatus(model.documents[id]);
		});

		scintilla->onNotify(SCN_UPDATEUI, [this, id = doc.id](NMHDR* nmhdr)
		{
			view.updateStatus(model.documents[id]);
		});

		view.updateStatus(model.documents[doc.id]);
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
			if (!model.documents.exists(id)) return;

			view.documentViews[id]->set_text(utf8);
			view.documentViews[id]->colorize();

		});
	}

	void doShowHelp()
	{
		mtl::dialog dlg;
		dlg.show_modal(IDD_ABOUTBOX, *view.mainWnd);
	}

	void doActivateDocument(std::wstring id)
	{
		view.activate(model.documents[id]);
	}

	void doDragOut(std::wstring id)
	{
		//::MessageBox(*view.mainWnd, id.c_str(), L"DRAG OUT", 0);
		std::wostringstream woss;
		woss << L"/open " << id;
		mtl::shell::execute_args(mtl::path_to_self(), woss.str());
	};

	void doExit()
	{
		//			view.mainWnd.destroy();
		scriptService.stop();
		::PostQuitMessage(0);
	};

	void doOpenFileDialog()
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
	};

	void doSaveFileDialog()
	{
		if (view.activeDocument().empty()) return;

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
			doSaveDocument(view.activeDocument(), path, enc, eol);
		}
	};

	void doExecuteScript()
	{
		if (view.activeDocument().empty()) return;

		std::string utf8 = view.documentViews[view.activeDocument()]->get_text();
		std::wstring fn = model.documents[view.activeDocument()].textFile.filename;
		scriptService.run(mtl::to_wstring(utf8), fn.c_str(), [this](long line, long pos, std::wstring err, std::wstring src)
		{
			std::wostringstream oss;
			oss << err << L" line " << line << "\r\n";
			oss << src;
			::MessageBoxW(*view.mainWnd, oss.str().c_str(), src.c_str(), MB_ICONERROR);
		});
	};

	void doShowScriptDialog()
	{
		view.scriptDialog.scriptService = &scriptService;
		HWND hWnd = view.scriptDialog.show_modeless(IDD_DIALOG_SCRIPTS, *view.mainWnd);
		::ShowWindow(hWnd, SW_SHOW);
	};




	void doDrop( IDataObject* ido, DWORD keyState, DWORD& effect)
	{
		effect = DROPEFFECT_COPY;

		mtl::dataobj_view dov(ido);
		if (dov.has(view.tabControl.dragTabFormat))
		{
			std::wstring id = dov.wstring(view.tabControl.dragTabFormat);

			if (model.documents.exists(id))
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

			if (view.activeDocument().empty()) return;
			view.documentViews[view.activeDocument()]->insert_text(utf8);

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

			if (view.activeDocument().empty()) return;
			view.documentViews[view.activeDocument()]->insert_text(utf8);

			if ((keyState & MK_SHIFT) || (keyState & MK_CONTROL))
			{
				effect = DROPEFFECT_MOVE;
			}
			return;
		}
	}

	void doShowFileChangedDialog(std::wstring id, std::wstring path)
	{
		mtl::dialog	dlgFileChanged;
		dlgFileChanged.onInitDlg([this, path](mtl::dialog& dlg)
		{
			std::wostringstream woss;
			woss << L"File " << path << " changed on Disk.\r\n\r\nReload File?";
			dlg.set_dlg_item_text(IDC_FILE_CHANGED_LABEL, woss.str());

			std::wostringstream woss2;
			woss2 << L"File " << mtl::path(path).filename() << " changed on Disk!";
			dlg.set_text(woss2.str());

		});

		if (IDOK == dlgFileChanged.show_modal(IDD_FILE_CHANGED, *view.mainWnd))
		{
			doReloadFile(id);
		}
	}

	EditorController(mtl::options& opt, FileService& fs, RotService& rs, ScriptService& sc)
		: editor(__uuidof(MTLEditor), model.instanceId()),
		on(this),
		fileService(fs),
		rotService(rs),
		scriptService(sc),
		model(fs, rs, sc)
	{
		// create COM Editor and register in ROT
		mtl::punk<IMTLEditor> mtlEditor(new MTLEditor(this));
		editor = mtlEditor;

		// start scriptService 
		scriptService.start(mtlEditor, *view.mainWnd);

		bind();
		handleCommandline(opt);
	}

	void bind()
	{
		on(model.onFileChanged) = &EditorController::doShowFileChangedDialog;

		on(view.toolBar.onCommand)
			.when(IDM_RUN).then([this]() { doExecuteScript(); })
			.when(IDM_FILE_OPEN).then([this]() { doOpenFileDialog(); })
			.when(IDM_FILE_NEW).then([this]() { doCreateNew(); });

		on(view.tabControl.onDragOut) = &EditorController::doDragOut;
		on(view.tabControl.onCloseTab) = &EditorController::doRemoveDocument;
		on(view.tabControl.onSelect).then([this](mtl::tab_ctrl::tab& item)
		{
			if (view.tabControl.hit_icon_test())
			{
				return;
			}

			doActivateDocument(item.id);
		});

		on(view.mainWnd.taskbar.onClose) = &EditorController::doRemoveDocument;
		on(view.mainWnd.taskbar.onActivate).then([this](std::wstring id) 
		{
			if (model.documents.exists(id))
			{
				view.tabControl.select(id);
				doActivateDocument(id);
			}
		});



		on(view.mainWnd.onCmd)
			.when(IDM_EXIT).then([this]() { doExit(); })
			.when(IDM_SAVE).then([this]() { doSaveFileDialog(); })
			.when(IDM_FILE_NEW).then([this]() { doShowScriptDialog(); })
			.when(IDM_EDIT_FIND).then([this]() { view.doShowFindReplaceDialog(IDM_EDIT_FIND); })
			.when(IDM_EDIT_FIND_REGEX).then([this]() { view.doShowFindReplaceDialog(IDM_EDIT_FIND_REGEX); })
			.when(IDM_EDIT_REPLACE).then([this]() { view.doShowFindReplaceDialog(IDM_EDIT_REPLACE); })
			.when(IDM_EDIT_REPLACE_REGEX).then([this]() { view.doShowFindReplaceDialog(IDM_EDIT_REPLACE_REGEX); })
			.when(IDM_ABOUT).then([this]() { doShowHelp(); });


		on(view.dropTarget->onDrop).then([this](IDataObject* ido, DWORD keyState, DWORD& effect)
		{
			doDrop(ido, keyState, effect);
		});


		//on(view.mainWnd.onCmd).when(IDM_ABOUT) = &EditorController::doShowHelp;

		/*
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


		on(view.mainWnd.onCmd)
			.when(IDM_SAVE)
			.then([this]()
		{
			doSaveFileDialog();
		});

		on(view.mainWnd.onCmd)
			.when(IDM_EDIT_FIND)
			.then([this]()
		{
			doShowFindReplaceDialog(IDM_EDIT_FIND);
		});

		on(view.mainWnd.onCmd)
			.when(IDM_EDIT_FIND_REGEX)
			.then([this]()
		{
			doShowFindReplaceDialog(IDM_EDIT_FIND_REGEX);
		});

		on(view.mainWnd.onCmd)
			.when(IDM_EDIT_REPLACE)
			.then([this]()
		{
			doShowFindReplaceDialog(IDM_EDIT_REPLACE);
		});

		on(view.mainWnd.onCmd)
			.when(IDM_EDIT_REPLACE_REGEX)
			.then([this]()
		{
			doShowFindReplaceDialog(IDM_EDIT_REPLACE_REGEX);
		});


		view.toolBar.onCommand(IDM_RUN, [this]()
		{
			doExecuteScript();
		});

		on(view.mainWnd.onFind)
			.then([this](FINDREPLACE* fr)
		{
			doFind(fr);
		});

		on(view.mainWnd.onReplace)
			.then([this](FINDREPLACE* fr)
		{
			doReplace(fr);
		});

		* /
		view.toolBar.onCommand(IDM_FILE_OPEN, [this]()
		{
			doOpenFileDialog();
		});

		view.toolBar.onCommand(IDM_FILE_NEW, [this]()
		{
			doCreateNew();
		});

		view.mainWnd.onCmd(IDM_FILE_NEW, [this]()
		{
			doShowScriptDialog();
		});

		view.mainWnd.onCmd(IDM_EXIT, [this]()
		{
			doExit();
		});


		view.tabControl.onDragOut([this](std::wstring id)
		{
			doDragOut(id);
		});


		view.tabControl.onCloseTab([this](std::wstring id)
		{
			doRemoveDocument(id);
		});
*/
/*
		view.mainWnd.onCmd(IDM_SAVE_AS, [this]()
		{
			::MessageBox(*view.mainWnd, L"SAVE AS", L"x", 0);
		});


		view.tabControl.onNotify(NM_CLICK, [this](NMHDR* hmhdr)
		{
			//::MessageBox(*view.mainWnd, L"NM_CLICK", L"tabControl.onNotify", 0);
		});
		*/
		/*
		view.tabControl.onSelect([this](mtl::tab_ctrl::tab& item)
		{
			if (view.tabControl.hit_icon_test())
			{
				return;
			}

			doActivateDocument(item.id);
		});

		view.dropTarget->onDrop([this](IDataObject* ido, DWORD keyState, DWORD& effect)
		{
			doDrop(ido, keyState, effect);
		});
		*/
	}

	void handleCommandline(mtl::options& opt)
	{
		if (opt.has(L"open"))
		{
			for (auto uid : opt.args())
			{
				uid = mtl::url_unescape(uid);
				if (!uid.empty() && uid.back() == '/')
				{
					uid = uid.substr(0, uid.size() - 1);
				}

				if (uid.starts_with(L"mte://"))
				{
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

