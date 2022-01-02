#pragma once

#include "MTL/sdk.h"
#include "MTL/win/wnd.h"
#include "MTL/win/clipboard.h"
#include "MTL/win/wnd.h"
#include "MTL/ole/shell.h"

#include "webview2/WebView2.h"

namespace mtl {

	class chrome_edge;

	inline static chrome_edge* chromeEdge(const std::wstring& path);

	inline std::wstring escape_json_str(const std::wstring& in)
	{
		std::wostringstream oss;

		for (auto it = in.begin(); it != in.end(); it++)
		{
			if ((*it) == L'\\' || (*it) == L'\'')
			{
				oss << '\\';
			}
			oss << (*it);
		}
		return oss.str();
	}

	namespace detail {

		template<class T, class I, class ... Args>
		class simple_callback : public implements<T(I)>
		{
		public:

			using callback = std::function<void(Args...)>;

			void dispose()
			{}

		protected:
			callback cb_;
		};

		template<class I, class ... Args>
		class callback : public simple_callback<callback<I, Args...>, I, Args...>
		{
		public:

			static punk<I> create_instance(simple_callback<callback<I, Args...>, I, Args...>::callback cb)
			{
				auto* handler = new callback<I, Args...>();
				handler->cb_ = cb;

				punk<I> result(handler);
				return result;
			}

			HRESULT __stdcall Invoke(Args... args)
			{
				this->cb_(args...);
				return S_OK;
			}
		};


		template<class I, class T>
		struct callback_maker;

		template<class I, class R, class T, class ... Args>
		struct callback_maker<I, R(T::*)(Args...) const>
		{
			template<class L>
			static auto make(L lambda)
			{
				return callback<I, Args...>::create_instance(lambda);
			}
		};

		template<class I, class R, class T, class ... Args>
		struct callback_maker<I, R(T::*)(Args...)>
		{
			template<class L>
			static auto make(L lambda)
			{
				return callback<I, Args...>::create_instance(lambda);
			}
		};

		template<class I, class L>
		auto make_callback(L lambda)
		{
			using type = decltype(&L::operator());
			return callback_maker<I, type>::make(lambda);
		}



		class webview_controller_callback :
			public simple_callback<
			webview_controller_callback,
			ICoreWebView2CreateCoreWebView2ControllerCompletedHandler,
			HRESULT,
			ICoreWebView2Controller*
			>
		{
		private:
			HWND hWnd_;

		public:

			static punk<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler> create_instance(HWND hWnd, callback cb)
			{
				webview_controller_callback* wc = new webview_controller_callback;
				wc->hWnd_ = hWnd;
				wc->cb_ = cb;

				punk<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler> result(wc);
				return result;
			}

			HRESULT __stdcall Invoke(HRESULT hr, ICoreWebView2Controller* controller)
			{
				RECT bounds;
				GetClientRect(hWnd_, &bounds);
				controller->put_Bounds(bounds);
				cb_(hr, controller);
				return S_OK;
			}
		};

	}

	class chrome_edge :
		public implements<
			chrome_edge(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler)
		>
	{
	private:
		punk<ICoreWebView2Environment> chromeEnv_;

		using callback = std::function<void(HRESULT, ICoreWebView2Controller*)>;
		using closure = std::pair<HWND, callback>;
		std::list<closure> callbacks_;

	public:

		std::wstring userdata;
		std::wstring chromeFolder;

		chrome_edge(const std::wstring& name)
		{
			userdata = app_files_path(name);
			//use static runtime
			//chromeFolder = L"C:\\webview2\\Microsoft.WebView2.FixedVersionRuntime.87.0.664.8.x64";
		}

		~chrome_edge()
		{}

		static punk<chrome_edge> create_instance(const std::wstring& name)
		{
			chrome_edge* chrome = new chrome_edge(name);

			punk<chrome_edge> result(chrome);
			return result;
		}

		void dispose()
		{

		}

		void create_webview(HWND hWnd, std::function<void(HRESULT, ICoreWebView2Controller*)> cb)
		{
			if (chromeEnv_)
			{
				auto cc = detail::webview_controller_callback::create_instance(hWnd, cb);
				chromeEnv_->CreateCoreWebView2Controller(hWnd, *cc);
				return;
			}

			callbacks_.push_back(std::make_pair(hWnd, cb));
			HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(chromeFolder.c_str(), userdata.c_str(), nullptr, this);

		}


		HRESULT __stdcall Invoke(HRESULT, ICoreWebView2Environment* env)
		{
			chromeEnv_ = env;
			for (auto it = callbacks_.begin(); it != callbacks_.end(); it++)
			{
				auto cc = detail::webview_controller_callback::create_instance((*it).first, (*it).second);
				env->CreateCoreWebView2Controller((*it).first, *cc);
			}
			callbacks_.clear();
			return S_OK;
		};

	};

	/////////////////////////////////////////////////////////////////////////////////////////////
	// html form window
	/////////////////////////////////////////////////////////////////////////////////////////////

	class html_wnd : public window<html_wnd>
	{
	public:

		std::wstring appDataPath;
		bool disableDragDrop = true;
		punk< ICoreWebView2Controller> webViewController;
		punk< ICoreWebView2> webview;

		mtl::event<void(std::wstring)> onMessage;
		mtl::event<void()> onDocumentLoad;
		mtl::event<void(std::wstring)> onDocumentTitleChanged;
		mtl::event<void(std::wstring,bool& cancel)> onNavigationStarted;

		// Construction

		html_wnd()
			:appDataPath(L"MTLwebview")
		{}

		html_wnd(std::wstring path)
			:appDataPath(path)
		{}

		~html_wnd()
		{}


		virtual bool navigate(const std::wstring& loc)
		{
			location_ = loc;

			mtl::chromeEdge(appDataPath)->create_webview(handle, [this](HRESULT hr, ICoreWebView2Controller* controller)
			{
				this->on_create_webview(location_, controller);
			});

			return true;
		}


		virtual void on_create_webview(std::wstring target, ICoreWebView2Controller* controller)
		{
			webViewController = controller;

			webViewController->get_CoreWebView2(&webview);

			// attach event handlers

			webview->add_DocumentTitleChanged(
				*detail::make_callback<ICoreWebView2DocumentTitleChangedEventHandler>(
					[this](ICoreWebView2* sender, IUnknown*)
			{
				this->on_document_title_changed_handler();
			}
					),
				&documentTitleChangedToken_
				);

			webview->add_NavigationStarting(
				*detail::make_callback<ICoreWebView2NavigationStartingEventHandler>(
					[this](ICoreWebView2* webView, ICoreWebView2NavigationStartingEventArgs* args)
			{
				this->on_navigation_started_handler(args);
			}
					),
				&navigationStartingToken_
				);

			webview->add_PermissionRequested(
				*detail::make_callback<ICoreWebView2PermissionRequestedEventHandler>(
					[this](ICoreWebView2* webView, ICoreWebView2PermissionRequestedEventArgs* args)
			{
				this->on_permission_request_handler(args);
			}
					),
				&permissionRequestToken_
				);

			webview->add_NavigationCompleted(
				*detail::make_callback< ICoreWebView2NavigationCompletedEventHandler>(
					[this](ICoreWebView2* webView, ICoreWebView2NavigationCompletedEventArgs* args)
			{



				BOOL success = FALSE;
				args->get_IsSuccess(&success);
				if (success)
					this->on_document_load_handler();
			}
					),
				&onDocumentLoadedToken_
				);

			webview->add_WebMessageReceived(
				*detail::make_callback< ICoreWebView2WebMessageReceivedEventHandler>(
					[this](ICoreWebView2* webView, ICoreWebView2WebMessageReceivedEventArgs* args)
			{
				LPWSTR json = 0;
				args->get_WebMessageAsJson(&json);
				this->on_message_handler(json);
				::CoTaskMemFree(json);
			}
					),
				&onMessageToken_
				);

			// load initial html page
			webview->Navigate(target.c_str());
		}

		// control HTML navigation

		void back()
		{
			if (!webview) return;
			webview->GoBack();
		}

		void forward()
		{
			if (!webview) return;
			webview->GoForward();
		}

		void reload()
		{
			if (!webview) return;
			webview->Reload();
		}

		void options()
		{
			if (!webview) return;
			webview->OpenDevToolsWindow();
		}

		void print()
		{
			if (!webview) return;
			webview->ExecuteScript(L"window.print();", nullptr);
		}

		void cut()
		{
			if (!webview) return;
			webview->ExecuteScript(L"document.execCommand('cut')", nullptr);
		}

		void copy()
		{
			if (!webview) return;
			webview->ExecuteScript(L"document.execCommand('copy')", nullptr);
		}

		void paste()
		{
			if (!webview) return;

			clipboard::lock clip(handle);

			std::wstring txt = clipboard::as_wstring(CF_UNICODETEXT);
			std::wstring pasteText = escape_json_str(txt);

			std::wostringstream oss;
			oss << L"(function(){ let el = document.activeElement; "
				<< L"el.value = el.value.substring(0,el.selectionStart) + "
				<< L"'" << pasteText.substr(0, pasteText.size() - 1);
			oss << L"' + "
				<< L"el.value.substring(el.selectionEnd,el.value.length); }) ();";

			std::wstring ws(oss.str());
			webview->ExecuteScript(ws.c_str(), nullptr);
		}

		void stop()
		{
			if (!webview) return;
			webview->Stop();
		}

		std::wstring location()
		{
			return location_;
		}


	protected:

		virtual HWND create_window(const wchar_t* title, HWND parent, RECT& r, int style, int exStyle, HMENU menu) override
		{
			HWND hWnd = window<html_wnd>::create_window(title, parent, r, style, exStyle, menu);

			if (title && wcslen(title) != 0)
			{
				navigate(title);
			}

			return hWnd;
		}

		// Event handlers 

		virtual void on_document_title_changed_handler()
		{
			LPWSTR title = nullptr;
			webview->get_DocumentTitle(&title);
			this->set_text(title);
			onDocumentTitleChanged.fire(title);
			::CoTaskMemFree(title);
		}

		virtual void on_navigation_started_handler(ICoreWebView2NavigationStartingEventArgs* args)
		{
			LPWSTR uri = nullptr;
			args->get_Uri(&uri);

			bool cancel = false;
			onNavigationStarted.fire(uri,cancel);
			if (cancel)
			{
				args->put_Cancel(TRUE);
			}
			::CoTaskMemFree(uri);
		}

		virtual void on_permission_request_handler(ICoreWebView2PermissionRequestedEventArgs* args)
		{
			LPWSTR uri = nullptr;
			COREWEBVIEW2_PERMISSION_KIND kind = COREWEBVIEW2_PERMISSION_KIND_UNKNOWN_PERMISSION;
			BOOL userInitiated = FALSE;

			args->get_Uri(&uri);
			args->get_PermissionKind(&kind);
			args->get_IsUserInitiated(&userInitiated);

			std::wostringstream oss;
			oss << L"Do you want to grant permission for ";

			switch (kind)
			{
			case COREWEBVIEW2_PERMISSION_KIND_CAMERA:
			{
				oss << "Camera";
				break;
			}
			case COREWEBVIEW2_PERMISSION_KIND_CLIPBOARD_READ:
			{
				oss << "Clipboard Read";
				break;
			}
			case COREWEBVIEW2_PERMISSION_KIND_GEOLOCATION:
			{
				oss << "GeoLocation";
				break;
			}
			case COREWEBVIEW2_PERMISSION_KIND_MICROPHONE:
			{
				oss << "Microphone";
				break;
			}
			case COREWEBVIEW2_PERMISSION_KIND_NOTIFICATIONS:
			{
				oss << "Notifications";
				break;
			}
			case COREWEBVIEW2_PERMISSION_KIND_OTHER_SENSORS:
			{
				oss << "Other Sensors";
				break;
			}
			}

			oss << L" to the website at "
				<< uri
				<< L"?\n\n";

			if (userInitiated)
			{
				oss << L"This request came from a user gesture.";
			}
			else
			{
				oss << L"This request did not come from a user gesture.";
			}

			int response = MessageBox(nullptr, oss.str().c_str(), L"Permission Request",
				MB_YESNOCANCEL | MB_ICONWARNING);

			COREWEBVIEW2_PERMISSION_STATE state =
				response == IDYES ? COREWEBVIEW2_PERMISSION_STATE_ALLOW
				: response == IDNO ? COREWEBVIEW2_PERMISSION_STATE_DENY
				: COREWEBVIEW2_PERMISSION_STATE_DEFAULT;

			args->put_State(state);

		}


		virtual void on_document_load_handler()
		{
			if (disableDragDrop)
			{
				webview->ExecuteScript(
					L"  document.body.addEventListener('dragover', function(e){ if(e.target.tagName != 'INPUT') {e.preventDefault();} }, false);"
					L"  document.body.addEventListener('drop',function(e){ if(e.target.tagName != 'INPUT') {e.preventDefault();} }, false); ",
					nullptr
				);
			}

			onDocumentLoad.fire();
		}


		virtual void on_message_handler(LPCWSTR json)
		{
			onMessage.fire(json);
		}


		// stamdard window messages

		LRESULT wm_destroy() override
		{
			return 0;

		}

		LRESULT wm_nc_destroy() override
		{
			if (webViewController)
			{
				webViewController->Close();
				webViewController.release();
			}
			return 0;
		}

		LRESULT wm_size(RECT& clientRect) override
		{
			if (webViewController)
			{
				RECT bounds = client_rect();
				webViewController->put_Bounds(bounds);
			};
			return 0;
		}

		LRESULT wm_search(FINDREPLACE* fi) override
		{
			bool down = fi->Flags & FR_DOWN;
			bool caseSensitive = fi->Flags & FR_MATCHCASE;
			bool wholeWord = fi->Flags & FR_WHOLEWORD;

			std::wostringstream oss;

			oss << L"window.find('" << escape_json_str(fi->lpstrFindWhat) << L"', "
				<< caseSensitive << L", " << !down << L", 1, " << wholeWord << ", 1, 1 );";

			webview->ExecuteScript(oss.str().c_str(), nullptr);

			return 0;
		}

		std::wstring				location_;
		EventRegistrationToken		documentTitleChangedToken_;
		EventRegistrationToken		navigationStartingToken_;
		EventRegistrationToken		permissionRequestToken_;
		EventRegistrationToken		onDocumentLoadedToken_;
		EventRegistrationToken		onMessageToken_;
	};

	inline static chrome_edge* chromeEdge(const std::wstring& path)
	{
		static std::map<std::wstring, punk<chrome_edge>> runtimes;

		if (runtimes.count(path) == 0)
		{
			punk<chrome_edge> ce = chrome_edge::create_instance(path);
			runtimes[path] = ce;
		}
		
		if (runtimes.count(path) == 0)
		{
			return nullptr;
		}

		return *runtimes[path];
	}

}

