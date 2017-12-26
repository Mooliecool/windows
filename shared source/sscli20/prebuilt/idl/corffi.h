// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==


/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
//@@MIDL_FILE_HEADING(  )

#ifdef _MSC_VER
#pragma warning( disable: 4049 )  /* more than 64k source lines */
#endif


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __corffi_h__
#define __corffi_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IManagedInstanceWrapper_FWD_DEFINED__
#define __IManagedInstanceWrapper_FWD_DEFINED__
typedef interface IManagedInstanceWrapper IManagedInstanceWrapper;
#endif 	/* __IManagedInstanceWrapper_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_corffi_0000 */
/* [local] */ 

typedef 
enum CorFFIBindingFlags
    {	CorFFIInvokeMethod	= 0x100,
	CorFFIGetField	= 0x400,
	CorFFISetField	= 0x800,
	CorFFIGetProperty	= 0x1000,
	CorFFISetProperty	= 0x2000
    } 	CorFFIBindingFlags;



extern RPC_IF_HANDLE __MIDL_itf_corffi_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_corffi_0000_v0_0_s_ifspec;

#ifndef __IManagedInstanceWrapper_INTERFACE_DEFINED__
#define __IManagedInstanceWrapper_INTERFACE_DEFINED__

/* interface IManagedInstanceWrapper */
/* [local][unique][uuid][object] */ 


EXTERN_C const IID IID_IManagedInstanceWrapper;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("96E328FE-9A83-439F-A298-8F9CAA2995F0")
    IManagedInstanceWrapper : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InvokeByName( 
            /* [in] */ LPCWSTR MemberName,
            /* [in] */ INT32 BindingFlags,
            /* [in] */ INT32 ArgCount,
            /* [out][in][optional] */ VARIANT *ArgList,
            /* [out][optional] */ VARIANT *pRetVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IManagedInstanceWrapperVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IManagedInstanceWrapper * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IManagedInstanceWrapper * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IManagedInstanceWrapper * This);
        
        HRESULT ( STDMETHODCALLTYPE *InvokeByName )( 
            IManagedInstanceWrapper * This,
            /* [in] */ LPCWSTR MemberName,
            /* [in] */ INT32 BindingFlags,
            /* [in] */ INT32 ArgCount,
            /* [out][in][optional] */ VARIANT *ArgList,
            /* [out][optional] */ VARIANT *pRetVal);
        
        END_INTERFACE
    } IManagedInstanceWrapperVtbl;

    interface IManagedInstanceWrapper
    {
        CONST_VTBL struct IManagedInstanceWrapperVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IManagedInstanceWrapper_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IManagedInstanceWrapper_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IManagedInstanceWrapper_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IManagedInstanceWrapper_InvokeByName(This,MemberName,BindingFlags,ArgCount,ArgList,pRetVal)	\
    (This)->lpVtbl -> InvokeByName(This,MemberName,BindingFlags,ArgCount,ArgList,pRetVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IManagedInstanceWrapper_InvokeByName_Proxy( 
    IManagedInstanceWrapper * This,
    /* [in] */ LPCWSTR MemberName,
    /* [in] */ INT32 BindingFlags,
    /* [in] */ INT32 ArgCount,
    /* [out][in][optional] */ VARIANT *ArgList,
    /* [out][optional] */ VARIANT *pRetVal);


void __RPC_STUB IManagedInstanceWrapper_InvokeByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IManagedInstanceWrapper_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


