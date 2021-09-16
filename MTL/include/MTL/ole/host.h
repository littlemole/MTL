#pragma once

#include "MTL/ole/control.h"

namespace MTL {

	inline HRESULT copyStorageTemp(IStorage* src, IStorage** copy)
	{
		if (!src || !copy)
			return E_POINTER;

		*copy = 0;
		wchar_t  path[MAX_PATH];
		wchar_t file[MAX_PATH];
		::GetTempPath(255, path);
		::GetTempFileName(path, L"mtl_", 0, file);

		punk<IStorage> store;
		if (S_OK == ::StgCreateDocfile(
			file, 
			STGM_DELETEONRELEASE | STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_TRANSACTED,
			0, 
			&store)
		)
		{
			HRESULT hr = src->CopyTo(0, 0, 0, *store);
			return store.queryInterface(copy);
		}
		return E_FAIL;
	}


	///////////////////////////////////////////////////////////////////////////////

	class InplaceFrame : public implements<InplaceFrame(IOleInPlaceFrame, of<IOleInPlaceFrame, IOleWindow>)>
	{
	private:

		HWND hWnd_;

	public:

		punk<IOleInPlaceActiveObject> activeObject;

		static punk<InplaceFrame> CreateInstance(HWND w)
		{
			InplaceFrame* instance = new InplaceFrame;
			instance->hWnd_ = w;
			return punk<InplaceFrame>(instance);
		}

		~InplaceFrame()
		{
			activeObject.release();
		}

		// IOleWindow
		HRESULT virtual __stdcall GetWindow(HWND FAR* lphwnd)
		{
			*lphwnd = hWnd_;
			return S_OK;
		}
		HRESULT virtual __stdcall ContextSensitiveHelp(BOOL fEnterMode)
		{
			return E_NOTIMPL;
		}

		// IOleInPlaceUIWindow
		HRESULT virtual __stdcall GetBorder(LPRECT lprectBorder)
		{
			return E_NOTIMPL;
		}
		HRESULT virtual __stdcall RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
		{
			return E_NOTIMPL;
		}
		HRESULT virtual __stdcall SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
		{
			return E_NOTIMPL;
		}
		HRESULT virtual __stdcall SetActiveObject(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR pszObjName)
		{
			activeObject = pActiveObject;
			return S_OK;
		}

		// IOleInPlaceFrame
		HRESULT virtual __stdcall InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
		{
			return E_NOTIMPL;
		}
		HRESULT virtual __stdcall SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
		{
			return E_NOTIMPL;
		}
		HRESULT virtual __stdcall RemoveMenus(HMENU hmenuShared)
		{
			return E_NOTIMPL;
		}
		HRESULT virtual __stdcall SetStatusText(LPCOLESTR pszStatusText)
		{
			return E_NOTIMPL;
		}
		HRESULT virtual __stdcall EnableModeless(BOOL fEnable)
		{
			return E_NOTIMPL;
		}

		HRESULT virtual __stdcall TranslateAccelerator(LPMSG lpmsg, WORD wID)
		{
			return S_OK;
		}
	};

	///////////////////////////////////////////////////////////////////////////////

	class ClientSite : public implements< ClientSite(IOleInPlaceSite, IOleClientSite, of<IOleInPlaceSite, IOleWindow>)>
	{
	private:

		HWND hWnd_;

		punk<IOleInPlaceFrame> frame_;

	public:

		static punk<ClientSite> CreateInstance(HWND w, IOleInPlaceFrame* f)
		{
			ClientSite* instance = new ClientSite;
			instance->hWnd_ = w;
			instance->frame_ = f;
			return punk<ClientSite>(instance);
		}

		~ClientSite()
		{
			frame_.release();
		}

		// IOleInPlaceSite
		HRESULT virtual __stdcall SaveObject()
		{
			return S_OK;
		}

		HRESULT virtual __stdcall GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk)
		{
			return E_NOTIMPL;
		}

		HRESULT virtual __stdcall GetContainer(LPOLECONTAINER FAR* ppContainer)
		{
			*ppContainer = 0;
			return E_NOINTERFACE;
		}

		HRESULT virtual __stdcall ShowObject()
		{
			return S_OK;
		}

		HRESULT virtual __stdcall OnShowWindow(BOOL fShow)
		{
			return S_OK;
		}

		HRESULT virtual __stdcall RequestNewObjectLayout()
		{
			return S_OK;
		}

		HRESULT virtual __stdcall ShowContextMenu(DWORD dwID, POINT* ppt, IUnknown* pcmdtReserved, IDispatch* pdispReserved)
		{
			return S_OK;
		}

		// IOleWindow
		HRESULT virtual __stdcall GetWindow(HWND FAR* lphwnd)
		{
			*lphwnd = hWnd_;
			return S_OK;
		}

		HRESULT virtual __stdcall ContextSensitiveHelp(BOOL fEnterMode)
		{
			return E_NOTIMPL;
		}


		// IOleInPlaceFrame
		HRESULT virtual __stdcall CanInPlaceActivate()
		{
			return S_OK;
		}

		HRESULT virtual __stdcall OnInPlaceActivate()
		{
			return S_OK;
		}

		HRESULT virtual __stdcall OnUIActivate()
		{
			return S_OK;
		}

		HRESULT virtual __stdcall GetWindowContext(LPOLEINPLACEFRAME FAR* lplpFrame, LPOLEINPLACEUIWINDOW FAR* lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
		{
			frame_->QueryInterface(IID_IOleInPlaceFrame, (void**)lplpFrame);
			//this->QueryInterface(IID_IOleInPlaceUIWindow, (void**)lplpDoc);

			*lplpDoc = 0;
			lpFrameInfo->fMDIApp = FALSE;
			lpFrameInfo->hwndFrame = hWnd_;
			lpFrameInfo->haccel = 0;
			lpFrameInfo->cAccelEntries = 0;

			RECT r;
			::GetClientRect(hWnd_, &r);
			*lprcPosRect = r;
			*lprcClipRect = r;
			return S_OK;
		}

		HRESULT virtual __stdcall Scroll(SIZE scrollExtent)
		{
			return E_NOTIMPL;
		}

		HRESULT virtual __stdcall OnUIDeactivate(BOOL fUndoable)
		{
			return S_OK;
		}

		HRESULT virtual __stdcall OnInPlaceDeactivate()
		{
			return S_OK;
		}

		HRESULT virtual __stdcall DiscardUndoState()
		{
			return S_OK;
		}

		HRESULT virtual __stdcall DeactivateAndUndo()
		{
			return S_OK;
		}

		HRESULT virtual __stdcall OnPosRectChange(LPCRECT lprcPosRect)
		{
			//		mol::punk<IOleInPlaceObject>	inplace(oleObject);
			//		inplace->SetObjectRects(lprcPosRect, lprcPosRect);		
			return S_OK;
		}
	};

	template<class C>
	class OleHost : public C
	{
	public:

		OleHost()
		{}

		virtual LRESULT wmEraseBackground(WPARAM wParam) override
		{
			// prevent bkgrnd erase
			return 1;
		}


		//////////////////////////////////////////////////////////////////////////////

		void dispose()
		{
			//ODBGS("~SimpleAxHost dies");

			if (oleObject)
			{
				oleObject->Close(0);
				oleObject.release();

				if (clientSite)
					clientSite.release();
			}

			if (frame)
				frame.release();
		}


		virtual LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_CREATE:
			{
				frame = InplaceFrame::CreateInstance(this->handle);
				punk<IOleInPlaceFrame> opf(frame);
				clientSite = ClientSite::CreateInstance(this->handle, *opf);
				break;
			}
			case WM_DESTROY:
			{
				LRESULT r = C::wndProc(hWnd, message, wParam, lParam);
				dispose();
				return r;
				break;
			}
			case WM_SIZE:
			{
				RECT clientRect = { 0, 0, 0, 0 };

				// get new width and height
				clientRect.right = LOWORD(lParam);
				clientRect.bottom = HIWORD(lParam);

				if (oleObject)
				{
					SIZEL s = { clientRect.right, clientRect.bottom };
					PixeltoHIMETRIC(&s);

					HRESULT hr = oleObject->SetExtent(DVASPECT_CONTENT, &s);

					punk<IOleInPlaceObject> oip(oleObject);
					if (oip)
					{
						oip->SetObjectRects(&clientRect, &clientRect);
					}
				}
				break;
			}
			}
			return C::wndProc(hWnd, message, wParam, lParam);
		}

	protected:

		bool initObject(REFCLSID clsid, IStorage* store)
		{
			HRESULT hr = createEmbeddedObject(clsid, store);
			if (hr != S_OK)
				return false;

			punk<IPersistStorage> ps(oleObject);
			if (ps)
			{
				hr = ps->InitNew(store);
				if (hr != S_OK)
					return false;
			}

			hr = showEmbeddedObject(clsid, store);
			if (hr != S_OK)
				return false;
			return true;
		}

		bool loadObject(REFCLSID clsid, IStorage* store)
		{
			HRESULT hr = createEmbeddedObject(clsid, store);
			if (hr != S_OK)
				return false;

			punk<IPersistStorage> ps(oleObject);
			if (!ps)
				return false;

			punk<IStorage> s;
			hr = copyStorageTemp(store, &s);
			if (hr != S_OK)
				return false;

			hr = ps->Load(s);
			if (hr != S_OK)
				return false;

			hr = showEmbeddedObject(clsid, store);
			if (hr != S_OK)
				return false;
			return true;
		}

	protected:

		punk<IOleObject>	oleObject;
		punk<ClientSite>	clientSite;
		punk<InplaceFrame>	frame;

		HRESULT createEmbeddedObject(REFCLSID clsid, IStorage* store)
		{
			if (oleObject)
			{
				oleObject->Close(OLECLOSE_NOSAVE);
				oleObject.release();
			}

			HRESULT hr = oleObject.createObject(clsid);
			if (hr != S_OK)
				return hr;

			DWORD miscStatus = 0;
			hr = oleObject->GetMiscStatus(DVASPECT_CONTENT, &miscStatus);
			if (hr != S_OK)
				return hr;

			if (miscStatus & OLEMISC_SETCLIENTSITEFIRST)
			{
				punk<IOleClientSite> cs(clientSite);
				hr = oleObject->SetClientSite(*cs);
			}
			return hr;
		}

		HRESULT showEmbeddedObject(REFCLSID clsid, IStorage* store)
		{
			HRESULT hr = oleObject->SetHostNames(L"My Host Name", 0);
			if (hr != S_OK)
				return hr;

			hr = ::OleSetContainedObject(*oleObject, TRUE);
			if (hr != S_OK)
				return hr;

			RECT r;
			::GetClientRect(this->handle, &r);
			
			punk<IOleClientSite> cs(clientSite);
			hr = oleObject->DoVerb(OLEIVERB_SHOW, NULL, *cs, -1, this->handle, &r);
			return hr;
		}


	};



}

