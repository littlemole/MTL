

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __test_h_h__
#define __test_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ITestLongCollection_FWD_DEFINED__
#define __ITestLongCollection_FWD_DEFINED__
typedef interface ITestLongCollection ITestLongCollection;

#endif 	/* __ITestLongCollection_FWD_DEFINED__ */


#ifndef __ITestSimpleUnknown_FWD_DEFINED__
#define __ITestSimpleUnknown_FWD_DEFINED__
typedef interface ITestSimpleUnknown ITestSimpleUnknown;

#endif 	/* __ITestSimpleUnknown_FWD_DEFINED__ */


#ifndef __ISubObject_FWD_DEFINED__
#define __ISubObject_FWD_DEFINED__
typedef interface ISubObject ISubObject;

#endif 	/* __ISubObject_FWD_DEFINED__ */


#ifndef __ITestObject_FWD_DEFINED__
#define __ITestObject_FWD_DEFINED__
typedef interface ITestObject ITestObject;

#endif 	/* __ITestObject_FWD_DEFINED__ */


#ifndef __ITestSimpleOleAout_FWD_DEFINED__
#define __ITestSimpleOleAout_FWD_DEFINED__
typedef interface ITestSimpleOleAout ITestSimpleOleAout;

#endif 	/* __ITestSimpleOleAout_FWD_DEFINED__ */


#ifndef __ITestSimpleDispatch_FWD_DEFINED__
#define __ITestSimpleDispatch_FWD_DEFINED__
typedef interface ITestSimpleDispatch ITestSimpleDispatch;

#endif 	/* __ITestSimpleDispatch_FWD_DEFINED__ */


#ifndef __MyLongCollection_FWD_DEFINED__
#define __MyLongCollection_FWD_DEFINED__

#ifdef __cplusplus
typedef class MyLongCollection MyLongCollection;
#else
typedef struct MyLongCollection MyLongCollection;
#endif /* __cplusplus */

#endif 	/* __MyLongCollection_FWD_DEFINED__ */


#ifndef __TestObject_FWD_DEFINED__
#define __TestObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class TestObject TestObject;
#else
typedef struct TestObject TestObject;
#endif /* __cplusplus */

#endif 	/* __TestObject_FWD_DEFINED__ */


#ifndef __SubObject_FWD_DEFINED__
#define __SubObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class SubObject SubObject;
#else
typedef struct SubObject SubObject;
#endif /* __cplusplus */

#endif 	/* __SubObject_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ITestLongCollection_INTERFACE_DEFINED__
#define __ITestLongCollection_INTERFACE_DEFINED__

/* interface ITestLongCollection */
/* [uuid][oleautomation][dual][object] */ 


EXTERN_C const IID IID_ITestLongCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a03d1420-b1ec-11d0-8c3a-00c04fc31d2f")
    ITestLongCollection : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *cnt) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ long index,
            /* [retval][out] */ long *cnt) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE _Enum( 
            /* [retval][out] */ IEnumVARIANT **enumerator) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITestLongCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITestLongCollection * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITestLongCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITestLongCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITestLongCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITestLongCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITestLongCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITestLongCollection * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            ITestLongCollection * This,
            /* [retval][out] */ long *cnt);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            ITestLongCollection * This,
            /* [in] */ long index,
            /* [retval][out] */ long *cnt);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *_Enum )( 
            ITestLongCollection * This,
            /* [retval][out] */ IEnumVARIANT **enumerator);
        
        END_INTERFACE
    } ITestLongCollectionVtbl;

    interface ITestLongCollection
    {
        CONST_VTBL struct ITestLongCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITestLongCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITestLongCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITestLongCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITestLongCollection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITestLongCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITestLongCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITestLongCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ITestLongCollection_get_Count(This,cnt)	\
    ( (This)->lpVtbl -> get_Count(This,cnt) ) 

#define ITestLongCollection_Item(This,index,cnt)	\
    ( (This)->lpVtbl -> Item(This,index,cnt) ) 

#define ITestLongCollection__Enum(This,enumerator)	\
    ( (This)->lpVtbl -> _Enum(This,enumerator) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITestLongCollection_INTERFACE_DEFINED__ */


#ifndef __ITestSimpleUnknown_INTERFACE_DEFINED__
#define __ITestSimpleUnknown_INTERFACE_DEFINED__

/* interface ITestSimpleUnknown */
/* [uuid][object] */ 


EXTERN_C const IID IID_ITestSimpleUnknown;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6FB050D8-438A-41B8-A56E-26D6F3444CE6")
    ITestSimpleUnknown : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Count( 
            /* [retval][out] */ long *cnt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ long index,
            /* [retval][out] */ long *cnt) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITestSimpleUnknownVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITestSimpleUnknown * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITestSimpleUnknown * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITestSimpleUnknown * This);
        
        HRESULT ( STDMETHODCALLTYPE *Count )( 
            ITestSimpleUnknown * This,
            /* [retval][out] */ long *cnt);
        
        HRESULT ( STDMETHODCALLTYPE *Item )( 
            ITestSimpleUnknown * This,
            /* [in] */ long index,
            /* [retval][out] */ long *cnt);
        
        END_INTERFACE
    } ITestSimpleUnknownVtbl;

    interface ITestSimpleUnknown
    {
        CONST_VTBL struct ITestSimpleUnknownVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITestSimpleUnknown_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITestSimpleUnknown_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITestSimpleUnknown_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITestSimpleUnknown_Count(This,cnt)	\
    ( (This)->lpVtbl -> Count(This,cnt) ) 

#define ITestSimpleUnknown_Item(This,index,cnt)	\
    ( (This)->lpVtbl -> Item(This,index,cnt) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITestSimpleUnknown_INTERFACE_DEFINED__ */


#ifndef __ISubObject_INTERFACE_DEFINED__
#define __ISubObject_INTERFACE_DEFINED__

/* interface ISubObject */
/* [uuid][oleautomation][dual][object] */ 


EXTERN_C const IID IID_ISubObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B555296F-D09B-4A4E-A148-E0715394A525")
    ISubObject : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *cnt) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR cnt) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ VARIANT *cnt) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ VARIANT cnt) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ISubObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISubObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISubObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISubObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISubObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISubObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISubObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISubObject * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            ISubObject * This,
            /* [retval][out] */ BSTR *cnt);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            ISubObject * This,
            /* [in] */ BSTR cnt);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            ISubObject * This,
            /* [retval][out] */ VARIANT *cnt);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Value )( 
            ISubObject * This,
            /* [in] */ VARIANT cnt);
        
        END_INTERFACE
    } ISubObjectVtbl;

    interface ISubObject
    {
        CONST_VTBL struct ISubObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISubObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISubObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISubObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISubObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISubObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISubObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISubObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISubObject_get_Name(This,cnt)	\
    ( (This)->lpVtbl -> get_Name(This,cnt) ) 

#define ISubObject_put_Name(This,cnt)	\
    ( (This)->lpVtbl -> put_Name(This,cnt) ) 

#define ISubObject_get_Value(This,cnt)	\
    ( (This)->lpVtbl -> get_Value(This,cnt) ) 

#define ISubObject_put_Value(This,cnt)	\
    ( (This)->lpVtbl -> put_Value(This,cnt) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISubObject_INTERFACE_DEFINED__ */


#ifndef __ITestObject_INTERFACE_DEFINED__
#define __ITestObject_INTERFACE_DEFINED__

/* interface ITestObject */
/* [uuid][oleautomation][dual][object] */ 


EXTERN_C const IID IID_ITestObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FE795DAE-119A-4141-96D3-F1DEDB5B2259")
    ITestObject : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ long *cnt) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ long cnt) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Desc( 
            /* [retval][out] */ BSTR *cnt) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Desc( 
            /* [in] */ BSTR cnt) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ VARIANT *cnt) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Type( 
            /* [in] */ VARIANT cnt) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Object( 
            /* [retval][out] */ IDispatch **cnt) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Object( 
            /* [in] */ IDispatch *cnt) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITestObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITestObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITestObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITestObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITestObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITestObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITestObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITestObject * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            ITestObject * This,
            /* [retval][out] */ long *cnt);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Value )( 
            ITestObject * This,
            /* [in] */ long cnt);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Desc )( 
            ITestObject * This,
            /* [retval][out] */ BSTR *cnt);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Desc )( 
            ITestObject * This,
            /* [in] */ BSTR cnt);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            ITestObject * This,
            /* [retval][out] */ VARIANT *cnt);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Type )( 
            ITestObject * This,
            /* [in] */ VARIANT cnt);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Object )( 
            ITestObject * This,
            /* [retval][out] */ IDispatch **cnt);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Object )( 
            ITestObject * This,
            /* [in] */ IDispatch *cnt);
        
        END_INTERFACE
    } ITestObjectVtbl;

    interface ITestObject
    {
        CONST_VTBL struct ITestObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITestObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITestObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITestObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITestObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITestObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITestObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITestObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ITestObject_get_Value(This,cnt)	\
    ( (This)->lpVtbl -> get_Value(This,cnt) ) 

#define ITestObject_put_Value(This,cnt)	\
    ( (This)->lpVtbl -> put_Value(This,cnt) ) 

#define ITestObject_get_Desc(This,cnt)	\
    ( (This)->lpVtbl -> get_Desc(This,cnt) ) 

#define ITestObject_put_Desc(This,cnt)	\
    ( (This)->lpVtbl -> put_Desc(This,cnt) ) 

#define ITestObject_get_Type(This,cnt)	\
    ( (This)->lpVtbl -> get_Type(This,cnt) ) 

#define ITestObject_put_Type(This,cnt)	\
    ( (This)->lpVtbl -> put_Type(This,cnt) ) 

#define ITestObject_get_Object(This,cnt)	\
    ( (This)->lpVtbl -> get_Object(This,cnt) ) 

#define ITestObject_put_Object(This,cnt)	\
    ( (This)->lpVtbl -> put_Object(This,cnt) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITestObject_INTERFACE_DEFINED__ */



#ifndef __ExampleLib_LIBRARY_DEFINED__
#define __ExampleLib_LIBRARY_DEFINED__

/* library ExampleLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ExampleLib;

#ifndef __ITestSimpleOleAout_INTERFACE_DEFINED__
#define __ITestSimpleOleAout_INTERFACE_DEFINED__

/* interface ITestSimpleOleAout */
/* [uuid][oleautomation][object] */ 


EXTERN_C const IID IID_ITestSimpleOleAout;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C54DDE14-1DF9-436D-B6EF-5BF023ED16E2")
    ITestSimpleOleAout : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Count( 
            /* [retval][out] */ long *cnt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ long index,
            /* [retval][out] */ long *cnt) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ITestSimpleOleAoutVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITestSimpleOleAout * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITestSimpleOleAout * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITestSimpleOleAout * This);
        
        HRESULT ( STDMETHODCALLTYPE *Count )( 
            ITestSimpleOleAout * This,
            /* [retval][out] */ long *cnt);
        
        HRESULT ( STDMETHODCALLTYPE *Item )( 
            ITestSimpleOleAout * This,
            /* [in] */ long index,
            /* [retval][out] */ long *cnt);
        
        END_INTERFACE
    } ITestSimpleOleAoutVtbl;

    interface ITestSimpleOleAout
    {
        CONST_VTBL struct ITestSimpleOleAoutVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITestSimpleOleAout_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITestSimpleOleAout_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITestSimpleOleAout_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITestSimpleOleAout_Count(This,cnt)	\
    ( (This)->lpVtbl -> Count(This,cnt) ) 

#define ITestSimpleOleAout_Item(This,index,cnt)	\
    ( (This)->lpVtbl -> Item(This,index,cnt) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITestSimpleOleAout_INTERFACE_DEFINED__ */


#ifndef __ITestSimpleDispatch_DISPINTERFACE_DEFINED__
#define __ITestSimpleDispatch_DISPINTERFACE_DEFINED__

/* dispinterface ITestSimpleDispatch */
/* [uuid] */ 


EXTERN_C const IID DIID_ITestSimpleDispatch;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("FFE85602-BA26-414A-870E-83CCFB74C95A")
    ITestSimpleDispatch : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct ITestSimpleDispatchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITestSimpleDispatch * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITestSimpleDispatch * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITestSimpleDispatch * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITestSimpleDispatch * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITestSimpleDispatch * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITestSimpleDispatch * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITestSimpleDispatch * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } ITestSimpleDispatchVtbl;

    interface ITestSimpleDispatch
    {
        CONST_VTBL struct ITestSimpleDispatchVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITestSimpleDispatch_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITestSimpleDispatch_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITestSimpleDispatch_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITestSimpleDispatch_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ITestSimpleDispatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ITestSimpleDispatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ITestSimpleDispatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __ITestSimpleDispatch_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_MyLongCollection;

#ifdef __cplusplus

class DECLSPEC_UUID("a03d1424-b1ec-11d0-8c3a-00c04fc31d2f")
MyLongCollection;
#endif

EXTERN_C const CLSID CLSID_TestObject;

#ifdef __cplusplus

class DECLSPEC_UUID("BA446D99-6A2F-4DE2-B644-101033A0250E")
TestObject;
#endif

EXTERN_C const CLSID CLSID_SubObject;

#ifdef __cplusplus

class DECLSPEC_UUID("C40BFA01-0FE0-4E55-8399-DE862BE9F4CB")
SubObject;
#endif
#endif /* __ExampleLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

unsigned long             __RPC_USER  BSTR_UserSize64(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize64(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal64(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal64(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree64(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


