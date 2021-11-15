

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#ifndef __Editor_h_h__
#define __Editor_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMTLEditorDocument_FWD_DEFINED__
#define __IMTLEditorDocument_FWD_DEFINED__
typedef interface IMTLEditorDocument IMTLEditorDocument;

#endif 	/* __IMTLEditorDocument_FWD_DEFINED__ */


#ifndef __IMTLEditorDocuments_FWD_DEFINED__
#define __IMTLEditorDocuments_FWD_DEFINED__
typedef interface IMTLEditorDocuments IMTLEditorDocuments;

#endif 	/* __IMTLEditorDocuments_FWD_DEFINED__ */


#ifndef __IMTLEditor_FWD_DEFINED__
#define __IMTLEditor_FWD_DEFINED__
typedef interface IMTLEditor IMTLEditor;

#endif 	/* __IMTLEditor_FWD_DEFINED__ */


#ifndef __IMTLScriptHostObject_FWD_DEFINED__
#define __IMTLScriptHostObject_FWD_DEFINED__
typedef interface IMTLScriptHostObject IMTLScriptHostObject;

#endif 	/* __IMTLScriptHostObject_FWD_DEFINED__ */


#ifndef __MTLEditor_FWD_DEFINED__
#define __MTLEditor_FWD_DEFINED__

#ifdef __cplusplus
typedef class MTLEditor MTLEditor;
#else
typedef struct MTLEditor MTLEditor;
#endif /* __cplusplus */

#endif 	/* __MTLEditor_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IMTLEditorDocument_INTERFACE_DEFINED__
#define __IMTLEditorDocument_INTERFACE_DEFINED__

/* interface IMTLEditorDocument */
/* [dual][oleautomation][object][helpstring][uuid] */ 


EXTERN_C const IID IID_IMTLEditorDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5AE58F46-47DC-41F8-883A-54B7719A2FDB")
    IMTLEditorDocument : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_documentId( 
            /* [retval][out] */ BSTR *docId) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_filename( 
            /* [retval][out] */ BSTR *fn) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_content( 
            /* [retval][out] */ BSTR *cnt) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMTLEditorDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMTLEditorDocument * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMTLEditorDocument * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMTLEditorDocument * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMTLEditorDocument * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMTLEditorDocument * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMTLEditorDocument * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMTLEditorDocument * This,
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
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_documentId )( 
            IMTLEditorDocument * This,
            /* [retval][out] */ BSTR *docId);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_filename )( 
            IMTLEditorDocument * This,
            /* [retval][out] */ BSTR *fn);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_content )( 
            IMTLEditorDocument * This,
            /* [retval][out] */ BSTR *cnt);
        
        END_INTERFACE
    } IMTLEditorDocumentVtbl;

    interface IMTLEditorDocument
    {
        CONST_VTBL struct IMTLEditorDocumentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMTLEditorDocument_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMTLEditorDocument_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMTLEditorDocument_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMTLEditorDocument_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMTLEditorDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMTLEditorDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMTLEditorDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IMTLEditorDocument_get_documentId(This,docId)	\
    ( (This)->lpVtbl -> get_documentId(This,docId) ) 

#define IMTLEditorDocument_get_filename(This,fn)	\
    ( (This)->lpVtbl -> get_filename(This,fn) ) 

#define IMTLEditorDocument_get_content(This,cnt)	\
    ( (This)->lpVtbl -> get_content(This,cnt) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMTLEditorDocument_INTERFACE_DEFINED__ */


#ifndef __IMTLEditorDocuments_INTERFACE_DEFINED__
#define __IMTLEditorDocuments_INTERFACE_DEFINED__

/* interface IMTLEditorDocuments */
/* [dual][oleautomation][object][helpstring][uuid] */ 


EXTERN_C const IID IID_IMTLEditorDocuments;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2CA50833-CE2D-4543-B9E6-46DF983C00E5")
    IMTLEditorDocuments : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_count( 
            /* [retval][out] */ long *cnt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE item( 
            /* [in] */ VARIANT idx,
            /* [retval][out] */ IMTLEditorDocument **doc) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE remove( 
            /* [in] */ VARIANT idx) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMTLEditorDocumentsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMTLEditorDocuments * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMTLEditorDocuments * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMTLEditorDocuments * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMTLEditorDocuments * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMTLEditorDocuments * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMTLEditorDocuments * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMTLEditorDocuments * This,
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
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_count )( 
            IMTLEditorDocuments * This,
            /* [retval][out] */ long *cnt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *item )( 
            IMTLEditorDocuments * This,
            /* [in] */ VARIANT idx,
            /* [retval][out] */ IMTLEditorDocument **doc);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *remove )( 
            IMTLEditorDocuments * This,
            /* [in] */ VARIANT idx);
        
        END_INTERFACE
    } IMTLEditorDocumentsVtbl;

    interface IMTLEditorDocuments
    {
        CONST_VTBL struct IMTLEditorDocumentsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMTLEditorDocuments_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMTLEditorDocuments_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMTLEditorDocuments_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMTLEditorDocuments_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMTLEditorDocuments_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMTLEditorDocuments_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMTLEditorDocuments_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IMTLEditorDocuments_get_count(This,cnt)	\
    ( (This)->lpVtbl -> get_count(This,cnt) ) 

#define IMTLEditorDocuments_item(This,idx,doc)	\
    ( (This)->lpVtbl -> item(This,idx,doc) ) 

#define IMTLEditorDocuments_remove(This,idx)	\
    ( (This)->lpVtbl -> remove(This,idx) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMTLEditorDocuments_INTERFACE_DEFINED__ */


#ifndef __IMTLEditor_INTERFACE_DEFINED__
#define __IMTLEditor_INTERFACE_DEFINED__

/* interface IMTLEditor */
/* [dual][oleautomation][object][helpstring][uuid] */ 


EXTERN_C const IID IID_IMTLEditor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BE5C3CDD-1E0A-4ECA-A0FA-FB9E2813932F")
    IMTLEditor : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SayHello( 
            /* [in] */ BSTR message) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ VARIANT value) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ VARIANT *value) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_documents( 
            /* [retval][out] */ IMTLEditorDocuments **docs) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMTLEditorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMTLEditor * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMTLEditor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMTLEditor * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMTLEditor * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMTLEditor * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMTLEditor * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMTLEditor * This,
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
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SayHello )( 
            IMTLEditor * This,
            /* [in] */ BSTR message);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Value )( 
            IMTLEditor * This,
            /* [in] */ VARIANT value);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            IMTLEditor * This,
            /* [retval][out] */ VARIANT *value);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_documents )( 
            IMTLEditor * This,
            /* [retval][out] */ IMTLEditorDocuments **docs);
        
        END_INTERFACE
    } IMTLEditorVtbl;

    interface IMTLEditor
    {
        CONST_VTBL struct IMTLEditorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMTLEditor_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMTLEditor_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMTLEditor_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMTLEditor_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMTLEditor_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMTLEditor_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMTLEditor_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IMTLEditor_SayHello(This,message)	\
    ( (This)->lpVtbl -> SayHello(This,message) ) 

#define IMTLEditor_put_Value(This,value)	\
    ( (This)->lpVtbl -> put_Value(This,value) ) 

#define IMTLEditor_get_Value(This,value)	\
    ( (This)->lpVtbl -> get_Value(This,value) ) 

#define IMTLEditor_get_documents(This,docs)	\
    ( (This)->lpVtbl -> get_documents(This,docs) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMTLEditor_INTERFACE_DEFINED__ */


#ifndef __IMTLScriptHostObject_INTERFACE_DEFINED__
#define __IMTLScriptHostObject_INTERFACE_DEFINED__

/* interface IMTLScriptHostObject */
/* [dual][oleautomation][object][helpstring][uuid] */ 


EXTERN_C const IID IID_IMTLScriptHostObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4F2CB374-9FA4-4135-A98C-C82B0F425CD6")
    IMTLScriptHostObject : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Import( 
            /* [in] */ BSTR value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMTLScriptHostObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMTLScriptHostObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMTLScriptHostObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMTLScriptHostObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMTLScriptHostObject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMTLScriptHostObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMTLScriptHostObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMTLScriptHostObject * This,
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
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Import )( 
            IMTLScriptHostObject * This,
            /* [in] */ BSTR value);
        
        END_INTERFACE
    } IMTLScriptHostObjectVtbl;

    interface IMTLScriptHostObject
    {
        CONST_VTBL struct IMTLScriptHostObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMTLScriptHostObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMTLScriptHostObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMTLScriptHostObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMTLScriptHostObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMTLScriptHostObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMTLScriptHostObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMTLScriptHostObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IMTLScriptHostObject_Import(This,value)	\
    ( (This)->lpVtbl -> Import(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMTLScriptHostObject_INTERFACE_DEFINED__ */



#ifndef __MTLEditor_LIBRARY_DEFINED__
#define __MTLEditor_LIBRARY_DEFINED__

/* library MTLEditor */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MTLEditor;

EXTERN_C const CLSID CLSID_MTLEditor;

#ifdef __cplusplus

class DECLSPEC_UUID("A258064A-8664-42F0-9676-E2EDEB23E3A9")
MTLEditor;
#endif
#endif /* __MTLEditor_LIBRARY_DEFINED__ */

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


