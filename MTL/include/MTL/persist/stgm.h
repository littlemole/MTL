#pragma once

#include "MTL/win32/uni.h"
#include "MTL/punk.h"
#include "MTL/util/path.h"
#include "MTL/win32/mem.h"
#include <sstream>
#include <ShlObj.h>

namespace MTL {

	class StgMedium : public STGMEDIUM
	{
	public:
		StgMedium()
		{
			this->tymed = 0;
			this->pUnkForRelease = 0;
			this->hGlobal = 0;
		}

		StgMedium(const STGMEDIUM& rhs)
		{
			this->tymed = rhs.tymed;
			this->pUnkForRelease = 0;
			this->hGlobal = 0;
			::CopyStgMedium(&rhs, this);
		}

		StgMedium(STGMEDIUM&& rhs) 
		{
			this->tymed = rhs.tymed;
			this->pUnkForRelease = rhs.pUnkForRelease;
			this->hGlobal = rhs.hGlobal;
			::ZeroMemory(&rhs, sizeof(STGMEDIUM));
		}

		~StgMedium()
		{
			::ReleaseStgMedium(this);
		}

		HRESULT copy_to(STGMEDIUM* stm)
		{
			if (!stm) return E_INVALIDARG;
			// nope ::ReleaseStgMedium(stm);

			::CopyStgMedium(this,stm);
			return S_OK;
		}

		StgMedium& operator=(StgMedium&& rhs)
		{
			if (this == &rhs)
			{
				return *this;
			}
			::ReleaseStgMedium(this);
			this->tymed = rhs.tymed;
			this->pUnkForRelease = rhs.pUnkForRelease;
			this->hGlobal = rhs.hGlobal;
			::ZeroMemory(&rhs, sizeof(STGMEDIUM));

			return *this;
		}


		StgMedium& operator=(const STGMEDIUM& rhs)
		{
			if (this == &rhs)
			{
				return *this;
			}
			::ReleaseStgMedium(this);
			::CopyStgMedium(&rhs, this);
			return *this;
		}

		StgMedium(HGLOBAL hglob, IUnknown* pUnk = 0)
		{
			this->tymed = TYMED_HGLOBAL;
			this->pUnkForRelease = pUnk;
			this->hGlobal = hglob;
			if (pUnk)
			{
				pUnk->AddRef();
			}
		}

		StgMedium(const std::wstring& s, int opt = GMEM_MOVEABLE | GMEM_NODISCARD, IUnknown* pUnk = 0)
		{
			Global glob(s, opt);
			this->tymed = TYMED_HGLOBAL;
			this->pUnkForRelease = pUnk;
			if (pUnk)
				pUnk->AddRef();
			this->hGlobal = *glob;
			glob.detach();
		}

		StgMedium(const std::string& s, int opt = GMEM_MOVEABLE | GMEM_NODISCARD, IUnknown* pUnk = 0)
		{
			Global glob(s, opt);
			this->tymed = TYMED_HGLOBAL;
			this->pUnkForRelease = pUnk;
			if (pUnk)
				pUnk->AddRef();
			this->hGlobal = *glob;
			glob.detach();
		}


		template<class T>
		StgMedium(const T& t, int opt = GMEM_MOVEABLE | GMEM_NODISCARD, IUnknown* pUnk = 0)
		{
			Global glob( (void*)&t, sizeof(T), opt);
			this->tymed = TYMED_HGLOBAL;
			this->pUnkForRelease = pUnk;
			this->hGlobal = *glob;
			if (pUnk)
				pUnk->AddRef();
			glob.detach();
		}


		StgMedium(void* v, size_t size, int opt = GMEM_MOVEABLE | GMEM_NODISCARD, IUnknown* pUnk = 0)
		{
			Global glob(v, size, opt);
			this->tymed = TYMED_HGLOBAL;
			this->pUnkForRelease = pUnk;
			this->hGlobal = *glob;
			if (pUnk)
				pUnk->AddRef();
			glob.detach();
		}

	private:
	};


	//////////////////////////////////////////////////////////////////////////////////

	struct format_etc : public FORMATETC
	{
		format_etc(
			CLIPFORMAT cf,
			DVTARGETDEVICE* dv = NULL,
			DWORD aspect = DVASPECT_CONTENT,
			LONG index = -1,
			DWORD t = TYMED_HGLOBAL
		)
		{
			cfFormat = cf;
			ptd = dv;
			dwAspect = aspect;
			lindex = index;
			tymed = t;
		}

		format_etc(const FORMATETC& rhs)
		{
			cfFormat = rhs.cfFormat;
			ptd = rhs.ptd;
			dwAspect = rhs.dwAspect;
			lindex = rhs.lindex;
			tymed = rhs.tymed;
		}

		virtual UINT format()
		{
			return cfFormat;
		}

		bool operator < (const format_etc & rhs) const
		{
			if( cfFormat != rhs.cfFormat)
				return cfFormat < rhs.cfFormat;

			if (ptd != rhs.ptd)
				return ptd < rhs.ptd;

			if (dwAspect != rhs.dwAspect)
				return dwAspect < rhs.dwAspect;

			if (lindex != rhs.lindex)
				return lindex < rhs.lindex;

			if (tymed != rhs.tymed)
				return tymed < rhs.tymed;

			return false;
		}
	};

	//////////////////////////////////////////////////////////////////////////////////

	struct format_etc_dropfile : public format_etc
	{
		format_etc_dropfile() : format_etc(CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL) {}
	};

	//////////////////////////////////////////////////////////////////////////////////

	struct format_etc_text : public format_etc
	{
		format_etc_text() : format_etc(CF_TEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL) {}
	};

	//////////////////////////////////////////////////////////////////////////////////

	struct format_etc_unicodetext : public format_etc
	{
		format_etc_unicodetext() : format_etc(CF_UNICODETEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL) {}
	};

	//////////////////////////////////////////////////////////////////////////////////

	struct format_etc_dropeffect : public format_etc
	{
		format_etc_dropeffect() : format_etc(format(), NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL) {}
		virtual UINT format() override
		{
			static UINT ui = ::RegisterClipboardFormat(CFSTR_PERFORMEDDROPEFFECT);
			return ui;
		}
	};

	//////////////////////////////////////////////////////////////////////////////////

	struct format_etc_pref_dropeffect : public format_etc
	{
		format_etc_pref_dropeffect() : format_etc(format(), NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL) {}
		virtual UINT format() override
		{
			static UINT ui = ::RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
			return ui;
		}
	};


	//////////////////////////////////////////////////////////////////////
	// a dummy OLE storage implementation
	// so we can provide a IStorage Interface
	//////////////////////////////////////////////////////////////////////

	namespace ole {

		class DummyStorage : public IStorage
		{
		public:

			HRESULT virtual __stdcall CreateStream(const WCHAR* pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream** ppstm)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall OpenStream(const WCHAR* pwcsName, void* reserved1, DWORD grfMode, DWORD reserved2, IStream** ppstm)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall CreateStorage(const WCHAR* pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage** ppstg)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall OpenStorage(const WCHAR* pwcsName, IStorage* pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage** ppstg)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall CopyTo(DWORD ciidExclude, IID const* rgiidExclude, SNB snbExclude, IStorage* pstgDest)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall MoveElementTo(const OLECHAR* pwcsName, IStorage* pstgDest, const OLECHAR* pwcsNewName, DWORD grfFlags)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall Commit(DWORD grfCommitFlags)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall Revert()
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall EnumElements(DWORD reserved1, void* reserved2, DWORD reserved3, IEnumSTATSTG** ppenum)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall DestroyElement(const OLECHAR* pwcsName)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall RenameElement(const WCHAR* pwcsOldName, const WCHAR* pwcsNewName)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall SetElementTimes(const WCHAR* pwcsName, FILETIME const* pctime, FILETIME const* patime, FILETIME const* pmtime)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall SetClass(REFCLSID clsid)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall SetStateBits(DWORD grfStateBits, DWORD grfMask)
			{
				return E_NOTIMPL;
			}

			HRESULT virtual __stdcall Stat(STATSTG* pstatstg, DWORD grfStatFlag)
			{
				return E_NOTIMPL;
			}

		};


		//////////////////////////////////////////////////////////////////////////////////


	}
}