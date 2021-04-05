#pragma once

#include "MTL/punk.h"
#include "MTL/variant.h"
#include "MTL/disp.h"
#include <ActivScp.h>

namespace MTL {

	/////////////////////////////////////////////////////////////////////////////////////////////

	class ActiveScript : public implements< ActiveScript(IActiveScriptSite, IActiveScriptSiteWindow)>
	{
	public:
		ActiveScript()
		{
			::ZeroMemory(&ei_, sizeof(ei_));
			hWnd_ = ::GetDesktopWindow();
		}

		ActiveScript(const std::wstring& engine)
		{
			::ZeroMemory(&ei_, sizeof(ei_));
			hWnd_ = ::GetDesktopWindow();

			HR hr = init(engine);
		}

		ActiveScript(const std::wstring& engine, HWND hWnd)
		{
			::ZeroMemory(&ei_, sizeof(ei_));
			hWnd_ = hWnd;

			HR hr = init(engine);
		}

		virtual ~ActiveScript() {}

		void setWindow(HWND wnd)
		{
			hWnd_ = wnd;
		}

		virtual HRESULT init(const std::wstring& engine)
		{
			engine_ = engine;

			activeScript.release();
			asp_.release();

			HRESULT hr;
			hr = getScriptEngine(engine_, &activeScript);
			if (hr != S_OK || !activeScript)
			{
				return hr;
			}

			punk<IActiveScriptProperty> asprop(activeScript);
			if (asprop)
			{
				variant v(SCRIPTLANGUAGEVERSION_5_8);
				hr = asprop->SetProperty(SCRIPTPROP_INVOKEVERSIONING, NULL, &v);
			}

			hr = activeScript.queryInterface(&asp_);
			if (hr != S_OK)
			{
				return hr;
			}

			hr = asp_->InitNew();
			if (hr != S_OK)
			{
				return hr;
			}

			hr = setHost((IActiveScriptSite*)this);
			if (hr != S_OK)
			{
				return hr;
			}

			hr = setState(SCRIPTSTATE_INITIALIZED);//SCRIPTSTATE_STARTED);//SCRIPTSTATE_INITIALIZED);
			if (hr != S_OK)
			{
				return hr;
			}

			return hr;
		}

		virtual HRESULT exec()
		{
			return setState(SCRIPTSTATE_STARTED);
		}

		HRESULT addNamedObject(IUnknown* punk, const std::wstring& obj, int state = SCRIPTITEM_ISVISIBLE)
		{
			punk->AddRef();
			if (objectMap_.count(obj) > 0)
			{
				objectMap_[obj]->Release();
			}
			objectMap_[obj] = punk;

			HRESULT hr = activeScript->AddNamedItem(obj.c_str(), state);
			if (hr == S_OK)
			{
				return S_OK;
			}
			objectMap_.erase(obj);
			punk->Release();
			return hr;
		}

		HRESULT removeNamedObject(const std::wstring& obj)
		{
			if (objectMap_.count(obj) > 0)
			{
				objectMap_[obj]->Release();
				objectMap_.erase(obj);

			}
			return S_OK;
		}

		virtual HRESULT setState(SCRIPTSTATE state)
		{
			if (activeScript)
				return activeScript->SetScriptState(state);
			return E_UNEXPECTED;
		}

		virtual HRESULT setHost(IActiveScriptSite* host)
		{
			if (!host)
				return E_FAIL;

			if (!activeScript)
				return E_UNEXPECTED;

			return activeScript->SetScriptSite(host);
		}

		virtual HRESULT close()
		{
			for (auto it = objectMap_.begin(); it != objectMap_.end(); it++)
			{
				(*it).second->Release();
			}
			objectMap_.clear();

			if (activeScript)
			{
				HRESULT hr = activeScript->Close();
				return hr;
			}
			return S_OK;
		}

		virtual HRESULT runScript(const std::wstring& script, int flag = SCRIPTTEXT_ISEXPRESSION | SCRIPTTEXT_ISVISIBLE)
		{
			varResult_.clear();
			::ZeroMemory(&ei_, sizeof(ei_));

			HRESULT hr;
			if (asp_)
			{
				hr = asp_->ParseScriptText(script.c_str(),
					NULL, 0, 0, 0, 0, flag,//SCRIPTTEXT_ISEXPRESSION|SCRIPTTEXT_ISVISIBLE ,
					&varResult_, &ei_);

				return hr;
			}
			return E_FAIL;
		}

		virtual HRESULT getScriptDispatch(const std::wstring& script, IDispatch** ppdisp)
		{
			if (activeScript)
				if (script.length() > 0)
					return activeScript->GetScriptDispatch(script.c_str(), ppdisp);
				else
					return activeScript->GetScriptDispatch(0, ppdisp);

			return E_FAIL;
		}

		virtual HRESULT addScriptlet(std::wstring& name, const std::wstring& handler, const std::wstring& obj, const std::wstring& subobj, const std::wstring& eventname)
		{
			varResult_.clear();
			::ZeroMemory(&ei_, sizeof(ei_));

			bstr bstrName;
			HRESULT hr = asp_->AddScriptlet(
				name.c_str(),
				handler.c_str(),
				obj.c_str(),
				subobj.c_str(),
				eventname.c_str(),
				L"", 0, 0, SCRIPTTEXT_ISVISIBLE, &bstrName, &ei_
			);

			return hr;
		}

		variant& result()
		{
			return varResult_;
		}

		EXCEPINFO& errorInfo()
		{
			return ei_;
		}

		punk<IActiveScript>					activeScript;


		//IActiveScriptSite
		virtual HRESULT  __stdcall GetLCID(LCID* plcid)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT  __stdcall GetItemInfo(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown** ppiunkItem, ITypeInfo** ppti)
		{
			if (SCRIPTINFO_IUNKNOWN & dwReturnMask)
			{
				for (auto it = objectMap_.begin(); it != objectMap_.end(); it++)
				{
					std::wstring tmp = (*it).first;
					if (_wcsicmp(tmp.c_str(), pstrName) == 0)
					{
						IUnknown* punk = (*it).second;
						return punk->QueryInterface(IID_IUnknown, (void**)ppiunkItem);
					}
				}
			}
			if (SCRIPTINFO_ITYPEINFO & dwReturnMask)
			{
				for (auto it = objectMap_.begin(); it != objectMap_.end(); it++)
				{
					std::wstring tmp = (*it).first;
					if (_wcsicmp(tmp.c_str(), pstrName) == 0)
					{
						IUnknown* unk = (*it).second;
						punk<IProvideClassInfo> pci(unk);
						if (pci)
						{
							return pci->GetClassInfo(ppti);
						}
					}
				}
			}
			return TYPE_E_ELEMENTNOTFOUND;
		}

		virtual HRESULT  __stdcall GetDocVersionString(BSTR* pbstrVersion)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT  __stdcall OnScriptTerminate(const VARIANT* pvarResult, const EXCEPINFO* pexcepinfo)
		{
			return S_OK;
		}

		virtual HRESULT  __stdcall OnStateChange(SCRIPTSTATE ssScriptState)
		{
			return S_OK;
		}

		virtual HRESULT  __stdcall OnScriptError(IActiveScriptError* pscripterror)
		{
			EXCEPINFO ex;
			pscripterror->GetExceptionInfo(&ex);
			bstr src;
			pscripterror->GetSourceLineText(&src);

			DWORD context;
			ULONG line;
			LONG pos;
			pscripterror->GetSourcePosition(&context, &line, &pos);

			bstr source(b_copy(ex.bstrSource));
			bstr desc(b_copy(ex.bstrDescription));

			std::wostringstream oss;
			oss << desc.str() << L" line " << line << "\r\n";
			oss << src.str();
			::MessageBoxW(0, oss.str().c_str(), source.str().c_str(), MB_ICONERROR);
			return S_OK;
		}

		virtual HRESULT  __stdcall OnEnterScript(void)
		{
			return S_OK;
		}

		virtual HRESULT  __stdcall OnLeaveScript(void)
		{
			return S_OK;
		}

		//IActiveScriptSiteWindow
		virtual HRESULT  __stdcall GetWindow(HWND* phwnd)
		{
			*phwnd = hWnd_;
			return S_OK;
		}

		virtual HRESULT  __stdcall EnableModeless(BOOL fEnable)
		{
			return S_OK;
		}

	protected:
		HWND								hWnd_;
		std::wstring						engine_;
		punk<IActiveScriptParse>			asp_;
		variant								varResult_;
		EXCEPINFO							ei_;

		std::map<std::wstring, IUnknown*>	objectMap_;


		
		struct __declspec(uuid("16d51579-a30b-4c8b-a276-0ff4dc41e755")) JScript9;


		HRESULT getScriptEngine(const std::wstring& engine, IActiveScript** ppas)
		{
			*ppas = 0;
			CLSID clsid;
			HRESULT hr;

			if (engine == L"JScript") {
				// try loading JScript 9 dll
				clsid = __uuidof(JScript9);
				hr = ::CoCreateInstance(clsid, 0, CLSCTX_ALL, IID_IActiveScript, (void**)ppas);
				if (hr == S_OK)
					return hr;
			}

			hr = ::CLSIDFromProgID( engine.c_str(), &clsid);
			if (hr == S_OK)
				hr = ::CoCreateInstance(clsid, 0, CLSCTX_ALL, IID_IActiveScript, (void**)ppas);
			return hr;
		}

	};

/////////////////////////////////////////////////////////////////////////////////////////////

std::wstring engine_from_filename(const std::wstring& filename)
{
	std::wstring ext= filename;
	size_t pos = filename.find_last_of(L".");
	if (pos == std::string::npos)
	{
		if (ext[0] != L'.')
		{
			std::wstring tmp(L".");
			tmp += ext;
			ext = tmp;
		}
	}
	else
	{
		ext = filename.substr(pos);
	}

	HKEY extKey;
	REGSAM sam = KEY_READ;

	if (ERROR_SUCCESS != ::RegOpenKeyExW(HKEY_CLASSES_ROOT, ext.c_str(), 0, sam, &extKey))
	{
		return L"";
	}

	wbuff buf(2048);
	long len = (long)buf.size();

	if (ERROR_SUCCESS != RegQueryValueW(extKey, 0, buf, &len) )
	{
		::RegCloseKey(extKey);
		return L"";
	}

	std::wstring extFile = buf.toString(len);

	HKEY fileKey;
	if (ERROR_SUCCESS != ::RegOpenKeyExW(HKEY_CLASSES_ROOT, extFile.c_str(), 0, sam, &fileKey))
	{
		::RegCloseKey(extKey);
		return L"";
	}

	HKEY engineKey;
	if (ERROR_SUCCESS != ::RegOpenKeyExW(fileKey, L"ScriptEngine", 0, sam, &engineKey))
	{
		::RegCloseKey(fileKey);
		::RegCloseKey(extKey);
		return L"";
	}

	// reset avail buffer size
	len = (long)buf.size();
	if (ERROR_SUCCESS != RegQueryValueW(engineKey, 0, buf, &len))
	{
		::RegCloseKey(engineKey);
		::RegCloseKey(fileKey);
		::RegCloseKey(extKey);
		return L"";
	}

	::RegCloseKey(engineKey);
	::RegCloseKey(fileKey);
	::RegCloseKey(extKey);

	return buf.toString(len);
}

} // end namespace
