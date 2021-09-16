#pragma once

#include "MTL/sdk.h"

namespace MTL {

	template<class T>
	class ClassObject;

	namespace detail {

		template<class T>
		class GetClassObject;

		template< class T>
		class derives;

		template<class T>
		class interfaces;

		template<class T>
		class Registrar;
	}

	template<class T>
	class implements;

	template<class T>
	class dispatch;

	template<class T>
	class aggregate;

	//////////////////////////////////////////////////////////////////////////////
	// ComDLL base class that exposes class factories
	//////////////////////////////////////////////////////////////////////////////

	class ComDLL
	{
	public:

		virtual BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) = 0;
		virtual HRESULT __stdcall DllGetClassObject(const CLSID& rclsid, const IID& riid, void** ppv) = 0;
		virtual HRESULT __stdcall DllRegisterServer(void) = 0;
		virtual HRESULT __stdcall DllUnregisterServer(void) = 0;
		virtual HRESULT __stdcall DllCanUnloadNow(void) = 0;
	};

} // end namespace MTL

