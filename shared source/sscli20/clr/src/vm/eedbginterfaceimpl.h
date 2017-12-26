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
/*
 *
 * COM+99 EE to Debugger Interface Implementation
 *
 */
#ifndef _eedbginterfaceimpl_h_
#define _eedbginterfaceimpl_h_

#ifdef DEBUGGING_SUPPORTED

#include "common.h"
#include "corpriv.h"
#include "hash.h"
#include "class.h"
#include "excep.h"
#include "threads.inl"
#include "field.h"
#include "eetwain.h"
#include "jitinterface.h"
#include "enc.h"
#include "stubmgr.h"

#include "eedbginterface.h"
#include "comsystem.h"
#include "debugdebugger.h"


#include "eeconfig.h"
#include "pefile.h"

class EEDbgInterfaceImpl : public EEDebugInterface
{
    VPTR_VTABLE_CLASS(EEDbgInterfaceImpl, EEDebugInterface);

public:

#ifndef DACCESS_COMPILE

    //
    // Setup and global data used by this interface.
    //
    static FORCEINLINE void Init(void)
    {
        g_pEEDbgInterfaceImpl = new EEDbgInterfaceImpl(); // new throws on failure
        }

    //
    // Cleanup any global data used by this interface.
    //
    static void Terminate(void);

#endif // #ifndef DACCESS_COMPILE

    Thread* GetThread(void);

    void SetEEThreadPtr(VOID* newPtr);

    StackWalkAction StackWalkFramesEx(Thread* pThread,
                                             PREGDISPLAY pRD,
                                             PSTACKWALKFRAMESCALLBACK pCallback,
                                             VOID* pData,
                                      unsigned int flags);

    Frame *GetFrame(CrawlFrame *pCF);

    bool InitRegDisplay(Thread* pThread,
                        const PREGDISPLAY pRD,
            const PCONTEXT pctx,
                        bool validContext);

    BOOL IsStringObject(Object* o);

    BOOL IsTypedReference(MethodTable* pMT);

    WCHAR* StringObjectGetBuffer(StringObject* so);

    DWORD StringObjectGetStringLength(StringObject* so);

    void* GetObjectFromHandle(OBJECTHANDLE handle);

    OBJECTHANDLE GetHandleFromObject(void *obj,
                              bool fStrongNewRef,
                              AppDomain *pAppDomain);

    void DbgDestroyHandle(OBJECTHANDLE oh,
                          bool fStrongNewRef);

    OBJECTHANDLE GetThreadException(Thread *pThread);

    bool IsThreadExceptionNull(Thread *pThread);

    void ClearThreadException(Thread *pThread);

    bool StartSuspendForDebug(AppDomain *pAppDomain,
                              BOOL fHoldingThreadStoreLock);

    bool SweepThreadsForDebug(bool forceSync);

    void ResumeFromDebug(AppDomain *pAppDomain);

    void MarkThreadForDebugSuspend(Thread* pRuntimeThread);

    void MarkThreadForDebugStepping(Thread* pRuntimeThread,
                                    bool onOff);

    void SetThreadFilterContext(Thread *thread,
                                CONTEXT *context);

    CONTEXT *GetThreadFilterContext(Thread *thread);

    DWORD GetThreadDebuggerWord(Thread *thread);

    void SetThreadDebuggerWord(Thread *thread,
                               DWORD dw);

    BOOL IsManagedNativeCode(const BYTE *address);

    MethodDesc *GetNativeCodeMethodDesc(const BYTE *address);

    BOOL IsInPrologOrEpilog(const BYTE *address,
                            size_t* prologSize);

    void DetermineIfOffsetsInFilterOrHandler(const BYTE *functionAddress,
                                                  DebugOffsetToHandlerInfo *pOffsetToHandlerInfo,
                                                  unsigned offsetToHandlerInfoLength);

    void GetMethodRegionInfo(const BYTE *pStart,
                             BYTE **pCold,
                             size_t *hotSize,
                             size_t *coldSize);


    size_t GetFunctionSize(MethodDesc *pFD);

    const BYTE* GetFunctionAddress(MethodDesc *pFD);

    void OnDebuggerTripThread(void);

    void DisablePreemptiveGC(void);

    void EnablePreemptiveGC(void);

    bool IsPreemptiveGCDisabled(void);

    DWORD MethodDescIsStatic(MethodDesc *pFD);

    Module *MethodDescGetModule(MethodDesc *pFD);

    COR_ILMETHOD* MethodDescGetILHeader(MethodDesc *pFD);

    ULONG MethodDescGetRVA(MethodDesc *pFD);

    MethodDesc *FindLoadedMethodRefOrDef(Module* pModule,
                                          mdToken memberRef);

    MethodDesc *LoadMethodDef(Module* pModule,
                              mdMethodDef methodDef,
                              DWORD numGenericArgs = 0,
                              TypeHandle *pGenericArgs = NULL,
                              TypeHandle *pOwnerTypeRes = NULL);

    TypeHandle FindLoadedClass(Module *pModule,
                             mdTypeDef classToken);

    TypeHandle FindLoadedInstantiation(Module *pModule,
                                       mdTypeDef typeDef,
                                       DWORD numGenericArgs,
                                       TypeHandle *pGenericArgs);

    TypeHandle FindLoadedFnptrType(TypeHandle *inst,
                                   DWORD ntypars);

    TypeHandle FindLoadedPointerOrByrefType(CorElementType et,
                                            TypeHandle elemtype);

    TypeHandle FindLoadedArrayType(CorElementType et,
                                   TypeHandle elemtype,
                                   unsigned rank);

    TypeHandle FindLoadedElementType(CorElementType et);

    TypeHandle LoadClass(Module *pModule,
                       mdTypeDef classToken);

    TypeHandle LoadInstantiation(Module *pModule,
                                 mdTypeDef typeDef,
                                 DWORD numGenericArgs,
                                 TypeHandle *pGenericArgs);

    TypeHandle LoadArrayType(CorElementType et,
                             TypeHandle elemtype,
                             unsigned rank);

    TypeHandle LoadPointerOrByrefType(CorElementType et,
                                      TypeHandle elemtype);

    TypeHandle LoadFnptrType(TypeHandle *inst,
                             DWORD ntypars);

    TypeHandle LoadElementType(CorElementType et);

    HRESULT GetMethodImplProps(Module *pModule,
                               mdToken tk,
                               DWORD *pRVA,
                               DWORD *pImplFlags);

    HRESULT GetParentToken(Module *pModule,
                           mdToken tk,
                           mdToken *pParentToken);

    void MarkDebuggerAttached(void);

    void MarkDebuggerUnattached(void);

    HRESULT IterateThreadsForAttach(BOOL *fEventSent,
                                    BOOL fAttaching);


    bool CrawlFrameIsGcSafe(CrawlFrame *pCF);

    bool IsStub(const BYTE *ip);

    bool DetectHandleILStubs(Thread *thread);

    bool TraceStub(const BYTE *ip,
                   TraceDestination *trace);

    bool FollowTrace(TraceDestination *trace);

    bool TraceFrame(Thread *thread,
                    Frame *frame,
                    BOOL fromPatch,
                    TraceDestination *trace,
                    REGDISPLAY *regs);

    bool TraceManager(Thread *thread,
                      StubManager *stubManager,
                      TraceDestination *trace,
                      CONTEXT *context,
                      BYTE **pRetAddr);

    void EnableTraceCall(Thread *thread);

    bool GetVirtualTraceCallTargets(TADDR** ppTargets, DWORD* pdwNumTargets);

    void DisableTraceCall(Thread *thread);

    void GetRuntimeOffsets(SIZE_T *pTLSIndex,
                           SIZE_T *pTLSIsSpecialIndex,
                           SIZE_T *pTLSCantStopIndex,
                           SIZE_T *pTLSIndexOfPredefs,
                           SIZE_T *pEEThreadStateOffset,
                           SIZE_T *pEEThreadStateNCOffset,
                           SIZE_T *pEEThreadPGCDisabledOffset,
                           DWORD  *pEEThreadPGCDisabledValue,
                           SIZE_T *pEEThreadDebuggerWordOffset,
                           SIZE_T *pEEThreadFrameOffset,
                           SIZE_T *pEEThreadMaxNeededSize,
                           DWORD  *pEEThreadSteppingStateMask,
                           DWORD  *pEEMaxFrameValue,
                           SIZE_T *pEEThreadDebuggerFilterContextOffset,
                           SIZE_T *pEEThreadCantStopOffset,
                           SIZE_T *pEEFrameNextOffset,
                           DWORD  *pEEIsManagedExceptionStateMask);

    void DebuggerModifyingLogSwitch (int iNewLevel,
                                     const WCHAR *pLogSwitchName);

    HRESULT SetIPFromSrcToDst(Thread *pThread,
                              IJitManager* pIJM,
                              METHODTOKEN MethodToken,
                              SLOT addrStart,
                              DWORD offFrom,
                              DWORD offTo,
                              bool fCanSetIPOnly,
                              PREGDISPLAY pReg,
                              PCONTEXT pCtx,
                              DWORD methodSize,
                              void *firstExceptionHandler,
                              void *pDji,
                              EHRangeTree *pEHRT);

    void SetDebugState(Thread *pThread,
                       CorDebugThreadState state);

    void SetAllDebugState(Thread *et,
                          CorDebugThreadState state);

    // This is pretty much copied from VM\COMSynchronizable's
    // INT32 __stdcall ThreadNative::GetThreadState, so propogate changes
    // to both functions
    CorDebugUserState GetPartialUserState( Thread *pThread );


    HRESULT FilterEnCBreakpointsByEH(DebuggerILToNativeMap   *m_sequenceMap,
                                     unsigned int             m_sequenceMapCount,
                                     COR_ILMETHOD_DECODER    *pMethodDecoderOld,
                                     CORDB_ADDRESS            addrOfCode,
                                     METHODTOKEN              methodToken,
                                     DWORD                    methodSize);

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    virtual unsigned GetSizeForCorElementType(CorElementType etyp);

#ifndef DACCESS_COMPILE
    virtual BOOL ObjIsInstanceOf(Object *pElement, TypeHandle toTypeHnd);
#endif
    
    virtual void ClearAllDebugInterfaceReferences(void);
    
#ifndef DACCESS_COMPILE
#ifdef _DEBUG
    virtual void ObjectRefFlush(Thread *pThread);
#endif
#endif
};

#endif // DEBUGGING_SUPPORTED

#endif // _eedbginterfaceimpl_h_
