#pragma once

#include "MTL/sdk.h"
#include <objbase.h>
#include <string>
#include <MTL/punk.h>
#include <MTL/win32/uni.h>
#include <MTL/win32/module.h>

namespace MTL {

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

        std::wstring dir = pathToSelfDirectory();
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
                HRESULT hr = pun.createObject(clsid, clsctx);
                if (hr == S_OK)
                {
                    return pun.queryInterface(t);
                }
            }
            return E_FAIL;
        }
    }

} // end namespace

