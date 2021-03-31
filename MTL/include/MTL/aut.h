#pragma once

#include <Windows.h>
#include <OleAuto.h>

#include <vector>
#include "MTL/punk.h"

namespace MTL {

	//////////////////////////////////////////////////////////////////////////////
	// IDispatch invocation helper
	//////////////////////////////////////////////////////////////////////////////

	class disp : public DISPPARAMS
	{
	public:

		disp(const IDispatch* disp)
			: disp_(disp), id_(DISPID_VALUE)
		{
			this->cArgs = 0;
			this->cNamedArgs = 0;
			this->rgdispidNamedArgs = 0;
			this->rgvarg = 0;
		}

		disp(const IDispatch* disp, DISPID id)
			: disp_(disp), id_(id)
		{
			this->cArgs = 0;
			this->cNamedArgs = 0;
			this->rgdispidNamedArgs = 0;
			this->rgvarg = 0;
		}

		disp(const IDispatch* disp, const std::wstring& member)
			: disp_(disp), id_(0)
		{
			this->cArgs = 0;
			this->cNamedArgs = 0;
			this->rgdispidNamedArgs = 0;
			this->rgvarg = 0;

			const wchar_t* names = member.c_str();
			HR hr = disp_->GetIDsOfNames(IID_NULL, (LPOLESTR*)&names, 1, LOCALE_USER_DEFAULT, &id_);
		}

		disp& member(DISPID id)
		{
			id_ = id;
			return *this;
		}

		disp& member(const std::wstring& member)
		{
			const wchar_t* names = member.c_str();
			HR hr = disp_->GetIDsOfNames(IID_NULL, (LPOLESTR*)&names, 1, LOCALE_USER_DEFAULT, &id_);
			return *this;
		}
		
		template<class ...Args>
		variant invoke(Args ... args)
		{
			std::vector<variant> params_{ variant(args)... };

			this->cArgs = (UINT)params_.size();
			if (this->cArgs != 0)
			{
				this->rgvarg = &params_[0];
			}

			variant result;
			HR hr = disp_->Invoke(id_, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, this, &result, 0, 0);

			return result;
		}
		
		variant get(DISPID id = -1)
		{
			if (id == -1) id = id_;
			DISPPARAMS dp{ 0, 0,0,0 };
			variant result;
			HR hr = disp_->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &dp, &result, 0, 0);
			return result;
		}

		variant put(variant v)
		{
			DISPPARAMS dp{ 0, 0,0,0 };
			dp.cArgs = 1;
			dp.rgvarg = &v;

			variant result;
			HR hr = disp_->Invoke(id_, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, &dp, &result, 0, 0);
			return result;
		}

		variant putref(VARIANT* v)
		{
			DISPPARAMS dp{ 0, 0,0,0 };
			dp.cArgs = 1;
			dp.rgvarg = v;

			variant result;
			HR hr = disp_->Invoke(id_, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUTREF, &dp, &result, 0, 0);
			return result;
		}

	private:
		punk<IDispatch> disp_;
		DISPID id_;
	};

}

