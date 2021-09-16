#pragma once

#include "MTL/punk.h"
#include "MTL/obj/impl.h"

namespace MTL {

	class EnumUnknown : public implements<EnumUnknown(IEnumUnknown)>
	{
	public:

		EnumUnknown(const std::vector<punk<IUnknown>>& v)
			: unks_(v)
		{}

		virtual HRESULT __stdcall Next(ULONG celt, IUnknown** rgelt, ULONG* pceltFetched) override
		{
			if (celt != 1)
				return E_INVALIDARG;

			if (rgelt == 0)
				return E_INVALIDARG;

			if (pceltFetched)
				*pceltFetched = 0;

			for (ULONG i = 0; i < celt; i++)
			{
				rgelt[i] = 0;
			}

			if (pos_ >= unks_.size())
			{
				return S_FALSE;
			}

			HRESULT hr = unks_[pos_].queryInterface(&(rgelt[0]));
			if (hr != S_OK)
				return hr;

			if (pceltFetched)
				*pceltFetched = 1;
			pos_++;
			return hr;
		}

		virtual HRESULT __stdcall Skip(ULONG celt) override
		{
			pos_ += celt;
			if (pos_ >= unks_.size())
				return S_FALSE;

			return S_OK;
		}

		virtual HRESULT __stdcall Reset(void) override
		{
			pos_ = 0;
			return S_OK;
		}

		virtual HRESULT __stdcall Clone(IEnumUnknown** ppenum) override
		{
			punk<IEnumUnknown> unk(new EnumUnknown(unks_));
			return unk.queryInterface(ppenum);
		}

	private:
		std::vector<punk<IUnknown>> unks_;
		int pos_ = 0;
	};

	class EnumVariant : public implements<EnumVariant(IEnumVARIANT)>
	{
	public:

		EnumVariant(const std::vector<variant>& v)
			: vars_(v)
		{}

		template<class T>
		EnumVariant(const std::vector<T>& v)
		{
			for (auto& it : v)
			{
				vars_.push_back(variant(it));
			}
		}

		virtual HRESULT __stdcall Next(ULONG celt, VARIANT* rgelt, ULONG* pceltFetched) override
		{
			if (celt != 1)
				return E_INVALIDARG;

			if (rgelt == 0)
				return E_INVALIDARG;

			if (pceltFetched)
				*pceltFetched = 0;

			if (pos_ >= vars_.size())
			{
				return S_FALSE;
			}

			vars_[pos_].copy_to(&(rgelt[0]));

			if (pceltFetched)
				*pceltFetched = 1;

			pos_++;
			return S_OK;
		}

		virtual HRESULT __stdcall Skip(ULONG celt) override
		{
			pos_ += celt;
			if (pos_ >= vars_.size())
				return S_FALSE;

			return S_OK;
		}

		virtual HRESULT __stdcall Reset(void) override
		{
			pos_ = 0;
			return S_OK;
		}

		virtual HRESULT __stdcall Clone(IEnumVARIANT** ppenum) override
		{
			punk<IEnumVARIANT> var(new EnumVariant(vars_));
			return var.queryInterface(ppenum);
		}

	private:
		std::vector<variant> vars_;
		int pos_ = 0;
	};


}