#pragma once
#include <windows.h>
#include <string>
#include <map>
#include <vector>
//#include <MTL/comem.h>

namespace MTL {

	class Clipboard
	{
	public:

		class Lock
		{
		public:
			Lock()
			{
				::OpenClipboard(NULL);
			}

			Lock(HWND hWnd)
			{
				::OpenClipboard(hWnd);
			}

			~Lock()
			{
				::CloseClipboard();
			}

		private:

		};

		static void clear(HWND hWnd = 0)
		{
			Lock lock(hWnd);
			::EmptyClipboard();
		}

		static void setData(HWND hWnd, UINT format, HANDLE data)
		{
			HWND owner = ::GetClipboardOwner();

			Lock lock(hWnd);
			if (owner != hWnd)
			{
				::EmptyClipboard();
			}
			::SetClipboardData(format, data);
		}

		static void setText(HWND hWnd, const std::wstring& txt)
		{
			Global glob(txt, GMEM_MOVEABLE);
			setData(hWnd, CF_UNICODETEXT, *glob);
		}

		static void setText(HWND hWnd, const std::string& txt)
		{
			Global glob(txt, GMEM_MOVEABLE);
			setData(hWnd, CF_TEXT, *glob);
		}

		static HANDLE getData(UINT format)
		{
			HANDLE handle = ::GetClipboardData(format);
			return handle;
		}

		static std::string as_string(UINT format)
		{
			Lock lock;
			HANDLE handle = ::GetClipboardData(format);
			if (!handle)
				return "";

			Global::Lock<char*> glock((HGLOBAL)handle);
			std::string result(*glock,(int) glock.size());
			return result;
		}

		static std::wstring as_wstring(UINT format)
		{
			Lock lock;
			HANDLE handle = ::GetClipboardData(format);
			if (!handle)
				return L"";

			Global::Lock<wchar_t*> glock((HGLOBAL)handle);
			std::wstring result(*glock, (int)(glock.size()/sizeof(wchar_t)));
			return result;
		}

		struct Format
		{
			Format(int id, const wchar_t* desc)
				: format_id(id), description(desc)
			{}

			int format_id;
			const wchar_t* description;
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

		static std::vector<Format> enumerate()
		{
			std::vector<Format> result;

			UINT format = 0;

			Lock lock;
			format = ::EnumClipboardFormats(format);
			while (format)
			{
				switch (format)
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
					result.push_back(Format(format, format_description()[format]));
					break;
				}

				default:
				{
					wchar_t buf[MAX_PATH];
					int c = ::GetClipboardFormatNameW(format, buf, MAX_PATH);

					if (c > 0)
					{
						std::wstring title(buf, c);
						result.push_back(Format(format, title.c_str()));
					}
				}
				}
				format = ::EnumClipboardFormats(format);
			}
			return result;
		}
	};
}