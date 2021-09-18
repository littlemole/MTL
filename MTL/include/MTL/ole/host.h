#pragma once

#include "mtl/ole/control.h"

namespace mtl {

	inline HRESULT copy_storage_temp(IStorage* src, IStorage** copy)
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
			return store.query_interface(copy);
		}
		return E_FAIL;
	}


	///////////////////////////////////////////////////////////////////////////////

	class inplace_frame : public implements<inplace_frame(IOleInPlaceFrame, of<IOleInPlaceFrame, IOleWindow>)>
	{
	private:

		HWND hWnd_;

	public:

		punk<IOleInPlaceActiveObject> activeObject;

		static punk<inplace_frame> create_instance(HWND w)
		{
			inplace_frame* instance = new inplace_frame;
			instance->hWnd_ = w;
			return punk<inplace_frame>(instance);
		}

		~inplace_frame()
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

	class client_site : public implements< client_site(IOleInPlaceSite, IOleClientSite, of<IOleInPlaceSite, IOleWindow>)>
	{
	private:

		HWND hWnd_;

		punk<IOleInPlaceFrame> frame_;

	public:

		static punk<client_site> create_instance(HWND w, IOleInPlaceFrame* f)
		{
			client_site* instance = new client_site;
			instance->hWnd_ = w;
			instance->frame_ = f;
			return punk<client_site>(instance);
		}

		~client_site()
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
	class ole_host : public C
	{
	public:

		ole_host()
		{}

		virtual LRESULT wm_erase_background(WPARAM wParam) override
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
				frame = inplace_frame::create_instance(this->handle);
				punk<IOleInPlaceFrame> opf(frame);
				clientSite = client_site::create_instance(this->handle, *opf);
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
					pixel_to_himetric(&s);

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

		bool init_object(REFCLSID clsid, IStorage* store)
		{
			HRESULT hr = create_embedded_object(clsid, store);
			if (hr != S_OK)
				return false;

			punk<IPersistStorage> ps(oleObject);
			if (ps)
			{
				hr = ps->InitNew(store);
				if (hr != S_OK)
					return false;
			}

			hr = show_embedded_object(clsid, store);
			if (hr != S_OK)
				return false;
			return true;
		}

		bool load_object(REFCLSID clsid, IStorage* store)
		{
			HRESULT hr = create_embedded_object(clsid, store);
			if (hr != S_OK)
				return false;

			punk<IPersistStorage> ps(oleObject);
			if (!ps)
				return false;

			punk<IStorage> s;
			hr = copy_storage_temp(store, &s);
			if (hr != S_OK)
				return false;

			hr = ps->Load(s);
			if (hr != S_OK)
				return false;

			hr = show_embedded_object(clsid, store);
			if (hr != S_OK)
				return false;
			return true;
		}

	protected:

		punk<IOleObject>	oleObject;
		punk<client_site>	clientSite;
		punk<inplace_frame>	frame;

		HRESULT create_embedded_object(REFCLSID clsid, IStorage* store)
		{
			if (oleObject)
			{
				oleObject->Close(OLECLOSE_NOSAVE);
				oleObject.release();
			}

			HRESULT hr = oleObject.create_object(clsid);
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

		HRESULT show_embedded_object(REFCLSID clsid, IStorage* store)
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

