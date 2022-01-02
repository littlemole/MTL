#include "Service.h"
#include "com.h"
#include "View.h"

mtl::path FileService::find(const std::wstring& relative_path)
{
	mtl::path p(relative_path);
	if (p.exists())
	{
		return p;
	}

	mtl::path tmp(mtl::path_to_self_directory() + L"\\" + relative_path);
	if (tmp.exists())
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

mtl::path FileService::find_in(const std::wstring& relative_path, const std::wstring& root)
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

IO_ERROR FileService::read(const std::wstring& path, long encoding, bool readonly, std::function<void(IO_ERROR, TextFile)> cb)
{
	mtl::path p = find(path);
	if (!p)
	{
		return IO_ERROR_NOT_FOUND;
	}

	mtl::file file(GENERIC_READ);
	DWORD r = file.open(p, OPEN_EXISTING);
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

IO_ERROR FileService::write(TextFile textFile, std::function<void(IO_ERROR)> cb)
{
	mtl::file file(GENERIC_READ | GENERIC_WRITE);

	DWORD r = file.open(textFile.filename, OPEN_ALWAYS | TRUNCATE_EXISTING);
	if (r != ERROR_SUCCESS)
	{
		return write_admin(textFile, cb);
	}

	std::string raw_bytes = mtl::encode_utf8(textFile.utf8, textFile.fileEncoding);

	r = file.async_content(raw_bytes, [this, cb](DWORD e) mutable
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

IO_ERROR FileService::read_admin(const std::wstring& path, long encoding, bool readonly, std::function<void(IO_ERROR, TextFile)> cb)
{
	return IO_ERROR_ACCESS_DENIED;
}


IO_ERROR FileService::write_admin(TextFile textFile, std::function<void(IO_ERROR)> cb)
{
	return IO_ERROR_ACCESS_DENIED;
}



EditorDocument RotService::transferTab(std::wstring instance, const std::wstring& from)
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

	result = EditorDocument{ newId,textFile, L"" };

	docs->remove(vId);
	return result;
}



void report_exception(mtl::chakra::active_ctx& ctx, std::function<void(long, long, std::wstring, std::wstring)>& onError)
{
	::JsValueRef ex = ctx.getAndClearException();
	mtl::chakra::value tmp(ex);
	std::wstring x = tmp.to_string();

	::JsValueRef names = nullptr;
	::JsGetOwnPropertyNames(ex, &names);
	mtl::chakra::value nameValues(names);

	int len = mtl::chakra::value(nameValues[L"length"]).as_int();

	std::wostringstream woss;
	for (int i = 0; i < len; i++)
	{
		std::wstring key = mtl::chakra::value(nameValues[i]).to_string();
		std::wstring s = mtl::chakra::value(tmp[key]).to_string();
		woss << key << L":" << s << std::endl;
	}

	mtl::ui_thread().submit([cb = onError, msg = woss.str(), x]() {
		cb(0, 0, msg, x);
	});
}


Script::Script(
	MainWindow* mainWnd, 
	ScriptService& service, 
	FileService& fs, 
	const std::wstring& i, 
	const std::wstring& s, 
	const std::wstring& fn )
	: 
		scriptService_(service), 
		fileService_(fs), 
		id_(i), 
		source_(s), 
		filename_(fn), 
		mainWnd_(mainWnd)
{}


Script::~Script()
{
	dispose();
}

void Script::dispose()
{
	wait_ = false;
	quit_ = true;
	for (auto& it : timeouts_)
	{
		mtl::timer::cancel(it);
	}
}


void Script::close()
{
	scriptService_.kill(id_);
}



JsValueRef CALLBACK Script::msgBoxCallback(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, void* callbackState)
{
	Script* script = (Script*)callbackState;

	auto lock = script->shared_from_this();
	
	if (script->quit_) return mtl::chakra::value::from_int(IDCANCEL);

	std::wstring title = L"title";
	std::wstring msg = L"msg";
	long options = 0;

	mtl::chakra::active_ctx ctx(*script->scriptContext);

	if (argumentCount > 1)
	{
		msg = mtl::chakra::value(arguments[1]).to_string();
	}
	if (argumentCount > 2)
	{
		title = mtl::chakra::value(arguments[2]).to_string();
	}
	if (argumentCount > 3)
	{
		options = mtl::chakra::value(arguments[3]).as_int();
	}

	int r = (int) ::MessageBox(script->mainWnd_->handle, msg.c_str(), title.c_str(), options);

	return mtl::chakra::value::from_int(r);
}


JsValueRef CALLBACK Script::waitCallback(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, void* callbackState)
{
	Script* script = (Script*)callbackState;

	auto lock = script->shared_from_this();

	script->wait(true);

	return JS_INVALID_REFERENCE;
}

void Script::promiseContinuation(JsValueRef task)
{
	JsValueRef ctx = nullptr;
	::JsGetContextOfObject(task, &ctx);
	::JsAddRef(ctx,nullptr);

	scriptService_.submit([this,task, ctx]()
	{
		{
			mtl::chakra::active_ctx ac(ctx);

			JsValueRef result = nullptr;
			JsValueRef global = nullptr;

			::JsGetGlobalObject(&global);
			::JsCallFunction(task, &global, 1, &result);

			if (ac.hasException())
			{
				report_exception(ac, onError_);
			}
		}
		::JsRelease(ctx, nullptr);
	});
}

void CALLBACK Script::PromiseContinuationCallback(JsValueRef task, void* callbackState)
{
	Script* script = (Script*)callbackState;
	script->promiseContinuation(task);
}


JsValueRef CALLBACK Script::CreateObjectCallback(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, void* callbackState)
{
	if (argumentCount < 2)
	{
		return JS_INVALID_REFERENCE;
	}

	Script* script = (Script*)callbackState;
	auto lock = script->shared_from_this();
	if (script->quit_) return JS_INVALID_REFERENCE;

	std::wstring progid = mtl::chakra::value(arguments[1]).to_string();

	mtl::punk<IUnknown> unk;
	HRESULT hr = unk.create_object(progid);
	if (hr != S_OK) return JS_INVALID_REFERENCE;
	if (!unk) return JS_INVALID_REFERENCE;

	mtl::punk<IDispatch> disp(unk);
	if (!disp) return JS_INVALID_REFERENCE;

	mtl::variant v(disp);
	JsValueRef ref = JS_INVALID_REFERENCE;
	::JsVariantToValue(&v, &ref);
	return ref;
}


JsValueRef CALLBACK Script::WinRTCallback(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, void* callbackState)
{
	if (argumentCount < 2)
	{
		return JS_INVALID_REFERENCE;
	}

	Script* script = (Script*)callbackState;
	auto lock = script->shared_from_this();
	if (script->quit_) return JS_INVALID_REFERENCE;

	std::wstring ns = mtl::chakra::value(arguments[1]).to_string();
	::JsProjectWinRTNamespace(ns.c_str());

	mtl::chakra::value r = script->eval(ns);
	return *r;
}


JsValueRef CALLBACK Script::importCallback(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, void* callbackState)
{
	if (argumentCount < 2)
	{
		return JS_INVALID_REFERENCE;
	}

	std::wstring src = mtl::chakra::value(arguments[1]).to_string();

	Script* script = (Script*)callbackState;

	auto lock = script->shared_from_this();

	if (script->quit_ == true) return JS_INVALID_REFERENCE;

//	mtl::ui_thread().submit([script,src]()
	//{
	JsValueRef result =	script->importSource(src);
	//});

	return result;

	//return JS_INVALID_REFERENCE;
}

JsValueRef CALLBACK Script::quitCallback(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, void* callbackState)
{
	Script* script = (Script*)callbackState;

	auto lock = script->shared_from_this();

	if (script->quit_ == true) return JS_INVALID_REFERENCE;

	mtl::ui_thread().submit([script]() 
	{
		script->close();
	});


	return JS_INVALID_REFERENCE;
}


JsValueRef CALLBACK Script::pipeCallback(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, void* callbackState)
{
	if (argumentCount < 3) return JS_INVALID_REFERENCE;

	Script* script = (Script*)callbackState;

	auto lock = script->shared_from_this();

	if (script->quit_ == true) return JS_INVALID_REFERENCE;

	std::wstring cli = mtl::chakra::value(arguments[1]).as_string();
	mtl::variant cb = mtl::chakra::value(arguments[2]).as_variant();

	if(cb.vt != VT_DISPATCH) return JS_INVALID_REFERENCE;

//	mtl::


	mtl::ui_thread().submit([script]()
	{
		script->close();
	});


	return JS_INVALID_REFERENCE;
}

JsValueRef CALLBACK Script::timeoutCallback(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, void* callbackState)
{
	Script* script = (Script*)callbackState;

	auto lock = script->shared_from_this();

	if (script->quit_ == true) return JS_INVALID_REFERENCE;

	if(argumentCount < 3) return JS_INVALID_REFERENCE;

	mtl::chakra::active_ctx ctx(*script->scriptContext);
	long ms = mtl::chakra::value(arguments[1]).as_int();

	mtl::chakra::value fun = arguments[2];
	mtl::chakra::value that = callee;

	std::weak_ptr<Script> weak(lock);

	UINT_PTR id = mtl::timer::set_timeout(ms, [weak,fun,that](UINT_PTR id)
	{
		auto s = weak.lock();

		if (s)
		{
			if (s->quit_ == true)
			{
				s->timeouts_.erase(id);
				return;
			}

			// double dispatch to ensure timer callback returns and keeps up msg loop
			s->scriptService_.submit([that,fun,weak,id]() 
			{
				auto s = weak.lock();

				if (s)
				{
					s->timeouts_.erase(id);

					if (s->quit_ == true)
					{
						return;
					}

					mtl::chakra::active_ctx ctx(*s->scriptContext);

					::JsValueRef t = *that;
					::JsValueRef r = nullptr;
					::JsValueRef f = *fun;
					JsErrorCode ec = ::JsCallFunction(f, &t, 1, &r);

					if (ctx.hasException())
					{
						report_exception(ctx, s->onError_);
					}
				}
			});
		}
	});

	script->timeouts_.insert(id);
	return JS_INVALID_REFERENCE;
}

bool Script::run(IUnknown* obj)
{
	scriptContext = rt.make_context();

	mtl::punk<IDispatch> disp(obj);
	mtl::variant var(*disp);

	{
		mtl::chakra::active_ctx ctx(*scriptContext);

		JsErrorCode jse = ::JsSetPromiseContinuationCallback(&Script::PromiseContinuationCallback, this);

		mtl::chakra::value globalObject(ctx.global());

		globalObject[L"Import"] = ctx.make_fun(&Script::importCallback, this);
		globalObject[L"MsgBox"] = ctx.make_fun(&Script::msgBoxCallback,this);
		globalObject[L"Wait"] = ctx.make_fun(&Script::waitCallback, this);
		globalObject[L"Quit"] = ctx.make_fun(&Script::quitCallback, this);
		globalObject[L"setTimeout"] = ctx.make_fun(&Script::timeoutCallback, this);
		globalObject[L"winRT"] = ctx.make_fun(&Script::WinRTCallback, this);
		globalObject[L"createObject"] = ctx.make_fun(&Script::CreateObjectCallback, this);

		::JsValueRef hostObject = mtl::chakra::value::from_variant(&var);
		globalObject[L"Application"] = hostObject;
		//globalObject[L"HelloWorld"] = mtl::chakra::value::from_string(L"Wonderful World");

		::JsValueRef result = ctx.run(source_, filename_);

		if (ctx.hasException())
		{
			report_exception(ctx, onError_);
			wait_ = false;
		}
	}
	if (!wait_)
	{
		quit_ = true;
		return true;
	}

	return false;
}

/*
UINT_PTR Script::set_timeout(unsigned int ms, IDispatch* cb)
{
	if (quit_ == true) return 0;

	mtl::punk<IDispatch> disp(cb);
	std::shared_ptr<Script> that = this->shared_from_this();

	UINT_PTR id = mtl::timer::set_timeout(ms, [this, that, disp](UINT_PTR id)
	{
		mtl::chakra::active_ctx ctx(*scriptContext);

		auto lock = that;

		that->timeouts_.erase(id);

		if (lock->quit_ == true)
		{
			return;
		}

		IDispatch* d = *disp;
		if (d)
		{
			try {
				mtl::automation call(d, DISPID_VALUE);
				call.invoke();
			}
			catch (HRESULT hr)
			{
				mtl::punk<IErrorInfo> ei;
				::GetErrorInfo(0, &ei);
				if (ei)
				{
					mtl::bstr desc;
					ei->GetDescription(&desc);

					std::wstring msg = desc.str();
					mtl::ui_thread().submit([cb = onError_,msg]() {
						cb(0, 0, msg, L"script error");
					});
				}
				else if (scriptContext.hasException())
				{
					report_exception(ctx, onError_);
				}
				else
				{
					mtl::ui_thread().submit( [cb = onError_]() {
						cb(0, 0, L"error in timeout", L"script error");
					});
				}
				close();
			}
		}
	});

	timeouts_.insert(id);

	return id;
}
*/

JsValueRef Script::eval(std::wstring src)
{
	mtl::chakra::active_ctx ctx(*scriptContext);

	JsValueRef result = ctx.run(src, L"");
	if (ctx.hasException())
	{
		report_exception(ctx, onError_);
	}
	return result;
}

JsValueRef Script::importSource(std::wstring file)
{
	std::wstring root = mtl::path(filename_).parent_dir();

	mtl::path p = fileService_.find_in(file, root);

	std::string content = mtl::slurp(p.str());

	mtl::chakra::active_ctx ctx(*scriptContext);

	::JsValueRef result = ctx.run(mtl::to_wstring(content), file);
	if (ctx.hasException())
	{
		report_exception(ctx, onError_);
	}

	return result;
}

void Script::onError(std::function<void(long, long, std::wstring, std::wstring)> cb)
{
	onError_ = cb;
}


/* -------------------------------- */


ScriptService::ScriptService( FileService& service)
	:  fileService(service)
{
}

ScriptService::~ScriptService()
{
	if (worker_.joinable())
	{
		worker_.join();
	}
}


void ScriptService::start(mtl::punk<IUnknown> unk, MainWindow* wnd)
{
	mainWnd = wnd;
	worker_ = std::thread(&ScriptService::threadfun, this, mtl::proxy<IUnknown>(unk));
}

void ScriptService::stop()
{
	if (worker_.joinable())
	{
		box_.stop();
	}
}

void ScriptService::run(std::wstring scriptSource, std::wstring filename, std::function<void(long, long, std::wstring, std::wstring)> onError)
{
	box_.submit([this, scriptSource, filename, onError]()
	{
		std::wstring id = mtl::new_guid();
		Script* script = new Script( mainWnd, *this, fileService, id, scriptSource, filename);
		script->onError(onError);

		scripts[id] = std::shared_ptr<Script>(script);
		bool done = scripts[id]->run( *unk_ );
		if (done)
		{
			scripts.erase(id);
		}
	});
}


void ScriptService::getScripts(std::function<void(std::map<std::wstring, std::wstring>)> cb)
{
	box_.submit([this, cb]()
	{
		std::map<std::wstring, std::wstring> result;

		for (auto& it : scripts)
		{
			result[it.first] = it.second->filename();
		}

		mtl::ui_thread().submit([cb, result]()
		{
			cb(result);
		});
	});
}

void ScriptService::kill(std::wstring id)
{
	box_.submit([this, id]()
	{
		erase(id);
	});
}


bool ScriptService::isScript(const std::wstring& id)
{
	return scripts.count(id) > 0;
}

void ScriptService::erase(std::wstring id)
{
	if (scripts.count(id))
	{
		std::shared_ptr<Script> s = scripts[id];
		s->dispose();
		scripts.erase(id);
		//s->close(); // beware recursion ! :(
	}
}



void ScriptService::threadfun(mtl::proxy<IUnknown> p)
{
	mtl::STA enter;

	unk_ = *p;
	box_.run();

	scripts.clear();
	unk_.release();
}


