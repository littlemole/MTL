// helloworld.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "helloRibbon.h"
#include "mtl/persist/json.h"
#include "mtl/win32/io.h"

#include <uiribbon.h>
#include "ribbon.h"

namespace mtl {

	class decimal : public DECIMAL
	{
	public:
		decimal()
		{
			::ZeroMemory(this, sizeof(this));
		}

		decimal(const DECIMAL& rhs)
		{
			::ZeroMemory(this, sizeof(this));
			*(DECIMAL*)this = rhs;
		}

		decimal(double val)
		{
			::VarDecFromR8(val, this);
		}

		int integer(int prec = 1)
		{
			return (int)number();
		}

		double number()
		{
			double result = 0.0;
			::VarR8FromDec(this, &result);
			return result;
		}
	};

	class prop_var : public PROPVARIANT
	{
	public:
		prop_var()
		{
			::PropVariantInit(this);
		}

		~prop_var()
		{
			::PropVariantClear(this);
		}

		prop_var(const prop_var& rhs)
		{
			::PropVariantInit(this);
			::PropVariantCopy(this, (PROPVARIANT*)&rhs);
		}

		prop_var(prop_var&& rhs)
		{
			::PropVariantInit(this);
			if (rhs.vt != VT_EMPTY)
			{
				*(PROPVARIANT*)this = (PROPVARIANT&&)rhs;
				rhs.vt = VT_EMPTY;
				rhs.uintVal = 0;
			}
		}

		prop_var& operator=(const prop_var& rhs)
		{
			if (this == &rhs)
			{
				return *this;
			}
			::PropVariantClear(this);
			::PropVariantCopy(this, (PROPVARIANT*)&rhs);

			return *this;
		}

		prop_var& operator=(prop_var&& rhs)
		{
			if (this == &rhs)
			{
				return *this;
			}
			::PropVariantClear(this);
			if (rhs.vt != VT_EMPTY)
			{
				*(PROPVARIANT*)this = (PROPVARIANT&&)rhs;
				rhs.vt = VT_EMPTY;
				rhs.uintVal = 0;
			}
			return *this;
		}

		prop_var(UINT32 value)
		{
			::PropVariantInit(this);
			vt = VT_UI4;
			uintVal = value;
		}

		prop_var(INT32 value)
		{
			::PropVariantInit(this);
			vt = VT_I4;
			intVal = value;
		}

		prop_var(const std::wstring& value)
		{
			::PropVariantInit(this);
			vt = VT_LPWSTR;
			::SHStrDup(value.c_str(), &pwszVal);
		}

		prop_var(const wchar_t* value)
		{
			::PropVariantInit(this);
			vt = VT_LPWSTR;
			::SHStrDup(value, &pwszVal);
		}


		prop_var(VARIANT_BOOL value)
		{
			::PropVariantInit(this);
			vt = VT_BOOL;
			boolVal = value;
		}

		prop_var(bool value)
		{
			::PropVariantInit(this);
			vt = VT_BOOL;
			boolVal = value ? VARIANT_TRUE : VARIANT_FALSE;
		}

		prop_var(DECIMAL dec)
		{
			::PropVariantInit(this);
			decVal = dec;
			vt = VT_DECIMAL; // order important
		}

	};


	class UIImage : public mtl::implements<UIImage(IUIImage)>
	{
	public:

		UINT32 img_id = 0;
		std::wstring img_path;

		UIImage() {}

		UIImage(UINT32 id) : img_id(id) {}
		UIImage(const std::wstring& p) : img_path(p) {}

		// Inherited via implements
		virtual HRESULT __stdcall GetBitmap(HBITMAP* bitmap) override
		{
			if (img_id)
			{
				*bitmap = mtl::the_bitmap_cache().get(img_id);
				if (!*bitmap) return S_FALSE;
			}
			else if (!img_path.empty())
			{
				*bitmap = mtl::the_bitmap_cache().get(img_path.c_str());
				if (!*bitmap) return S_FALSE;
			}
			return S_OK;
		}

	};

	class prop_set : public mtl::implements<prop_set(IUISimplePropertySet)>
	{
	public:

		std::wstring label;
		UINT32 catId = UI_COLLECTION_INVALIDINDEX;
		UINT32 cmdType = UI_COMMANDTYPE_UNKNOWN;
		UINT32 cmdId = -1;
		UINT32 pinned = 0;
		UINT32 img_id = 0;
		std::wstring img_path;

		prop_set() {}

		prop_set(const std::wstring& l) : label(l) {}
		prop_set(const std::wstring& l, UINT32 id) : label(l), cmdId(id) {}
		prop_set(const std::wstring& l, UINT32 id, UINT32 img, std::wstring path) : label(l), cmdId(id), img_id(img), img_path(path) {}


		// Inherited via implements
		virtual HRESULT __stdcall GetValue(REFPROPERTYKEY key, PROPVARIANT* value) override
		{
			if (key == UI_PKEY_ItemImage)
			{
				if (img_id)
				{
					mtl::punk<IUIImage> img(new UIImage(img_id));
					value->vt = VT_UNKNOWN;
					return img.query_interface(&value->punkVal);
				}
				else if (!img_path.empty())
				{
					mtl::punk<IUIImage> img(new UIImage(img_path));
					value->vt = VT_UNKNOWN;
					return img.query_interface(&value->punkVal);
				}
				return S_FALSE;
			}
			else if (key == UI_PKEY_Label)
			{
				::PropVariantInit(value);
				value->vt = VT_BSTR;
				value->bstrVal = ::SysAllocString(label.c_str());
				return S_OK;
			}
			else if (key == UI_PKEY_CategoryId)
			{
				::PropVariantInit(value);
				value->vt = VT_UI4;
				value->uintVal = catId;
				return S_OK;
			}
			else if (key == UI_PKEY_CommandId)
			{
				if (cmdId != -1)
				{
					::PropVariantInit(value);
					value->vt = VT_UI4;
					value->uintVal = cmdId;
					return S_OK;
				}
				return S_FALSE;
			}
			else if (key == UI_PKEY_CommandType)
			{
				::PropVariantInit(value);
				value->vt = VT_UI4;
				value->uintVal = cmdType;
				return S_OK;
			}
			else if (key == UI_PKEY_Pinned)
			{
				::PropVariantInit(value);
				value->vt = VT_UI4;
				value->uintVal = pinned;
				return S_OK;
			}
			return E_FAIL;
		}

	};

	struct guid_comparer
	{
		bool operator()(const GUID& Left, const GUID& Right) const
		{
			// comparison logic goes here
			return memcmp(&Left, &Right, sizeof(Right)) < 0;
		}
	};

	namespace details {


		class ribbon_cmd_hamndler : public mtl::implements<ribbon_cmd_hamndler(IUICommandHandler)>
		{
		public:

			mtl::event<UINT32(const PROPERTYKEY&, const PROPVARIANT&)> onCommand;
			mtl::event<UINT32()> onCmd;

			std::map< UINT32, std::map<GUID, mtl::prop_var, mtl::guid_comparer>> values;

			// Inherited via implements
			virtual HRESULT __stdcall Execute(UINT32 commandId, UI_EXECUTIONVERB verb, const PROPERTYKEY* key, const PROPVARIANT* currentValue, IUISimplePropertySet* commandExecutionProperties) override
			{
				if (verb == UI_EXECUTIONVERB_EXECUTE)
				{
					onCmd.fire(commandId);
					onCommand.fire(commandId, *key, *currentValue);
				}
				return S_OK;
			}

			virtual HRESULT __stdcall UpdateProperty(UINT32 commandId, REFPROPERTYKEY key, const PROPVARIANT* currentValue, PROPVARIANT* newValue) override
			{
				if (values.count(commandId))
				{
					auto& vals = values[commandId];
					if (vals.count(key.fmtid))
					{
						auto& val = vals[key.fmtid];
						HRESULT hr = ::PropVariantCopy(newValue, &val);
						OutputDebugString(L">>>>>>>>>>>>>>>>>>>>>>>>>>>");
						return hr;
					}
				}
				::PropVariantCopy(newValue, currentValue);
				return S_OK;
			}

		};

		class ribbon_application : public mtl::implements< ribbon_application(IUIApplication)>
		{
		public:

			UINT ulRibbonHeight = 0;

			mtl::punk<ribbon_cmd_hamndler> commandHandler;

			ribbon_application()
			{
				commandHandler = new ribbon_cmd_hamndler();
			}

			// Inherited via implements
			virtual HRESULT __stdcall OnViewChanged(UINT32 viewId, UI_VIEWTYPE typeID, IUnknown* view, UI_VIEWVERB verb, INT32 uReasonCode) override
			{
				mtl::punk<IUIRibbon> ribbon(view);

				if (!ribbon)
					return E_FAIL;

				mtl::HR hr = ribbon->GetHeight(&ulRibbonHeight);

				return S_OK;
			}

			virtual HRESULT __stdcall OnCreateUICommand(UINT32 commandId, UI_COMMANDTYPE typeID, IUICommandHandler** cmdHandler) override
			{
				return commandHandler.query_interface(cmdHandler);
				//		return S_OK;
			}

			virtual HRESULT __stdcall OnDestroyUICommand(UINT32 commandId, UI_COMMANDTYPE typeID, IUICommandHandler* commandHandler) override
			{
				return E_NOTIMPL;
			}

		};
	}

	class ribbon
	{
	public:

		struct item {
			UINT32 id = 0;
			std::wstring label;
			UINT32 img_id = 0;
			std::wstring img_path;
		};

		mtl::punk<IUIFramework> uiFramework;
		mtl::punk<details::ribbon_application> uiApplication;
		HWND mainWnd = nullptr;

		ribbon()
		{

		}

		UINT height()
		{
			return uiApplication->ulRibbonHeight;
		}

		ribbon& init(HWND hWnd)
		{
			mainWnd = hWnd;

			mtl::HR hr = uiFramework.create_object(CLSID_UIRibbonFramework, CLSCTX_INPROC_SERVER);

			uiApplication = new details::ribbon_application();

			hr = uiFramework->Initialize(hWnd, (IUIApplication*)*uiApplication);

			return *this;
		}

		ribbon& load()
		{
			mtl::HR hr = uiFramework->LoadUI( mtl::module_instance(), L"APPLICATION_RIBBON");
			return *this;
		}

		ribbon& mode(UINT m)
		{
			uiFramework->SetModes(UI_MAKEAPPMODE(0) | UI_MAKEAPPMODE(m));
			return *this;
		}

		ribbon& value(UINT32 id, const PROPERTYKEY& key, mtl::prop_var pv)
		{
			uiApplication->commandHandler->values[id][key.fmtid] = pv;
			return *this;
		}

		ribbon& set(UINT32 id, const PROPERTYKEY& key, mtl::prop_var pv)
		{
			mtl::HR hr = uiFramework->SetUICommandProperty(id, key, pv);
			return *this;
		}

		ribbon& add(UINT32 id, const std::vector<item>& items )
		{
			mtl::prop_var src = get(id, UI_PKEY_ItemsSource);
			if (src.vt = VT_UNKNOWN)
			{
				mtl::punk<IUICollection> source(src.punkVal);
				if (source)
				{
					for (auto& item : items)
					{
						mtl::punk<mtl::prop_set> pp1( new mtl::prop_set( item.label, item.id, item.img_id, item.img_path ));
						mtl::HR hr = source->Add(*pp1);
					}
				}
			}
			return *this;
		}

		ribbon& clear(UINT32 id)
		{
			mtl::prop_var src = get(id, UI_PKEY_ItemsSource);
			if (src.vt = VT_UNKNOWN)
			{
				mtl::punk<IUICollection> source(src.punkVal);
				if (source)
				{
					source->Clear();
				}
			}
			return *this;
		}

		UINT32 count(UINT32 id)
		{
			mtl::prop_var src = get(id, UI_PKEY_ItemsSource);
			if (src.vt = VT_UNKNOWN)
			{
				mtl::punk<IUICollection> source(src.punkVal);
				if (source)
				{
					UINT32 result = 0;
					mtl::HR hr = source->GetCount(&result);
					return result;
				}
			}
			return -1;
		}

		ribbon& select(UINT32 id, UINT32 index)
		{
			set(id, UI_PKEY_SelectedItem, index);
			return *this;
		}

		UINT32 selected(UINT32 id)
		{
			mtl::prop_var src = get(id, UI_PKEY_SelectedItem);
			if (src.vt = VT_UI4)
			{
				return src.uintVal;
			}
			return -1;
		}

		//uiFramework->SetUICommandProperty(ID_CMD_SYNTAX, UI_PKEY_SelectedItem, pv4);


		mtl::prop_var get(UINT32 id, const PROPERTYKEY& key)
		{
			mtl::prop_var result;
			mtl::HR hr = uiFramework->GetUICommandProperty(id, key, &result);
			return result;
		}

		UINT32 cmd(UINT32 id)
		{
			UINT32 sel = selected(id);
			if (sel == -1) return -1;

			mtl::prop_var src = get(id, UI_PKEY_ItemsSource);
			if (src.vt = VT_UNKNOWN)
			{
				mtl::punk<IUICollection> source(src.punkVal);
				if (source)
				{
					mtl::punk<IUnknown> unk;
					mtl::HR hr = source->GetItem(sel, &unk);
					if (unk)
					{
						mtl::punk<IUISimplePropertySet> sps(unk);
						if (sps)
						{
							mtl::prop_var result;
							hr = sps->GetValue(UI_PKEY_CommandId, &result);
							if (result.vt == VT_UI4)
							{
								return result.uintVal;
							}
						}
					}
				}
			}
			return -1;
		}

		ribbon& on(UINT32 id, std::function<void()> cb)
		{
			uiApplication->commandHandler->onCmd(id, cb);
			return *this;
		}

		ribbon& onRibbon(UINT32 id, std::function<void(const PROPERTYKEY& key, const PROPVARIANT& value)> cb)
		{
			uiApplication->commandHandler->onCommand(id, cb);
			return *this;
		}


		bool boolean(UINT32 id)
		{
			mtl::prop_var result = get(id, UI_PKEY_BooleanValue);
			if (result.vt = VT_BOOL)
			{
				return result.boolVal == VARIANT_TRUE ? true : false;
			}
			return false;
		}

		mtl::decimal decimal_val(UINT32 id)
		{
			mtl::prop_var value = get(id, UI_PKEY_DecimalValue);

			mtl::decimal d = value.decVal;
			return d;
		}

		ribbon& layout(UINT32 id, const std::wstring& format)
		{
			value(id, UI_PKEY_RepresentativeString, format.c_str() );
			return *this;
		}
	};
}

class AboutDlg : public mtl::dialog
{
public:
	/*
	mtl::dlg_value< IDC_CHECK_LOVE, bool> hasLove;

	mtl::dlg_value< IDC_LIST_LOVE, std::vector<std::wstring>> kindValues;
	mtl::dlg_selection< IDC_LIST_LOVE, std::wstring> loveKind;

	mtl::dlg_value< IDC_COMBO_LOVE, std::vector<std::wstring>> artValues;
	mtl::dlg_selection< IDC_COMBO_LOVE, std::wstring> loveArt;
	*/
	AboutDlg()
	{
		/*
		hasLove = true;
		kindValues = { L"a bit", L"some what", L"yuh cool", L"ETERNAL LOVE" };
		artValues = { L"Red", L"Green", L"Blue" };

		binding(
			hasLove,
			kindValues,
			loveKind,
			artValues,
			loveArt
		);
		*/
	}

};










class ContentWindow : public mtl::window<ContentWindow>
{
public:

	mtl::button butt;

	virtual LRESULT wm_create() override
	{
		// create chld button
		RECT r = { 0,0,120,40 };
		butt.create(IDC_BUTT, L"click me", handle, r, WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON);

		butt.onCommand(IDC_BUTT, [this]()
		{
			::MessageBox(handle, L"click", L"clack", 0);
		});

		return 0;
	}

	virtual LRESULT wm_draw(HDC hdc, RECT& bounds) override
	{
		HBITMAP bmp = mtl::gui().bitmap(IDI_JPEG);// , 500, 314);

		mtl::dc dc(hdc);
		dc.bit_blit(bmp, 0, 0);

		HBITMAP bmp2 = mtl::gui().bitmap(ID_CMD_EXIT);// , 500, 314);

		dc.alpha_blend(bmp2, 200, 200,64,64);

		return 0;
	}

	virtual LRESULT wm_size(RECT& clientRect) override
	{
		RECT r = { 0,0,0,0 };
		//		do nothing here
		//		layout.do_layout(clientRect, r);
		return 0;
	}

	virtual LRESULT wm_destroy() override
	{
		// over and out
		return 0;
	}

};


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

		taskdialog_.dwFlags = TDF_ENABLE_HYPERLINKS | TDF_ALLOW_DIALOG_CANCELLATION | TDF_SIZE_TO_CONTENT| TDF_USE_HICON_MAIN| TDF_USE_HICON_FOOTER| TDF_USE_COMMAND_LINKS;
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

	HRESULT wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{
		handle = hWnd;
		switch (msg)
		{
			case TDN_BUTTON_CLICKED : 
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


	static HRESULT callback( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR data)
	{
		task_dlg* that = (task_dlg*)data;
		return that->wndProc(hWnd, msg, wParam, lParam);
	}
};



class MainWindow : public mtl::window<MainWindow>
{
public:

	mtl::menu	menu;
	AboutDlg aboutDlg;
	ContentWindow contentWindow;

	mtl::default_layout	layout;
	mtl::ribbon ribbon;


	MainWindow()
	{
		mtl::the_bitmap_cache().load(IDI_JPEG, CLSID_WICJpegDecoder, L"JPEG");

		create(L"Hello World", WS_OVERLAPPEDWINDOW| WS_CLIPCHILDREN|WS_CLIPSIBLINGS,0,*menu);
		show();

	}

	virtual LRESULT wm_create() override
	{
		RECT cRect = client_rect();
		HWND hWnd = contentWindow.create(IDC_CONTENT, L"", handle, cRect, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS );

		layout = {							// Default Layout with a
			{										//  vector<Widget> of
/*				{									//   Widget holding a
					*statusBar,						//    statusbar
					mtl::layout::style::SOUTH		//    docked to SOUTH
				},									//  and
				*/
				{	
					*contentWindow,
					mtl::layout::style::FILL		//   filling remaining space
				}
			}
		};

		ribbon
			.init(handle)
			.value(ID_CMD_ZOOM, UI_PKEY_MinValue, mtl::decimal(1.0))//, 1))
			.value(ID_CMD_ZOOM, UI_PKEY_DecimalPlaces, 1)
			.layout(ID_CMD_SYNTAX, L"XXXXXXXXXXX")
			.load()
			.add(ID_CMD_SYNTAX, {
				{ 1, L"plain", ID_CMD_PLAIN },
				{ 2, L"html",0, L"img\\html-file.png"},
				{ 3, L"javascript" }
			})
			.select(ID_CMD_SYNTAX, 0);


		ribbon
			.on(ID_CMD_EXIT, [this] ()
			{
				::PostQuitMessage(0); 
			})
			.on(ID_CMD_NEW, [this] ()
			{
				if (ribbon.boolean(ID_CMD_NEW))
				{
					::MessageBox(handle, L"checked", L"toggle", 0);
				}
			})
			.on(ID_CMD_ZOOM, [this] () 
			{
				mtl::decimal d = ribbon.decimal_val(ID_CMD_ZOOM);

				double zoomValue = 0;
				::VarR8FromDec( &d, &zoomValue);
			
				std::wostringstream woss;
				woss << L"Zoom: " << zoomValue << L" " << d.Lo64;
				set_text(woss.str());
			})
			.on(ID_CMD_SYNTAX, [this] ()
			{
				UINT32 sel = ribbon.selected(ID_CMD_SYNTAX);
				if (sel != -1)
				{
					UINT32 cmd = ribbon.cmd(ID_CMD_SYNTAX);
					std::wostringstream woss;
					woss << L"Syntax: " << sel << " [" << cmd << "]";
					::MessageBox(handle, woss.str().c_str(), L"syntax", 0);
				}
			});

		return 0;
	}

	virtual LRESULT wm_command(int id) override
	{
		switch (id)
		{
			case IDM_ABOUT:
			{
				// show about dialog
				LRESULT r = aboutDlg.show_modal(IDD_ABOUTBOX, handle);
				if (r == IDOK)
				{
					int result = IDCANCEL;
					//HRESULT hr = ::TaskDialog( handle, mtl::module_instance(), L"About", L"Main Instruction", L"Content",
						//TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON, MAKEINTRESOURCE(IDI_SMALL), &result);

					task_dlg dlg;

					if (dlg
						.wnd(handle)
						.buttons(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON)
						.title(L"About")
						.instruction(L"My instruction")
						.content(L"CONTENT with a <a href=\"somewhere.com\">ref</a> to the void.")
						.add_radio_button({ 101, L"one"})
						.add_radio_button({ 102, L"two"})
						.add_button( {201, L"BUTT"})
						.verification(L"verify me")
						.expanded_title(L"expand details") 
						.expanded(L"o-hu")
						.icon(IDI_SMALL)
						.footer(L"footer")
						.footer_icon(IDI_SMALL)
						.onNavigate( [this](std::wstring target){
						::MessageBox(handle,target.c_str(), L"LINK", 0);
							return S_FALSE;
						})
						.onCreate([&dlg]() {
							dlg.disable(IDOK);
							dlg.disable(201);
						})
						.onChecked( [&dlg](bool checked) 
						{
							if (checked)
							{
								dlg.enable(IDOK);
								dlg.enable(201);
							}
						})
						.onClick([this](UINT id) {

							::MessageBox(handle, L"BUTT", L"OK", 0);
							return S_OK;
						})
						.show() != IDCANCEL)
					{
						std::wostringstream woss;
						woss << L"button " << dlg.button << L" radio (" << dlg.radio << L") [" << dlg.checked << L"]";
						::MessageBox(handle, woss.str().c_str(), L"OK", 0);
					}
					/*
					if (aboutDlg.hasLove)
					{
						std::wstring love = aboutDlg.loveKind;
						std::wstring art = aboutDlg.loveArt;

						std::wstringstream woss;
						woss << "I love you " << art << L" you too";
						::MessageBox(handle, woss.str().c_str(), love.c_str(),0);
					}
					*/
				}
				break;
			}
			case IDM_EXIT:
			{
				// kill ourselves
				destroy();
				break;
			}
		}

		return 0;
	}

	virtual LRESULT wm_size(RECT& clientRect) override
	{
		RECT r = { 0,0,0,0 };

		UINT ribbonHeight = ribbon.height();//
		//uiApplication->ulRibbonHeight;
		clientRect.top += ribbonHeight;

		layout.do_layout(clientRect, r);

		return 0;
	}

	virtual LRESULT wm_destroy() override
	{
		// over and out
		::PostQuitMessage(0);

		return 0;
	}
};


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    mtl::STA enter;

    mtl::application app(hInstance);

	MainWindow mainWnd;

	mtl::accelerators acc(*mainWnd);
	acc.create({ { 'T', IDM_ABOUT, FCONTROL | FVIRTKEY } });
	mtl::accelerator() = acc;
	return app.run(mtl::accelerator());

}


