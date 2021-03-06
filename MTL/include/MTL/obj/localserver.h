#pragma once

#include "mtl/obj/obj.h"
#include "mtl/win32/box.h"
#include <iostream>

namespace mtl {

	namespace details {

		template<class T>
		class registrar
		{
		public:

			static void register_class_objects(std::vector<DWORD>& cookies)
			{}
		};

		template<class T>
		class registrar<void(T)>
		{
		public:

			static void register_class_objects(std::vector<DWORD>& cookies)
			{
				static class_object<localserver<T>> classObject;

				DWORD cookie = 0;
				HRESULT hr = ::CoRegisterClassObject(
					__uuidof(T),
					(IUnknown*)&classObject,
					CLSCTX_LOCAL_SERVER,
					REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED,
					&cookie
				);

				if (hr != S_OK) exit(1);

				cookies.push_back(cookie);
			}
		};

		template<class T, class ... Args>
		class registrar<void(T, Args...)>
		{
		public:

			static void register_class_objects(std::vector<DWORD>& cookies)
			{
				registrar<void(T)>::register_class_objects(cookies);
				registrar<void(Args...)>::register_class_objects(cookies);
			}
		};

	} // end namespace details

	template<class ... Args>
	class local_server
	{
	public:
		local_server()
		{
			DWORD mainThreadId = ::GetCurrentThreadId();

			the_com_module().onUnLoad = [mainThreadId]() {
				::PostThreadMessage(mainThreadId, WM_QUIT, 0, 0);
			};

			::CoSuspendClassObjects();

			details::registrar<void(Args...)>::register_class_objects(cookies_);

			::CoResumeClassObjects();
		}

		~local_server()
		{
			for (DWORD cookie : cookies_)
			{
				HRESULT hr = ::CoRevokeClassObject(cookie);
				if (hr != S_OK) exit(1);
			}
		}

		int run()
		{
			return ui_thread().run();
		}

	private:
		std::vector<DWORD> cookies_;
	};


} // end namespace MTL


/////////////////////////////////////////////////////////////
// local server
/////////////////////////////////////////////////////////////


#define LOCAL_SERVER(...)									\
															\
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,				\
	_In_opt_ HINSTANCE hPrevInstance,						\
	_In_ LPWSTR    lpCmdLine,								\
	_In_ int       nCmdShow)								\
{															\
	UNREFERENCED_PARAMETER(hPrevInstance);					\
	UNREFERENCED_PARAMETER(lpCmdLine);						\
	STA enter;												\
	mtl::local_server<__VA_ARGS__> server;					\
	int r = server.run();									\
															\
	return r;												\
}


#define LOCAL_SERVER_SELFREG(...)							\
															\
HRESULT register_module();									\
HRESULT unregister_module();								\
															\
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,				\
	_In_opt_ HINSTANCE hPrevInstance,						\
	_In_ LPWSTR    lpCmdLine,								\
	_In_ int       nCmdShow)								\
{															\
	UNREFERENCED_PARAMETER(hPrevInstance);					\
	UNREFERENCED_PARAMETER(lpCmdLine);						\
	STA enter;												\
															\
	std::wstring cli(lpCmdLine);							\
	if( cli.find(L"/RegServer") != std::string::npos ||		\
		cli.find(L"-RegServer") != std::string::npos )      \
	{														\
		std::cout << "RegServer" << std::endl;				\
		return register_module() == S_OK ? 0 : 1;			\
	}														\
	if (cli.find(L"/UnregServer") != std::string::npos ||	\
		cli.find(L"-UnregServer") != std::string::npos )	\
	{														\
		std::cout << "UnregServer" << std::endl;			\
		return unregister_module() == S_OK ? 0 : 1;			\
	}														\
															\
	mtl::local_server<__VA_ARGS__> server;					\
	int r = server.run();									\
															\
	return r;												\
}
