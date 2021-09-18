#pragma once

#include "mtl/obj/obj.h"

extern mtl::com_dll& inproc_server();

namespace mtl {

	//////////////////////////////////////////////////////////////////////////////
	// Class Factory implementations
	//////////////////////////////////////////////////////////////////////////////

	namespace details {

		template<class T>
		class get_class_object;

		template<>
		class get_class_object<void()>
		{
		public:

			static HRESULT __stdcall DllGetClassObject(const CLSID& rclsid, const IID& riid, void** ppv)
			{
				return REGDB_E_CLASSNOTREG;
			}
		};

		template<class T, class ...Args>
		class get_class_object<void(T, Args...)>
		{
		public:

			static HRESULT __stdcall DllGetClassObject(const CLSID& rclsid, const IID& riid, void** ppv)
			{
				if (::IsEqualCLSID(rclsid, __uuidof(T)))
				{
					static class_object<T> classObject;
					return classObject.QueryInterface(riid, ppv);
				}
				return get_class_object<void(Args...)>::DllGetClassObject(rclsid, riid, ppv);
			}
		};

	} // end namespace details


	//////////////////////////////////////////////////////////////////////////////
	// concrete DLL Inproc-Server
	//////////////////////////////////////////////////////////////////////////////

	template<class ... Args>
	class inproc_server : public com_dll
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

			return details::get_class_object<void(Args...)>::DllGetClassObject(rclsid, riid, ppv);
		}

		HRESULT __stdcall DllCanUnloadNow(void) override
		{
			if (the_com_module().can_unload())
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
mtl::com_dll& inproc_Server()																			\
{																										\
	static mtl::inproc_server<__VA_ARGS__> dll;															\
	return dll;																							\
}																										\
																										\
EXTERN_C BOOL __stdcall DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)				\
{																										\
	return inproc_server().DllMain(hinstDLL,fdwReason,lpvReserved );									\
}																										\
																										\
EXTERN_C HRESULT __stdcall DllGetClassObject( const CLSID & rclsid, const IID & riid, void ** ppv)		\
{																										\
	return inproc_server().DllGetClassObject(rclsid,riid,ppv);											\
}																										\
																										\
EXTERN_C HRESULT __stdcall DllCanUnloadNow(void)														\
{																										\
	return inproc_server().DllCanUnloadNow();															\
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

