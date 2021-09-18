#pragma once

#include <mtl/sdk.h>
#include <mtl/punk.h>
#include <mtl/obj/impl.h>

namespace mtl {

    namespace details {

        template<class I>
        class call_object : public implements<call_object<I>(ISynchronize)>
        {
        public:

            punk<I> async;
            punk<ISynchronize> sync;
            punk<IUnknown> inner;
            std::function<void(I*)> callback;

            bool signaled = false;

            call_object(IUnknown* unk)
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

            ~call_object()
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

            void then(std::function<void(I*)> cb)
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
            async_com_holder(call_object<I>* c)
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
            call_object<I>* co;
        };

    } // end namespace details

    template<class I>
    details::async_com_holder<I> async_com(IUnknown* unk)
    {
        details::call_object<I>* co = new details::call_object<I>(unk);
        return co;
    }

}