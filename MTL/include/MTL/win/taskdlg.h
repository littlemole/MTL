#pragma once

#include "mtl/sdk.h"
#include "mtl/win/gdi.h"

namespace mtl {

	class task_dlg
	{
	public:

		HWND handle = nullptr;

		std::wstring mainTitle;
		std::wstring mainInstruction;
		std::wstring mainContent;
		std::wstring mainFooter;
		std::wstring verificationText;
		std::wstring expandedControlText;

		std::wstring expandedInformation;
		mtl::icon mainIcon;
		mtl::icon footerIcon;

		BOOL checked = false;
		int radio = 0;
		int button = 0;

		std::function<HRESULT(UINT id)> onButtonHandler;
		std::function<HRESULT(std::wstring)> onHyperlinkHandler;
		std::function<void()> onCreateHandler;
		std::function<void(bool)> onCheckedHandler;

		class task_button : public  _TASKDIALOG_BUTTON
		{
		public:

			std::wstring title;

			task_button()
			{
				this->nButtonID = 0;
				this->pszButtonText = 0;
			}

			task_button(int id, const std::wstring& str)
			{
				title = str;
				this->nButtonID = id;
				this->pszButtonText = title.c_str();
			}

			task_button(const task_button& rhs)
			{
				title = rhs.title;
				this->nButtonID = rhs.nButtonID;
				this->pszButtonText = title.c_str();
			}

			task_button(task_button&& rhs)
			{
				title = rhs.title;
				rhs.title = L"";
				rhs.pszButtonText = rhs.title.c_str();
				this->nButtonID = rhs.nButtonID;
				rhs.nButtonID = 0;
				this->pszButtonText = title.c_str();
			}

			task_button& operator=(const task_button& rhs)
			{
				if (this == &rhs)
				{
					return *this;
				}

				title = rhs.title;
				this->nButtonID = rhs.nButtonID;
				this->pszButtonText = title.c_str();

				return *this;
			}

			task_button& operator=(task_button&& rhs)
			{
				if (this == &rhs)
				{
					return *this;
				}

				title = rhs.title;
				rhs.title = L"";
				rhs.pszButtonText = rhs.title.c_str();

				this->nButtonID = rhs.nButtonID;
				rhs.nButtonID = 0;

				this->pszButtonText = title.c_str();
				return *this;
			}
		};

		std::vector<task_button> buttons_;
		std::vector<task_button> radio_buttons_;

		task_dlg()
		{
			::ZeroMemory(&taskdialog_, sizeof(taskdialog_));
			taskdialog_.cbSize = sizeof(taskdialog_);
			taskdialog_.hInstance = mtl::module_instance();

			taskdialog_.dwFlags = TDF_ENABLE_HYPERLINKS | TDF_ALLOW_DIALOG_CANCELLATION | TDF_SIZE_TO_CONTENT | TDF_USE_HICON_MAIN | TDF_USE_HICON_FOOTER | TDF_USE_COMMAND_LINKS;
			taskdialog_.dwCommonButtons = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
			taskdialog_.nDefaultButton = IDOK;
			taskdialog_.cButtons = 0;
			taskdialog_.cRadioButtons = 0;
			taskdialog_.cxWidth = 0;
			taskdialog_.nDefaultRadioButton = 0;
		}

		task_dlg& onCreate(std::function<void()> cb)
		{
			onCreateHandler = cb;
			return *this;
		}

		task_dlg& onClick(std::function<HRESULT(UINT)> cb)
		{
			onButtonHandler = cb;
			return *this;
		}

		task_dlg& onNavigate(std::function<HRESULT(std::wstring)> cb)
		{
			onHyperlinkHandler = cb;
			return *this;
		}


		task_dlg& onChecked(std::function<void(bool)> cb)
		{
			onCheckedHandler = cb;
			return *this;
		}

		task_dlg& wnd(HWND hWnd)
		{
			taskdialog_.hwndParent = hWnd;
			return *this;
		}

		task_dlg& flags(TASKDIALOG_FLAGS flags)
		{
			taskdialog_.dwFlags = flags;
			return *this;
		}

		DWORD flags()
		{
			return taskdialog_.dwFlags;
		}

		task_dlg& buttons(TASKDIALOG_COMMON_BUTTON_FLAGS  flags)
		{
			taskdialog_.dwCommonButtons = flags;
			return *this;
		}

		task_dlg& icon(int id)
		{
			mainIcon = ::LoadIcon(mtl::module_instance(), MAKEINTRESOURCE(id));
			return *this;
		}

		task_dlg& footer_icon(int id)
		{
			footerIcon = ::LoadIcon(mtl::module_instance(), MAKEINTRESOURCE(id));
			return *this;
		}


		task_dlg& title(const std::wstring& str)
		{
			mainTitle = str;
			return *this;
		}

		task_dlg& instruction(const std::wstring& str)
		{
			mainInstruction = str;
			return *this;
		}

		task_dlg& content(const std::wstring& str)
		{
			mainContent = str;
			return *this;
		}


		task_dlg& footer(const std::wstring& str)
		{
			mainFooter = str;
			return *this;
		}

		task_dlg& add_button(const task_button& but)
		{
			buttons_.push_back(but);
			return *this;
		}

		task_dlg& add_radio_button(const task_button& but)
		{
			radio_buttons_.push_back(but);
			return *this;
		}

		task_dlg& default_button(int id)
		{
			taskdialog_.nDefaultButton = id;
			return *this;
		}


		task_dlg& default_radio_button(int id)
		{
			taskdialog_.nDefaultRadioButton = id;
			return *this;
		}


		task_dlg& verification(const std::wstring& str)
		{
			verificationText = str;
			return *this;
		}


		task_dlg& expanded_title(const std::wstring& str)
		{
			expandedControlText = str;
			return *this;
		}

		task_dlg& expanded(const std::wstring& str)
		{
			expandedInformation = str;
			return *this;
		}

		void enable(UINT id)
		{
			::SendMessage(handle, TDM_ENABLE_BUTTON, (WPARAM)id, (LPARAM)1);
		}

		void disable(UINT id)
		{
			::SendMessage(handle, TDM_ENABLE_BUTTON, (WPARAM)id, (LPARAM)0);
		}

		int show()
		{
			taskdialog_.pszWindowTitle = mainTitle.c_str();
			taskdialog_.pszMainInstruction = mainInstruction.c_str();
			taskdialog_.pszContent = mainContent.c_str();
			taskdialog_.pszFooter = mainFooter.c_str();
			taskdialog_.hMainIcon = *mainIcon;
			taskdialog_.hFooterIcon = *footerIcon;
			taskdialog_.cButtons = (UINT)buttons_.size();
			taskdialog_.cRadioButtons = (UINT)radio_buttons_.size();
			taskdialog_.pButtons = 0;
			taskdialog_.pRadioButtons = 0;
			taskdialog_.pszExpandedInformation = expandedInformation.c_str();

			if (!taskdialog_.cButtons)
			{
				taskdialog_.dwFlags &= ~TDF_USE_COMMAND_LINKS;
			}

			std::vector< _TASKDIALOG_BUTTON> butts;

			if (!buttons_.empty())
			{
				for (auto& b : buttons_)
				{
					butts.push_back(b);
				}
				taskdialog_.pButtons = &butts[0];
			}

			std::vector< _TASKDIALOG_BUTTON> radio_butts;

			if (!radio_buttons_.empty())
			{
				for (auto& b : radio_buttons_)
				{
					radio_butts.push_back(b);
				}
				taskdialog_.pRadioButtons = &radio_butts[0];
			}

			taskdialog_.pszVerificationText = verificationText.c_str();
			taskdialog_.pszExpandedControlText = expandedControlText.c_str();
			taskdialog_.pszExpandedInformation = expandedInformation.c_str();

			taskdialog_.pfCallback = &task_dlg::callback;
			taskdialog_.lpCallbackData = (LONG_PTR)this;

			HRESULT hr = ::TaskDialogIndirect(&taskdialog_, &button, &radio, &checked);
			if (hr != S_OK) return IDCANCEL;

			return button;
		}

		~task_dlg()
		{}

		task_dlg(const task_dlg& rhs) = delete;

		task_dlg& operator = (const task_dlg& rhs) = delete;

	private:

		TASKDIALOGCONFIG taskdialog_;

		HRESULT wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			handle = hWnd;
			switch (msg)
			{
			case TDN_BUTTON_CLICKED:
			{
				int id = (int)wParam;
				if (onButtonHandler)
				{
					return onButtonHandler(id);
				}

				if (id == IDOK || id == IDCANCEL)
				{
					return S_OK;
				}
				return S_FALSE;
				break;
			}
			case TDN_HYPERLINK_CLICKED:
			{
				std::wstring url((wchar_t*)lParam);

				if (onHyperlinkHandler)
				{
					return onHyperlinkHandler(url);
				}
				return S_FALSE;
				break;
			}
			case TDN_CREATED:
			{
				if (onCreateHandler)
				{
					onCreateHandler();
				}
				break;
			}
			case TDN_VERIFICATION_CLICKED:
			{
				bool checked = (BOOL)wParam;
				if (onCheckedHandler)
				{
					onCheckedHandler(checked);
				}
				break;
			}
			}
			return S_OK;
		}


		static HRESULT callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR data)
		{
			task_dlg* that = (task_dlg*)data;
			return that->wndProc(hWnd, msg, wParam, lParam);
		}
	};


}