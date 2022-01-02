

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 04:14:07 2038
 */
/* Compiler settings for Editor.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        EXTERN_C __declspec(selectany) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IMTLHtmlDocument,0xE0EA2D05,0xDC44,0x4C1B,0xAC,0x13,0xE8,0x78,0xB3,0x58,0xFB,0xAB);


MIDL_DEFINE_GUID(IID, IID_IMTLEditorDocument,0x5AE58F46,0x47DC,0x41F8,0x88,0x3A,0x54,0xB7,0x71,0x9A,0x2F,0xDB);


MIDL_DEFINE_GUID(IID, IID_IMTLEditorDocuments,0x2CA50833,0xCE2D,0x4543,0xB9,0xE6,0x46,0xDF,0x98,0x3C,0x00,0xE5);


MIDL_DEFINE_GUID(IID, IID_IMTLEditor,0xBE5C3CDD,0x1E0A,0x4ECA,0xA0,0xFA,0xFB,0x9E,0x28,0x13,0x93,0x2F);


MIDL_DEFINE_GUID(IID, IID_IMTLScriptHostObject,0x4F2CB374,0x9FA4,0x4135,0xA9,0x8C,0xC8,0x2B,0x0F,0x42,0x5C,0xD6);


MIDL_DEFINE_GUID(IID, LIBID_MTLEditor,0x0F7E34D4,0x236B,0x421E,0x84,0x37,0xB9,0xAD,0xA5,0x9D,0x60,0xAE);


MIDL_DEFINE_GUID(CLSID, CLSID_MTLEditor,0xA258064A,0x8664,0x42F0,0x96,0x76,0xE2,0xED,0xEB,0x23,0xE3,0xA9);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



