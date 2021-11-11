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

        int run()
        {
            return  ui_thread().run();
        }        

        int run(mtl::accelerators& accel)
        {
            return  ui_thread().run(accel);
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
  //  private:
   //     HACCEL hAccelTable_ = nullptr;
    };

}
