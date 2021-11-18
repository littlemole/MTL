#pragma once

#include "mtl/sdk.h"
#include "mtl/win/wnd.h"

#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace mtl {


    inline void sleep(int ms = 20)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    struct accel : public ACCEL
    {
        accel()
        {
            this->fVirt = 0;
            this->key = (WORD)0; 
            this->cmd = 0;
        }

        accel(int vkey, int cmd, int flags = 0 )
        {
            this->fVirt = flags;
            this->key = (WORD)vkey; 
            this->cmd = cmd;
        }

        accel(char key, int cmd, int flags = 0 )
        {
            this->fVirt = flags;
            this->key = (WORD)key; 
            this->cmd = cmd;
        }
    };

    class accelerators
    {
    public:

        accelerators()
        {}

        accelerators(HWND wnd, int id)
            : hWnd(wnd)
        {
            load(id);
        }

        accelerators(HWND wnd)
            : hWnd(wnd)
        {}

        ~accelerators()
        {
            free();
        }

        accelerators(const accelerators& rhs)
        {
            if(rhs.accel_ == nullptr) return;

            hWnd = rhs.hWnd;
            int s = ::CopyAcceleratorTable(rhs.accel_,nullptr,0);
            ACCEL* acc = new ACCEL[s];
            ::CopyAcceleratorTable(rhs.accel_,acc,s);
            create(acc,s);

            delete[] acc;
        }

        accelerators(accelerators&& rhs)
        {
            accel_ = rhs.accel_;
            rhs.accel_ = nullptr;

            hWnd = rhs.hWnd;
            rhs.hWnd = nullptr;
        }

        accelerators& operator=(const accelerators& rhs)
        {
            if(this == &rhs) return *this;

            free();

            if(rhs.accel_ == nullptr)
            {
                return *this;
            }

            hWnd = rhs.hWnd;
            int s = ::CopyAcceleratorTable(rhs.accel_,nullptr,0);
            ACCEL* acc = new ACCEL[s];
            ::CopyAcceleratorTable(rhs.accel_,acc,s);
            create(acc,s);

            delete[] acc;

            return *this;
        }

        accelerators& operator=(accelerators&& rhs)
        {
            if(this == &rhs) return *this;

            free();

            accel_ = rhs.accel_;
            rhs.accel_ = nullptr;

            hWnd = rhs.hWnd;
            rhs.hWnd = nullptr;

            return *this;
        }

        std::vector<accel> get()
        {
            std::vector<accel> result;
            if(accel_ == nullptr) return result;

            int s = ::CopyAcceleratorTable(accel_,nullptr,0);
            result.resize(s);
            ::CopyAcceleratorTable(accel_,&result[0],s);

            return result;
        }

        bool load(int id)
        {
            free();
            accel_ = ::LoadAccelerators(mtl::module_instance(),MAKEINTRESOURCE(id));
            return accel_ != nullptr;
        }

        bool create( ACCEL* accels, int size )
        {
            free();
            accel_ = ::CreateAcceleratorTable(accels,size);
            return accel_ != nullptr;
        }

        bool create( std::vector<accel> accels )
        {
            free();
            if(accels.empty()) return true;

            return create( (ACCEL*)&accels[0], (int)accels.size());
        }

        void free()
        {
            if(accel_)
            {
                ::DestroyAcceleratorTable(accel_);
                accel_ = nullptr;
            }
            //hWnd = nullptr;
        }

        HACCEL operator*()
        {
            return accel_;
        }

        HACCEL detach()
        {
            HACCEL result = accel_;
            accel_ = nullptr;
            return result;
        }

        HWND handle()
        {
            return hWnd;
        }

        bool translate(MSG& msg) const
        {
            if (::TranslateAccelerator(hWnd, accel_, &msg))
            {
                return true;
            }
            return false;
        }

    private:
        HWND hWnd = nullptr;
        HACCEL accel_ = nullptr;
    };

    inline accelerators& accelerator()
    {
        static accelerators acc;
        return acc;
    }


    // THERE IS ONLY ONE THREADBOX !!!
    template<class T>
    class thread_box;

    // IDEA: submit with timeout, default = now()
    // priority_queue based on timeout
    template<>
    class thread_box<void()>
    {
    public:

       // accelerators keyboardAccelerators;
        HWND hWnd = nullptr;

        using task_t = std::function<void()>;

        thread_box()
        {
            event_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        }

        ~thread_box()
        {
            ::CloseHandle(event_);
        }

        void stop()
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                stop_ = true;
                ::SetEvent(event_);
            }
            ::SleepEx(100, TRUE);
        }

        bool stopped()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return stop_;
        }

        void submit(const task_t& t)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stop_)
            {
                return;
            }
            queue_.push_back(t);
            ::SetEvent(event_);
        }

        void operator()(const task_t& t)
        {
            submit(t);
        }

        HANDLE wait_handle()
        {
            return event_;
        }

        bool empty() const
        {
            const std::lock_guard<std::mutex> lock(mutex_);
            return queue_.empty();
        }

        bool wait(int ms = INFINITE)
        {
            HANDLE handles = event_;
            while (!stopped())
            {
                DWORD r = ::WaitForMultipleObjectsEx(1, &handles, FALSE, ms, TRUE);
                if (r == WAIT_IO_COMPLETION)
                {
                    continue;
                }
                if (r == WAIT_OBJECT_0)
                {
                    pull();
                    return true;
                }
                if (r == WAIT_TIMEOUT || r == WAIT_FAILED)
                {
                    break;
                }
            }
            sleep(30);
            return false;
        }

        int run()
        {
            auto msg_handler = [](MSG& msg)
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            };

            return run(msg_handler);
        }

        int run(accelerators& accels)
        {
            auto msg_handler = [this,accels](MSG& msg)
            {
                if(!accels.translate(msg))
                {
                    ::TranslateMessage(&msg);
                    ::DispatchMessage(&msg);
                }
            };

            return run(msg_handler);
        }

        int run( const std::function<void(MSG&)> msg_handler)
        {
            MSG msg = { 0,0,0,0 };
            while (!stopped())
            {
                HANDLE handles = event_;
                DWORD r = ::MsgWaitForMultipleObjectsEx(1, &handles, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE | MWMO_ALERTABLE);

                if (stopped())
                {
                    break;
                }

                if (r == WAIT_TIMEOUT)
                {

                    continue;
                }
                if (r == WAIT_IO_COMPLETION)
                {
                    continue;
                }

                if (r == WAIT_OBJECT_0)
                {
                   pull();
                   continue;
                }

                if (!::GetMessage(&msg, 0, 0, 0))
                {
                    break;
                }

                if (!::IsWindow(msg.hwnd))
                {
                    OutputDebugString(L"INVALID WIND\r\n");
                    //continue;
                }

                if (modeless_dlg().is_dialog_message(msg))
                {
                    continue;
                }

                if (msg.message == WmSearch())
                {
                    FINDREPLACE* fr = (FINDREPLACE*)msg.lParam;
                    if (fr->Flags & FR_DIALOGTERM)
                    {
                        modeless_dlg().remove(fr->hwndOwner);
                    }
                    continue;
                }

                msg_handler(msg);
            }
            sleep(30);
            return (int)msg.wParam;
        }

        bool pull()
        {
            task_t task;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (stop_)
                {
                    return false;
                }
                if (queue_.empty())
                    return false;

                task = queue_.front();
                queue_.pop_front();
            }
            try
            {
                task();
            }
            catch (...)
            {
                return false;
            }

            return true;
        }

    private:

        bool   stop_ = false;
        mutable std::mutex mutex_;
        std::deque<task_t> queue_;
        HANDLE event_;
    };
    /*
    template<class ... Args>
    class thread_box<void(Args...)>
    {
    public:

        using task_t = std::function<void(Args...)>;

        thread_box()
        {
            event_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        }

        ~thread_box()
        {
            ::CloseHandle(event_);
        }

        void stop()
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                stop_ = true;
                ::SetEvent(event_);
            }
        }

        bool stopped()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return stop_;
        }

        void submit( task_t t)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stop_)
            {
                return;
            }
            queue_.push_back(t);
            ::SetEvent(event_);
        }

        void operator()( task_t t)
        {
            submit(t);
        }

        HANDLE wait_handle()
        {
            return event_;
        }

        bool empty() const
        {
            const std::lock_guard<std::mutex> lock(mutex_);
            return queue_.empty();
        }

        bool wait(Args ... args, int ms = INFINITE)
        {
            HANDLE handles = event_;
            while (!stopped())
            {
                DWORD r = ::WaitForMultipleObjectsEx(1, &handles, FALSE, ms, TRUE);
                if (r == WAIT_IO_COMPLETION)
                {
                    continue;
                }
                if (r == WAIT_OBJECT_0)
                {
                    pull(args...);
                    return true;
                }
                if (r == WAIT_TIMEOUT || r == WAIT_FAILED)
                {
                    break;
                }
            }
            sleep(30);
            return false;
        }


        bool pull(Args ... args)
        {
            task_t task;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (stop_)
                {
                    return false;
                }
                if (queue_.empty())
                    return false;

                task = queue_.front();
                queue_.pop_front();
            }
            try
            {
                task(args...);
            }
            catch (...)
            {
                return false;
            }

            return true;
        }

    private:

        bool   stop_ = false;
        mutable std::mutex mutex_;
        std::deque<task_t> queue_;
        HANDLE event_;
    };
    */
    inline thread_box<void()>& ui_thread()
    {
        static thread_box<void()> uithread;
        return uithread;
    }

    inline void on_ui_thread( const std::function<void()>& fun)
    {
        ui_thread().submit(fun);
    }

    template<class ... Args>
    void task( std::function<void()> fun, Args&& ... args)
    {
        std::thread t(fun,std::forward<Args>(args)... );
        t.detach();
    }
    
}

