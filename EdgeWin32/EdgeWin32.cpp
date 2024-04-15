#include "pch.h"
#include "EdgeWin32.h"
#include <map>
#include "webview2/WebView2.h"

#include "mtl/edge/chrome.h"


extern HMODULE g_hModule;

const wchar_t* EDGE_WC = L"EDGE_WIN32_WC";

class Chrome
{
public:

    HWND hWnd;
    std::wstring location;
    mtl::punk< ICoreWebView2Controller> webViewController;
    mtl::punk< ICoreWebView2> webview;

    EventRegistrationToken		navigationStartingToken;
    EventRegistrationToken		onDocumentLoadedToken;
    EventRegistrationToken		onMessageToken;
    EventRegistrationToken		permissionRequestToken;
};

extern mtl::punk<mtl::chrome_edge> edge;
std::map<HWND, Chrome*> theMap;

void on_permission_request_handler(ICoreWebView2PermissionRequestedEventArgs* args);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        Chrome* chrome = new Chrome();
        chrome->hWnd = hWnd;

        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        bool ctx_enabled = !( cs->dwExStyle & WM_STYLE_EX_NO_CTX_MENU );

        mtl::wbuff buf(4096);
        ::GetWindowTextW(hWnd, buf, buf.size());
        chrome->location = buf.toString();// L"about:blank";
        
        theMap[hWnd] = chrome;
        ::SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)chrome);
        if (!edge)
        {
            edge = mtl::chrome_edge::create_instance(EDGE_WC);
        }
        edge->create_webview(hWnd, [hWnd,chrome, ctx_enabled](HRESULT hr, ICoreWebView2Controller* controller)
        {
            std::cout << "webview created" << std::endl;

            Chrome* chrome = theMap[hWnd];

            chrome->webViewController = controller;

            chrome->webViewController->get_CoreWebView2(&chrome->webview);

            mtl::punk<ICoreWebView2Settings> settings;
            chrome->webview->get_Settings(&settings);
            settings->put_AreDefaultContextMenusEnabled(ctx_enabled);

            chrome->webview->Navigate(chrome->location.c_str());

            chrome->webview->add_NavigationStarting(
                *mtl::detail::make_callback<ICoreWebView2NavigationStartingEventHandler>(
                    [hWnd, chrome](ICoreWebView2* webView, ICoreWebView2NavigationStartingEventArgs* args)
                {
                    LPWSTR uri = nullptr;
                    args->get_Uri(&uri);

                    NM_NAVIGATE notification;
                    notification.hdr.code = NM_NAVIGATE_START;
                    notification.hdr.idFrom = ::GetDlgCtrlID(hWnd);
                    notification.hdr.hwndFrom = hWnd;
                    notification.uri = uri;

                    bool cancel = ::SendMessage(::GetParent(hWnd),WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
                    
                    if (cancel)
                    {
                        args->put_Cancel(TRUE);
                    }
                    else
                    {
                        chrome->location = uri;
                    }
                    ::CoTaskMemFree(uri);
                }
                ), &chrome->navigationStartingToken
            );

            chrome->webview->add_NavigationCompleted(
                *mtl::detail::make_callback< ICoreWebView2NavigationCompletedEventHandler>(
                    [hWnd,chrome](ICoreWebView2* webView, ICoreWebView2NavigationCompletedEventArgs* args)
                {
                    BOOL success = FALSE;
                    args->get_IsSuccess(&success);

                    int code = NM_NAVIGATE_SUCCESS;
                    if (!success) code = NM_NAVIGATE_ERROR;

                    NM_NAVIGATE notification;
                    notification.hdr.code = code;
                    notification.hdr.idFrom = ::GetDlgCtrlID(hWnd);
                    notification.hdr.hwndFrom = hWnd;
                    notification.uri = chrome->location.c_str();

                    ::SendMessageW(::GetParent(hWnd), WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
                }
                ),&chrome->onDocumentLoadedToken
            );

            chrome->webview->add_WebMessageReceived(
                *mtl::detail::make_callback< ICoreWebView2WebMessageReceivedEventHandler>(
                    [hWnd,chrome](ICoreWebView2* webView, ICoreWebView2WebMessageReceivedEventArgs* args)
                {
                    LPWSTR json = 0;
                    args->get_WebMessageAsJson(&json);
               
                    NM_JSON notification;
                    notification.hdr.code = NM_JSON_MSG;
                    notification.hdr.idFrom = ::GetDlgCtrlID(hWnd);
                    notification.hdr.hwndFrom = hWnd;
                    notification.json = json;

                    ::SendMessageW(::GetParent(hWnd), WM_NOTIFY, notification.hdr.idFrom, (LPARAM)&notification);
                    ::CoTaskMemFree(json);
                }
                ),
                &chrome->onMessageToken
            );

            chrome->webview->add_PermissionRequested(
                *mtl::detail::make_callback<ICoreWebView2PermissionRequestedEventHandler>(
                    [hWnd,chrome](ICoreWebView2* webView, ICoreWebView2PermissionRequestedEventArgs* args)
                {
                    on_permission_request_handler(args);
                }
                ),
                &chrome->permissionRequestToken
            );
        });
        
        return 0;
    }
    case WM_EDGE_NAVIGATE:
    {
        Chrome* chrome = theMap[hWnd];

        if (chrome)
        {
            wchar_t* location = (wchar_t*)wParam;
            chrome->webview->Navigate(location);
        }
        return 0;
        break;
    }
    case WM_EDGE_JSON_MSG:
    {
        Chrome* chrome = theMap[hWnd];

        if (chrome)
        {
            wchar_t* json = (wchar_t*)wParam;
            chrome->webview->PostWebMessageAsJson(json);
        }
        return 0;
        break;
    }
    case WM_EDGE_JAVASCRIPT:
    {
        Chrome* chrome = theMap[hWnd];

        if (chrome)
        {
            wchar_t* javascript = (wchar_t*)wParam;
            chrome->webview->ExecuteScript(javascript, nullptr);
        }
        return 0;
        break;
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDOK:
            //DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT r;
        ::GetClientRect(hWnd, &r);
        DrawTextW(hdc, L"HELO CHILD", -1,&r, DT_CENTER| DT_VCENTER| DT_SINGLELINE);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_SIZE:
    {
        UINT width = LOWORD(lParam);
        UINT height = HIWORD(lParam);

        Chrome* chrome = theMap[hWnd];

        if (chrome)
        {
            if (chrome->webViewController)
            {
                RECT r;
                ::GetClientRect(hWnd, &r);
                chrome->webViewController->put_Bounds(r);
            };
        }
        break;
    }
    case WM_NCDESTROY:
    {
        Chrome* chrome = theMap[hWnd];

        if (chrome)
        {
            if (chrome->webViewController)
            {
                chrome->webViewController->Close();
                chrome->webViewController.release();
            }
            theMap.erase(hWnd);
        }
        return 0;
    }
    case WM_DESTROY:
        //PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void registerEdgeClass()
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    HINSTANCE hInst = ::GetModuleHandle(NULL);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInst;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = EDGE_WC;
    wcex.hIconSm =0;

    ATOM a = ::RegisterClassExW(&wcex);
    int i = 1+1;
}

void on_permission_request_handler(ICoreWebView2PermissionRequestedEventArgs* args)
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
