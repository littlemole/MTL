#pragma once

#include "mvc.h"

#include "mtl/disp/aut.h"
#include <DispEx.h>

class EditorController;

class MTLHtmlDocument :
	public mtl::implements<MTLHtmlDocument(mtl::dual<IMTLHtmlDocument>)>
{
public:

	std::wstring id;
	EditorController* controller = nullptr;

	mtl::punk<IDispatch> onLoadCb;
	mtl::punk<IDispatch> onCloseCb;

	mtl::punk<IDispatch> scriptController;
	mtl::punk<IDispatch> onMsg;
	mtl::punk<IDispatch> onNavigateCb;

	mtl::sink<void()> onLoadSink;
	mtl::sink<void(std::wstring)> onCloseSink;
	mtl::sink<void(std::wstring)> onMsgSink;
	mtl::sink<void(std::wstring,bool& cancel)> onNavigateSink;

	MTLHtmlDocument(const std::wstring& docId, EditorController* ctrl);

	virtual HRESULT __stdcall put_onLoad( IDispatch* cb)override
	{
		if (!cb)
		{
			onLoadCb.release();
			return S_OK;
		}
		onLoadCb = cb;
		return S_OK;
	}

	virtual HRESULT __stdcall put_onClose( IDispatch* cb) override
	{
		if (!cb)
		{
			onCloseCb.release();
			return S_OK;
		}
		onCloseCb = cb;
		return S_OK;
	}

	virtual HRESULT __stdcall put_Controller(IDispatch* cb) override;

	virtual HRESULT __stdcall get_Controller(IDispatch** cb) override
	{
		if (!cb) return E_INVALIDARG;
		*cb = 0;

		if (scriptController)
		{
			return scriptController.query_interface(cb);
		}
		return S_OK;
	}

	virtual HRESULT __stdcall  put_onMsg(IDispatch* cb) override
	{
		onMsg = cb;
		return S_OK;
	}

	virtual HRESULT __stdcall  postMsg(BSTR msg) override;

	virtual HRESULT __stdcall  put_onNavigate(IDispatch* disp) override
	{
		onNavigateCb = disp;
		return S_OK;
	}


	virtual HRESULT __stdcall close() override;

private:

	void clear();
};

class MTLEditorDocument :
	public mtl::implements<MTLEditorDocument(mtl::dual<IMTLEditorDocument>)>
{
public:

	std::wstring id;
	EditorController* controller = nullptr;

	MTLEditorDocument(const std::wstring& docId, EditorController* ctrl)
		: id(docId), controller(ctrl)
	{}

	virtual HRESULT __stdcall get_documentId(BSTR* docId) override;
	virtual HRESULT __stdcall get_filename(BSTR* fn) override;
	virtual HRESULT __stdcall get_content(BSTR* cnt) override;
	virtual HRESULT __stdcall get_obj(IDispatch** obj) override;
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
	virtual HRESULT __stdcall openHTML(BSTR file, IDispatch** cb);
};

/*
class MTLScriptHostObject :
	public mtl::implements<MTLScriptHostObject(mtl::dual<IMTLScriptHostObject>)>
{
public:

	MTLScriptHostObject(HWND mainWnd, std::shared_ptr<Script> script, ScriptService& service)
		: hWnd(mainWnd),script_(script), scriptService_(service)
	{
		id_ = script->id();
	}

	virtual HRESULT __stdcall Import(BSTR value) override
	{
		std::shared_ptr<Script> script = script_.lock();
		if (script)
		{
			script->importSource(mtl::bstr_view(value).str());
		}
		return S_OK;
	}

	virtual HRESULT __stdcall setTimeout(long ms, IDispatch* cb, long* id) override
	{
		std::shared_ptr<Script> script = script_.lock();
		if (script)
		{
			UINT_PTR i = script->set_timeout(ms, cb);
			if (id) *id = (long)i;
		}
		return S_OK;
	}

	virtual HRESULT __stdcall Wait() override
	{
		std::shared_ptr<Script> script = script_.lock();
		if (script)
		{
			script->wait(true);;
		}
		return S_OK;
	}

	virtual HRESULT __stdcall Quit() override
	{
		std::shared_ptr<Script> script = script_.lock();
		if (script)
		{
			if (scriptService_.isScript(id_))
			{
				script->close();
			}
		}
		return S_OK;
	}

	virtual HRESULT __stdcall CreateObject(BSTR progid, IDispatch** result) override
	{
		if (!result) return E_INVALIDARG;
		*result = nullptr;

		mtl::punk<IUnknown> unk;
		HRESULT hr = unk.create_object(mtl::bstr_view(progid).str());
		if (hr == S_OK)
		{
			return unk.query_interface(result);
		}
		return S_FALSE;
	}

	virtual HRESULT __stdcall WinRT(BSTR ns, VARIANT* result) override
	{
		::JsProjectWinRTNamespace(mtl::bstr_view(ns).str().c_str());

		std::shared_ptr<Script> script = script_.lock();
		if (script)
		{
			mtl::chakra::value r = script->eval(mtl::bstr_view(ns).str());
			::JsValueToVariant(*r, result);
		}
		return S_OK;
	}


	virtual HRESULT __stdcall MsgBox(BSTR text, BSTR title, long options, long* result) override
	{
		std::wstring txt = mtl::bstr_view(text).str();
		std::wstring caption = mtl::bstr_view(title).str();

		std::shared_ptr<Script> script = script_.lock();
		if (script)
		{
			long r = ::MessageBox(
				hWnd,
				txt.c_str(),
				caption.c_str(),
				options
			);

			if (result) *result = r;
		}
		return S_OK;
	}


private:
	HWND hWnd = nullptr;
	std::weak_ptr<Script> script_;
	std::wstring id_;
	ScriptService& scriptService_;
};

class __declspec(uuid("{982599D2-AC7E-45DB-8E17-95985EDFEBF2}")) IChakraDispWrapper : public IDispatch 
{
public:

	virtual HRESULT __stdcall unwrap(VARIANT* v) = 0;
};

class __declspec(uuid("{E52CA4D5-0052-4208-B2EB-06F09585799D}")) IV8DispWrapper : public IDispatchEx
{
public:

	virtual HRESULT __stdcall unwrap(VARIANT* v) = 0;
};

inline mtl::variant unwrap(VARIANTARG* v);

class V8DispWrapper : public mtl::implements<V8DispWrapper(IV8DispWrapper, mtl::of<IV8DispWrapper, IDispatch>, mtl::of<IV8DispWrapper, IDispatchEx>)>
{
public:

	mtl::punk<IDispatch> ref;

	V8DispWrapper(IDispatch* r)
		: ref(r)
	{
	}

	~V8DispWrapper()
	{
	}

	virtual HRESULT __stdcall GetTypeInfoCount(UINT* pctinfo) override
	{
		if (!pctinfo)
			return E_INVALIDARG;
		*pctinfo = 0;
		return S_OK;
	}
	virtual HRESULT __stdcall GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override
	{
		return E_NOTIMPL;
	}
	virtual HRESULT __stdcall GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override
	{
		if (!rgDispId) return E_INVALIDARG;

		return ref->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
		return DISP_E_UNKNOWNNAME;
	}

	virtual HRESULT __stdcall Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override
	{
		return ref->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
		return DISP_E_MEMBERNOTFOUND;
	}

	virtual HRESULT __stdcall  GetDispID( BSTR bstrName, DWORD grfdex, DISPID* pid) override
	{
		std::wstring s = mtl::bstr_view(bstrName).str();
		LPOLESTR str = (wchar_t*)s.c_str();
		HRESULT hr = ref->GetIDsOfNames(IID_NULL, &str, 1, LOCALE_USER_DEFAULT, pid);
		return hr;
	}

	virtual HRESULT __stdcall InvokeEx( DISPID id, LCID lcid, WORD wFlags, DISPPARAMS* pdp, VARIANT* pvarRes, EXCEPINFO* pei, IServiceProvider* pspCaller) override
	{
		HRESULT hr = ref->Invoke(id, IID_NULL, lcid, wFlags, pdp, pvarRes, pei, nullptr);
		return hr;
	}

	virtual HRESULT __stdcall DeleteMemberByName( BSTR bstrName, DWORD grfdex)override
	{
		return E_NOTIMPL;
	}

	virtual HRESULT __stdcall DeleteMemberByDispID( DISPID id) override
	{
		return E_NOTIMPL;
	}

	virtual HRESULT __stdcall GetMemberProperties( DISPID id, DWORD grfdexFetch, DWORD* pgrfdex) override
	{
		return E_NOTIMPL;
	}

	virtual HRESULT __stdcall GetMemberName( DISPID id, BSTR* pbstrName) override
	{
		return E_NOTIMPL;
	}

	virtual HRESULT __stdcall GetNextDispID( DWORD grfdex, DISPID id, DISPID* pid) override
	{
		return E_NOTIMPL;
	}

	virtual HRESULT __stdcall GetNameSpaceParent( IUnknown** ppunk) override
	{
		return E_NOTIMPL;
	}

	virtual HRESULT __stdcall unwrap(VARIANT* v) override
	{
		if (!v) return E_INVALIDARG;
		/ *
		mtl::chakra::active_ctx ac(ctx);
		mtl::variant result;
		::JsValueToVariant(*ref, &result);

		result.copy_to(v);
		* /
		return S_OK;
	}
};




inline JsValueRef chakraFunctionWrapper(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, void* callbackState)
{
	JsContextRef ctx = nullptr;
	::JsGetContextOfObject(callee, &ctx);
	::JsAddRef(ctx, nullptr);
	std::vector<mtl::variant> args;
	for (int i = 1; i < argumentCount;i++)
	{
		mtl::variant v;
		::JsValueToVariant(arguments[i], &v);
		args.push_back(v);
	}

	mtl::punk<IDispatch> disp ((IDispatch*)callbackState);

	((IDispatch*)callbackState)->Release();
	mtl::ui_thread().submit( [disp,args,ctx]() mutable
	{
		{
			mtl::chakra::active_ctx ac(ctx);

			DISPPARAMS params;
			params.cArgs = args.size();
			params.cNamedArgs = 0;
			params.rgdispidNamedArgs = 0;
			params.rgvarg = args.empty() ? 0 : &args[0];
			mtl::variant result;

			HRESULT hr = disp->Invoke(0, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &result, nullptr, nullptr);
			::JsRelease(ctx,nullptr);
		}
		
	});


	return JS_INVALID_REFERENCE;
}


inline mtl::variant unwrap(VARIANTARG* v)
{
	mtl::variant result;
	if (v->vt == VT_DISPATCH)
	{
		mtl::punk<IChakraDispWrapper> wrapper(v->pdispVal);
		if (wrapper)
		{
			wrapper->unwrap(&result);
			return mtl::variant(result);
		}
		else
		{
			JsValueRef ref = nullptr;
			v->pdispVal->AddRef();
			::JsCreateFunction(chakraFunctionWrapper, v->pdispVal, &ref);
			::JsValueToVariant(ref, &result);
			return mtl::variant(result);
		}
		/ **
		else
		{
			result = v->pdispVal;
		}
		mtl::punk<IV8DispWrapper> v8wrapper(v->pdispVal);
		if (!v8wrapper)
		{
			v8wrapper = new V8DispWrapper(result.pdispVal);
		}
		* /
		//return mtl::variant(v8wrapper);
	}
	return mtl::variant(*v);
}


class ChakraDispWrapper : public mtl::implements<ChakraDispWrapper(IChakraDispWrapper,mtl::of<IChakraDispWrapper,IDispatch>)>
{
public:

	mtl::chakra::value that;
	mtl::chakra::value ref;
	JsContextRef ctx = nullptr;

	ChakraDispWrapper(JsValueRef t, JsValueRef r)
		:that(t),ref(r)
	{
		::JsGetContextOfObject(r, &ctx);
		::JsAddRef(ctx, nullptr);
		mtl::chakra::active_ctx ac(ctx);

	}

	ChakraDispWrapper(JsValueRef t,JsContextRef c, mtl::variant v)
		:that(t),ctx(c)
	{
		mtl::chakra::active_ctx ac(ctx);

		JsValueRef value = nullptr;
		::JsVariantToValue(&v, &value);
		ref = value;
	}

	~ChakraDispWrapper()
	{
		::JsRelease(ctx, nullptr);
	}

	virtual HRESULT __stdcall GetTypeInfoCount(UINT* pctinfo) override
	{
		if (!pctinfo)
			return E_INVALIDARG;
		*pctinfo = 0;
		return S_OK;
	}
	virtual HRESULT __stdcall GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override
	{
		return E_NOTIMPL;
	}
	virtual HRESULT __stdcall GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override
	{
		if (!rgDispId) return E_INVALIDARG;


		mtl::chakra::active_ctx ac(ctx);
		mtl::variant v;
		::JsValueToVariant(*ref, &v);

		if (v.vt != VT_DISPATCH)
		{
			return DISP_E_UNKNOWNNAME;
		}

		mtl::bstr n{ mtl::ole_char(*rgszNames) };

		mtl::punk<IDispatchEx> dispEx(v.pdispVal);
		if (dispEx)
		{
			DISPID dispid = 0;
			HRESULT hr = dispEx->GetDispID(*n, 0, &dispid);
			if (hr == S_OK)
			{
				*rgDispId = dispid;
				return S_OK;
			}
		}

		mtl::punk<IDispatch> disp(v.pdispVal);
		if (disp)
		{
			HRESULT hr = disp->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
			if (hr == S_OK)
			{
				return S_OK;
			}
		}
		return DISP_E_UNKNOWNNAME;
	}

	virtual HRESULT __stdcall Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override
	{
		mtl::chakra::active_ctx ac(ctx);
		mtl::variant v;
		::JsValueToVariant(*ref, &v);

		if (v.vt != VT_DISPATCH)
		{
			return DISP_E_MEMBERNOTFOUND;
		}

		std::vector<mtl::variant> params;

		for (int i = 0; i < pDispParams->cArgs; i++)
		{
			mtl::variant v = ::unwrap(&(pDispParams->rgvarg[i]));
			params.push_back(v);
		}

		DISPPARAMS args;
		args.cArgs = params.size();
		args.cNamedArgs = 0;
		args.rgvarg = params.empty() ? 0 : &params[0];
		args.rgdispidNamedArgs = 0;

		mtl::variant result;

		if (dispIdMember == 0)
		{
			std::vector<JsValueRef> vec;
			JsValueRef undef = nullptr;
			::JsGetUndefinedValue(&undef);
			vec.push_back(*that); // thisTarget

			for (int i = 0; i < params.size(); i++)
			{
				JsValueRef p = nullptr;
				::JsVariantToValue(&args.rgvarg[i], &p);
				vec.push_back(p);
			}
			JsValueRef res = nullptr;
			JsErrorCode jec = ::JsCallFunction( *ref, &vec[0], vec.size(), &res);
			if (jec == JsNoError)
			{
				::JsValueToVariant(res, &result);

				if (result.vt == VT_DISPATCH)
				{
					mtl::punk<ChakraDispWrapper> wrapper{ new ChakraDispWrapper(*ref, ctx,result) };
					pVarResult->vt = VT_DISPATCH;
					return wrapper->QueryInterface(IID_IDispatch, (void**)&(pVarResult->pdispVal));
				}
				result.copy_to(pVarResult);

				return S_OK;
			} 
			if (ac.hasException())
			{
				::JsValueRef ex = ac.getAndClearException();
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

				::MessageBox(0, woss.str().c_str(), x.c_str(), 0);
			}
			return S_FALSE;
		}

		
		mtl::punk<IDispatchEx> dispEx(v.pdispVal);
		if (dispEx)
		{
			HRESULT hr = dispEx->InvokeEx(dispIdMember, lcid, wFlags, &args, &result, pExcepInfo, nullptr);
			if (hr == S_OK)
			{
				if (result.vt == VT_DISPATCH)
				{
					mtl::punk<ChakraDispWrapper> wrapper{ new ChakraDispWrapper(*ref,ctx,result) };
					pVarResult->vt = VT_DISPATCH;
					return wrapper->QueryInterface(IID_IDispatch, (void**)&(pVarResult->pdispVal));
				}
				result.copy_to(pVarResult);
				return S_OK;
			}
		}

		mtl::punk<IDispatch> disp(v.pdispVal);
		if (disp)
		{
			HRESULT hr = disp->Invoke(dispIdMember, riid, lcid, wFlags, &args, &result, pExcepInfo, puArgErr);
			if (hr == S_OK)
			{
				if (result.vt == VT_DISPATCH)
				{
					mtl::punk<ChakraDispWrapper> wrapper{ new ChakraDispWrapper(*ref,ctx,result) };
					pVarResult->vt = VT_DISPATCH;
					return wrapper->QueryInterface(IID_IDispatch, (void**)&(pVarResult->pdispVal));
				}
				result.copy_to(pVarResult);
				return S_OK;
			}
		}

		return DISP_E_MEMBERNOTFOUND;
	}

	virtual HRESULT __stdcall unwrap(VARIANT* v) override
	{
		if (!v) return E_INVALIDARG;
		
		mtl::chakra::active_ctx ac(ctx);
		mtl::variant result;
		::JsValueToVariant(*ref, &result);

		result.copy_to(v);
		return S_OK;
	}
};
*/

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

	/*
	static void CALLBACK PromiseContinuationCallback(JsValueRef task, void* callbackState)
	{
		JsValueRef result;
		JsValueRef global;
		JsGetGlobalObject(&global);
		JsCallFunction(task, &global, 1, &result);
	}

	static void CALLBACK ProjectionEnqueueCallback(JsProjectionCallback jsCallback, JsProjectionCallbackContext jsContext, void* callbackState)
	{
		jsCallback(jsContext);
	}


	virtual HRESULT __stdcall winRT(BSTR winrtNs, IDispatch** wrt)
	{
		if (!wrt)
		{
			return E_INVALIDARG;
		}

		*wrt = nullptr;

		std::wstring wrtNs = mtl::bstr_view(winrtNs).str();

		mtl::chakra::active_ctx ctx(*scriptCtx);

//		JsErrorCode jse = ::JsSetProjectionEnqueueCallback(&ProjectionEnqueueCallback, this);
		

		JsErrorCode jec = ::JsProjectWinRTNamespace(L"Windows.Storage");
		//JsErrorCode jec = ::JsProjectWinRTNamespace(wrtNs.c_str());
		if (jec != JsNoError)
		{
			//return S_FALSE;
		}

		//jse = ::JsSetProjectionEnqueueCallback(&ProjectionEnqueueCallback, this);

		JsValueRef result = ctx.run(wrtNs, L"");


		mtl::punk<ChakraDispWrapper> wrapper{ new ChakraDispWrapper(result,result) };

		return wrapper.query_interface(wrt);

		/ *
		::JsAddRef(result,nullptr);
		mtl::variant v;
		jec = ::JsValueToVariant(result, &v);
		if (jec != JsNoError)
		{
			return S_FALSE;
		}
		if( v.vt != VT_DISPATCH && v.vt != VT_UNKNOWN)
		{
			return S_FALSE;
		}
		* /
		//mtl::punk<IDispatch> disp;
		//HRESULT hr = v.pdispVal->QueryInterface(IID_IDispatch, (void**)&disp);


		/ *
		mtl::punk<IDispatchEx> dispEx(*wrt);

		DISPID dispid;
		hr = dispEx->GetDispID( *mtl::bstr(L"StorageFile"), 0, &dispid);
		mtl::automation disp(*wrt, dispid);

		mtl::variant sf = disp.get();

		mtl::punk<IDispatch> sfDisp(sf.pdispVal);

		mtl::automation disp2(*sfDisp, L"getFileFromPathAsync");
		mtl::variant f = disp2.get();
		* /
		//return hr;
	}


*/


};



