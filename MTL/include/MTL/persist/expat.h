#pragma once

#include "mtl/persist/stream.h"
#include "mtl/disp/bstr.h"
#include "mtl/disp/disp.h"
#include "mtl/disp/variant.h"
#include "mtl/disp/sf_array.h"
#include "mtl/util/base64.h"
#include "metacpp/meta.h"
#include <patex/document.h>

namespace meta {
    namespace impl {

        inline void toXml(const ::meta::EntityName& n, ::mtl::bstr from, patex::xml::ElementPtr to);
        inline void fromXml(const meta::EntityName& name, patex::xml::ElementPtr from, ::mtl::bstr& to);

        inline void toXml(const ::meta::EntityName& n, ::mtl::variant from, patex::xml::ElementPtr to);
        inline void fromXml(const meta::EntityName& name, patex::xml::ElementPtr from, ::mtl::variant& to);

        inline void toXml(const ::meta::EntityName& n, ::mtl::punk<IDispatch> from, patex::xml::ElementPtr to);
        inline void fromXml(const meta::EntityName& name, patex::xml::ElementPtr from, ::mtl::punk<IDispatch>& to);

        inline void toXml(const ::meta::EntityName& n, ::mtl::punk<IUnknown> from, patex::xml::ElementPtr to);
        inline void fromXml(const meta::EntityName& name, patex::xml::ElementPtr from, ::mtl::punk<IUnknown>& to);
    }
}

#include "metacpp/xml.h"


namespace meta {
    namespace impl {

        inline void toXml(const meta::EntityName& n, ::mtl::bstr from, patex::xml::ElementPtr to)
        {
            string2Xml(n, from.to_string(), to);
        }

        inline void toXml(const meta::EntityName& n, ::mtl::variant from, patex::xml::ElementPtr to)
        {
            if (!n.name) return;

            patex::xml::ElementPtr el = meta::impl::createElement(n, to);
            if (el)
            {
                to->appendChild(el);

                std::string vt = std::to_string(from.vt);
                el->setAttribute("vt", vt);

                if (from.vt == VT_EMPTY)
                {
                    return;
                }

                if (from.vt == VT_DISPATCH || from.vt == VT_UNKNOWN)
                {
                    ::mtl::punk<IUnknown> unk(from.punkVal);
                    toXml("object", unk, el);
                    return;
                }

                std::string val = from.to_string();
                if (!val.empty())
                {
                    patex::xml::TextPtr txt = to->ownerDocument()->createTextNode(val);
                    if (txt)
                    {
                        el->appendChild(txt);
                    }
                }
            }
        }

        inline void toXml(const meta::EntityName& n, ::mtl::punk<IDispatch> from, patex::xml::ElementPtr to)
        {
            ::mtl::punk<IUnknown> unk(from);
            toXml(n, unk, to);
        }

        inline void toXml(const meta::EntityName& n, ::mtl::punk<IUnknown> from, patex::xml::ElementPtr to)
        {
            if (!n.name) return;
            if (!from) return;

            patex::xml::ElementPtr el = meta::impl::createElement(n, to);
            if (el)
            {
                to->appendChild(el);

                CLSID clsid;
                ::mtl::punk<IPersistStream> ps(from);
                ps->GetClassID(&clsid);

                std::wstring uuid = ::mtl::guid_to_string(clsid);
                el->setAttribute("clsid", ::mtl::to_string(uuid));

                ::mtl::stream strm;
                ps->Save(*strm, FALSE);

                strm.reset();
                std::string data = mtl::base64_encode(strm.read());
                el->setAttribute("data", data);
            }
        }

        inline void fromXml(const meta::EntityName& name, patex::xml::ElementPtr from, ::mtl::bstr& to)
        {
            to = fromXml(name, from);
        }

        inline void fromXml(const meta::EntityName& name, patex::xml::ElementPtr from, ::mtl::punk<IDispatch>& to)
        {
            ::mtl::punk<IUnknown> unk;
            fromXml(name, from, unk);

            ::mtl::punk<IDispatch> disp(unk);
            to = disp;
        }

        inline void fromXml(const meta::EntityName& name, patex::xml::ElementPtr from, ::mtl::punk<IUnknown>& to)
        {
            to.release();

            patex::xml::ElementPtr el;
            if (name.ns_uri())
            {
                el = from->childNodes()->getChildByNameNS(name.name, name.ns_uri());
            }
            else
            {
                el = from->childNodes()->getChildByName(name.name);
            }

            if (el)
            {
                std::string uuid = el->attr("clsid");
                CLSID clsid = ::mtl::string_to_guid(::mtl::to_wstring(uuid));

                ::mtl::punk<IUnknown> unk;
                HRESULT hr = unk.create_object(clsid);
                if (hr != S_OK)
                {
                    return;
                }

                to = unk;

                std::string data = mtl::base64_decode(el->attr("data"));
                if (data.empty())
                {
                    return;
                }

                ::mtl::stream stream(data);

                ::mtl::punk<IPersistStream> ps(unk);
                if (ps)
                {
                    ps->Load(*stream);
                }
            }
        }

        inline void fromXml(const meta::EntityName& name, patex::xml::ElementPtr from, ::mtl::variant& to)
        {
            to = ::mtl::variant();

            patex::xml::ElementPtr el;
            if (name.ns_uri())
            {
                el = from->childNodes()->getChildByNameNS(name.name, name.ns_uri());
            }
            else
            {
                el = from->childNodes()->getChildByName(name.name);
            }

            if (el)
            {
                std::string vts = el->attr("vt");
                std::istringstream iss(vts);
                VARTYPE vt = VT_EMPTY;
                iss >> vt;
                to.vt = vt;

                if (vt == VT_DISPATCH || vt == VT_UNKNOWN)
                {
                    auto obj = el->getElementByTagName("object");
                    std::string uuid = obj->attr("clsid");
                    CLSID clsid = ::mtl::string_to_guid(::mtl::to_wstring(uuid));

                    ::mtl::punk<IUnknown> unk;
                    HRESULT hr = unk.create_object(clsid);
                    if (hr != S_OK)
                    {
                        return;
                    }

                    ::mtl::punk<IPersistStream> ps(unk);
                    if (ps)
                    {
                        std::string data = mtl::base64_decode(obj->attr("data"));
                        ::mtl::stream strm(data);
                        ps->Load(*strm);
                    }
                    if (vt == VT_UNKNOWN)
                    {
                        to = ::mtl::variant(*unk);
                    }
                    if (vt == VT_DISPATCH)
                    {
                        ::mtl::punk<IDispatch> disp;
                        to = ::mtl::variant(*disp);
                    }
                    return;
                }
                std::string val = el->innerXml();
                switch (vt)
                {
                case VT_BOOL:
                {
                    VARIANT_BOOL vb = VARIANT_FALSE;
                    std::istringstream iss(val);
                    iss >> vb;
                    to.boolVal = vb;
                    break;
                }
                case VT_BSTR:
                {
                    to.bstrVal = ::SysAllocStringLen(::mtl::to_wstring(val).c_str(), (ULONG)val.size());
                    break;
                }
                case VT_I4:
                {
                    std::istringstream iss(val);
                    iss >> to.lVal;
                    break;
                }
                case VT_I2:
                {
                    std::istringstream iss(val);
                    iss >> to.iVal;
                    break;
                }
                case VT_UI4:
                {
                    std::istringstream iss(val);
                    iss >> to.ulVal;
                    break;
                }
                case VT_UI2:
                {
                    std::istringstream iss(val);
                    iss >> to.uiVal;
                    break;
                }
                case VT_R4:
                {
                    std::istringstream iss(val);
                    iss >> to.fltVal;
                    break;
                }
                case VT_R8:
                {
                    std::istringstream iss(val);
                    iss >> to.dblVal;
                    break;
                }

                default:
                {
                    to.vt = VT_EMPTY;
                }
                }
                return;
            }
        }

    }
}


