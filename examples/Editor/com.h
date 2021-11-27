#pragma once

#include "mvc.h"


class EditorController;

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

	virtual HRESULT __stdcall WinRT(BSTR ns) override
	{
		::JsProjectWinRTNamespace(mtl::bstr_view(ns).str().c_str());
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
