#pragma once

#include "MTL/persist/xml.h"

namespace MTL {

    template<class T>
    class PersistStream : public IPersistStreamInit
    {
    public:

        virtual HRESULT __stdcall GetClassID(CLSID* pClassID) override
        {
            if (!pClassID)
                return E_INVALIDARG;

            *pClassID = __uuidof(T);
            return S_OK;
        }

        virtual HRESULT __stdcall IsDirty(void) override
        {
            return S_FALSE;
        }

        virtual HRESULT __stdcall Load(IStream* pStm) override
        {
            stream_view stream(pStm);
            std::string xml = stream.read();
            fromXml(xml, *(T*)this);
            return S_OK;
        }

        virtual HRESULT __stdcall Save(IStream* pStm, BOOL fClearDirty) override
        {
            
            punk<IXMLDOMDocument> doc = toXml((*(T*)this));
            punk<IXMLDOMElement> root;
            doc->get_documentElement(&root);
            bstr xml;
            root->get_xml(&xml);
            
            stream_view sw(pStm);
            sw.size(0);
            sw.write(xml.to_string());
            return S_OK;
        }

        virtual HRESULT __stdcall GetSizeMax(ULARGE_INTEGER* pcbSize) override
        {
            punk<IXMLDOMDocument> doc = toXml((*(T*)this));
            punk<IXMLDOMElement> root;
            doc->get_documentElement(&root);
            bstr xml;
            root->get_xml(&xml);
            pcbSize->QuadPart = xml.to_string().size();
            return S_OK;
        }

        virtual HRESULT __stdcall InitNew(void) override
        {
            return S_OK;
        }

    };

    class streamable {};

    namespace details
    {
        template< class T, class ... Args>
        class derives<T(streamable, Args...)> : public PersistStream<T>, public derives<T(Args...)>
        {};

        template<class T, class ... Args>
        class interfaces<T(streamable, Args...)> : public interfaces<T(PersistStream<T>, Args...)>
        {};

        template<class T, class ... Args>
        class interfaces<T(PersistStream<T>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                if (::IsEqualGUID(riid, IID_IPersist))
                {
                    *ppvObject = (IPersist*)(IPersistStreamInit*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, IID_IPersistStreamInit))
                {
                    *ppvObject = (IPersistStreamInit*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, IID_IPersistStream))
                {
                    *ppvObject = (IPersistStream*)(IPersistStreamInit*)(that);
                    that->AddRef();
                    return S_OK;
                }

                return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
            }
        };
    }


    template<class T>
    class PersistFile : public IPersistFile
    {
    public:

        virtual HRESULT __stdcall GetClassID(CLSID* pClassID) override
        {
            if (!pClassID)
                return E_INVALIDARG;

            *pClassID = __uuidof(T);
            return S_OK;
        }

        virtual HRESULT __stdcall IsDirty(void) override
        {
            return S_FALSE;
        }

        virtual HRESULT __stdcall Load(LPCOLESTR pszFileName, DWORD dwMode) override
        {
            filename_ = pszFileName;
            Path p(pszFileName);
            Stream stream(p, dwMode);
            if (!stream)
            {
                return E_FAIL;
            }

            std::string xml = stream.read();
            
            MsxmlDocument doc(xml);
            if (doc)
            {
                punk<IXMLDOMElement> root = doc.documentRoot();                    
                punk<IXMLDOMNode> node(root);

                bstr bname;
                node->get_baseName(&bname);
                if (bname.to_string() == "object")
                {
                    auto child = impl::firstChild(root);
                    bstr xml;
                    child->get_xml(&xml);

                    Stream mem(xml.to_string());
                    T* that = (T*)this;
                    return ((IPersistStreamInit*)that)->Load(*mem);
                }
            }

            return E_FAIL;
        }

        virtual HRESULT __stdcall Save(LPCOLESTR pszFileName, BOOL fRemember) override
        {
            if (fRemember)
            {
                filename_ = pszFileName;
            }

            Path p(pszFileName);
            Stream stream(p);
            if (!stream)
            {
                return E_FAIL;
            }
            stream.size(0);

            MsxmlDocument doc(CLSID_DOMDocument60);

            punk<IXMLDOMElement> obj;
            HR hr = doc->createElement(*bstr("object"), &obj);

            CLSID clsid = __uuidof(T);
            std::wstring uuid = guid_to_string(clsid);
            hr = obj->setAttribute(*bstr("clsid"), variant(ole_char(uuid.c_str())));
            doc.appendChild( obj);

            T* that = (T*)this;
            Stream tmp;
            hr = ((IPersistStreamInit*)(that))->Save(*tmp, FALSE);

            tmp.reset();
            std::string xml = tmp.read();

            MsxmlDocument tmpDoc(xml);
            if (tmpDoc)
            {
                punk<IXMLDOMElement> tmpRoot = tmpDoc.documentRoot();
                obj->appendChild(*tmpRoot, nullptr);

                bstr tmpXml;
                hr = doc->get_xml(&tmpXml);

                std::string result = tmpXml.to_string();
                return stream->Write(result.data(), (ULONG)result.size(), 0);
            }
            return E_FAIL;
        }

        virtual HRESULT __stdcall SaveCompleted(LPCOLESTR pszFileName) override
        {
            if (pszFileName)
            {
                filename_ = pszFileName;
            }
            return S_OK;
        }

        virtual HRESULT __stdcall GetCurFile(LPOLESTR* ppszFileName) override
        {
            if (!ppszFileName)
                return E_INVALIDARG;

            *ppszFileName = (LPOLESTR)::CoTaskMemAlloc((filename_.size() + 1) * sizeof(wchar_t));
            memcpy(*ppszFileName, filename_.c_str(), (filename_.size() + 1) * sizeof(wchar_t));
            return S_OK;
        }

    protected:
        std::wstring filename_;
    };


    class serializable {};
    namespace details
    {
        template< class T, class ... Args>
        class derives<T(serializable, Args...)> : public PersistFile<T>, public derives<T(Args...)>
        {};

        template<class T, class ... Args>
        class interfaces<T(serializable, Args...)> : public interfaces<T(PersistFile<T>, Args...)>
        {};

        template<class T, class ... Args>
        class interfaces<T(PersistFile<T>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                if (::IsEqualGUID(riid, IID_IPersist))
                {
                    *ppvObject = (IPersist*)(IPersistFile*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, IID_IPersistFile))
                {
                    *ppvObject = (IPersistFile*)(that);
                    that->AddRef();
                    return S_OK;
                }

                return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
            }
        };
    }

    template<class T>
    class PersistStorage : public IPersistStorage
    {
    public:

        virtual HRESULT __stdcall GetClassID(CLSID* pClassID) override
        {
            if (!pClassID)
                return E_INVALIDARG;

            *pClassID = __uuidof(T);
            return S_OK;
        }

        virtual HRESULT __stdcall IsDirty(void) override
        {
            return S_FALSE;
        }

        virtual HRESULT __stdcall InitNew(IStorage* pStg) override
        {
            // storage = pStg;
            return S_OK;
        }

        virtual HRESULT __stdcall Load(IStorage* pStg) override
        {
            storage_view storage(pStg);

            auto stream = storage.open_stream(L"objectstore");

            CLSID clsid;
            HRESULT hr = ::ReadClassStm(*stream, &clsid);

            T* that = (T*)this;
            hr = ((IPersistStreamInit*)that)->Load(*stream);
            return hr;
        }

        virtual HRESULT __stdcall Save(IStorage* pStgSave, BOOL fSameAsLoad) override
        {
            storage_view storage(pStgSave);
            auto stream = storage.create_stream(L"objectstore");

            HRESULT hr = ::WriteClassStm(*stream, __uuidof(T));
            if (hr != S_OK)
            {
                return hr;
            }

            IPersistStreamInit* t = (IPersistStreamInit*)(T*)this;
            hr = t->Save(*stream, FALSE);
            if (hr != S_OK)
            {
                return hr;
            }

            storage.commit();
            return hr;
        }

        virtual HRESULT __stdcall SaveCompleted(IStorage* pStgNew) override
        {
            return S_OK;
        }

        virtual HRESULT __stdcall HandsOffStorage(void) override
        {
            return S_OK;
        }

    };


    class storable {};
    namespace details
    {

        template< class T, class ... Args>
        class derives<T(storable, Args...)> : public PersistStorage<T>, public derives<T(Args...)>
        {};

        template<class T, class ... Args>
        class interfaces<T(storable, Args...)> : public interfaces<T(PersistStorage<T>, Args...)>
        {};

        template<class T, class ... Args>
        class interfaces<T(PersistStorage<T>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                if (::IsEqualGUID(riid, IID_IPersist))
                {
                    *ppvObject = (IPersist*)(IPersistStorage*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, IID_IPersistStorage))
                {
                    *ppvObject = (IPersistStorage*)(that);
                    that->AddRef();
                    return S_OK;
                }

                return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
            }
        };
    }


    template<class T>
    class Persistent : public PersistStream<T>, public PersistFile<T>, public PersistStorage<T>
    {
    public:

        virtual HRESULT __stdcall GetClassID(CLSID* pClassID) override
        {
            if (!pClassID)
                return E_INVALIDARG;

            *pClassID = __uuidof(T);
            return S_OK;
        }

        virtual HRESULT __stdcall IsDirty(void) override
        {
            return isDirty_ ? S_OK : S_FALSE;
        }

        virtual HRESULT __stdcall Save(IStream* pStm, BOOL fClearDirty) override
        {
            HRESULT hr = PersistStream<T>::Save(pStm, fClearDirty);
            if (hr != S_OK)
            {
                return hr;
            }
            if (fClearDirty)
            {
                isDirty_ = false;
            }
            return S_OK;
        }

        void setDirty(bool b)
        {
            isDirty_ = b;
        }

    protected:
        bool isDirty_ = false;
    };


    class persistable {};
    namespace details
    {

        template< class T, class ... Args>
        class derives<T(persistable, Args...)> : public Persistent<T>, public derives<T(Args...)>
        {};

        template<class T, class ... Args>
        class interfaces<T(persistable, Args...)> : public interfaces<T(Persistent<T>, Args...)>
        {};

        template<class T, class ... Args>
        class interfaces<T(Persistent<T>, Args...)>
        {
        public:

            static HRESULT __stdcall QueryInterface(T* that, REFIID riid, void** ppvObject)
            {
                if (::IsEqualGUID(riid, IID_IPersist))
                {
                    *ppvObject = (IPersist*)(IPersistStorage*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, IID_IPersistStorage))
                {
                    *ppvObject = (IPersistStorage*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, IID_IPersistFile))
                {
                    *ppvObject = (IPersistFile*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, IID_IPersistStream))
                {
                    *ppvObject = (IPersistStream*)(IPersistStreamInit*)(that);
                    that->AddRef();
                    return S_OK;
                }

                if (::IsEqualGUID(riid, IID_IPersistStreamInit))
                {
                    *ppvObject = (IPersistStreamInit*)(that);
                    that->AddRef();
                    return S_OK;
                }

                return interfaces<T(Args...)>::QueryInterface(that, riid, ppvObject);
            }
        };
    }


}

