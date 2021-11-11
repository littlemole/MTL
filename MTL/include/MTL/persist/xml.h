#pragma once

#ifndef _MOL_DEF_GUARD_DEFINE_META_SERIALIZER_MSXML_XXX_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_META_SERIALIZER_MSXML_XXX_DEF_GUARD_


#include "mtl/punk.h"
#include "mtl/disp/disp.h"
#include "mtl/disp/bstr.h"
#include "mtl/disp/variant.h"
#include "mtl/util/base64.h"
#include "mtl/persist/stream.h"

#include "metacpp/meta.h"
#include <msxml6.h>

namespace mtl {

	///////////////////////////////////////////////////////////////////////////////////////////

	using ElementPtr = punk<IXMLDOMElement>;

	class MsxmlDocument
	{
	public:

		MsxmlDocument()
		{}

		MsxmlDocument(const CLSID& clsid)
		{
			create(clsid);
		}

		MsxmlDocument(const std::string& xml, const CLSID& clsid = CLSID_DOMDocument60)
		{
			create(clsid);
			parse(to_wstring(xml));
		}

		MsxmlDocument(const std::wstring& xml, const CLSID& clsid = CLSID_DOMDocument60)
		{
			create(clsid);
			parse(xml);
		}

		void create(const CLSID& clsid = CLSID_DOMDocument60)
		{
			HR hr = doc_.create_object(clsid);
			hr = doc_->put_async(VARIANT_FALSE);
		}

		bool parse(const std::wstring& xml)
		{
			if (!doc_) return false;

			VARIANT_BOOL vbSuccess = VARIANT_FALSE;
			HR hr = doc_->loadXML(*bstr(ole_char(xml.c_str())), &vbSuccess);
			parseSuccess_ = vbSuccess == VARIANT_TRUE;
			return parseSuccess_;
		}

		ElementPtr documentRoot()
		{
			ElementPtr result;
			HR hr = doc_->get_documentElement(&result);
			return result;
		}

		MsxmlDocument& appendChild(ElementPtr el)
		{
			doc_->appendChild(*el, nullptr);
			return *this;
		}

		punk<IXMLDOMNode> import(ElementPtr el, VARIANT_BOOL deep = VARIANT_TRUE)
		{
			punk<IXMLDOMNode> result;
			punk<IXMLDOMDocument3> doc3(doc_);
			if (doc3)
			{
				doc3->importNode(*el, deep, &result);
			}
			return result;
		}

		punk<IXMLDOMDocument> appendChildrenOf(ElementPtr el)
		{
			punk<IXMLDOMNodeList> nodes;
			HR hr = el->get_childNodes(&nodes);

			long len = 0;
			hr = nodes->get_length(&len);
			for (long i = 0; i < len; i++)
			{
				punk<IXMLDOMNode> node;
				HRESULT hr = nodes->get_item(i, &node);
				punk<IXMLDOMNode> tmpNode;
				hr = node->cloneNode(VARIANT_TRUE, &tmpNode);
				doc_->appendChild(*tmpNode, nullptr);
			}
			return doc_;
		}

		operator bool()
		{
			return parseSuccess_;
		}

		IXMLDOMDocument* operator->()
		{
			return *doc_;
		}

		punk<IXMLDOMDocument> operator*()
		{
			return doc_;
		}

	private:
		bool parseSuccess_ = false;
		punk<IXMLDOMDocument> doc_;
	};

	// forwards

	template<class T>
	void toXml(const std::vector<T>& from, ElementPtr to);

	template<class T>
	void toXml(std::vector<T>& from, ElementPtr to);

	template<class T>
	void toXml(const T& from, ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

	template<class T>
	void toXml(T& from, ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);


	namespace impl {

		// forwards

		template<class T>
		void toXml(const meta::EntityName& n, const std::vector<T>& from, ElementPtr to);

		template<class T>
		void toXml(const meta::EntityName& n, std::vector<T>& from, ElementPtr to);

		template<class T>
		void toXml(const meta::EntityName& n, const T& from, ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

		template<class T>
		void toXml(const meta::EntityName& n, T& from, ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

		template<class T>
		void fromXml(const meta::EntityName& name, ElementPtr from, T& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

		template<class T>
		void fromXml(const meta::EntityName& name, ElementPtr from, std::vector<T>& to);

		template<class T>
		void toXml(const meta::EntityName& n, punk<T> from, ElementPtr to);
		void toXml(const meta::EntityName& n, bstr from, ElementPtr to);
		void toXml(const meta::EntityName& n, variant from, ElementPtr to);


		template<class T>
		void fromXml(const meta::EntityName& name, ElementPtr from, punk<T>& to);
		void fromXml(const meta::EntityName& name, ElementPtr from, bstr& to);
		void fromXml(const meta::EntityName& name, ElementPtr from, variant& to);

		///////////////////////////////////////////////////////////////////////////////////////////

		// helpers


		inline ElementPtr createElement(const meta::EntityName& n, ElementPtr to)
		{
			ElementPtr el;
			punk<IXMLDOMDocument> doc;

			punk<IXMLDOMNode> node(to);
			HR hr = node->get_ownerDocument(&doc);

			if (!n.ns_uri())
			{
				hr = doc->createElement(*bstr(n.name), &el);
			}
			else
			{
				bstr ns;
				HR hr = node->get_namespaceURI(&ns);

				if ( to_string(ns.str()) == n.ns_uri())
				{
					hr = doc->createElement(*bstr(n.name), &el);
				}
				else
				{
					punk<IXMLDocument2> doc2(doc);
					punk<IXMLDOMNode> e;
					hr = doc->createNode(variant(NODE_ELEMENT), *bstr(n.name), *bstr( n.ns_uri()), &e );
					el = e;
				}
			}
			return el;
		}

		inline void set_text( punk<IXMLDOMElement> el, std::string val)
		{
			punk<IXMLDOMDocument> doc;
			HR hr = el->get_ownerDocument(&doc);

			bstr value(val.c_str());

			punk<IXMLDOMText> txt;
			hr = doc->createTextNode(*value, &txt);

			el->appendChild(*txt, nullptr);
		}

		inline void set_text(punk<IXMLDOMElement> el, std::wstring val)
		{
			punk<IXMLDOMDocument> doc;
			HR hr = el->get_ownerDocument(&doc);

			bstr value(mtl::ole_char(val.c_str()));

			punk<IXMLDOMText> txt;
			hr = doc->createTextNode(*value, &txt);

			el->appendChild(*txt, nullptr);
		}

		inline void set_attribute(const meta::EntityName& n, const std::string& from, ElementPtr to)
		{
			punk<IXMLDOMDocument> doc;

			punk<IXMLDOMNode> node(to);


			if (n.ns_uri() )
			{
				punk<IXMLDOMNode> a;
				HR hr = doc->createNode(variant(NODE_ATTRIBUTE), *bstr(n.name), *bstr(n.ns_uri()), &a);
				
				punk<IXMLDOMAttribute> at(a);
				hr = to->setAttributeNode( *at, nullptr);	
				hr = a->put_nodeValue(mtl::variant(from.c_str()));
			}
			else
			{
				to->setAttribute(*bstr(n.name), variant(from.c_str()));
			}
		}

		inline void set_attribute(const meta::EntityName& n, const std::wstring& from, ElementPtr to)
		{
			punk<IXMLDOMDocument> doc;

			punk<IXMLDOMNode> node(to);


			if (n.ns_uri())
			{
				punk<IXMLDOMNode> a;
				HR hr = doc->createNode(variant(NODE_ATTRIBUTE), *bstr(n.name), *bstr(n.ns_uri()), &a);

				punk<IXMLDOMAttribute> at(a);
				hr = to->setAttributeNode(*at, nullptr);

				hr = a->put_nodeValue(mtl::variant(mtl::ole_char(from.c_str())));
			}
			else
			{
				to->setAttribute(*bstr(n.name), variant(mtl::ole_char(from.c_str())));
			}
		}

		inline punk<IXMLDOMElement> firstChild(punk<IXMLDOMElement> parent)
		{
			punk<IXMLDOMNodeList> childNodes;
			HR hr = parent->get_childNodes(&childNodes);

			long len = 0;
			hr = childNodes->get_length(&len);
			for (long i = 0; i < len; i++)
			{
				punk<IXMLDOMNode> node;
				hr = childNodes->get_item(i, &node);

				DOMNodeType type;
				hr = node->get_nodeType(&type);
				if (type == NODE_ELEMENT)
				{
					return node;
				}
			}

			punk<IXMLDOMElement> result;
			return result;
		}

		inline punk<IXMLDOMElement> getChildElement(punk<IXMLDOMElement> parent, const char* name, const char* ns_uri = nullptr)
		{
			punk<IXMLDOMNodeList> childNodes;
			HR hr = parent->get_childNodes(&childNodes);

			long len = 0;
			hr = childNodes->get_length(&len);
			for (long i = 0; i < len; i++)
			{
				punk<IXMLDOMNode> node;
				hr = childNodes->get_item(i, &node);

				bstr nodeName;
				node->get_baseName(&nodeName);
				if (nodeName.to_string() == name)
				{
					if (ns_uri)
					{
						bstr ns;
						node->get_namespaceURI(&ns);
						if (ns.to_string() == ns_uri)
						{
							punk<IXMLDOMElement> result(node);
							return result;
						}
					}
					else
					{
						punk<IXMLDOMElement> result(node);
						return result;
					}
				}
			}
			punk<IXMLDOMElement> result;
			return result;
		}

		inline void forEachChildElement( std::function<void(punk<IXMLDOMElement>)> cb, punk<IXMLDOMElement> parent, const char* name, const char* ns_uri = nullptr)
		{
			punk<IXMLDOMNodeList> childNodes;
			HR hr = parent->get_childNodes(&childNodes);

			long len = 0;
			hr = childNodes->get_length(&len);
			for (long i = 0; i < len; i++)
			{
				punk<IXMLDOMNode> node;
				hr = childNodes->get_item(i, &node);

				bstr nodeName;
				node->get_baseName(&nodeName);
				if (nodeName.to_string() == name)
				{
					if (ns_uri)
					{
						bstr ns;
						node->get_namespaceURI(&ns);
						if (ns.to_string() == ns_uri)
						{
							punk<IXMLDOMElement> result(node);
							cb(result);
						}
					}
					else
					{
						punk<IXMLDOMElement> result(node);
						cb( result);
					}
				}
			}
		}

		inline std::string getChildElementText(punk<IXMLDOMElement> parent, const char* name, const char* ns_uri = nullptr)
		{
			punk<IXMLDOMElement> childNode = getChildElement(parent, name, ns_uri);
			if (!childNode)
			{
				return "";
			}

			bstr value;
			childNode->get_text(&value);
			return value.to_string();
		}


		inline std::wstring getChildElementWideText(punk<IXMLDOMElement> parent, const char* name, const char* ns_uri = nullptr)
		{
			punk<IXMLDOMElement> childNode = getChildElement(parent, name, ns_uri);
			if (!childNode)
			{
				return L"";
			}

			bstr value;
			childNode->get_text(&value);
			return value.str();
		}

		inline void string2Xml(const meta::EntityName& n, const std::string& from, ElementPtr to)
		{
			if (!n.name) return;

			if (n.is_attribute())
			{
				set_attribute(n, from, to);
				return;
			}

			ElementPtr el = createElement(n, to);
			if (el)
			{
				to->appendChild( *el, nullptr);
				if (!from.empty())
				{
					set_text(el, from);
				}
			}
		}

		inline void string2Xml(const meta::EntityName& n, const std::wstring& from, ElementPtr to)
		{
			if (!n.name) return;

			if (n.is_attribute())
			{
				set_attribute(n, from, to);
				return;
			}

			ElementPtr el = createElement(n, to);
			if (el)
			{
				to->appendChild(*el, nullptr);
				if (!from.empty())
				{
					set_text(el, from);
				}
			}
		}

		inline std::string fromXml(const meta::EntityName& name, ElementPtr from)
		{
			if (!name.name)
			{
				bstr txt;
				HRESULT hr = from->get_text(&txt);
				if (hr != S_OK) return "";
				return txt.to_string();
			}

			if (name.is_attribute())
			{
				if (name.ns_uri())
				{
					punk<IXMLDOMNode> n(from);
					punk<IXMLDOMNamedNodeMap> nodes;
					HRESULT hr = n->get_attributes(&nodes);
					if (hr != S_OK) return "";

					punk<IXMLDOMNode> a;
					hr = nodes->getQualifiedItem(*bstr(name.name), *bstr(name.ns_uri()), &a);
					if (hr != S_OK) return "";

					bstr txt;
					hr = a->get_text(&txt);
					if (hr != S_OK) return "";
					return txt.to_string();
				}
				else
				{
					variant val;
					from->getAttribute(*bstr(name.name), &val);
					return val.to_string();
				}
			}

			return getChildElementText(from, name.name, name.ns_uri());
		}

		inline std::wstring fromXmlwstring(const meta::EntityName& name, ElementPtr from)
		{
			if (!name.name)
			{
				bstr txt;
				HRESULT hr = from->get_text(&txt);
				if (hr != S_OK) return L"";
				return txt.str();
			}

			if (name.is_attribute())
			{
				if (name.ns_uri())
				{
					punk<IXMLDOMNode> n(from);
					punk<IXMLDOMNamedNodeMap> nodes;
					HRESULT hr = n->get_attributes(&nodes);
					if (hr != S_OK) return L"";

					punk<IXMLDOMNode> a;
					hr = nodes->getQualifiedItem(*bstr(name.name), *bstr(name.ns_uri()), &a);
					if (hr != S_OK) return L"";

					bstr txt;
					hr = a->get_text(&txt);
					if (hr != S_OK) return L"";
					return txt.str();
				}
				else
				{
					variant val;
					from->getAttribute(*bstr(name.name), &val);
					return val.to_wstring();
				}
			}

			return getChildElementWideText(from, name.name, name.ns_uri());
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////

		// toXml terminator

		inline void toXml(const meta::EntityName& n, const std::string& from, ElementPtr to)
		{
			string2Xml(n, from, to);
		}

		inline void toXml(const meta::EntityName& n, const std::wstring& from, ElementPtr to)
		{
			string2Xml(n, from, to);
		}

		inline void toXml(const meta::EntityName& n, std::string& from, ElementPtr to)
		{
			string2Xml(n, from, to);
		}

		inline void toXml(const meta::EntityName& n, std::wstring& from, ElementPtr to)
		{
			string2Xml(n, from, to);
		}

		template<class T>
		void toXml(const meta::EntityName& n, const T& from, ElementPtr to, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
		{
			if (!n.name) return;

			std::ostringstream oss;
			oss << from;

			std::string tmp = oss.str();

			string2Xml(n, tmp, to);
		}

		template<class T>
		void toXml(const meta::EntityName& n, T& from, ElementPtr to, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
		{
			if (!n.name) return;

			std::ostringstream oss;
			oss << from;

			std::string tmp = oss.str();

			string2Xml(n, tmp, to);
		}

		///////////////////////////////////////////////////////////////////////////////////////////

		// fromXml termiinators

		inline void fromXml(const meta::EntityName& name, ElementPtr from, std::string& to)
		{
			to = fromXml(name, from);
		}


		inline void fromXml(const meta::EntityName& name, ElementPtr from, std::wstring& to)
		{
			to = fromXmlwstring(name, from);
		}

		template<class T>
		void fromXml(const meta::EntityName& name, ElementPtr from, T& to, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
		{
			std::istringstream iss(fromXml(name, from));
			iss >> to;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////

		// toXml recursive impl

		template<class T>
		void toXml(const meta::EntityName& n, const T& from, ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type*)
		{
			if (n.name)
			{
				ElementPtr el = createElement(n, to);
				to->appendChild(*el, nullptr);

				auto visitor = [&el](auto n, auto m)
				{
					toXml(n, *m, el);
				};
				meta::visit(from, visitor);
			}
			else
			{
				auto visitor = [&to](auto n, auto m)
				{
					toXml(n, *m, to);
				};
				meta::visit(from, visitor);
			}
		}

		template<class T>
		void toXml(const meta::EntityName& n, T& from, ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type*)
		{
			if (n.name)
			{
				ElementPtr el = createElement(n, to);
				to->appendChild(*el, nullptr);

				auto visitor = [&el](auto n, auto m) mutable
				{
					toXml(n, *m, el);
				};
				meta::visit(from, visitor);
			}
			else
			{
				auto visitor = [&to](auto n, auto m) mutable
				{
					toXml(n, *m, to);
				};
				meta::visit(from, visitor);
			}
		}


		template<class T>
		void toXml(const meta::EntityName& n, const std::vector<T>& from, ElementPtr to)
		{
			for (auto& f : from)
			{
				toXml(n, f, to);
			}
		}

		template<class T>
		void toXml(const meta::EntityName& n, std::vector<T>& from, ElementPtr to)
		{
			for (auto& f : from)
			{
				toXml(n, f, to);
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////

		// fromXml recursive impl

		template<class T>
		void fromXml(const meta::EntityName& name, ElementPtr from, T& to, typename std::enable_if<std::is_class<T>::value>::type*)
		{
			ElementPtr el = from;

			if (name.name)
			{
				ElementPtr tmp = getChildElement(from, name.name, name.ns_uri());
				if (tmp)
				{
					el = tmp;
				}
				else
				{
					return;
				}
			}
			
			auto visitor = [&el](auto n, auto m) mutable
			{
				using value_t = std::remove_reference_t<typename std::remove_reference_t<decltype(m)>::setter_value_t>;
				value_t value;

				fromXml(n, el, value);
				m = value;
			};
			meta::visit(to, visitor);
		}


		template<class T>
		void fromXml(const meta::EntityName& name, ElementPtr from, std::vector<T>& to)
		{
			to.clear();

			forEachChildElement( 
				[&to](punk<IXMLDOMElement> el)
				{
					T t;
					fromXml(meta::EntityName(), el, t);
					to.push_back(std::move(t));
				},
				from,
				name.name, 
				name.ns_uri()
			);
		}


		inline void toXml(const meta::EntityName& n, bstr from, ElementPtr to)
		{
			string2Xml(n, from.to_string(), to);
		}

		inline void toXml(const meta::EntityName& n, variant from, ElementPtr to)
		{
			if (!n.name) return;

			ElementPtr el = createElement(n, to);
			if (el)
			{
				to->appendChild( *el, nullptr);

				std::string vt = std::to_string(from.vt);
				el->setAttribute( *bstr("vt"), variant(vt.c_str()) );

				if (from.vt == VT_EMPTY)
				{
					return;
				}

				if (from.vt == VT_DISPATCH || from.vt == VT_UNKNOWN)
				{
					punk<IUnknown> unk(from.punkVal);
					toXml("object", unk, el);
					return;
				}

				std::string val = from.to_string();
				if (!val.empty())
				{
					set_text(el, val);
				}
			}
		}


		template<class T>
		void toXml(const meta::EntityName& n, punk<T> from, ElementPtr to)
		{
			if (!n.name) return;
			if (!from) return;

			ElementPtr el = createElement(n, to);
			if (el)
			{

				punk<IPersistStream> ps(from);

				CLSID clsid;
				ps->GetClassID(&clsid);

				std::wstring uuid = guid_to_string(clsid);
				el->setAttribute(*bstr("clsid"), variant(ole_char(uuid.c_str())));

				mtl::stream stream;
				ps->Save(*stream, FALSE);

				stream.reset();

				std::string payload = stream.read();
				
				size_t pos = payload.find_first_not_of(" \r\n\t");
				if (pos != std::string::npos)
				{
					if (payload[pos] == '<')
					{
						MsxmlDocument doc(payload);
						if (doc)
						{
							auto root = doc.documentRoot();

							punk<IXMLDOMDocument> elDoc;
							el->get_ownerDocument(&elDoc);
							
							punk<IXMLDOMDocument3> elDoc3(elDoc);
							punk<IXMLDOMNode> node;
							elDoc3->importNode( *root, VARIANT_TRUE, &node);
							
							el->appendChild(*node, nullptr);
							to->appendChild(*el, nullptr);
							return;
						}
					}
				}
				
				std::string data = base64_encode(payload);
				el->setAttribute(*bstr("data"), variant(data.c_str()));
				to->appendChild(*el, nullptr);
			}
		}

		inline void fromXml(const meta::EntityName& name, ElementPtr from, bstr& to)
		{
			to = fromXml(name, from);
		}

		template<class T>
		void fromXml(const meta::EntityName& name, ElementPtr from, punk<T>& to)
		{
			to.release();

			ElementPtr el = getChildElement(from, name.name, name.ns_uri());
			if (el)
			{
				variant attr;
				el->getAttribute(*bstr("clsid"), &attr);

				std::wstring uuid = attr.to_wstring();
				CLSID clsid = ::mtl::string_to_guid(uuid);

				::mtl::punk<IUnknown> unk;
				HRESULT hr = unk.create_object(clsid);
				if (hr != S_OK)
				{
					return;
				}

				to = unk;

				std::string data;
				variant vdata;
				el->getAttribute(*bstr("data"), &vdata);
				if ( !(vdata.vt == VT_EMPTY || vdata.vt == VT_NULL) )
				{
					data = mtl::base64_decode(vdata.to_string());
					if (data.empty())
					{
						return;
					}
				}
				else
				{
					auto child = impl::firstChild(el);
					bstr xml;
					child->get_xml(&xml);
					data = xml.to_string();
					if (data.empty())
					{
						return;
					}
				}

				::mtl::stream stream(data);

				::mtl::punk<IPersistStream> ps(unk);
				if (ps)
				{
					ps->Load(*stream);
				}
			}
		}
	
		inline void fromXml(const meta::EntityName& name, ElementPtr from, variant& to)
		{
			to = variant();

			ElementPtr el;

			el = getChildElement(from, name.name, name.ns_uri());

			if (el)
			{
				variant vts;
				el->getAttribute(*bstr("vt"), &vts);
				std::istringstream iss(vts.to_string());
				VARTYPE vt = VT_EMPTY;
				iss >> vt;
				to.vt = vt;

				if (vt == VT_DISPATCH || vt == VT_UNKNOWN)
				{
					ElementPtr obj = getChildElement(el, "object");
					if (obj)
					{
						variant attr;
						obj->getAttribute(*bstr("clsid"), &attr);
						CLSID clsid = string_to_guid(attr.to_wstring());

						punk<IUnknown> unk;
						HRESULT hr = unk.create_object(clsid);
						if (hr != S_OK)
						{
							return;
						}

						punk<IPersistStream> ps(unk);
						if (ps)
						{
							std::string data;

							variant vdata;
							obj->getAttribute(*bstr("data"), &vdata);
							if( !( vdata.vt == VT_EMPTY || vdata.vt == VT_NULL))
							{
								data = base64_decode(vdata.to_string());
							}
							else
							{
								auto child = impl::firstChild(obj);
								bstr xml;
								child->get_xml(&xml);
								data = xml.to_string();
							}
							stream strm(data);
							ps->Load(*strm);
						}
						if (vt == VT_UNKNOWN)
						{
							to = variant(*unk);
						}
						if (vt == VT_DISPATCH)
						{
							punk<IDispatch> disp;
							to = variant(*disp);
						}
						return;

					}
				}

				bstr vval;
				el->get_text(&vval);
				std::string val = vval.to_string();
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
						to.bstrVal = ::SysAllocStringLen(to_wstring(val).c_str(), (ULONG)val.size());
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

	} // end namespace impl

	///////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////

	// toXml API

	template<class T>
	void toXml(const T& from, ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type*)
	{
		const auto& r = meta::entity_root<T>();

		impl::toXml(r, from, to);
	}


	template<class T>
	void toXml(T& from, ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type*)
	{
		const auto& r = meta::entity_root<T>();

		impl::toXml(r, from, to);
	}

	template<class T>
	punk<IXMLDOMDocument> toXml(const T& t)
	{
		MsxmlDocument doc(CLSID_DOMDocument60);

		punk<IXMLDOMElement> el;
		HR hr = doc->createElement(*bstr("dummy"), &el);

		toXml(t, el );

		return doc.appendChildrenOf(el);
	}

	template<class T>
	punk<IXMLDOMDocument> toXml(T& t)
	{
		MsxmlDocument doc(CLSID_DOMDocument60);

		punk<IXMLDOMElement> el;
		HR hr = doc->createElement( *bstr("dummy"),&el);

		toXml(t, el);

		return doc.appendChildrenOf(el);
	}

	template<class T>
	void toXml(const std::vector<T>& from, ElementPtr to)
	{
		auto r = meta::entity_root<std::vector<T>>();
		if (!r.name) return;

		auto el = impl::createElement(r, to);
		to->appendChild(el);

		for (auto& i : from)
		{
			toXml(i, el);
		}
	}

	template<class T>
	void toXml(std::vector<T>& from, ElementPtr to)
	{
		auto r = meta::entity_root<std::vector<T>>();
		if (!r.name) return;

		auto el = impl::createElement(r, to);
		to->appendChild(el);

		for (auto& i : from)
		{
			toXml(i, el);
		}
	}

	//////////////////////////////////////////////////////////////

	// fromXml Api

	template<class T>
	void fromXml(ElementPtr from, T& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr)
	{
		const auto& r = meta::entity_root<T>();

		impl::fromXml(r, from, to);
		return;
	}


	template<class T>
	void fromXml(ElementPtr from, std::vector<T>& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr)
	{
		const auto& r = meta::entity_root<std::vector<T>>();
		const auto& rt = meta::entity_root<T>();

		if (r.name)
		{
			ElementPtr el = from;
			ElementPtr tmp = impl::getChildElement(el, r.name, r.ns_uri());
			if (tmp)
			{
				el = tmp;
			}

			impl::fromXml(rt, el, to);
			return;
		}
	}

	template<class T>
	void fromXml(punk<IXMLDOMDocument> doc, T& t)
	{
		punk<IXMLDOMElement> root;
		HR hr = doc->get_documentElement(&root);

		punk<IXMLDOMElement> dummy;
		hr = doc->createElement(*bstr("dummy"), &dummy);
		hr = dummy->appendChild(*root, nullptr);

		fromXml(dummy, t);
	}


	template<class T>
	void fromXml(const std::string& xml, T& t)
	{
		MsxmlDocument doc(xml);
		if (doc)
		{
			fromXml(*doc, t);
		}
	}

	//////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////

} // end namespace mtl

#endif

