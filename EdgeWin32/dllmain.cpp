// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "EdgeWin32.h"
#include "framework.h"
#include "mtl/edge/chrome.h"

HMODULE g_hModule = nullptr;

mtl::punk<mtl::chrome_edge> edge;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            g_hModule = hModule;
            registerEdgeClass();
            break;
        }
        case DLL_THREAD_ATTACH: break;
        case DLL_THREAD_DETACH: break;
        case DLL_PROCESS_DETACH:
        {
            if (edge) edge.release();
            break;
        }
        return TRUE;
    }
    return TRUE;
}

