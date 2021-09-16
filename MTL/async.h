#pragma once

#include <MTL/sdk.h>
#include <MTL/punk.h>
#include <MTL/obj/impl.h>

namespace MTL {

    namespace details {

        template<class I>
        class CallObject : public implements<CallObject<I>(ISynchronize)>
        {
        public:

            punk<I> async;
            punk<ISynchronize> sync;
            punk<IUnknown> inner;
            std::function<void(I*)> callback;

            bool signaled = false;

            CallObject(IUnknown* unk)
            {
                punk<ICallFactory> cf(unk);
                punk<IUnknown> ua;
                this->AddRef();
                HR hr = cf->CreateCall(__uuidof(I), (IUnknown*)(ISynchronize*)this, __uuidof(IUnknown), &ua);
                //this->Release();

                punk<I> a(ua);
                punk<ISynchronize> s(ua);

                inner = ua;
                async = a;
                sync = s;
            }

            ~CallObject()
            {}

            HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override
            {
                if (::IsEqualGUID(riid, __uuidof(ISynchronize)))
                {
                    *ppvObject = (ISynchronize*)(this);
                    this->AddRef();
                    return S_OK;
                }
                if (::IsEqualGUID(riid, __uuidof(IUnknown)))
                {
                    *ppvObject = (IUnknown*)(ISynchronize*)(this);
                    this->AddRef();
                    return S_OK;
                }

                HRESULT hr = inner->QueryInterface(riid, ppvObject);
                return hr;
            }

            // Inherited via implements
            virtual HRESULT __stdcall Wait(DWORD dwFlags, DWORD dwMilliseconds) override
            {
                return sync->Wait(dwFlags, dwMilliseconds);
            }

            virtual HRESULT __stdcall Signal(void) override
            {
                HRESULT hr = sync->Signal();

                if (callback)
                {
                    std::cout << "eager DONE" << std::endl;
                    callback(*async);

                    this->async.release();
                    this->sync.release();
                    this->inner.release();

                    this->Release();
                    return S_OK;
                }

                signaled = true;
                return S_OK;
            }

            virtual HRESULT __stdcall Reset(void) override
            {
                return sync->Reset();
            }

            void then(std::function<void(AsyncITestComExe*)> cb)
            {
                callback = cb;
                if (signaled)
                {
                    signaled = false;
                    callback(*async);
                    this->async.release();
                    this->sync.release();
                    this->inner.release();

                    this->Release();

                }
            }

        };

        template<class I>
        class async_com_holder
        {
        public:
            async_com_holder(CallObject<I>* c)
                : co(c)
            {}

            auto operator->()
            {
                return co->async;
            }

            template<class F>
            void then(F f)
            {
                co->then(f);
            }

        private:
            CallObject<I>* co;
        };

    } // end namespace details

    template<class I>
    details::async_com_holder<I> async_com(IUnknown* unk)
    {
        details::CallObject<I>* co = new details::CallObject<I>(unk);
        return co;
    }

}