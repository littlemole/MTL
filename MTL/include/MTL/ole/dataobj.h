#pragma once

#include "MTL/obj/impl.h"
#include "MTL/persist/stream.h"
#include "MTL/persist/stgm.h"
#include <map>
#include <shellapi.h>

namespace MTL {

	class DataObject : public IDataObject
	{
	public:

		virtual HRESULT __stdcall GetData(FORMATETC* pformatetcIn, STGMEDIUM* pmedium) override
		{
			if (!pformatetcIn) return E_INVALIDARG;
			if (formats_.count(*pformatetcIn) == 0)
			{
				return DV_E_FORMATETC;
			}
						
			Global::Lock<void*> lock(formats_[*pformatetcIn].hGlobal);
			Global dest(*lock, lock.size(), GHND | GMEM_SHARE);
			pmedium->hGlobal = *dest;
			pmedium->tymed = TYMED_HGLOBAL;
			dest.detach();
			return S_OK;
		}


		virtual HRESULT __stdcall GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium) override
		{
			if (!pformatetc) return E_INVALIDARG;
			if (!pmedium) return E_INVALIDARG;
			if (formats_.count(*pformatetc) == 0)
			{
				return DV_E_FORMATETC;
			}

			if (formats_[*pformatetc].tymed != pmedium->tymed)
			{
				return DV_E_TYMED;
			}

			switch (pmedium->tymed)
			{
				case TYMED_HGLOBAL:
				{
					Global::Lock<void*> lock(formats_[*pformatetc].hGlobal);
					Global glob(lock.get(), lock.size(), GHND | GMEM_SHARE);
					pmedium->hGlobal = *glob;
					glob.detach();
					return S_OK;
				}
				case TYMED_ISTREAM:
				{
					stream_view src(formats_[*pformatetc].pstm);
					src.reset();
					std::string data = src.read();
					src.reset();

					stream_view sv(pmedium->pstm);
					sv.reset();
					sv.write(data);
					return S_OK;
				}
			}

			return E_NOTIMPL;
		}

		virtual HRESULT __stdcall QueryGetData(FORMATETC* pformatetc) override
		{
			if (!pformatetc) return E_INVALIDARG;
			if (formats_.count(*pformatetc) == 0)
			{
				return DV_E_FORMATETC;
			}
			return S_OK;
		}

		virtual HRESULT __stdcall GetCanonicalFormatEtc(FORMATETC* pformatectIn, FORMATETC* pformatetcOut) override
		{
			if (!pformatectIn) return E_INVALIDARG;
			if (!pformatetcOut) return E_INVALIDARG;
			if (formats_.count(*pformatectIn) == 0)
			{
				return DV_E_FORMATETC;
			}

			*pformatetcOut = *pformatectIn;
			return S_OK;
		}

		virtual HRESULT __stdcall SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease) override
		{
			if (!pformatetc) return E_INVALIDARG;
			if (!pmedium) return E_INVALIDARG;

			formats_[*pformatetc] = StgMedium(*pmedium);

			return S_OK;
		}

		virtual HRESULT __stdcall EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc) override
		{
			std::vector<FORMATETC> v;
			for (auto& f : formats_)
			{
				v.push_back(f.first);
			}
			if (v.empty())
			{
				return ::SHCreateStdEnumFmtEtc(0, 0, ppenumFormatEtc);
			}
			return ::SHCreateStdEnumFmtEtc( (UINT)v.size(), &v[0], ppenumFormatEtc);
		}

		virtual HRESULT __stdcall DAdvise(FORMATETC* pformatetc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection) override
		{
			return E_NOTIMPL;
		}

		virtual HRESULT __stdcall DUnadvise(DWORD dwConnection) override
		{
			return E_NOTIMPL;
		}

		virtual HRESULT __stdcall EnumDAdvise(IEnumSTATDATA** ppenumAdvise) override
		{
			return E_NOTIMPL;
		}

	protected:
		std::map<format_etc, StgMedium> formats_;
	};

	class DataTransferObject : public implements<DataTransferObject(::MTL::DataObject)>
	{
	public:

		DataTransferObject()
		{}

		DataTransferObject(MTL::format_etc& fe, MTL::StgMedium& stgm)
		{
			init(fe, stgm);
		}

		DataTransferObject( MTL::format_etc& fe, void* d, size_t s)
		{
			MTL::StgMedium stgm(d, s);
			init(fe, stgm);
		}

		DataTransferObject(MTL::format_etc& fe, const std::wstring& s)
		{
			MTL::StgMedium stgm(s);
			init(fe, stgm);
		}

		DataTransferObject(MTL::format_etc& fe, const std::string& s)
		{
			MTL::StgMedium stgm(s);
			init(fe, stgm);
		}

		virtual ~DataTransferObject() {}

		HRESULT setDropFiles(const std::vector<std::wstring> v)
		{
			std::wstring s;
			int c = 0;
			for (unsigned int i = 0; i < v.size(); i++)
			{
				s += v[i];
				s += L'\0';
				c += (int)(v[i].size() + 1) * sizeof(wchar_t);
			}

			Global glob;
			glob.alloc(c + 1 + sizeof(DROPFILES), GHND | GMEM_SHARE);
			Global::Lock<DROPFILES*> lock(*glob);

			DROPFILES* drop = *lock;
			drop->pFiles = sizeof(DROPFILES);
			drop->fWide = TRUE;
			memcpy(((char*)drop + sizeof(DROPFILES)), s.c_str(), c + sizeof(wchar_t));

			StgMedium medium(*glob);// Nono , GHND | GMEM_SHARE);
			format_etc_dropfile fed;
			HRESULT hr = SetData(&fed, &medium, TRUE);
			glob.detach();

			return hr;
		}

	protected:

		void init(MTL::format_etc& fe, MTL::StgMedium& stgm)
		{
			SetData(&fe, &stgm, TRUE);
		}
	};

	template<class T>
	punk<IDataObject> dataObject(MTL::format_etc& fe, const T& t)
	{
		MTL::punk<IDataObject> ido(new MTL::DataTransferObject(fe, t));
		return ido;
	}

	template<class T>
	punk<IDataObject> dataObject(CLIPFORMAT cf, const T& t)
	{
		MTL::format_etc fe(cf);
		return dataObject(fe, t);
	}

	inline punk<IDataObject> dataObject(MTL::format_etc& fe, void* p, size_t s)
	{
		MTL::punk<IDataObject> ido(new MTL::DataTransferObject(fe, p,s));
		return ido;
	}

	inline punk<IDataObject> dataObject(CLIPFORMAT cf, void* p, size_t s)
	{
		MTL::format_etc fe(cf);
		return dataObject(fe, p, s);
	}


	class dataobj_view
	{
	public:
		dataobj_view(IDataObject* dataObj)
			: dataObject(dataObj)
		{}

		bool has(MTL::format_etc& fe)
		{
			HRESULT hr = dataObject->QueryGetData(&fe);
			return hr == S_OK;
		}

		bool has(CLIPFORMAT cf)
		{
			MTL::format_etc fe(cf);
			return has(fe);
		}

		std::wstring wstring(MTL::format_etc& fe)
		{
			MTL::StgMedium sm;
			HRESULT hr = dataObject->GetData(&fe, &sm);
			if (hr != S_OK)
				return L"";

			if (sm.tymed != TYMED_HGLOBAL || !sm.hGlobal)
				return L"";

			MTL::Global::Lock<wchar_t*> glob(sm.hGlobal);
			std::wstring result(*glob);
			return result;
		}

		std::wstring wstring(CLIPFORMAT cf)
		{
			MTL::format_etc fe(cf);
			return wstring(fe);
		}


		std::string string(MTL::format_etc& fe)
		{
			MTL::StgMedium sm;
			HRESULT hr = dataObject->GetData(&fe, &sm);
			if (hr != S_OK)
				return "";

			if (sm.tymed != TYMED_HGLOBAL || !sm.hGlobal)
				return "";

			MTL::Global::Lock<char*> glob(sm.hGlobal);
			std::string result(*glob);
			return result;
		}

		std::string string(CLIPFORMAT cf)
		{
			MTL::format_etc fe(cf);
			return string(fe);
		}

		std::vector<std::wstring> dropFiles()
		{
			std::vector<std::wstring>	v;

			if (!dataObject)
				return v;

			format_etc_dropfile	fe;
			MTL::StgMedium		sm;

			if (S_OK != dataObject->GetData(&fe, &sm))
			{
				return v;
			}

			Global::Lock<HDROP> lock(sm.hGlobal);
			HDROP hDrop = *lock;
			if (!hDrop)
			{
				return v;
			}

			int n = ::DragQueryFile(hDrop, 0xFFFFFFFF, 0, 0);
			int i = 0;
			while (true)
			{
				int len = ::DragQueryFile(hDrop, i, 0, 0);
				if (!len)
					break;

				wbuff buf(len + 1);
				::DragQueryFile(hDrop, i, buf, len + 1);
				v.push_back(buf.toString());
				i++;
			}
			return v;
		}

	private:
		IDataObject* dataObject = nullptr;
	};


	template<class T>
	bool doDragDrop( 
		MTL::format_etc& fe, 
		const T& t, 
		DWORD effect = DROPEFFECT_COPY | DROPEFFECT_MOVE, 
		DWORD* pEffect = nullptr, 
		IDropSource* drop = *MTL::dropSource()
	)
	{
		DWORD tmp;
		pEffect = pEffect ? pEffect : &tmp;

		auto obj = dataObject(fe, t);

		HRESULT hr = ::DoDragDrop(*obj, drop, effect, pEffect);

		return hr == DRAGDROP_S_DROP;
	}

	template<class T>
	bool doDragDrop(
		CLIPFORMAT cf,
		const T& t,
		DWORD effect = DROPEFFECT_COPY | DROPEFFECT_MOVE,
		DWORD* pEffect = nullptr,
		IDropSource* drop = *MTL::dropSource()
	)
	{
		DWORD tmp;
		pEffect = pEffect ? pEffect : &tmp;

		auto obj = dataObject(cf, t);

		HRESULT hr = ::DoDragDrop(*obj, drop, effect, pEffect);

		return hr == DRAGDROP_S_DROP;
	}

	/* deprecated */
	inline std::vector<std::wstring> vectorFromDataObject(IDataObject* ido)
	{
		format_etc_dropfile			fe;

		STGMEDIUM					sm;
		wchar_t*					fname = 0;
		int							len = 0;
		int							n = 0;

		std::vector<std::wstring>	v;

		if (!ido)
			return v;

		if (S_OK != ido->GetData(&fe, &sm))
		{
			return v;
		}


		Global glob(sm.hGlobal);
		Global::Lock<HDROP> lock(*glob);
		HDROP hDrop = *lock;
		if (hDrop)
		{
			n = ::DragQueryFile(hDrop, 0xFFFFFFFF, 0, 0);
			int i = 0;
			while (true)
			{
				len = ::DragQueryFile(hDrop, i, 0, 0);
				if (!len)
					break;

				wbuff buf(len + 1);
				::DragQueryFile(hDrop, i, buf, len + 1);
				v.push_back(buf.toString());
				i++;
			}
		}
		glob.detach();
		::ReleaseStgMedium(&sm);
		return v;
	}

	namespace Shell{

		class DataObject : public implements<DataObject(::MTL::DataObject)>
		{
		public:
			DataObject(std::vector<std::wstring>& v, bool cut = false)
				: cut_(cut)
			{
				{
					DWORD de = DROPEFFECT_COPY;
					if (cut_)
					{
						de = DROPEFFECT_MOVE;
					}

					StgMedium medium(de, GHND | GMEM_SHARE);
					format_etc_pref_dropeffect fepde;
					::MTL::DataObject::SetData(&fepde, &medium, TRUE);
				}
				
				{
					std::string s;
					for (unsigned int i = 0; i < v.size(); i++)
					{
						s += to_string(v[i]);
						s += "\r\n";
					}
					StgMedium medium(s, GHND | GMEM_SHARE);
					format_etc_text fet;
					::MTL::DataObject::SetData(&fet, &medium, TRUE);
				}

				{
					std::wstring s;
					for (unsigned int i = 0; i < v.size(); i++)
					{
						s += v[i];
						s += L"\r\n";
					}
					StgMedium medium(s, GHND | GMEM_SHARE);
					format_etc_unicodetext feut;
					::MTL::DataObject::SetData(&feut, &medium, TRUE);
				}
				
				{
					std::wstring s;
					int c = 0;
					for (unsigned int i = 0; i < v.size(); i++)
					{
						s += v[i];
						s += L'\0';
						c += (int)(v[i].size() + 1)*sizeof(wchar_t);
					}

					Global glob;
					glob.alloc(c + 1 + sizeof(DROPFILES), GHND | GMEM_SHARE);
					Global::Lock<DROPFILES*> lock(*glob);

					DROPFILES* drop = *lock;
					drop->pFiles = sizeof(DROPFILES);
					drop->fWide = TRUE;
					memcpy(((char*)drop + sizeof(DROPFILES)), s.c_str(), c + sizeof(wchar_t));

					StgMedium medium(*glob);// Nono , GHND | GMEM_SHARE);
					format_etc_dropfile fed;
					::MTL::DataObject::SetData(&fed, &medium, TRUE);
					glob.detach();
				}
			}

			HRESULT __stdcall SetData(FORMATETC* pFormatetc, STGMEDIUM* pmedium, BOOL fRelease)
			{
				//ODBGS("ShellDataObject::SetData");

				format_etc_dropeffect fede;

				if (pFormatetc->cfFormat == fede.format())
				{
					if (pmedium->tymed == TYMED_HGLOBAL)
					{
						Global glob(pmedium->hGlobal);
						Global::Lock<DWORD*> lock(*glob);
						DWORD de = **lock;
						glob.detach();
						onDrop.fire(de);
					}
					//return S_OK;
				}
				if (fRelease)
					::ReleaseStgMedium(pmedium);
				return S_OK;
			}

			Event<void(DWORD)> onDrop;
		private:
			bool							cut_;
		};
	}

	namespace details {

		template<class T, class ... Args>
		class interfaces<T(DataObject, Args...)>
		{
		public:

			static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
			{
				if (::IsEqualGUID(riid, IID_IDataObject))
				{
					*ppvObject = (IDataObject*)(that);
					that->AddRef();
					return S_OK;
				}

				return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
			}
		};
	}


	template<class T>
	class DropTargetBase : public implements<T(IDropTarget)>
	{
	public:
		virtual ~DropTargetBase() {}

		HRESULT virtual __stdcall DragEnter(IDataObject*, DWORD grfKeyState, POINTL, DWORD* pEffect)
		{
			//*pEffect = DROPEFFECT_COPY;
			if (grfKeyState & MK_SHIFT) *pEffect = DROPEFFECT_MOVE;
			if (grfKeyState & MK_CONTROL) *pEffect = DROPEFFECT_MOVE;
			return S_OK;
		}

		HRESULT virtual __stdcall DragOver(DWORD grfKeyState, POINTL, DWORD* pEffect)
		{
			//*pEffect = DROPEFFECT_COPY;
			if (grfKeyState & MK_SHIFT) *pEffect = DROPEFFECT_MOVE;
			if (grfKeyState & MK_CONTROL) *pEffect = DROPEFFECT_MOVE;
			return S_OK;
		}

		HRESULT virtual __stdcall DragLeave()
		{
			return S_OK;
		}

		HRESULT virtual __stdcall Drop(IDataObject*, DWORD grfKeyState, POINTL, DWORD* pEffect)
		{
			*pEffect = DROPEFFECT_NONE;
			return S_OK;
		}
	};

	template<class T>
	class DropSrcBase : public implements<T(IDropSource)>
	{
	public:
		virtual ~DropSrcBase() {}

		HRESULT virtual __stdcall QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
		{
			if (fEscapePressed)
				return DRAGDROP_S_CANCEL;
			if (!(grfKeyState & MK_LBUTTON))
				return DRAGDROP_S_DROP;
			return S_OK;
		}

		HRESULT virtual __stdcall GiveFeedback(DWORD dwEffect)
		{
			return DRAGDROP_S_USEDEFAULTCURSORS;
		}

	};

	class DefaultDropSource : public DropSrcBase<DefaultDropSource>
	{
	public:

	};

	inline MTL::punk<IDropSource> dropSource()
	{
		MTL::punk<IDropSource> source(new DefaultDropSource());
		return source;
	}

	class DefaultDropTarget : public DropTargetBase<DefaultDropTarget>
	{
	public:

		MTL::Event<void(IDataObject*,DWORD,DWORD&)> onDrop;
		MTL::format_etc fe;
		DWORD effect = 0;

		DefaultDropTarget(MTL::format_etc f, DWORD ef = DROPEFFECT_COPY) 
			: fe(f), effect(ef) 
		{}

		HRESULT virtual __stdcall Drop(IDataObject* ido, DWORD grfKeyState, POINTL, DWORD* pEffect)
		{
			OutputDebugString(L"DROP");

			DWORD effect = *pEffect;
			onDrop.fire(ido, grfKeyState,effect);
			*pEffect = effect;
			return S_OK;
		}
	};

	inline MTL::punk<DefaultDropTarget> dropTarget(MTL::format_etc fe)
	{
		MTL::punk<DefaultDropTarget> target(new DefaultDropTarget(fe));
		return target;
	}


	inline MTL::punk<DefaultDropTarget> dropTarget(CLIPFORMAT cf)
	{
		MTL::format_etc fe(cf);
		return dropTarget(fe);
	}

}