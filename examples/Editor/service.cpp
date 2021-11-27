#include "Service.h"
#include "com.h"

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

	result.id = newId;
	result.textFile = std::move(textFile);

	docs->remove(vId);
	return result;
}


Script::Script(HWND mainWnd, ScriptService& service, FileService& fs, const std::wstring& i, const std::wstring& s, const std::wstring& fn)
	: scriptService_(service), fileService_(fs), id_(i), source_(s), filename_(fn), hWnd(mainWnd)
{
}


Script::~Script()
{
	dispose();
}

void Script::dispose()
{
	for (auto& it : timeouts_)
	{
		mtl::timer::cancel(it);
	}
}

bool Script::run(mtl::punk<IUnknown> obj)
{
	mtl::punk<MTLScriptHostObject> host(new MTLScriptHostObject(hWnd, this->shared_from_this(), scriptService_));
	host.query_interface(&hostObj_);

	scriptContext = rt.make_context();

	mtl::punk<IDispatch> disp(obj);
	mtl::variant var(*disp);

	{
		mtl::chakra::active_ctx ctx(*scriptContext);
		mtl::chakra::value globalObject(ctx.global());

		::JsValueRef hostObject = mtl::chakra::value::from_variant(&var);

		disp = hostObj_;
		mtl::variant var2(*disp);
		::JsValueRef host2Object = mtl::chakra::value::from_variant(&var2);

		globalObject[L"Application"] = hostObject;
		globalObject[L"Chakra"] = host2Object;
		globalObject[L"HelloWorld"] = mtl::chakra::value::from_string(L"Wonderful World");

		std::wstring tmp = mtl::chakra::value(globalObject[L"HelloWorld"]).to_string();

		::JsValueRef result = ctx.run(source_, filename_);

		if (ctx.hasException())
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

			//::MessageBox(0, woss.str().c_str(), x.c_str(), 0);

			mtl::ui_thread().submit([cb = onError_, msg = woss.str(), x]() {
				cb(0, 0, msg, x);
			});
			wait_ = false;
		}
	}
	if (!wait_)
	{
		return true;
	}

	return false;
}

UINT_PTR Script::set_timeout(unsigned int ms, IDispatch* cb)
{
	mtl::punk<IDispatch> disp(cb);
	std::shared_ptr<Script> that = this->shared_from_this();

	UINT_PTR id = mtl::timer::set_timeout(ms, [this,that, disp](UINT_PTR id)
	{
		that->timeouts_.erase(id);
		DISPPARAMS dispParams = { 0, 0, 0, 0 };
		IDispatch* d = *disp;
		if (d)
		{
			EXCEPINFO ex;
			::ZeroMemory(&ex, sizeof(ex));

			HRESULT hr = d->Invoke(DISPID_VALUE, IID_NULL, 0, DISPATCH_METHOD, &dispParams, 0, &ex, 0);
			if (hr!= S_OK)
			{
				if (scriptContext.hasException())
				{
					::JsValueRef ex = scriptContext.getAndClearException();
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

					//::MessageBox(0, woss.str().c_str(), x.c_str(), 0);

					mtl::ui_thread().submit([cb = onError_, msg = woss.str(), x]() {
						cb(0, 0, msg, x);
					});
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


void Script::close()
{
	wait_ = false;
	//scriptContext.dispose();
	//rt.dispose();	
	dispose();
	scriptService_.erase(id_);
}


void Script::importSource(std::wstring file)
{
	std::wstring root = mtl::path(filename_).parent_dir();

	mtl::path p = fileService_.find_in(file, root);

	std::string content = mtl::slurp(p.str());

	mtl::chakra::active_ctx ctx(*scriptContext);

	::JsValueRef result = ctx.run(mtl::to_wstring(content), file);
}

void Script::onError(std::function<void(long, long, std::wstring, std::wstring)> cb)
{
	onError_ = cb;
}



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


void ScriptService::start(mtl::punk<IUnknown> unk, HWND hWnd)
{
	mainWnd = hWnd;
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
		bool done = scripts[id]->run(unk_);
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


