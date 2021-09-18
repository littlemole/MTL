#pragma once

#include "mtl/sdk.h"
#include <mtl/punk.h>
#include <mtl/win32/uni.h>
#include <mtl/win32/module.h>
#include <objbase.h>
#include <string>

namespace mtl {

    template<class T>
    inline HRESULT CoRunLocalServer(const std::wstring& server, const CLSID& clsid, T** t, int clsctx = CLSCTX_ALL)
    {
        punk<IClassFactory> cf;
        HRESULT hr = ::CoGetClassObject(clsid, clsctx, 0, IID_IClassFactory, (void**) &cf);
        if (hr == S_OK)
        {
            hr = cf->CreateInstance(0, __uuidof(T), (void**)t);
            return hr;
        }

        STARTUPINFO info;
        ::ZeroMemory(&info, sizeof(info));
        info.cb = sizeof(info);

        PROCESS_INFORMATION pi;
        ::ZeroMemory(&pi, sizeof(pi));

        std::wstring dir = path_to_self_directory();
        std::wstring self = dir;
        self += L"\\";
        self += server;

        BOOL bSuccess = ::CreateProcessW(self.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, dir.c_str(), &info, &pi);
        if (!bSuccess)
        {
            DWORD dw = ::GetLastError();
            return E_FAIL;
            //_com_error err(dw);
        }
        else
        {
           //::WaitForInputIdle(pi.hProcess, 5000);
            DWORD ms = 6000;
            DWORD cnt = 0;

            while (cnt < ms)
            {
                ::Sleep(20);
                cnt += 20;

                punk<T> pun;
                HRESULT hr = pun.create_object(clsid, clsctx);
                if (hr == S_OK)
                {
                    return pun.query_interface(t);
                }
            }
            return E_FAIL;
        }
    }

} // end namespace

