#pragma once
#include "mtl/sdk.h"
#include "mtl/win32/mem.h"
#include <string>
#include <map>
#include <vector>
//#include <MTL/comem.h>

namespace mtl {

	class clipboard
	{
	public:

		class lock
		{
		public:
			lock()
			{
				::OpenClipboard(NULL);
			}

			lock(HWND hWnd)
			{
				::OpenClipboard(hWnd);
			}

			~lock()
			{
				::CloseClipboard();
			}

		private:

		};

		static void clear(HWND hWnd = 0)
		{
			lock guard(hWnd);
			::EmptyClipboard();
		}

		static void set(HWND hWnd, UINT format, HANDLE data)
		{
			HWND owner = ::GetClipboardOwner();

			lock guard(hWnd);
			if (owner != hWnd)
			{
				::EmptyClipboard();
			}
			::SetClipboardData(format, data);
		}

		static void setText(HWND hWnd, const std::wstring& txt)
		{
			global glob(txt, GMEM_MOVEABLE);
			set(hWnd, CF_UNICODETEXT, *glob);
		}

		static void setText(HWND hWnd, const std::string& txt)
		{
			global glob(txt, GMEM_MOVEABLE);
			set(hWnd, CF_TEXT, *glob);
		}

		static HANDLE get(UINT format)
		{
			HANDLE handle = ::GetClipboardData(format);
			return handle;
		}

		static std::string as_string(UINT format)
		{
			lock giard;
			HANDLE handle = ::GetClipboardData(format);
			if (!handle)
				return "";

			global::lock<char*> glock((HGLOBAL)handle);
			std::string result(*glock,(int) glock.size());
			return result;
		}

		static std::wstring as_wstring(UINT format)
		{
			lock guard;
			HANDLE handle = ::GetClipboardData(format);
			if (!handle)
				return L"";

			global::lock<wchar_t*> glock((HGLOBAL)handle);
			std::wstring result(*glock, (int)(glock.size()/sizeof(wchar_t)));
			return result;
		}

		struct format
		{
			format(int id, const std::wstring& desc)
				: format_id(id), description(desc)
			{}

			int format_id;
			std::wstring description;
		};

		static std::map<int, const wchar_t*>& format_description()
		{
			static std::map<int, const wchar_t*> map = {
				{ CF_TEXT, L"Text"},
				{ CF_BITMAP, L"Bitmap"},
				{ CF_METAFILEPICT, L"Metafile Picture"},
				{ CF_SYLK, L"SYLK"},
				{ CF_DIF, L"DIF"},
				{ CF_TIFF, L"TIFF"},
				{ CF_OEMTEXT, L"OEM Text"},
				{ CF_DIB, L"Device Independent Bitmap"},
				{ CF_PALETTE, L"Palette"},
				{ CF_PENDATA, L"Pen Data"},
				{ CF_RIFF, L"RIFF"},
				{ CF_WAVE, L"Wave"},
				{ CF_UNICODETEXT, L"Unicode Text"},
				{ CF_ENHMETAFILE, L"Enhanced Metafile"},
				{ CF_HDROP, L"File Path(s)"},
				{ CF_LOCALE, L"Locale"},
				{ CF_DIBV5, L"DIB (V5)"}
			};
			return map;
		}

		static UINT register_format(const std::wstring& desc)
		{
			UINT f = ::RegisterClipboardFormat(desc.c_str());

			return f;
		}

		static std::vector<format> enumerate()
		{
			std::vector<format> result;

			UINT f = 0;

			lock guard;
			f = ::EnumClipboardFormats(f);
			while (f)
			{
				switch (f)
				{
				case CF_TEXT:
				case CF_BITMAP:
				case CF_METAFILEPICT:
				case CF_SYLK:
				case CF_DIF:
				case CF_TIFF:
				case CF_OEMTEXT:
				case CF_DIB:
				case CF_PALETTE:
				case CF_PENDATA:
				case CF_RIFF:
				case CF_WAVE:
				case CF_UNICODETEXT:
				case CF_ENHMETAFILE:
				case CF_HDROP:
				case CF_LOCALE:
				case CF_DIBV5:
				{
					const wchar_t* desc = format_description()[f];
					result.push_back(format(f, desc));
					break;
				}

				default:
				{
					wchar_t buf[MAX_PATH];
					int c = ::GetClipboardFormatNameW(f, buf, MAX_PATH);

					if (c > 0)
					{
						result.push_back(format(f,std::wstring(buf,c)));
					}
				}
				}
				f = ::EnumClipboardFormats(f);
			}
			return result;
		}
	};
}