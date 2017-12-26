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

#ifndef __clrinternal_h__
#define __clrinternal_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IExecutionEngine_FWD_DEFINED__
#define __IExecutionEngine_FWD_DEFINED__
typedef interface IExecutionEngine IExecutionEngine;
#endif 	/* __IExecutionEngine_FWD_DEFINED__ */


#ifndef __IEEMemoryManager_FWD_DEFINED__
#define __IEEMemoryManager_FWD_DEFINED__
typedef interface IEEMemoryManager IEEMemoryManager;
#endif 	/* __IEEMemoryManager_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_clrinternal_0000 */
/* [local] */ 

EXTERN_GUID(IID_IExecutionEngine, 0x7AF02DAC, 0x2A33, 0x494b, 0xA0, 0x9F, 0x25, 0xE0, 0x0A, 0x93, 0xC6, 0xF8);
EXTERN_GUID(IID_IEEMemoryManager, 0x17713b61, 0xb59f, 0x4e13, 0xba, 0xaf, 0x91, 0x62, 0x3d, 0xc8, 0xad, 0xc0);
typedef void *CRITSEC_COOKIE;

typedef void *EVENT_COOKIE;

typedef void *SEMAPHORE_COOKIE;

typedef void *MUTEX_COOKIE;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_clrinternal_0000_0001
    {	CRST_DEFAULT	= 0,
	CRST_REENTRANCY	= 0x1,
	CRST_UNSAFE_SAMELEVEL	= 0x2,
	CRST_UNSAFE_COOPGC	= 0x4,
	CRST_UNSAFE_ANYMODE	= 0x8,
	CRST_DEBUGGER_THREAD	= 0x10,
	CRST_HOST_BREAKABLE	= 0x20,
	CRST_SUSPEND_THREAD	= 0x40
    } 	CrstFlags;

typedef VOID ( __stdcall *PTLS_CALLBACK_FUNCTION )( 
    PVOID __MIDL_0000);



extern RPC_IF_HANDLE __MIDL_itf_clrinternal_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_clrinternal_0000_v0_0_s_ifspec;

#ifndef __IExecutionEngine_INTERFACE_DEFINED__
#define __IExecutionEngine_INTERFACE_DEFINED__

/* interface IExecutionEngine */
/* [object][local][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IExecutionEngine;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7AF02DAC-2A33-494b-A09F-25E00A93C6F8")
    IExecutionEngine : public IUnknown
    {
    public:
        virtual void STDMETHODCALLTYPE TLS_AssociateCallback( 
            /* [in] */ DWORD slot,
            /* [in] */ PTLS_CALLBACK_FUNCTION callback) = 0;
        
        virtual DWORD STDMETHODCALLTYPE TLS_GetMasterSlotIndex( void) = 0;
        
        virtual PVOID STDMETHODCALLTYPE TLS_GetValue( 
            /* [in] */ DWORD slot) = 0;
        
        virtual BOOL STDMETHODCALLTYPE TLS_CheckValue( 
            /* [in] */ DWORD slot,
            /* [out] */ PVOID *pValue) = 0;
        
        virtual void STDMETHODCALLTYPE TLS_SetValue( 
            /* [in] */ DWORD slot,
            /* [in] */ PVOID pData) = 0;
        
        virtual void STDMETHODCALLTYPE TLS_ThreadDetaching( void) = 0;
        
        virtual CRITSEC_COOKIE STDMETHODCALLTYPE CreateLock( 
            /* [in] */ LPCSTR szTag,
            /* [in] */ LPCSTR level,
            /* [in] */ CrstFlags flags) = 0;
        
        virtual void STDMETHODCALLTYPE DestroyLock( 
            /* [in] */ CRITSEC_COOKIE lock) = 0;
        
        virtual void STDMETHODCALLTYPE AcquireLock( 
            /* [in] */ CRITSEC_COOKIE lock) = 0;
        
        virtual void STDMETHODCALLTYPE ReleaseLock( 
            /* [in] */ CRITSEC_COOKIE lock) = 0;
        
        virtual EVENT_COOKIE STDMETHODCALLTYPE CreateAutoEvent( 
            /* [in] */ BOOL bInitialState) = 0;
        
        virtual EVENT_COOKIE STDMETHODCALLTYPE CreateManualEvent( 
            /* [in] */ BOOL bInitialState) = 0;
        
        virtual void STDMETHODCALLTYPE CloseEvent( 
            /* [in] */ EVENT_COOKIE event) = 0;
        
        virtual BOOL STDMETHODCALLTYPE ClrSetEvent( 
            /* [in] */ EVENT_COOKIE event) = 0;
        
        virtual BOOL STDMETHODCALLTYPE ClrResetEvent( 
            /* [in] */ EVENT_COOKIE event) = 0;
        
        virtual DWORD STDMETHODCALLTYPE WaitForEvent( 
            /* [in] */ EVENT_COOKIE event,
            /* [in] */ DWORD dwMilliseconds,
            /* [in] */ BOOL bAlertable) = 0;
        
        virtual DWORD STDMETHODCALLTYPE WaitForSingleObject( 
            /* [in] */ HANDLE handle,
            /* [in] */ DWORD dwMilliseconds) = 0;
        
        virtual SEMAPHORE_COOKIE STDMETHODCALLTYPE ClrCreateSemaphore( 
            /* [in] */ DWORD dwInitial,
            /* [in] */ DWORD dwMax) = 0;
        
        virtual void STDMETHODCALLTYPE ClrCloseSemaphore( 
            /* [in] */ SEMAPHORE_COOKIE semaphore) = 0;
        
        virtual DWORD STDMETHODCALLTYPE ClrWaitForSemaphore( 
            /* [in] */ SEMAPHORE_COOKIE semaphore,
            /* [in] */ DWORD dwMilliseconds,
            /* [in] */ BOOL bAlertable) = 0;
        
        virtual BOOL STDMETHODCALLTYPE ClrReleaseSemaphore( 
            /* [in] */ SEMAPHORE_COOKIE semaphore,
            /* [in] */ LONG lReleaseCount,
            /* [in] */ LONG *lpPreviousCount) = 0;
        
        virtual MUTEX_COOKIE STDMETHODCALLTYPE ClrCreateMutex( 
            /* [in] */ LPSECURITY_ATTRIBUTES lpMutexAttributes,
            /* [in] */ BOOL bInitialOwner,
            /* [in] */ LPCTSTR lpName) = 0;
        
        virtual DWORD STDMETHODCALLTYPE ClrWaitForMutex( 
            /* [in] */ MUTEX_COOKIE mutex,
            /* [in] */ DWORD dwMilliseconds,
            /* [in] */ BOOL bAlertable) = 0;
        
        virtual BOOL STDMETHODCALLTYPE ClrReleaseMutex( 
            /* [in] */ MUTEX_COOKIE mutex) = 0;
        
        virtual void STDMETHODCALLTYPE ClrCloseMutex( 
            /* [in] */ MUTEX_COOKIE mutex) = 0;
        
        virtual DWORD STDMETHODCALLTYPE ClrSleepEx( 
            /* [in] */ DWORD dwMilliseconds,
            /* [in] */ BOOL bAlertable) = 0;
        
        virtual BOOL STDMETHODCALLTYPE ClrAllocationDisallowed( void) = 0;
        
        virtual void STDMETHODCALLTYPE GetLastThrownObjectExceptionFromThread( 
            /* [out] */ void **ppvException) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IExecutionEngineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IExecutionEngine * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IExecutionEngine * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IExecutionEngine * This);
        
        void ( STDMETHODCALLTYPE *TLS_AssociateCallback )( 
            IExecutionEngine * This,
            /* [in] */ DWORD slot,
            /* [in] */ PTLS_CALLBACK_FUNCTION callback);
        
        DWORD ( STDMETHODCALLTYPE *TLS_GetMasterSlotIndex )( 
            IExecutionEngine * This);
        
        PVOID ( STDMETHODCALLTYPE *TLS_GetValue )( 
            IExecutionEngine * This,
            /* [in] */ DWORD slot);
        
        BOOL ( STDMETHODCALLTYPE *TLS_CheckValue )( 
            IExecutionEngine * This,
            /* [in] */ DWORD slot,
            /* [out] */ PVOID *pValue);
        
        void ( STDMETHODCALLTYPE *TLS_SetValue )( 
            IExecutionEngine * This,
            /* [in] */ DWORD slot,
            /* [in] */ PVOID pData);
        
        void ( STDMETHODCALLTYPE *TLS_ThreadDetaching )( 
            IExecutionEngine * This);
        
        CRITSEC_COOKIE ( STDMETHODCALLTYPE *CreateLock )( 
            IExecutionEngine * This,
            /* [in] */ LPCSTR szTag,
            /* [in] */ LPCSTR level,
            /* [in] */ CrstFlags flags);
        
        void ( STDMETHODCALLTYPE *DestroyLock )( 
            IExecutionEngine * This,
            /* [in] */ CRITSEC_COOKIE lock);
        
        void ( STDMETHODCALLTYPE *AcquireLock )( 
            IExecutionEngine * This,
            /* [in] */ CRITSEC_COOKIE lock);
        
        void ( STDMETHODCALLTYPE *ReleaseLock )( 
            IExecutionEngine * This,
            /* [in] */ CRITSEC_COOKIE lock);
        
        EVENT_COOKIE ( STDMETHODCALLTYPE *CreateAutoEvent )( 
            IExecutionEngine * This,
            /* [in] */ BOOL bInitialState);
        
        EVENT_COOKIE ( STDMETHODCALLTYPE *CreateManualEvent )( 
            IExecutionEngine * This,
            /* [in] */ BOOL bInitialState);
        
        void ( STDMETHODCALLTYPE *CloseEvent )( 
            IExecutionEngine * This,
            /* [in] */ EVENT_COOKIE event);
        
        BOOL ( STDMETHODCALLTYPE *ClrSetEvent )( 
            IExecutionEngine * This,
            /* [in] */ EVENT_COOKIE event);
        
        BOOL ( STDMETHODCALLTYPE *ClrResetEvent )( 
            IExecutionEngine * This,
            /* [in] */ EVENT_COOKIE event);
        
        DWORD ( STDMETHODCALLTYPE *WaitForEvent )( 
            IExecutionEngine * This,
            /* [in] */ EVENT_COOKIE event,
            /* [in] */ DWORD dwMilliseconds,
            /* [in] */ BOOL bAlertable);
        
        DWORD ( STDMETHODCALLTYPE *WaitForSingleObject )( 
            IExecutionEngine * This,
            /* [in] */ HANDLE handle,
            /* [in] */ DWORD dwMilliseconds);
        
        SEMAPHORE_COOKIE ( STDMETHODCALLTYPE *ClrCreateSemaphore )( 
            IExecutionEngine * This,
            /* [in] */ DWORD dwInitial,
            /* [in] */ DWORD dwMax);
        
        void ( STDMETHODCALLTYPE *ClrCloseSemaphore )( 
            IExecutionEngine * This,
            /* [in] */ SEMAPHORE_COOKIE semaphore);
        
        DWORD ( STDMETHODCALLTYPE *ClrWaitForSemaphore )( 
            IExecutionEngine * This,
            /* [in] */ SEMAPHORE_COOKIE semaphore,
            /* [in] */ DWORD dwMilliseconds,
            /* [in] */ BOOL bAlertable);
        
        BOOL ( STDMETHODCALLTYPE *ClrReleaseSemaphore )( 
            IExecutionEngine * This,
            /* [in] */ SEMAPHORE_COOKIE semaphore,
            /* [in] */ LONG lReleaseCount,
            /* [in] */ LONG *lpPreviousCount);
        
        MUTEX_COOKIE ( STDMETHODCALLTYPE *ClrCreateMutex )( 
            IExecutionEngine * This,
            /* [in] */ LPSECURITY_ATTRIBUTES lpMutexAttributes,
            /* [in] */ BOOL bInitialOwner,
            /* [in] */ LPCTSTR lpName);
        
        DWORD ( STDMETHODCALLTYPE *ClrWaitForMutex )( 
            IExecutionEngine * This,
            /* [in] */ MUTEX_COOKIE mutex,
            /* [in] */ DWORD dwMilliseconds,
            /* [in] */ BOOL bAlertable);
        
        BOOL ( STDMETHODCALLTYPE *ClrReleaseMutex )( 
            IExecutionEngine * This,
            /* [in] */ MUTEX_COOKIE mutex);
        
        void ( STDMETHODCALLTYPE *ClrCloseMutex )( 
            IExecutionEngine * This,
            /* [in] */ MUTEX_COOKIE mutex);
        
        DWORD ( STDMETHODCALLTYPE *ClrSleepEx )( 
            IExecutionEngine * This,
            /* [in] */ DWORD dwMilliseconds,
            /* [in] */ BOOL bAlertable);
        
        BOOL ( STDMETHODCALLTYPE *ClrAllocationDisallowed )( 
            IExecutionEngine * This);
        
        void ( STDMETHODCALLTYPE *GetLastThrownObjectExceptionFromThread )( 
            IExecutionEngine * This,
            /* [out] */ void **ppvException);
        
        END_INTERFACE
    } IExecutionEngineVtbl;

    interface IExecutionEngine
    {
        CONST_VTBL struct IExecutionEngineVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IExecutionEngine_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IExecutionEngine_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IExecutionEngine_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IExecutionEngine_TLS_AssociateCallback(This,slot,callback)	\
    (This)->lpVtbl -> TLS_AssociateCallback(This,slot,callback)

#define IExecutionEngine_TLS_GetMasterSlotIndex(This)	\
    (This)->lpVtbl -> TLS_GetMasterSlotIndex(This)

#define IExecutionEngine_TLS_GetValue(This,slot)	\
    (This)->lpVtbl -> TLS_GetValue(This,slot)

#define IExecutionEngine_TLS_CheckValue(This,slot,pValue)	\
    (This)->lpVtbl -> TLS_CheckValue(This,slot,pValue)

#define IExecutionEngine_TLS_SetValue(This,slot,pData)	\
    (This)->lpVtbl -> TLS_SetValue(This,slot,pData)

#define IExecutionEngine_TLS_ThreadDetaching(This)	\
    (This)->lpVtbl -> TLS_ThreadDetaching(This)

#define IExecutionEngine_CreateLock(This,szTag,level,flags)	\
    (This)->lpVtbl -> CreateLock(This,szTag,level,flags)

#define IExecutionEngine_DestroyLock(This,lock)	\
    (This)->lpVtbl -> DestroyLock(This,lock)

#define IExecutionEngine_AcquireLock(This,lock)	\
    (This)->lpVtbl -> AcquireLock(This,lock)

#define IExecutionEngine_ReleaseLock(This,lock)	\
    (This)->lpVtbl -> ReleaseLock(This,lock)

#define IExecutionEngine_CreateAutoEvent(This,bInitialState)	\
    (This)->lpVtbl -> CreateAutoEvent(This,bInitialState)

#define IExecutionEngine_CreateManualEvent(This,bInitialState)	\
    (This)->lpVtbl -> CreateManualEvent(This,bInitialState)

#define IExecutionEngine_CloseEvent(This,event)	\
    (This)->lpVtbl -> CloseEvent(This,event)

#define IExecutionEngine_ClrSetEvent(This,event)	\
    (This)->lpVtbl -> ClrSetEvent(This,event)

#define IExecutionEngine_ClrResetEvent(This,event)	\
    (This)->lpVtbl -> ClrResetEvent(This,event)

#define IExecutionEngine_WaitForEvent(This,event,dwMilliseconds,bAlertable)	\
    (This)->lpVtbl -> WaitForEvent(This,event,dwMilliseconds,bAlertable)

#define IExecutionEngine_WaitForSingleObject(This,handle,dwMilliseconds)	\
    (This)->lpVtbl -> WaitForSingleObject(This,handle,dwMilliseconds)

#define IExecutionEngine_ClrCreateSemaphore(This,dwInitial,dwMax)	\
    (This)->lpVtbl -> ClrCreateSemaphore(This,dwInitial,dwMax)

#define IExecutionEngine_ClrCloseSemaphore(This,semaphore)	\
    (This)->lpVtbl -> ClrCloseSemaphore(This,semaphore)

#define IExecutionEngine_ClrWaitForSemaphore(This,semaphore,dwMilliseconds,bAlertable)	\
    (This)->lpVtbl -> ClrWaitForSemaphore(This,semaphore,dwMilliseconds,bAlertable)

#define IExecutionEngine_ClrReleaseSemaphore(This,semaphore,lReleaseCount,lpPreviousCount)	\
    (This)->lpVtbl -> ClrReleaseSemaphore(This,semaphore,lReleaseCount,lpPreviousCount)

#define IExecutionEngine_ClrCreateMutex(This,lpMutexAttributes,bInitialOwner,lpName)	\
    (This)->lpVtbl -> ClrCreateMutex(This,lpMutexAttributes,bInitialOwner,lpName)

#define IExecutionEngine_ClrWaitForMutex(This,mutex,dwMilliseconds,bAlertable)	\
    (This)->lpVtbl -> ClrWaitForMutex(This,mutex,dwMilliseconds,bAlertable)

#define IExecutionEngine_ClrReleaseMutex(This,mutex)	\
    (This)->lpVtbl -> ClrReleaseMutex(This,mutex)

#define IExecutionEngine_ClrCloseMutex(This,mutex)	\
    (This)->lpVtbl -> ClrCloseMutex(This,mutex)

#define IExecutionEngine_ClrSleepEx(This,dwMilliseconds,bAlertable)	\
    (This)->lpVtbl -> ClrSleepEx(This,dwMilliseconds,bAlertable)

#define IExecutionEngine_ClrAllocationDisallowed(This)	\
    (This)->lpVtbl -> ClrAllocationDisallowed(This)

#define IExecutionEngine_GetLastThrownObjectExceptionFromThread(This,ppvException)	\
    (This)->lpVtbl -> GetLastThrownObjectExceptionFromThread(This,ppvException)

#endif /* COBJMACROS */


#endif 	/* C style interface */



void STDMETHODCALLTYPE IExecutionEngine_TLS_AssociateCallback_Proxy( 
    IExecutionEngine * This,
    /* [in] */ DWORD slot,
    /* [in] */ PTLS_CALLBACK_FUNCTION callback);


void __RPC_STUB IExecutionEngine_TLS_AssociateCallback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


DWORD STDMETHODCALLTYPE IExecutionEngine_TLS_GetMasterSlotIndex_Proxy( 
    IExecutionEngine * This);


void __RPC_STUB IExecutionEngine_TLS_GetMasterSlotIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


PVOID STDMETHODCALLTYPE IExecutionEngine_TLS_GetValue_Proxy( 
    IExecutionEngine * This,
    /* [in] */ DWORD slot);


void __RPC_STUB IExecutionEngine_TLS_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IExecutionEngine_TLS_CheckValue_Proxy( 
    IExecutionEngine * This,
    /* [in] */ DWORD slot,
    /* [out] */ PVOID *pValue);


void __RPC_STUB IExecutionEngine_TLS_CheckValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE IExecutionEngine_TLS_SetValue_Proxy( 
    IExecutionEngine * This,
    /* [in] */ DWORD slot,
    /* [in] */ PVOID pData);


void __RPC_STUB IExecutionEngine_TLS_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE IExecutionEngine_TLS_ThreadDetaching_Proxy( 
    IExecutionEngine * This);


void __RPC_STUB IExecutionEngine_TLS_ThreadDetaching_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


CRITSEC_COOKIE STDMETHODCALLTYPE IExecutionEngine_CreateLock_Proxy( 
    IExecutionEngine * This,
    /* [in] */ LPCSTR szTag,
    /* [in] */ LPCSTR level,
    /* [in] */ CrstFlags flags);


void __RPC_STUB IExecutionEngine_CreateLock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE IExecutionEngine_DestroyLock_Proxy( 
    IExecutionEngine * This,
    /* [in] */ CRITSEC_COOKIE lock);


void __RPC_STUB IExecutionEngine_DestroyLock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE IExecutionEngine_AcquireLock_Proxy( 
    IExecutionEngine * This,
    /* [in] */ CRITSEC_COOKIE lock);


void __RPC_STUB IExecutionEngine_AcquireLock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE IExecutionEngine_ReleaseLock_Proxy( 
    IExecutionEngine * This,
    /* [in] */ CRITSEC_COOKIE lock);


void __RPC_STUB IExecutionEngine_ReleaseLock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


EVENT_COOKIE STDMETHODCALLTYPE IExecutionEngine_CreateAutoEvent_Proxy( 
    IExecutionEngine * This,
    /* [in] */ BOOL bInitialState);


void __RPC_STUB IExecutionEngine_CreateAutoEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


EVENT_COOKIE STDMETHODCALLTYPE IExecutionEngine_CreateManualEvent_Proxy( 
    IExecutionEngine * This,
    /* [in] */ BOOL bInitialState);


void __RPC_STUB IExecutionEngine_CreateManualEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE IExecutionEngine_CloseEvent_Proxy( 
    IExecutionEngine * This,
    /* [in] */ EVENT_COOKIE event);


void __RPC_STUB IExecutionEngine_CloseEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IExecutionEngine_ClrSetEvent_Proxy( 
    IExecutionEngine * This,
    /* [in] */ EVENT_COOKIE event);


void __RPC_STUB IExecutionEngine_ClrSetEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IExecutionEngine_ClrResetEvent_Proxy( 
    IExecutionEngine * This,
    /* [in] */ EVENT_COOKIE event);


void __RPC_STUB IExecutionEngine_ClrResetEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


DWORD STDMETHODCALLTYPE IExecutionEngine_WaitForEvent_Proxy( 
    IExecutionEngine * This,
    /* [in] */ EVENT_COOKIE event,
    /* [in] */ DWORD dwMilliseconds,
    /* [in] */ BOOL bAlertable);


void __RPC_STUB IExecutionEngine_WaitForEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


DWORD STDMETHODCALLTYPE IExecutionEngine_WaitForSingleObject_Proxy( 
    IExecutionEngine * This,
    /* [in] */ HANDLE handle,
    /* [in] */ DWORD dwMilliseconds);


void __RPC_STUB IExecutionEngine_WaitForSingleObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


SEMAPHORE_COOKIE STDMETHODCALLTYPE IExecutionEngine_ClrCreateSemaphore_Proxy( 
    IExecutionEngine * This,
    /* [in] */ DWORD dwInitial,
    /* [in] */ DWORD dwMax);


void __RPC_STUB IExecutionEngine_ClrCreateSemaphore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE IExecutionEngine_ClrCloseSemaphore_Proxy( 
    IExecutionEngine * This,
    /* [in] */ SEMAPHORE_COOKIE semaphore);


void __RPC_STUB IExecutionEngine_ClrCloseSemaphore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


DWORD STDMETHODCALLTYPE IExecutionEngine_ClrWaitForSemaphore_Proxy( 
    IExecutionEngine * This,
    /* [in] */ SEMAPHORE_COOKIE semaphore,
    /* [in] */ DWORD dwMilliseconds,
    /* [in] */ BOOL bAlertable);


void __RPC_STUB IExecutionEngine_ClrWaitForSemaphore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IExecutionEngine_ClrReleaseSemaphore_Proxy( 
    IExecutionEngine * This,
    /* [in] */ SEMAPHORE_COOKIE semaphore,
    /* [in] */ LONG lReleaseCount,
    /* [in] */ LONG *lpPreviousCount);


void __RPC_STUB IExecutionEngine_ClrReleaseSemaphore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


MUTEX_COOKIE STDMETHODCALLTYPE IExecutionEngine_ClrCreateMutex_Proxy( 
    IExecutionEngine * This,
    /* [in] */ LPSECURITY_ATTRIBUTES lpMutexAttributes,
    /* [in] */ BOOL bInitialOwner,
    /* [in] */ LPCTSTR lpName);


void __RPC_STUB IExecutionEngine_ClrCreateMutex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


DWORD STDMETHODCALLTYPE IExecutionEngine_ClrWaitForMutex_Proxy( 
    IExecutionEngine * This,
    /* [in] */ MUTEX_COOKIE mutex,
    /* [in] */ DWORD dwMilliseconds,
    /* [in] */ BOOL bAlertable);


void __RPC_STUB IExecutionEngine_ClrWaitForMutex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IExecutionEngine_ClrReleaseMutex_Proxy( 
    IExecutionEngine * This,
    /* [in] */ MUTEX_COOKIE mutex);


void __RPC_STUB IExecutionEngine_ClrReleaseMutex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE IExecutionEngine_ClrCloseMutex_Proxy( 
    IExecutionEngine * This,
    /* [in] */ MUTEX_COOKIE mutex);


void __RPC_STUB IExecutionEngine_ClrCloseMutex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


DWORD STDMETHODCALLTYPE IExecutionEngine_ClrSleepEx_Proxy( 
    IExecutionEngine * This,
    /* [in] */ DWORD dwMilliseconds,
    /* [in] */ BOOL bAlertable);


void __RPC_STUB IExecutionEngine_ClrSleepEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IExecutionEngine_ClrAllocationDisallowed_Proxy( 
    IExecutionEngine * This);


void __RPC_STUB IExecutionEngine_ClrAllocationDisallowed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE IExecutionEngine_GetLastThrownObjectExceptionFromThread_Proxy( 
    IExecutionEngine * This,
    /* [out] */ void **ppvException);


void __RPC_STUB IExecutionEngine_GetLastThrownObjectExceptionFromThread_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IExecutionEngine_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_clrinternal_0009 */
/* [local] */ 

#if !defined(_WINNT_) && !defined(_NTMMAPI_)
typedef void *PMEMORY_BASIC_INFORMATION;

#endif


extern RPC_IF_HANDLE __MIDL_itf_clrinternal_0009_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_clrinternal_0009_v0_0_s_ifspec;

#ifndef __IEEMemoryManager_INTERFACE_DEFINED__
#define __IEEMemoryManager_INTERFACE_DEFINED__

/* interface IEEMemoryManager */
/* [object][local][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IEEMemoryManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("17713B61-B59F-4e13-BAAF-91623DC8ADC0")
    IEEMemoryManager : public IUnknown
    {
    public:
        virtual LPVOID STDMETHODCALLTYPE ClrVirtualAlloc( 
            /* [in] */ LPVOID lpAddress,
            /* [in] */ SIZE_T dwSize,
            /* [in] */ DWORD flAllocationType,
            /* [in] */ DWORD flProtect) = 0;
        
        virtual BOOL STDMETHODCALLTYPE ClrVirtualFree( 
            /* [in] */ LPVOID lpAddress,
            /* [in] */ SIZE_T dwSize,
            /* [in] */ DWORD dwFreeType) = 0;
        
        virtual SIZE_T STDMETHODCALLTYPE ClrVirtualQuery( 
            /* [in] */ const void *lpAddress,
            /* [in] */ PMEMORY_BASIC_INFORMATION lpBuffer,
            /* [in] */ SIZE_T dwLength) = 0;
        
        virtual BOOL STDMETHODCALLTYPE ClrVirtualProtect( 
            /* [in] */ LPVOID lpAddress,
            /* [in] */ SIZE_T dwSize,
            /* [in] */ DWORD flNewProtect,
            /* [in] */ DWORD *lpflOldProtect) = 0;
        
        virtual HANDLE STDMETHODCALLTYPE ClrGetProcessHeap( void) = 0;
        
        virtual HANDLE STDMETHODCALLTYPE ClrHeapCreate( 
            /* [in] */ DWORD flOptions,
            /* [in] */ SIZE_T dwInitialSize,
            /* [in] */ SIZE_T dwMaximumSize) = 0;
        
        virtual BOOL STDMETHODCALLTYPE ClrHeapDestroy( 
            /* [in] */ HANDLE hHeap) = 0;
        
        virtual LPVOID STDMETHODCALLTYPE ClrHeapAlloc( 
            /* [in] */ HANDLE hHeap,
            /* [in] */ DWORD dwFlags,
            /* [in] */ SIZE_T dwBytes) = 0;
        
        virtual BOOL STDMETHODCALLTYPE ClrHeapFree( 
            /* [in] */ HANDLE hHeap,
            /* [in] */ DWORD dwFlags,
            /* [in] */ LPVOID lpMem) = 0;
        
        virtual BOOL STDMETHODCALLTYPE ClrHeapValidate( 
            /* [in] */ HANDLE hHeap,
            /* [in] */ DWORD dwFlags,
            /* [in] */ const void *lpMem) = 0;
        
        virtual HANDLE STDMETHODCALLTYPE ClrGetProcessExecutableHeap( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEEMemoryManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEEMemoryManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEEMemoryManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEEMemoryManager * This);
        
        LPVOID ( STDMETHODCALLTYPE *ClrVirtualAlloc )( 
            IEEMemoryManager * This,
            /* [in] */ LPVOID lpAddress,
            /* [in] */ SIZE_T dwSize,
            /* [in] */ DWORD flAllocationType,
            /* [in] */ DWORD flProtect);
        
        BOOL ( STDMETHODCALLTYPE *ClrVirtualFree )( 
            IEEMemoryManager * This,
            /* [in] */ LPVOID lpAddress,
            /* [in] */ SIZE_T dwSize,
            /* [in] */ DWORD dwFreeType);
        
        SIZE_T ( STDMETHODCALLTYPE *ClrVirtualQuery )( 
            IEEMemoryManager * This,
            /* [in] */ const void *lpAddress,
            /* [in] */ PMEMORY_BASIC_INFORMATION lpBuffer,
            /* [in] */ SIZE_T dwLength);
        
        BOOL ( STDMETHODCALLTYPE *ClrVirtualProtect )( 
            IEEMemoryManager * This,
            /* [in] */ LPVOID lpAddress,
            /* [in] */ SIZE_T dwSize,
            /* [in] */ DWORD flNewProtect,
            /* [in] */ DWORD *lpflOldProtect);
        
        HANDLE ( STDMETHODCALLTYPE *ClrGetProcessHeap )( 
            IEEMemoryManager * This);
        
        HANDLE ( STDMETHODCALLTYPE *ClrHeapCreate )( 
            IEEMemoryManager * This,
            /* [in] */ DWORD flOptions,
            /* [in] */ SIZE_T dwInitialSize,
            /* [in] */ SIZE_T dwMaximumSize);
        
        BOOL ( STDMETHODCALLTYPE *ClrHeapDestroy )( 
            IEEMemoryManager * This,
            /* [in] */ HANDLE hHeap);
        
        LPVOID ( STDMETHODCALLTYPE *ClrHeapAlloc )( 
            IEEMemoryManager * This,
            /* [in] */ HANDLE hHeap,
            /* [in] */ DWORD dwFlags,
            /* [in] */ SIZE_T dwBytes);
        
        BOOL ( STDMETHODCALLTYPE *ClrHeapFree )( 
            IEEMemoryManager * This,
            /* [in] */ HANDLE hHeap,
            /* [in] */ DWORD dwFlags,
            /* [in] */ LPVOID lpMem);
        
        BOOL ( STDMETHODCALLTYPE *ClrHeapValidate )( 
            IEEMemoryManager * This,
            /* [in] */ HANDLE hHeap,
            /* [in] */ DWORD dwFlags,
            /* [in] */ const void *lpMem);
        
        HANDLE ( STDMETHODCALLTYPE *ClrGetProcessExecutableHeap )( 
            IEEMemoryManager * This);
        
        END_INTERFACE
    } IEEMemoryManagerVtbl;

    interface IEEMemoryManager
    {
        CONST_VTBL struct IEEMemoryManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEEMemoryManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEEMemoryManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEEMemoryManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEEMemoryManager_ClrVirtualAlloc(This,lpAddress,dwSize,flAllocationType,flProtect)	\
    (This)->lpVtbl -> ClrVirtualAlloc(This,lpAddress,dwSize,flAllocationType,flProtect)

#define IEEMemoryManager_ClrVirtualFree(This,lpAddress,dwSize,dwFreeType)	\
    (This)->lpVtbl -> ClrVirtualFree(This,lpAddress,dwSize,dwFreeType)

#define IEEMemoryManager_ClrVirtualQuery(This,lpAddress,lpBuffer,dwLength)	\
    (This)->lpVtbl -> ClrVirtualQuery(This,lpAddress,lpBuffer,dwLength)

#define IEEMemoryManager_ClrVirtualProtect(This,lpAddress,dwSize,flNewProtect,lpflOldProtect)	\
    (This)->lpVtbl -> ClrVirtualProtect(This,lpAddress,dwSize,flNewProtect,lpflOldProtect)

#define IEEMemoryManager_ClrGetProcessHeap(This)	\
    (This)->lpVtbl -> ClrGetProcessHeap(This)

#define IEEMemoryManager_ClrHeapCreate(This,flOptions,dwInitialSize,dwMaximumSize)	\
    (This)->lpVtbl -> ClrHeapCreate(This,flOptions,dwInitialSize,dwMaximumSize)

#define IEEMemoryManager_ClrHeapDestroy(This,hHeap)	\
    (This)->lpVtbl -> ClrHeapDestroy(This,hHeap)

#define IEEMemoryManager_ClrHeapAlloc(This,hHeap,dwFlags,dwBytes)	\
    (This)->lpVtbl -> ClrHeapAlloc(This,hHeap,dwFlags,dwBytes)

#define IEEMemoryManager_ClrHeapFree(This,hHeap,dwFlags,lpMem)	\
    (This)->lpVtbl -> ClrHeapFree(This,hHeap,dwFlags,lpMem)

#define IEEMemoryManager_ClrHeapValidate(This,hHeap,dwFlags,lpMem)	\
    (This)->lpVtbl -> ClrHeapValidate(This,hHeap,dwFlags,lpMem)

#define IEEMemoryManager_ClrGetProcessExecutableHeap(This)	\
    (This)->lpVtbl -> ClrGetProcessExecutableHeap(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



LPVOID STDMETHODCALLTYPE IEEMemoryManager_ClrVirtualAlloc_Proxy( 
    IEEMemoryManager * This,
    /* [in] */ LPVOID lpAddress,
    /* [in] */ SIZE_T dwSize,
    /* [in] */ DWORD flAllocationType,
    /* [in] */ DWORD flProtect);


void __RPC_STUB IEEMemoryManager_ClrVirtualAlloc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IEEMemoryManager_ClrVirtualFree_Proxy( 
    IEEMemoryManager * This,
    /* [in] */ LPVOID lpAddress,
    /* [in] */ SIZE_T dwSize,
    /* [in] */ DWORD dwFreeType);


void __RPC_STUB IEEMemoryManager_ClrVirtualFree_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


SIZE_T STDMETHODCALLTYPE IEEMemoryManager_ClrVirtualQuery_Proxy( 
    IEEMemoryManager * This,
    /* [in] */ const void *lpAddress,
    /* [in] */ PMEMORY_BASIC_INFORMATION lpBuffer,
    /* [in] */ SIZE_T dwLength);


void __RPC_STUB IEEMemoryManager_ClrVirtualQuery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IEEMemoryManager_ClrVirtualProtect_Proxy( 
    IEEMemoryManager * This,
    /* [in] */ LPVOID lpAddress,
    /* [in] */ SIZE_T dwSize,
    /* [in] */ DWORD flNewProtect,
    /* [in] */ DWORD *lpflOldProtect);


void __RPC_STUB IEEMemoryManager_ClrVirtualProtect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HANDLE STDMETHODCALLTYPE IEEMemoryManager_ClrGetProcessHeap_Proxy( 
    IEEMemoryManager * This);


void __RPC_STUB IEEMemoryManager_ClrGetProcessHeap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HANDLE STDMETHODCALLTYPE IEEMemoryManager_ClrHeapCreate_Proxy( 
    IEEMemoryManager * This,
    /* [in] */ DWORD flOptions,
    /* [in] */ SIZE_T dwInitialSize,
    /* [in] */ SIZE_T dwMaximumSize);


void __RPC_STUB IEEMemoryManager_ClrHeapCreate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IEEMemoryManager_ClrHeapDestroy_Proxy( 
    IEEMemoryManager * This,
    /* [in] */ HANDLE hHeap);


void __RPC_STUB IEEMemoryManager_ClrHeapDestroy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


LPVOID STDMETHODCALLTYPE IEEMemoryManager_ClrHeapAlloc_Proxy( 
    IEEMemoryManager * This,
    /* [in] */ HANDLE hHeap,
    /* [in] */ DWORD dwFlags,
    /* [in] */ SIZE_T dwBytes);


void __RPC_STUB IEEMemoryManager_ClrHeapAlloc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IEEMemoryManager_ClrHeapFree_Proxy( 
    IEEMemoryManager * This,
    /* [in] */ HANDLE hHeap,
    /* [in] */ DWORD dwFlags,
    /* [in] */ LPVOID lpMem);


void __RPC_STUB IEEMemoryManager_ClrHeapFree_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IEEMemoryManager_ClrHeapValidate_Proxy( 
    IEEMemoryManager * This,
    /* [in] */ HANDLE hHeap,
    /* [in] */ DWORD dwFlags,
    /* [in] */ const void *lpMem);


void __RPC_STUB IEEMemoryManager_ClrHeapValidate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HANDLE STDMETHODCALLTYPE IEEMemoryManager_ClrGetProcessExecutableHeap_Proxy( 
    IEEMemoryManager * This);


void __RPC_STUB IEEMemoryManager_ClrGetProcessExecutableHeap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEEMemoryManager_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


