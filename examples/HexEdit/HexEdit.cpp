
#include "HexEdit.h"
#include "MTL/util/path.h"
#include "MTL/ole/img.h"
#include "MTL/ole/explorer.h"

// Model holding relevant applicationstate
// HexModel provides load and search functionality

class HexModel
{
public:

	HexModel()
	{}

	std::wstring load(const std::wstring& filename, bool readOnly)
	{
		if (map_)
			map_.close();

		filename_ = filename;

		// map file, if necessary RO
		bool sucess = false;
		if (readOnly)
			sucess = map_.map(filename, GENERIC_READ);
		else
			sucess = map_.map(filename);

		if (!sucess) return L"";

		fsize_ = (unsigned int)map_.size();

		searchPos_ = 0;

		// return new main window text
		std::wstringstream oss;
		oss << filename;
		if (readOnly)
			oss << L" [readonly]";

		return oss.str();
	}


	const std::wstring& filename()
	{
		return filename_;
	}

	mtl::memory_mapped_file& map()
	{
		return map_;
	}

	unsigned int fsize()
	{
		return fsize_;
	}

	const size_t searchPos()
	{
		return searchPos_;
	}

	bool search(const std::wstring& txt, DWORD flags, std::function<void(unsigned int)> cb)
	{
		if (!map_)
			return false;

		if (!(flags & FR_DOWN))
			return searchUp(txt, flags, cb);

		size_t s = txt.size();
		size_t p = searchPos_;

		while (p < fsize_)
		{
			char* c = map_[p];
			if ((*c == txt[0]) || (!(flags & FR_MATCHCASE) && (tolower(txt[0]) == tolower(*c))))
			{
				if (p + s >= fsize_)
					break;

				int result = 0;
				if (flags & FR_MATCHCASE)
					result = wcsncmp(txt.c_str(), (const wchar_t*)c, txt.size());
				else
					result = _wcsnicmp(txt.c_str(), (const wchar_t*)c, txt.size());

				if (result != 0)
				{
					std::string s = mtl::to_string(txt);
					if (flags & FR_MATCHCASE)
						result = strncmp(s.c_str(), (const char*)c, s.size());
					else
						result = _strnicmp(s.c_str(), (const char*)c, s.size());
				}

				if (result == 0)
				{
					size_t tmp = p / 4;
					tmp = tmp * 4;
					searchPos_ = p + s;

					cb((unsigned int)tmp);
					return true;
				}
			}
			p++;
		}
		searchPos_ = 0;
		::MessageBox(0, L"end of file", L"searching down ...", 0);
		return false;
	}

	bool searchUp(const std::wstring& txt, int flags, std::function<void(unsigned int)> cb)
	{
		if (!map_)
			return false;

		size_t s = txt.size();
		size_t p = searchPos_;

		while (p != 0)
		{
			p--;
			if (p < s)
				break;

			char* c = map_[p - s];
			if ((*c == txt[0]) || (!(flags & FR_MATCHCASE) && (tolower(txt[0]) == tolower(*c))))
			{
				int result = 0;
				if (flags & FR_MATCHCASE)
					result = _wcsnicmp(txt.c_str(), (const wchar_t*)c, txt.size());
				else
					result = _wcsnicmp(txt.c_str(), (const wchar_t*)c, txt.size());

				if (result != 0)
				{
					std::string s = mtl::to_string(txt);
					if (flags & FR_MATCHCASE)
						result = strncmp(s.c_str(), (const char*)c, s.size());
					else
						result = _strnicmp(s.c_str(), (const char*)c, s.size());
				}

				if (result == 0)
				{
					size_t tmp = (p - s) / 4;
					tmp = tmp * 4;
					searchPos_ = p - s + 1;

					cb((unsigned int)tmp);
					return true;
				}
			}

		}
		searchPos_ = fsize_;
		::MessageBox(0, L"start of file", L"searching up ...", 0);
		return false;
	}

private:

	std::wstring			filename_;
	mtl::memory_mapped_file	map_;
	unsigned int		    fsize_ = 0;
	size_t					searchPos_ = 0;
};


// Main View widget - knows how to render and navigate a hex view 
// based on the HexModel state

class HexWindow : public mtl::scroll_wnd<HexWindow>
{
public:

	HexModel& model;
	mtl::event<void(std::wstring)> onOffset;

	HexWindow(HexModel& m)
		: model(m)
    {
		fontDesc_ = mtl::font_desc(L"Lucida Console", 18);

		color_ = RGB(238, 236, 238);
		br_ = ::CreateSolidBrush(color_);
    }

	~HexWindow()
	{
		::DeleteObject(br_);
		::DeleteObject(font_);
	}

	void setFont(LOGFONT* lf)
	{
		fontDesc_ = mtl::font_desc(lf);
		recalc();
		::InvalidateRect(handle, 0, TRUE);
		::UpdateWindow(handle);
	}

	void reset()
	{
		offset_ = 0;
		caretX_ = 0;
		caretY_ = 0;

		recalc();
		scrollTo(0);
		//sendMsg(WM_LAYOUT, 0, 0);

		::InvalidateRect(this->handle,0, FALSE);
		::SetFocus(this->handle);
	}

	void scrollTo(unsigned int off, bool adjust = true)
	{
		if (!model.map())
			return;
		if (model.fsize() <= off)
			return;

		off = off / 4;
		off = off * 4;

		updateOffset(off);

		if (adjust)
		{
			off = off / cols_;
			if (off > 10)
				off -= 10;
			else
				off = 0;
		}

		set_scroll_pos(off);
	}

	virtual LRESULT wm_draw(HDC hdc, RECT& r) override
	{
		mtl::dc dc(hdc);
		dc.select(font_);

		// fill offset background if necessary
		if (horizontalScrollInfo_.nPos < 9)
		{
			RECT rc = r;
			rc.right = (9 - horizontalScrollInfo_.nPos) * tmwidth_;
			::FillRect(hdc, &rc, br_);
			::MoveToEx(hdc, rc.right, 0, 0);
			::LineTo(hdc, rc.right, r.bottom);
		}

		// draw ascii bkg rect
		int lx = tmwidth_ * ((cols_ * 2) + (cols_ / 4) + 13 - horizontalScrollInfo_.nPos);
		RECT rc = r;
		rc.left = lx;
		::FillRect(hdc, &rc, br_);

		rc.left += tmwidth_;
		RECT tr = rc;
		for (unsigned int i = 0; i < cols_; i += 8)
		{
			if (i > cols_)
				break;

			tr.left = rc.left + tmwidth_ * (i);
			tr.right = tr.left + tmwidth_ * 4;
			::FillRect(hdc, &tr, (HBRUSH)::GetStockObject(WHITE_BRUSH));
		}

		// draw ascii seperator line

		::MoveToEx(hdc, lx, 0, 0);
		::LineTo(hdc, lx, r.bottom);

		if (!model.map())
			return 0;

		// start drawing output determined by y scrollpos
		unsigned int c = verticalScrollInfo_.nPos * cols_;
		while (c < model.fsize())
		{
			int  x = 0;
			int  y = 0;
			int  tab = 0;
			static wchar_t buf[32];

			// determine y offset
			y = (c / cols_ - verticalScrollInfo_.nPos) * (tmheight_ + 4);

			// print offset
			wsprintf(buf, L"%08X:", c);
			x = -horizontalScrollInfo_.nPos * tmwidth_;

			dc.set_bk_color(color_);
			dc.text_out(x, y, buf);
			dc.set_bk_color(RGB(255, 255, 255));

			// now print binary AND ascii values for this row
			for (unsigned int i = 0; i < cols_; i++)
			{
				// determine proper x offset
				x = i * (tmwidth_ * 2) + tab + (tmwidth_ * 11);

				// make sure we haven't reached end of mapping
				if (c > model.fsize() - 1)
					break;

				// get current char from map and convert to uint
				unsigned char v = *(model.map()[c]);
				unsigned int iv = (unsigned int)v;

				// add tab every 4 (DWORD) cols
				if ((i + 1) % 4 == 0)
					tab += tmwidth_;

				// print this char hex value
				dc.set_bk_color(RGB(255, 255, 255));
				wsprintf(buf, L"%02x", iv);
				y = (c / cols_ - verticalScrollInfo_.nPos) * (tmheight_ + 4);
				dc.text_out(x - horizontalScrollInfo_.nPos * tmwidth_, y, buf);

				// print ascii value
				wsprintf(buf, L"%c", v);
				if ((i % 8) > 3)
					dc.set_bk_color(color_);
				else
					dc.set_bk_color(RGB(255, 255, 255));

				// determine x offset for ascii display
				x = (cols_ * 2) + (cols_ / 4) + i + 14;

				x = x * tmwidth_;

				dc.text_out(x - horizontalScrollInfo_.nPos * tmwidth_, y, buf);

				// next char value
				c++;

				// boil out if outside display
				if ((c / cols_ - verticalScrollInfo_.nPos) * tmheight_ > (unsigned int)r.bottom)
					break;
			}
			// boil out if outside display
			if ((c / cols_ - verticalScrollInfo_.nPos) * tmheight_ > (unsigned int)r.bottom)
				break;
		}

		drawOffset(dc, offset_, RGB(0, 0, 0));

		return 0;
	}


	virtual LRESULT wm_erase_background(WPARAM wParam) override
	{
		// prevent bkgrnd erase
		return 1;
	}

	virtual LRESULT wm_create() override
	{
		recalc();
		return 0;
	}

	virtual LRESULT wm_dpi_changed(RECT* r) override
	{
		recalc();
		return 0;
	}

	virtual LRESULT on_left_button_down(int x, int y)
	{
		setOffsetByMousePos(x, y);
		::SetFocus(this->handle);
		return 0;
	}

	virtual LRESULT on_key_down(int key)
	{
		switch (key)
		{
		case VK_HOME:
		{
			caretX_ = 0;
			caretY_ = 0;
			break;
		}
		case VK_END:
		{
			caretX_ = (model.fsize() % cols_) * 2;
			caretY_ = (model.fsize() / cols_);
			if (caretY_ > 0)
				--caretY_;

			break;
		}
		case VK_PRIOR:
		{
			if (caretY_ >= verticalScrollInfo_.nPage)
				caretY_ -= verticalScrollInfo_.nPage;
			else
				caretY_ = 0;
			break;
		}
		case VK_NEXT:
		{
			if (caretY_ + verticalScrollInfo_.nPage < scrollRows_)
				caretY_ += verticalScrollInfo_.nPage;
			else
				caretY_ = scrollRows_ - 1;
			break;
		}
		case VK_RIGHT:
		{
			caretX_ += 8;
			if (caretX_ >= cols_ * 2)
			{
				caretX_ = 0;
				if ((caretY_ + 1) * cols_ < model.fsize())
					caretY_++;
			}
			break;
		}
		case VK_DELETE:
		case VK_LEFT:
		{
			if (caretX_ == 0)
			{
				caretX_ = cols_ * 2 - 1;
				if (caretY_ > 0)
					caretY_--;
			}
			else
			{
				if (caretX_ <= 8)
					caretX_ = 0;
				else
					caretX_ -= 8;
			}
			break;
		}
		case VK_UP:
		{
			if (caretY_ > 0)
				caretY_--;
			break;
		}
		case VK_DOWN:
		{
			if ((caretY_ + 1) * cols_ < model.fsize())
			{
				caretY_++;
			}
			break;
		}
		}
		unsigned int off = (caretX_ / 8) * 4 + caretY_ * cols_;
		if ((caretY_ < (unsigned int)(verticalScrollInfo_.nPos)) || (caretY_ - (unsigned int)(verticalScrollInfo_.nPos) > (unsigned int)(verticalScrollInfo_.nPage)))
		{
			scrollTo(off);
		}
		
		updateOffset(off);
		return 0;
	}

	virtual LRESULT on_char(char c)
	{
		switch (c)
		{
		case '\b': //backspace
		{
			send_msg(WM_KEYDOWN, WPARAM(VK_DELETE), 0);
			return 0;
		}
		case '\t': //tab
		{
			break;
		}
		case '\n': //linefeed
		{
			if ((caretY_ + 1) * cols_ < model.fsize())
				caretY_++;
			break;
		}
		case '\r': //return
		{
			caretX_ = 0;
			if ((caretY_ + 1) * cols_ < model.fsize())
				caretY_++;
			break;
		}
		case '\x1B': //escape
		{
			break;
		}
		default: // characters
		{
			break;
		}
		} 


		unsigned int off = (caretX_ / 8) * 4 + caretY_ * cols_;
		if ((caretY_ < (unsigned int)(verticalScrollInfo_.nPos)) || (caretY_ - (unsigned int)(verticalScrollInfo_.nPos) > (unsigned int)(verticalScrollInfo_.nPage)))
		{
			scrollTo(off);
		}
		updateOffset(off);
		return 0;
	}

	LRESULT virtual wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override
	{
		switch (message)
		{
		case WM_LBUTTONDOWN:
		{
			int mx = GET_X_LPARAM(lParam);
			int my = GET_Y_LPARAM(lParam);

			return on_left_button_down(mx, my);
		}
		case WM_KEYDOWN:
		{
			return on_key_down((int)wParam);
		}
		case WM_CHAR:
		{
			for (int i = 0; i < (int)LOWORD(lParam); i++)
			{
				on_char((char)wParam);
			}
			return 0;
		}
		}

		return mtl::scroll_wnd<HexWindow>::wndProc(hWnd, message, wParam, lParam);
	}

private:

	void setOffsetByMousePos(unsigned int mx, unsigned int my)
	{
		if (!model.map())
			return;

		// get scroll pos
		int y = verticalScrollInfo_.nPos;
		int x = horizontalScrollInfo_.nPos;

		// determine row
		y += my / (tmheight_ + 4);

		// determine col
		mx = mx + x * tmwidth_;

		// check if address got clicked
		if (mx <= (tmwidth_ * 11))
		{
			caretX_ = 0;
			caretY_ = y;

			// offset determined by rows (y)
			unsigned int off = y * cols_;
			if (off >= model.fsize())
				return;
			updateOffset(off);
			return;
		}

		// adjust mx for address cols
		mx = mx - 11 * tmwidth_;

		// check if out of bounds
		if (mx >= tmwidth_ * ((cols_ * 2) + cols_ / 4))
			return;

		// offset of DWORD clicked

		caretX_ = (mx / (tmwidth_));
		caretX_ = caretX_ - (caretX_ / 9);
		caretY_ = y;

		// offset of DWORD clicked
		unsigned int off = (caretX_ / 8) * 4 + y * cols_;

		if (off >= model.fsize())
			return;

		updateOffset(off);
	}

    // update offset 
	void updateOffset(unsigned int offset)
	{
		// get DC to highlight
		mtl::wnd_dc wdc(this->handle);
		wdc.select(font_);

		//remove old highlite
		drawOffset(wdc, offset_, true);

		setOffset(offset);

		//prepare offset addres as hex string for toolbar
		std::wostringstream woss;
		woss << currentAddress_ << L" : " << currentOffsetValue_;
		onOffset.fire(woss.str());

		// draw offest
		wdc.set_bk_color(RGB(238, 236, 238));
		drawOffset(wdc, offset_, false);
	}

	// helper
	void setOffset(unsigned int offset)
	{
		//change offset value
		offset_ = offset;

		//prepare offset addres as hex string for toolbar
		std::wostringstream ossAddr;
		ossAddr.fill(L'0');
		ossAddr << std::hex
			<< std::setw(8)
			<< (offset);

		currentAddress_ = ossAddr.str();

		//prepare offset value as hex string
		unsigned char c = 0;
		unsigned int  i = 0;
		std::wostringstream ossVal;
		ossVal.fill(L'0');
		ossVal << std::hex << std::setw(2);

		c = *(model.map()[offset]);
		i = c;
		ossVal << i << std::setw(2);

		c = *(model.map()[offset + 1]);
		i = c;
		ossVal << i << std::setw(2);

		c = *(model.map()[offset + 2]);
		i = c;
		ossVal << i << std::setw(2);

		c = *(model.map()[offset + 3]);
		i = c;
		ossVal << i;

		currentOffsetValue_ = ossVal.str();
	}

    // draw offset helper
    void drawOffset(mtl::dc& dcv, unsigned int off, bool erase = false)
	{
		if (!model.map())
			return;

		if (off >= model.fsize())
			return;

		if (off < model.map().offset() || off >(model.map().offset() + model.map().page_size()))
			return;

		RECT r;
		::GetClientRect(this->handle,&r);

		// // determine proper x offset
		int y = off / cols_;
		int x = off % cols_;

		int tab = (1 + x / 4);
		x = x * 2;
		x = x + 10 - horizontalScrollInfo_.nPos + tab;
		x = x * tmwidth_;

		y = y - verticalScrollInfo_.nPos;
		y = y * (tmheight_ + 4);

		RECT rf(x - 2, y - 2, x + 4 + tmwidth_ * 8, y + tmheight_ + 4);
		if (erase)
			::FillRect(*dcv, &rf, (HBRUSH)::GetStockObject(WHITE_BRUSH));
		else
			::FillRect(*dcv, &rf, br_);

		int end = 4;
		if (model.fsize() - off < 4)
		{
			end = end - (4-(model.fsize() - off));
		}

		for (int i = 0; i < end; i++)
		{
			unsigned char v = *(model.map()[off+i]);
			unsigned int iv = (unsigned int)v;

			// add tab every 4 (DWORD) cols
			if ((i + 1) % 4 == 0)
				tab += tmwidth_;

			// print this char hex value
			wchar_t buf[24];
			wsprintf(buf, L"%02x", iv);
			y = ( (off+i) / cols_ - verticalScrollInfo_.nPos) * (tmheight_ + 4);
			dcv.text_out(x - horizontalScrollInfo_.nPos * tmwidth_ + i * tmwidth_*2, y, buf);

		}
		if (!erase)
			::DrawEdge(*dcv, &rf, EDGE_ETCHED, BF_RECT);
	}

	// DPI awareness inside here
	void recalc()
	{
		// get dpi
		int iDpi = ::GetDpiForWindow(this->handle);

		// scale our fontdesc height by dpi
		mtl::font_desc fd(fontDesc_);
		fd.font()->lfHeight = -::MulDiv(fontDesc_.font()->lfHeight, iDpi, 96);

		// create a new font for new dpi
		if (font_) ::DeleteObject(font_);
		font_ = ::CreateFontIndirect(fd.font());

		// determine text metrics of font for new dpi
		mtl::wnd_dc wdc(this->handle);
		wdc.select(font_);

		// remember text metrics
		TEXTMETRIC tm;
		::GetTextMetrics(*wdc, &tm);
		tmwidth_ = tm.tmAveCharWidth;
		tmheight_ = tm.tmHeight;

		// set scroll info according to dpi text metrics
		int scrollCellHeight = tmheight_ + 4;
		unsigned int nScrollRows = model.fsize() / cols_ + 1;
		set_v_scroll( scrollCellHeight, nScrollRows);

		int scrollCellWidth = tmwidth_;
		unsigned int nScrollCols = 11 + (cols_ * 2) + (cols_ / 4) + 2 + cols_;
		set_h_scroll(scrollCellWidth, nScrollCols);
	}


	// offset and cursor pos
    unsigned int		    offset_		= 0;
	unsigned int		    caretX_		= 0;
	unsigned int		    caretY_		= 0;

    // layout info
    unsigned int		    cols_      = 24;
    unsigned int		    tmwidth_   = 0;
    unsigned int		    tmheight_  = 0;

	std::wstring			currentOffsetValue_;
	std::wstring			currentAddress_;

    // GDI references
    HBRUSH				    br_			= nullptr;
	HFONT					font_		= nullptr;
	COLORREF			    color_;
	mtl::font_desc			fontDesc_;
};


// main (frame) window

class MainWindow : public mtl::window<MainWindow>
{
public:

	mtl::event<int()>				onCmd;
	mtl::event<void(FINDREPLACE&)>	onFind;
	mtl::default_layout				layout;

	virtual LRESULT wm_command(int id) override
	{
		onCmd.fire(id);
		return 0;
	}

	virtual LRESULT wm_search(FINDREPLACE* fr) override
	{
		if (!fr) return 0;

		onFind.fire(*fr);
		return 0;
	}

	virtual LRESULT wm_erase_background(WPARAM wParam) override
	{
		// prevent bkgrnd erase
		return 1;
	}

    virtual LRESULT wm_size(RECT& clientRect) override
    {
        RECT r = { 0,0,0,0 };
        layout.do_layout(clientRect, r);
        return 0;
    }

    virtual LRESULT wm_destroy() override
    {
		onCmd.fire(IDM_EXIT);
        return 0;
    }
};

// View constructs Widget tree and Layout
// View owns all Win32 widgets

class HexView
{
public:

	HexModel			model;
	MainWindow			mainWnd;
	HexWindow			hexWnd;

	mtl::menu			menu;
	mtl::status_bar		statusBar;
	mtl::splitter		splitter;
	mtl::explorer_tree  tree;
	mtl::search_dlg		searchDlg;

	HexView(HexModel& m)
		: model(m), hexWnd(m), menu(IDC_HEXEDIT)
	{
		// create main window
		HWND hWnd = mainWnd.create(L"Hex Edit", WS_OVERLAPPEDWINDOW,0,*menu);

		// get main window client rect dimension
		RECT r1;
		::GetClientRect(hWnd, &r1);

		// create normal child windows, initially sized to full client rect
		// layout will resize them approp
		tree.create(IDC_CHILD, hWnd, r1, WS_CHILD | WS_VISIBLE| WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		hexWnd.create(IDC_HEXWND, L"", hWnd, r1, WS_CHILD | WS_VISIBLE | WS_VSCROLL| WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		statusBar.create(IDC_STATUS, L"", hWnd, r1);

		// create splitter at given initial position and given width
		r1.left = 200;
		r1.right = 208;
		splitter.create(IDC_SPLITTER, hWnd, r1, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

		// set the layout
		mainWnd.layout = {							// Default Layout
			{										//  vector<Widget>
				{									//   Widget
					*statusBar,						//    holding statusbar
					mtl::layout::style::SOUTH		//    docked to SOUTH
				},
				{									//   Widget
					{								//    SplitterLayout
						*tree,						//     Left Splitter child window: tree
						*splitter,					//     Splitter Window Separator
						*hexWnd						//     Right Splitter child wind
					},  
					mtl::layout::style::FILL		//   filling remaining space
				}
			}
		};

		::ShowWindow(*mainWnd, SW_SHOW);
		::UpdateWindow(*mainWnd);
	}
};

// Controller handles User Input
// Controller owns Model and View

class HexController
{
public:

	HexModel model;
	HexView view;

	HexController()
		:view(model)
	{
		view.mainWnd.onCmd(ID_FILE_OPEN, [this]()
		{
			mtl::open_dlg openDlg(view.mainWnd.handle);
			if (openDlg.open(OFN_READONLY))
			{
				std::wstring ws = model.load(openDlg.filename(), true);
				if (!ws.empty())
				{
					view.mainWnd.set_text(mtl::path(ws).filename());
					view.hexWnd.reset();
				}
			}
		});

		view.tree.onSelect([this](std::wstring path)
		{
			mtl::path p(path);
			if (!p.is_dir())
			{
				std::wstring ws = model.load(path, true);
				if (!ws.empty())
				{
					view.mainWnd.set_text(mtl::path(ws).filename());
					view.hexWnd.reset();
				}
			}
		});

		view.hexWnd.onOffset([this](std::wstring offset)
		{
			view.statusBar.set_text(offset);
		});

		view.mainWnd.onCmd(ID_FILE_FIND, [this]()
		{
			view.searchDlg.find(view.mainWnd.handle, FR_DOWN);
		});


		view.mainWnd.onFind([this](FINDREPLACE& fr)
		{
			model.search(fr.lpstrFindWhat, fr.Flags, [this](unsigned int pos)
			{
				view.hexWnd.scrollTo(pos);
			});
		});

		view.mainWnd.onCmd(ID_FILE_FONT, [this]()
		{
			mtl::pick_font picker(CF_INITTOLOGFONTSTRUCT | CF_NOSTYLESEL | CF_FIXEDPITCHONLY | CF_NOSCRIPTSEL);
			if (picker.choose(view.mainWnd.handle))
			{
				view.hexWnd.setFont(picker.font());
			}
		});

		view.mainWnd.onCmd(IDM_ABOUT, [this]()
		{
			mtl::dialog dlg;
			dlg.show_modal(IDD_ABOUTBOX, view.mainWnd.handle);
		});

		view.mainWnd.onCmd(IDM_EXIT, []()
		{
			::PostQuitMessage(0);
		});
	}
};


// go WinMain, go!

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	mtl::STA enter;

    mtl::application app(hInstance);

	HexController controller;

    return app.run();
}
