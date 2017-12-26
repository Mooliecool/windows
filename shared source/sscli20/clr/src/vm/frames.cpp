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
 *    FRAMES.CPP:
 */

#include "common.h"
#include "log.h"
#include "frames.h"
#include "threads.h"
#include "object.h"
#include "method.hpp"
#include "class.h"
#include "excep.h"
#include "security.h"
#include "stublink.h"
#include "fieldmarshaler.h"
#include "objecthandle.h"
#include "siginfo.hpp"
#include "comstringbuffer.h"
#include "gc.h"
#include "dllimportcallback.h"
#include "comvariant.h"
#include "stackwalk.h"
#include "dbginterface.h"
#include "gms.h"
#include "eeconfig.h"
#include "remoting.h"
#include "ecall.h"
#include "marshaler.h"
#include "clsload.hpp"
#include "cgensys.h"
#include "virtualcallstub.h"
#include "mdaassistantsptr.h"


#if CHECK_APP_DOMAIN_LEAKS
#define CHECK_APP_DOMAIN    GC_CALL_CHECK_APP_DOMAIN
#else
#define CHECK_APP_DOMAIN    0
#endif

//-----------------------------------------------------------------------
#if _DEBUG
//-----------------------------------------------------------------------

#ifndef DACCESS_COMPILE

unsigned dbgStubCtr = 0;
unsigned dbgStubTrip = 0xFFFFFFFF;

void Frame::Log() {
    WRAPPER_CONTRACT;

    if (!LoggingOn(LF_STUBS, LL_INFO1000000))
        return;

    dbgStubCtr++;
    if (dbgStubCtr > dbgStubTrip) {
        dbgStubCtr++;      // basicly a nop to put a breakpoint on.
    }

    MethodDesc* method = GetFunction();
    if (GetVTablePtr() == UMThkCallFrame::GetUMThkCallFrameVPtr())
        method = ((UMThkCallFrame*) this)->GetUMEntryThunk()->GetMethod();

    STRESS_LOG3(LF_STUBS, LL_INFO1000000, "STUBS: In Stub with Frame %p assoc Method %pM FrameType = %pV\n", this, method, *((void**) this));

    char buff[64];
    char* frameType;
    if (GetVTablePtr() == PrestubMethodFrame::GetMethodFrameVPtr())
        frameType = "PreStub";
    else if (GetVTablePtr() == UMThkCallFrame::GetUMThkCallFrameVPtr())
        frameType = "UMThkCallFrame";
    else if (   GetVTablePtr() == NDirectMethodFrameStandalone::GetMethodFrameVPtr()
             || GetVTablePtr() == NDirectMethodFrameStandaloneCleanup::GetMethodFrameVPtr()
             || GetVTablePtr() == NDirectMethodFrameGeneric::GetMethodFrameVPtr()
#ifdef _X86_
             || GetVTablePtr() == NDirectMethodFrameSlim::GetMethodFrameVPtr()
#endif // _X86_             
             ) {
        // Right now, compiled COM interop stubs actually build NDirect frames
        // so have to test for this separately
        if (method->IsNDirect())
        {
            sprintf_s(buff, COUNTOF(buff), "PInvoke target" FMT_ADDR,
                      DBG_ADDR(((NDirectMethodDesc*) method)->GetNDirectTarget()));
            frameType = buff;
        }
        else
        {
            frameType = "Interop";
        }
    }
    else 
        frameType = "Unknown";

    if (method != 0)
        LOG((LF_STUBS, LL_INFO1000000, 
             "IN %s Stub Method = %s::%s SIG %s ESP of return" FMT_ADDR "\n",
             frameType, 
             method->m_pszDebugClassName,
             method->m_pszDebugMethodName,
             method->m_pszDebugMethodSignature,
             DBG_ADDR(GetReturnAddressPtr())));
    else 
        LOG((LF_STUBS, LL_INFO1000000, 
             "IN %s Stub Method UNKNOWN ESP of return" FMT_ADDR "\n", 
             frameType, 
             DBG_ADDR(GetReturnAddressPtr()) ));

    _ASSERTE(GetThread()->PreemptiveGCDisabled());
}

//-----------------------------------------------------------------------
// This function is used to log transitions in either direction 
// between unmanaged code and CLR/managed code.
// This is typically done in a stub that sets up a Frame, which is
// passed as an argument to this function.

void __stdcall Frame::LogTransition(Frame* frame)
{

    CONTRACTL {
        DEBUG_ONLY;
        NOTHROW;
        ENTRY_POINT;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    BEGIN_ENTRYPOINT_VOIDRET;

#ifdef _X86_
    // On x86, StubLinkerCPU::EmitMethodStubProlog calls Frame::LogTransition
    // but the caller of EmitMethodStubProlog sets the GSCookie later on.
    // So the cookie is not initialized by the point we get here.
#else
    _ASSERTE(*frame->GetGSCookiePtr() == GetProcessGSCookie());
#endif

    if (Frame::ShouldLogTransitions())
        frame->Log();

    END_ENTRYPOINT_VOIDRET;
} // void Frame::Log()

#endif // #ifndef DACCESS_COMPILE

//-----------------------------------------------------------------------
#endif




//-----------------------------------------------------------------------
// Count of the number of frame types
const size_t FRAME_TYPES_COUNT = 
#define FRAME_TYPE_NAME(frameType) +1
#include "frames.h"
;

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Implementation of the global table of names.  On the DAC side, just the global pointer.
//  On the runtime side, the array of names.
#if !defined(DACCESS_COMPILE)
    #define FRAME_TYPE_NAME(x) {x::GetFrameVtable(), #x} ,
    static FrameTypeName FrameTypeNameTable[] = {
    #include "frames.h"
    };
#endif // !defined(DACCESS_COMPILE)


SPTR_IMPL(FrameTypeName, Frame, s_pFrameTypeNames)
#if !defined(DACCESS_COMPILE)
    = FrameTypeNameTable
#endif // !defined(DACCESS_COMPILE)
    ;  

/* static */
PTR_CSTR Frame::GetFrameTypeName(TADDR vtbl)
{
    for (size_t i=0; i<FRAME_TYPES_COUNT; ++i)
    {
        if (vtbl == s_pFrameTypeNames[(int)i].vtbl)
        {
            return s_pFrameTypeNames[(int)i].name;
        }
    }

    return NULL;
} // char* Frame::FrameTypeName()


#if defined (_DEBUG_IMPL)   // _DEBUG and !DAC
//-----------------------------------------------------------------------


void Frame::LogFrame(
    int         LF,                     // Log facility for this call.
    int         LL)                     // Log Level for this call.
{
    char        buf[32];
    char        *pFrameType;
    pFrameType = GetFrameTypeName();
    
    if (pFrameType == NULL)
    {
        _ASSERTE(!"New Frame type needs to be added to FrameTypeName()");
        // Pointer is up to 17chars + vtbl@ = 22 chars
        sprintf_s(buf, COUNTOF(buf), "vtbl@%p", GetVTablePtr());
        pFrameType = buf;
    }

    LOG((LF, LL, "FRAME: addr:%p, next:%p, type:%s\n",
         this, m_Next, pFrameType));
} // void Frame::LogFrame()

void Frame::LogFrameChain(
    int         LF,                     // Log facility for this call.
    int         LL)                     // Log Level for this call.
{
    if (!LoggingOn(LF, LL))
        return;
    
    Frame *pFrame = this;
    while (pFrame != FRAME_TOP)
    {
        pFrame->LogFrame(LF, LL);
        pFrame = pFrame->m_Next;
    }
} // void Frame::LogFrameChain()

//-----------------------------------------------------------------------
#endif // _DEBUG_IMPL
//-----------------------------------------------------------------------

#ifndef DACCESS_COMPILE

// This contains the vtables of all the Frame types.
static PtrHashMap s_frameVTables;

void     CheckMaxGSOffset()
{
#ifdef _DEBUG

    INT32 maxGSOffset = 0;
    const char * pName = NULL;

    for (PtrHashMap::PtrIterator i = s_frameVTables.begin(); !i.end(); ++i)
    {
        TADDR vtab = (TADDR) i.GetValue();
        Frame * pFrame = (Frame *)&vtab;
        INT32 offs = pFrame->GetOffsOfGSCookie();
        if (offs < maxGSOffset)
        {
            maxGSOffset = offs;
            pName = Frame::GetFrameTypeName(vtab);
        }
    }


#if defined(_X86_)
    const INT32 MAX_GS_OFFSET = -70;
#endif

    _ASSERTE(MAX_GS_OFFSET <= maxGSOffset);
    LOG((LF_STUBS, LL_EVERYTHING, "%s has largest NegInfo of size %d bytes\n", pName, -maxGSOffset));

#endif // _DEBUG
}

// static
void Frame::Init()
{
    // create a table big enough for all the frame types, not in asynchronous mode, and with no lock owner
    s_frameVTables.Init(2 * FRAME_TYPES_COUNT, FALSE, &g_lockTrustMeIAmThreadSafe);

#define FRAME_TYPE_NAME(frameType)                          \
    s_frameVTables.InsertValue(frameType::GetFrameVtable(), \
                               (LPVOID) frameType::GetFrameVtable());
#include "frames.h"

    CheckMaxGSOffset();

} // void Frame::Init()

#endif // DACCESS_COMPILE

// Returns NULL if the Frame is already corrupted
// Else returns the location of the expected GSCookie
// Note that Frame::GetGSCookiePtr is a virtual method, and so cannot
// be used without first checking if the vtable is corrupt.

// static
PTR_GSCookie Frame::SafeGetGSCookiePtr(Frame * pFrame)
{
    WRAPPER_CONTRACT;

    _ASSERTE(pFrame != FRAME_TOP);

    if (pFrame == NULL || pFrame == FRAME_TOP)
        return NULL;
    
#ifndef DACCESS_COMPILE
    TADDR vptr = pFrame->GetVTablePtr();

    if (s_frameVTables.LookupValue(vptr, (LPVOID) vptr) == (LPVOID) INVALIDENTRY)
        return NULL;   
#endif

    PTR_GSCookie pGSCookie = pFrame->GetGSCookiePtr();
    return pGSCookie;
}

//-----------------------------------------------------------------------
#ifndef DACCESS_COMPILE
//-----------------------------------------------------------------------
// Link and Unlink this frame.
//-----------------------------------------------------------------------

VOID Frame::Push()
{
    WRAPPER_CONTRACT;

    Push(GetThread());
}

VOID Frame::Push(Thread *pThread)
{
    WRAPPER_CONTRACT;

    _ASSERTE(*GetGSCookiePtr() == GetProcessGSCookie());
    
    m_Next = pThread->GetFrame();

    // PAGE_SIZE is used to relax the assert for cases where two Frames are
    // declared in the same source function. We cannot predict the order
    // in which the C compiler will lay them out in the stack frame.
    // So PAGE_SIZE is a guess of the maximum stack frame size of any method
    // with multiple Frames in mscorwks.dll
    _ASSERTE(((m_Next == FRAME_TOP) ||
              (PBYTE(m_Next) + (2 * PAGE_SIZE)) > PBYTE(this)) &&
             "Pushing a frame out of order ?");
    
    _ASSERTE(// If AssertOnFailFast is set, the test expects to do stack overrun 
             // corruptions. In that case, the Frame chain may be corrupted,
             // and the rest of the assert is not valid.
             // Note that the corrupted Frame chain will be detected 
             // during stack-walking.
             !g_pConfig->fAssertOnFailFast() ||
             (m_Next == FRAME_TOP) ||
             (*m_Next->GetGSCookiePtr() == GetProcessGSCookie()));
    
    pThread->SetFrame(this);
}

VOID Frame::Pop()
{
    WRAPPER_CONTRACT;

    Pop(GetThread());
}

VOID Frame::Pop(Thread *pThread)
{
    WRAPPER_CONTRACT;

    _ASSERTE(pThread->GetFrame() == this && "Popping a frame out of order ?");
    _ASSERTE(*GetGSCookiePtr() == GetProcessGSCookie());
    _ASSERTE(// If AssertOnFailFast is set, the test expects to do stack overrun 
             // corruptions. In that case, the Frame chain may be corrupted,
             // and the rest of the assert is not valid.
             // Note that the corrupted Frame chain will be detected 
             // during stack-walking.
             !g_pConfig->fAssertOnFailFast() ||
             (m_Next == FRAME_TOP) ||
             (*m_Next->GetGSCookiePtr() == GetProcessGSCookie()));

    pThread->SetFrame(m_Next);
}

//-----------------------------------------------------------------------
#endif // #ifndef DACCESS_COMPILE
//---------------------------------------------------------------
// Get the extra param for shared generic code.
//---------------------------------------------------------------
void *FramedMethodFrame::GetParamTypeArg()
{
    WRAPPER_CONTRACT;

    // This gets called while creating stack traces during exception handling.
    // Using the ArgIterator constructor calls ArgIterator::Init which calls GetInitialOfsAdjust
    // which calls SizeOfActualFixedArgStack, which thinks it may load value types.
    // However all these will have previously been loaded.
    // 
    // I'm not entirely convinced this is the best places to put this: CrawlFrame::GetExactGenericArgsToken
    // may be another option.
    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

    MethodDesc *pFunction = GetFunction();
    _ASSERTE (pFunction->RequiresInstArg());
    // For some reason GetParamTypeArgOffset must be called after iterating 
    // through all args.
    MetaSig msig(pFunction);
    ArgIterator argit (this, &msig);
    
    UINT32 NumArguments = msig.NumFixedArgs();
    
    for (UINT32 i = 0;i<NumArguments;i++)
    {
        argit.GetNextArgAddr();
        msig.NextArg();
    }
    
    INT regNum;
    INT offs = argit.GetParamTypeArgOffset(&regNum);
    return *((void**)(offs + (LPBYTE)this));
}

TADDR FramedMethodFrame::GetAddrOfThis()
{
    WRAPPER_CONTRACT;
    MetaSig* pSig = NULL;
#if THISPTR_LOCATION > 0
    // some platforms needs to know if the method has a retbuff
    // arg or not to find where the this pointer is.
    MetaSig msig(GetFunction());
    pSig = &msig;
#endif
    return ArgIterator::GetThisOffset(pSig) +
        PTR_HOST_TO_TADDR(this);
}


#ifndef DACCESS_COMPILE

//-----------------------------------------------------------------------
// A rather specialized routine for the exclusive use of the PreStub.
//-----------------------------------------------------------------------
VOID PrestubMethodFrame::Push()
{
    WRAPPER_CONTRACT;

    // Initializes the frame's VPTR. This assumes C++ puts the vptr
    // at offset 0 for a class not using MI, but this is no different
    // than the assumption that COM Classic makes.
    *((TADDR*)this) = GetMethodFrameVPtr();
    *GetGSCookiePtr() = GetProcessGSCookie();

    // Link frame into the chain.
    Frame::Push();
}

#endif // #ifndef DACCESS_COMPILE

BOOL PrestubMethodFrame::TraceFrame(Thread *thread, BOOL fromPatch,
                                    TraceDestination *trace, REGDISPLAY *regs)
{
    WRAPPER_CONTRACT;

    //
    // We want to set a frame patch, unless we're already at the
    // frame patch, in which case we'll trace addrof_code which 
    // should be set by now.
    //

    if (fromPatch)
    {
        trace->InitForStub((TADDR) GetFunction()->GetSafeAddrofCode());
    }
    else
    {
        trace->InitForStub((TADDR) ThePreStub()->GetEntryPoint());
    }

    LOG((LF_CORDB, LL_INFO10000,
         "PrestubMethodFrame::TraceFrame: ip=" FMT_ADDR "\n", DBG_ADDR(trace->GetAddress()) ));
    
    return TRUE;
}


BOOL SecurityFrame::TraceFrame(Thread *thread, BOOL fromPatch,
                               TraceDestination *trace, REGDISPLAY *regs)
{
    WRAPPER_CONTRACT;

    //
    // We should only be called when we're in security code (i.e., in
    // DoDeclarativeActions. We're also claiming that the security
    // stub will only do work _before_ calling the true stub and not
    // after, so we know the wrapped stub hasn't been called yet and
    // we'll be able to trace to it. If this were to get called after
    // the wrapped stub had been called, then we're try to trace to it
    // again and never hit the new patch.
    //
    _ASSERTE(!fromPatch);

    //
    // We're assuming that the security frame is a) the only interceptor
    // or b) at least the first one. This is always true for V1.
    //
    MethodDesc *pMD = GetFunction();
    Stub *stub = pMD->GetStub();

    //
    // This had better be an intercept stub, since that what we wanted!
    //
    PREFIX_ASSUME(stub != NULL);
    _ASSERTE(stub->IsIntercept());
    while (stub->IsIntercept())
    {
        //
        // Grab the wrapped stub.
        //
        InterceptStub *is = (InterceptStub*)stub;
        if (*is->GetInterceptedStub() == NULL)
        {
            trace->InitForStub(*is->GetRealAddr());
            return TRUE;
        }

        stub = *is->GetInterceptedStub();
    }

    //
    // The wrapped sub better not be another interceptor. (See the
    // comment above.)
    //
    _ASSERTE(!stub->IsIntercept());
    
    LOG((LF_CORDB, LL_INFO10000,
         "SecurityFrame::TraceFrame: intercepted "
         "stub=" FMT_ADDR ", ep=" FMT_ADDR "\n",
         DBG_ADDR(stub), DBG_ADDR(stub->GetEntryPoint()) ));

    trace->InitForStub((TADDR) stub->GetEntryPoint());

    
    return TRUE;
}

Frame::Interception PrestubMethodFrame::GetInterception()
{
    LEAF_CONTRACT;

    //
    // The only direct kind of interception done by the prestub 
    // is class initialization.
    //

    return INTERCEPTION_CLASS_INIT;
}

Frame::Interception InterceptorFrame::GetInterception()
{
    WRAPPER_CONTRACT;

    // The SecurityDesc gets set just before calling the intercepted target
    // We may have turned on preemptive-GC for SendEvent(). So cast away the OBJECTREF

    bool isNull = (NULL == *(size_t*)GetAddrOfSecurityDesc());

    return (isNull ? INTERCEPTION_SECURITY : INTERCEPTION_NONE);
}


#ifndef DACCESS_COMPILE





//-----------------------------------------------------------------------
// GCFrames
//-----------------------------------------------------------------------


//--------------------------------------------------------------------
// This constructor pushes a new GCFrame on the frame chain.
//--------------------------------------------------------------------
GCFrame::GCFrame(OBJECTREF *pObjRefs, UINT numObjRefs, BOOL maybeInterior)
{
    WRAPPER_CONTRACT;

#ifdef THREAD_DELAY
    if (g_pConfig !=NULL && g_pConfig->m_pThreadDelay != NULL) 
    {
        if (g_pConfig->m_pThreadDelay->SpikeOn ())
            g_pConfig->m_pThreadDelay->SpikeDelay ();
        if (g_pConfig->m_pThreadDelay->DelayOn ())
            g_pConfig->m_pThreadDelay->ShortDelay ();
    }
#endif //THREAD_DELAY
		
    Init(GetThread(), pObjRefs, numObjRefs, maybeInterior);
}

void GCFrame::Init(Thread *pThread, OBJECTREF *pObjRefs, UINT numObjRefs, BOOL maybeInterior)
{
    WRAPPER_CONTRACT;

#ifdef USE_CHECKED_OBJECTREFS
    if (!maybeInterior) {
        UINT i;
        for(i = 0; i < numObjRefs; i++)
            Thread::ObjectRefProtected(&pObjRefs[i]);
        
        for (i = 0; i < numObjRefs; i++) {
            pObjRefs[i]->Validate();
        }
    }


#endif

    m_pObjRefs      = pObjRefs;
    m_numObjRefs    = numObjRefs;
    m_pCurThread    = pThread;
    m_MaybeInterior = maybeInterior;

    Frame::Push(m_pCurThread);
}


//
// GCFrame Object Scanning
//
// This handles scanning/promotion of GC objects that were
// protected by the programmer explicitly protecting it in a GC Frame
// via the GCPROTECTBEGIN / GCPROTECTEND facility...
//

void GCFrame::GcScanRoots(promote_func *fn, ScanContext* sc)
{
    WRAPPER_CONTRACT;

    Object **pRefs;

    pRefs = (Object**) m_pObjRefs;

    for (UINT i = 0;i < m_numObjRefs; i++)  {

        LOG((LF_GC, INFO3, "GC Protection Frame Promoting" FMT_ADDR "to",
             DBG_ADDR(OBJECTREF_TO_UNCHECKED_OBJECTREF(m_pObjRefs[i])) ));
        if (m_MaybeInterior)
            PromoteCarefully(fn, pRefs[i], sc, GC_CALL_INTERIOR|CHECK_APP_DOMAIN);
        else
            (*fn)(pRefs[i], sc, 0);
        LOG((LF_GC, INFO3, FMT_ADDR "\n", DBG_ADDR(OBJECTREF_TO_UNCHECKED_OBJECTREF(m_pObjRefs[i])) ));
    }
}


//--------------------------------------------------------------------
// Pops the GCFrame and cancels the GC protection.
//--------------------------------------------------------------------
VOID GCFrame::Pop()
{
    WRAPPER_CONTRACT;

    Frame::Pop(m_pCurThread);
#ifdef _DEBUG
    m_pCurThread->EnableStressHeap();
    for(UINT i = 0; i < m_numObjRefs; i++)
        Thread::ObjectRefNew(&m_pObjRefs[i]);       // Unprotect them
#endif
}

CustomGCFrame::CustomGCFrame(GCSCANFUNCTION scanFunc, void *context)
{
    m_pScanFunction = scanFunc;
    m_pContext = context;

    Frame::Push();
}

VOID CustomGCFrame::Pop()
{
    Frame::Pop();
}

#ifdef _DEBUG

struct IsProtectedByGCFrameStruct
{
    OBJECTREF       *ppObjectRef;
    UINT             count;
};

static StackWalkAction IsProtectedByGCFrameStackWalkFramesCallback(
    CrawlFrame      *pCF,
    VOID            *pData
)
{
    WRAPPER_CONTRACT;

    IsProtectedByGCFrameStruct *pd = (IsProtectedByGCFrameStruct*)pData;
    Frame *pFrame = pCF->GetFrame();
    if (pFrame) {
        if (pFrame->Protects(pd->ppObjectRef)) {
            pd->count++;
        }
    }
    return SWA_CONTINUE;
}

BOOL IsProtectedByGCFrame(OBJECTREF *ppObjectRef)
{
    WRAPPER_CONTRACT;

    // Just report TRUE if GCStress is not on.  This satisfies the asserts that use this
    // code without the cost of actually determining it.
    if (g_pConfig->GetGCStressLevel() == 0)
        return TRUE;

    if (!pThrowableAvailable(ppObjectRef)) {
        return TRUE;
    }

    CONTRACT_VIOLATION(ThrowsViolation);
    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE ();
    IsProtectedByGCFrameStruct d = {ppObjectRef, 0};
    GetThread()->StackWalkFrames(IsProtectedByGCFrameStackWalkFramesCallback, &d);
    if (d.count > 1) {
        _ASSERTE(!"Multiple GCFrames protecting the same pointer. This will cause GC corruption!");
    }
    return d.count != 0;
}
#endif

void ProtectByRefsFrame::GcScanRoots(promote_func *fn, ScanContext *sc)
{
    WRAPPER_CONTRACT;

    ByRefInfo *pByRefInfos = m_brInfo;
    while (pByRefInfos)
    {
        if (!CorIsPrimitiveType(pByRefInfos->typ))
        {
            if (pByRefInfos->typeHandle.IsValueType())
            {
                ProtectValueClassFrame::PromoteValueClassEmbeddedObjects(fn, sc, pByRefInfos->typeHandle, 
                                                 pByRefInfos->data);
            }
            else
            {
                Object *pObject = *((Object **)&pByRefInfos->data);

                LOG((LF_GC, INFO3, "ProtectByRefs Frame Promoting" FMT_ADDR "to ", DBG_ADDR(pObject) ));

                (*fn)(pObject, sc, CHECK_APP_DOMAIN);

                *((Object **)&pByRefInfos->data) = pObject;

                LOG((LF_GC, INFO3, FMT_ADDR "\n", DBG_ADDR(pObject) ));
            }
        }
        pByRefInfos = pByRefInfos->pNext;
    }
}


ProtectByRefsFrame::ProtectByRefsFrame(Thread *pThread, ByRefInfo *brInfo) : 
    m_brInfo(brInfo),  m_pThread(pThread)
{
    WRAPPER_CONTRACT;

    m_Next = m_pThread->GetFrame();
    m_pThread->SetFrame(this);
}

void ProtectByRefsFrame::Pop()
{
    WRAPPER_CONTRACT;

    Frame::Pop(m_pThread);
}


void ProtectValueClassFrame::GcScanRoots(promote_func *fn, ScanContext *sc)
{
    WRAPPER_CONTRACT;

    ValueClassInfo *pVCInfo = m_pVCInfo;
    while (pVCInfo != NULL)
    {
        if (!CorIsPrimitiveType(pVCInfo->typ))
        {
            _ASSERTE(pVCInfo->typeHandle.IsValueType());
            PromoteValueClassEmbeddedObjects(
                fn, 
                sc, 
                pVCInfo->typeHandle, 
                pVCInfo->pData);
        }
        pVCInfo = pVCInfo->pNext;
    }
}


ProtectValueClassFrame::ProtectValueClassFrame(Thread *pThread, ValueClassInfo *pVCInfo) : 
    m_pVCInfo(pVCInfo),  m_pThread(pThread)
{
    WRAPPER_CONTRACT;

    m_Next = m_pThread->GetFrame();
    m_pThread->SetFrame(this);
}

void ProtectValueClassFrame::Pop()
{
    WRAPPER_CONTRACT;

    Frame::Pop(m_pThread);
}

void ProtectValueClassFrame::PromoteValueClassEmbeddedObjects(promote_func *fn, ScanContext *sc, 
                                                          TypeHandle ty, PVOID pvObject)
{
    WRAPPER_CONTRACT;

    // This type approximation is OK since we are only looking at field information for the purposes of
    // GC, and all compatible instantiations share the same GC characteristics
    ApproxFieldDescIterator fdIterator(ty.GetMethodTable(), ApproxFieldDescIterator::INSTANCE_FIELDS);
    FieldDesc* pFD;

    while ((pFD = fdIterator.Next()) != NULL)
    {
        if (!CorIsPrimitiveType(pFD->GetFieldType()))
        {
            if (pFD->IsByValue())
            {
                // recurse
                PromoteValueClassEmbeddedObjects(fn, sc, pFD->LookupApproxFieldTypeHandle().GetMethodTable(),
                                                pFD->GetAddress(pvObject));
            }
            else if (pFD->IsObjRef ())
            {
                fn(*((Object **) pFD->GetAddress(pvObject)), sc, 
                   CHECK_APP_DOMAIN);
            }
        }
    }
}

//
// Promote Caller Stack
//
//

void FramedMethodFrame::PromoteCallerStackWorker(promote_func* fn, ScanContext* sc, BOOL fPinArgs)
{
    WRAPPER_CONTRACT;

    // I believe this is the contract:
    //CONTRACTL
    //{
    //    INSTANCE_CHECK;
    //    NOTHROW;
    //    GC_NOTRIGGER;
    //    FORBID_FAULT;
    //    MODE_ANY;
    //}
    //CONTRACTL_END

    PCCOR_SIGNATURE pCallSig;
    MethodDesc   *pFunction;

    LOG((LF_GC, INFO3, "    Promoting method caller Arguments\n" ));

    // We're going to have to look at the signature to determine
    // which arguments a are pointers....First we need the function
    pFunction = GetFunction();
    if (! pFunction)
        return;

    // Now get the signature...
    pCallSig = pFunction->GetSig();
    if (! pCallSig)
        return;

    //If not "vararg" calling convention, assume "default" calling convention
    if (MetaSig::GetCallingConvention(pFunction->GetModule(),pCallSig) != IMAGE_CEE_CS_CALLCONV_VARARG)
    {
        MetaSig msig(pFunction);            
        ArgIterator argit (this, &msig);
        PromoteCallerStackHelper (fn, sc, fPinArgs, &argit, &msig);
    }
    else
    {   
        VASigCookie* varArgSig = GetVASigCookie();

        //Note: no instantiations needed for varargs 
        MetaSig msig (varArgSig->mdVASig, 
                      SigParser::LengthOfSig(varArgSig->mdVASig),
                      varArgSig->pModule, 
                      NULL, 
                      NULL);
        ArgIterator argit (this, &msig);
        PromoteCallerStackHelper (fn, sc, fPinArgs, &argit, &msig);
    }
}

void FramedMethodFrame::PromoteCallerStackHelper(promote_func* fn, 
                                                 ScanContext* sc, BOOL fPinArgs,
                                                 ArgIterator *pargit, MetaSig *pmsig)
{
    WRAPPER_CONTRACT;
    // I believe this is the contract:
    //CONTRACTL
    //{
    //    INSTANCE_CHECK;
    //    NOTHROW;
    //    GC_NOTRIGGER;
    //    FORBID_FAULT;
    //    MODE_ANY;
    //}
    //CONTRACTL_END


    MethodDesc      *pFunction;
    DWORD           GcFlags;

    pFunction = GetFunction();

    // NOTE: pFunction->IsStatic may not return the same data as the ArgIterator or MetaSig.
    //   Look into removing the dependency on pFunction for this information.
    // promote 'this' for non-static methods
    if (!pFunction->IsStatic())
    {
        BOOL interior = pFunction->GetMethodTable()->IsValueType();

        LPVOID* pThis = pargit->GetThisAddr();
        LOG((LF_GC, INFO3, 
             "    'this' Argument at " FMT_ADDR "promoted from" FMT_ADDR "\n", 
             DBG_ADDR(pThis), DBG_ADDR(*pThis) ));

        if (interior)
            PromoteCarefully(fn, *(Object **)pThis, sc, GC_CALL_INTERIOR|CHECK_APP_DOMAIN);
        else
            (fn)( *(Object **)pThis, sc, CHECK_APP_DOMAIN);
    }

    if (pmsig->HasRetBuffArg())
    {
        LPVOID* pRetBuffArg = pargit->GetRetBuffArgAddr();
        GcFlags = GC_CALL_INTERIOR;
        if (fPinArgs)
        {
            GcFlags |= GC_CALL_PINNED;
            LOG((LF_GC, INFO3, "    ret buf Argument pinned at " FMT_ADDR "\n", DBG_ADDR(*pRetBuffArg) ));
        }
        LOG((LF_GC, INFO3, "    ret buf Argument promoted from" FMT_ADDR "\n", DBG_ADDR(*pRetBuffArg) ));
        PromoteCarefully(fn, *(Object**) pRetBuffArg, sc, GcFlags|CHECK_APP_DOMAIN);
    }

    if (fPinArgs)
    {

        //

        _ASSERTE(!pFunction->HasClassOrMethodInstantiation());

        CorElementType typ;
        LPVOID pArgAddr;
        while (typ = pmsig->PeekArg(), NULL != (pArgAddr = pargit->GetNextArgAddr()))
        {
            if (typ == ELEMENT_TYPE_SZARRAY)
            {
                ArrayBase *pArray = *((ArrayBase**)pArgAddr);

                if (pArray)
                {
                    (fn)(*(Object**)pArgAddr, sc, 
                         GC_CALL_PINNED | CHECK_APP_DOMAIN);
                }
            }
            else if (typ == ELEMENT_TYPE_BYREF)
            {
                {
                    (fn)(*(Object**)pArgAddr, sc, 
                         GC_CALL_PINNED | GC_CALL_INTERIOR | CHECK_APP_DOMAIN);
                }


            }
            else if (typ == ELEMENT_TYPE_STRING || (typ == ELEMENT_TYPE_CLASS && pmsig->IsStringType()))
            {
                (fn)(*(Object**)pArgAddr, sc, GC_CALL_PINNED);
            }
            else if (typ == ELEMENT_TYPE_CLASS || typ == ELEMENT_TYPE_OBJECT)
            {
                Object *pObj = *(Object**)pArgAddr;
                if (pObj != NULL)
                {
                    MethodTable *pMT = pObj->GetGCSafeMethodTable();
                    _ASSERTE(sizeof(UINT_PTR) == sizeof(MethodTable*));
                    // <NICE> Document why IsObjectClass is used here </NICE>
                    if (pMT->IsObjectClass() || pMT->IsBlittable() || pMT->HasLayout())
                    {
                        (fn)(*(Object**)pArgAddr, sc, 
                             GC_CALL_PINNED | CHECK_APP_DOMAIN);
                    }
                    else
                    {
                        (fn)(*(Object**)pArgAddr, sc, 
                             CHECK_APP_DOMAIN);
                    }
                }
            }
            else
            {
                pmsig->GcScanRoots(pArgAddr, fn, sc);
            }
        }
    }
    else
    {
        LPVOID pArgAddr;
    
        while (NULL != (pArgAddr = pargit->GetNextArgAddr()))
        {
            pmsig->GcScanRoots(pArgAddr, fn, sc);
        }
    }

}



#if defined (_DEBUG) && !defined (DACCESS_COMPILE)
// For IsProtectedByGCFrame, we need to know whether a given object ref is protected 
// by a ComPlusMethodFrame or a ComMethodFrame. Since GCScanRoots for those frames are 
// quite complicated, we don't want to duplicate their logic so we call GCScanRoots with 
// IsObjRefProtected (a fake promote function) and an extended ScanContext to do the checking.

struct IsObjRefProtectedScanContext : public ScanContext
{
    OBJECTREF * oref_to_check;
    BOOL        oref_protected;
    IsObjRefProtectedScanContext (OBJECTREF * oref)
    {
        thread_under_crawl = GetThread ();
        promotion = TRUE;
        oref_to_check = oref;
        oref_protected = FALSE;
    }
};

void IsObjRefProtected (Object*& object, ScanContext* sc, DWORD)
{
    LEAF_CONTRACT;
    IsObjRefProtectedScanContext * orefProtectedSc = (IsObjRefProtectedScanContext *)sc;
    if (&object == (Object **)(orefProtectedSc->oref_to_check))
        orefProtectedSc->oref_protected = TRUE;
}
#endif //defined (_DEBUG) && !defined (DACCESS_COMPILE)

#if defined (_DEBUG) && !defined (DACCESS_COMPILE)
BOOL FramedMethodFrame::Protects(OBJECTREF * ppORef)
{
    WRAPPER_CONTRACT;
    IsObjRefProtectedScanContext sc (ppORef);
    GcScanRoots (IsObjRefProtected, &sc);
    return sc.oref_protected;
}
#endif //defined (_DEBUG) && !defined (DACCESS_COMPILE)

#ifndef DACCESS_COMPILE

VOID ReportPointersFromStruct(promote_func *fn, ScanContext *sc, void *data)
{
    STATIC_CONTRACT_SO_TOLERANT;

    StackStructData *pStackInfo = (StackStructData*)data;
    if (pStackInfo == NULL || pStackInfo->pLocation == NULL)
        return; // nothing to do

    // the array of StackStructData on the stack is terminated by an "empty" element so this condition should be met sooner or later
    while (pStackInfo->pLocation)
    {
        MethodTable *pMT = pStackInfo->pValueType;
        if (!pMT->ContainsPointers ())
        {
            pStackInfo++;
            continue;
        }

        CGCDesc* map = CGCDesc::GetCGCDescFromMT(pMT);
        CGCDescSeries* cur = map->GetHighestSeries();
        CGCDescSeries* last = map->GetLowestSeries();
        DWORD size = pMT->GetBaseSize();
        _ASSERTE(cur >= last);

        void *src = pStackInfo->pLocation;
        do                                                                  
        {   
            // offset to embedded references in this series must be
            // adjusted by the VTable pointer, when in the unboxed state.
            size_t offset = cur->GetSeriesOffset() - sizeof(void*);
            OBJECTREF* srcPtr = (OBJECTREF*)(((BYTE*) src) + offset);
            OBJECTREF* srcPtrStop = (OBJECTREF*)((BYTE*) srcPtr + cur->GetSeriesSize() + size);         
            while (srcPtr < srcPtrStop)                                         
            {   
                (*fn)(*(Object**)srcPtr, sc, 0);
                srcPtr++;
            }                                                               
            cur--;                                                              
        } while (cur >= last);                                              

        pStackInfo++;
    }
}

#endif //DACCESS_COMPILE

//+----------------------------------------------------------------------------
//
//  Method:     TPMethodFrame::GcScanRoots    public
//
//  Synopsis:   GC protects arguments on the stack
//
//  History:    17-Feb-99                       Created
//
//+----------------------------------------------------------------------------
void TPMethodFrame::GcScanRoots(promote_func *fn, ScanContext* sc)
{
    WRAPPER_CONTRACT;

    // Delegate to FramedMethodFrame
    FramedMethodFrame::GcScanRoots(fn, sc);
    FramedMethodFrame::PromoteCallerStack(fn, sc);

    // Promote the returned object
    if(GetFunction()->ReturnsObject() == MetaSig::RETOBJ)
    {
        (*fn)(GetReturnObject(), sc, CHECK_APP_DOMAIN);
    }
    else if (GetFunction()->ReturnsObject() == MetaSig::RETBYREF)
    {
        PromoteCarefully(fn, GetReturnObject(), sc, GC_CALL_INTERIOR|CHECK_APP_DOMAIN);
    }
#ifdef ENREGISTERED_RETURNTYPE_MAXSIZE
    else
    {
        MetaSig mSig(GetFunction());
        
        if (mSig.HasNonStandardByvalReturn())
        {
            //
            // in these cases, the return value ptr points to a value class that might contain
            // objects in it.  On IA64, we have 4 slots to scan..
            //
            OBJECTREF oThrowable = NULL;
            // the type must already be loaded
            MethodTable * pReturnTypeMT = mSig.GetRetTypeHandle (&oThrowable,  ClassLoader :: DontLoadTypes).GetMethodTable ();
            // no failures in GC
            _ASSERTE (oThrowable == NULL && pReturnTypeMT != NULL);

            StackStructData structDataForGC[] = { StackStructData (GetReturnValuePtr(), pReturnTypeMT), StackStructData()};
            ReportPointersFromStruct (fn, sc, structDataForGC);
        }
    }
#endif // ENREGISTERED_RETURNTYPE_MAXSIZE

    return;
}

#endif // #ifndef DACCESS_COMPILE

#if !defined(DACCESS_COMPILE)
#endif // !DACCESS_COMPILE

//+----------------------------------------------------------------------------
//
//  Method:     TPMethodFrame::GcScanRoots    public
//
//  Synopsis:   Return where the frame will execute next - the result is filled
//              into the given "trace" structure.  The frame is responsible for
//              detecting where it is in its execution lifetime.
//
//
//  History:    26-Jun-99                       Created
//
//+----------------------------------------------------------------------------
BOOL TPMethodFrame::TraceFrame(Thread *thread, BOOL fromPatch, 
                               TraceDestination *trace, REGDISPLAY *regs)
{
    WRAPPER_CONTRACT;

    // Sanity check
    _ASSERTE(NULL != TheTPStub());

    // We want to set a frame patch, unless we're already at the
    // frame patch, in which case we'll trace addrof_code which 
    // should be set by now.

    if (fromPatch)
    {
        trace->InitForStub((TADDR) GetFunction()->GetSafeAddrofCode());
    }
    else
    {
        Stub *tpStub = TheTPStub();
        PREFIX_ASSUME(tpStub != NULL);
        USHORT patchOffset = tpStub->GetPatchOffset();


        trace->InitForStub((TADDR) (tpStub->GetEntryPoint() + patchOffset));
    }
    
    return TRUE;

}

//+----------------------------------------------------------------------------
//
//  Method:     TPMethodFrame::GcScanRoots    public
//
//  Synopsis:   Return only a valid method descriptor. TPMethodFrame has slot
//              number in the prolog and bytes to pop in the epilog portions of
//              the stub. It should not allow crawling during such weird periods.
//
//  History:    17-Feb-99                       Created
//
//+----------------------------------------------------------------------------
MethodDesc *TPMethodFrame::GetFunction()
{
    WRAPPER_CONTRACT;

    if (m_Datum >> 16) {
        return PTR_MethodDesc(m_Datum);
    }
    else {
        return NULL;
    }
}

#ifndef DACCESS_COMPILE

VOID SecurityFrame::GcScanRoots(promote_func *fn, ScanContext* sc)
{
    WRAPPER_CONTRACT;

    Object         **pObject;

    // Do all parent scans first
    FramedMethodFrame::GcScanRoots(fn, sc);

    // Promote Security Object
    pObject = (Object **) GetAddrOfSecurityDesc();
    if (*pObject != NULL)
        {
            LOG((LF_GC, INFO3, "        Promoting Security Object from" FMT_ADDR "to ", DBG_ADDR(*pObject) ));
            (*fn)( *pObject, sc, CHECK_APP_DOMAIN );
            LOG((LF_GC, INFO3, FMT_ADDR "\n", DBG_ADDR(*pObject) ));
        }
    return;
}

#endif // #ifndef DACCESS_COMPILE



#ifndef DACCESS_COMPILE


//
// UMThkCalFrame::Promote Callee Stack
// If we are the topmost frame, and if GC is in progress
// then there might be some arguments that we have to
// during marshalling

void UMThkCallFrame::PromoteCalleeStack(promote_func* fn, 
                                       ScanContext* sc)
{
    WRAPPER_CONTRACT;

    PCCOR_SIGNATURE pCallSig;
    Module         *pModule;
    BYTE           *pArgument;

    LOG((LF_GC, INFO3, "    Promoting UMThk call frame marshalled arguments\n" ));

    // the marshalled objects are placed above the frame above the locals

    // Get pointer to GCInfo.
    UnmanagedToManagedCallGCInfo* pGCInfo = GetGCInfoPtr();

    // If the args don't need GC protection then we are done.
    if (!pGCInfo->IsArgsGCProtectionEnabled())
        return;

    // for interpreted case, the args were _alloca'ed , so find the pointer
    // to args from the header offset info
    pArgument = (BYTE *)GetPointerToDstArgs(); // pointer to the args

    _ASSERTE(pArgument != NULL);
    // For Now get the signature...
    DWORD cbSigSize;
    pCallSig = GetTargetCallSig(&cbSigSize);
    if (! pCallSig)
        return;

    pModule = GetTargetModule();
    _ASSERTE(pModule);

    MetaSig msig(pCallSig, cbSigSize, pModule, NULL, NULL);
    MetaSig msig2(pCallSig, cbSigSize, pModule, NULL, NULL);
    ArgIterator argit(NULL, &msig2, GetUMEntryThunk()->GetUMThunkMarshInfo()->IsStatic());


    int ofs;
    while (0 != (ofs = argit.GetNextOffset()))
    {
        msig.NextArg();
        msig.GcScanRoots(pArgument + ofs, fn, sc);
    }
}

// used by PromoteCalleeStack to get the destination function sig
// NOTE: PromoteCalleeStack only promotes bona-fide arguments, and not
// the "this" reference. The whole purpose of PromoteCalleeStack is
// to protect the partially constructed argument array during
// the actual process of argument marshaling.
PCCOR_SIGNATURE UMThkCallFrame::GetTargetCallSig(DWORD *pcbSigSize)
{
    WRAPPER_CONTRACT;

    return GetUMEntryThunk()->GetUMThunkMarshInfo()->GetSig(pcbSigSize);
}

// Same for destination function module.
Module *UMThkCallFrame::GetTargetModule()
{
    WRAPPER_CONTRACT;

    return GetUMEntryThunk()->GetUMThunkMarshInfo()->GetModule();
}

#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE
void UMThkCallFrame::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;
    UnmanagedToManagedCallFrame::EnumMemoryRegions(flags);

    // Pieces of the UMEntryThunk need to be saved.
    UMEntryThunk *pThunk = GetUMEntryThunk();
    DacEnumMemoryRegion(PTR_HOST_TO_TADDR(pThunk), sizeof(UMEntryThunk));        
    
    UMThunkMarshInfo *pMarshInfo = pThunk->GetUMThunkMarshInfo();
    DacEnumMemoryRegion(PTR_HOST_TO_TADDR(pMarshInfo), sizeof(UMThunkMarshInfo));        
}
#endif

// Return the # of stack bytes pushed by the unmanaged caller.
UINT UMThkCallFrame::GetNumCallerStackBytes()
{
    WRAPPER_CONTRACT;

    return GetUMEntryThunk()->GetUMThunkMarshInfo()->GetCbRetPop();
}


#ifndef DACCESS_COMPILE

const BYTE* UMThkCallFrame::GetManagedTarget()
{
    WRAPPER_CONTRACT;

    UMEntryThunk *umet = GetUMEntryThunk();

    if (umet)
    {
        return umet->GetManagedTarget();
    }
    else
    {
        return NULL;
    }
}





//-------------------------------------------------------------------
// Executes each stored cleanup task and resets the worklist back
// to empty. Some task types are conditional based on the
// "fBecauseOfException" flag. This flag distinguishes between
// cleanups due to normal method termination and cleanups due to
// an exception.
//-------------------------------------------------------------------
VOID __stdcall CleanupWorkList::Cleanup(BOOL fBecauseOfException)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    CleanupNode *pnode = m_pNodes;

    // Make one non-gc-triggering pass to chop off protected marshalers.
    // We don't want to be calling a marshaler that's already been
    // deallocated due to a GC happening during the cleanup itself.
    while (pnode)
    {
        if (pnode->m_type == CL_PROTECTEDMARSHALER)
            pnode->m_pMarshaler = NULL;

        pnode = pnode->m_next;
    }

    pnode = m_pNodes;

    if (pnode == NULL)
        return;

#ifdef _DEBUG
    ADID thisDomainId = GetAppDomain()->GetId();
#endif

    while (pnode)
    {
        // Should never end up in cleanup from another domain. Should always call cleanup prior to returning from
        // the domain where the cleanup list was created.
        _ASSERTE(thisDomainId == pnode->m_dwDomainId);

        switch(pnode->m_type)
        {
            case CL_COTASKFREE:
                CoTaskMemFree(pnode->m_pv);
                break;

            case CL_FASTFREE:
                // This collapse will actually deallocate the node itself (it
                // should always be the last node in the list anyway). Make sure
                // we don't attempt to read the next pointer after the
                // deallocation.
                _ASSERTE(pnode->m_next == NULL);
                GetThread()->m_MarshalAlloc.Collapse(pnode->m_pv);
                m_pNodes = NULL;
                return;


            case CL_NSTRUCTDESTROY: 
                pnode->nd.m_pFieldMarshaler->DestroyNative(pnode->nd.m_pNativeData);
                break;

            case CL_RESTORECULTURE:
                {
                    GCX_COOP();

                    OBJECTREF CultureInfoObj = ObjectToOBJECTREF(pnode->m_oref);
                    OBJECTREF ExceptionObj = NULL;
                    GCPROTECT_BEGIN(CultureInfoObj)
                    GCPROTECT_BEGIN(ExceptionObj)
                    {
                        EX_TRY
                        {
                            GetThread()->SetCulture(&CultureInfoObj, FALSE);
                        }
                        EX_CATCH
                        {
                        }
                        EX_END_CATCH(SwallowAllExceptions);
                    }
                    GCPROTECT_END();
                    GCPROTECT_END();
                }
                break;

            case CL_NEWLAYOUTDESTROYNATIVE:
                FmtClassDestroyNative(pnode->nlayout.m_pnative, pnode->nlayout.m_pMT);
                break;

            case CL_PROTECTEDOBJREF: //fallthru
            case CL_ISVISIBLETOGC:
            case CL_PROTECTEDMARSHALER:
                // nothing to do here.
                break;

            case CL_MARSHALER_EXCEP:        //fallthru
            case CL_MARSHALERREINIT_EXCEP:
                if (fBecauseOfException)
                {
#if defined(_DEBUG) && !defined(STUBS_AS_IL)

                    int dummy;
                    _ASSERTE( sizeof(size_t) >= sizeof(void*) );  //ensure the assert expression below is doing a safe cast
                    _ASSERTE( ((size_t)&dummy) < (size_t) (pnode->m_pMarshaler) );
#endif
                    if (pnode->m_type == CL_MARSHALER_EXCEP)
                    {
                        pnode->m_pMarshaler->DoExceptionCleanup();
                    }
                    else
                    {
                        _ASSERTE(pnode->m_type == CL_MARSHALERREINIT_EXCEP);
                        pnode->m_pMarshaler->DoExceptionReInit();
                    }
                }
                break;

            case CL_RELEASESAFEHANDLE:
                {
                    GCX_COOP();

                    SAFEHANDLE SafeHandleObj = (SAFEHANDLE)ObjectToOBJECTREF(pnode->m_oref);
                    OBJECTREF ExceptionObj = NULL;
                    GCPROTECT_BEGIN(SafeHandleObj)
                    GCPROTECT_BEGIN(ExceptionObj)
                    {
                        EX_TRY
                        {
                            SafeHandleObj->Release();  
                        }
                        EX_CATCH
                        {
                        }
                        EX_END_CATCH(SwallowAllExceptions);
                    }
                    GCPROTECT_END();
                    GCPROTECT_END();
                }
                break;

            case CL_BACKPROPAGATESAFEHANDLE:
                {
                    // Check whether the new handle value actually differs from
                    // the original. Since the back propagation is actually to a
                    // new safe handle instance doing otherwise would lead to
                    // two managed handles wrapping the native handle and a
                    // double release at some point.
                    if (*pnode->bpsh.m_pHandle == pnode->bpsh.m_hOldHandle)
                        break;

                    GCX_COOP();

                    SAFEHANDLE SafeHandleObj = (SAFEHANDLE)ObjectToOBJECTREF(pnode->bpsh.m_oref);
                    OBJECTREF ExceptionObj = NULL;
                    GCPROTECT_BEGIN(ExceptionObj)
                    GCPROTECT_BEGIN(SafeHandleObj)
                    {
                        EX_TRY
                        {
                            SafeHandleObj->SetHandle(*(pnode->bpsh.m_pHandle));
                        }
                        EX_CATCH
                        {
                        }
                        EX_END_CATCH(SwallowAllExceptions);
                    }
                    GCPROTECT_END();
                    GCPROTECT_END();
                }
                break;

            case CL_BACKPROPAGATECRITICALHANDLE:
                {
                    // Check whether the new handle value actually differs from
                    // the original. Since the back propagation is actually to a
                    // new critical handle instance doing otherwise would lead to
                    // two managed handles wrapping the native handle and a
                    // double release at some point.
                    if (*pnode->bpsh.m_pHandle == pnode->bpsh.m_hOldHandle)
                        break;

                    GCX_COOP();

                    CRITICALHANDLE CriticalHandleObj = (CRITICALHANDLE)ObjectToOBJECTREF(pnode->bpsh.m_oref);
                    GCPROTECT_BEGIN(CriticalHandleObj)
                    {
                        CriticalHandleObj->SetHandle(*(pnode->bpsh.m_pHandle));
                    }
                    GCPROTECT_END();
                }
                break;

            default:
                _ASSERTE(!"Bad CleanupNode type.");
        }

        pnode = pnode->m_next;
    }

    m_pNodes = NULL;
}

//-------------------------------------------------------------------
// Inserts a new task of the given type and datum.
//-------------------------------------------------------------------
CleanupWorkList::CleanupNode*
CleanupWorkList::Schedule(CleanupType ct, LPVOID pv)
{
    CONTRACT (CleanupWorkList::CleanupNode*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;
    
    CleanupNode *pnode = (CleanupNode *)(GetThread()->m_MarshalAlloc.AllocNoThrow(sizeof(CleanupNode)));
    if (pnode)
    {
        pnode->m_type = ct;
        pnode->m_pv   = pv;
        pnode->m_next = m_pNodes;
#ifdef _DEBUG
        pnode->m_dwDomainId = GetAppDomain()->GetId();
#endif
        m_pNodes      = pnode;
    }
    RETURN pnode;
}





//-------------------------------------------------------------------
// Schedules an unconditional free of the native version
// of an NStruct reference field. Note that pNativeData points into
// the middle of the external part of the NStruct, so someone
// has to hold a gc reference to the wrapping NStruct until
// the destroy is done.
//-------------------------------------------------------------------
VOID CleanupWorkList::ScheduleUnconditionalNStructDestroy(const FieldMarshaler *pFieldMarshaler, LPVOID pNativeData)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;
    
    CleanupNode *pnode = (CleanupNode *)(GetThread()->m_MarshalAlloc.AllocNoThrow(sizeof(CleanupNode)));
    if (!pnode)
    {
        pFieldMarshaler->DestroyNative(pNativeData);
        ENCLOSE_IN_EXCEPTION_HANDLER(ThrowOutOfMemory);
    }
    PREFIX_ASSUME(pnode != NULL);
    
    pnode->m_type               = CL_NSTRUCTDESTROY;
    pnode->m_next               = m_pNodes;
    pnode->nd.m_pFieldMarshaler = pFieldMarshaler;
    pnode->nd.m_pNativeData     = pNativeData;
#ifdef _DEBUG
    pnode->m_dwDomainId         = GetAppDomain()->GetId();
#endif
    m_pNodes                    = pnode;
}




//-------------------------------------------------------------------
// CleanupWorkList::ScheduleLayoutDestroyNative
// schedule cleanup of marshaled struct fields and of the struct itself.
// Throws a COM+ exception if failed.
//-------------------------------------------------------------------
LPVOID CleanupWorkList::NewScheduleLayoutDestroyNative(MethodTable *pMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    CleanupNode *pnode = NULL;
    LPVOID       pNative = NULL;

    pNative = GetThread()->m_MarshalAlloc.AllocSafeThrow(pMT->GetNativeSize());
    FillMemory(pNative, pMT->GetNativeSize(), 0);

    pnode = (CleanupNode *)(GetThread()->m_MarshalAlloc.AllocSafeThrow(sizeof(CleanupNode)));

    pnode->m_type               = CL_NEWLAYOUTDESTROYNATIVE;
    pnode->m_next               = m_pNodes;
    pnode->nlayout.m_pnative    = pNative;
    pnode->nlayout.m_pMT        = pMT;
#ifdef _DEBUG
    pnode->m_dwDomainId         = GetAppDomain()->GetId();
#endif
    m_pNodes                    = pnode;

    return pNative;
}

//-------------------------------------------------------------------
// CleanupWorkList::ScheduleSafeHandleRelease
// schedule releasing the SafeHandle which decreses it's ref count
// by 1.
// Throws a COM+ exception if failed.
//-------------------------------------------------------------------
VOID CleanupWorkList::ScheduleSafeHandleRelease(SAFEHANDLE *pSafeHandleObj)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pSafeHandleObj));
        PRECONDITION(*pSafeHandleObj != NULL);
    }
    CONTRACTL_END;

    CleanupNode *pNode = Schedule(CL_RELEASESAFEHANDLE, NULL);
    if (!pNode)
    {
        ENCLOSE_IN_EXCEPTION_HANDLER(ThrowOutOfMemory);
    }
    PREFIX_ASSUME(pNode != NULL);

    pNode->m_oref = OBJECTREFToObject((OBJECTREF)*pSafeHandleObj);
}

//-------------------------------------------------------------------
// CleanupWorkList::ScheduleRefSafeHandleBackPropagate
// schedule back propagating the handle to the safe handle.
// Back propagation only happens if the new handle is different from the
// original value (this matters since the propagation is to a different
// safe handle instance).
// Throws a COM+ exception if failed.
//-------------------------------------------------------------------
VOID CleanupWorkList::ScheduleRefSafeHandleBackPropagate(SAFEHANDLE *pSafeHandleObj, LPVOID *pHandle, LPVOID hOldHandle)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pSafeHandleObj));
        PRECONDITION(*pSafeHandleObj != NULL);
        PRECONDITION(CheckPointer(pHandle));
    }
    CONTRACTL_END;

    CleanupNode *pnode = (CleanupNode *)(GetThread()->m_MarshalAlloc.AllocSafeThrow(sizeof(CleanupNode)));

    pnode->m_type               = CL_BACKPROPAGATESAFEHANDLE;
    pnode->m_next               = m_pNodes;
    pnode->bpsh.m_oref          = OBJECTREFToObject((OBJECTREF)*pSafeHandleObj);
    pnode->bpsh.m_pHandle       = pHandle;
    pnode->bpsh.m_hOldHandle    = hOldHandle;
#ifdef _DEBUG
    pnode->m_dwDomainId         = GetAppDomain()->GetId();
#endif
    m_pNodes                    = pnode;
}

//-------------------------------------------------------------------
// CleanupWorkList::ScheduleRefCriticalHandleBackPropagate
// schedule back propagating the handle to the critical handle.
// Back propagation only happens if the new handle is different from the
// original value (this matters since the propagation is to a different
// critical handle instance).
// Throws a COM+ exception if failed.
//-------------------------------------------------------------------
VOID CleanupWorkList::ScheduleRefCriticalHandleBackPropagate(CRITICALHANDLE *pCriticalHandleObj, LPVOID *pHandle, LPVOID hOldHandle)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pCriticalHandleObj));
        PRECONDITION(*pCriticalHandleObj != NULL);
        PRECONDITION(CheckPointer(pHandle));
    }
    CONTRACTL_END;

    CleanupNode *pnode = (CleanupNode *)(GetThread()->m_MarshalAlloc.AllocSafeThrow(sizeof(CleanupNode)));

    pnode->m_type               = CL_BACKPROPAGATECRITICALHANDLE;
    pnode->m_next               = m_pNodes;
    pnode->bpsh.m_oref          = OBJECTREFToObject((OBJECTREF)*pCriticalHandleObj);
    pnode->bpsh.m_pHandle       = pHandle;
    pnode->bpsh.m_hOldHandle    = hOldHandle;
#ifdef _DEBUG
    pnode->m_dwDomainId         = GetAppDomain()->GetId();
#endif
    m_pNodes                    = pnode;
}


//-------------------------------------------------------------------
// CoTaskFree memory unconditionally
//-------------------------------------------------------------------
VOID CleanupWorkList::ScheduleCoTaskFree(LPVOID pv)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if( pv != NULL)
    {
        if (!Schedule(CL_COTASKFREE, pv))
        {
            CoTaskMemFree(pv);
            ENCLOSE_IN_EXCEPTION_HANDLER(ThrowOutOfMemory);
        }
    }
}


//-------------------------------------------------------------------
// CoTaskFree memory unconditionally, non-throwing version
//-------------------------------------------------------------------
HRESULT CleanupWorkList::ScheduleCoTaskFreeNonThrow(LPVOID pv)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if( pv != NULL)
    {
        if (!Schedule(CL_COTASKFREE, pv))
        {
            CoTaskMemFree(pv);
            return E_FAIL;
        }
    }

    return S_OK;
}



//-------------------------------------------------------------------
// StackingAllocator.Collapse during exceptions
//-------------------------------------------------------------------
VOID CleanupWorkList::ScheduleFastFree(LPVOID checkpoint)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
    } CONTRACTL_END;

    if (!Schedule(CL_FASTFREE, checkpoint))
    {
        GetThread()->m_MarshalAlloc.Collapse(checkpoint);
        ENCLOSE_IN_EXCEPTION_HANDLER ( ThrowOutOfMemory );
    }
}



//-------------------------------------------------------------------
// Schedule restoring thread's current culture to the specified 
// culture.
//-------------------------------------------------------------------
VOID CleanupWorkList::ScheduleUnconditionalCultureRestore(OBJECTREF *pCultureObj)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    CleanupNode *pNode = Schedule(CL_RESTORECULTURE, NULL);
    if (!pNode)
    {
        ENCLOSE_IN_EXCEPTION_HANDLER(ThrowOutOfMemory);
    }
    PREFIX_ASSUME(pNode != NULL);

    pNode->m_oref = OBJECTREFToObject(*pCultureObj);
}


//-------------------------------------------------------------------
// CleanupWorkList::NewProtectedObjRef()
// holds a protected objref (used for creating the buffer for
// an unmanaged->managed byref object marshal. We can't use an
// objecthandle because modifying those without using the handle
// api opens up writebarrier violations.
//
// Must have called IsVisibleToGc() first.
//-------------------------------------------------------------------
OBJECTREF* CleanupWorkList::NewProtectedObjectRef(OBJECTREF oref)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    CleanupNode *pNew;
    GCPROTECT_BEGIN(oref);

#ifdef _DEBUG
    {
        CleanupNode *pNode = m_pNodes;
            while (pNode)
            {
                if (pNode->m_type == CL_ISVISIBLETOGC)
                {
                    break;
                }
            pNode = pNode->m_next;
        }

        if (pNode == NULL)
        {
            _ASSERTE(!"NewProtectedObjectRef called without proper gc-scanning. The big comment right after this assert says a lot more. Read it.");
            // READ THIS! When you use a node of this type, you must
            // invoke CleanupWorklist::GCScanRoots() as part of
            // your gcscan net. Because this node type was added
            // late in the project and cleanup lists did
            // not have a GC-scanning requirement prior to
            // this, we've added this assert to remind
            // you of this requirement. You will not be permitted
            // to add this node type to a cleanuplist until
            // you make a one-time call to "IsVisibleToGc()" on
            // the cleanup list. That call certifies that
            // you've read and understood this warning and have
            // implemented the gc-scanning required.
        }
    }
#endif


    pNew = Schedule(CL_PROTECTEDOBJREF, NULL);
    if (!pNew)
    {
        ENCLOSE_IN_EXCEPTION_HANDLER(ThrowOutOfMemory);
    }
    PREFIX_ASSUME(pNew != NULL);
    
    pNew->m_oref = OBJECTREFToObject(oref);    

    GCPROTECT_END();
    return (OBJECTREF*)&(pNew->m_oref);
}

CleanupWorkList::MarshalerCleanupNode * CleanupWorkList::ScheduleMarshalerCleanupOnException(Marshaler *pMarshaler)
{
    CONTRACT (CleanupWorkList::MarshalerCleanupNode*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());

        // make sure some idiot didn't ignore the warning not to add fields to
        // MarshalerCleanupNode.
        PRECONDITION(sizeof(CleanupNode) == sizeof(MarshalerCleanupNode));

        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    CleanupNode *pNew = Schedule(CL_MARSHALER_EXCEP, pMarshaler);
    if (!pNew)
    {
        ENCLOSE_IN_EXCEPTION_HANDLER(ThrowOutOfMemory);
    }

    RETURN (*(CleanupWorkList::MarshalerCleanupNode**)&pNew);
}

//-------------------------------------------------------------------
// CleanupWorkList::NewProtectedObjRef()
// holds a Marshaler. The cleanupworklist will own the task
// of calling the marshaler's GcScanRoots fcn.
//
// It makes little architectural sense for the CleanupWorkList to
// own this item. But it's late in the project to be adding
// fields to frames, and it so happens everyplace we need this thing,
// there's alreay a cleanuplist. So it's elected.
//
// Must have called IsVisibleToGc() first.
//-------------------------------------------------------------------
VOID CleanupWorkList::NewProtectedMarshaler(Marshaler *pMarshaler)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END;

    CleanupNode *pNew;

#ifdef _DEBUG
    {
        CleanupNode *pNode = m_pNodes;
            while (pNode)
            {
                if (pNode->m_type == CL_ISVISIBLETOGC)
                {
                    break;
                }
            pNode = pNode->m_next;
        }

        if (pNode == NULL)
        {
            _ASSERTE(!"NewProtectedObjectRef called without proper gc-scanning. The big comment right after this assert says a lot more. Read it.");
            // READ THIS! When you use a node of this type, you must
            // invoke CleanupWorklist::GCScanRoots() as part of
            // your gcscan net. Because this node type was added
            // late in the project and cleanup lists did
            // not have a GC-scanning requirement prior to
            // this, we've added this assert to remind
            // you of this requirement. You will not be permitted
            // to add this node type to a cleanuplist until
            // you make a one-time call to "IsVisibleToGc()" on
            // the cleanup list. That call certifies that
            // you've read and understood this warning and have
            // implemented the gc-scanning required.
        }
    }
#endif


    pNew = Schedule(CL_PROTECTEDMARSHALER, pMarshaler);
    if (!pNew)
    {
        ENCLOSE_IN_EXCEPTION_HANDLER(ThrowOutOfMemory);
    }
}




//-------------------------------------------------------------------
// If you've called IsVisibleToGc(), must call this.
//-------------------------------------------------------------------
void CleanupWorkList::GcScanRoots(promote_func *fn, ScanContext* sc)
{
    WRAPPER_CONTRACT;

    CleanupNode *pnode = m_pNodes;

    while (pnode) {

        switch(pnode->m_type) 
        {
            case CL_PROTECTEDOBJREF: 
            case CL_RELEASESAFEHANDLE:
            case CL_RESTORECULTURE:
                if (pnode->m_oref != NULL)
                {
                    LOG((LF_GC, INFO3, "GC Protection Frame Promoting" FMT_ADDR "to",
                         DBG_ADDR(pnode->m_oref) ));
                    (*fn)(pnode->m_oref, sc, 0);
                    LOG((LF_GC, INFO3, FMT_ADDR "\n", DBG_ADDR(pnode->m_oref) ));
                }
                break;

            case CL_PROTECTEDMARSHALER:
                if (pnode->m_pMarshaler)
                {
                    pnode->m_pMarshaler->GcScanRoots(fn, sc);
                }
                break;

            case CL_BACKPROPAGATESAFEHANDLE:
            case CL_BACKPROPAGATECRITICALHANDLE:
                if (pnode->bpsh.m_oref != NULL)
                {
                    LOG((LF_GC, INFO3, "GC Protection Frame Promoting" FMT_ADDR "to",
                         DBG_ADDR(pnode->bpsh.m_oref) ));
                    (*fn)(pnode->bpsh.m_oref, sc, 0);
                    LOG((LF_GC, INFO3, FMT_ADDR "\n", DBG_ADDR(pnode->bpsh.m_oref) ));
                }
                break;                

            default:
                break;
        }

        pnode = pnode->m_next;
    }
}


//-------------------------------------------------------------------
// Destructor (calls Cleanup(FALSE))
//-------------------------------------------------------------------
CleanupWorkList::~CleanupWorkList()
{
    WRAPPER_CONTRACT;

    Cleanup(FALSE);
}

#endif // #ifndef DACCESS_COMPILE


//------------------------------------------------------------
// Constructor
//------------------------------------------------------------
ArgIterator::ArgIterator(FramedMethodFrame *pFrame, MetaSig* pSig)
{
    WRAPPER_CONTRACT;

    Init((LPBYTE)pFrame, pSig, pFrame->GetFunction()->IsStatic(),
        sizeof(FramedMethodFrame), FramedMethodFrame::GetOffsetOfArgumentRegisters());
}

//------------------------------------------------------------
// Another constructor when you dont have active frame FramedMethodFrame
//------------------------------------------------------------
ArgIterator::ArgIterator(LPBYTE pFrameBase, MetaSig* pSig, BOOL fIsStatic)
{
    WRAPPER_CONTRACT;

    Init(pFrameBase, pSig, fIsStatic,
        sizeof(FramedMethodFrame), FramedMethodFrame::GetOffsetOfArgumentRegisters());
}

//------------------------------------------------------------
// An even more primitive constructor when dont have have a
// a FramedMethodFrame
//------------------------------------------------------------
ArgIterator::ArgIterator(LPBYTE pFrameBase, MetaSig* pSig, int stackArgsOfs, int regArgsOfs)
{
    WRAPPER_CONTRACT;

    Init(pFrameBase, pSig, !pSig->HasThis(), stackArgsOfs, regArgsOfs);
}

//------------------------------------------------------------
// helper for ArgIterator::Init.  This should return the offset
// of the first stack parameter (that is not a special argument
// like 'this', the return buffer pointer, or varargs token).
//------------------------------------------------------------
static int GetInitialOfsAdjust(MetaSig* pSig, BOOL fIsStatic)
{
    WRAPPER_CONTRACT;

#ifdef _X86_
    return pSig->SizeOfActualFixedArgStack(fIsStatic);
#else

    int initOfs = 0;
    
    if (!fIsStatic)
        initOfs += sizeof(void*);

#if defined(_PPC_) // retbuf
    if (pSig->HasRetBuffArg())
        initOfs += sizeof(void*);
#endif


    // The vararg sig token is passed first so we need to skip over it
    if (pSig->IsVarArg() || pSig->IsTreatAsVarArg())
        initOfs += sizeof(void*);

    return initOfs;
#endif
}

//------------------------------------------------------------
// ArgIterator common init
//------------------------------------------------------------
void ArgIterator::Init(LPBYTE pFrameBase, MetaSig* pSig, BOOL fIsStatic, int stackArgsOfs, int regArgsOfs)
{
    WRAPPER_CONTRACT;

    m_curOfs = stackArgsOfs + GetInitialOfsAdjust(pSig, fIsStatic);

    m_pFrameBase = pFrameBase;

    m_pSig = pSig;
    m_pSig->Reset();                // Reset the enum so we are at the beginning of the signature

    m_argNum = -1;

    m_numRegistersUsed = 0;

    m_regArgsOfs = regArgsOfs;

    BYTE callConv = pSig->GetCallingConvention();

    if (!(fIsStatic)) {
        IsArgumentInRegister(&m_numRegistersUsed, ELEMENT_TYPE_CLASS, sizeof(void*), TRUE, callConv, NULL);
    }

    if (pSig->HasRetBuffArg())
        m_numRegistersUsed++;

    
    _ASSERTE(m_numRegistersUsed <= NUM_ARGUMENT_REGISTERS);
}

int ArgIterator::GetThisOffset(MetaSig* pSig)
{
    WRAPPER_CONTRACT;

#if defined(_X86_)
    int offsetOfThis = FramedMethodFrame::GetOffsetOfArgumentRegisters() + offsetof(ArgumentRegisters, THIS_REG);
#else
    int offsetOfThis = (int) sizeof(FramedMethodFrame);
#endif

#if THISPTR_LOCATION > 0
    _ASSERTE(NULL != pSig); // requires pSig
    if (pSig->HasRetBuffArg())
    {
        // AMD64 always has the return buffer argument first, even before the this pointer
        offsetOfThis += sizeof(void*);
    }
#endif

    return offsetOfThis;
}

int ArgIterator::GetRetBuffArgOffset(MetaSig* pSig, UINT *pRegStructOfs/* = NULL*/)
{
    WRAPPER_CONTRACT;

#if defined(_X86_)
    _ASSERTE(NUM_ARGUMENT_REGISTERS > 1);

    // Assume it is the first register argument
    int ret = FramedMethodFrame::GetOffsetOfArgumentRegisters() + (NUM_ARGUMENT_REGISTERS - 1)* sizeof(void*);
    if (pRegStructOfs)
        *pRegStructOfs = (NUM_ARGUMENT_REGISTERS - 1) * sizeof(void*);
        
    // if non-static, however, it is the next argument
    if (pSig->HasThis()) {
        ret -= sizeof(void*);
        if (pRegStructOfs) {
            (*pRegStructOfs) -= sizeof(void*);
        }
    }
    return(ret);
#elif defined(_PPC_)
    if (pRegStructOfs)
        *pRegStructOfs = (DWORD) -1;
    return sizeof(FramedMethodFrame);
#else
    PORTABILITY_ASSERT("Calling convention not specified for new platform");
    return 0;
#endif
}

int ArgIterator::GetVASigCookieOffset(MetaSig* pSig)
{
    WRAPPER_CONTRACT;

#if defined(_X86_)
    return sizeof(FramedMethodFrame);
#else
    int offsetOfVACookie = (int) sizeof(FramedMethodFrame);

    if (pSig->HasThis())
    {
        offsetOfVACookie += sizeof(void*);
    }

#if THISPTR_LOCATION > 0
    if (pSig->HasRetBuffArg())
    {
        // AMD64 always has the return buffer argument first, even before the this pointer
        offsetOfVACookie += sizeof(void*);
    }
#endif

    return offsetOfVACookie;
#endif
}

/*---------------------------------------------------------------------------------
    Same as GetNextOffset, but uses cached argument type and size info.
    DOES NOT ALTER state of MetaSig::m_pLastType etc !!
-----------------------------------------------------------------------------------*/

int ArgIterator::GetNextOffsetFaster(BYTE *pType, UINT32 *pStructSize, UINT *pRegStructOfs/* = NULL*/)
{
    WRAPPER_CONTRACT;

    int result = m_curOfs;
    
    if (m_pSig->m_flags & MetaSig::SIG_OFFSETS_INITTED)
    {
        if (m_curOfs != 0) 
        {
            m_argNum++;
            _ASSERTE(m_argNum <= MAX_CACHED_SIG_SIZE);
            BYTE typ = m_pSig->m_types[m_argNum];
            *pType = typ;

            if (typ == ELEMENT_TYPE_END) {
                m_curOfs = result = 0;
            } 
            else 
            {
                *pStructSize = m_pSig->m_sizes[m_argNum];

                if (m_pSig->m_offsets[m_argNum] != -1)
                {
                    if (pRegStructOfs) {
                        *pRegStructOfs = m_pSig->m_offsets[m_argNum];
                    }
                    result = m_regArgsOfs + m_pSig->m_offsets[m_argNum];
                } 
                else 
                {
                    if (pRegStructOfs) {
                        *pRegStructOfs = (UINT)(-1);
                    }

                    UINT32 stackSize = StackElemSize(*pStructSize);

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
                    if (MetaSig::IsArgPassedByRef(stackSize))
                    {
                        stackSize = STACK_ELEM_SIZE;
                    }
#endif

#if !(defined(STACK_GROWS_DOWN_ON_ARGS_WALK)^defined(STACK_GROWS_UP_ON_ARGS_WALK))
#error One and only one between STACK_GROWS_DOWN_ON_ARGS_WALK and STACK_GROWS_UP_ON_ARGS_WALK must be defined!
#endif

#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
                    m_curOfs -= stackSize;
#endif
                    result = m_curOfs;
#ifdef STACK_GROWS_UP_ON_ARGS_WALK
                    m_curOfs += stackSize;
#endif
                }
            }
        }
    }
    else 
    {
            
        UINT32 structSize;
        if (m_curOfs != 0) {
            BYTE typ = m_pSig->NextArgNormalized(&structSize);
            *pType = typ;
            int offsetIntoArgumentRegisters;

            if (typ == ELEMENT_TYPE_END) {
                m_curOfs = result = 0;
            }
            else {
                *pStructSize = structSize;
                BYTE callingconvention = m_pSig->GetCallingConvention();

                if (IsArgumentInRegister(&m_numRegistersUsed, typ, structSize, FALSE, callingconvention, &offsetIntoArgumentRegisters)) {
                    if (pRegStructOfs) {
                        *pRegStructOfs = offsetIntoArgumentRegisters;
                    }
                    return m_regArgsOfs + offsetIntoArgumentRegisters;
                } 
                else {
                    if (pRegStructOfs) {
                        *pRegStructOfs = (UINT)(-1);
                    }

                    UINT32 stackSize = StackElemSize(structSize);

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
                    if (MetaSig::IsArgPassedByRef(stackSize))
                    {
                        stackSize = STACK_ELEM_SIZE;
                    }
#endif

#if !(defined(STACK_GROWS_DOWN_ON_ARGS_WALK)^defined(STACK_GROWS_UP_ON_ARGS_WALK))
#error One and only one between STACK_GROWS_DOWN_ON_ARGS_WALK and STACK_GROWS_UP_ON_ARGS_WALK must be defined!
#endif

#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
                    m_curOfs -= stackSize;
#endif
                    result = m_curOfs;
#ifdef STACK_GROWS_UP_ON_ARGS_WALK
                    m_curOfs += stackSize;
#endif
                }
            }
        }
    }   // Cache initted or not
    
    return result;
}

//-----------------------------------------------------------
// Get the extra param offset for shared generic code
//-----------------------------------------------------------
int ArgIterator::GetParamTypeArgOffset(INT *pRegStructOfs)
{
    WRAPPER_CONTRACT;

    if (IsArgumentInRegister(&m_numRegistersUsed,
                                ELEMENT_TYPE_I,
                                sizeof(void*), FALSE,
                                m_pSig->GetCallingConvention(),
                                pRegStructOfs))
    {
        return m_regArgsOfs + *pRegStructOfs;
    }
    
    *pRegStructOfs = -1;

    return m_curOfs - StackElemSize(sizeof(void*));
}

//------------------------------------------------------------
// Same as GetNextArgAddr() but returns a byte offset from
// the Frame* pointer. This offset can be positive *or* negative.
//
// Returns 0 once you've hit the end of the list. Since the
// the offset is relative to the Frame* pointer itself, 0 can
// never point to a valid argument.
//------------------------------------------------------------
int ArgIterator::GetNextOffset(BYTE *pType, UINT32 *pStructSize, UINT *pRegStructOfs/* = NULL*/)
{
    WRAPPER_CONTRACT;

    int result = m_curOfs;

    if (m_curOfs != 0) 
    {
        BYTE typ = m_pSig->NextArgNormalized();
        *pType = typ;
        int offsetIntoArgumentRegisters;
        UINT32 structSize;

        if (typ == ELEMENT_TYPE_END) 
        {
            m_curOfs = result = 0;
        } 
        else 
        {
            structSize = m_pSig->GetLastTypeSize();
            *pStructSize = structSize;

            if (IsArgumentInRegister(&m_numRegistersUsed, typ, structSize, FALSE, m_pSig->GetCallingConvention(), &offsetIntoArgumentRegisters)) 
            {
                if (pRegStructOfs) 
                {
                    *pRegStructOfs = offsetIntoArgumentRegisters;
                }
                return m_regArgsOfs + offsetIntoArgumentRegisters;
            }
            else
            {
                if (pRegStructOfs) 
                {
                    *pRegStructOfs = (UINT)(-1);
                }

                UINT32 stackSize = StackElemSize(structSize);

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
                if (MetaSig::IsArgPassedByRef(stackSize))
                {
                    stackSize = STACK_ELEM_SIZE;
                }
#endif

#if !(defined(STACK_GROWS_DOWN_ON_ARGS_WALK)^defined(STACK_GROWS_UP_ON_ARGS_WALK))
#error One and only one between STACK_GROWS_DOWN_ON_ARGS_WALK and STACK_GROWS_UP_ON_ARGS_WALK must be defined!
#endif

#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
                m_curOfs -= stackSize;
#endif
                result = m_curOfs;
#ifdef STACK_GROWS_UP_ON_ARGS_WALK
                m_curOfs += stackSize;
#endif
            }
        }
    }

    return result;
}

//------------------------------------------------------------
// Each time this is called, this returns a pointer to the next
// argument. This pointer points directly into the caller's stack.
// Whether or not object arguments returned this way are gc-protected
// depends on the exact type of frame.
//
// Returns NULL once you've hit the end of the list.
//------------------------------------------------------------
LPVOID ArgIterator::GetNextArgAddr(BYTE *pType, UINT32 *pStructSize)
{
    WRAPPER_CONTRACT;

    int ofs = GetNextOffset(pType, pStructSize);
    if (ofs) {
        return ofs + m_pFrameBase;
    } else {
        return NULL;
    }
}

#ifndef DACCESS_COMPILE

//------------------------------------------------------------
// Returns the type of the last arg visited
//------------------------------------------------------------
TypeHandle ArgIterator::GetArgType()
{ 
    WRAPPER_CONTRACT;

    if (m_pSig->m_flags & MetaSig::SIG_OFFSETS_INITTED)
    {
        // 
        // Sync the sig walker with the arg number
        //

        if (-1 != m_argNum) 
        {
            m_pSig->Reset();
            for (int i=0; i<=m_argNum; i++)
                m_pSig->NextArg();
        }
    }

    return m_pSig->GetLastTypeHandleThrowing(); 
}

//------------------------------------------------------------------
// Returns true if m_pSig is currently in the varargs area
//     amd false is we are in the fixed args area
//------------------------------------------------------------------
bool ArgIterator::atSentinel()
{ 
    WRAPPER_CONTRACT;

    return (m_pSig->GetArgProps().AtSentinel());
}

HelperMethodFrame::HelperMethodFrame(struct MachState* ms, MethodDesc* pMD, ArgumentRegisters * regArgs)
{
    WRAPPER_CONTRACT;

    Init(GetThread(), ms, pMD, regArgs);
}

// This is for virtual call dispatch, in a case where we only know the size of the
// stack arguments. This works because we are guaranteed that the only time we will
// try to run managed code is if we're about to throw an exception, in which case
// the enregistered and stack arguments are going to be lost as soon as the frame is
// unwound.
HelperMethodFrame::HelperMethodFrame(tagWithRefData dummytag, struct MachState* ms,
                                     WORD wRefData, ArgumentRegisters *regArgs)
{
    WRAPPER_CONTRACT;
    _ASSERTE(dummytag == WithRefData);
    Init(GetThread(), ms, NULL, regArgs);
    m_wRefData = wRefData;
    m_Attribs |= FRAME_ATTR_NO_MD;
}

void HelperMethodFrame::LazyInit(void* FcallFtnEntry, struct LazyMachState* ms)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE(!ms->isValid());

    m_MachState = ms;
    m_FCallEntry = (TADDR)FcallFtnEntry;
    m_pThread = GetThread();
    Push(m_pThread);

//
// Let's speed up CHK & DBG builds
//
}

#endif // #ifndef DACCESS_COMPILE

MethodDesc* HelperMethodFrame::GetFunction()
{
    WRAPPER_CONTRACT;
    // m_Datum is used for stack argument size with lazy stub dispatch
    if (IsRefDataFrame())
        return NULL;
#ifndef DACCESS_COMPILE
    InsureInit(false, NULL);
    return PTR_MethodDesc(m_Datum);
#else
    if (m_MachState->isValid())
    {
        return PTR_MethodDesc(m_Datum);
    }
    else
    {
        return ECall::MapTargetBackToMethod(m_FCallEntry);
    }
#endif
}

MetaSig::RETURNTYPE HelperMethodFrame::ReturnsObject() 
{
    WRAPPER_CONTRACT;

    if (GetFunction() != 0)
        return(GetFunction()->ReturnsObject());
    
    unsigned attrib = GetFrameAttribs();
    if (attrib & FRAME_ATTR_RETURNOBJ)
        return(MetaSig::RETOBJ);
    if (attrib & FRAME_ATTR_RETURN_INTERIOR)
        return(MetaSig::RETBYREF);
    
    return(MetaSig::RETNONOBJ);
}

#ifndef DACCESS_COMPILE

void HelperMethodFrame::Init(Thread *pThread, struct MachState* ms, MethodDesc* pMD, ArgumentRegisters * regArgs)
{  
    WRAPPER_CONTRACT;

    m_Datum = (TADDR)pMD;
    m_Attribs = FRAME_ATTR_NONE;
    m_RegArgs = regArgs;
    m_pThread = pThread;
    m_FCallEntry = 0;
    
    m_MachState = ms;
    InsureInit(true, NULL);


    Push(pThread);
}

#endif // #ifndef DACCESS_COMPILE

TADDR HelperMethodFrame::InsureInit(bool initialInit,
                                    MachState* unwindState) 
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    } CONTRACTL_END;

    if (m_MachState->isValid())
        return PTR_TO_TADDR(m_MachState->_pRetAddr);

    _ASSERTE(m_Attribs != 0xCCCCCCCC);

    // If we're a stub dispatch frame, then we should be fully init'd
    CONSISTENCY_CHECK(!IsRefDataFrame());

#ifndef DACCESS_COMPILE
    if (!initialInit)
    {
        m_Datum = (TADDR)ECall::MapTargetBackToMethod(m_FCallEntry);

        // if this is an FCall, we should find it
        _ASSERTE(m_FCallEntry == 0 || m_Datum != 0);

        m_RegArgs = 0;
    }
#endif

    // because TRUE FCalls can be called from via reflection, com-interop etc.
    // we cant rely on the fact that we are called from jitted code to find the
    // caller of the FCALL.   Thus FCalls must erect the frame directly in the
    // FCall.  For JIT helpers, however, we can rely on this, and so they can
    // be sneaker and defer the HelperMethodFrame setup to a helper etc
   
    // Work with a copy so that we only write the values once.
    // this avoids race conditions.
    LazyMachState* lazy = PTR_LazyMachState(PTR_TO_TADDR(m_MachState));
    MachState unwound;
    TADDR retAddr;
    
    if (!initialInit &&
        m_FCallEntry == 0 &&
        !(m_Attribs & Frame::FRAME_ATTR_EXACT_DEPTH)) // Jit Helper
    {
        retAddr = LazyMachState::unwindLazyState(lazy, &unwound, 4,
                            (MachState::TestFtn)
                            ExecutionManager::FindJitManPCOnly);
    }
    else if (!initialInit &&
             (m_Attribs & Frame::FRAME_ATTR_CAPTURE_DEPTH_2) != 0)
    {
        // explictly told depth
        retAddr = LazyMachState::unwindLazyState(lazy, &unwound, 2);
    }
    else
    {
        // True FCall 
        retAddr = LazyMachState::unwindLazyState(lazy, &unwound, 1);
    }

#if !defined(DACCESS_COMPILE)
    lazy->setLazyStateFromUnwind(&unwound, retAddr);
#else  // DACCESS_COMPILE
    if (unwindState)
    {
        *unwindState = unwound;
    }
#endif

#ifdef _DEBUG
    //bool fRetAddrPtrCanBeNull = false;

    /* _ASSERTE( (m_Attribs & FRAME_ATTR_POSSIBLE_MD)          ||
              (fRetAddrPtrCanBeNull && (retAddr == NULL)) ||
              isLegalManagedCodeCaller(*PTR_TADDR(retAddr)) ); */
#endif // _DEBUG

    return retAddr;
}

#ifndef DACCESS_COMPILE

void HelperMethodFrame::GcScanRoots(promote_func *fn, ScanContext* sc) 
{
    WRAPPER_CONTRACT;

    _ASSERTE(m_MachState->isValid());       // we have calle InsureInit


    // Note that if we don't have a MD or registe args, then do dont to GC promotion of args
    if (!IsRefDataFrame() && (GetFunction() == 0 || m_RegArgs == 0))
        return;

#ifdef _X86_
    GCCONTEXT ctx;
    ctx.f = fn;
    ctx.sc = sc;
    ctx.cf = NULL;

    BYTE* framePtr      = (BYTE*) m_MachState->pRetAddr();
    int   stackArgsOffs = sizeof(void*);                   // the arguments start right above the return address
    int   regArgsOffs   = ((BYTE*) m_RegArgs) - framePtr;   // convert reg args pointer to offset

    if (IsRefDataFrame())
    {
        NativeDelayFixupObjRefData refData(m_wRefData);
        refData.PromoteArgs(framePtr, &ctx, stackArgsOffs, regArgsOffs);
        return;
    }

    MethodDesc* pMD = GetFunction();
    CONSISTENCY_CHECK(CheckPointer(pMD));
    MetaSig msig(pMD);
    if (msig.HasThis())
    {
        DoPromote(fn, sc, (OBJECTREF *) &m_RegArgs->THIS_REG,
                  pMD->GetMethodTable()->IsValueType());
    }

    if (msig.HasRetBuffArg())
    {
        INT32 * pRetBuffArg = msig.HasThis() ? &m_RegArgs->ARGUMENT_REG2 : &m_RegArgs->ARGUMENT_REG1;
        DoPromote(fn, sc, (OBJECTREF *) pRetBuffArg, TRUE);
    }

    if (msig.GetCallingConvention() == IMAGE_CEE_CS_CALLCONV_VARARG)
    {
        // For varargs, set up msig using the varArgSig token passed on the stack
        VASigCookie* varArgSig = *((VASigCookie**) (framePtr + stackArgsOffs));

        MetaSig varArgMSig(varArgSig->mdVASig, 
                           SigParser::LengthOfSig(varArgSig->mdVASig),
                           varArgSig->pModule, 
                           NULL, 
                           NULL);
        msig = &varArgMSig;
    }

    promoteArgs(framePtr, &msig, &ctx, 
                stackArgsOffs, regArgsOffs, false);
#else
    PORTABILITY_ASSERT(!"NYI HelperMethodFrame::GcScanRoots");
#endif // _X86_

}


#ifdef _DEBUG

BOOL IsValidHelperMethodCaller (LPCVOID pvReturnAddress)
{
    if (ExecutionManager::FindJitMan((BYTE*)pvReturnAddress))
        return TRUE;

// ROTORTODO : find out if this is necessary and implement it if it is
    return FALSE;
}

#endif // _DEBUG

#endif // #ifndef DACCESS_COMPILE



#include "comdelegate.h"

Assembly* SecureDelegateFrame::GetAssembly()
{
    WRAPPER_CONTRACT;

#if !defined(DACCESS_COMPILE)
    // obtain the frame off the delegate pointer
    DELEGATEREF delegate = (DELEGATEREF) GetThis();
    _ASSERTE(delegate);
    Assembly* pAssembly = (Assembly*) delegate->GetMethodPtrAux();
    _ASSERTE(pAssembly != NULL);
    return pAssembly;
#else
    DacNotImpl();
    return NULL;
#endif
}

BOOL SecureDelegateFrame::TraceFrame(Thread *thread, BOOL fromPatch, TraceDestination *trace, REGDISPLAY *regs)
{
    WRAPPER_CONTRACT;

    _ASSERTE(!fromPatch);

    // Unlike multicast delegates, secure delegates only call one method.  So, we should just return false here
    // and let the step out logic continue to the caller of the secure delegate stub.
    LOG((LF_CORDB, LL_INFO1000, "SDF::TF: return FALSE\n"));

    return FALSE;
}

BOOL MulticastFrame::TraceFrame(Thread *thread, BOOL fromPatch,
                                TraceDestination *trace, REGDISPLAY *regs)
{
    WRAPPER_CONTRACT;

    _ASSERTE(!fromPatch);

#if defined(DACCESS_COMPILE)
    return FALSE;

#else // !DACCESS_COMPILE
    LOG((LF_CORDB,LL_INFO10000, "MulticastFrame::TF FromPatch:0x%x, at 0x%x\n", fromPatch, GetControlPC(regs)));

    // At this point we have no way to recover the Stub object from the control pc.  We can't use the MD stored
    // in the MulticastFrame because it points to the dummy Invoke() method, not the method we want to call.

    BYTE *pbDel = NULL;
    int delegateCount = 0;

#if defined(_X86_)
    // At this point the counter hasn't been incremented yet.
    delegateCount = *regs->pEdi + 1;
    pbDel = *(BYTE **)( (size_t)*(regs->pEsi) + MulticastFrame::GetOffsetOfThis());
#elif defined(_PPC_)
    // At this point the counter hasn't been incremented yet.
    delegateCount = *regs->pR[29-13] + 1;
    pbDel = *(BYTE**)GetAddrOfThis();
#else
    delegateCount = 0;
    PORTABILITY_ASSERT("MulticastFrame::TraceFrame (frames.cpp)");
#endif

    int totalDelegateCount = (int)*(size_t*)(pbDel + DelegateObject::GetOffsetOfInvocationCount());

    _ASSERTE( COMDelegate::IsTrueMulticastDelegate( ObjectToOBJECTREF((Object*)pbDel) ) );


    if (delegateCount == totalDelegateCount)
    {
        LOG((LF_CORDB, LL_INFO1000, "MF::TF: Executed all stubs, should return\n"));
        // We've executed all the stubs, so we should return
        return FALSE;
    }
    else
    {
        // We're going to execute stub delegateCount next, so go and grab it.
        BYTE *pbDelInvocationList = *(BYTE **)(pbDel + DelegateObject::GetOffsetOfInvocationList());

        pbDel = *(BYTE**)( ((ArrayBase *)pbDelInvocationList)->GetDataPtr() +
                           ((ArrayBase *)pbDelInvocationList)->GetComponentSize()*delegateCount);

        _ASSERTE(pbDel);
        return DelegateInvokeStubManager::TraceDelegateObject(pbDel, trace);
    }
#endif // !DACCESS_COMPILE
}

void InlinedCallFrame::GetEEInfo(CORINFO_EE_INFO * pEEInfo)
{
    CORINFO_EE_INFO::InlinedCallFrameInfo * pInfo = &pEEInfo->inlinedCallFrameInfo;
    
    pInfo->size                          = sizeof(GSCookie) + sizeof(InlinedCallFrame);
    
    pInfo->offsetOfGSCookie              = 0;
    pInfo->offsetOfFrameVptr             = sizeof(GSCookie);
    pInfo->offsetOfFrameLink             = sizeof(GSCookie) + Frame::GetOffsetOfNextLink();
    pInfo->offsetOfCallSiteSP            = sizeof(GSCookie) + offsetof(InlinedCallFrame, m_pCallSiteSP);
    pInfo->offsetOfCalleeSavedRegisters  = sizeof(GSCookie) + offsetof(InlinedCallFrame, m_pCalleeSavedRegisters);
#ifdef _X86_    
    pInfo->offsetOfCalleeSavedEbp        = pInfo->offsetOfCalleeSavedRegisters + offsetof(CalleeSavedRegisters, ebp);
#endif
    pInfo->offsetOfCallTarget            = sizeof(GSCookie) + offsetof(InlinedCallFrame, m_Datum);
    pInfo->offsetOfReturnAddress         = sizeof(GSCookie) + offsetof(InlinedCallFrame, m_pCallerReturnAddress);
}

#ifndef DACCESS_COMPILE

void UnmanagedToManagedCallFrame::GcScanRootsWorker(promote_func *fn, ScanContext* sc)
{
    WRAPPER_CONTRACT;

    if (GetCleanupWorkList())
        GetCleanupWorkList()->GcScanRoots(fn, sc);

    // don't need to worry about the object moving as it is stored in a weak handle
    // but do need to report it so it doesn't get collected if the only reference to
    // it is in this frame. So only do something if are in promotion phase. And if are
    // in reloc phase this could cause invalid refs as the object may have been moved.
    if (! sc->promotion)
        return;

    Context *returnContext = GetReturnContext();
    if (returnContext)
    {
        _ASSERTE(returnContext->GetDomain());    // this will make sure is a valid pointer

        // We are in the middle of the GC. OBJECTREFs can't be used here since their built-in validation
        // chokes on objects that have been relocated already
        Object *pRef = returnContext->GetExposedObjectRawUnchecked();
        if (pRef == NULL)
            return;

        LOG((LF_GC, INFO3, "UnmanagedToManagedCallFrame Protection Frame Promoting" FMT_ADDR "to ", DBG_ADDR(pRef) ));
        (*fn)(pRef, sc, CHECK_APP_DOMAIN);
        LOG((LF_GC, INFO3, FMT_ADDR "\n", DBG_ADDR(pRef) ));
    }
}

void ContextTransitionFrame::GcScanRoots(promote_func *fn, ScanContext* sc)
{
    WRAPPER_CONTRACT;

    // Don't check app domains here - m_ReturnExecutionContext is in the parent frame's app domain
    (*fn) (m_ReturnExecutionContext, sc, 0);
    LOG((LF_GC, INFO3, "    " FMT_ADDR "\n", DBG_ADDR(m_ReturnExecutionContext) ));

    // Don't check app domains here - m_LastThrownObjectInParentContext is in the parent frame's app domain
    (*fn) (m_LastThrownObjectInParentContext, sc, 0);    
    LOG((LF_GC, INFO3, "    " FMT_ADDR "\n", DBG_ADDR(m_LastThrownObjectInParentContext) ));
    
    // If the context transition frame owns an unmanaged to managed frame, we need
    // to scan it's roots as well.
    if (m_pOwnedUMCallFrame)
        m_pOwnedUMCallFrame->GcScanRootsFromCtxTransFrame(fn, sc);
    
    // don't need to worry about the object moving as it is stored in a weak handle
    // but do need to report it so it doesn't get collected if the only reference to
    // it is in this frame. So only do something if are in promotion phase. And if are
    // in reloc phase this could cause invalid refs as the object may have been moved.
    if (! sc->promotion)
        return;

    Context *returnContext = GetReturnContext();
    PREFIX_ASSUME(returnContext != NULL);
    _ASSERTE(returnContext);
    _ASSERTE(returnContext->GetDomain());    // this will make sure is a valid pointer

    // We are in the middle of the GC. OBJECTREFs can't be used here since their built-in validation
    // chokes on objects that have been relocated already
    Object *pRef = returnContext->GetExposedObjectRawUnchecked();
    if (pRef == NULL)
        return;

    LOG((LF_GC, INFO3, "ContextTransitionFrame Protection Frame Promoting" FMT_ADDR "to ", DBG_ADDR(pRef) ));
    // Don't check app domains here - the objects are in the parent frame's app domain

    (*fn)(pRef, sc, 0);
    LOG((LF_GC, INFO3, FMT_ADDR "\n", DBG_ADDR(pRef) ));    
}

void UnmanagedToManagedCallFrame::ExceptionUnwind()
{
    WRAPPER_CONTRACT;

    UnmanagedToManagedFrame::ExceptionUnwind();
    GetCleanupWorkList()->Cleanup(TRUE);
    AppDomain::ExceptionUnwind(this);
}

#endif // #ifndef DACCESS_COMPILE

UMEntryThunk *UMThkCallFrame::GetUMEntryThunk()
{
    WRAPPER_CONTRACT;
    return PTR_UMEntryThunk(GetDatum());
}


#ifndef DACCESS_COMPILE




/*static*/ TADDR UMThkCallFrame::GetUMThkCallFrameVPtr()
{
    LEAF_CONTRACT;

    RETURNFRAMEVPTR(UMThkCallFrame);
}

/* report all the remaining args in 'msig' (but not THIS if present), to the GC. 
   'framePtr' points at the frame (promote doesn't assume anthing about its structure)
   'msig' describes the arguments 
   'ctx' has the GC reporting info
   'stackArgsOffs' is the byte offset from 'framePtr' where the arguments start
   'regArgsOffs' is the offset to find the register args to promote,
   'skipFixedArgs' is true if we only want to report the varargs
      on the x86 target we should pass false for skipFixedArgs
      for the _WIN64 targets we should pass true for skipFixedArgs
*/
void promoteArgs(BYTE* framePtr, MetaSig* msig, GCCONTEXT* ctx, 
                 int stackArgsOffs, int regArgsOffs, bool skipFixedArgs) 
{
    WRAPPER_CONTRACT;

    ArgIterator argit(framePtr, msig, stackArgsOffs, regArgsOffs);

    LPVOID  pArgAddr    = argit.GetNextArgAddr();
    bool inVarArgs = false;
    while (NULL != pArgAddr)
    {
        if (argit.atSentinel())
            inVarArgs = true;

        // if skipFixedArgs is false we report all arguments
        //  otherwise we just report the varargs.
        if (!skipFixedArgs || inVarArgs)
            msig->GcScanRoots(pArgAddr, ctx->f, ctx->sc);

        pArgAddr   = argit.GetNextArgAddr();
    }
}

#endif // #ifndef DACCESS_COMPILE

