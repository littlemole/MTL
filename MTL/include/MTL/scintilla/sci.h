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
#include "MTL/win/ctrl.h"
#include "MTL/win/codlg.h"
//#include "MTL/win/dlg.h"
#include "MTL/util/path.h"
//#include "MTL/obj/impl.h"
//#include "MTL/ole/shell.h"
//#include "MTL/disp/variant.h"
#include "MTL/util/rgb.h"
#include "mtl/persist/xml.h"
#include "scintilla//Scintilla.h"
#include "scintilla//ILexer.h"
#include "scintilla//SciLexer.h"

#include <fstream>

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

	struct XmlStyleSet
	{
		int id;
		std::string syntax;
		std::vector<std::string> keywords;
		std::vector<XmlStyle> style;
	};

	struct XmlStyleSets
	{
		std::vector<XmlStyleSet> styleSet;
	};

}

template<>
struct meta::Data<mtl::XmlStyle>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("style"),
			member("id", &mtl::XmlStyle::id, meta::attribute()),
			member("desc", &mtl::XmlStyle::desc, meta::attribute()),
			member("background-color", &mtl::XmlStyle::backgroundColor, meta::attribute()),
			member("foreground-color", &mtl::XmlStyle::foreColor, meta::attribute()),
			member("bold", &mtl::XmlStyle::bold, meta::attribute()),
			member("eol", &mtl::XmlStyle::eol, meta::attribute()),
			member("italic", &mtl::XmlStyle::italic, meta::attribute()),
			member("font-size", &mtl::XmlStyle::fontSize, meta::attribute()),
			member("font", &mtl::XmlStyle::font, meta::attribute())
		);
	}
};




template<>
struct meta::Data<mtl::XmlStyleSet>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("styleset"),
			member("id", &mtl::XmlStyleSet::id, meta::attribute()),
			member("syntax", &mtl::XmlStyleSet::syntax, meta::attribute()),
			member("keywords", &mtl::XmlStyleSet::keywords),
			member("style", &mtl::XmlStyleSet::style)
		);
	}
};


template<>
struct meta::Data<mtl::XmlStyleSets>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("stylesets"),
			member("styleset", &mtl::XmlStyleSets::styleSet)
		);
	}
};

namespace mtl {

class scintilla_wnd;

template<>
class window_class<scintilla_wnd>
{
public:
	const wchar_t* name()
	{
		return L"Scintilla";
	}
};

class scintilla_wnd : public ctrl<scintilla_wnd>
{
public:

	XmlStyleSets xmlStyleSets;

	scintilla_wnd()
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
			case WM_PRINTCLIENT:
			{
				HDC hdc = (HDC)wParam;

				return 1;
				break;
			}

		}
		return ctrl<scintilla_wnd>::wndProc(hwnd, message, wParam, lParam);
	}

	void notifications(int mask = SC_MODEVENTMASKALL)
	{
		send_msg(SCI_SETMODEVENTMASK, (WPARAM)mask, 0);
	}

	scintilla_wnd& set_code_page(int cp)
	{
		send_msg(SCI_SETCODEPAGE, (WPARAM)cp, (LPARAM)0);
		return *this;
	}

	int get_length()
	{
		return (int) send_msg(SCI_GETLENGTH, (WPARAM)0, (LPARAM)0);
	}

	char get_char_at(int pos)
	{
		return (char) send_msg(SCI_GETCHARAT, (WPARAM)pos, (LPARAM)0);
	}

	int get_line_length(int line)
	{
		return (int)send_msg(SCI_LINELENGTH, (WPARAM)line, (LPARAM)0);
	}

	int get_line_count()
	{
		return (int)send_msg(SCI_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
	}

	int get_first_visible_line()
	{
		return (int)send_msg(SCI_GETFIRSTVISIBLELINE, (WPARAM)0, (LPARAM)0);
	}

	int get_lines_on_screen()
	{
		return (int)send_msg(SCI_LINESONSCREEN, (WPARAM)0, (LPARAM)0);
	}

	scintilla_wnd& goto_line(int line)
	{
		send_msg(SCI_GOTOLINE, (WPARAM)line, (LPARAM)0);
		return *this;
	}

	int line_from_pos(int pos)
	{
		return (int)send_msg(SCI_LINEFROMPOSITION, (WPARAM)pos, (LPARAM)0);
	}

	int line_end_pos(int line)
	{
		return (int)send_msg(SCI_GETLINEENDPOSITION, (WPARAM)line, (LPARAM)0);
	}

	int pos_from_line(int line)
	{
		return (int)send_msg(SCI_POSITIONFROMLINE, (WPARAM)line, (LPARAM)0);
	}

	int pos()
	{
		return (int)send_msg(SCI_GETCURRENTPOS, (WPARAM)0, (LPARAM)0);
	}

	scintilla_wnd& pos(int pos)
	{
		send_msg(SCI_SETCURRENTPOS, (WPARAM)pos, (LPARAM)0);
		return *this;
	}

	int anchor()
	{
		return (int)send_msg(SCI_GETANCHOR, (WPARAM)0, (LPARAM)0);
	}

	scintilla_wnd& anchor(int pos)
	{
		send_msg(SCI_SETANCHOR, (WPARAM)pos, (LPARAM)0);
		return *this;
	}

	scintilla_wnd& insert_text(const std::string& txt, int p = -1)
	{
		if (p == -1)
			p = (int)pos();
		int pos = p + (int)txt.size();
		send_msg(SCI_INSERTTEXT, (WPARAM)p, (LPARAM)txt.c_str());
		this->set_selection(pos, pos);
		return *this;
	}

	scintilla_wnd& replace_selection(const std::string& txt)
	{
		send_msg(SCI_REPLACESEL, (WPARAM)0, (LPARAM)(txt.c_str()));
		return *this;
	}

	scintilla_wnd& set_selection(int anchorPos, int currentPos)
	{
		send_msg(SCI_SETSEL, (WPARAM)anchorPos, (LPARAM)currentPos);
		return *this;
	}

	int get_selection_start()
	{
		return (int)send_msg(SCI_GETSELECTIONSTART, (WPARAM)0, (LPARAM)0);
	}

	scintilla_wnd& set_selection_start(int pos)
	{
		send_msg(SCI_SETSELECTIONSTART, (WPARAM)pos, (LPARAM)0);
		return *this;
	}

	scintilla_wnd& set_selection_end(int pos)
	{
		send_msg(SCI_SETSELECTIONEND, (WPARAM)pos, (LPARAM)0);
		return *this;
	}

	int get_selection_end()
	{
		return (int) send_msg(SCI_GETSELECTIONEND, (WPARAM)0, (LPARAM)0);
	}

	bool get_modified()
	{
		return (bool)send_msg(SCI_GETMODIFY, (WPARAM)0, (LPARAM)0);
	}

	scintilla_wnd& set_read_only(bool b)
	{
		send_msg(SCI_SETREADONLY, (WPARAM)b, (LPARAM)0);
		return *this;
	}

	bool get_read_only(bool b)
	{
		return (bool)send_msg(SCI_GETREADONLY, (WPARAM)0, (LPARAM)0);
	}


	scintilla_wnd& set_text(const std::string& txt)
	{
		send_msg(SCI_SETTEXT, (WPARAM)0, (LPARAM)(txt.c_str()));
		return *this;
	}

	std::string get_text()
	{
		size_t len = length() +1 ;
		cbuff buf(len);
		LRESULT lr = send_msg(SCI_GETTEXT, (WPARAM)len, (LPARAM)(char*)buf);
		return buf.toString();
	}

	size_t length()
	{
		return (size_t)send_msg(SCI_GETLENGTH, 0, 0);
	}

	scintilla_wnd& set_drop_target(IDropTarget* dropTarget)
	{
		::RevokeDragDrop(handle);
		::RegisterDragDrop(handle, dropTarget);
		return *this;
	}

	scintilla_wnd& set_save_point()
	{
		send_msg(SCI_SETSAVEPOINT, (WPARAM)0, (LPARAM)0);
		return *this;
	}

	scintilla_wnd& cut()
	{
		send_msg(SCI_CUT, (WPARAM)0, (LPARAM)0);
		return *this;
	}

	scintilla_wnd& copy()
	{
		send_msg(SCI_COPY, (WPARAM)0, (LPARAM)0);
		return *this;
	}

	scintilla_wnd& paste()
	{
		send_msg(SCI_PASTE, (WPARAM)0, (LPARAM)0);
		return *this;
	}

	scintilla_wnd& undo()
	{
		send_msg(SCI_UNDO, (WPARAM)0, (LPARAM)0);
		return *this;
	}

	scintilla_wnd& redo()
	{
		send_msg(SCI_REDO, (WPARAM)0, (LPARAM)0);
		return *this;
	}

	int get_tab_width()
	{
		return (int)send_msg(SCI_GETTABWIDTH, (WPARAM)0, (LPARAM)0);
	}

	scintilla_wnd& set_tab_width(int w)
	{
		send_msg(SCI_SETTABWIDTH, (WPARAM)w, (LPARAM)0);
		return *this;
	}

	scintilla_wnd& set_use_tabs(bool b)
	{
		send_msg(SCI_SETUSETABS, (WPARAM)b, (LPARAM)0);
		return *this;
	}

	bool get_use_tabs()
	{
		return (bool)send_msg(SCI_GETUSETABS, (WPARAM)0, (LPARAM)0);
	}


	scintilla_wnd& print_page(bool bDraw, Sci_RangeToFormat* pfr)
	{
		send_msg(SCI_FORMATRANGE, (WPARAM)bDraw, (LPARAM)pfr);
		return *this;
	}

	scintilla_wnd& set_next_search_pos(int i)
	{
		nextSearchPos_ = i;
		return *this;
	}

	LRESULT find_text(int searchFlags, Sci_TextToFind* ttf)
	{
		return send_msg(SCI_FINDTEXT, (WPARAM)searchFlags, (LPARAM)ttf);
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
		chrg.cpMax = (int)get_length();

		if (0 == (options & FR_DOWN))
		{
			int len = (int)get_length();

			if (nextSearchPos_ == 0)
				chrg.cpMin = len - nextSearchPos_;//
			chrg.cpMax = 0;
		}

		ft.lpstrText = (char*)(what.c_str());
		ft.chrg = chrg;

		int r = (int)find_text(options, &ft);

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

		set_selection(r, ft.chrgText.cpMax);
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
					nextSearchPos_-=2;
				}
				else
				{
					nextSearchPos_+=2;
				}
			}

		searchOptions_ = options;
		chrg.cpMin = nextSearchPos_;
		chrg.cpMax = (int)this->get_length();

		if (!(options & FR_DOWN))
		{
			int len = (int)this->get_length();

			if (nextSearchPos_ == 0)
				chrg.cpMin = len - nextSearchPos_;//
			chrg.cpMax = 0;
		}

		ft.lpstrText = (char*)(what.c_str());
		ft.chrg = chrg;

		int r = (int)this->find_text(options, &ft);

		if (r == -1)
		{
			chrg.cpMin = 0;
			chrg.cpMax = 0;
			nextSearchPos_ = 0;
			set_selection(0, 0);
			return false;
		}

		chrg = ft.chrgText;

		if (!(options & FR_DOWN))
			nextSearchPos_ = r - 1;
		else
			nextSearchPos_ = r + (long)whith.size();//chrg.cpMax;//+1;

		set_selection(chrg.cpMin, chrg.cpMax);
		replace_selection(whith);
		chrg.cpMax = chrg.cpMin + (long)whith.size();
		set_selection(chrg.cpMin, chrg.cpMax);

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

	SciDoc get_document()
	{
		SCIDOC* doc = (SCIDOC*)send_msg(SCI_GETDOCPOINTER, 0, 0);
		return SciDoc(handle, doc);
	}

	void set_document(SCIDOC* doc)
	{
		send_msg(SCI_SETDOCPOINTER, 0, (WPARAM) doc);
	}

	SciDoc create_document()
	{
		SCIDOC* doc = (SCIDOC*)send_msg(SCI_CREATEDOCUMENT, 0, 0);
		SciDoc result(handle, doc);
		::SendMessage(handle, SCI_RELEASEDOCUMENT, 0, (LPARAM)*result);
		return result;
	}


	// zoom

	scintilla_wnd& zoom_in()
	{
		send_msg(SCI_ZOOMIN, (WPARAM)0, (LPARAM)(0));
		return *this;
	}

	scintilla_wnd& zoom_out()
	{
		send_msg(SCI_ZOOMOUT, (WPARAM)0, (LPARAM)(0));
		return *this;
	}

	scintilla_wnd& set_zoom(int zoom)
	{
		send_msg(SCI_SETZOOM, (WPARAM)zoom, (LPARAM)(0));
		return *this;
	}

	int get_zoom()
	{
		return (int)send_msg(SCI_GETZOOM, (WPARAM)0, (LPARAM)(0));
	}

	scintilla_wnd& set_margin_type(int margin, int type)
	{
		send_msg(SCI_SETMARGINTYPEN, (WPARAM)margin, (LPARAM)(type));
		return *this;
	}

	int get_margin_type(int margin)
	{
		return (int)send_msg(SCI_GETMARGINTYPEN, (WPARAM)margin, (LPARAM)(0));
	}

	scintilla_wnd& set_margin_width(int margin, int w)
	{
		send_msg(SCI_SETMARGINWIDTHN, (WPARAM)margin, (LPARAM)(w));
		return *this;
	}

	int get_margin_width(int margin)
	{
		return (int)send_msg(SCI_SETMARGINWIDTHN, (WPARAM)margin, (LPARAM)(0));
	}

	scintilla_wnd& set_margin_style(int margin)
	{
		send_msg(SCI_SETMARGINTYPEN, margin, SC_MARGIN_FORE);
		send_msg(SCI_SETMARGINTYPEN, margin, SC_MARGIN_BACK);
		return *this;
	}

	scintilla_wnd& set_overtype(bool b)
	{
		send_msg(SCI_SETOVERTYPE, (WPARAM)b, (LPARAM)(0));
		return *this;
	}

	bool get_overtype()
	{
		return (bool)send_msg(SCI_GETOVERTYPE, (WPARAM)0, (LPARAM)(0));
	}

	LRESULT line_scroll(int col, int line)
	{
		return send_msg(SCI_LINESCROLL, (WPARAM)col, (LPARAM)(line));
	}

	LRESULT scroll_caret()
	{
		return send_msg(SCI_SCROLLCARET, (WPARAM)0, (LPARAM)(0));
	}

	int get_caret_width()
	{
		return (int) send_msg(SCI_SETCARETWIDTH, (WPARAM)0, (LPARAM)(0));
	}

	scintilla_wnd& set_caret_width(int w)
	{
		send_msg(SCI_GETCARETWIDTH, (WPARAM)w, (LPARAM)(0));
		return *this;
	}

	LRESULT get_focus()
	{
		return send_msg(SCI_GETFOCUS, (WPARAM)0, (LPARAM)(0));
	}

	int get_scroll_width()
	{
		return (int)send_msg(SCI_GETSCROLLWIDTH, (WPARAM)0, (LPARAM)(0));
	}

	scintilla_wnd& set_scroll_width(int w)
	{
		send_msg(SCI_SETSCROLLWIDTH, (WPARAM)w, (LPARAM)(0));
		return *this;
	}

	scintilla_wnd& set_use_popup(bool b)
	{
		send_msg(SCI_USEPOPUP, (WPARAM)b, (LPARAM)(0));
		return *this;
	}

	scintilla_wnd& set_indent(int w)
	{
		post_msg(SCI_SETINDENT, (WPARAM)w, (LPARAM)(0));
		return *this;
	}

	int get_indent()
	{
		return (int)send_msg(SCI_GETINDENT, (WPARAM)0, (LPARAM)(0));
	}

	scintilla_wnd& set_line_indent(int l, int w)
	{
		send_msg(SCI_SETLINEINDENTATION, (WPARAM)l, (LPARAM)(w));
		return *this;
	}

	int get_line_indent(int l)
	{
		return (int)send_msg(SCI_GETLINEINDENTATION, (WPARAM)l, (LPARAM)(0));
	}

	scintilla_wnd& set_tab_indents(bool b)
	{
		send_msg(SCI_SETTABINDENTS, (WPARAM)b, (LPARAM)(0));
		return *this;
	}

	bool get_tab_indents()
	{
		return (bool)send_msg(SCI_GETTABINDENTS, (WPARAM)0, (LPARAM)(0));
	}

	scintilla_wnd& set_backspace_unindents(bool b)
	{
		send_msg(SCI_SETBACKSPACEUNINDENTS, (WPARAM)b, (LPARAM)(0));
		return *this;
	}

	bool get_backspace_unindents()
	{
		return (bool) send_msg(SCI_GETBACKSPACEUNINDENTS, (WPARAM)0, (LPARAM)(0));
	}

	scintilla_wnd& set_view_eol(bool b)
	{
		send_msg(SCI_SETVIEWEOL, (WPARAM)b, (LPARAM)(0));
		return *this;
	}

	bool get_view_eol()
	{
		return (bool)send_msg(SCI_GETVIEWEOL, (WPARAM)0, (LPARAM)(0));
	}

	scintilla_wnd& set_eol_mode(int mode)
	{
		send_msg(SCI_SETEOLMODE, (WPARAM)mode, (LPARAM)(0));
		return *this;
	}

	int get_eol_mode()
	{
		return (int) send_msg(SCI_GETEOLMODE, (WPARAM)0, (LPARAM)(0));
	}

	scintilla_wnd& convert_eol(int mode)
	{
		send_msg(SCI_CONVERTEOLS, (WPARAM)mode, (LPARAM)(0));
		return *this;
	}

	scintilla_wnd& brace_highlight(int pos1, int pos2)
	{
		send_msg(SCI_BRACEHIGHLIGHT, (WPARAM)pos1, (LPARAM)(pos2));
		return *this;
	}

	LRESULT brace_match(int pos)
	{
		return send_msg(SCI_BRACEMATCH, (WPARAM)pos, (LPARAM)(0));
	}

	// Annotations
	
	scintilla_wnd& set_annotation(int i, const std::wstring& str)
	{
		std::string tmp = to_string(str);
		send_msg(SCI_ANNOTATIONSETTEXT, (WPARAM)i, (LPARAM)(tmp.c_str()));
		return *this;
	}

	scintilla_wnd& set_annotation_style(int i, int style)
	{
		send_msg(SCI_ANNOTATIONSETSTYLE, (WPARAM)i, (LPARAM)(style));
		return *this;
	}

	scintilla_wnd& clear_annotation(int i)
	{
		send_msg(SCI_ANNOTATIONSETTEXT, (WPARAM)i, (LPARAM)(0));
		return *this;
	}

	scintilla_wnd& clear_annotations()
	{
		send_msg(SCI_ANNOTATIONCLEARALL, (WPARAM)0, (LPARAM)(0));
		return *this;
	}

	scintilla_wnd& show_annotations(int style)
	{
		send_msg(SCI_ANNOTATIONSETVISIBLE, (WPARAM)style, (LPARAM)(0));
		return *this;
	}

	scintilla_wnd& clear_all_markers()
	{
		send_msg(SCI_MARKERDELETEALL, (WPARAM)-1, (LPARAM)(0));
		return *this;
	}

	scintilla_wnd& set_marker(int line, int num = 2)
	{
		LRESULT markerhandle = send_msg(SCI_MARKERADD, (WPARAM)line, (LPARAM)(num));
		markers_.insert(markerhandle);
		return *this;
	}

	scintilla_wnd& remove_marker(int line, int num = 2)
	{
		for (std::set<LRESULT>::iterator it = markers_.begin(); it != markers_.end(); it++)
		{
			LRESULT markerhandler = (*it);
			LRESULT l = send_msg(SCI_MARKERLINEFROMHANDLE, (WPARAM)markerhandler, (LPARAM)(0));
			if (l == line)
			{
				markers_.erase(markerhandler);
				send_msg(SCI_MARKERDELETEHANDLE, (WPARAM)markerhandler, (LPARAM)(0));
				return *this;
			}
		}
		return *this;
	}

	std::set<int> get_markers()
	{
		std::set<int> lines;
		for (std::set<LRESULT>::iterator it = markers_.begin(); it != markers_.end(); it++)
		{
			LRESULT markerhandler = (*it);
			int line = (int)send_msg(SCI_MARKERLINEFROMHANDLE, (WPARAM)markerhandler, (LPARAM)(0));
			int pos = (int)pos_from_line(line);
			lines.insert(pos);
		}
		return lines;
	}

	bool has_marker(int line, int mask = 0x04)
	{
		LRESULT val = send_msg(SCI_MARKERGET, (WPARAM)(line), (LPARAM)(0));
		if (mask & val)
		{
			return true;
		}
		return false;
	}

	bool toggle_marker(int line)
	{
		bool b = has_marker(line);
		if (!b)
		{
			set_marker(line);
			return b;
		}
		remove_marker(line);
		return !b;
	}

	scintilla_wnd& use_markers(bool b)
	{
		if (b)
		{

			LRESULT fore = send_msg(SCI_STYLEGETFORE, (WPARAM)STYLE_DEFAULT, (LPARAM)0);
			LRESULT back = send_msg(SCI_STYLEGETBACK, (WPARAM)STYLE_DEFAULT, (LPARAM)0);

			send_msg(SCI_SETMARGINWIDTHN, (WPARAM)(2), (LPARAM)(16));
			send_msg(SCI_SETMARGINSENSITIVEN, (WPARAM)(2), (LPARAM)(true));
			send_msg(SCI_MARKERDEFINE, (WPARAM)(2), (LPARAM)(SC_MARK_CIRCLE));

			send_msg(SCI_SETMARGINMASKN, (WPARAM)(1), (LPARAM)(0));
			send_msg(SCI_SETMARGINMASKN, (WPARAM)(2), (LPARAM)(~SC_MASK_FOLDERS));
			//sendMessage( SCI_MARKERSETBACK, (WPARAM)(2), (LPARAM)(back) );
			//sendMessage(SCI_MARKERSETFORE, (WPARAM)(2), (LPARAM)(fore));
			//sendMessage(SCI_SETFOLDMARGINCOLOUR, (WPARAM)1, (LPARAM)(fore));
			//sendMessage(SCI_SETFOLDMARGINHICOLOUR, (WPARAM)1, (LPARAM)(back));

		//	sendMessage(SCI_SETMARGINTYPEN, (WPARAM)(2) ,(LPARAM)(SC_MARGIN_BACK));
		//	sendMessage(SCI_SETMARGINTYPEN, (WPARAM)(2), (LPARAM)(SC_MARGIN_FORE));
			set_caret_forecol((int)fore);
			return *this;
		}
		send_msg(SCI_SETMARGINWIDTHN, (WPARAM)(2), (LPARAM)(0));
		send_msg(SCI_SETMARGINSENSITIVEN, (WPARAM)(2), (LPARAM)(false));

		return *this;
	}

	scintilla_wnd& highlite_line(int line)
	{
		if (highlight_ != 0)
		{
			send_msg(SCI_MARKERDELETEHANDLE, (WPARAM)highlight_, (LPARAM)(0));
			highlight_ = 0;
		}

		if (line == -1)
		{
			return *this;
		}

		send_msg(SCI_MARKERDEFINE, (WPARAM)(1), (LPARAM)(SC_MARK_BACKGROUND));
		send_msg(SCI_MARKERSETBACK, (WPARAM)(1), (LPARAM)(RGB(255, 245, 199)));
		highlight_ = send_msg(SCI_MARKERADD, (WPARAM)line, (LPARAM)(1));

		return *this;
	}


	// Lexer and Styles

	scintilla_wnd& set_lexer(int lex)
	{
		send_msg(SCI_SETLEXER, (WPARAM)lex, (LPARAM)0);
		return *this;
	}

	int get_lexer()
	{
		return (int)send_msg(SCI_GETLEXER, (WPARAM)0, (LPARAM)0);
	}

	scintilla_wnd& set_mode(int lex)
	{
		set_lexer(SCLEX_CPP);

		for (auto& styleSet : xmlStyleSets.styleSet)
		{
			if (styleSet.id == lex)
			{
				size_t c = styleSet.keywords.size();
				for (int i = 0; i < c; i++)
				{
					set_keywords(i, styleSet.keywords[i]);
				}
				break;
			}
		}
		
		colorize();
		return *this;
	}

	scintilla_wnd& colorize(int start = 0, int end = -1)
	{
		send_msg(SCI_COLOURISE, (WPARAM)start, (LPARAM)end);
		return *this;
	}

	scintilla_wnd& set_style(int style, COLORREF fore, COLORREF back = 0, int size = 0, const char* font = 0)
	{
		style_set_fore(style, fore);
		if (back)
			style_set_back(style, back);
		if (size >= 1)
			style_set_size(style, size);
		if (font)
			style_set_font(style, font);

		return *this;
	}

	scintilla_wnd& set_caret_forecol(int col)
	{
		send_msg(SCI_SETCARETFORE, (WPARAM)col, (LPARAM)col);
		return *this;
	}


	scintilla_wnd& style_set_font(int style, const std::string& font)
	{
		send_msg(SCI_STYLESETFONT, (WPARAM)style, (LPARAM)(font.c_str()));
		return *this;
	}

	scintilla_wnd& style_set_size(int style, int size)
	{
		send_msg(SCI_STYLESETSIZE, (WPARAM)style, (LPARAM)size);
		return *this;
	}

	scintilla_wnd& style_set_fore(int style, int col)
	{
		if (style == STYLE_LINENUMBER)
		{
			set_caret_forecol(col);
			send_msg(SCI_SETSELBACK, (WPARAM)(1), (LPARAM)(col));
		}
		send_msg(SCI_STYLESETFORE, (WPARAM)style, (LPARAM)col);
		return *this;
	}

	scintilla_wnd& style_set_back(int style, int col)
	{
		if (style == STYLE_LINENUMBER)
		{
			send_msg(SCI_SETSELFORE, (WPARAM)(1), (LPARAM)(col));
		}
		send_msg(SCI_STYLESETBACK, (WPARAM)style, (LPARAM)col);
		return *this;
	}

	scintilla_wnd& set_keywords(int keySet, const std::string& keyList)
	{
		send_msg(SCI_SETKEYWORDS, (WPARAM)keySet, (LPARAM)(keyList.c_str()));
		return *this;
	}

	scintilla_wnd& style_set_bold(int style, bool bold)
	{
		send_msg(SCI_STYLESETBOLD, (WPARAM)style, (LPARAM)bold);
		return *this;
	}

	scintilla_wnd& style_set_italic(int style, bool bold)
	{
		send_msg(SCI_STYLESETITALIC, (WPARAM)style, (LPARAM)bold);
		return *this;
	}

	scintilla_wnd& style_set_eol_filled(int style, bool filled)
	{
		send_msg(SCI_STYLESETEOLFILLED, (WPARAM)style, (LPARAM)filled);
		return *this;
	}

	scintilla_wnd& load_xml(const std::wstring& path)
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

				COLORREF foreCol = hex_to_rgb(xmlStyle.foreColor);
				COLORREF backCol = hex_to_rgb(xmlStyle.backgroundColor);

				set_style(xmlStyle.id, foreCol, backCol, xmlStyle.fontSize, xmlStyle.font.c_str());
					//	setKeywords((xmlStyleSet.id, xmlStyleSet.keywords);

				if (xmlStyle.bold == "true")
				{
					style_set_bold(xmlStyle.id, true);
				}
				if (xmlStyle.italic == "true")
				{
					style_set_italic(xmlStyle.id, true);
				}
				if (xmlStyle.eol == "true")
				{
					style_set_eol_filled(xmlStyle.id, true);
				}
			}
		}
		return *this;
	}


	int search_pos()
	{
		return nextSearchPos_;
	}

	bool search_pos(unsigned int pos)
	{
		if (pos <= (unsigned int)this->get_length())
		{
			nextSearchPos_ = pos;
			return true;
		}
		return false;
	}

	int search_options()
	{
		return searchOptions_;
	}

	bool search_options(int options)
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


class sci_lexer
{
public:
	sci_lexer()
	{
		::LoadLibrary(L"SciLexer.dll");
	}
};

}
