#pragma once

#include "mtl/win32/box.h" 
#include "mtl/win/wnd.h" 

namespace mtl {

    class application
    {
    public:
        application(HINSTANCE hInst)
        {
            module_instance() = hInst;
            UINT unused = WmReflect();
        }

        int run(HWND acceleree = nullptr, int accelId = 0)
        {
            if (::IsWindow(acceleree) && accelId)
            {
                hAccelTable_ = ::LoadAccelerators(module_instance(), MAKEINTRESOURCE(accelId));
                return  ui_thread().run(acceleree, hAccelTable_);
            }
            return  ui_thread().run();
        }

/* MSG msg;

            // Main message loop:
            while (GetMessage(&msg, nullptr, 0, 0))
            {
                if (!TranslateAccelerator(msg.hwnd, hAccelTable_, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }

            return (int)msg.wParam;
        }
        */
    private:
        HACCEL hAccelTable_ = nullptr;
    };

}
