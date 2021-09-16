#pragma once

#include "MTL/sdk.h"
#include "MTL/punk.h"
#include "MTL/win32/uni.h"
#include "MTL/disp/bstr.h"
#include <MLang.h>
#include <regex>

namespace MTL {

	using CodePage = std::pair<int, std::wstring>;
	using CodePages = std::map<int, CodePage >;

	//inline CodePages& codePages();
	inline const CodePage& systemDefaultCodePage();

	namespace details {

		inline CodePages& codePages();

		class CodePagesEnumeration
		{
			friend inline CodePagesEnumeration& codePagesEnumeration();
			friend inline const CodePage& MTL::systemDefaultCodePage();
		public:
			
		private:

			CodePagesEnumeration()
			{
				::EnumSystemCodePages(codePageEnumProc, CP_INSTALLED);
				::GetCPInfoEx(CP_ACP, 0, &cpi_);
			}

			static BOOL CALLBACK codePageEnumProc(LPTSTR lpCodePageString)
			{
				int cp = _wtoi(lpCodePageString);

				CPINFOEX cpi;
				if (::GetCPInfoEx(cp, 0, &cpi))
				{
					std::wstring s(cpi.CodePageName);
					MTL::details::codePages().insert(
						std::make_pair(
							cp, std::make_pair(cp, s)
						)
					);
				}
				return TRUE;
			}
			CPINFOEX cpi_;
		};

		inline CodePagesEnumeration& codePagesEnumeration()
		{
			static CodePagesEnumeration cpe;
			return cpe;
		}

		inline CodePages& codePages()
		{
			static CodePages cp;
			return cp;
		}
	}

	inline CodePages& codePages()
	{
		static details::CodePagesEnumeration& cpe = details::codePagesEnumeration();
		static CodePages& cp = details::codePages();
		return cp;
	}

	inline const CodePage& systemDefaultCodePage()
	{
		UINT codePage = details::codePagesEnumeration().cpi_.CodePage;
			
		static CodePage cp(
			std::make_pair(
				details::codePagesEnumeration().cpi_.CodePage,
				details::codePagesEnumeration().cpi_.CodePageName
			)
		);
		return cp;
	}

	inline const CodePage& UTF7()
	{
		return codePages()[CP_UTF7];
	}

	inline const CodePage& UTF8()
	{
		return codePages()[CP_UTF8];
	}

	inline const CodePage& UTF16()
	{
		static CodePage cp(CP_WINUNICODE, L"1200 (Unicode utf-16)");
		return cp;
	}

	inline const CodePage& ANSI()
	{
		static CodePage cp(CP_WINANSI, L"Ansi (Windows Default)");
		return cp;
	}

	struct FileEncoding
	{
		enum eol_mode { UNIX, DOS } eol;
		unsigned int codePage = CP_UTF8;
		bool isBinary = false;
		bool hasBOM = false;

	};

	namespace details {
		class FileSniffer
		{
		public:

			FileSniffer(const char* raw_bytes, size_t len)
				:raw_bytes_(raw_bytes), len_(len)
			{}

			FileEncoding  investigate()
			{
				return investigate("");
			}

			FileEncoding  investigate(const std::string& h)
			{
				std::string c(raw_bytes_, len_);
				FileEncoding result = getEncoding(c,h);

				// check eol mode 
				size_t p = c.find_first_of("\r\n");
				if (p != std::string::npos)
				{
					if (c[p] == 0x0a)
					{
						result.eol = FileEncoding::UNIX;
					}
					else
					{
						result.eol = FileEncoding::DOS;
					}
				}

				// check for binaries
				bool isBinary = false;
				p = 0;
				while ((c[p]) && (p < c.size()))
					p++;

				if (p < c.size())
				{
					switch (result.codePage)
					{
						case CP_WINUNICODE:
						{
							//TODO: smell
							if ((p != c.size()) && ((p + 1) % 2 != 0))
								isBinary = true;
							break;
						}
						case CP_UTF8:
						default:
						{
							if (p != c.size())
								isBinary = true;
							break;
						}
					}
				}
				result.isBinary = isBinary;
				return result;
			}

			static bool hasUTF8_BOM(const char* c, size_t len)
			{
				if (len < 3)
					return false;
				if ((unsigned char)c[0] == UTF8_BOM()[0])
				{
					if ((unsigned char)c[1] == UTF8_BOM()[1])
					{
						if ((unsigned char)c[2] == UTF8_BOM()[2])
						{
							return true;
						}
					}
				}
				return false;
			}

			static bool hasUTF16_BOM(const char* c, size_t len)
			{
				if (len < 2)
					return false;
				if ((unsigned char)c[0] == UTF16LE_BOM()[0])
				{
					if ((unsigned char)c[1] == UTF16LE_BOM()[1])
					{
						return true;
					}
				}
				return false;
			}

			static const unsigned char* UTF8_BOM()
			{
				static const unsigned char BOM[] = { 0xef, 0xbb, 0xbf };
				return BOM;
			}

			static const unsigned char* UTF16LE_BOM()
			{
				static const unsigned char BOM[] = { 0xff, 0xfe, 0x00 };
				return BOM;
			}

			//std::string convertToUTF8(const std::string& raw_bytes, long enc_hint);

		private:

			const char* raw_bytes_ = 0;
			size_t len_ = 0;

			FileEncoding  getEncoding( const std::string& str,  const std::string& h)
			{
				punk<IMultiLanguage> ml;
				HR hr = ml.createObject(CLSID_CMultiLanguage);

				punk<IMultiLanguage2> ml2(ml);

				// first look for supported ByteOrder4marks
				if (hasUTF8_BOM(raw_bytes_,len_) == true)
				{
					return FileEncoding{ FileEncoding::UNIX, CP_UTF8, false, true };
				}

				if (hasUTF16_BOM(raw_bytes_, len_) == true)
				{
					return FileEncoding{ FileEncoding::UNIX, CP_WINUNICODE, false, true };
				}

				// second check headers for charset specs in content-type
				if (!h.empty())
				{
					std::smatch m;
					std::regex e("content-type.*:(.*);.*charset.*=(.*)");

					if (std::regex_search(h, m, e))
					{
						std::wstring s = MTL::to_wstring(m[2]);
						s = trim(s);

						bstr b((ole_char(s.c_str())));
						MIMECSETINFO minfo;

						HRESULT hr = ml2->GetCharsetInfo( *b, &minfo);
						if (hr == S_OK)
						{
							return FileEncoding{ FileEncoding::UNIX, minfo.uiInternetEncoding, false, false };
						}
					}
				}

				// third, try to find xml decl
				//	<?xml version="1.0" encoding="ISO-8859-1" ?>

				std::string c = str.substr(0, std::min(1024, (int)str.size()));

				std::smatch m_xml;
				std::regex e_xml("<\\?xml +version *= *('|\")[^'\"]*('|\") +encoding *= *('|\")([^'\"]*)('|\")");

				if (std::regex_search(c, m_xml, e_xml))
				{
					std::wstring s = to_wstring(m_xml[4]);
					s = trim(s);

					bstr b(ole_char(s.c_str()));
					MIMECSETINFO minfo;

					HRESULT hr = ml2->GetCharsetInfo( *b, &minfo);
					if (hr == S_OK)
					{
						return FileEncoding{ FileEncoding::UNIX, minfo.uiInternetEncoding, false, false };
					}
				}

				// forth, search body for meta-tag
				// caseless match <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">

				std::smatch m_meta;
				std::regex e_meta("<meta[^>'\"]*http-equiv *= *('|\")content-type('|\") *content *= *('|\")[^'\"]*charset[^'\"=]*=([^'\"]*)('|\") */? *>");

				if (std::regex_search(c, m_meta, e_meta))
				{
					std::string s = m_meta[4];
					s = trim(s);

					// can't be UTF-16 - if we matched the *ASCII* string "UTF-16", then encoding is broken!
					if (s != "UTF-16")
					{
						bstr b(s.c_str());
						MIMECSETINFO minfo;

						HRESULT hr = ml2->GetCharsetInfo( *b, &minfo);
						if (hr == S_OK)
						{
							return FileEncoding{ FileEncoding::UNIX, minfo.uiInternetEncoding, false, false };
						}
					}
				}

				// fith, search body for HTML5 meta-tag <meta charset="UTF-8">

				std::smatch m_metac;
				std::regex e_metac("<meta *charset=['\"]([^'\"]*)['\"]>");
				if (std::regex_search(c, m_metac, e_metac))
				{
					std::string s = m_metac[1];
					s = trim(s);

					// can't be UTF-16 - if we matched the *ASCII* string "UTF-16", then encoding is broken!
					if (s != "UTF-16")
					{
						bstr b(s);
						MIMECSETINFO minfo;

						HRESULT hr = ml2->GetCharsetInfo( *b, &minfo);
						if (hr == S_OK)
						{
							return FileEncoding{ FileEncoding::UNIX, minfo.uiInternetEncoding, false, false };
						}
					}
				}


				// no result yet? start start guessing with IMultiLang
				const int dEncInfoSize = 10;
				int s = dEncInfoSize;
				int l = (int)c.size();
				DetectEncodingInfo dEncInf[dEncInfoSize];
				ml2->DetectInputCodepage(MLDETECTCP_HTML, 0, (char*)c.c_str(), &l, dEncInf, &s);

				if (s)
				{
					DetectEncodingInfo dei = dEncInf[s - 1];
					while (s)
					{
						if (dEncInf[s - 1].nConfidence > 100)
							if (dEncInf[s - 1].nConfidence >= dei.nConfidence)
								dei = dEncInf[s - 1];
						s--;
					}

					return FileEncoding{ FileEncoding::UNIX, dei.nCodePage, false, false };
				}
				return FileEncoding{ FileEncoding::UNIX, CP_UTF8, false, false };;
			}

		};

	}

	inline FileEncoding sniff(const char* raw_bytes, size_t len)
	{
		details::FileSniffer sniffer(raw_bytes, len);
		return sniffer.investigate();
	}

	inline std::string raw_bytes_as_utf8(FileEncoding& fe, const char* raw_bytes, size_t len)
	{
		if (fe.codePage == CP_UTF8)
		{
			size_t adjust = 0;
			if (fe.hasBOM)
			{
				adjust = 3;
			}
			return std::string(raw_bytes + adjust, len - adjust);
		}
		if (fe.codePage == CP_WINUNICODE)
		{
			size_t adjust = 0;
			if (fe.hasBOM)
			{
				adjust = 2;
			}

			std::wstring ws( (wchar_t*)(raw_bytes+adjust), (len - adjust)/sizeof(wchar_t) );
			return to_string(ws);
		}
		std::wstring ws(to_wstring(raw_bytes, (int)len, fe.codePage));
		return to_string(ws);
	}

	inline std::wstring raw_bytes_as_wstring(FileEncoding& fe, const char* raw_bytes, size_t len)
	{
		if (fe.codePage == CP_UTF8)
		{
			size_t adjust = 0;
			if (fe.hasBOM)
			{
				adjust = 3;
			}
			return to_wstring(raw_bytes + adjust, (int)( len - adjust));
		}
		if (fe.codePage == CP_WINUNICODE)
		{
			size_t adjust = 0;
			if (fe.hasBOM)
			{
				adjust = 2;
			}

			return std::wstring((wchar_t*)(raw_bytes + adjust), (len - adjust) / sizeof(wchar_t));
		}
		return to_wstring(raw_bytes,(int)len,fe.codePage);
	}

}