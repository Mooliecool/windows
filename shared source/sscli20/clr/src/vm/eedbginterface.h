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
//
// COM+99 EE to Debugger Interface Header
//

#ifndef _eedbginterface_h_
#define _eedbginterface_h_

#include "common.h"
#include "corpriv.h"
#include "hash.h"
#include "class.h"
#include "excep.h"
#include "threads.h"
#include "field.h"
#include "stackwalk.h"


#include "cordebug.h"
#include "../debug/inc/common.h"

class MethodDesc;
class Frame;
//
// The purpose of this object is to provide EE funcationality back to
// the debugger. This represents the entire set of EE functions used
// by the debugger.
//
// We will make this interface smaller over time to minimize the link
// between the EE and the Debugger.
//
typedef BOOL (*HashMapEnumCallback)(HashMap* h,
                                    void* pData,
                                    ULONG value);

typedef enum AttachAppDomainEventsEnum
{
    SEND_ALL_EVENTS,
    ONLY_SEND_APP_DOMAIN_CREATE_EVENTS,
    DONT_SEND_CLASS_EVENTS,
    ONLY_SEND_CLASS_EVENTS
} AttachAppDomainEventsEnum;

typedef VPTR(class EEDebugInterface) PTR_EEDebugInterface;

struct DebugOffsetToHandlerInfo {
    SIZE_T offset;
    BOOL isInFilterOrHandler;
};

class EEDebugInterface
{
    VPTR_BASE_VTABLE_CLASS(EEDebugInterface);

public:

    //
    // Functions exported from the EE to the debugger.
    //

    virtual Thread* GetThread(void) = 0;

#ifndef DACCESS_COMPILE

    virtual void SetEEThreadPtr(VOID* newPtr) = 0;

    virtual StackWalkAction StackWalkFramesEx(Thread* pThread,
                                              PREGDISPLAY pRD,
                                              PSTACKWALKFRAMESCALLBACK pCallback,
                                              VOID* pData,
                                              unsigned int flags) = 0;

    virtual Frame *GetFrame(CrawlFrame*) = 0;

    virtual bool InitRegDisplay(Thread* pThread,
                                const PREGDISPLAY pRD,
                                const PCONTEXT pctx,
                                bool validContext) = 0;

    virtual BOOL IsStringObject(Object* o) = 0;

    virtual BOOL IsTypedReference(MethodTable* pMT) = 0;

    virtual WCHAR* StringObjectGetBuffer(StringObject* so) = 0;

    virtual DWORD StringObjectGetStringLength(StringObject* so) = 0;

    virtual void *GetObjectFromHandle(OBJECTHANDLE handle) = 0;

    virtual OBJECTHANDLE GetHandleFromObject(void *obj,
                                      bool fStrongNewRef,
                                      AppDomain *pAppDomain) = 0;

    virtual void DbgDestroyHandle( OBJECTHANDLE oh, bool fStrongNewRef ) = 0;

    virtual OBJECTHANDLE GetThreadException(Thread *pThread) = 0;

    virtual bool IsThreadExceptionNull(Thread *pThread) = 0;

    virtual void ClearThreadException(Thread *pThread) = 0;

    virtual bool StartSuspendForDebug(AppDomain *pAppDomain,
                                      BOOL fHoldingThreadStoreLock = FALSE) = 0;

    virtual void ResumeFromDebug(AppDomain *pAppDomain)= 0;

    virtual void MarkThreadForDebugSuspend(Thread* pRuntimeThread) = 0;

    virtual void MarkThreadForDebugStepping(Thread* pRuntimeThread,
                                            bool onOff) = 0;

    virtual void SetThreadFilterContext(Thread *thread,
                                        CONTEXT *context) = 0;

    virtual CONTEXT *GetThreadFilterContext(Thread *thread) = 0;

    virtual DWORD GetThreadDebuggerWord(Thread *thread) = 0;

    virtual void SetThreadDebuggerWord(Thread *thread,
                                       DWORD dw) = 0;

    virtual BOOL IsManagedNativeCode(const BYTE *address) = 0;

#endif // #ifndef DACCESS_COMPILE

    virtual MethodDesc *GetNativeCodeMethodDesc(const BYTE *address) = 0;

#ifndef DACCESS_COMPILE

    virtual BOOL IsInPrologOrEpilog(const BYTE *address,
                                    size_t* prologSize) = 0;

    virtual void DetermineIfOffsetsInFilterOrHandler(const BYTE *functionAddress,
                                                          DebugOffsetToHandlerInfo *pOffsetToHandlerInfo,
                                                          unsigned offsetToHandlerInfoLength) = 0;

#endif // #ifndef DACCESS_COMPILE

    virtual void GetMethodRegionInfo(const BYTE *pStart,
                                     BYTE **pCold,
                                     size_t *hotSize,
                                     size_t *coldSize) = 0;


    virtual size_t GetFunctionSize(MethodDesc *pFD) = 0;

    virtual const BYTE* GetFunctionAddress(MethodDesc *pFD) = 0;

#ifndef DACCESS_COMPILE


    //
    // New methods to support the new debugger.
    //

    virtual MethodDesc *FindLoadedMethodRefOrDef(Module* pModule,
                                                   mdMemberRef memberRef) = 0;

    virtual MethodDesc *LoadMethodDef(Module* pModule,
                                      mdMethodDef methodDef,
                                      DWORD numGenericArgs = 0,
                                      TypeHandle *pGenericArgs = NULL,
                                      TypeHandle *pOwnerType = NULL) = 0;

    // These will lookup a type, and if it's not loaded, return the null TypeHandle
    virtual TypeHandle FindLoadedClass(Module *pModule,
                                     mdTypeDef classToken) = 0;

    virtual TypeHandle FindLoadedElementType(CorElementType et) = 0;

    virtual TypeHandle FindLoadedInstantiation(Module *pModule,
                                               mdTypeDef typeDef,
                                               DWORD ntypars,
                                               TypeHandle *inst) = 0;

    virtual TypeHandle FindLoadedFnptrType(TypeHandle *inst,
                                           DWORD ntypars) = 0;

    virtual TypeHandle FindLoadedPointerOrByrefType(CorElementType et,
                                                    TypeHandle elemtype) = 0;

    virtual TypeHandle FindLoadedArrayType(CorElementType et,
                                           TypeHandle elemtype,
                                           unsigned rank) = 0;

    // These will lookup a type, and if it's not loaded, will load and run
    // the class init etc.
    virtual TypeHandle LoadClass(Module *pModule,
                               mdTypeDef classToken) = 0;

    virtual TypeHandle LoadElementType(CorElementType et) = 0;

    virtual TypeHandle LoadInstantiation(Module *pModule,
                                         mdTypeDef typeDef,
                                         DWORD ntypars,
                                         TypeHandle *inst) = 0;

    virtual TypeHandle LoadFnptrType(TypeHandle *inst,
                                     DWORD ntypars) = 0;

    virtual TypeHandle LoadPointerOrByrefType(CorElementType et,
                                              TypeHandle elemtype) = 0;

    virtual TypeHandle LoadArrayType(CorElementType et,
                                     TypeHandle elemtype,
                                     unsigned rank) = 0;

    virtual HRESULT GetMethodImplProps(Module *pModule,
                                       mdToken tk,
                                       DWORD *pRVA,
                                       DWORD *pImplFlags) = 0;

    virtual HRESULT GetParentToken(Module *pModule,
                                   mdToken tk,
                                   mdToken *pParentToken) = 0;

    virtual void OnDebuggerTripThread(void) = 0;

    virtual bool IsPreemptiveGCDisabled(void) = 0;

    virtual void DisablePreemptiveGC(void) = 0;

    virtual void EnablePreemptiveGC(void) = 0;

    virtual DWORD MethodDescIsStatic(MethodDesc *pFD) = 0;

#endif // #ifndef DACCESS_COMPILE

    virtual Module *MethodDescGetModule(MethodDesc *pFD) = 0;

#ifndef DACCESS_COMPILE

    virtual COR_ILMETHOD* MethodDescGetILHeader(MethodDesc *pFD) = 0;

    virtual ULONG MethodDescGetRVA(MethodDesc *pFD) = 0;

    virtual void MarkDebuggerAttached(void) = 0;

    virtual void MarkDebuggerUnattached(void) = 0;

    virtual HRESULT IterateThreadsForAttach(BOOL *fEventSent,
                                            BOOL fAttaching) = 0;

    virtual bool CrawlFrameIsGcSafe(CrawlFrame *pCF) = 0;

    virtual bool SweepThreadsForDebug(bool forceSync) = 0;

   virtual void GetRuntimeOffsets(SIZE_T *pTLSIndex,
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
                                  SIZE_T *pEEThreadCantStopMask,
                                  SIZE_T *pEEFrameNextOffset,
                                  DWORD  *pEEIsManagedExceptionStateMask) = 0;

    virtual bool IsStub(const BYTE *ip) = 0;

#endif // #ifndef DACCESS_COMPILE

    virtual bool DetectHandleILStubs(Thread *thread) = 0;

    virtual bool TraceStub(const BYTE *ip, TraceDestination *trace) = 0;

#ifndef DACCESS_COMPILE

    virtual bool FollowTrace(TraceDestination *trace) = 0;

    virtual bool TraceFrame(Thread *thread,
                            Frame *frame,
                            BOOL fromPatch,
                            TraceDestination *trace,
                            REGDISPLAY *regs) = 0;

    virtual bool TraceManager(Thread *thread,
                              StubManager *stubManager,
                              TraceDestination *trace,
                              CONTEXT *context,
                              BYTE **pRetAddr) = 0;

    virtual void EnableTraceCall(Thread *thread) = 0;
    virtual bool GetVirtualTraceCallTargets(TADDR** ppTargets, DWORD* pdwNumTargets) = 0;
    virtual void DisableTraceCall(Thread *thread) = 0;

#endif // #ifndef DACCESS_COMPILE

#ifndef DACCESS_COMPILE

    virtual void DebuggerModifyingLogSwitch (int iNewLevel,
                                             const WCHAR *pLogSwitchName) = 0;

#if defined(_X86_) || defined(_WIN64)
    virtual HRESULT SetIPFromSrcToDst(Thread *pThread,
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
                          EHRangeTree *pEHRT) = 0;
#endif // _X86_ || _WIN64

    virtual void SetDebugState(Thread *pThread,
                               CorDebugThreadState state) = 0;

    virtual void SetAllDebugState(Thread *et,
                                  CorDebugThreadState state) = 0;

    virtual CorDebugUserState GetPartialUserState( Thread *pThread ) = 0;


    virtual HRESULT FilterEnCBreakpointsByEH(DebuggerILToNativeMap *m_sequenceMap,
                                             unsigned int m_sequenceMapCount,
                                             COR_ILMETHOD_DECODER *pMethodDecoderOld,
                                             CORDB_ADDRESS addrOfCode,
                                             METHODTOKEN methodToken,
                                             DWORD methodSize) = 0;

#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags) = 0;
#endif

    virtual unsigned GetSizeForCorElementType(CorElementType etyp) = 0;

#ifndef DACCESS_COMPILE
    virtual BOOL ObjIsInstanceOf(Object *pElement, TypeHandle toTypeHnd) = 0;
#endif

    virtual void ClearAllDebugInterfaceReferences(void) = 0;

#ifndef DACCESS_COMPILE
#ifdef _DEBUG
    virtual void ObjectRefFlush(Thread *pThread) = 0;
#endif
#endif
};

#endif // _eedbginterface_h_
