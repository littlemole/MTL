#pragma once

#include "mtl/sdk.h"
#include "mtl/punk.h"
#include "mtl/obj/impl.h"
#include "mtl/disp/bstr.h"
#include "mtl/disp/variant.h"
#include "mtl/persist/json.h"

#include <OAIdl.h>

namespace mtl {

	namespace typelib {

		class MetaParam
		{
		public:

			MetaParam() {};
			MetaParam(const std::wstring& n, const std::wstring& t)
				: name(n), type(t)
			{};

			std::wstring name;
			std::wstring type;

			int dispid;

			bool isPointer = false;
			bool isPointerPointer = false;
			bool isRetVal = false;
			bool isIn = false;
			bool isOut = false;

			static constexpr auto meta()
			{
				using namespace ::meta;

				return data(
					entity_root("param"),
					member("name", &MetaParam::name),
					member("type", &MetaParam::type),
					member("dispid", &MetaParam::dispid),
					member("in", &MetaParam::isIn),
					member("out", &MetaParam::isOut),
					member("retval", &MetaParam::isRetVal),
					member("pointer", &MetaParam::isPointer),
					member("pointerpointer", &MetaParam::isPointerPointer)
				);
			}
		};

		//////////////////////////////////////////////////////////////////////////////////////////

		class MetaFunc
		{
		public:

			MetaFunc() {};
			MetaFunc(const std::wstring& n, const std::wstring& t)
				: name(n), type(t)
			{};

			std::wstring name;
			std::wstring type;
			std::wstring funkind;
			std::wstring desc;

			bool isDisp = false;
			bool isStdCall = false;
			bool isPropPut = false;
			bool isPropPutRef = false;
			bool isPropGet = false;

			long dispid = 0;
			long vtindex = 0;

			std::vector<MetaParam> params;
		};

		//////////////////////////////////////////////////////////////////////////////////////////

		class MetaInterface
		{
		public:

			MetaInterface() {};
			MetaInterface(const std::wstring& t, const std::wstring& b, GUID& g)
				: type(t),base(b),guid(mtl::guid_to_string(g))
			{}

			std::wstring type;
			std::wstring base;
			std::wstring desc;
			std::wstring guid;

			bool isDisp = false;
			bool isDual = false;

			std::vector<MetaFunc> functions;
			std::vector<MetaParam> properties;
		};

		class MetaInterfaceRef
		{
		public:

			MetaInterfaceRef() {};
			MetaInterfaceRef(const std::wstring& t)
				: type(t)
			{}

			std::wstring type;

			bool isSource = false;
			bool isDefault = false;
		};

		class MetaClass
		{
		public:

			MetaClass() {};
			MetaClass(const std::wstring& t, GUID& g) 
				: type(t),guid(mtl::guid_to_string(g))
			{};

			std::wstring type;
			std::wstring desc;
			std::wstring guid;

			long major = 0;
			long minor = 0;
			std::wstring progId;

			std::vector<MetaInterfaceRef> interfaces;
		};

		//////////////////////////////////////////////////////////////////////////////////////////

		class MetaEnum
		{
		public:

			MetaEnum() {};
			MetaEnum(const std::wstring& t)
				: type(t)
			{};

			std::wstring type;
			std::wstring desc;

			std::vector<MetaParam> enums;
		};


		//////////////////////////////////////////////////////////////////////////////////////////

		class MetaRecord
		{
		public:

			MetaRecord() {};
			MetaRecord(const std::wstring& t)
				: type(t)
			{}

			std::wstring type;
			std::wstring desc;

			std::vector<MetaParam> members;
		};

		class MetaLibrary
		{
		public:
			long major = 0;
			long minor = 0;
			long count = 0;

			std::wstring name;
			std::wstring desc;
			std::wstring guid;

			std::vector<MetaInterface> interfaces;
			std::vector<MetaClass> objects;
			std::vector<MetaRecord> records;
			std::vector<MetaEnum> enums;

		};
	}
}

/*
template<>
struct meta::Data<mtl::typelib::MetaParam>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("param"),
			member("name", &mtl::typelib::MetaParam::name),
			member("type", &mtl::typelib::MetaParam::type),
			member("in", &mtl::typelib::MetaParam::isIn),
			member("out", &mtl::typelib::MetaParam::isOut),
			member("retval", &mtl::typelib::MetaParam::isRetVal),
			member("pointer", &mtl::typelib::MetaParam::isPointer),
			member("pointerpointer", &mtl::typelib::MetaParam::isPointerPointer)
		);
	}
};
*/

template<>
struct meta::Data<mtl::typelib::MetaFunc>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("function"),
			member("name", &mtl::typelib::MetaFunc::name),
			member("type", &mtl::typelib::MetaFunc::type),
			member("description", &mtl::typelib::MetaFunc::desc),
			member("dispid", &mtl::typelib::MetaFunc::dispid),
			member("funkind", &mtl::typelib::MetaFunc::funkind),
			member("dispatch", &mtl::typelib::MetaFunc::isDisp),
			member("stdcall", &mtl::typelib::MetaFunc::isStdCall),
			//member("pointer", &mtl::typelib::MetaFunc::isPointer),
			member("propget", &mtl::typelib::MetaFunc::isPropGet),
			member("propput", &mtl::typelib::MetaFunc::isPropPut),
			member("propputref", &mtl::typelib::MetaFunc::isPropPutRef),
			//member("retpointer", &mtl::typelib::MetaFunc::isRetPointer),
			member("vtindex", &mtl::typelib::MetaFunc::vtindex),
			member("param", &mtl::typelib::MetaFunc::params)
		);
	}
};


template<>
struct meta::Data<mtl::typelib::MetaInterface>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("interface"),
			member("base", &mtl::typelib::MetaInterface::base),
			member("type", &mtl::typelib::MetaInterface::type),
			member("guid", &mtl::typelib::MetaInterface::guid),
			member("description", &mtl::typelib::MetaInterface::desc),
			member("dispatch", &mtl::typelib::MetaInterface::isDisp),
			member("dual", &mtl::typelib::MetaInterface::isDual),
			member("property", &mtl::typelib::MetaInterface::properties),
			member("function", &mtl::typelib::MetaInterface::functions)
		);
	}
};


template<>
struct meta::Data<mtl::typelib::MetaInterfaceRef>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("interface-ref"),
			member("type", &mtl::typelib::MetaInterfaceRef::type),
			member("default", &mtl::typelib::MetaInterfaceRef::isDefault),
			member("source", &mtl::typelib::MetaInterfaceRef::isSource)
		);
	}
};

template<>
struct meta::Data<mtl::typelib::MetaClass>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("object"),
			member("desc", &mtl::typelib::MetaClass::desc),
			member("type", &mtl::typelib::MetaClass::type),
			member("guid", &mtl::typelib::MetaClass::guid),
			member("major", &mtl::typelib::MetaClass::major),
			member("minor", &mtl::typelib::MetaClass::minor),
			member("progid", &mtl::typelib::MetaClass::progId),
			member("interface", &mtl::typelib::MetaClass::interfaces)
		);
	}
};


template<>
struct meta::Data<mtl::typelib::MetaEnum>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("enum"),
			member("desc", &mtl::typelib::MetaEnum::desc),
			member("type", &mtl::typelib::MetaEnum::type),
			member("enum", &mtl::typelib::MetaEnum::enums)
		);
	}
};



template<>
struct meta::Data<mtl::typelib::MetaRecord>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("record"),
			member("desc", &mtl::typelib::MetaRecord::desc),
			member("type", &mtl::typelib::MetaRecord::type),
			member("member", &mtl::typelib::MetaRecord::members)
		);
	}
};



template<>
struct meta::Data<mtl::typelib::MetaLibrary>
{
	static constexpr auto meta()
	{
		return meta::data(
			entity_root("library"),
			member("desc", &mtl::typelib::MetaLibrary::desc),
			member("name", &mtl::typelib::MetaLibrary::name),
			member("major", &mtl::typelib::MetaLibrary::major),
			member("minor", &mtl::typelib::MetaLibrary::minor),
			member("guid", &mtl::typelib::MetaLibrary::guid),
			member("count", &mtl::typelib::MetaLibrary::count),
			member("interface", &mtl::typelib::MetaLibrary::interfaces),
			member("object", &mtl::typelib::MetaLibrary::objects),
			member("enums", &mtl::typelib::MetaLibrary::enums),
			member("record", &mtl::typelib::MetaLibrary::records)
		);
	}
};

namespace mtl {

	namespace typelib {

		inline const wchar_t* varname(int vt)
		{
			static const wchar_t* varnames[] = {
				L"VT_EMPTY",
				L"VT_NULL",
				L"short", //"VT_I2",
				L"long", //"VT_I4",
				L"float", //"VT_R4",
				L"double", //"VT_R8",
				L"CY", //"VT_CY",
				L"DATE", //"VT_DATE",
				L"BSTR", //"VT_BSTR",
				L"IDispatch*", //"VT_DISPATCH",
				L"VT_ERROR",
				L"VARIANT_BOOL", //"VT_BOOL",
				L"VARIANT", //"VT_VARIANT",
				L"IUnknown*", //"VT_UNKNOWN",
				L"VT_DECIMAL",
				L"!!!! ERRR",
				L"char", //"VT_I1",
				L"unsigned char", //"""VT_UI1",
				L"unsigned short",
				L"unsigned long",
				L"longlong",
				L"unsinged longlong",
				L"int",
				L"unsigned int",
				L"void",
				L"HRESULT",
				L"VT_PTR",
				L"SAFEARRAY",
				L"VT_CARRAY",
				L"VT_USERDEFINED",
				L"char",
				L"wchar_t",
				L"VT_RECORD",
				L"int*",
				L"unsigned int *",
				L"FILETIME",
				L"VT_BLOB",
				L"IStream",
				L"IStorage",
				L"VT_STREAMED_OBJECT",
				L"VT_STORED_OBJECT",
				L"VT_BLOB_OBJECT",
				L"VT_CF",
				L"GUID",
				L"VT_VERSIONED_STREAM",
				L"VT_BSTR_BLOB",
				L"VT_VECTOR",
				L"VT_ARRAY",
				L"VT_BYREF",
				L"VT_RESERVED",
				L"VT_ILLEGAL",
				L"VT_ILLEGALMASKED",
				L"VT_TYPEMASK"
			};

			return varnames[vt];
		}

		inline std::wstring getName(mtl::punk<ITypeInfo>& typeInfo, int index)
		{
			mtl::bstr name;
			if (S_OK == typeInfo->GetDocumentation(index, &name, NULL, NULL, NULL))
			{
				return name.str();
			}
			return L"";
		}

		inline std::wstring getDescription(mtl::punk<ITypeInfo>& typeInfo, int index)
		{
			mtl::bstr desc;
			if (S_OK == typeInfo->GetDocumentation(index, NULL, &desc, NULL, NULL))
			{
				return desc.str();
			}
			return L"";
		}

		inline std::wstring reftypeName(mtl::punk<ITypeInfo>& type_info, HREFTYPE& rt, int index = MEMBERID_NIL)
		{
			mtl::punk<ITypeInfo> t;
			if (S_OK == type_info->GetRefTypeInfo(rt, &t))
			{
				mtl::bstr fname;
				t->GetDocumentation(index, &fname, NULL, 0, NULL);
				return fname.str();
			}
			return L"";
		}

		inline bool isPointer(mtl::punk<ITypeInfo>& type_info, TYPEDESC* td)
		{
			bool result = false;
			VARTYPE vt = td->vt;
			if (vt == VT_PTR)
			{
				result = true;
			}
			return result;
		}

		inline bool isPointerPointer(mtl::punk<ITypeInfo>& type_info, TYPEDESC* td)
		{
			bool result = false;
			VARTYPE vt = td->vt;
			if (vt == VT_PTR)
			{
				vt = td->lptdesc->vt;
				if (vt == VT_PTR)
				{
					result = true;
				}
			}
			return result;
		}

		inline std::wstring getType(mtl::punk<ITypeInfo>& type_info, TYPEDESC* td)
		{
			bool isPointer = false;
			bool isPointerPointer = false;

			VARTYPE vt = td->vt;
			if (vt == VT_PTR)
			{
				isPointer = true;
				vt = td->lptdesc->vt;
				if (vt == VT_PTR)
				{
					isPointerPointer = true;
					vt = td->lptdesc->lptdesc->vt;
					if (vt == VT_USERDEFINED)
					{
						return reftypeName(type_info, td->lptdesc->lptdesc->hreftype) + L"**";
					}
				}
				else
				{
					if (vt == VT_USERDEFINED)
					{
						return reftypeName(type_info, td->lptdesc->hreftype) + L"*";
					}
				}
			}
			else
			{
				if (vt == VT_USERDEFINED)
				{
					return reftypeName(type_info, td->hreftype);
				}
			}
			std::wstring ret = varname(vt);
			if (isPointer)
				ret += L"*";
			if (isPointerPointer)
				ret += L"*";
			return ret;
		}


		inline void parse_funcdesc(mtl::punk<ITypeInfo>& typeInfo, typelib::MetaInterface& itf, int fi)
		{
			FUNCDESC* fd = nullptr;
			typeInfo->GetFuncDesc(fi, &fd);

			if (!fd) return;

			std::wstring name = getName(typeInfo, fd->memid);

			std::wstring retType = getType(typeInfo, &(fd->elemdescFunc.tdesc));

			typelib::MetaFunc metafunc(name, retType);

			std::wstring funKind = L"";
			if (fd->funckind == FUNC_VIRTUAL || fd->funckind == FUNC_PUREVIRTUAL)
			{
				funKind = L"virtual";
			}
			metafunc.funkind = funKind;

			if (fd->callconv == CC_STDCALL)
			{
				metafunc.isStdCall = true;
			}

			int k = fd->invkind;
			switch (k)
			{
			case INVOKE_PROPERTYGET:
			{
				metafunc.isPropGet = true;
				break;
			}
			case INVOKE_PROPERTYPUT:
			{
				metafunc.isPropPut = true;
				break;
			}
			case INVOKE_PROPERTYPUTREF:
			{
				metafunc.isPropPutRef = true;
				break;
			}
			}

			UINT cnames = fd->cParams + 1;
			mtl::bstr* names = new mtl::bstr[cnames];

			if (S_OK != typeInfo->GetNames(fd->memid, (BSTR*)names, cnames, &cnames))
			{
				typeInfo->ReleaseFuncDesc(fd);
				return;
			}

			for (unsigned int j = 1; j < cnames; j++)
			{
				std::wstring pType = getType(typeInfo, &(fd->lprgelemdescParam[j - 1].tdesc));
				typelib::MetaParam metaparam(names[j].str(), pType);

				metaparam.isPointer = isPointer(typeInfo, &(fd->lprgelemdescParam[j - 1].tdesc));
				metaparam.isPointerPointer = isPointerPointer(typeInfo, &(fd->lprgelemdescParam[j - 1].tdesc));

				if (fd->lprgelemdescParam[j - 1].paramdesc.wParamFlags & PARAMFLAG_FRETVAL)
				{
					metaparam.isRetVal = true;
				}
				if (fd->lprgelemdescParam[j - 1].paramdesc.wParamFlags & PARAMFLAG_FIN)
				{
					metaparam.isIn = true;
				}
				if (fd->lprgelemdescParam[j - 1].paramdesc.wParamFlags & PARAMFLAG_FOUT)
				{
					metaparam.isOut = true;
				}
				metafunc.params.push_back(metaparam);
			}

			for (unsigned int j = cnames - 1; j < (unsigned int)fd->cParams; j++)
			{
				std::wstring pType = getType(typeInfo, &(fd->lprgelemdescParam[j].tdesc));
				std::wostringstream oss;
				oss << L"param" << j;

				typelib::MetaParam metaparam(oss.str(), pType);

				metaparam.isPointer = isPointer(typeInfo, &(fd->lprgelemdescParam[j - 1].tdesc));
				metaparam.isPointerPointer = isPointerPointer(typeInfo, &(fd->lprgelemdescParam[j - 1].tdesc));


				if (fd->lprgelemdescParam[j].paramdesc.wParamFlags & PARAMFLAG_FRETVAL)
				{
					metaparam.isRetVal = true;
				}
				if (fd->lprgelemdescParam[j].paramdesc.wParamFlags & PARAMFLAG_FIN)
				{
					metaparam.isIn = true;
				}
				if (fd->lprgelemdescParam[j].paramdesc.wParamFlags & PARAMFLAG_FOUT)
				{
					metaparam.isOut = true;
				}
				metafunc.params.push_back(metaparam);
			}
			delete[] names;

			if (itf.isDisp)
			{
				metafunc.dispid = fd->memid;
			}
			metafunc.vtindex = fd->oVft;

			metafunc.desc = getDescription(typeInfo, fd->memid);

			typeInfo->ReleaseFuncDesc(fd);

			itf.functions.push_back(metafunc);
		}

		inline void parse_typeinfo(typelib::MetaLibrary& result, int index, std::wstring name, std::wstring desc, mtl::punk<ITypeInfo>& typeInfo, mtl::punk<ITypeLib>& lib)
		{
			TYPEATTR* attr;
			TYPEKIND kind = TKIND_ENUM;
			int cVars = 0;
			int cFuncs = 0;
			int cImplTypes = 0;
			int wTypeFlags = 0;
			int major = 0;
			int minor = 0;
			GUID guid;

			if (S_OK == typeInfo->GetTypeAttr(&attr))
			{
				kind = attr->typekind;
				cVars = attr->cVars;
				cFuncs = attr->cFuncs;
				guid = attr->guid;
				cImplTypes = attr->cImplTypes;
				wTypeFlags = attr->wTypeFlags;
				
				major = attr->wMajorVerNum;
				minor = attr->wMinorVerNum;
			}

			typeInfo->ReleaseTypeAttr(attr);

			switch (kind)
			{
			case TKIND_ENUM:
			{
				typelib::MetaEnum metaenum(name);
				metaenum.desc = desc;

				for (int i = 0; i < cVars; i++)
				{
					VARDESC* vd;
					typeInfo->GetVarDesc(i, &vd);

					mtl::variant v(vd->lpvarValue);
					typelib::MetaParam metaparam(getName(typeInfo, vd->memid), v.to_wstring());

					typeInfo->ReleaseVarDesc(vd);

					metaenum.enums.push_back(metaparam);
				}
				result.enums.push_back(metaenum);
				break;
			}
			case TKIND_RECORD:
			{
				typelib::MetaRecord metarecord(name);
				for (int i = 0; i < cVars; i++)
				{
					VARDESC* vd;
					typeInfo->GetVarDesc(i, &vd);

					typelib::MetaParam metaparam(getType(typeInfo, &vd->elemdescVar.tdesc), getName(typeInfo, vd->memid));
					
					typeInfo->ReleaseVarDesc(vd);

					metarecord.members.push_back(metaparam);
				}
				result.records.push_back(metarecord);
				break;
			}
			case TKIND_MODULE:
			{
				break;
			}
			case TKIND_INTERFACE:
			{
				std::wstring base;
				typelib::MetaInterface itf(name, base, guid);
				for (int i = 0; i < cFuncs; i++)
				{
					parse_funcdesc(typeInfo, itf, i);
				}
				result.interfaces.push_back(itf);
				break;
			}
			case TKIND_DISPATCH:
			{
				std::wstring base;
				typelib::MetaInterface disp(name, base, guid);

				// impl types
				for (int i = 0; i < cImplTypes; i++)
				{
					HREFTYPE rt;
					typeInfo->GetRefTypeOfImplType(i, &rt);
					if (rt)
					{
						disp.base = reftypeName(typeInfo, rt);
					}
				}

				HREFTYPE rt;
				if (S_OK == typeInfo->GetRefTypeOfImplType(-1, &rt))
				{
					if (rt)
					{
						disp.isDual = true;
						disp.isDisp = true;

						mtl::punk<ITypeInfo> ti;
						if (S_OK == typeInfo->GetRefTypeInfo(rt, &ti))
						{
							for (int i = 0; i < cFuncs; i++)
							{
								parse_funcdesc(ti, disp, i);
							}
							result.interfaces.push_back(disp);
						}
					}
				}


				// Dispatcheria	
				if (!disp.isDual)
				{
					if (wTypeFlags & TYPEFLAG_FDUAL)
					{
						disp.isDual = true;
						for (int i = 7; i < cFuncs; i++)
						{
							parse_funcdesc(typeInfo, disp, i);
						}

					}
					else
					{
						for (int i = 0; i < cFuncs; i++)
						{
							parse_funcdesc(typeInfo, disp, i);
						}
						for (int i = 0; i < cVars; i++)
						{
							VARDESC* vd;
							typeInfo->GetVarDesc(i, &vd);

							typelib::MetaParam metaparam(getName(typeInfo, vd->memid), getType(typeInfo, &vd->elemdescVar.tdesc) );
							metaparam.dispid = vd->memid;

							typeInfo->ReleaseVarDesc(vd);

							disp.properties.push_back(metaparam);
						}
					}
					result.interfaces.push_back(disp);
				}
				break;
			}
			case TKIND_COCLASS:
			{
				typelib::MetaClass mc(name, guid);
				for (int i = 0; i < cImplTypes; i++)
				{
					HREFTYPE rt;
					if (S_OK == typeInfo->GetRefTypeOfImplType(i, &rt))
					{
						std::wstring n = reftypeName(typeInfo, rt);

						typelib::MetaInterfaceRef ifc;
						ifc.type = n;

						INT flags = 0;
						typeInfo->GetImplTypeFlags(i, &flags);

						if (flags & IMPLTYPEFLAG_FRESTRICTED)
							break;
						if (flags & IMPLTYPEFLAG_FSOURCE)
							ifc.isSource = true;
						if (flags & IMPLTYPEFLAG_FDEFAULT)
							ifc.isDefault = true;

						mc.interfaces.push_back(ifc);
					}
				}

				mc.major = major ? major : result.major;
				mc.minor = minor ? minor : result.minor;;

				mc.progId = result.name + L"." + mc.type;
				bstr doc;
				lib->GetDocumentation(MEMBERID_NIL, 0, &doc, 0, 0);
				mc.desc = doc.str();

				result.objects.push_back(mc);

				break;
			}
			case TKIND_ALIAS:
			{
				//			tl_typedef(ti, i, meta);
				break;
			}
			case TKIND_UNION:
			{
				break;
			}
			}
		}
	}

	inline typelib::MetaLibrary load_typelib(const std::wstring& path)
	{
		typelib::MetaLibrary result;
		result.desc = L"an error occured";
		result.name = L"UNKNOWN";

		mtl::punk<ITypeLib> lib;

		HRESULT hr = ::LoadTypeLibEx(path.c_str(), REGKIND_NONE, &lib);
		if (hr != S_OK)
		{
			//cry();
			return result;
		}

		TLIBATTR* tla;
		hr = lib->GetLibAttr(&tla);

		result.guid = mtl::guid_to_string(tla->guid);
		result.major = tla->wMajorVerNum;
		result.minor = tla->wMinorVerNum;

		lib->ReleaseTLibAttr(tla);

		result.count = lib->GetTypeInfoCount();

		mtl::bstr name;
		if (S_OK == lib->GetDocumentation(-1, &name, NULL, NULL, NULL))
		{
			result.name = name.str();
		}

		mtl::bstr desc;
		if (S_OK == lib->GetDocumentation(-1, NULL, &desc, NULL, NULL))
		{
			result.desc = desc.str();
		}

		for (int i = 0; i < result.count; i++)
		{
			mtl::punk<ITypeInfo> typeInfo;

			if (S_OK == lib->GetTypeInfo(i, &typeInfo))
			{
				mtl::bstr name;
				if (S_OK == lib->GetDocumentation(i, &name, NULL, NULL, NULL))
				{
					//result.name = name.str();
				}

				mtl::bstr desc;
				if (S_OK == lib->GetDocumentation(i, NULL, &desc, NULL, NULL))
				{
					//result.desc = desc.str();
				}

				parse_typeinfo(result,i,name.str(),desc.str(),typeInfo,lib);
			}
		}

		return result;
	}
}