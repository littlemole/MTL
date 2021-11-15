#pragma once

#include "mtl/sdk.h"
#include "mtl/win/gdi.h"
#include "mtl/obj/impl.h"
#include "mtl/ole/img.h"
#include <uiribbon.h>
#include <shlwapi.h>

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

	namespace details {



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

		class ribbon_cmd_hamndler : public mtl::implements<ribbon_cmd_hamndler(IUICommandHandler)>
		{
		public:

			mtl::event<UINT32(const PROPERTYKEY&, const PROPVARIANT&)> onCommand;
			mtl::event<UINT32()> onCmd;

			std::map< UINT32, std::map<GUID, prop_var, guid_comparer>> values;

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
		{}

		// init step 1 - pass main window handle
		ribbon& init(HWND hWnd)
		{
			mainWnd = hWnd;

			mtl::HR hr = uiFramework.create_object(CLSID_UIRibbonFramework, CLSCTX_INPROC_SERVER);

			uiApplication = new details::ribbon_application();

			hr = uiFramework->Initialize(hWnd, (IUIApplication*)*uiApplication);

			return *this;
		}

		// init step 2 - set default value, can be called multiple times
		ribbon& value(UINT32 id, const PROPERTYKEY& key, details::prop_var pv)
		{
			uiApplication->commandHandler->values[id][key.fmtid] = pv;
			return *this;
		}

		// init step 3 - set default layout string for dropdowns etc, can be called multiple times
		ribbon& layout(UINT32 id, const std::wstring& format)
		{
			value(id, UI_PKEY_RepresentativeString, format.c_str());
			return *this;
		}

		// init step 4 - load ribbon from embedded binary markup
		ribbon& load()
		{
			mtl::HR hr = uiFramework->LoadUI(mtl::module_instance(), L"APPLICATION_RIBBON");
			return *this;
		}

		// get ribbon height
		UINT height()
		{
			return uiApplication->ulRibbonHeight;
		}

		// set ribbon application mode
		ribbon& mode(UINT m)
		{
			uiFramework->SetModes(UI_MAKEAPPMODE(0) | UI_MAKEAPPMODE(m));
			return *this;
		}

		// set a ribbon property
		ribbon& set(UINT32 id, const PROPERTYKEY& key, details::prop_var pv)
		{
			mtl::HR hr = uiFramework->SetUICommandProperty(id, key, pv);
			return *this;
		}

		// add items to a dropdown or gallery
		ribbon& add(UINT32 id, const std::vector<item>& items)
		{
			details::prop_var src = get(id, UI_PKEY_ItemsSource);
			if (src.vt = VT_UNKNOWN)
			{
				mtl::punk<IUICollection> source(src.punkVal);
				if (source)
				{
					for (auto& item : items)
					{
						mtl::punk<details::prop_set> pp1(new details::prop_set(item.label, item.id, item.img_id, item.img_path));
						mtl::HR hr = source->Add(*pp1);
					}
				}
			}
			return *this;
		}

		// clear a dropdown or gallery
		ribbon& clear(UINT32 id)
		{
			details::prop_var src = get(id, UI_PKEY_ItemsSource);
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

		// count items in a drodown or gallery
		UINT32 count(UINT32 id)
		{
			details::prop_var src = get(id, UI_PKEY_ItemsSource);
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

		// select and empty in a dropdown or gallery
		ribbon& select(UINT32 id, UINT32 index)
		{
			set(id, UI_PKEY_SelectedItem, index);
			return *this;
		}

		// get the selected index of a dropdown or gallery
		UINT32 selected(UINT32 id)
		{
			details::prop_var src = get(id, UI_PKEY_SelectedItem);
			if (src.vt = VT_UI4)
			{
				return src.uintVal;
			}
			return -1;
		}

		// get a variant value of a ribbon control
		details::prop_var get(UINT32 id, const PROPERTYKEY& key)
		{
			details::prop_var result;
			mtl::HR hr = uiFramework->GetUICommandProperty(id, key, &result);
			return result;
		}

		// for a dropdown or gallery, get the cmd id (not the index) set for the item
		UINT32 cmd(UINT32 id)
		{
			UINT32 sel = selected(id);
			if (sel == -1) return -1;

			details::prop_var src = get(id, UI_PKEY_ItemsSource);
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
							details::prop_var result;
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

		// set a ribbon command handler (click, push, select, etc) to be invoked for the control id specified
		ribbon& on(UINT32 id, std::function<void()> cb)
		{
			uiApplication->commandHandler->onCmd(id, cb);
			return *this;
		}

		// ex version of above, will pass parameters as PROPVARIANTS
		ribbon& onRibbon(UINT32 id, std::function<void(const PROPERTYKEY& key, const PROPVARIANT& value)> cb)
		{
			uiApplication->commandHandler->onCommand(id, cb);
			return *this;
		}

		// get boolen state for checkboxes or push-buttons
		bool boolean(UINT32 id)
		{
			details::prop_var result = get(id, UI_PKEY_BooleanValue);
			if (result.vt = VT_BOOL)
			{
				return result.boolVal == VARIANT_TRUE ? true : false;
			}
			return false;
		}

		// get decimal value from control, for example spinner
		decimal decimal_val(UINT32 id)
		{
			details::prop_var value = get(id, UI_PKEY_DecimalValue);

			decimal d = value.decVal;
			return d;
		}

	};

}
