#pragma once

#include "MTL/win32/box.h" 
#include "MTL/win/wind.h" 

namespace MTL {




    class Application
    {
    public:
        Application(HINSTANCE hInst)
        {
            module_instance() = hInst;
            UINT unused = WmReflect();
        }

        HACCEL loadAccelerators(int id)
        {
            hAccelTable_ = LoadAccelerators(module_instance(), MAKEINTRESOURCE(id));
            return hAccelTable_;
        }

        int run()
        {
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
