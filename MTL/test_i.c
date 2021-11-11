

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 04:14:07 2038
 */
/* Compiler settings for tests\test.idl:
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

MIDL_DEFINE_GUID(IID, IID_ITestLongCollection,0xa03d1420,0xb1ec,0x11d0,0x8c,0x3a,0x00,0xc0,0x4f,0xc3,0x1d,0x2f);


MIDL_DEFINE_GUID(IID, IID_ITestSimpleUnknown,0x6FB050D8,0x438A,0x41B8,0xA5,0x6E,0x26,0xD6,0xF3,0x44,0x4C,0xE6);


MIDL_DEFINE_GUID(IID, IID_ISubObject,0xB555296F,0xD09B,0x4A4E,0xA1,0x48,0xE0,0x71,0x53,0x94,0xA5,0x25);


MIDL_DEFINE_GUID(IID, IID_ITestObject,0xFE795DAE,0x119A,0x4141,0x96,0xD3,0xF1,0xDE,0xDB,0x5B,0x22,0x59);


MIDL_DEFINE_GUID(IID, LIBID_ExampleLib,0xa03d1423,0xb1ec,0x11d0,0x8c,0x3a,0x00,0xc0,0x4f,0xc3,0x1d,0x2f);


MIDL_DEFINE_GUID(IID, IID_ITestSimpleOleAout,0xC54DDE14,0x1DF9,0x436D,0xB6,0xEF,0x5B,0xF0,0x23,0xED,0x16,0xE2);


MIDL_DEFINE_GUID(IID, DIID_ITestSimpleDispatch,0xFFE85602,0xBA26,0x414A,0x87,0x0E,0x83,0xCC,0xFB,0x74,0xC9,0x5A);


MIDL_DEFINE_GUID(CLSID, CLSID_MyLongCollection,0xa03d1424,0xb1ec,0x11d0,0x8c,0x3a,0x00,0xc0,0x4f,0xc3,0x1d,0x2f);


MIDL_DEFINE_GUID(CLSID, CLSID_TestObject,0xBA446D99,0x6A2F,0x4DE2,0xB6,0x44,0x10,0x10,0x33,0xA0,0x25,0x0E);


MIDL_DEFINE_GUID(CLSID, CLSID_SubObject,0xC40BFA01,0x0FE0,0x4E55,0x83,0x99,0xDE,0x86,0x2B,0xE9,0xF4,0xCB);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



