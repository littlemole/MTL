#pragma once

#include "MTL/sdk.h"
//#include "MTL/win32/module.h"
//#include "MTL/win32/mem.h"
#include "MTL/win/clipboard.h"
#include "MTL/win/app.h"
#include "MTL/win/wc.h"
#include "MTL/win/wnd.h"
#include "MTL/win/gdi.h"
//#include "MTL/win/layout.h"
//#include "MTL/win/ctrl.h"
#include "MTL/win/codlg.h"
//#include "MTL/win/dlg.h"
#include "MTL/util/path.h"
//#include "MTL/obj/impl.h"
//#include "MTL/ole/shell.h"
//#include "MTL/disp/variant.h"
#include "MTL/util/rgb.h"
#include "mtl/persist/xml.h"

namespace mtl {


struct XmlStyle
{
	int id;
	std::string desc;
	std::string backgroundColor;
	std::string foreColor;
	std::string bold;
	std::string eol;
	std::string italic;
	int fontSize;
	std::string font;
};

template<>
struct meta::Data<XmlStyle>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("style"),
			member("id", &XmlStyle::id, meta::attribute()),
			member("desc", &XmlStyle::desc, meta::attribute()),
			member("background-color", &XmlStyle::backgroundColor, meta::attribute()),
			member("foreground-color", &XmlStyle::foreColor, meta::attribute()),
			member("bold", &XmlStyle::bold, meta::attribute()),
			member("eol", &XmlStyle::eol, meta::attribute()),
			member("italic", &XmlStyle::italic, meta::attribute()),
			member("font-size", &XmlStyle::fontSize, meta::attribute()),
			member("font", &XmlStyle::font, meta::attribute())
		);
	}
};


struct XmlStyleSet
{
	int id;
	std::string syntax;
	std::vector<std::string> keywords;
	std::vector<XmlStyle> style;
};

template<>
struct meta::Data<XmlStyleSet>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("styleset"),
			member("id", &XmlStyleSet::id, meta::attribute()),
			member("syntax", &XmlStyleSet::syntax, meta::attribute()),
			member("keywords", &XmlStyleSet::keywords),
			member("style", &XmlStyleSet::style)
		);
	}
};

struct XmlStyleSets
{
	std::vector<XmlStyleSet> styleSet;
};

template<>
struct meta::Data<XmlStyleSets>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("stylesets"),
			member("styleset", &XmlStyleSets::styleSet)
		);
	}
};



class ScintillaWnd;

template<>
class window_class<ScintillaWnd>
{
public:
	const wchar_t* name()
	{
		return L"Scintilla";
	}
};

class ScintillaWnd : public ctrl<ScintillaWnd>
{
public:

	XmlStyleSets xmlStyleSets;

	ScintillaWnd()
	{

	}

	virtual LRESULT wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
	{
		switch (message)
		{
			case WM_ERASEBKGND:
			{
				return 1;
				break;
			}
		}
		return ctrl<ScintillaWnd>::wndProc(hwnd, message, wParam, lParam);
	}

	/*
	virtual LRESULT wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
	{
		switch (message)
		{
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLORSCROLLBAR:
		{
			RECT cr = getClientRect();
			break;
		}
		}
		return Ctrl<ScintillaWnd>::wndProc(hwnd, message, wParam, lParam);
	}
	*/

	LRESULT setCodePage(int cp)
	{
		return sendMsg(SCI_SETCODEPAGE, (WPARAM)cp, (LPARAM)0);
	}
	LRESULT getLength()
	{
		return sendMsg(SCI_GETLENGTH, (WPARAM)0, (LPARAM)0);
	}

	LRESULT getCharAt(int pos)
	{
		return sendMsg(SCI_GETCHARAT, (WPARAM)pos, (LPARAM)0);
	}

	LRESULT getLineLength(int line)
	{
		return sendMsg(SCI_LINELENGTH, (WPARAM)line, (LPARAM)0);
	}

	LRESULT getLineCount()
	{
		return sendMsg(SCI_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
	}

	LRESULT getFirstVisibleLine()
	{
		return sendMsg(SCI_GETFIRSTVISIBLELINE, (WPARAM)0, (LPARAM)0);
	}

	LRESULT getLinesOnScreen()
	{
		return sendMsg(SCI_LINESONSCREEN, (WPARAM)0, (LPARAM)0);
	}

	LRESULT gotoLine(int line)
	{
		return sendMsg(SCI_GOTOLINE, (WPARAM)line, (LPARAM)0);
	}

	LRESULT lineFromPos(int pos)
	{
		return sendMsg(SCI_LINEFROMPOSITION, (WPARAM)pos, (LPARAM)0);
	}

	LRESULT lineEndPos(int line)
	{
		return sendMsg(SCI_GETLINEENDPOSITION, (WPARAM)line, (LPARAM)0);
	}

	LRESULT posFromLine(int line)
	{
		return sendMsg(SCI_POSITIONFROMLINE, (WPARAM)line, (LPARAM)0);
	}

	LRESULT pos()
	{
		return sendMsg(SCI_GETCURRENTPOS, (WPARAM)0, (LPARAM)0);
	}

	LRESULT pos(int pos)
	{
		return sendMsg(SCI_SETCURRENTPOS, (WPARAM)pos, (LPARAM)0);
	}

	LRESULT anchor()
	{
		return sendMsg(SCI_GETANCHOR, (WPARAM)0, (LPARAM)0);
	}

	void anchor(int pos)
	{
		sendMsg(SCI_SETANCHOR, (WPARAM)pos, (LPARAM)0);
	}

	LRESULT insertText(const std::string& txt, int p = -1)
	{
		if (p == -1)
			p = (int)pos();
		int pos = p + (int)txt.size();
		sendMsg(SCI_INSERTTEXT, (WPARAM)p, (LPARAM)txt.c_str());
		this->setSel(pos, pos);
		return TRUE;
	}

	LRESULT replaceSel(const std::string& txt)
	{
		return sendMsg(SCI_REPLACESEL, (WPARAM)0, (LPARAM)(txt.c_str()));
	}

	LRESULT setSel(int anchorPos, int currentPos)
	{
		return sendMsg(SCI_SETSEL, (WPARAM)anchorPos, (LPARAM)currentPos);
	}

	LRESULT getSelStart()
	{
		return sendMsg(SCI_GETSELECTIONSTART, (WPARAM)0, (LPARAM)0);
	}

	void setSelStart(int pos)
	{
		sendMsg(SCI_SETSELECTIONSTART, (WPARAM)pos, (LPARAM)0);
	}

	void setSelEnd(int pos)
	{
		sendMsg(SCI_SETSELECTIONEND, (WPARAM)pos, (LPARAM)0);
	}

	LRESULT getSelEnd()
	{
		return sendMsg(SCI_GETSELECTIONEND, (WPARAM)0, (LPARAM)0);
	}

	LRESULT getModified()
	{
		return sendMsg(SCI_GETMODIFY, (WPARAM)0, (LPARAM)0);
	}

	LRESULT setReadOnly(bool b)
	{
		return sendMsg(SCI_SETREADONLY, (WPARAM)b, (LPARAM)0);
	}

	LRESULT getReadOnly(bool b)
	{
		return sendMsg(SCI_GETREADONLY, (WPARAM)0, (LPARAM)0);
	}


	LRESULT setText(const std::string& txt)
	{
		return  sendMsg(SCI_SETTEXT, (WPARAM)0, (LPARAM)(txt.c_str()));
	}

	std::string getText()
	{
		size_t len = length() +1 ;
		cbuff buf(len);
		LRESULT lr = sendMsg(SCI_GETTEXT, (WPARAM)len, (LPARAM)(char*)buf);
		return buf.toString();
	}

	size_t length()
	{
		return (size_t)sendMsg(SCI_GETLENGTH, 0, 0);
	}

	void setDropTarget(IDropTarget* dropTarget)
	{
		::RevokeDragDrop(handle);
		::RegisterDragDrop(handle, dropTarget);
	}

	LRESULT setSavePoint()
	{
		return sendMsg(SCI_SETSAVEPOINT, (WPARAM)0, (LPARAM)0);
	}

	LRESULT cut()
	{
		return sendMsg(SCI_CUT, (WPARAM)0, (LPARAM)0);
	}

	LRESULT copy()
	{
		return sendMsg(SCI_COPY, (WPARAM)0, (LPARAM)0);
	}

	LRESULT paste()
	{
		return sendMsg(SCI_PASTE, (WPARAM)0, (LPARAM)0);
	}

	LRESULT undo()
	{
		return sendMsg(SCI_UNDO, (WPARAM)0, (LPARAM)0);
	}

	LRESULT redo()
	{
		return sendMsg(SCI_REDO, (WPARAM)0, (LPARAM)0);
	}

	LRESULT getTabWidth()
	{
		return sendMsg(SCI_GETTABWIDTH, (WPARAM)0, (LPARAM)0);
	}

	LRESULT setTabWidth(int w)
	{
		return sendMsg(SCI_SETTABWIDTH, (WPARAM)w, (LPARAM)0);
	}

	LRESULT setUseTabs(bool b)
	{
		return sendMsg(SCI_SETUSETABS, (WPARAM)b, (LPARAM)0);
	}

	LRESULT getUseTabs()
	{
		return sendMsg(SCI_GETUSETABS, (WPARAM)0, (LPARAM)0);
	}



	LRESULT printPage(bool bDraw, Sci_RangeToFormat* pfr)
	{
		return sendMsg(SCI_FORMATRANGE, (WPARAM)bDraw, (LPARAM)pfr);
	}

	void getnextSearchPos_(int i)
	{
		nextSearchPos_ = i;
	}

	LRESULT findText(int searchFlags, Sci_TextToFind* ttf)
	{
		return sendMsg(SCI_FINDTEXT, (WPARAM)searchFlags, (LPARAM)ttf);
	}

	bool search(const std::string& what, int options)
	{

		Sci_TextToFind ft;
		Sci_CharacterRange chrg;

		if (nextSearchPos_ != 0)
			if ((searchOptions_ & FR_DOWN) != (options & FR_DOWN))
			{
				if ((options & FR_DOWN) == 0)
				{
					nextSearchPos_ -= 2;
				}
				else
				{
					nextSearchPos_ += 2;
				}
			}

		searchOptions_ = options;
		chrg.cpMin = nextSearchPos_;
		chrg.cpMax = (int)getLength();

		if (0 == (options & FR_DOWN))
		{
			int len = (int)getLength();

			if (nextSearchPos_ == 0)
				chrg.cpMin = len - nextSearchPos_;//
			chrg.cpMax = 0;
		}

		ft.lpstrText = (char*)(what.c_str());
		ft.chrg = chrg;

		int r = (int)findText(options, &ft);

		if (r == -1)
		{
			chrg.cpMin = 0;
			chrg.cpMax = 0;
			nextSearchPos_ = 0;
			//setSel(0,0);
			return false;
		}

		//chrg.cpMin = r;
		//chrg.cpMax = r + (long)what.size();

		if (0 == (options & FR_DOWN))
			nextSearchPos_ = r - 1;
		else
			nextSearchPos_ = ft.chrgText.cpMax + 1;

		setSel(r, ft.chrgText.cpMax);
		return true;
	}


	bool replace(const std::string& what, const std::string& whith, int options)
	{
		Sci_TextToFind ft;
		Sci_CharacterRange chrg;

		if (nextSearchPos_ != 0)
			if ((searchOptions_ & FR_DOWN) != (options & FR_DOWN))
			{
				if ((options & FR_DOWN) == 0)
				{
					// nextSearchPos_-=2;
				}
				else
				{
					// nextSearchPos_+=2;
				}
			}

		searchOptions_ = options;
		chrg.cpMin = nextSearchPos_;
		chrg.cpMax = (int)this->getLength();

		if (!(options & FR_DOWN))
		{
			int len = (int)this->getLength();

			if (nextSearchPos_ == 0)
				chrg.cpMin = len - nextSearchPos_;//
			chrg.cpMax = 0;
		}

		ft.lpstrText = (char*)(what.c_str());
		ft.chrg = chrg;

		int r = (int)this->findText(options, &ft);

		if (r == -1)
		{
			chrg.cpMin = 0;
			chrg.cpMax = 0;
			nextSearchPos_ = 0;
			setSel(0, 0);
			return false;
		}

		//chrg.cpMin = r;
		//chrg.cpMax = r + (long)what.size();

		if (!(options & FR_DOWN))
			nextSearchPos_ = r - 1;
		else
			nextSearchPos_ = r + (long)whith.size();//chrg.cpMax;//+1;

		setSel(chrg.cpMin, chrg.cpMax);
		replaceSel(whith);
		chrg.cpMax = chrg.cpMin + (long)whith.size();
		setSel(chrg.cpMin, chrg.cpMax);

		return true;
	}

	// doc

	struct SCIDOC {};

	class SciDoc
	{
	public:
		SciDoc()
		{}

		SciDoc(HWND hWnd)
			: hWnd_(hWnd)
		{}

		SciDoc(HWND hWnd, SCIDOC* doc)
			: hWnd_(hWnd),
			  sciDoc_(doc)
		{
			::SendMessage(hWnd_,SCI_ADDREFDOCUMENT, 0, (LPARAM)sciDoc_);
		}

		SciDoc(const SciDoc& rhs)
			: hWnd_(rhs.hWnd_), sciDoc_(rhs.sciDoc_)
		{
			if (sciDoc_)
			{
				::SendMessage(hWnd_, SCI_ADDREFDOCUMENT, 0, (LPARAM)sciDoc_);
			}
		}


		SciDoc(SciDoc&& rhs)
			: hWnd_(rhs.hWnd_), sciDoc_(rhs.sciDoc_)
		{
			rhs.hWnd_ = nullptr;
			rhs.sciDoc_ = nullptr;
		}

		SciDoc& operator=(const SciDoc& rhs)
		{
			if (this->sciDoc_ == rhs.sciDoc_)
			{
				return *this;
			}
			if (sciDoc_)
			{
				::SendMessage(hWnd_, SCI_RELEASEDOCUMENT, 0, (LPARAM)sciDoc_);
			}
			hWnd_ = rhs.hWnd_;
			sciDoc_ = rhs.sciDoc_;
			if (sciDoc_)
			{
				::SendMessage(hWnd_, SCI_ADDREFDOCUMENT, 0, (LPARAM)sciDoc_);
			}
			return *this;
		}

		SciDoc& operator=(SciDoc&& rhs)
		{
			if (this->sciDoc_ == rhs.sciDoc_)
			{
				return *this;
			}
			if (sciDoc_)
			{
				::SendMessage(hWnd_, SCI_RELEASEDOCUMENT, 0, (LPARAM)sciDoc_);
			}
			hWnd_ = rhs.hWnd_;
			sciDoc_ = rhs.sciDoc_;
			rhs.hWnd_ = nullptr;
			rhs.sciDoc_ = nullptr;
			return *this;
		}


		~SciDoc()
		{
			::SendMessage(hWnd_, SCI_RELEASEDOCUMENT, 0, (LPARAM)sciDoc_);
		}

		SCIDOC* operator*()
		{
			return sciDoc_;
		}

	private:
		HWND hWnd_ = nullptr;
		SCIDOC* sciDoc_ = nullptr;
	};

	SciDoc getDocument()
	{
		SCIDOC* doc = (SCIDOC*)sendMsg(SCI_GETDOCPOINTER, 0, 0);
		return SciDoc(handle, doc);
	}

	void setDocument(SCIDOC* doc)
	{
		sendMsg(SCI_SETDOCPOINTER, 0, (WPARAM) doc);
	}

	SciDoc createDocument()
	{
		SCIDOC* doc = (SCIDOC*)sendMsg(SCI_CREATEDOCUMENT, 0, 0);
		SciDoc result(handle, doc);
		::SendMessage(handle, SCI_RELEASEDOCUMENT, 0, (LPARAM)*result);
		return result;
	}


	// zoom

	LRESULT zoom_in()
	{
		return sendMsg(SCI_ZOOMIN, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT zoom_out()
	{
		return sendMsg(SCI_ZOOMOUT, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT setZoom(int zoom)
	{
		return sendMsg(SCI_SETZOOM, (WPARAM)zoom, (LPARAM)(0));
	}

	LRESULT getZoom()
	{
		return sendMsg(SCI_GETZOOM, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT setMarginType(int margin, int type)
	{
		return sendMsg(SCI_SETMARGINTYPEN, (WPARAM)margin, (LPARAM)(type));
	}

	LRESULT getMarginType(int margin)
	{
		return sendMsg(SCI_GETMARGINTYPEN, (WPARAM)margin, (LPARAM)(0));
	}

	LRESULT setMarginWidth(int margin, int w)
	{
		return sendMsg(SCI_SETMARGINWIDTHN, (WPARAM)margin, (LPARAM)(w));
	}

	LRESULT getMarginWidth(int margin)
	{
		return sendMsg(SCI_SETMARGINWIDTHN, (WPARAM)margin, (LPARAM)(0));
	}

	LRESULT setMarginStyle(int margin)
	{
		sendMsg(SCI_SETMARGINTYPEN, margin, SC_MARGIN_FORE);
		return sendMsg(SCI_SETMARGINTYPEN, margin, SC_MARGIN_BACK);
	}

	LRESULT setOvertype(bool b)
	{
		return sendMsg(SCI_SETOVERTYPE, (WPARAM)b, (LPARAM)(0));
	}

	LRESULT getOvertype()
	{
		return sendMsg(SCI_GETOVERTYPE, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT lineScroll(int col, int line)
	{
		return sendMsg(SCI_LINESCROLL, (WPARAM)col, (LPARAM)(line));
	}

	LRESULT scrollCaret()
	{
		return sendMsg(SCI_SCROLLCARET, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT getCaretWidth()
	{
		return sendMsg(SCI_SETCARETWIDTH, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT setCaretWidth(int w)
	{
		return sendMsg(SCI_GETCARETWIDTH, (WPARAM)w, (LPARAM)(0));
	}

	LRESULT getFocus()
	{
		return sendMsg(SCI_GETFOCUS, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT getScrollWidth()
	{
		return sendMsg(SCI_GETSCROLLWIDTH, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT setScrollWidth(int w)
	{
		return sendMsg(SCI_SETSCROLLWIDTH, (WPARAM)w, (LPARAM)(0));
	}

	LRESULT setUsePopUp(bool b)
	{
		return sendMsg(SCI_USEPOPUP, (WPARAM)b, (LPARAM)(0));
	}

	LRESULT setIndent(int w)
	{
		return postMsg(SCI_SETINDENT, (WPARAM)w, (LPARAM)(0));
	}

	LRESULT getIndent()
	{
		return sendMsg(SCI_GETINDENT, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT setLineIndent(int l, int w)
	{
		return sendMsg(SCI_SETLINEINDENTATION, (WPARAM)l, (LPARAM)(w));
	}

	LRESULT getLineIndent(int l)
	{
		return sendMsg(SCI_GETLINEINDENTATION, (WPARAM)l, (LPARAM)(0));
	}

	LRESULT setTabIndents(bool b)
	{
		return sendMsg(SCI_SETTABINDENTS, (WPARAM)b, (LPARAM)(0));
	}

	LRESULT getTabIndents()
	{
		return sendMsg(SCI_GETTABINDENTS, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT setBackSpaceUnindents(bool b)
	{
		return sendMsg(SCI_SETBACKSPACEUNINDENTS, (WPARAM)b, (LPARAM)(0));
	}

	LRESULT getBackSpaceUnindents()
	{
		return sendMsg(SCI_GETBACKSPACEUNINDENTS, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT setViewEol(bool b)
	{
		return sendMsg(SCI_SETVIEWEOL, (WPARAM)b, (LPARAM)(0));
	}

	LRESULT getViewEol()
	{
		return sendMsg(SCI_GETVIEWEOL, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT setEolMode(int mode)
	{
		return sendMsg(SCI_SETEOLMODE, (WPARAM)mode, (LPARAM)(0));
	}

	LRESULT getEolMode()
	{
		return sendMsg(SCI_GETEOLMODE, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT convertEol(int mode)
	{
		return sendMsg(SCI_CONVERTEOLS, (WPARAM)mode, (LPARAM)(0));
	}

	LRESULT braceHighlight(int pos1, int pos2)
	{
		return sendMsg(SCI_BRACEHIGHLIGHT, (WPARAM)pos1, (LPARAM)(pos2));
	}

	LRESULT braceMatch(int pos)
	{
		return sendMsg(SCI_BRACEMATCH, (WPARAM)pos, (LPARAM)(0));
	}

	// Annotations
	
	void setAnnotation(int i, const std::wstring& str)
	{
		std::string tmp = to_string(str);
		sendMsg(SCI_ANNOTATIONSETTEXT, (WPARAM)i, (LPARAM)(tmp.c_str()));
	}

	void setAnnotationStyle(int i, int style)
	{
		sendMsg(SCI_ANNOTATIONSETSTYLE, (WPARAM)i, (LPARAM)(style));
	}

	void clearAnnotation(int i)
	{
		sendMsg(SCI_ANNOTATIONSETTEXT, (WPARAM)i, (LPARAM)(0));
	}

	void clearAnnotations()
	{
		sendMsg(SCI_ANNOTATIONCLEARALL, (WPARAM)0, (LPARAM)(0));
	}

	void showAnnotations(int style)
	{
		sendMsg(SCI_ANNOTATIONSETVISIBLE, (WPARAM)style, (LPARAM)(0));
	}

	void clearAllMarkers()
	{
		sendMsg(SCI_MARKERDELETEALL, (WPARAM)-1, (LPARAM)(0));
	}

	void setMarker(int line, int num = 2)
	{
		LRESULT markerhandle = sendMsg(SCI_MARKERADD, (WPARAM)line, (LPARAM)(num));
		markers_.insert(markerhandle);
	}

	void removeMarker(int line, int num = 2)
	{
		for (std::set<LRESULT>::iterator it = markers_.begin(); it != markers_.end(); it++)
		{
			LRESULT markerhandler = (*it);
			LRESULT l = sendMsg(SCI_MARKERLINEFROMHANDLE, (WPARAM)markerhandler, (LPARAM)(0));
			if (l == line)
			{
				markers_.erase(markerhandler);
				sendMsg(SCI_MARKERDELETEHANDLE, (WPARAM)markerhandler, (LPARAM)(0));
				return;
			}
		}
	}

	std::set<int> getMarkers()
	{
		std::set<int> lines;
		for (std::set<LRESULT>::iterator it = markers_.begin(); it != markers_.end(); it++)
		{
			LRESULT markerhandler = (*it);
			int line = (int)sendMsg(SCI_MARKERLINEFROMHANDLE, (WPARAM)markerhandler, (LPARAM)(0));
			int pos = (int)posFromLine(line);
			lines.insert(pos);
		}
		return lines;
	}

	bool hasMarker(int line, int mask = 0x04)
	{
		LRESULT val = sendMsg(SCI_MARKERGET, (WPARAM)(line), (LPARAM)(0));
		if (mask & val)
		{
			return true;
		}
		return false;
	}

	bool toggleMarker(int line)
	{
		bool b = hasMarker(line);
		if (!b)
		{
			setMarker(line);
			return b;
		}
		removeMarker(line);
		return !b;
	}

	void useMarkers(bool b)
	{
		if (b)
		{

			LRESULT fore = sendMsg(SCI_STYLEGETFORE, (WPARAM)STYLE_DEFAULT, (LPARAM)0);
			LRESULT back = sendMsg(SCI_STYLEGETBACK, (WPARAM)STYLE_DEFAULT, (LPARAM)0);

			sendMsg(SCI_SETMARGINWIDTHN, (WPARAM)(2), (LPARAM)(16));
			sendMsg(SCI_SETMARGINSENSITIVEN, (WPARAM)(2), (LPARAM)(true));
			sendMsg(SCI_MARKERDEFINE, (WPARAM)(2), (LPARAM)(SC_MARK_CIRCLE));

			sendMsg(SCI_SETMARGINMASKN, (WPARAM)(1), (LPARAM)(0));
			sendMsg(SCI_SETMARGINMASKN, (WPARAM)(2), (LPARAM)(~SC_MASK_FOLDERS));
			//sendMessage( SCI_MARKERSETBACK, (WPARAM)(2), (LPARAM)(back) );
			//sendMessage(SCI_MARKERSETFORE, (WPARAM)(2), (LPARAM)(fore));
			//sendMessage(SCI_SETFOLDMARGINCOLOUR, (WPARAM)1, (LPARAM)(fore));
			//sendMessage(SCI_SETFOLDMARGINHICOLOUR, (WPARAM)1, (LPARAM)(back));

		//	sendMessage(SCI_SETMARGINTYPEN, (WPARAM)(2) ,(LPARAM)(SC_MARGIN_BACK));
		//	sendMessage(SCI_SETMARGINTYPEN, (WPARAM)(2), (LPARAM)(SC_MARGIN_FORE));
			setCaretForeCol((int)fore);
			return;
		}
		sendMsg(SCI_SETMARGINWIDTHN, (WPARAM)(2), (LPARAM)(0));
		sendMsg(SCI_SETMARGINSENSITIVEN, (WPARAM)(2), (LPARAM)(false));
	}

	void highliteLine(int line)
	{
		if (highlight_ != 0)
		{
			sendMsg(SCI_MARKERDELETEHANDLE, (WPARAM)highlight_, (LPARAM)(0));
			highlight_ = 0;
		}

		if (line == -1)
		{
			return;
		}

		sendMsg(SCI_MARKERDEFINE, (WPARAM)(1), (LPARAM)(SC_MARK_BACKGROUND));
		sendMsg(SCI_MARKERSETBACK, (WPARAM)(1), (LPARAM)(RGB(255, 245, 199)));
		highlight_ = sendMsg(SCI_MARKERADD, (WPARAM)line, (LPARAM)(1));
	}


	// Lexer and Styles

	void setLexer(int lex)
	{
		sendMsg(SCI_SETLEXER, (WPARAM)lex, (LPARAM)0);
	}

	LRESULT getLexer()
	{
		return sendMsg(SCI_GETLEXER, (WPARAM)0, (LPARAM)0);
	}

	void setMode(int lex)
	{
		setLexer(SCLEX_CPP);

		for (auto& styleSet : xmlStyleSets.styleSet)
		{
			if (styleSet.id == lex)
			{
				size_t c = styleSet.keywords.size();
				for (int i = 0; i < c; i++)
				{
					setKeywords(i, styleSet.keywords[i]);
				}
				break;
			}
		}
		
		colorize();
	}

	void colorize(int start = 0, int end = -1)
	{
		sendMsg(SCI_COLOURISE, (WPARAM)start, (LPARAM)end);
	}

	void setStyle(int style, COLORREF fore, COLORREF back = 0, int size = 0, const char* font = 0)
	{
		styleSetFore(style, fore);
		if (back)
			styleSetBack(style, back);
		if (size >= 1)
			styleSetSize(style, size);
		if (font)
			styleSetFont(style, font);
	}

	LRESULT setCaretForeCol(int col)
	{
		return sendMsg(SCI_SETCARETFORE, (WPARAM)col, (LPARAM)col);
	}


	LRESULT styleSetFont(int style, const std::string& font)
	{
		return sendMsg(SCI_STYLESETFONT, (WPARAM)style, (LPARAM)(font.c_str()));
	}

	LRESULT styleSetSize(int style, int size)
	{
		return sendMsg(SCI_STYLESETSIZE, (WPARAM)style, (LPARAM)size);
	}

	LRESULT styleSetFore(int style, int col)
	{
		if (style == STYLE_LINENUMBER)
		{
			setCaretForeCol(col);
			sendMsg(SCI_SETSELBACK, (WPARAM)(1), (LPARAM)(col));
		}
		return sendMsg(SCI_STYLESETFORE, (WPARAM)style, (LPARAM)col);
	}

	LRESULT styleSetBack(int style, int col)
	{
		if (style == STYLE_LINENUMBER)
		{
			sendMsg(SCI_SETSELFORE, (WPARAM)(1), (LPARAM)(col));
		}
		return sendMsg(SCI_STYLESETBACK, (WPARAM)style, (LPARAM)col);
	}

	LRESULT setKeywords(int keySet, const std::string& keyList)
	{
		return sendMsg(SCI_SETKEYWORDS, (WPARAM)keySet, (LPARAM)(keyList.c_str()));
	}

	LRESULT styleSetBold(int style, bool bold)
	{
		return sendMsg(SCI_STYLESETBOLD, (WPARAM)style, (LPARAM)bold);
	}

	LRESULT styleSetItalic(int style, bool bold)
	{
		return sendMsg(SCI_STYLESETITALIC, (WPARAM)style, (LPARAM)bold);
	}

	LRESULT styleSetEolFilled(int style, bool filled)
	{
		return sendMsg(SCI_STYLESETEOLFILLED, (WPARAM)style, (LPARAM)filled);
	}

	void load_xml(const std::wstring& path)
	{
		std::ifstream fs;
		fs.open(path, std::ios::binary | std::ios::in);
		std::ostringstream oss;
		while (fs)
		{
			char buf[1024];
			fs.read(buf, 1024);
			size_t c = fs.gcount();
			oss.write(buf, c);
		}
		fs.close();

		std::string xml = oss.str();
		
		fromXml(xml, xmlStyleSets);
		//for (size_t i = 0; i < 1; i++)//xmlStyleSets.styleSet.size(); i++)
		size_t i = 8;
		{
			XmlStyleSet& xmlStyleSet = xmlStyleSets.styleSet[i];

			int styleSetId = xmlStyleSet.id;

			for (size_t j = 0; j < xmlStyleSet.style.size(); j++)
			
			{
				XmlStyle& xmlStyle = xmlStyleSet.style[j];

				COLORREF foreCol = hex2rgb(xmlStyle.foreColor);
				COLORREF backCol = hex2rgb(xmlStyle.backgroundColor);

				setStyle(xmlStyle.id, foreCol, backCol, xmlStyle.fontSize, xmlStyle.font.c_str());
					//	setKeywords((xmlStyleSet.id, xmlStyleSet.keywords);

				if (xmlStyle.bold == "true")
				{
					styleSetBold(xmlStyle.id, true);
				}
				if (xmlStyle.italic == "true")
				{
					styleSetItalic(xmlStyle.id, true);
				}
				if (xmlStyle.eol == "true")
				{
					styleSetEolFilled(xmlStyle.id, true);
				}
			}
		}

	}


	int searchPos()
	{
		return nextSearchPos_;
	}

	bool searchPos(unsigned int pos)
	{
		if (pos <= (unsigned int)this->getLength())
		{
			nextSearchPos_ = pos;
			return true;
		}
		return false;
	}

	int searchOptions()
	{
		return searchOptions_;
	}

	bool searchOptions(int options)
	{
		searchOptions_ = options;
		return true;
	}

	protected:

	int						nextSearchPos_ = 0;
	int						searchOptions_ = FR_DOWN;
	LRESULT					highlight_ = 0;
	std::set<LRESULT>		markers_;

};


class SciLexer
{
public:
	SciLexer()
	{
		::LoadLibrary(L"SciLexer.dll");
	}
};

}
