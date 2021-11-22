#pragma once

#include "mtl/sdk.h"
#include "mtl/punk.h"
#include "mtl/win/wnd.h"
//#include "mtl/obj/impl.h"
#include "mtl/ole/img.h"

#include <dwmapi.h>
//#include <shlwapi.h>

namespace mtl {

	namespace details {

		class taskbar_tab : public mtl::window<taskbar_tab>
		{
		public:

			std::wstring id;
			HWND owner = nullptr;
			HWND mainWnd = nullptr;

			mtl::event<void(std::wstring)> onClose;
			mtl::event<void(std::wstring)> onActivate;

			mtl::bitmap cache;

			taskbar_tab(HWND o, HWND m) : owner(o), mainWnd(m)
			{
				id = mtl::new_guid();
			}

			taskbar_tab(std::wstring key, HWND o, HWND m) : id(key), owner(o), mainWnd(m)
			{
			}

			virtual LRESULT wm_create() override
			{
				BOOL fForceIconic = TRUE;
				BOOL fHasIconicBitmap = TRUE;

				mtl::HR hr = ::DwmSetWindowAttribute(
					handle,
					DWMWA_FORCE_ICONIC_REPRESENTATION,
					&fForceIconic,
					sizeof(fForceIconic));

				hr = ::DwmSetWindowAttribute(
					handle,
					DWMWA_HAS_ICONIC_BITMAP,
					&fHasIconicBitmap,
					sizeof(fHasIconicBitmap));

				return 0;
			}

			LRESULT virtual wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				switch (message)
				{
				case WM_CREATE:
				{
					//mtl::timer::set_timeout(150, [this,hWnd](UINT_PTR) {
						if (::IsWindowVisible(owner))
						{
							RECT r;
							::GetClientRect(owner, &r);
							int w = r.right - r.left;
							int h = r.bottom - r.top;
							cache = mtl::screenshot(owner, w, h, true);
							DwmInvalidateIconicBitmaps(hWnd);
						}
					//});
					break;
				}
				case WM_ACTIVATE:
				{
					if (LOWORD(wParam) == WA_ACTIVE)
					{
						onActivate.fire(id);
						DwmInvalidateIconicBitmaps(hWnd);
						return 0;
					}
					break;
				}
				case WM_SYSCOMMAND:
				{
					if (wParam != SC_CLOSE)
					{
						LRESULT lResult = ::SendMessage(mainWnd, WM_SYSCOMMAND, wParam, lParam);
						return lResult;
					}
					else
					{
						onClose.fire(id);
						return 0;
					}
					break;
				}
				case WM_CLOSE:
				{
					onClose.fire(id);
					return 0;
					break;
				}
				case WM_DWMSENDICONICTHUMBNAIL:
				{
					int w = HIWORD(lParam);
					int h = LOWORD(lParam);

					RECT r;
					::GetClientRect(owner, &r);

					if (::IsWindowVisible(owner))
					{
						cache = mtl::screenshot(owner, r.right, r.bottom, true);
					}
					mtl::wnd_dc dc(owner);

					auto thumbnail = mtl::bitmap::make_transparent_dib_section(w, h);

					{
						mtl::compatible_dc cdc(*dc);
						cdc.select(*thumbnail);

						mtl::compatible_dc cdc_src(*dc);
						cdc_src.select(*cache);

						BITMAP bm;
						::GetObject(*cache, sizeof(bm), &bm);

						if (bm.bmWidth < w && bm.bmHeight < h)
						{
							::BitBlt(*cdc, 0, 0, bm.bmWidth, bm.bmHeight, *cdc_src, 0, 0, SRCCOPY);
						}
						else
						{
							::StretchBlt(*cdc, 0, 0, w, h, *cdc_src, 0, 0, w , h , SRCCOPY);
						}
					}

					::DwmSetIconicThumbnail(handle, *thumbnail, 0);
					return 0;
					break;
				}
				case WM_DWMSENDICONICLIVEPREVIEWBITMAP:
				{
					RECT r;
					::GetWindowRect(owner, &r);

					RECT wr;
					::GetClientRect(mainWnd, &wr);

					::AdjustWindowRect(&wr, WS_OVERLAPPED, TRUE);
					POINT pt = { wr.left,wr.top };
					::ClientToScreen(mainWnd, &pt);


					POINT ptOffset = { r.left - pt.x, r.top - pt.y };


					if (::IsWindowVisible(owner))
					{
						/*
						RECT r;
						::GetClientRect(owner, &r);

						POINT pt = { r.left,r.top };
						::ClientToScreen(owner, &pt);
						//::ScreenToClient(owner, &pt);

						RECT rm;
						::GetClientRect(mainWnd, &rm);
						POINT pt2 = { rm.left, rm.top };

						::ClientToScreen(mainWnd, &pt2);
						*/
						int w = r.right - r.left;
						int h = r.bottom - r.top;
						cache = mtl::screenshot(owner, w, h, true);

						//pt.x -= pt2.x;
						//pt.y =  pt.y - pt2.y;

						//pt.x = 0;
						//pt.y = 0;
						//cache = *bmp;
					}
					::DwmSetIconicLivePreviewBitmap(handle, *cache, &ptOffset, 1);// DWM_SIT_DISPLAYFRAME);

					return 0;
					break;
				}
				}
				return mtl::window<taskbar_tab>::wndProc(hWnd, message, wParam, lParam);
			}
		};

	}

	class taskbar
	{
	public:

		mtl::punk<ITaskbarList> taskBar;
		mtl::punk<ITaskbarList3> taskBar3;

		std::map<std::wstring, std::unique_ptr<details::taskbar_tab>> tabs;

		mtl::event<void(std::wstring)> onClose;
		mtl::event<void(std::wstring)> onActivate;

		HWND& mainWnd;

		taskbar(HWND& main)
			: mainWnd(main)
		{
			mtl::HR hr = taskBar.create_object(CLSID_TaskbarList);
			hr = taskBar->HrInit();
			hr = taskBar.query_interface(&taskBar3);
		}

		taskbar& activate(std::wstring id)
		{
			if (tabs.count(id))
			{
				taskBar3->ActivateTab(tabs[id]->handle);
			}
			return *this;
		}

		taskbar& remove(std::wstring id)
		{
			taskBar3->UnregisterTab(tabs[id]->handle);
			tabs[id]->destroy();
			tabs.erase(id);
			return *this;
		}

		bool exists(const std::wstring& id)
		{
			return tabs.count(id);
		}

		details::taskbar_tab* tab(const std::wstring& id)
		{
			if (exists(id))
			{
				return tabs[id].get();
			}
			return nullptr;
		}

		HWND hWnd(const std::wstring& id)
		{
			if (exists(id))
			{
				return  tabs[id]->owner;
			}
			return nullptr;
		}

		void set_text(std::wstring id, const std::wstring& txt)
		{
			if (tabs.count(id))
			{
				::SetWindowText(tabs[id]->handle, txt.c_str());
			}
		}

		void add(std::wstring id, HWND tab, const std::wstring& txt)
		{
			add(id, tab);
			set_text(id, txt);
		}

		void add(std::wstring id, HWND tab)
		{
			mtl::wbuff buf(256);
			::GetWindowText(tab, buf, (int)buf.size());

			details::taskbar_tab* tw = new details::taskbar_tab(id, tab, mainWnd);

			RECT r = { -32000 ,-32000 ,10,10 };
			tw->create(0, buf, 0, r, WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);

			tw->onActivate([this, tw](std::wstring id)
			{
				taskBar3->ActivateTab(tw->handle);
				onActivate.fire(id);
			});
			tw->onClose([this](std::wstring id)
			{
				onClose.fire(id);
			});

			tabs[tw->id] = std::unique_ptr<details::taskbar_tab>(tw);
			taskBar3->RegisterTab(tw->handle, mainWnd);
			taskBar3->SetTabOrder(tw->handle, nullptr);
		}

	};

}