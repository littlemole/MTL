

/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if defined(_M_AMD64)


#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#include "ndr64types.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "test_h.h"

#define TYPE_FORMAT_STRING_SIZE   1245                              
#define PROC_FORMAT_STRING_SIZE   621                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   2            

typedef struct _test_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } test_MIDL_TYPE_FORMAT_STRING;

typedef struct _test_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } test_MIDL_PROC_FORMAT_STRING;

typedef struct _test_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } test_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

static const RPC_SYNTAX_IDENTIFIER  _NDR64_RpcTransferSyntax = 
{{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}};



extern const test_MIDL_TYPE_FORMAT_STRING test__MIDL_TypeFormatString;
extern const test_MIDL_PROC_FORMAT_STRING test__MIDL_ProcFormatString;
extern const test_MIDL_EXPR_FORMAT_STRING test__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITestLongCollection_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITestLongCollection_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITestSimpleUnknown_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITestSimpleUnknown_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISubObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISubObject_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ITestObject_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ITestObject_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE NDR64_UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const test_MIDL_PROC_FORMAT_STRING test__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure get_Value */


	/* Procedure get_Count */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */


	/* Parameter cnt */

/* 26 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 28 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 32 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 34 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 36 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Item */

/* 38 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 40 */	NdrFcLong( 0x0 ),	/* 0 */
/* 44 */	NdrFcShort( 0x8 ),	/* 8 */
/* 46 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 48 */	NdrFcShort( 0x8 ),	/* 8 */
/* 50 */	NdrFcShort( 0x24 ),	/* 36 */
/* 52 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 54 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 64 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 66 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 68 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cnt */

/* 70 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 72 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 74 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 76 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 78 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 80 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure _Enum */

/* 82 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 84 */	NdrFcLong( 0x0 ),	/* 0 */
/* 88 */	NdrFcShort( 0x9 ),	/* 9 */
/* 90 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 92 */	NdrFcShort( 0x0 ),	/* 0 */
/* 94 */	NdrFcShort( 0x8 ),	/* 8 */
/* 96 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 98 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */
/* 102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter enumerator */

/* 108 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 110 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 112 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 114 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 116 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Count */

/* 120 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 122 */	NdrFcLong( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x3 ),	/* 3 */
/* 128 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 132 */	NdrFcShort( 0x24 ),	/* 36 */
/* 134 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 136 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 144 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */

/* 146 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 148 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 152 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 154 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 156 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Item */

/* 158 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 160 */	NdrFcLong( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x4 ),	/* 4 */
/* 166 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 168 */	NdrFcShort( 0x8 ),	/* 8 */
/* 170 */	NdrFcShort( 0x24 ),	/* 36 */
/* 172 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 174 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 182 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 184 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 186 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 188 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cnt */

/* 190 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 192 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 194 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 196 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 198 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 200 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Name */

/* 202 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 204 */	NdrFcLong( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0x7 ),	/* 7 */
/* 210 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 214 */	NdrFcShort( 0x8 ),	/* 8 */
/* 216 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 218 */	0xa,		/* 10 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 220 */	NdrFcShort( 0x1 ),	/* 1 */
/* 222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 226 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */

/* 228 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 230 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 232 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Return value */

/* 234 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 236 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Name */

/* 240 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 242 */	NdrFcLong( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x8 ),	/* 8 */
/* 248 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 252 */	NdrFcShort( 0x8 ),	/* 8 */
/* 254 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 256 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 260 */	NdrFcShort( 0x1 ),	/* 1 */
/* 262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */

/* 266 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 268 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 270 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */

	/* Return value */

/* 272 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 274 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 276 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Value */

/* 278 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 280 */	NdrFcLong( 0x0 ),	/* 0 */
/* 284 */	NdrFcShort( 0x9 ),	/* 9 */
/* 286 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0x8 ),	/* 8 */
/* 292 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 294 */	0xa,		/* 10 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 296 */	NdrFcShort( 0x1 ),	/* 1 */
/* 298 */	NdrFcShort( 0x0 ),	/* 0 */
/* 300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 302 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */

/* 304 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 306 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 308 */	NdrFcShort( 0x4bc ),	/* Type Offset=1212 */

	/* Return value */

/* 310 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 312 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 314 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Value */

/* 316 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 318 */	NdrFcLong( 0x0 ),	/* 0 */
/* 322 */	NdrFcShort( 0xa ),	/* 10 */
/* 324 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 328 */	NdrFcShort( 0x8 ),	/* 8 */
/* 330 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 332 */	0xa,		/* 10 */
			0xc5,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance has big byval param */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */
/* 336 */	NdrFcShort( 0x1 ),	/* 1 */
/* 338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 340 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */

/* 342 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 344 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 346 */	NdrFcShort( 0x4ce ),	/* Type Offset=1230 */

	/* Return value */

/* 348 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 350 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Value */

/* 354 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 356 */	NdrFcLong( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x8 ),	/* 8 */
/* 362 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 364 */	NdrFcShort( 0x8 ),	/* 8 */
/* 366 */	NdrFcShort( 0x8 ),	/* 8 */
/* 368 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 370 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 378 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */

/* 380 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 382 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 384 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 386 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 388 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 390 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Desc */

/* 392 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 394 */	NdrFcLong( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x9 ),	/* 9 */
/* 400 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 404 */	NdrFcShort( 0x8 ),	/* 8 */
/* 406 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 408 */	0xa,		/* 10 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 410 */	NdrFcShort( 0x1 ),	/* 1 */
/* 412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 416 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */

/* 418 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 420 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 422 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Return value */

/* 424 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 426 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 428 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Desc */

/* 430 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 432 */	NdrFcLong( 0x0 ),	/* 0 */
/* 436 */	NdrFcShort( 0xa ),	/* 10 */
/* 438 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 442 */	NdrFcShort( 0x8 ),	/* 8 */
/* 444 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 446 */	0xa,		/* 10 */
			0x45,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance */
/* 448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 450 */	NdrFcShort( 0x1 ),	/* 1 */
/* 452 */	NdrFcShort( 0x0 ),	/* 0 */
/* 454 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */

/* 456 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 458 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 460 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */

	/* Return value */

/* 462 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 464 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 466 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Type */

/* 468 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 470 */	NdrFcLong( 0x0 ),	/* 0 */
/* 474 */	NdrFcShort( 0xb ),	/* 11 */
/* 476 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 480 */	NdrFcShort( 0x8 ),	/* 8 */
/* 482 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 484 */	0xa,		/* 10 */
			0x43,		/* Ext Flags:  new corr desc, clt corr check, has range on conformance */
/* 486 */	NdrFcShort( 0x1 ),	/* 1 */
/* 488 */	NdrFcShort( 0x0 ),	/* 0 */
/* 490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 492 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */

/* 494 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 496 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 498 */	NdrFcShort( 0x4bc ),	/* Type Offset=1212 */

	/* Return value */

/* 500 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 502 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Type */

/* 506 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 508 */	NdrFcLong( 0x0 ),	/* 0 */
/* 512 */	NdrFcShort( 0xc ),	/* 12 */
/* 514 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 518 */	NdrFcShort( 0x8 ),	/* 8 */
/* 520 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 522 */	0xa,		/* 10 */
			0xc5,		/* Ext Flags:  new corr desc, srv corr check, has range on conformance has big byval param */
/* 524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 526 */	NdrFcShort( 0x1 ),	/* 1 */
/* 528 */	NdrFcShort( 0x0 ),	/* 0 */
/* 530 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */

/* 532 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 534 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 536 */	NdrFcShort( 0x4ce ),	/* Type Offset=1230 */

	/* Return value */

/* 538 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 540 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Object */

/* 544 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 546 */	NdrFcLong( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0xd ),	/* 13 */
/* 552 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 556 */	NdrFcShort( 0x8 ),	/* 8 */
/* 558 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 560 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 568 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */

/* 570 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 572 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 574 */	NdrFcShort( 0x4d8 ),	/* Type Offset=1240 */

	/* Return value */

/* 576 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 578 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 580 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Object */

/* 582 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 584 */	NdrFcLong( 0x0 ),	/* 0 */
/* 588 */	NdrFcShort( 0xe ),	/* 14 */
/* 590 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 592 */	NdrFcShort( 0x0 ),	/* 0 */
/* 594 */	NdrFcShort( 0x8 ),	/* 8 */
/* 596 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 598 */	0xa,		/* 10 */
			0x41,		/* Ext Flags:  new corr desc, has range on conformance */
/* 600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 606 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cnt */

/* 608 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 610 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 612 */	NdrFcShort( 0x1b0 ),	/* Type Offset=432 */

	/* Return value */

/* 614 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 616 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 618 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const test_MIDL_TYPE_FORMAT_STRING test__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 12 */	NdrFcLong( 0x20404 ),	/* 132100 */
/* 16 */	NdrFcShort( 0x0 ),	/* 0 */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 22 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 24 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 26 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 28 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 30 */	NdrFcShort( 0x26 ),	/* Offset= 38 (68) */
/* 32 */	
			0x13, 0x0,	/* FC_OP */
/* 34 */	NdrFcShort( 0x18 ),	/* Offset= 24 (58) */
/* 36 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 38 */	NdrFcShort( 0x2 ),	/* 2 */
/* 40 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 42 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 44 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 46 */	0x0 , 
			0x0,		/* 0 */
/* 48 */	NdrFcLong( 0x0 ),	/* 0 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 58 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 60 */	NdrFcShort( 0x8 ),	/* 8 */
/* 62 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (36) */
/* 64 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 66 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 68 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */
/* 72 */	NdrFcShort( 0x8 ),	/* 8 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */
/* 76 */	NdrFcShort( 0xffd4 ),	/* Offset= -44 (32) */
/* 78 */	
			0x12, 0x0,	/* FC_UP */
/* 80 */	NdrFcShort( 0xffea ),	/* Offset= -22 (58) */
/* 82 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 84 */	NdrFcShort( 0x0 ),	/* 0 */
/* 86 */	NdrFcShort( 0x8 ),	/* 8 */
/* 88 */	NdrFcShort( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (78) */
/* 92 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 94 */	NdrFcShort( 0x45e ),	/* Offset= 1118 (1212) */
/* 96 */	
			0x13, 0x0,	/* FC_OP */
/* 98 */	NdrFcShort( 0x446 ),	/* Offset= 1094 (1192) */
/* 100 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 102 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 104 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 106 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 108 */	0x0 , 
			0x0,		/* 0 */
/* 110 */	NdrFcLong( 0x0 ),	/* 0 */
/* 114 */	NdrFcLong( 0x0 ),	/* 0 */
/* 118 */	NdrFcShort( 0x2 ),	/* Offset= 2 (120) */
/* 120 */	NdrFcShort( 0x10 ),	/* 16 */
/* 122 */	NdrFcShort( 0x2f ),	/* 47 */
/* 124 */	NdrFcLong( 0x14 ),	/* 20 */
/* 128 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 130 */	NdrFcLong( 0x3 ),	/* 3 */
/* 134 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 136 */	NdrFcLong( 0x11 ),	/* 17 */
/* 140 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 142 */	NdrFcLong( 0x2 ),	/* 2 */
/* 146 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 148 */	NdrFcLong( 0x4 ),	/* 4 */
/* 152 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 154 */	NdrFcLong( 0x5 ),	/* 5 */
/* 158 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 160 */	NdrFcLong( 0xb ),	/* 11 */
/* 164 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 166 */	NdrFcLong( 0xa ),	/* 10 */
/* 170 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 172 */	NdrFcLong( 0x6 ),	/* 6 */
/* 176 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (408) */
/* 178 */	NdrFcLong( 0x7 ),	/* 7 */
/* 182 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 184 */	NdrFcLong( 0x8 ),	/* 8 */
/* 188 */	NdrFcShort( 0xff64 ),	/* Offset= -156 (32) */
/* 190 */	NdrFcLong( 0xd ),	/* 13 */
/* 194 */	NdrFcShort( 0xdc ),	/* Offset= 220 (414) */
/* 196 */	NdrFcLong( 0x9 ),	/* 9 */
/* 200 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (432) */
/* 202 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 206 */	NdrFcShort( 0xf4 ),	/* Offset= 244 (450) */
/* 208 */	NdrFcLong( 0x24 ),	/* 36 */
/* 212 */	NdrFcShort( 0x38a ),	/* Offset= 906 (1118) */
/* 214 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 218 */	NdrFcShort( 0x384 ),	/* Offset= 900 (1118) */
/* 220 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 224 */	NdrFcShort( 0x382 ),	/* Offset= 898 (1122) */
/* 226 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 230 */	NdrFcShort( 0x380 ),	/* Offset= 896 (1126) */
/* 232 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 236 */	NdrFcShort( 0x37e ),	/* Offset= 894 (1130) */
/* 238 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 242 */	NdrFcShort( 0x37c ),	/* Offset= 892 (1134) */
/* 244 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 248 */	NdrFcShort( 0x37a ),	/* Offset= 890 (1138) */
/* 250 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 254 */	NdrFcShort( 0x378 ),	/* Offset= 888 (1142) */
/* 256 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 260 */	NdrFcShort( 0x362 ),	/* Offset= 866 (1126) */
/* 262 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 266 */	NdrFcShort( 0x360 ),	/* Offset= 864 (1130) */
/* 268 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 272 */	NdrFcShort( 0x36a ),	/* Offset= 874 (1146) */
/* 274 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 278 */	NdrFcShort( 0x360 ),	/* Offset= 864 (1142) */
/* 280 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 284 */	NdrFcShort( 0x362 ),	/* Offset= 866 (1150) */
/* 286 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 290 */	NdrFcShort( 0x360 ),	/* Offset= 864 (1154) */
/* 292 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 296 */	NdrFcShort( 0x35e ),	/* Offset= 862 (1158) */
/* 298 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 302 */	NdrFcShort( 0x35c ),	/* Offset= 860 (1162) */
/* 304 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 308 */	NdrFcShort( 0x35a ),	/* Offset= 858 (1166) */
/* 310 */	NdrFcLong( 0x10 ),	/* 16 */
/* 314 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 316 */	NdrFcLong( 0x12 ),	/* 18 */
/* 320 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 322 */	NdrFcLong( 0x13 ),	/* 19 */
/* 326 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 328 */	NdrFcLong( 0x15 ),	/* 21 */
/* 332 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 334 */	NdrFcLong( 0x16 ),	/* 22 */
/* 338 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 340 */	NdrFcLong( 0x17 ),	/* 23 */
/* 344 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 346 */	NdrFcLong( 0xe ),	/* 14 */
/* 350 */	NdrFcShort( 0x338 ),	/* Offset= 824 (1174) */
/* 352 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 356 */	NdrFcShort( 0x33c ),	/* Offset= 828 (1184) */
/* 358 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 362 */	NdrFcShort( 0x33a ),	/* Offset= 826 (1188) */
/* 364 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 368 */	NdrFcShort( 0x2f6 ),	/* Offset= 758 (1126) */
/* 370 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 374 */	NdrFcShort( 0x2f4 ),	/* Offset= 756 (1130) */
/* 376 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 380 */	NdrFcShort( 0x2f2 ),	/* Offset= 754 (1134) */
/* 382 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 386 */	NdrFcShort( 0x2e8 ),	/* Offset= 744 (1130) */
/* 388 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 392 */	NdrFcShort( 0x2e2 ),	/* Offset= 738 (1130) */
/* 394 */	NdrFcLong( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x0 ),	/* Offset= 0 (398) */
/* 400 */	NdrFcLong( 0x1 ),	/* 1 */
/* 404 */	NdrFcShort( 0x0 ),	/* Offset= 0 (404) */
/* 406 */	NdrFcShort( 0xffff ),	/* Offset= -1 (405) */
/* 408 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 410 */	NdrFcShort( 0x8 ),	/* 8 */
/* 412 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 414 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 416 */	NdrFcLong( 0x0 ),	/* 0 */
/* 420 */	NdrFcShort( 0x0 ),	/* 0 */
/* 422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 424 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 426 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 428 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 430 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 432 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 434 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 438 */	NdrFcShort( 0x0 ),	/* 0 */
/* 440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 442 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 444 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 446 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 448 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 450 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 452 */	NdrFcShort( 0x2 ),	/* Offset= 2 (454) */
/* 454 */	
			0x13, 0x0,	/* FC_OP */
/* 456 */	NdrFcShort( 0x284 ),	/* Offset= 644 (1100) */
/* 458 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x89,		/* 137 */
/* 460 */	NdrFcShort( 0x20 ),	/* 32 */
/* 462 */	NdrFcShort( 0xa ),	/* 10 */
/* 464 */	NdrFcLong( 0x8 ),	/* 8 */
/* 468 */	NdrFcShort( 0x64 ),	/* Offset= 100 (568) */
/* 470 */	NdrFcLong( 0xd ),	/* 13 */
/* 474 */	NdrFcShort( 0x98 ),	/* Offset= 152 (626) */
/* 476 */	NdrFcLong( 0x9 ),	/* 9 */
/* 480 */	NdrFcShort( 0xcc ),	/* Offset= 204 (684) */
/* 482 */	NdrFcLong( 0xc ),	/* 12 */
/* 486 */	NdrFcShort( 0x100 ),	/* Offset= 256 (742) */
/* 488 */	NdrFcLong( 0x24 ),	/* 36 */
/* 492 */	NdrFcShort( 0x170 ),	/* Offset= 368 (860) */
/* 494 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 498 */	NdrFcShort( 0x18c ),	/* Offset= 396 (894) */
/* 500 */	NdrFcLong( 0x10 ),	/* 16 */
/* 504 */	NdrFcShort( 0x1b0 ),	/* Offset= 432 (936) */
/* 506 */	NdrFcLong( 0x2 ),	/* 2 */
/* 510 */	NdrFcShort( 0x1d0 ),	/* Offset= 464 (974) */
/* 512 */	NdrFcLong( 0x3 ),	/* 3 */
/* 516 */	NdrFcShort( 0x1f0 ),	/* Offset= 496 (1012) */
/* 518 */	NdrFcLong( 0x14 ),	/* 20 */
/* 522 */	NdrFcShort( 0x210 ),	/* Offset= 528 (1050) */
/* 524 */	NdrFcShort( 0xffff ),	/* Offset= -1 (523) */
/* 526 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 528 */	NdrFcShort( 0x0 ),	/* 0 */
/* 530 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 532 */	NdrFcShort( 0x0 ),	/* 0 */
/* 534 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 536 */	0x0 , 
			0x0,		/* 0 */
/* 538 */	NdrFcLong( 0x0 ),	/* 0 */
/* 542 */	NdrFcLong( 0x0 ),	/* 0 */
/* 546 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 550 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 552 */	0x0 , 
			0x0,		/* 0 */
/* 554 */	NdrFcLong( 0x0 ),	/* 0 */
/* 558 */	NdrFcLong( 0x0 ),	/* 0 */
/* 562 */	
			0x13, 0x0,	/* FC_OP */
/* 564 */	NdrFcShort( 0xfe06 ),	/* Offset= -506 (58) */
/* 566 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 568 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 570 */	NdrFcShort( 0x10 ),	/* 16 */
/* 572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 574 */	NdrFcShort( 0x6 ),	/* Offset= 6 (580) */
/* 576 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 578 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 580 */	
			0x11, 0x0,	/* FC_RP */
/* 582 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (526) */
/* 584 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 586 */	NdrFcShort( 0x0 ),	/* 0 */
/* 588 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 592 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 594 */	0x0 , 
			0x0,		/* 0 */
/* 596 */	NdrFcLong( 0x0 ),	/* 0 */
/* 600 */	NdrFcLong( 0x0 ),	/* 0 */
/* 604 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 608 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 610 */	0x0 , 
			0x0,		/* 0 */
/* 612 */	NdrFcLong( 0x0 ),	/* 0 */
/* 616 */	NdrFcLong( 0x0 ),	/* 0 */
/* 620 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 622 */	NdrFcShort( 0xff30 ),	/* Offset= -208 (414) */
/* 624 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 626 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 628 */	NdrFcShort( 0x10 ),	/* 16 */
/* 630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 632 */	NdrFcShort( 0x6 ),	/* Offset= 6 (638) */
/* 634 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 636 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 638 */	
			0x11, 0x0,	/* FC_RP */
/* 640 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (584) */
/* 642 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 646 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 650 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 652 */	0x0 , 
			0x0,		/* 0 */
/* 654 */	NdrFcLong( 0x0 ),	/* 0 */
/* 658 */	NdrFcLong( 0x0 ),	/* 0 */
/* 662 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 666 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 668 */	0x0 , 
			0x0,		/* 0 */
/* 670 */	NdrFcLong( 0x0 ),	/* 0 */
/* 674 */	NdrFcLong( 0x0 ),	/* 0 */
/* 678 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 680 */	NdrFcShort( 0xff08 ),	/* Offset= -248 (432) */
/* 682 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 684 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 686 */	NdrFcShort( 0x10 ),	/* 16 */
/* 688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 690 */	NdrFcShort( 0x6 ),	/* Offset= 6 (696) */
/* 692 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 694 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 696 */	
			0x11, 0x0,	/* FC_RP */
/* 698 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (642) */
/* 700 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 702 */	NdrFcShort( 0x0 ),	/* 0 */
/* 704 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 706 */	NdrFcShort( 0x0 ),	/* 0 */
/* 708 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 710 */	0x0 , 
			0x0,		/* 0 */
/* 712 */	NdrFcLong( 0x0 ),	/* 0 */
/* 716 */	NdrFcLong( 0x0 ),	/* 0 */
/* 720 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 724 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 726 */	0x0 , 
			0x0,		/* 0 */
/* 728 */	NdrFcLong( 0x0 ),	/* 0 */
/* 732 */	NdrFcLong( 0x0 ),	/* 0 */
/* 736 */	
			0x13, 0x0,	/* FC_OP */
/* 738 */	NdrFcShort( 0x1c6 ),	/* Offset= 454 (1192) */
/* 740 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 742 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 744 */	NdrFcShort( 0x10 ),	/* 16 */
/* 746 */	NdrFcShort( 0x0 ),	/* 0 */
/* 748 */	NdrFcShort( 0x6 ),	/* Offset= 6 (754) */
/* 750 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 752 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 754 */	
			0x11, 0x0,	/* FC_RP */
/* 756 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (700) */
/* 758 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 760 */	NdrFcLong( 0x2f ),	/* 47 */
/* 764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 768 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 770 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 772 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 774 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 776 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 778 */	NdrFcShort( 0x1 ),	/* 1 */
/* 780 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 782 */	NdrFcShort( 0x4 ),	/* 4 */
/* 784 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 786 */	0x0 , 
			0x0,		/* 0 */
/* 788 */	NdrFcLong( 0x0 ),	/* 0 */
/* 792 */	NdrFcLong( 0x0 ),	/* 0 */
/* 796 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 798 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 800 */	NdrFcShort( 0x18 ),	/* 24 */
/* 802 */	NdrFcShort( 0x0 ),	/* 0 */
/* 804 */	NdrFcShort( 0xa ),	/* Offset= 10 (814) */
/* 806 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 808 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 810 */	NdrFcShort( 0xffcc ),	/* Offset= -52 (758) */
/* 812 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 814 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 816 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (776) */
/* 818 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 820 */	NdrFcShort( 0x0 ),	/* 0 */
/* 822 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 826 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 828 */	0x0 , 
			0x0,		/* 0 */
/* 830 */	NdrFcLong( 0x0 ),	/* 0 */
/* 834 */	NdrFcLong( 0x0 ),	/* 0 */
/* 838 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 842 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 844 */	0x0 , 
			0x0,		/* 0 */
/* 846 */	NdrFcLong( 0x0 ),	/* 0 */
/* 850 */	NdrFcLong( 0x0 ),	/* 0 */
/* 854 */	
			0x13, 0x0,	/* FC_OP */
/* 856 */	NdrFcShort( 0xffc6 ),	/* Offset= -58 (798) */
/* 858 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 860 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 862 */	NdrFcShort( 0x10 ),	/* 16 */
/* 864 */	NdrFcShort( 0x0 ),	/* 0 */
/* 866 */	NdrFcShort( 0x6 ),	/* Offset= 6 (872) */
/* 868 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 870 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 872 */	
			0x11, 0x0,	/* FC_RP */
/* 874 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (818) */
/* 876 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 878 */	NdrFcShort( 0x8 ),	/* 8 */
/* 880 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 882 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 884 */	NdrFcShort( 0x10 ),	/* 16 */
/* 886 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 888 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 890 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (876) */
			0x5b,		/* FC_END */
/* 894 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 896 */	NdrFcShort( 0x20 ),	/* 32 */
/* 898 */	NdrFcShort( 0x0 ),	/* 0 */
/* 900 */	NdrFcShort( 0xa ),	/* Offset= 10 (910) */
/* 902 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 904 */	0x36,		/* FC_POINTER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 906 */	0x0,		/* 0 */
			NdrFcShort( 0xffe7 ),	/* Offset= -25 (882) */
			0x5b,		/* FC_END */
/* 910 */	
			0x11, 0x0,	/* FC_RP */
/* 912 */	NdrFcShort( 0xfeb8 ),	/* Offset= -328 (584) */
/* 914 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 916 */	NdrFcShort( 0x1 ),	/* 1 */
/* 918 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 920 */	NdrFcShort( 0x0 ),	/* 0 */
/* 922 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 924 */	0x0 , 
			0x0,		/* 0 */
/* 926 */	NdrFcLong( 0x0 ),	/* 0 */
/* 930 */	NdrFcLong( 0x0 ),	/* 0 */
/* 934 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 936 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 938 */	NdrFcShort( 0x10 ),	/* 16 */
/* 940 */	NdrFcShort( 0x0 ),	/* 0 */
/* 942 */	NdrFcShort( 0x6 ),	/* Offset= 6 (948) */
/* 944 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 946 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 948 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 950 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (914) */
/* 952 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 954 */	NdrFcShort( 0x2 ),	/* 2 */
/* 956 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 960 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 962 */	0x0 , 
			0x0,		/* 0 */
/* 964 */	NdrFcLong( 0x0 ),	/* 0 */
/* 968 */	NdrFcLong( 0x0 ),	/* 0 */
/* 972 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 974 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 976 */	NdrFcShort( 0x10 ),	/* 16 */
/* 978 */	NdrFcShort( 0x0 ),	/* 0 */
/* 980 */	NdrFcShort( 0x6 ),	/* Offset= 6 (986) */
/* 982 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 984 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 986 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 988 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (952) */
/* 990 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 992 */	NdrFcShort( 0x4 ),	/* 4 */
/* 994 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 998 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1000 */	0x0 , 
			0x0,		/* 0 */
/* 1002 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1006 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1010 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1012 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1014 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1018 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1024) */
/* 1020 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1022 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1024 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 1026 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (990) */
/* 1028 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1030 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1032 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1036 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1038 */	0x0 , 
			0x0,		/* 0 */
/* 1040 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1044 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1048 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1050 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1052 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1054 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1056 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1062) */
/* 1058 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 1060 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1062 */	
			0x13, 0x20,	/* FC_OP [maybenull_sizeis] */
/* 1064 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1028) */
/* 1066 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1068 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1070 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1072 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1074 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1076 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1078 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1080 */	NdrFcShort( 0xffc8 ),	/* -56 */
/* 1082 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1084 */	0x0 , 
			0x0,		/* 0 */
/* 1086 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1090 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1094 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1096 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (1066) */
/* 1098 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1100 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1102 */	NdrFcShort( 0x38 ),	/* 56 */
/* 1104 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (1074) */
/* 1106 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1106) */
/* 1108 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1110 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1112 */	0x40,		/* FC_STRUCTPAD4 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 1114 */	0x0,		/* 0 */
			NdrFcShort( 0xfd6f ),	/* Offset= -657 (458) */
			0x5b,		/* FC_END */
/* 1118 */	
			0x13, 0x0,	/* FC_OP */
/* 1120 */	NdrFcShort( 0xfebe ),	/* Offset= -322 (798) */
/* 1122 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1124 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1126 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1128 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1130 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1132 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1134 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1136 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 1138 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1140 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1142 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1144 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1146 */	
			0x13, 0x0,	/* FC_OP */
/* 1148 */	NdrFcShort( 0xfd1c ),	/* Offset= -740 (408) */
/* 1150 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1152 */	NdrFcShort( 0xfba0 ),	/* Offset= -1120 (32) */
/* 1154 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1156 */	NdrFcShort( 0xfd1a ),	/* Offset= -742 (414) */
/* 1158 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1160 */	NdrFcShort( 0xfd28 ),	/* Offset= -728 (432) */
/* 1162 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1164 */	NdrFcShort( 0xfd36 ),	/* Offset= -714 (450) */
/* 1166 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 1168 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1170) */
/* 1170 */	
			0x13, 0x0,	/* FC_OP */
/* 1172 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1192) */
/* 1174 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1176 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1178 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1180 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1182 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1184 */	
			0x13, 0x0,	/* FC_OP */
/* 1186 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1174) */
/* 1188 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 1190 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1192 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1194 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1198 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1198) */
/* 1200 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1202 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1204 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1206 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1208 */	NdrFcShort( 0xfbac ),	/* Offset= -1108 (100) */
/* 1210 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1212 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1214 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1216 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1220 */	NdrFcShort( 0xfb9c ),	/* Offset= -1124 (96) */
/* 1222 */	
			0x11, 0x0,	/* FC_RP */
/* 1224 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1230) */
/* 1226 */	
			0x12, 0x0,	/* FC_UP */
/* 1228 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (1192) */
/* 1230 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1232 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1234 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1238 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1226) */
/* 1240 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1242 */	NdrFcShort( 0xfcd6 ),	/* Offset= -810 (432) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            },
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ITestLongCollection, ver. 0.0,
   GUID={0xa03d1420,0xb1ec,0x11d0,{0x8c,0x3a,0x00,0xc0,0x4f,0xc3,0x1d,0x2f}} */

#pragma code_seg(".orpc")
static const unsigned short ITestLongCollection_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    38,
    82
    };



/* Object interface: ITestSimpleUnknown, ver. 0.0,
   GUID={0x6FB050D8,0x438A,0x41B8,{0xA5,0x6E,0x26,0xD6,0xF3,0x44,0x4C,0xE6}} */

#pragma code_seg(".orpc")
static const unsigned short ITestSimpleUnknown_FormatStringOffsetTable[] =
    {
    120,
    158
    };



/* Object interface: ISubObject, ver. 0.0,
   GUID={0xB555296F,0xD09B,0x4A4E,{0xA1,0x48,0xE0,0x71,0x53,0x94,0xA5,0x25}} */

#pragma code_seg(".orpc")
static const unsigned short ISubObject_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    202,
    240,
    278,
    316
    };



/* Object interface: ITestObject, ver. 0.0,
   GUID={0xFE795DAE,0x119A,0x4141,{0x96,0xD3,0xF1,0xDE,0xDB,0x5B,0x22,0x59}} */

#pragma code_seg(".orpc")
static const unsigned short ITestObject_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    354,
    392,
    430,
    468,
    506,
    544,
    582
    };



#endif /* defined(_M_AMD64)*/



/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if defined(_M_AMD64)



extern const USER_MARSHAL_ROUTINE_QUADRUPLE NDR64_UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif


#include "ndr64types.h"
#include "pshpack8.h"


typedef 
NDR64_FORMAT_CHAR
__midl_frag222_t;
extern const __midl_frag222_t __midl_frag222;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag221_t;
extern const __midl_frag221_t __midl_frag221;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag220_t;
extern const __midl_frag220_t __midl_frag220;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag219_t;
extern const __midl_frag219_t __midl_frag219;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag215_t;
extern const __midl_frag215_t __midl_frag215;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag214_t;
extern const __midl_frag214_t __midl_frag214;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag212_t;
extern const __midl_frag212_t __midl_frag212;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag211_t;
extern const __midl_frag211_t __midl_frag211;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag210_t;
extern const __midl_frag210_t __midl_frag210;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag209_t;
extern const __midl_frag209_t __midl_frag209;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag207_t;
extern const __midl_frag207_t __midl_frag207;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag206_t;
extern const __midl_frag206_t __midl_frag206;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag205_t;
extern const __midl_frag205_t __midl_frag205;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag204_t;
extern const __midl_frag204_t __midl_frag204;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag202_t;
extern const __midl_frag202_t __midl_frag202;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag201_t;
extern const __midl_frag201_t __midl_frag201;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag200_t;
extern const __midl_frag200_t __midl_frag200;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag198_t;
extern const __midl_frag198_t __midl_frag198;

typedef 
struct _NDR64_USER_MARSHAL_FORMAT
__midl_frag197_t;
extern const __midl_frag197_t __midl_frag197;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag196_t;
extern const __midl_frag196_t __midl_frag196;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag195_t;
extern const __midl_frag195_t __midl_frag195;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag192_t;
extern const __midl_frag192_t __midl_frag192;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag189_t;
extern const __midl_frag189_t __midl_frag189;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag188_t;
extern const __midl_frag188_t __midl_frag188;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag180_t;
extern const __midl_frag180_t __midl_frag180;

typedef 
NDR64_FORMAT_CHAR
__midl_frag177_t;
extern const __midl_frag177_t __midl_frag177;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag176_t;
extern const __midl_frag176_t __midl_frag176;

typedef 
NDR64_FORMAT_CHAR
__midl_frag173_t;
extern const __midl_frag173_t __midl_frag173;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag172_t;
extern const __midl_frag172_t __midl_frag172;

typedef 
NDR64_FORMAT_CHAR
__midl_frag171_t;
extern const __midl_frag171_t __midl_frag171;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag170_t;
extern const __midl_frag170_t __midl_frag170;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag169_t;
extern const __midl_frag169_t __midl_frag169;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag168_t;
extern const __midl_frag168_t __midl_frag168;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag160_t;
extern const __midl_frag160_t __midl_frag160;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag159_t;
extern const __midl_frag159_t __midl_frag159;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag158_t;
extern const __midl_frag158_t __midl_frag158;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag157_t;
extern const __midl_frag157_t __midl_frag157;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag154_t;
extern const __midl_frag154_t __midl_frag154;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag153_t;
extern const __midl_frag153_t __midl_frag153;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag152_t;
extern const __midl_frag152_t __midl_frag152;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag151_t;
extern const __midl_frag151_t __midl_frag151;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag149_t;
extern const __midl_frag149_t __midl_frag149;

typedef 
NDR64_FORMAT_CHAR
__midl_frag148_t;
extern const __midl_frag148_t __midl_frag148;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag147_t;
extern const __midl_frag147_t __midl_frag147;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag146_t;
extern const __midl_frag146_t __midl_frag146;

typedef 
NDR64_FORMAT_CHAR
__midl_frag139_t;
extern const __midl_frag139_t __midl_frag139;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag138_t;
extern const __midl_frag138_t __midl_frag138;

typedef 
NDR64_FORMAT_CHAR
__midl_frag131_t;
extern const __midl_frag131_t __midl_frag131;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag130_t;
extern const __midl_frag130_t __midl_frag130;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag129_t;
extern const __midl_frag129_t __midl_frag129;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag128_t;
extern const __midl_frag128_t __midl_frag128;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag126_t;
extern const __midl_frag126_t __midl_frag126;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag125_t;
extern const __midl_frag125_t __midl_frag125;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag124_t;
extern const __midl_frag124_t __midl_frag124;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag123_t;
extern const __midl_frag123_t __midl_frag123;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag120_t;
extern const __midl_frag120_t __midl_frag120;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag119_t;
extern const __midl_frag119_t __midl_frag119;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag118_t;
extern const __midl_frag118_t __midl_frag118;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag115_t;
extern const __midl_frag115_t __midl_frag115;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag114_t;
extern const __midl_frag114_t __midl_frag114;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag113_t;
extern const __midl_frag113_t __midl_frag113;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag110_t;
extern const __midl_frag110_t __midl_frag110;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag109_t;
extern const __midl_frag109_t __midl_frag109;

typedef 
struct 
{
    struct _NDR64_FIX_ARRAY_HEADER_FORMAT frag1;
}
__midl_frag108_t;
extern const __midl_frag108_t __midl_frag108;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag107_t;
extern const __midl_frag107_t __midl_frag107;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag102_t;
extern const __midl_frag102_t __midl_frag102;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag5;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag6;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag7;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag8;
    } frag2;
}
__midl_frag101_t;
extern const __midl_frag101_t __midl_frag101;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag100_t;
extern const __midl_frag100_t __midl_frag100;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag97_t;
extern const __midl_frag97_t __midl_frag97;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag96_t;
extern const __midl_frag96_t __midl_frag96;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag95_t;
extern const __midl_frag95_t __midl_frag95;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_NO_REPEAT_FORMAT frag1;
        struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag2;
        struct _NDR64_POINTER_FORMAT frag3;
        struct _NDR64_NO_REPEAT_FORMAT frag4;
        struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag5;
        struct _NDR64_POINTER_FORMAT frag6;
        NDR64_FORMAT_CHAR frag7;
    } frag2;
}
__midl_frag94_t;
extern const __midl_frag94_t __midl_frag94;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag92_t;
extern const __midl_frag92_t __midl_frag92;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag91_t;
extern const __midl_frag91_t __midl_frag91;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag90_t;
extern const __midl_frag90_t __midl_frag90;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag87_t;
extern const __midl_frag87_t __midl_frag87;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag86_t;
extern const __midl_frag86_t __midl_frag86;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag85_t;
extern const __midl_frag85_t __midl_frag85;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag80_t;
extern const __midl_frag80_t __midl_frag80;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag79_t;
extern const __midl_frag79_t __midl_frag79;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag72_t;
extern const __midl_frag72_t __midl_frag72;

typedef 
struct 
{
    struct _NDR64_POINTER_FORMAT frag1;
}
__midl_frag71_t;
extern const __midl_frag71_t __midl_frag71;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_REPEAT_FORMAT frag1;
        struct 
        {
            struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT frag1;
            struct _NDR64_POINTER_FORMAT frag2;
        } frag2;
        NDR64_FORMAT_CHAR frag3;
    } frag2;
    struct _NDR64_ARRAY_ELEMENT_INFO frag3;
}
__midl_frag68_t;
extern const __midl_frag68_t __midl_frag68;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag67_t;
extern const __midl_frag67_t __midl_frag67;

typedef 
struct 
{
    struct _NDR64_ENCAPSULATED_UNION frag1;
    struct _NDR64_UNION_ARM_SELECTOR frag2;
    struct _NDR64_UNION_ARM frag3;
    struct _NDR64_UNION_ARM frag4;
    struct _NDR64_UNION_ARM frag5;
    struct _NDR64_UNION_ARM frag6;
    struct _NDR64_UNION_ARM frag7;
    struct _NDR64_UNION_ARM frag8;
    struct _NDR64_UNION_ARM frag9;
    struct _NDR64_UNION_ARM frag10;
    struct _NDR64_UNION_ARM frag11;
    struct _NDR64_UNION_ARM frag12;
    NDR64_UINT32 frag13;
}
__midl_frag66_t;
extern const __midl_frag66_t __midl_frag66;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag65_t;
extern const __midl_frag65_t __midl_frag65;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag64_t;
extern const __midl_frag64_t __midl_frag64;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag63_t;
extern const __midl_frag63_t __midl_frag63;

typedef 
struct 
{
    struct _NDR64_CONF_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_REGION_FORMAT frag1;
        struct _NDR64_MEMPAD_FORMAT frag2;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag3;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag4;
    } frag2;
}
__midl_frag62_t;
extern const __midl_frag62_t __midl_frag62;

typedef 
struct 
{
    struct _NDR64_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag53_t;
extern const __midl_frag53_t __midl_frag53;

typedef 
struct 
{
    NDR64_FORMAT_UINT32 frag1;
    struct _NDR64_EXPR_VAR frag2;
}
__midl_frag44_t;
extern const __midl_frag44_t __midl_frag44;

typedef 
struct 
{
    struct _NDR64_NON_ENCAPSULATED_UNION frag1;
    struct _NDR64_UNION_ARM_SELECTOR frag2;
    struct _NDR64_UNION_ARM frag3;
    struct _NDR64_UNION_ARM frag4;
    struct _NDR64_UNION_ARM frag5;
    struct _NDR64_UNION_ARM frag6;
    struct _NDR64_UNION_ARM frag7;
    struct _NDR64_UNION_ARM frag8;
    struct _NDR64_UNION_ARM frag9;
    struct _NDR64_UNION_ARM frag10;
    struct _NDR64_UNION_ARM frag11;
    struct _NDR64_UNION_ARM frag12;
    struct _NDR64_UNION_ARM frag13;
    struct _NDR64_UNION_ARM frag14;
    struct _NDR64_UNION_ARM frag15;
    struct _NDR64_UNION_ARM frag16;
    struct _NDR64_UNION_ARM frag17;
    struct _NDR64_UNION_ARM frag18;
    struct _NDR64_UNION_ARM frag19;
    struct _NDR64_UNION_ARM frag20;
    struct _NDR64_UNION_ARM frag21;
    struct _NDR64_UNION_ARM frag22;
    struct _NDR64_UNION_ARM frag23;
    struct _NDR64_UNION_ARM frag24;
    struct _NDR64_UNION_ARM frag25;
    struct _NDR64_UNION_ARM frag26;
    struct _NDR64_UNION_ARM frag27;
    struct _NDR64_UNION_ARM frag28;
    struct _NDR64_UNION_ARM frag29;
    struct _NDR64_UNION_ARM frag30;
    struct _NDR64_UNION_ARM frag31;
    struct _NDR64_UNION_ARM frag32;
    struct _NDR64_UNION_ARM frag33;
    struct _NDR64_UNION_ARM frag34;
    struct _NDR64_UNION_ARM frag35;
    struct _NDR64_UNION_ARM frag36;
    struct _NDR64_UNION_ARM frag37;
    struct _NDR64_UNION_ARM frag38;
    struct _NDR64_UNION_ARM frag39;
    struct _NDR64_UNION_ARM frag40;
    struct _NDR64_UNION_ARM frag41;
    struct _NDR64_UNION_ARM frag42;
    struct _NDR64_UNION_ARM frag43;
    struct _NDR64_UNION_ARM frag44;
    struct _NDR64_UNION_ARM frag45;
    struct _NDR64_UNION_ARM frag46;
    struct _NDR64_UNION_ARM frag47;
    struct _NDR64_UNION_ARM frag48;
    struct _NDR64_UNION_ARM frag49;
    NDR64_UINT32 frag50;
}
__midl_frag43_t;
extern const __midl_frag43_t __midl_frag43;

typedef 
struct 
{
    struct _NDR64_BOGUS_STRUCTURE_HEADER_FORMAT frag1;
    struct 
    {
        struct _NDR64_SIMPLE_REGION_FORMAT frag1;
        struct _NDR64_EMBEDDED_COMPLEX_FORMAT frag2;
        struct _NDR64_SIMPLE_MEMBER_FORMAT frag3;
    } frag2;
}
__midl_frag42_t;
extern const __midl_frag42_t __midl_frag42;

typedef 
struct 
{
    struct _NDR64_CONF_ARRAY_HEADER_FORMAT frag1;
    struct _NDR64_ARRAY_ELEMENT_INFO frag2;
}
__midl_frag30_t;
extern const __midl_frag30_t __midl_frag30;

typedef 
struct 
{
    struct _NDR64_CONF_STRUCTURE_HEADER_FORMAT frag1;
}
__midl_frag29_t;
extern const __midl_frag29_t __midl_frag29;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
    struct _NDR64_PARAM_FORMAT frag4;
}
__midl_frag20_t;
extern const __midl_frag20_t __midl_frag20;

typedef 
struct _NDR64_CONSTANT_IID_FORMAT
__midl_frag14_t;
extern const __midl_frag14_t __midl_frag14;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag13_t;
extern const __midl_frag13_t __midl_frag13;

typedef 
struct _NDR64_POINTER_FORMAT
__midl_frag12_t;
extern const __midl_frag12_t __midl_frag12;

typedef 
struct 
{
    struct _NDR64_PROC_FORMAT frag1;
    struct _NDR64_PARAM_FORMAT frag2;
    struct _NDR64_PARAM_FORMAT frag3;
}
__midl_frag11_t;
extern const __midl_frag11_t __midl_frag11;

typedef 
NDR64_FORMAT_UINT32
__midl_frag1_t;
extern const __midl_frag1_t __midl_frag1;

static const __midl_frag222_t __midl_frag222 =
0x5    /* FC64_INT32 */;

static const __midl_frag221_t __midl_frag221 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x00020400,
        0x0000,
        0x0000,
        {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
    }
};

static const __midl_frag220_t __midl_frag220 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag221
};

static const __midl_frag219_t __midl_frag219 =
{ 
/* put_Object */
    { 
    /* put_Object */      /* procedure put_Object */
        (NDR64_UINT32) 786755 /* 0xc0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* cnt */      /* parameter cnt */
        &__midl_frag220,
        { 
        /* cnt */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag222,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag215_t __midl_frag215 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag220
};

static const __midl_frag214_t __midl_frag214 =
{ 
/* get_Object */
    { 
    /* get_Object */      /* procedure get_Object */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* cnt */      /* parameter cnt */
        &__midl_frag215,
        { 
        /* cnt */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag222,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag212_t __midl_frag212 =
{ 
/* *_wireVARIANT */
    0x21,    /* FC64_UP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag42
};

static const __midl_frag211_t __midl_frag211 =
{ 
/* wireVARIANT */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 1 /* 0x1 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 24 /* 0x18 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag212
};

static const __midl_frag210_t __midl_frag210 =
{ 
/* *wireVARIANT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag211
};

static const __midl_frag209_t __midl_frag209 =
{ 
/* put_Type */
    { 
    /* put_Type */      /* procedure put_Type */
        (NDR64_UINT32) 36438339 /* 0x22c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation, actual guaranteed */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* cnt */      /* parameter cnt */
        &__midl_frag211,
        { 
        /* cnt */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], SimpleRef */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag222,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag207_t __midl_frag207 =
{ 
/* *_wireVARIANT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag42
};

static const __midl_frag206_t __midl_frag206 =
{ 
/* wireVARIANT */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 1 /* 0x1 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 24 /* 0x18 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag207
};

static const __midl_frag205_t __midl_frag205 =
{ 
/* *wireVARIANT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 4 /* 0x4 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag206
};

static const __midl_frag204_t __midl_frag204 =
{ 
/* get_Type */
    { 
    /* get_Type */      /* procedure get_Type */
        (NDR64_UINT32) 4849987 /* 0x4a0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn, ClientCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* cnt */      /* parameter cnt */
        &__midl_frag206,
        { 
        /* cnt */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustSize, MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag222,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag202_t __midl_frag202 =
{ 
/* *FLAGGED_WORD_BLOB */
    0x21,    /* FC64_UP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag29
};

static const __midl_frag201_t __midl_frag201 =
{ 
/* wireBSTR */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 8 /* 0x8 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag202
};

static const __midl_frag200_t __midl_frag200 =
{ 
/* put_Desc */
    { 
    /* put_Desc */      /* procedure put_Desc */
        (NDR64_UINT32) 2883907 /* 0x2c0143 */,    /* auto handle */ /* IsIntrepreted, [object], ClientMustSize, HasReturn, ServerCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* cnt */      /* parameter cnt */
        &__midl_frag201,
        { 
        /* cnt */
            1,
            1,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [in], ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag222,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag198_t __midl_frag198 =
{ 
/* *FLAGGED_WORD_BLOB */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag29
};

static const __midl_frag197_t __midl_frag197 =
{ 
/* wireBSTR */
    0xa2,    /* FC64_USER_MARSHAL */
    (NDR64_UINT8) 128 /* 0x80 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    (NDR64_UINT16) 7 /* 0x7 */,
    (NDR64_UINT16) 8 /* 0x8 */,
    (NDR64_UINT32) 8 /* 0x8 */,
    (NDR64_UINT32) 0 /* 0x0 */,
    &__midl_frag198
};

static const __midl_frag196_t __midl_frag196 =
{ 
/* *wireBSTR */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 4 /* 0x4 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag197
};

static const __midl_frag195_t __midl_frag195 =
{ 
/* get_Desc */
    { 
    /* get_Desc */      /* procedure get_Desc */
        (NDR64_UINT32) 4849987 /* 0x4a0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn, ClientCorrelation */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* cnt */      /* parameter cnt */
        &__midl_frag197,
        { 
        /* cnt */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* MustSize, MustFree, [out], SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag222,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag192_t __midl_frag192 =
{ 
/* put_Value */
    { 
    /* put_Value */      /* procedure put_Value */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* cnt */      /* parameter cnt */
        &__midl_frag222,
        { 
        /* cnt */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag222,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag189_t __midl_frag189 =
{ 
/* *long */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 12 /* 0xc */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag222
};

static const __midl_frag188_t __midl_frag188 =
{ 
/* get_Value */
    { 
    /* get_Value */      /* procedure get_Value */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* cnt */      /* parameter cnt */
        &__midl_frag222,
        { 
        /* cnt */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag222,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag180_t __midl_frag180 =
{ 
/* *UINT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag222
};

static const __midl_frag177_t __midl_frag177 =
0x7    /* FC64_INT64 */;

static const __midl_frag176_t __midl_frag176 =
{ 
/* *ULONGLONG */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag177
};

static const __midl_frag173_t __midl_frag173 =
0x4    /* FC64_INT16 */;

static const __midl_frag172_t __midl_frag172 =
{ 
/* *USHORT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag173
};

static const __midl_frag171_t __midl_frag171 =
0x10    /* FC64_CHAR */;

static const __midl_frag170_t __midl_frag170 =
{ 
/* *CHAR */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag171
};

static const __midl_frag169_t __midl_frag169 =
{ 
/* *DECIMAL */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag168
};

static const __midl_frag168_t __midl_frag168 =
{ 
/* DECIMAL */
    { 
    /* DECIMAL */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* DECIMAL */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */
    }
};

static const __midl_frag160_t __midl_frag160 =
{ 
/* **_wireVARIANT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag207
};

static const __midl_frag159_t __midl_frag159 =
{ 
/* *_wireSAFEARRAY */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag62
};

static const __midl_frag158_t __midl_frag158 =
{ 
/* **_wireSAFEARRAY */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag159
};

static const __midl_frag157_t __midl_frag157 =
{ 
/* ***_wireSAFEARRAY */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag158
};

static const __midl_frag154_t __midl_frag154 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag220
};

static const __midl_frag153_t __midl_frag153 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x00000000,
        0x0000,
        0x0000,
        {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
    }
};

static const __midl_frag152_t __midl_frag152 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag153
};

static const __midl_frag151_t __midl_frag151 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag152
};

static const __midl_frag149_t __midl_frag149 =
{ 
/* **FLAGGED_WORD_BLOB */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag198
};

static const __midl_frag148_t __midl_frag148 =
0xc    /* FC64_FLOAT64 */;

static const __midl_frag147_t __midl_frag147 =
{ 
/* *DATE */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag148
};

static const __midl_frag146_t __midl_frag146 =
{ 
/* *CY */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag53
};

static const __midl_frag139_t __midl_frag139 =
0xb    /* FC64_FLOAT32 */;

static const __midl_frag138_t __midl_frag138 =
{ 
/* *FLOAT */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag139
};

static const __midl_frag131_t __midl_frag131 =
0x2    /* FC64_INT8 */;

static const __midl_frag130_t __midl_frag130 =
{ 
/* *BYTE */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 8 /* 0x8 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag131
};

static const __midl_frag129_t __midl_frag129 =
{ 
/* *_wireBRECORD */
    0x22,    /* FC64_OP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag94
};

static const __midl_frag128_t __midl_frag128 =
{ 
/*  */
    { 
    /* *hyper */
        0x22,    /* FC64_OP */
        (NDR64_UINT8) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag125
    }
};

static const __midl_frag126_t __midl_frag126 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 0 /* 0x0 */
    }
};

static const __midl_frag125_t __midl_frag125 =
{ 
/* *hyper */
    { 
    /* *hyper */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* *hyper */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag126
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag177
    }
};

static const __midl_frag124_t __midl_frag124 =
{ 
/* HYPER_SIZEDARR */
    { 
    /* HYPER_SIZEDARR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* HYPER_SIZEDARR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag128,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag123_t __midl_frag123 =
{ 
/*  */
    { 
    /* *ULONG */
        0x22,    /* FC64_OP */
        (NDR64_UINT8) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag120
    }
};

static const __midl_frag120_t __midl_frag120 =
{ 
/* *ULONG */
    { 
    /* *ULONG */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* *ULONG */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 4 /* 0x4 */,
        &__midl_frag126
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 4 /* 0x4 */,
        &__midl_frag222
    }
};

static const __midl_frag119_t __midl_frag119 =
{ 
/* DWORD_SIZEDARR */
    { 
    /* DWORD_SIZEDARR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* DWORD_SIZEDARR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag123,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag118_t __midl_frag118 =
{ 
/*  */
    { 
    /* *short */
        0x22,    /* FC64_OP */
        (NDR64_UINT8) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag115
    }
};

static const __midl_frag115_t __midl_frag115 =
{ 
/* *short */
    { 
    /* *short */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* *short */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag126
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag173
    }
};

static const __midl_frag114_t __midl_frag114 =
{ 
/* WORD_SIZEDARR */
    { 
    /* WORD_SIZEDARR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* WORD_SIZEDARR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag118,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag113_t __midl_frag113 =
{ 
/*  */
    { 
    /* *byte */
        0x22,    /* FC64_OP */
        (NDR64_UINT8) 32 /* 0x20 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag110
    }
};

static const __midl_frag110_t __midl_frag110 =
{ 
/* *byte */
    { 
    /* *byte */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* *byte */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag126
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag131
    }
};

static const __midl_frag109_t __midl_frag109 =
{ 
/* BYTE_SIZEDARR */
    { 
    /* BYTE_SIZEDARR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* BYTE_SIZEDARR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag113,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag108_t __midl_frag108 =
{ 
/*  */
    { 
    /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
        0x40,    /* FC64_FIX_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* struct _NDR64_FIX_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */
    }
};

static const __midl_frag107_t __midl_frag107 =
{ 
/*  */
    { 
    /* **struct _NDR64_POINTER_FORMAT */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag102
    }
};

static const __midl_frag102_t __midl_frag102 =
{ 
/* ** */
    { 
    /* **struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag126
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *struct _NDR64_POINTER_FORMAT */
                0x24,    /* FC64_IP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag153
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag152
    }
};

static const __midl_frag101_t __midl_frag101 =
{ 
/* SAFEARR_HAVEIID */
    { 
    /* SAFEARR_HAVEIID */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_HAVEIID */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        0,
        0,
        &__midl_frag107,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x4,    /* FC64_INT16 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x4,    /* FC64_INT16 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag108
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag100_t __midl_frag100 =
{ 
/*  */
    { 
    /* **_wireBRECORD */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag92
    }
};

static const __midl_frag97_t __midl_frag97 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 4 /* 0x4 */
    }
};

static const __midl_frag96_t __midl_frag96 =
{ 
/* *byte */
    { 
    /* *byte */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 0 /* 0x0 */,
        { 
        /* *byte */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag97
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 1 /* 0x1 */,
        &__midl_frag131
    }
};

static const __midl_frag95_t __midl_frag95 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x0000002f,
        0x0000,
        0x0000,
        {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
    }
};

static const __midl_frag94_t __midl_frag94 =
{ 
/* _wireBRECORD */
    { 
    /* _wireBRECORD */
        0x31,    /* FC64_PSTRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* _wireBRECORD */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 24 /* 0x18 */
    },
    { 
    /*  */
        { 
        /* struct _NDR64_NO_REPEAT_FORMAT */
            0x80,    /* FC64_NO_REPEAT */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* *struct _NDR64_POINTER_FORMAT */
            0x24,    /* FC64_IP */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag95
        },
        { 
        /* struct _NDR64_NO_REPEAT_FORMAT */
            0x80,    /* FC64_NO_REPEAT */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
            (NDR64_UINT32) 16 /* 0x10 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* *byte */
            0x22,    /* FC64_OP */
            (NDR64_UINT8) 32 /* 0x20 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag96
        },
        0x93    /* FC64_END */
    }
};

static const __midl_frag92_t __midl_frag92 =
{ 
/* **_wireBRECORD */
    { 
    /* **_wireBRECORD */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **_wireBRECORD */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag126
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *_wireBRECORD */
                0x22,    /* FC64_OP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag94
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag129
    }
};

static const __midl_frag91_t __midl_frag91 =
{ 
/* SAFEARR_BRECORD */
    { 
    /* SAFEARR_BRECORD */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_BRECORD */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag100,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag90_t __midl_frag90 =
{ 
/*  */
    { 
    /* **_wireVARIANT */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag87
    }
};

static const __midl_frag87_t __midl_frag87 =
{ 
/* **_wireVARIANT */
    { 
    /* **_wireVARIANT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **_wireVARIANT */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag126
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *_wireVARIANT */
                0x22,    /* FC64_OP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag42
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag207
    }
};

static const __midl_frag86_t __midl_frag86 =
{ 
/* SAFEARR_VARIANT */
    { 
    /* SAFEARR_VARIANT */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_VARIANT */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag90,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag85_t __midl_frag85 =
{ 
/*  */
    { 
    /* **struct _NDR64_POINTER_FORMAT */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag80
    }
};

static const __midl_frag80_t __midl_frag80 =
{ 
/* ** */
    { 
    /* **struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag126
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *struct _NDR64_POINTER_FORMAT */
                0x24,    /* FC64_IP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag221
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag220
    }
};

static const __midl_frag79_t __midl_frag79 =
{ 
/* SAFEARR_DISPATCH */
    { 
    /* SAFEARR_DISPATCH */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_DISPATCH */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag85,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag72_t __midl_frag72 =
{ 
/* SAFEARR_UNKNOWN */
    { 
    /* SAFEARR_UNKNOWN */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_UNKNOWN */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag107,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag71_t __midl_frag71 =
{ 
/*  */
    { 
    /* **FLAGGED_WORD_BLOB */
        0x20,    /* FC64_RP */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        &__midl_frag68
    }
};

static const __midl_frag68_t __midl_frag68 =
{ 
/* **FLAGGED_WORD_BLOB */
    { 
    /* **FLAGGED_WORD_BLOB */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* **FLAGGED_WORD_BLOB */
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag126
    },
    { 
    /*  */
        { 
        /* struct _NDR64_REPEAT_FORMAT */
            0x82,    /* FC64_VARIABLE_REPEAT */
            { 
            /* struct _NDR64_REPEAT_FORMAT */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT8) 0 /* 0x0 */
            },
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 8 /* 0x8 */,
            (NDR64_UINT32) 0 /* 0x0 */,
            (NDR64_UINT32) 1 /* 0x1 */
        },
        { 
        /*  */
            { 
            /* struct _NDR64_POINTER_INSTANCE_HEADER_FORMAT */
                (NDR64_UINT32) 0 /* 0x0 */,
                (NDR64_UINT32) 0 /* 0x0 */
            },
            { 
            /* *FLAGGED_WORD_BLOB */
                0x22,    /* FC64_OP */
                (NDR64_UINT8) 0 /* 0x0 */,
                (NDR64_UINT16) 0 /* 0x0 */,
                &__midl_frag29
            }
        },
        0x93    /* FC64_END */
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag198
    }
};

static const __midl_frag67_t __midl_frag67 =
{ 
/* SAFEARR_BSTR */
    { 
    /* SAFEARR_BSTR */
        0x35,    /* FC64_FORCED_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* SAFEARR_BSTR */
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 16 /* 0x10 */,
        0,
        0,
        &__midl_frag71,
    },
    { 
    /*  */
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x5,    /* FC64_INT32 */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x14,    /* FC64_POINTER */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag66_t __midl_frag66 =
{ 
/* SAFEARRAYUNION */
    { 
    /* SAFEARRAYUNION */
        0x50,    /* FC64_ENCAPSULATED_UNION */
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT8) 0 /* 0x0 */,
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM_SELECTOR */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 10 /* 0xa */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 8 /* 0x8 */,
        &__midl_frag67,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 13 /* 0xd */,
        &__midl_frag72,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 9 /* 0x9 */,
        &__midl_frag79,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 12 /* 0xc */,
        &__midl_frag86,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 36 /* 0x24 */,
        &__midl_frag91,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 32781 /* 0x800d */,
        &__midl_frag101,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16 /* 0x10 */,
        &__midl_frag109,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 2 /* 0x2 */,
        &__midl_frag114,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 3 /* 0x3 */,
        &__midl_frag119,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 20 /* 0x14 */,
        &__midl_frag124,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    (NDR64_UINT32) 4294967295 /* 0xffffffff */
};

static const __midl_frag65_t __midl_frag65 =
{ 
/* SAFEARRAYBOUND */
    { 
    /* SAFEARRAYBOUND */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* SAFEARRAYBOUND */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */
    }
};

static const __midl_frag64_t __midl_frag64 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x3,    /* FC64_UINT16 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 0 /* 0x0 */
    }
};

static const __midl_frag63_t __midl_frag63 =
{ 
/*  */
    { 
    /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag64
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag65
    }
};

static const __midl_frag62_t __midl_frag62 =
{ 
/* _wireSAFEARRAY */
    { 
    /* _wireSAFEARRAY */
        0x36,    /* FC64_CONF_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* _wireSAFEARRAY */
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 1 /* 0x1 */,
        (NDR64_UINT32) 56 /* 0x38 */,
        0,
        0,
        0,
        &__midl_frag63,
    },
    { 
    /*  */
        { 
        /* _wireSAFEARRAY */
            0x30,    /* FC64_STRUCT */
            (NDR64_UINT8) 1 /* 0x1 */,
            (NDR64_UINT16) 12 /* 0xc */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_MEMPAD_FORMAT */
            0x90,    /* FC64_STRUCTPADN */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 4 /* 0x4 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag66
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag53_t __midl_frag53 =
{ 
/* CY */
    { 
    /* CY */
        0x30,    /* FC64_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* CY */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */
    }
};

static const __midl_frag44_t __midl_frag44 =
{ 
/*  */
    (NDR64_UINT32) 1 /* 0x1 */,
    { 
    /* struct _NDR64_EXPR_VAR */
        0x3,    /* FC_EXPR_VAR */
        0x3,    /* FC64_UINT16 */
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */
    }
};

static const __midl_frag43_t __midl_frag43 =
{ 
/* __MIDL_IOleAutomationTypes_0004 */
    { 
    /* __MIDL_IOleAutomationTypes_0004 */
        0x51,    /* FC64_NON_ENCAPSULATED_UNION */
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT8) 0 /* 0x0 */,
        0x6,    /* FC64_UINT32 */
        (NDR64_UINT32) 16 /* 0x10 */,
        &__midl_frag44,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM_SELECTOR */
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT8) 7 /* 0x7 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT32) 47 /* 0x2f */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 20 /* 0x14 */,
        &__midl_frag177,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 3 /* 0x3 */,
        &__midl_frag222,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 17 /* 0x11 */,
        &__midl_frag131,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 2 /* 0x2 */,
        &__midl_frag173,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 4 /* 0x4 */,
        &__midl_frag139,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 5 /* 0x5 */,
        &__midl_frag148,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 11 /* 0xb */,
        &__midl_frag173,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 10 /* 0xa */,
        &__midl_frag222,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 6 /* 0x6 */,
        &__midl_frag53,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 7 /* 0x7 */,
        &__midl_frag148,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 8 /* 0x8 */,
        &__midl_frag198,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 13 /* 0xd */,
        &__midl_frag152,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 9 /* 0x9 */,
        &__midl_frag220,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 8192 /* 0x2000 */,
        &__midl_frag158,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 36 /* 0x24 */,
        &__midl_frag129,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16420 /* 0x4024 */,
        &__midl_frag129,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16401 /* 0x4011 */,
        &__midl_frag130,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16386 /* 0x4002 */,
        &__midl_frag172,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16387 /* 0x4003 */,
        &__midl_frag180,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16404 /* 0x4014 */,
        &__midl_frag176,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16388 /* 0x4004 */,
        &__midl_frag138,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16389 /* 0x4005 */,
        &__midl_frag147,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16395 /* 0x400b */,
        &__midl_frag172,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16394 /* 0x400a */,
        &__midl_frag180,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16390 /* 0x4006 */,
        &__midl_frag146,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16391 /* 0x4007 */,
        &__midl_frag147,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16392 /* 0x4008 */,
        &__midl_frag149,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16397 /* 0x400d */,
        &__midl_frag151,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16393 /* 0x4009 */,
        &__midl_frag154,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 24576 /* 0x6000 */,
        &__midl_frag157,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16396 /* 0x400c */,
        &__midl_frag160,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16 /* 0x10 */,
        &__midl_frag171,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 18 /* 0x12 */,
        &__midl_frag173,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 19 /* 0x13 */,
        &__midl_frag222,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 21 /* 0x15 */,
        &__midl_frag177,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 22 /* 0x16 */,
        &__midl_frag222,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 23 /* 0x17 */,
        &__midl_frag222,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 14 /* 0xe */,
        &__midl_frag168,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16398 /* 0x400e */,
        &__midl_frag169,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16400 /* 0x4010 */,
        &__midl_frag170,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16402 /* 0x4012 */,
        &__midl_frag172,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16403 /* 0x4013 */,
        &__midl_frag180,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16405 /* 0x4015 */,
        &__midl_frag176,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16406 /* 0x4016 */,
        &__midl_frag180,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 16407 /* 0x4017 */,
        &__midl_frag180,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 0 /* 0x0 */,
        0,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    { 
    /* struct _NDR64_UNION_ARM */
        (NDR64_INT64) 1 /* 0x1 */,
        0,
        (NDR64_UINT32) 0 /* 0x0 */
    },
    (NDR64_UINT32) 4294967295 /* 0xffffffff */
};

static const __midl_frag42_t __midl_frag42 =
{ 
/* _wireVARIANT */
    { 
    /* _wireVARIANT */
        0x34,    /* FC64_BOGUS_STRUCT */
        (NDR64_UINT8) 7 /* 0x7 */,
        { 
        /* _wireVARIANT */
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 32 /* 0x20 */,
        0,
        0,
        0,
    },
    { 
    /*  */
        { 
        /* _wireVARIANT */
            0x30,    /* FC64_STRUCT */
            (NDR64_UINT8) 3 /* 0x3 */,
            (NDR64_UINT16) 16 /* 0x10 */,
            (NDR64_UINT32) 0 /* 0x0 */
        },
        { 
        /* struct _NDR64_EMBEDDED_COMPLEX_FORMAT */
            0x91,    /* FC64_EMBEDDED_COMPLEX */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            &__midl_frag43
        },
        { 
        /* struct _NDR64_SIMPLE_MEMBER_FORMAT */
            0x93,    /* FC64_END */
            (NDR64_UINT8) 0 /* 0x0 */,
            (NDR64_UINT16) 0 /* 0x0 */,
            (NDR64_UINT32) 0 /* 0x0 */
        }
    }
};

static const __midl_frag30_t __midl_frag30 =
{ 
/*  */
    { 
    /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
        0x41,    /* FC64_CONF_ARRAY */
        (NDR64_UINT8) 1 /* 0x1 */,
        { 
        /* struct _NDR64_CONF_ARRAY_HEADER_FORMAT */
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag97
    },
    { 
    /* struct _NDR64_ARRAY_ELEMENT_INFO */
        (NDR64_UINT32) 2 /* 0x2 */,
        &__midl_frag173
    }
};

static const __midl_frag29_t __midl_frag29 =
{ 
/* FLAGGED_WORD_BLOB */
    { 
    /* FLAGGED_WORD_BLOB */
        0x32,    /* FC64_CONF_STRUCT */
        (NDR64_UINT8) 3 /* 0x3 */,
        { 
        /* FLAGGED_WORD_BLOB */
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0
        },
        (NDR64_UINT8) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        &__midl_frag30
    }
};

static const __midl_frag20_t __midl_frag20 =
{ 
/* Item */
    { 
    /* Item */      /* procedure Item */
        (NDR64_UINT32) 524611 /* 0x80143 */,    /* auto handle */ /* IsIntrepreted, [object], HasReturn */
        (NDR64_UINT32) 32 /* 0x20 */ ,  /* Stack size */
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT32) 40 /* 0x28 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 3 /* 0x3 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* index */      /* parameter index */
        &__midl_frag222,
        { 
        /* index */
            0,
            0,
            0,
            1,
            0,
            0,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [in], Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* cnt */      /* parameter cnt */
        &__midl_frag222,
        { 
        /* cnt */
            0,
            0,
            0,
            0,
            1,
            0,
            1,
            0,
            1,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            1
        },    /* [out], Basetype, SimpleRef, UseCache */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag222,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        24 /* 0x18 */,   /* Stack offset */
    }
};

static const __midl_frag14_t __midl_frag14 =
{ 
/* struct _NDR64_CONSTANT_IID_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 1 /* 0x1 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    {
        0x00020404,
        0x0000,
        0x0000,
        {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
    }
};

static const __midl_frag13_t __midl_frag13 =
{ 
/* *struct _NDR64_POINTER_FORMAT */
    0x24,    /* FC64_IP */
    (NDR64_UINT8) 0 /* 0x0 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag14
};

static const __midl_frag12_t __midl_frag12 =
{ 
/* **struct _NDR64_POINTER_FORMAT */
    0x20,    /* FC64_RP */
    (NDR64_UINT8) 16 /* 0x10 */,
    (NDR64_UINT16) 0 /* 0x0 */,
    &__midl_frag13
};

static const __midl_frag11_t __midl_frag11 =
{ 
/* _Enum */
    { 
    /* _Enum */      /* procedure _Enum */
        (NDR64_UINT32) 655683 /* 0xa0143 */,    /* auto handle */ /* IsIntrepreted, [object], ServerMustSize, HasReturn */
        (NDR64_UINT32) 24 /* 0x18 */ ,  /* Stack size */
        (NDR64_UINT32) 0 /* 0x0 */,
        (NDR64_UINT32) 8 /* 0x8 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 0 /* 0x0 */,
        (NDR64_UINT16) 2 /* 0x2 */,
        (NDR64_UINT16) 0 /* 0x0 */
    },
    { 
    /* enumerator */      /* parameter enumerator */
        &__midl_frag12,
        { 
        /* enumerator */
            1,
            1,
            0,
            0,
            1,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* MustSize, MustFree, [out] */
        (NDR64_UINT16) 0 /* 0x0 */,
        8 /* 0x8 */,   /* Stack offset */
    },
    { 
    /* HRESULT */      /* parameter HRESULT */
        &__midl_frag222,
        { 
        /* HRESULT */
            0,
            0,
            0,
            0,
            1,
            1,
            1,
            1,
            0,
            0,
            0,
            0,
            0,
            (NDR64_UINT16) 0 /* 0x0 */,
            0
        },    /* [out], IsReturn, Basetype, ByValue */
        (NDR64_UINT16) 0 /* 0x0 */,
        16 /* 0x10 */,   /* Stack offset */
    }
};

static const __midl_frag1_t __midl_frag1 =
(NDR64_UINT32) 0 /* 0x0 */;


#include "poppack.h"


static const USER_MARSHAL_ROUTINE_QUADRUPLE NDR64_UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize64
            ,BSTR_UserMarshal64
            ,BSTR_UserUnmarshal64
            ,BSTR_UserFree64
            },
            {
            VARIANT_UserSize64
            ,VARIANT_UserMarshal64
            ,VARIANT_UserUnmarshal64
            ,VARIANT_UserFree64
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ITestLongCollection, ver. 0.0,
   GUID={0xa03d1420,0xb1ec,0x11d0,{0x8c,0x3a,0x00,0xc0,0x4f,0xc3,0x1d,0x2f}} */

#pragma code_seg(".orpc")
static const FormatInfoRef ITestLongCollection_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag188,
    &__midl_frag20,
    &__midl_frag11
    };


static const MIDL_SYNTAX_INFO ITestLongCollection_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    test__MIDL_ProcFormatString.Format,
    &ITestLongCollection_FormatStringOffsetTable[-3],
    test__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &ITestLongCollection_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO ITestLongCollection_ProxyInfo =
    {
    &Object_StubDesc,
    test__MIDL_ProcFormatString.Format,
    &ITestLongCollection_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)ITestLongCollection_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO ITestLongCollection_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    test__MIDL_ProcFormatString.Format,
    (unsigned short *) &ITestLongCollection_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)ITestLongCollection_SyntaxInfo
    };
CINTERFACE_PROXY_VTABLE(10) _ITestLongCollectionProxyVtbl = 
{
    &ITestLongCollection_ProxyInfo,
    &IID_ITestLongCollection,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* ITestLongCollection::get_Count */ ,
    (void *) (INT_PTR) -1 /* ITestLongCollection::Item */ ,
    (void *) (INT_PTR) -1 /* ITestLongCollection::_Enum */
};


static const PRPC_STUB_FUNCTION ITestLongCollection_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _ITestLongCollectionStubVtbl =
{
    &IID_ITestLongCollection,
    &ITestLongCollection_ServerInfo,
    10,
    &ITestLongCollection_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: ITestSimpleUnknown, ver. 0.0,
   GUID={0x6FB050D8,0x438A,0x41B8,{0xA5,0x6E,0x26,0xD6,0xF3,0x44,0x4C,0xE6}} */

#pragma code_seg(".orpc")
static const FormatInfoRef ITestSimpleUnknown_Ndr64ProcTable[] =
    {
    &__midl_frag188,
    &__midl_frag20
    };


static const MIDL_SYNTAX_INFO ITestSimpleUnknown_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    test__MIDL_ProcFormatString.Format,
    &ITestSimpleUnknown_FormatStringOffsetTable[-3],
    test__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &ITestSimpleUnknown_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO ITestSimpleUnknown_ProxyInfo =
    {
    &Object_StubDesc,
    test__MIDL_ProcFormatString.Format,
    &ITestSimpleUnknown_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)ITestSimpleUnknown_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO ITestSimpleUnknown_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    test__MIDL_ProcFormatString.Format,
    (unsigned short *) &ITestSimpleUnknown_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)ITestSimpleUnknown_SyntaxInfo
    };
CINTERFACE_PROXY_VTABLE(5) _ITestSimpleUnknownProxyVtbl = 
{
    &ITestSimpleUnknown_ProxyInfo,
    &IID_ITestSimpleUnknown,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* ITestSimpleUnknown::Count */ ,
    (void *) (INT_PTR) -1 /* ITestSimpleUnknown::Item */
};

const CInterfaceStubVtbl _ITestSimpleUnknownStubVtbl =
{
    &IID_ITestSimpleUnknown,
    &ITestSimpleUnknown_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: ISubObject, ver. 0.0,
   GUID={0xB555296F,0xD09B,0x4A4E,{0xA1,0x48,0xE0,0x71,0x53,0x94,0xA5,0x25}} */

#pragma code_seg(".orpc")
static const FormatInfoRef ISubObject_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag195,
    &__midl_frag200,
    &__midl_frag204,
    &__midl_frag209
    };


static const MIDL_SYNTAX_INFO ISubObject_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    test__MIDL_ProcFormatString.Format,
    &ISubObject_FormatStringOffsetTable[-3],
    test__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &ISubObject_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO ISubObject_ProxyInfo =
    {
    &Object_StubDesc,
    test__MIDL_ProcFormatString.Format,
    &ISubObject_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)ISubObject_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO ISubObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    test__MIDL_ProcFormatString.Format,
    (unsigned short *) &ISubObject_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)ISubObject_SyntaxInfo
    };
CINTERFACE_PROXY_VTABLE(11) _ISubObjectProxyVtbl = 
{
    &ISubObject_ProxyInfo,
    &IID_ISubObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* ISubObject::get_Name */ ,
    (void *) (INT_PTR) -1 /* ISubObject::put_Name */ ,
    (void *) (INT_PTR) -1 /* ISubObject::get_Value */ ,
    (void *) (INT_PTR) -1 /* ISubObject::put_Value */
};


static const PRPC_STUB_FUNCTION ISubObject_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _ISubObjectStubVtbl =
{
    &IID_ISubObject,
    &ISubObject_ServerInfo,
    11,
    &ISubObject_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: ITestObject, ver. 0.0,
   GUID={0xFE795DAE,0x119A,0x4141,{0x96,0xD3,0xF1,0xDE,0xDB,0x5B,0x22,0x59}} */

#pragma code_seg(".orpc")
static const FormatInfoRef ITestObject_Ndr64ProcTable[] =
    {
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    (FormatInfoRef)(LONG_PTR) -1,
    &__midl_frag188,
    &__midl_frag192,
    &__midl_frag195,
    &__midl_frag200,
    &__midl_frag204,
    &__midl_frag209,
    &__midl_frag214,
    &__midl_frag219
    };


static const MIDL_SYNTAX_INFO ITestObject_SyntaxInfo [  2 ] = 
    {
    {
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    test__MIDL_ProcFormatString.Format,
    &ITestObject_FormatStringOffsetTable[-3],
    test__MIDL_TypeFormatString.Format,
    UserMarshalRoutines,
    0,
    0
    }
    ,{
    {{0x71710533,0xbeba,0x4937,{0x83,0x19,0xb5,0xdb,0xef,0x9c,0xcc,0x36}},{1,0}},
    0,
    0 ,
    (unsigned short *) &ITestObject_Ndr64ProcTable[-3],
    0,
    NDR64_UserMarshalRoutines,
    0,
    0
    }
    };

static const MIDL_STUBLESS_PROXY_INFO ITestObject_ProxyInfo =
    {
    &Object_StubDesc,
    test__MIDL_ProcFormatString.Format,
    &ITestObject_FormatStringOffsetTable[-3],
    (RPC_SYNTAX_IDENTIFIER*)&_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)ITestObject_SyntaxInfo
    
    };


static const MIDL_SERVER_INFO ITestObject_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    test__MIDL_ProcFormatString.Format,
    (unsigned short *) &ITestObject_FormatStringOffsetTable[-3],
    0,
    (RPC_SYNTAX_IDENTIFIER*)&_NDR64_RpcTransferSyntax,
    2,
    (MIDL_SYNTAX_INFO*)ITestObject_SyntaxInfo
    };
CINTERFACE_PROXY_VTABLE(15) _ITestObjectProxyVtbl = 
{
    &ITestObject_ProxyInfo,
    &IID_ITestObject,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* ITestObject::get_Value */ ,
    (void *) (INT_PTR) -1 /* ITestObject::put_Value */ ,
    (void *) (INT_PTR) -1 /* ITestObject::get_Desc */ ,
    (void *) (INT_PTR) -1 /* ITestObject::put_Desc */ ,
    (void *) (INT_PTR) -1 /* ITestObject::get_Type */ ,
    (void *) (INT_PTR) -1 /* ITestObject::put_Type */ ,
    (void *) (INT_PTR) -1 /* ITestObject::get_Object */ ,
    (void *) (INT_PTR) -1 /* ITestObject::put_Object */
};


static const PRPC_STUB_FUNCTION ITestObject_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3,
    NdrStubCall3
};

CInterfaceStubVtbl _ITestObjectStubVtbl =
{
    &IID_ITestObject,
    &ITestObject_ServerInfo,
    15,
    &ITestObject_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    test__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x60001, /* Ndr library version */
    0,
    0x801026e, /* MIDL Version 8.1.622 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x2000001, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * const _test_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_ITestLongCollectionProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ISubObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITestObjectProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ITestSimpleUnknownProxyVtbl,
    0
};

const CInterfaceStubVtbl * const _test_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_ITestLongCollectionStubVtbl,
    ( CInterfaceStubVtbl *) &_ISubObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_ITestObjectStubVtbl,
    ( CInterfaceStubVtbl *) &_ITestSimpleUnknownStubVtbl,
    0
};

PCInterfaceName const _test_InterfaceNamesList[] = 
{
    "ITestLongCollection",
    "ISubObject",
    "ITestObject",
    "ITestSimpleUnknown",
    0
};

const IID *  const _test_BaseIIDList[] = 
{
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    0,
    0
};


#define _test_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _test, pIID, n)

int __stdcall _test_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _test, 4, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _test, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _test, 4, *pIndex )
    
}

const ExtendedProxyFileInfo test_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _test_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _test_StubVtblList,
    (const PCInterfaceName * ) & _test_InterfaceNamesList,
    (const IID ** ) & _test_BaseIIDList,
    & _test_IID_Lookup, 
    4,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_AMD64)*/

