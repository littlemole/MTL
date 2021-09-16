#pragma once

#include "MTL/win/wind.h"
#include <windowsx.h>

namespace MTL {

	struct Widget;

	class Layout
	{
	public:

		enum class Style {
			NONE,
			FILL,
			NORTH,
			EAST,
			SOUTH,
			WEST
		};

		virtual ~Layout() {}

		Layout()
		{}

		Layout(std::vector<Widget>&& widgets)
			: widgets_(std::move(widgets))
		{}

		void move(Widget& w, RECT& dest);

		virtual void do_layout(RECT& r, RECT& p);

	protected:
		std::vector<Widget> widgets_;
	};

	class DefaultLayout : public Layout
	{
	public:

		DefaultLayout()
		{}

		DefaultLayout(std::vector<Widget>&& widgets)
			: Layout(std::move(widgets))
		{}

		//	void move(Widget& w, RECT& dest);

		//	void do_layout(RECT& r);

	};

	class ColumnLayout : public Layout
	{
	public:

		//	RECT padding = { 0,0,0,0 };

			//struct Widget;

		ColumnLayout()
		{}

		ColumnLayout(std::vector<Widget>&& widgets)
			: Layout(std::move(widgets))
		{}

		//void move(Widget& w, RECT& dest);

		void do_layout(RECT& r, RECT& p) override;

	};

	class RowLayout : public Layout
	{
	public:

		RowLayout()
		{}

		RowLayout(std::vector<Widget>&& widgets)
			: Layout(std::move(widgets))
		{}

		void do_layout(RECT& r, RECT& p) override;
	};

	class Splitter : public Window<Splitter>
	{
	public:

		Splitter()
		{
			static int unused = []() // init once
			{
				windowClass<Splitter>().hCursor = ::LoadCursor(nullptr, IDC_HAND);// IDC_SIZEWE);
				return 0;
			}();
		}

		LRESULT virtual wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

	private:
		POINT			lp_;
		HCURSOR			prevCursor_;
		bool			bTrack_;
	};

	class SplitterLayout :  public Layout
	{
	public:

		SplitterLayout(HWND w1, HWND w2, HWND w3);

		void do_layout(RECT& r, RECT& p) override;

	};


	struct Widget
	{
		HWND hWnd = nullptr;
		Layout::Style style;
		std::shared_ptr<Layout> layout;
		RECT padding = { 0,0,0,0 };

		Widget() {}

		Widget(const Widget& rhs)
			:hWnd(rhs.hWnd), style(rhs.style), layout(rhs.layout), padding(rhs.padding)
		{
		}

		Widget(Widget&& rhs)
			:hWnd(rhs.hWnd), style(rhs.style), padding(rhs.padding)
		{
			if (rhs.layout)
			{
				layout = std::shared_ptr<Layout>(std::move(rhs.layout));
			}
			rhs.hWnd = 0;
			rhs.layout = 0;
		}

		Widget& operator=(const Widget& rhs)
		{
			if (&rhs == this)
			{
				return *this;
			}

			hWnd = rhs.hWnd;
			style = rhs.style;
			layout = rhs.layout;
			padding = rhs.padding;

			return *this;
		}

		Widget& operator=(Widget&& rhs)
		{
			if (&rhs == this)
			{
				return *this;
			}

			hWnd = rhs.hWnd;
			style = rhs.style;
			padding = rhs.padding;
			if (rhs.layout)
			{
				layout = std::move(rhs.layout);
			}
			rhs.hWnd = 0;
			rhs.layout = 0;
			return *this;
		}

		Widget(SplitterLayout&& l, Layout::Style s = MTL::Layout::Style::NONE)
			:layout(new SplitterLayout(std::move(l))), style(s)
		{}

		Widget(HWND wnd, Layout::Style s = MTL::Layout::Style::NONE)
			:hWnd(wnd), style(s)
		{}

		Widget(DefaultLayout&& l, Layout::Style s)
			:layout(new DefaultLayout(std::move(l))), style(s)
		{}

		Widget(ColumnLayout&& l, RECT&& p)
			:layout(new ColumnLayout(std::move(l))), padding(p)
		{}

		Widget(RowLayout&& l, RECT&& p)
			:layout(new RowLayout(std::move(l))), padding(p)
		{}

		/*
		Widget(ColumnLayout&& l, Layout::Style s)
			:layout(std::move(l)), style(s)
		{}
		*/
	};

	inline void Layout::move(Widget& w, RECT& dest)
	{
		if (w.hWnd)
		{
			::SetWindowPos(w.hWnd, NULL, dest.left, dest.top, dest.right - dest.left, dest.bottom - dest.top, SWP_NOZORDER | SWP_NOACTIVATE);
			::UpdateWindow(w.hWnd);
		}
		else if (!w.layout->widgets_.empty())
		{
			w.layout->do_layout(dest, w.padding);
		}
	}

	inline void Layout::do_layout(RECT& r, RECT& p)
	{
		RECT& avail = r;
		for (auto& w : widgets_)
		{
			if (w.hWnd == NULL)
			{
				RECT padded = {
					avail.left + w.padding.left,
					avail.top + w.padding.top,
					avail.right - w.padding.left - w.padding.right,
					avail.bottom - w.padding.top - w.padding.bottom
				};
				avail = padded;
				move(w, avail);
				continue;
			}
			switch (w.style)
			{
			case Style::NONE:
			{
				break;
			}
			case Style::FILL:
			{
				RECT dest = avail;
				move(w, dest);
				return;
				break;
			}
			case Style::NORTH:
			{
				RECT dest = avail;
				RECT wr;
				::GetWindowRect(w.hWnd, &wr);

				dest.top = 0;
				dest.bottom = wr.bottom - wr.top;

				move(w, dest);

				avail.top = dest.bottom;
				break;
			}
			case Style::EAST:
			{
				RECT dest = avail;
				RECT wr;
				::GetWindowRect(w.hWnd, &wr);
				dest.left = avail.right - (wr.right - wr.left);

				move(w, dest);

				avail.right = dest.left;
				break;
			}
			case Style::SOUTH:
			{
				RECT dest = avail;
				RECT wr;
				::GetWindowRect(w.hWnd, &wr);
				dest.top = avail.bottom - (wr.bottom - wr.top);

				move(w, dest);

				avail.bottom = dest.top;
				break;
			}
			case Style::WEST:
			{
				RECT dest = avail;
				RECT wr;
				::GetWindowRect(w.hWnd, &wr);
				dest.left = 0;
				dest.right = wr.right - wr.left;

				move(w, dest);

				avail.left = dest.right;
				break;
			}
			}
		}
	}

	inline void ColumnLayout::do_layout(RECT& padded, RECT& p)
	{
		/*
		int width = 0;
		for (auto& w : widgets_)
		{
			RECT wRect;
			::GetWindowRect(w.hWnd, &wRect);
			int w = wRect.right - wRect.left;
			if (w > width)
			{
				width = w;
			}
		}

		int cellHeight = (padded.bottom - padded.top) / widgets_.size();
		*/
		int top = padded.top;
		for (int i = 0; i < widgets_.size(); i++)
		{
			auto& w = widgets_[i];
			RECT target = {
				padded.left,
				top, //padded.top + (i * cellHeight),
				padded.right,
				top//padded.top + (i*cellHeight) + cellHeight
			};

			RECT wRect;
			::GetWindowRect(w.hWnd, &wRect);

			target.bottom = top + (wRect.bottom - wRect.top);
			top = target.bottom + p.top + p.bottom;

			int x = ((target.right - target.left) - (wRect.right - wRect.left)) / 2;
			int y = ((target.bottom - target.top) - (wRect.bottom - wRect.top)) / 2;

			RECT adjust = { x,y,x,y };

			switch (w.style)
			{
			case Style::NONE: { break; }
			case Style::NORTH: { adjust = { x,0,x,2 * y }; break; }
			case Style::EAST: { adjust = { 2 * x,y,0,y }; break; }
			case Style::SOUTH: { adjust = { x,2 * y,x,0 }; break; }
			case Style::WEST: { adjust = { 0,y,2 * x,y }; break; }
			}

			RECT dest = {
				target.left + adjust.left,
				target.top + adjust.top,
				target.left + adjust.left + (wRect.right - wRect.left),
				target.top + adjust.top + (wRect.bottom - wRect.top)
			};

			move(w, dest);
		}
	}


	inline void RowLayout::do_layout(RECT& padded, RECT& p)
	{
		/*
		int height = 0;
		for (auto& w : widgets_)
		{
			RECT wRect;
			::GetWindowRect(w.hWnd, &wRect);
			int size = wRect.bottom - wRect.top;
			if (size > height)
			{
				height = size;
			}
		}

		int cellWidth = (padded.right - padded.left) / widgets_.size();
		*/
		int left = padded.left;
		for (int i = 0; i < widgets_.size(); i++)
		{
			auto& w = widgets_[i];
			RECT target = {
				left,
				padded.top, //padded.top + (i * cellHeight),
				padded.right,
				padded.bottom//padded.top + (i*cellHeight) + cellHeight
			};

			RECT wRect;
			::GetWindowRect(w.hWnd, &wRect);

			target.right = left + (wRect.right - wRect.left);
			left = target.right + p.left + p.right;

			int x = ((target.right - target.left) - (wRect.right - wRect.left)) / 2;
			int y = ((target.bottom - target.top) - (wRect.bottom - wRect.top)) / 2;

			RECT adjust = { x,y,x,y };

			switch (w.style)
			{
			case Style::NONE: { break; }
			case Style::NORTH: { adjust = { x,0,x,2 * y }; break; }
			case Style::EAST: { adjust = { 2 * x,y,0,y }; break; }
			case Style::SOUTH: { adjust = { x,2 * y,x,0 }; break; }
			case Style::WEST: { adjust = { 0,y,2 * x,y }; break; }
			}

			RECT dest = {
				target.left + adjust.left,
				target.top + adjust.top,
				target.left + adjust.left + (wRect.right - wRect.left),
				target.top + adjust.top + (wRect.bottom - wRect.top)
			};

			move(w, dest);
		}

		padded = {

		};
	}

	inline SplitterLayout::SplitterLayout(HWND w1, HWND w2, HWND w3)
		: Layout(std::move(std::vector<Widget>({ Widget(w1),Widget(w2),Widget(w3) })))
	{
	}

	// TODO add support for invisible windows. only splitter if both childs are visible!
	inline void SplitterLayout::do_layout(RECT& r, RECT& p)
	{
		RECT wr1;
		RECT wr2;
		RECT wr3;
		::GetWindowRect(widgets_[0].hWnd, &wr1);
		::GetWindowRect(widgets_[1].hWnd, &wr2);
		::GetWindowRect(widgets_[2].hWnd, &wr3);

		bool isWnd1Visible = ::IsWindowVisible(widgets_[0].hWnd);
		bool isWnd2Visible = ::IsWindowVisible(widgets_[1].hWnd);
		bool isWnd3Visible = ::IsWindowVisible(widgets_[2].hWnd);

		if ((!isWnd1Visible) || (!isWnd3Visible) )
		{
			::ShowWindow(widgets_[1].hWnd, SW_HIDE);
			if (isWnd1Visible)
			{
				move(widgets_[0], r);
				::InvalidateRect(widgets_[0].hWnd, 0, TRUE);
				::UpdateWindow(widgets_[0].hWnd);
			}
			else if (isWnd3Visible)
			{
				move(widgets_[2], r);
				::InvalidateRect(widgets_[2].hWnd, 0, TRUE);
				::UpdateWindow(widgets_[2].hWnd);
			}
			return;
		}

		::ShowWindow(widgets_[1].hWnd, SW_SHOW);

		POINT pt = { wr2.left, wr2.top };
		::ScreenToClient(::GetParent(widgets_[1].hWnd), &pt);

		RECT dest = r;
		dest.left = 0;
		dest.right = pt.x;
		move(widgets_[0], dest);
		::InvalidateRect(widgets_[0].hWnd, 0, TRUE);
		::UpdateWindow(widgets_[0].hWnd);

		dest = r;
		dest.left = pt.x + (wr2.right - wr2.left);
		move(widgets_[2], dest);
		::InvalidateRect(widgets_[2].hWnd, 0, TRUE);
		::UpdateWindow(widgets_[2].hWnd);

		dest = r;
		dest.top = 0;
		dest.left = pt.x;
		dest.right = pt.x + (wr2.right - wr2.left);
		move(widgets_[1], dest);
		::InvalidateRect(widgets_[1].hWnd, 0, TRUE);
		::UpdateWindow(widgets_[1].hWnd);

	}


	inline LRESULT Splitter::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CREATE:
		{
			break;
		}
		case WM_PAINT:
		{
			MTL::PaintDC dc(hWnd);
			RECT r;
			::GetClientRect(hWnd, &r);
			HBRUSH br = GetStockBrush(LTGRAY_BRUSH);
			::FillRect(*dc, &r, br);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			if (!bTrack_)
			{
				::GetCursorPos(&lp_);
				::SetCapture(this->handle);
				prevCursor_ = ::SetCursor(::LoadCursor(nullptr, IDC_SIZEWE));
				bTrack_ = true;
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (bTrack_)
			{
				POINT pm; 
				::GetCursorPos(&pm);

				int dx = lp_.x - pm.x;

				POINT tmp = pm;
				::ScreenToClient(::GetParent(this->handle), &tmp);

				RECT rc;
				::GetClientRect(::GetParent(this->handle), &rc);

				if (dx > 0 && (tmp.x + dx < 50))
				{
					break;
				}

				if ( dx < 0 && (tmp.x + dx > rc.right-50))
				{
					break;
				}

				lp_ = pm;
				RECT wr;
				::GetWindowRect(this->handle, &wr);
				::SetWindowPos(this->handle, 0, tmp.x+dx, 0, wr.right - wr.left, wr.bottom- wr.top, SWP_NOZORDER|SWP_NOACTIVATE);
				::SendMessage(::GetParent(this->handle), WM_LAYOUT, 0, 0);
				::UpdateWindow(::GetParent(this->handle));
			}
			break;
		}
		case WM_LBUTTONUP:
		{
			if (bTrack_)
			{
				::SendMessage(::GetParent(this->handle), WM_LAYOUT, 0, 0);
				::ReleaseCapture();
				::SetCursor(prevCursor_);
				bTrack_ = false;
			}
			break;
		}
		
		default: {
			break;
		}
		}
		return Window<Splitter>::wndProc(hWnd, message, wParam, lParam);
	}

}