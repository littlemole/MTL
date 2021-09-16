#pragma once

#include "MTL/sdk.h"
#include "MTL/win32/module.h"

namespace MTL {

    template<class T>
    class WindowClass : public WNDCLASSEXW
    {
    public:

        WindowClass()
        {
            cbSize = sizeof(WNDCLASSEX);
            style = CS_HREDRAW | CS_VREDRAW;
            lpfnWndProc = &WndProc;
            cbClsExtra = 0;
            cbWndExtra = 0;
            hInstance = module_instance();
            hIcon = ::LoadIcon(module_instance(), IDI_APPLICATION);
            hCursor = ::LoadCursor(nullptr, IDC_ARROW);
            hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            lpszMenuName = 0;// MAKEINTRESOURCEW(IDC_WUI);

            const char* n = typeid(T).name();
            const size_t cSize = strlen(n) + 1;
            wchar_t* wc = new wchar_t[cSize];
            mbstowcs_s(0, wc, cSize, n, cSize);

            lpszClassName = wc;// szWindowClass;
            hIconSm = (HICON) ::LoadIcon(module_instance(), IDI_APPLICATION);

        }

        ~WindowClass()
        {
            delete[] lpszClassName;
        }

        const wchar_t* name()
        {
            if (!atom)
            {
                atom = ::RegisterClassExW(this);
            }
            return lpszClassName;
        }

        void setMenu(int id)
        {
           this->lpszMenuName = (LPCWSTR)MAKEINTRESOURCE(id);
        }

    private:

        ATOM atom = 0;

        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)
            {
            case WM_NCCREATE:
            {
                CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
                T* that = (T*)(cs->lpCreateParams);
                that->handle = hWnd;
                ::SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)that);
                // fallthru
            }
            default:
            {
                LONG_PTR l = ::GetWindowLongPtrW(hWnd, GWLP_USERDATA);
                if (!l)
                {
                    return DefWindowProc(hWnd, message, wParam, lParam);
                }

                T* that = (T*)l;
                return that->wndProc(hWnd, message, wParam, lParam);
            }
            }
            return 0;
        }
    };

    template<class T>
    WindowClass<T>& windowClass()
    {
        static WindowClass<T> clazz;
        return clazz;
    }


}
