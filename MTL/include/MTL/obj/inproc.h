#pragma once

#include "MTL/obj/obj.h"

extern MTL::ComDLL& inprocServer();

namespace MTL {

	//////////////////////////////////////////////////////////////////////////////
	// Class Factory implementations
	//////////////////////////////////////////////////////////////////////////////

	namespace details {

		template<class T>
		class GetClassObject;

		template<>
		class GetClassObject<void()>
		{
		public:

			static HRESULT __stdcall DllGetClassObject(const CLSID& rclsid, const IID& riid, void** ppv)
			{
				return REGDB_E_CLASSNOTREG;
			}
		};

		template<class T, class ...Args>
		class GetClassObject<void(T, Args...)>
		{
		public:

			static HRESULT __stdcall DllGetClassObject(const CLSID& rclsid, const IID& riid, void** ppv)
			{
				if (::IsEqualCLSID(rclsid, __uuidof(T)))
				{
					static ClassObject<T> classObject;
					return classObject.QueryInterface(riid, ppv);
				}
				return GetClassObject<void(Args...)>::DllGetClassObject(rclsid, riid, ppv);
			}
		};

	} // end namespace details


	//////////////////////////////////////////////////////////////////////////////
	// concrete DLL Inproc-Server
	//////////////////////////////////////////////////////////////////////////////

	template<class ... Args>
	class InprocServer : public ComDLL
	{
	public:

		BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) override
		{
			if (fdwReason == DLL_PROCESS_ATTACH)
			{
				module_instance() = hinstDLL;
			}
			return TRUE;
		}

		HRESULT __stdcall DllGetClassObject(const CLSID& rclsid, const IID& riid, void** ppv) override
		{
			if (!ppv)
				return E_INVALIDARG;

			*ppv = 0;

			return details::GetClassObject<void(Args...)>::DllGetClassObject(rclsid, riid, ppv);
		}

		HRESULT __stdcall DllCanUnloadNow(void) override
		{
			if (comModule().canUnload())
				return S_OK;

			return S_FALSE;
		}

		virtual HRESULT __stdcall DllRegisterServer(void)
		{
			return S_OK;
		}

		virtual HRESULT __stdcall DllUnregisterServer(void)
		{
			return S_OK;
		}
	};

} // end namespace MTL

//////////////////////////////////////////////////////////////////////////////////
// implement DLL exports for Inproc server helper MACRO
//////////////////////////////////////////////////////////////////////////////////

#define DLL_COCLASS_EXPORTS(...)																		\
MTL::ComDLL& inprocServer()																				\
{																										\
	static MTL::InprocServer<__VA_ARGS__> dll;															\
	return dll;																							\
}																										\
																										\
EXTERN_C BOOL __stdcall DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)				\
{																										\
	return inprocServer().DllMain(hinstDLL,fdwReason,lpvReserved );										\
}																										\
																										\
EXTERN_C HRESULT __stdcall DllGetClassObject( const CLSID & rclsid, const IID & riid, void ** ppv)		\
{																										\
	return inprocServer().DllGetClassObject(rclsid,riid,ppv);											\
}																										\
																										\
EXTERN_C HRESULT __stdcall DllCanUnloadNow(void)														\
{																										\
	return inprocServer().DllCanUnloadNow();															\
}																										\

#define INPROC_SERVER_EXPORTS(...)																		\
DLL_COCLASS_EXPORTS(__VA_ARGS__)																		\
																										\
EXTERN_C HRESULT __stdcall DllRegisterServer(void)														\
{																										\
	return S_OK;																						\
}																										\
																										\
EXTERN_C HRESULT __stdcall DllUnregisterServer(void)													\
{																										\
	return S_OK;																						\
}																										\

#define INPROC_SERVER_SELFREG(...)																		\
DLL_COCLASS_EXPORTS(__VA_ARGS__)																		\
																										\
HRESULT register_module();																				\
EXTERN_C HRESULT __stdcall DllRegisterServer(void)														\
{																										\
	return register_module();																			\
}																										\
																										\
HRESULT unregister_module();																			\
EXTERN_C HRESULT __stdcall DllUnregisterServer(void)													\
{																										\
	return unregister_module();																			\
}																										\

