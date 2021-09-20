#pragma once

#include <mtl/punk.h>
#include <mtl/win/gdi.h>
#include <mtl/win/wnd.h>
#include <mtl/obj/impl.h>
#include <mtl/ole/dataobj.h>

#include <ocidl.h>
#include <objbase.h>
#include <ole2.h>
#include <OleCtl.h>

namespace mtl {


//#define HIMETRIC_PER_INCH   2540
//#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
//#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)

	constexpr int HIMETRIC_PER_INCH = 2540;

	inline int MAP_PIX_TO_LOGHIM(int x, int ppli)
	{
		return ::MulDiv(HIMETRIC_PER_INCH, (x), (ppli));
	}

	inline int MAP_LOGHIM_TO_PIX(int x, int ppli)
	{
		return ::MulDiv((ppli), (x), HIMETRIC_PER_INCH);
	}

	inline void pixel_to_himetric(SIZE* size)
	{
		int nPixelsPerInchX;
		int nPixelsPerInchY;

		HDC hDCScreen = ::GetDC(NULL);
		nPixelsPerInchX = ::GetDeviceCaps(hDCScreen, LOGPIXELSX);
		nPixelsPerInchY = ::GetDeviceCaps(hDCScreen, LOGPIXELSY);
		::ReleaseDC(NULL, hDCScreen);

		size->cx = MAP_PIX_TO_LOGHIM(size->cx, nPixelsPerInchX);
		size->cy = MAP_PIX_TO_LOGHIM(size->cy, nPixelsPerInchY);
	}
	/////////////////////////////////////////////////////////////////////////////////

	inline void himetric_to_pixel(SIZE* sz)//, HDC hdc)
	{
		HDC hdc = ::GetWindowDC(NULL);
		int nMapMode;
		if ((nMapMode = ::GetMapMode(hdc)) < MM_ISOTROPIC && nMapMode != MM_TEXT)
		{
			// when using a constrained map mode, map against physical inch

			::SetMapMode(hdc, MM_HIMETRIC);
			POINT pt;
			pt.x = sz->cx;
			pt.y = sz->cy;
			::LPtoDP(hdc, &pt, 1);
			sz->cx = pt.x;
			sz->cy = pt.y;
			::SetMapMode(hdc, nMapMode);
		}
		else
		{
			// map against logical inch for non-constrained mapping modes
			int cxPerInch, cyPerInch;
			cxPerInch = ::GetDeviceCaps(hdc, LOGPIXELSX);
			cyPerInch = ::GetDeviceCaps(hdc, LOGPIXELSY);
			sz->cx = MulDiv(sz->cx, cxPerInch, HIMETRIC_PER_INCH);
			sz->cy = MulDiv(sz->cy, cyPerInch, HIMETRIC_PER_INCH);
		}
		POINT pt;
		pt.x = sz->cx;
		pt.y = sz->cy;
		::DPtoLP(hdc, &pt, 1);
		sz->cx = pt.x;
		sz->cy = pt.y;
	}



	template<class T>
	class control : public IOleObject, public IOleControl, public IOleInPlaceObject,public data_object, public IViewObject2, public window<T>
	{
	public:

		control()
		{
			posRect_ = { 0,0,200,200 };
			sizel_ = { 200,200 };

			::CreateOleAdviseHolder(&adviseHolder_);
			::CreateDataAdviseHolder(&dataAdviseHolder_);

			active_ = punk<ole_inplace_active_obj>(new ole_inplace_active_obj(this));
		}

		~control()
		{
			/*
			if (::IsWindow(this->handle))
			{
				::DestroyWindow(this->handle);
			}
			*/
		}

		virtual LRESULT wm_destroy() override
		{
			T* that = (T*)this;
			that->Release();
			return 0;
		}


		// IDataObject
		virtual HRESULT __stdcall GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium) override
		{
			// check params
			if (pmedium == NULL)
				return E_INVALIDARG;

			if (pformatetcIn == NULL)
				return E_INVALIDARG;

			memset(pmedium, 0, sizeof(STGMEDIUM));

			if ((pformatetcIn->tymed & TYMED_MFPICT) == 0)
				return DATA_E_FORMATETC;

			// determine size
			SIZEL sizeMetric;
			this->GetExtent(pformatetcIn->dwAspect, &sizeMetric);

			SIZE s = { sizeMetric.cx,sizeMetric.cy };
			himetric_to_pixel(&s);
			RECTL rectl = { 0 ,0, s.cx, s.cy };
			RECT r = { 0 ,0, s.cx, s.cy };

			// draw to meta DC
			meta_dc mdc;
			dc dcv(*mdc);
			dcv.save();
			dcv.set_window_org(0, 0);
			dcv.set_window_ext(rectl.right, rectl.bottom);
			this->wm_draw(*mdc,r);// , & rectl, & rectl);
			dcv.restore(-1);

			HMETAFILE metafile = mdc.close();
			if (metafile == NULL)
				return E_UNEXPECTED;

			// assemble result MFP and put on global memory
			METAFILEPICT mfp;
			mfp.hMF = metafile;
			mfp.mm = MM_ANISOTROPIC;
			mfp.xExt = sizeMetric.cx;
			mfp.yExt = sizeMetric.cy;

			global glob(&mfp, sizeof(METAFILEPICT), GMEM_SHARE | GMEM_MOVEABLE);

			// set results
			pmedium->tymed = TYMED_MFPICT;
			pmedium->hGlobal = *glob;
			pmedium->pUnkForRelease = NULL;

			// release ownership
			glob.detach();

			return S_OK;
		}

		// OleObject
		virtual HRESULT __stdcall SetClientSite(IOleClientSite* pClientSite) override
		{
			if (!pClientSite) return E_INVALIDARG;
			clientSite_ = pClientSite;
			return S_OK;
		}

		virtual HRESULT __stdcall GetClientSite(IOleClientSite** ppClientSite) override
		{
			if (!ppClientSite) return E_INVALIDARG;
			return clientSite_.query_interface(ppClientSite);
		}

		virtual HRESULT __stdcall SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj) override
		{
			containerApp_ = szContainerApp;
			containerObj_ = szContainerObj;
			return S_OK;
		}

		virtual HRESULT __stdcall Close(DWORD dwSaveOption) override
		{
			if (adviseHolder_)
			{
				adviseHolder_->SendOnClose();
			}

			this->inplaceDeactivate();
			HWND hwnd = this->handle;
			if (hwnd && ::IsWindow(hwnd))
			{
				::SetParent(hwnd, 0);
				::DestroyWindow(hwnd);
			}
			active_.release();
			return S_OK;
		}

		virtual HRESULT __stdcall SetMoniker(DWORD dwWhichMoniker, IMoniker* pmk) override
		{
			return S_OK;
		}

		virtual HRESULT __stdcall GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk) override
		{
			if (!clientSite_)
				return E_UNEXPECTED;
			return clientSite_->GetMoniker(dwAssign, dwWhichMoniker, ppmk);
		}

		virtual HRESULT __stdcall InitFromData(IDataObject* pDataObject, BOOL fCreation, DWORD dwReserved) override
		{
			return E_NOTIMPL;
		}

		virtual HRESULT __stdcall GetClipboardData(DWORD dwReserved, IDataObject** ppDataObject) override
		{
			*ppDataObject = NULL;
			return E_NOTIMPL;
		}

		virtual HRESULT __stdcall DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite* pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect) override
		{
			clientSite_ = pActiveSite;
			parentWindow_ = hwndParent;
			if (lprcPosRect)
			{
				setRects(*lprcPosRect, *lprcPosRect);
			}

			isDirty_ = true;

			if (iVerb == OLEIVERB_HIDE)
			{
				return OleVerb_Hide(iVerb, lpmsg, pActiveSite, lindex, hwndParent, lprcPosRect);
			}
			else if (iVerb == OLEIVERB_SHOW)
			{
				return OleVerb_Show(iVerb, lpmsg, pActiveSite, lindex, hwndParent, lprcPosRect);
			}
			else if (iVerb == OLEIVERB_OPEN)
			{
				return OleVerb_Open(iVerb, lpmsg, pActiveSite, lindex, hwndParent, lprcPosRect);
			}
			else if (iVerb == OLEIVERB_PRIMARY)
			{
				return OleVerb_Primary(iVerb, lpmsg, pActiveSite, lindex, hwndParent, lprcPosRect);
			}
			if (iVerb == OLEIVERB_INPLACEACTIVATE)
			{
				return OleVerb_InplaceActivate(iVerb, lpmsg, pActiveSite, lindex, hwndParent, lprcPosRect);
			}
			if (iVerb == OLEIVERB_UIACTIVATE)
			{
				return OleVerb_UIActivate(iVerb, lpmsg, pActiveSite, lindex, hwndParent, lprcPosRect);
			}
			if (iVerb == OLEIVERB_PROPERTIES)
			{
				return OleVerb_Properties(iVerb, lpmsg, pActiveSite, lindex, hwndParent, lprcPosRect);
			}
			return OleVerb(iVerb, lpmsg, pActiveSite, lindex, hwndParent, lprcPosRect);
		}


		virtual HRESULT __stdcall EnumVerbs(IEnumOLEVERB** ppEnumOleVerb) override
		{
			return OLE_S_USEREG;
		}

		virtual HRESULT __stdcall Update(void) override
		{
			return S_OK;
		}

		virtual HRESULT __stdcall IsUpToDate(void) override
		{
			return S_OK;
		}

		virtual HRESULT __stdcall GetUserClassID(CLSID* pClsid) override
		{
			if (!pClsid) return E_INVALIDARG;
			*pClsid = __uuidof(T);
			return S_OK;
		}

		virtual HRESULT __stdcall GetUserType(DWORD dwFormOfType, LPOLESTR* pszUserType) override
		{
			CLSID clsid;
			if (S_OK == this->GetUserClassID(&clsid))
				return ::OleRegGetUserType(clsid, dwFormOfType, pszUserType);
			return E_FAIL;
		}

		virtual void sendOnDataChange()
		{
			if (dataAdviseHolder_)
				dataAdviseHolder_->SendOnDataChange(this, 0, 0);

			isDirty_ = true;
		}

		virtual HRESULT __stdcall SetExtent(DWORD dwDrawAspect, SIZEL* psizel) override
		{
			if (!psizel) return E_INVALIDARG;
			sizel_ = *psizel;
			sendOnDataChange();
			return S_OK;
		}

		virtual HRESULT __stdcall GetExtent(DWORD dwDrawAspect, SIZEL* psizel) override
		{
			if (!psizel) return E_INVALIDARG;
			*psizel = sizel_;
			return S_OK;
		}

		virtual HRESULT __stdcall Advise(IAdviseSink* pAdvSink, DWORD* pdwConnection) override
		{
			return adviseHolder_->Advise(pAdvSink, pdwConnection);
		}

		virtual HRESULT __stdcall Unadvise(DWORD dwConnection) override
		{
			return adviseHolder_->Unadvise(dwConnection);
		}

		virtual HRESULT __stdcall EnumAdvise(IEnumSTATDATA** ppenumAdvise) override
		{
			return adviseHolder_->EnumAdvise(ppenumAdvise);
		}

		virtual HRESULT __stdcall GetMiscStatus(DWORD dwAspect, DWORD* pdwStatus) override
		{
			return OLE_S_USEREG;
		}

		virtual HRESULT __stdcall SetColorScheme(LOGPALETTE* pLogpal) override
		{
			return E_NOTIMPL;
		}

		// IOleControl

		virtual HRESULT __stdcall GetControlInfo(CONTROLINFO* pCI)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT __stdcall OnMnemonic(MSG* pMsg) 
		{
			return E_NOTIMPL;
		}

		virtual HRESULT __stdcall OnAmbientPropertyChange( DISPID dispID) 
		{
			return S_OK;
		}


		virtual HRESULT __stdcall FreezeEvents(BOOL bFreeze) 
		{
			return S_OK;
		}


		// IOleInPlaceObject
		virtual HRESULT __stdcall GetWindow(HWND* phwnd) override
		{
			if (!phwnd) return E_INVALIDARG;
			*phwnd = this->handle;
			return S_OK;
		}
		virtual HRESULT __stdcall ContextSensitiveHelp(BOOL fEnterMode) override
		{
			return E_NOTIMPL;
		}
		virtual HRESULT __stdcall InPlaceDeactivate(void) override
		{
			return this->inplaceDeactivate();
		}
		virtual HRESULT __stdcall UIDeactivate(void) override
		{
			return this->uiDeactivate();
		}
		virtual HRESULT __stdcall SetObjectRects(LPCRECT lprcPosRect, LPCRECT lprcClipRect) override
		{
			if (lprcPosRect)
			{
				setRects(*lprcPosRect, *lprcClipRect);
/*				posRect_ = *lprcPosRect;
				if (::IsWindow(this->handle))
				{
					::InvalidateRect(this->handle, 0, TRUE);
					::UpdateWindow(this->handle);
				}
*/
			}
			return S_OK;
		}
		virtual HRESULT __stdcall ReactivateAndUndo(void) override
		{
			return S_OK;
		}

	protected:

		// OLE verbs

		virtual HRESULT __stdcall OleVerb_Hide(LONG iVerb, LPMSG lpmsg, IOleClientSite* pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
		{
			if (::IsWindow(this->handle))
				::ShowWindow(this->handle, SW_HIDE);
			if (clientSite_)
			{
				if (site_)
					this->inplaceDeactivate();
				clientSite_->OnShowWindow(FALSE);
			}
			return S_OK;
		}

		virtual HRESULT __stdcall OleVerb_Show(LONG iVerb, LPMSG lpmsg, IOleClientSite* pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
		{
			if (clientSite_)
			{
				clientSite_->ShowObject();
				clientSite_->OnShowWindow(TRUE);
			}
			this->inplaceActivate();
			this->uiActivate();
			return S_OK;
		}

		virtual HRESULT __stdcall OleVerb_Open(LONG iVerb, LPMSG lpmsg, IOleClientSite* pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
		{
			return OleVerb_Show(iVerb, lpmsg, pActiveSite, lindex, hwndParent, lprcPosRect);
		}

		virtual HRESULT __stdcall OleVerb_Primary(LONG iVerb, LPMSG lpmsg, IOleClientSite* pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
		{
			return OleVerb_Show(iVerb, lpmsg, pActiveSite, lindex, hwndParent, lprcPosRect);
		}

		virtual HRESULT __stdcall OleVerb_InplaceActivate(LONG iVerb, LPMSG lpmsg, IOleClientSite* pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
		{
			if (clientSite_)
			{
				clientSite_->ShowObject();
				clientSite_->OnShowWindow(TRUE);
			}
			this->inplaceActivate();
			return S_OK;
		}

		virtual HRESULT __stdcall OleVerb_UIActivate(LONG iVerb, LPMSG lpmsg, IOleClientSite* pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
		{
			if (clientSite_)
			{
				clientSite_->ShowObject();
				clientSite_->OnShowWindow(TRUE);
			}
			this->inplaceActivate();
			this->uiActivate();
			return S_OK;
		}

		virtual HRESULT __stdcall OleVerb_Properties(LONG iVerb, LPMSG lpmsg, IOleClientSite* pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
		{
			return S_OK;
		}

		virtual HRESULT __stdcall OleVerb(LONG iVerb,
			LPMSG lpmsg,
			IOleClientSite* pActiveSite,
			LONG lindex,
			HWND hwndParent,
			LPCRECT lprcPosRect)
		{
			return S_OK;
		}

		// IViewObject(2)

		virtual HRESULT __stdcall Draw(DWORD dwDrawAspect, LONG lindex, void* pvAspect, DVTARGETDEVICE* ptd, HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds, BOOL(__stdcall* pfnContinue)(ULONG_PTR dwContinue), ULONG_PTR dwContinue) override
		{
			T* t = (T*)this;
			RECT r = {
				lprcBounds->left,
				lprcBounds->top,
				lprcBounds->right,
				lprcBounds->bottom
			};
//			::GetClientRect(this->handle,&r);
			t->T::wm_draw(hdcDraw,r);// , lprcBounds, lprcWBounds);
			return S_OK;
		}

		virtual HRESULT __stdcall GetColorSet(DWORD dwDrawAspect, LONG lindex, void* pvAspect, DVTARGETDEVICE* ptd, HDC hicTargetDev, LOGPALETTE** ppColorSet) override
		{
			*ppColorSet = NULL;
			return S_OK;
		}

		virtual HRESULT __stdcall Freeze(DWORD dwDrawAspect, LONG lindex, void* pvAspect, DWORD* pdwFreeze) override
		{
			return S_OK;
		}

		virtual HRESULT __stdcall Unfreeze(DWORD dwFreeze) override
		{
			return S_OK;
		}

		virtual HRESULT __stdcall SetAdvise(DWORD aspects, DWORD advf, IAdviseSink* pAdvSink) override
		{
			if (pAdvSink == 0)
			{
				adviseSink_.release();
				return S_OK;
			}
			adviseAspect_ = aspects;
			adviseAdvf_ = advf;
			adviseSink_ = pAdvSink;
			return S_OK;
		}

		virtual HRESULT __stdcall GetAdvise(DWORD* pAspects, DWORD* pAdvf, IAdviseSink** ppAdvSink) override
		{
			if (!adviseSink_)
			{
				*ppAdvSink = NULL;
				return S_OK;
			}
			if (pAspects)
				*pAspects = adviseAspect_;
			if (pAdvf)
				*pAdvf = adviseAdvf_;

			return adviseSink_.query_interface(IID_IAdviseSink, (void**)ppAdvSink);
		}

		virtual HRESULT __stdcall GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE* ptd, LPSIZEL lpsizel) override
		{
			T* t = (T*)this;
			return t->GetExtent(dwDrawAspect, lpsizel);
		}

		// helpers

		void setRects(const RECT& pos, const RECT& clip)
		{
			posRect_ = pos;
			RECT r;
			::IntersectRect(&r, &posRect_, &clip);

			//posRect_ = r;

			HWND hwnd = this->handle;
			if (hwnd && ::IsWindow(hwnd))
			{
				//::SetWindowPos(hwnd,0,r.left,r.top,r.right-r.left,r.bottom-r.top, SWP_NOZORDER|SWP_NOACTIVATE);
				::MoveWindow(hwnd, r.left, r.top, r.right - r.left, r.bottom - r.top, TRUE);
			}
		}


		virtual HRESULT uiDeactivate()
		{
			uiActive_ = false;

			if (frame_)
			{
				frame_->SetActiveObject(0, 0);
			}
			if (uiWnd_)
			{
				uiWnd_->SetActiveObject(0, 0);
			}
			if (site_)
			{
				punk<IOleControlSite> iocs;
				if (S_OK == site_.query_interface(&iocs))
				{
					if (iocs)
					{
						iocs->OnFocus(FALSE);
					}
				}
				site_->OnUIDeactivate(FALSE);
			}

			return S_OK;
		}

		virtual HRESULT inplaceDeactivate()
		{
			uiDeactivate();
			HWND wnd = this->handle;
			if (::IsWindow(wnd))
				::ShowWindow(wnd, SW_HIDE);

			if (frame_)
			{
				frame_->SetActiveObject(0, 0);
				frame_.release();
			}
			if (uiWnd_)
			{
				uiWnd_->SetActiveObject(0, 0);
				uiWnd_.release();
			}
			if (site_)
			{
				site_->OnInPlaceDeactivate();
			}
			if (site_)
			{
				site_.release();
			}
			if (::IsWindow(this->handle))
				::SetWindowPos(this->handle, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

			return S_OK;
		}

		virtual HRESULT inplaceActivate()
		{
			RECT rPos, rClip;
			if (!clientSite_)
				return E_FAIL;

			if (site_)
			{
				return S_OK;
			}

			if (!site_)
			{
				//HRESULT hr = clientSite_.queryInterface(IID_IOleInPlaceSite,(void**)&site_);
				HRESULT hr = clientSite_.query_interface(&site_);
				if (hr != S_OK)
				{
					return hr;
				}
			}

			HRESULT hr = site_->CanInPlaceActivate();
			if (hr != S_OK)
			{
				site_.release();
				return hr;
			}


			hr = site_->OnInPlaceActivate();
			if (hr != S_OK)
			{
				return hr;
			}

			HWND parent = 0;
			hr = site_->GetWindow(&parent);
			if (hr != S_OK)
			{
				site_.release();
				return hr;
			}

			frame_.release();
			uiWnd_.release();

			OLEINPLACEFRAMEINFO oipfi;
			::ZeroMemory(&oipfi, sizeof(oipfi));
			oipfi.cb = sizeof(oipfi);

			hr = site_->GetWindowContext(&frame_, &uiWnd_, &rPos, &rClip, &oipfi);
			if (hr != S_OK)
			{
				site_.release();
				return hr;
			}

			if (!::IsWindow(this->handle))
			{
				this->create((size_t)1, parent, rPos, WS_CHILD, 0);
				T* that = (T*)this;
				that->AddRef();
			}

			HWND hwnd = this->handle;
			if (::IsWindow(hwnd))
			{
				::SetParent(hwnd, parent);
				::ShowWindow(hwnd, SW_SHOW);
				::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				//::SetActiveWindow(hwnd);
				::SetFocus(hwnd);
			}
			clientSite_->ShowObject();
			if (site_)
			{
				punk<IOleControlSite> iocs;
				if (S_OK == site_.query_interface(&iocs))
				{
					if (iocs)
					{
						iocs->OnFocus(TRUE);
					}
				}
				::SetFocus(hwnd);
			}
			return hr;
		}

		virtual HRESULT uiActivate()
		{
			if (uiActive_)
				return S_OK;

			uiActive_ = true;
			if (site_)
				site_->OnUIActivate();

			if (frame_ && active_)
			{
				frame_->SetActiveObject(*active_, 0);
			}
			if (uiWnd_ && active_)
			{
				uiWnd_->SetActiveObject(*active_, 0);
			}

			HWND hwnd = this->handle;
			if (::IsWindow(hwnd))
			{
				::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			//this->createInplaceMenu();
			return S_OK;
		}

		// inner IOleInPlaceActiveObject
		class ole_inplace_active_obj :
			public implements<ole_inplace_active_obj(IOleInPlaceActiveObject, of<IOleInPlaceActiveObject,IOleWindow>) >
		{
		public:

			ole_inplace_active_obj(control<T>* ctrl)
			{
				ctrl_ = ctrl;
				T* that = (T*)ctrl;
				//that->AddRef();
			}

			virtual ~ole_inplace_active_obj()
			{
				T* that = (T*)ctrl_;
				//that->Release();
			}

			virtual HRESULT __stdcall GetWindow(HWND* phwnd) override
			{
				if (!phwnd) return E_INVALIDARG;

				*phwnd = this->ctrl_->handle;
				return S_OK;
			}

			virtual HRESULT __stdcall ContextSensitiveHelp(BOOL fEnterMode)
			{
				return E_NOTIMPL;
			}

			virtual HRESULT __stdcall TranslateAccelerator(LPMSG lpmsg)
			{
				return S_FALSE;
			}

			virtual HRESULT __stdcall OnFrameWindowActivate(BOOL fActivate)
			{
				return OnDocWindowActivate(fActivate);
			}

			virtual HRESULT __stdcall OnDocWindowActivate(BOOL fActivate)
			{
				if (fActivate)
				{
					if (ctrl_->frame_)
					{
						ctrl_->frame_->SetActiveObject(this, 0);
						/*
						if (ctrl_->sharedMenu_)
						{
							ctrl_->frame_->SetMenu(ctrl_->sharedMenu_, ctrl_->holemenu_, ctrl_->getOleWindow());
							ctrl_->createInplaceMenu();
						}
						*/
					}
				}
				else
				{
					if (ctrl_->frame_)
					{
						ctrl_->frame_->SetActiveObject(0, 0);
					}
				}
				return S_OK;
			}

			virtual HRESULT __stdcall ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow, BOOL fFrameWindow)
			{
				return S_OK;
			}

			virtual HRESULT __stdcall EnableModeless(BOOL fEnable)
			{
				return S_OK;
			}

		protected:
			control<T>* ctrl_;
		};


		punk<ole_inplace_active_obj>    active_;
		punk<IOleInPlaceSite>			site_;
		punk<IOleClientSite>			clientSite_;
		punk<IOleInPlaceFrame>			frame_;
		punk<IOleInPlaceUIWindow>		uiWnd_;
		punk<IOleAdviseHolder>			adviseHolder_;
		punk<IDataAdviseHolder>			dataAdviseHolder_;
		punk<IAdviseSink>				adviseSink_;

		std::wstring					containerApp_;
		std::wstring					containerObj_;
		SIZEL							sizel_;
		RECT							posRect_;

		HWND							parentWindow_ = nullptr;
		bool							isDirty_ = false;
		bool							uiActive_ = false;

		DWORD							adviseAspect_;
		DWORD							adviseAdvf_;

};

	namespace details
	{
		template<class T, class ... Args>
		class interfaces<T(control<T>, Args...)>
		{
		public:

			static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
			{
				if (::IsEqualGUID(riid, IID_IOleObject))
				{
					*ppvObject = (IOleObject*)(that);
					that->AddRef();
					return S_OK;
				}

				if (::IsEqualGUID(riid, IID_IOleControl))
				{
					*ppvObject = (IOleControl*)(that);
					that->AddRef();
					return S_OK;
				}

				if (::IsEqualGUID(riid, IID_IOleWindow))
				{
					*ppvObject = (IOleWindow*)(that);
					that->AddRef();
					return S_OK;
				}

				if (::IsEqualGUID(riid, IID_IOleInPlaceObject))
				{
					*ppvObject = (IOleInPlaceObject*)(that);
					that->AddRef();
					return S_OK;
				}

				if (::IsEqualGUID(riid, IID_IDataObject))
				{
					*ppvObject = (IDataObject*)(that);
					that->AddRef();
					return S_OK;
				}

				if (::IsEqualGUID(riid, IID_IViewObject2))
				{
					*ppvObject = (IViewObject2*)(that);
					that->AddRef();
					return S_OK;
				}

				if (::IsEqualGUID(riid, IID_IViewObject))
				{
					*ppvObject = (IViewObject*)(IViewObject2*)(that);
					that->AddRef();
					return S_OK;
				}

				return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
			}
		};

	}

}