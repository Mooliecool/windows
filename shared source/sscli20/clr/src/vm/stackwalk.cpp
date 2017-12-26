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
/*  STACKWALK.CPP:
 *
 */

#include "common.h"
#include "frames.h"
#include "threads.h"
#include "threads.inl"
#include "stackwalk.h"
#include "excep.h"
#include "eetwain.h"
#include "codeman.h"
#include "eeconfig.h"
#include "stackprobe.h"
#include "dbginterface.h"
#include "generics.h"


#ifdef _DEBUG
void* forceFrame;   // Variable used to force a local variable to the frame
#endif

CrawlFrame::CrawlFrame()
{
    LEAF_CONTRACT;
    pCurGSCookie = NULL;
    pFirstGSCookie = NULL;
    isCachedMethod = FALSE;
}

Assembly* CrawlFrame::GetAssembly()
{
    WRAPPER_CONTRACT;
    Frame *pF = GetFrame();
    if (pF != NULL)
        return pF->GetAssembly();
    else if (pFunc != NULL)
        return pFunc->GetModule()->GetAssembly();
    else
        return NULL;
}

MetaSig::RETURNTYPE CrawlFrame::ReturnsObject()
{
    CONTRACTL {
#ifdef _DEBUG
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
#else //_DEBUG
        NOTHROW;
#endif //_DEBUG        
        GC_NOTRIGGER;
    } CONTRACTL_END;

    if (isFrameless)
        return pFunc->ReturnsObject();
    return pFrame->ReturnsObject();
}

OBJECTREF* CrawlFrame::GetAddrOfSecurityObject()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    if (isFrameless)
    {
        _ASSERTE(pFunc);

#if defined(_X86_)
        if (isCachedMethod)
        {
            return pSecurityObject;
        }
        else
#endif // _X86_
        {
            return (static_cast <OBJECTREF*>(codeMgrInstance->GetAddrOfSecurityObject(this)));
        }
    }
    else
    {
        /*ISSUE: Are there any other functions holding a security desc? */
        if (pFunc && (pFunc->IsIL() || pFunc->IsNoMetadata()))
                return static_cast<FramedMethodFrame*>
                    (pFrame)->GetAddrOfSecurityDesc();
    }
    return NULL;
}

LPVOID CrawlFrame::GetInfoBlock()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(isFrameless);
    _ASSERTE(JitManagerInstance && methodToken);
    return JitManagerInstance->GetGCInfo(methodToken);
}

unsigned CrawlFrame::GetOffsetInFunction()
{
    _ASSERTE(!"NYI");
    return 0;
}

OBJECTREF CrawlFrame::GetObject()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        if (!IsGCSpecialThread()) {MODE_COOPERATIVE;} else {MODE_ANY;}
    } CONTRACTL_END;

    if (!pFunc || pFunc->IsStatic() || pFunc->GetClass()->IsValueClass())
        return NULL;


    if (isFrameless)
    {
        EECodeInfo codeInfo(methodToken, JitManagerInstance);
        return codeMgrInstance->GetInstance(pRD,
                                            JitManagerInstance->GetGCInfo(methodToken),
                                            &codeInfo,
                                            relOffset);
    }
    else
    {
        _ASSERTE(pFrame);
        _ASSERTE(pFunc);
        /*ISSUE: we already know that we have (at least) a method */
        /*       might need adjustment as soon as we solved the
                 jit-helper frame question
        */

        return ((FramedMethodFrame*)pFrame)->GetThis();
    }
}


#ifdef _X86_
//-----------------------------------------------------------------------------
// Get the "Ambient SP" from a  CrawlFrame.
// This will be null if there is no Ambient SP (eg, in the prolog / epilog, 
// or on certain platforms),
//-----------------------------------------------------------------------------
TADDR CrawlFrame::GetAmbientSPFromCrawlFrame()
{
#ifdef DACCESS_COMPILE
    DacNotImpl();
    return NULL;
#else
    // Usage based off Excepx86.cpp's COMPlusUnwindCallback 
    EHContext context; 

    REGDISPLAY * pReg = this->GetRegisterSet();
    context.Setup(GetControlPC(pReg), pReg);
    
    EECodeInfo codeInfo(this->GetMethodToken(), this->GetJitManager()); 
    
    // we set nesting level to zero because it won't be used for esp-framed methods, 
    // and zero is at least valid for ebp based methods (where we won't use the ambient esp anyways) 
    DWORD nestingLevel = 0; 
    return this->GetCodeManager()->GetAmbientSP(
        &context, 
        this->GetInfoBlock(), 
        nestingLevel, 
        &codeInfo, 
        this->GetCodeManState() 
        ); 
#endif // DACCESS_COMPILE
}
#endif


void *CrawlFrame::GetParamTypeArg()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    if (!pFunc || !pFunc->RequiresInstArg())
    {
        return NULL;
    }


    if (isFrameless)
    {
        EECodeInfo codeInfo(methodToken, JitManagerInstance);
        return codeMgrInstance->GetParamTypeArg(pRD,
                                                JitManagerInstance->GetGCInfo(methodToken),
                                                &codeInfo,
                                                relOffset);
    }
    else
    {
        _ASSERTE(pFrame);
        _ASSERTE(pFunc);
        return ((FramedMethodFrame*)pFrame)->GetParamTypeArg();
    }
}



// [pClassInstantiation] : Always filled in, though may be set to NULL if no inst.
// [pMethodInst] : Always filled in, though may be set to NULL if no inst.
void CrawlFrame::GetExactGenericInstantiations(TypeHandle **pClassInst,
                                               TypeHandle **pMethodInst)
{

    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pClassInst));
        PRECONDITION(CheckPointer(pMethodInst));
    } CONTRACTL_END;

    TypeHandle specificClass;
    MethodDesc* specificMethod;

    BOOL ret = Generics::GetExactInstantiationsFromCallInformation(GetFunction(),
                                                        GetExactGenericArgsToken(),
                                                        &specificClass,
                                                        &specificMethod
                                                       );

    if (!ret)
    {
        _ASSERTE(!"Cannot return exact class instantiation when we are requested to.");
    }

    *pClassInst = specificMethod->GetExactClassInstantiation(specificClass);     
    *pMethodInst = specificMethod->GetMethodInstantiation();
}

void *CrawlFrame::GetExactGenericArgsToken()
{

    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    MethodDesc* pFunc = GetFunction();

    if (!pFunc || !pFunc->IsSharedByGenericInstantiations())
        return NULL;

    if (pFunc->AcquiresInstMethodTableFromThis())
    {
        OBJECTREF obj = GetObject();
        if (obj == NULL)
            return NULL;
        return (void *) obj->GetMethodTable();
    }
    else
    {
        _ASSERTE(pFunc->RequiresInstArg());
        return (void *) GetParamTypeArg();
    }
}

    /* Is this frame at a safe spot for GC?
     */
bool CrawlFrame::IsGcSafe()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(codeMgrInstance);
    EECodeInfo codeInfo(methodToken, JitManagerInstance);
    return codeMgrInstance->IsGcSafe(pRD,
                                     JitManagerInstance->GetGCInfo(methodToken),
                                     &codeInfo,
                                     0);
}

inline void CrawlFrame::GotoNextFrame()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    //
    // Update app domain if this frame caused a transition
    //

    AppDomain *pRetDomain = pFrame->GetReturnDomain();
    if (pRetDomain != NULL)
        pAppDomain = pRetDomain;
    pFrame = pFrame->Next();
    
#ifndef DACCESS_COMPILE
    if (pFrame != FRAME_TOP)
        SetCurGSCookie(Frame::SafeGetGSCookiePtr(pFrame));
#endif // DACCESS_COMPILE
}

//******************************************************************************
#ifndef DACCESS_COMPILE

// For asynchronous stackwalks, the thread being walked may not be suspended.
// It could cause a buffer-overrun while the stack-walk is in progress.
// To detect this, we can only use data that is guarded by a GSCookie
// that has been recently checked.
// This function should be called after doing any time-consuming activity
// during stack-walking to reduce the window in which a buffer-overrun
// could cause an problems.
//
// To keep things simple, we do this checking even for synchronous stack-walks.
void CrawlFrame::CheckGSCookies()
{
    WRAPPER_CONTRACT;

    if (pFirstGSCookie == NULL)
        return;
    
    if (*pFirstGSCookie != GetProcessGSCookie())
        DoJITFailFast();
    
    if(*pCurGSCookie   != GetProcessGSCookie())
        DoJITFailFast();
}

void CrawlFrame::SetCurGSCookie(GSCookie * pGSCookie)
{
    WRAPPER_CONTRACT;

#ifdef _X86_
    // The thread may be interrupted before the GSCookie has been set in the Frame
    // So just disable GS-checking when stackwalk is due to DoStackSnapshot.
    //
    // The real fix is to change the stubs to publish the Frame only after the
    // GSCookie has been initialized. This is currently a problem on x86 because
    // StubLinkerCPU::EmitMethodStubProlog() publishes the Frame, but the caller
    // sets the GSCookie
    if (isProfilerDoStackSnapshot)
    {
        _ASSERTE(CORProfilerStackSnapshotEnabled());
        _ASSERTE(pFirstGSCookie == NULL && pCurGSCookie == NULL);
        return;
    }
#endif
    
    if (pGSCookie == NULL)
        DoJITFailFast();
    
    pCurGSCookie = pGSCookie;
    if (pFirstGSCookie == NULL)
        pFirstGSCookie = pGSCookie;
    
    CheckGSCookies();
}

#endif // DACCESS_COMPILE


//******************************************************************************
#if defined(ELIMINATE_FEF)
//******************************************************************************
// Advance to the next ExInfo.  Typically done when an ExInfo has been used and
//  should not be used again.
//******************************************************************************
void ExInfoWalker::WalkOne()
{
    if (m_pExInfo)
    {
        LOG((LF_EH, LL_INFO10000, "ExInfoWalker::WalkOne: advancing ExInfo chain: pExInfo:%p, pContext:%p; prev:%p, pContext:%p\n",
              m_pExInfo, m_pExInfo->m_pContext, m_pExInfo->m_pPrevNestedInfo, m_pExInfo->m_pPrevNestedInfo?m_pExInfo->m_pPrevNestedInfo->m_pContext:0));
        m_pExInfo = m_pExInfo->m_pPrevNestedInfo;
    }
} // void ExInfoWalker::WalkOne()

//******************************************************************************
// Attempt to find an ExInfo with a pContext that is higher (older) than
//  a given minimum location.  (It is the pContext's SP that is relevant.)
//******************************************************************************
void ExInfoWalker::WalkToPosition(
    void        *pMinimum,                  // Starting point of stack walk.
    BOOL        bPopFrames)                 // If true, ResetUseExInfoForStackwalk on each exinfo.
{
    while (m_pExInfo &&
           ((GetSPFromContext() < (TADDR)pMinimum) ||
            (GetSPFromContext() == NULL)) )
    {
        // Try the next ExInfo, if there is one.
        LOG((LF_EH, LL_INFO10000, "ExInfoWalker::WalkToPosition: searching ExInfo chain: m_pExInfo:%p, pContext:%p; prev:%p, pContext:%p; pStartFrame:%p\n",
              m_pExInfo, m_pExInfo->m_pContext, m_pExInfo->m_pPrevNestedInfo, m_pExInfo->m_pPrevNestedInfo?m_pExInfo->m_pPrevNestedInfo->m_pContext:0, pMinimum));
        if (bPopFrames)
        {   // If caller asked for it, reset the bit which indicates that this ExInfo marks a fault from managed code.
            //  This is done so that the fault can be effectively "unwound" from the stack, similarly to how Frames
            //  are unlinked from the Frame chain.
            m_pExInfo->m_ExceptionFlags.ResetUseExInfoForStackwalk();
        }
        m_pExInfo = m_pExInfo->m_pPrevNestedInfo;
    }
    // At this point, m_pExInfo is NULL, or points to a pContext that is greater than pMinimum.
} // void ExInfoWalker::WalkToPosition()

//******************************************************************************
// Attempt to find an ExInfo with a pContext that has an IP in managed code.
//******************************************************************************
void ExInfoWalker::WalkToManaged()
{
    while (m_pExInfo)
    {
        // See if the current ExInfo has a CONTEXT that "returns" to managed code, and, if so, exit the loop.
        if (m_pExInfo->m_ExceptionFlags.UseExInfoForStackwalk() && GetContext() && ExecutionManager::FindJitMan((PBYTE)GetIP(GetContext())))
        {
                break;
        }
        // No, so skip to next, if any.
        LOG((LF_EH, LL_INFO1000, "ExInfoWalker::WalkToManaged: searching for ExInfo->managed: m_pExInfo:%p, pContext:%p, sp:%p; prev:%p, pContext:%p\n",
              m_pExInfo, GetContext(), GetSPFromContext(), m_pExInfo->m_pPrevNestedInfo, m_pExInfo->m_pPrevNestedInfo?m_pExInfo->m_pPrevNestedInfo->m_pContext:0));
        m_pExInfo = m_pExInfo->m_pPrevNestedInfo;
    }
    // At this point, m_pExInfo is NULL, or points to a pContext that has an IP in managed code.
} // void ExInfoWalker::WalkToManaged()
#endif // defined(ELIMINATE_FEF)


IJitManager* Thread::FillInCrawlFrameFromControlPC(CrawlFrame* pCF, PBYTE ControlPC)
{
    IJitManager* pEEJM = ExecutionManager::FindJitMan(ControlPC, IJitManager::GetScanFlags());
    
    pCF->JitManagerInstance = pEEJM;
    pCF->codeMgrInstance    = NULL;
    pCF->isFrameless        = (pEEJM != NULL);
    
    if (pCF->isFrameless)
    {
        pCF->codeMgrInstance = pEEJM->GetCodeManager();
    }

    return pEEJM;
}

StackWalkAction Thread::HandleSpecialFramelessStub(
    CrawlFrame* pCF, 
    PSTACKWALKFRAMESCALLBACK pCallback, 
    VOID* pData, 
    UINT flags,
    bool fMakeCallbacks
    DEBUG_ARG(UINT32 uLoopIteration))
{

    return SWA_CONTINUE;
}


#ifndef DACCESS_COMPILE


#if !defined(DACCESS_COMPILE) && !defined(_WIN64)
#define STACKWALKER_MAY_POP_FRAMES
#endif

#ifdef _DEBUG
void Thread::DebugLogStackWalkInfo(CrawlFrame* pCF, __in_z LPSTR pszTag, UINT32 uLoopIteration)
{
    if (pCF->isFrameless)
    {
        LPSTR pszType = "";

        if (pCF->pFunc->IsNoMetadata())
        {
            pszType = "[no metadata]";
        }
        
        LOG((LF_GCROOTS, LL_INFO10000, "STACKWALK: [%02x] %s: FRAMELESS: PC="FMT_ADDR" SP="FMT_ADDR" method=%s %s\n",
                uLoopIteration,
                pszTag,
                DBG_ADDR(GetControlPC(pCF->pRD)), 
                DBG_ADDR(GetRegdisplaySP(pCF->pRD)),
                pCF->pFunc->m_pszDebugMethodName, 
                pszType));
    }
    else
    {
        LOG((LF_GCROOTS, LL_INFO10000, "STACKWALK: [%02x] %s: EXPLICIT : PC=" FMT_ADDR " SP=" FMT_ADDR " Frame="FMT_ADDR" vtbl=" FMT_ADDR "\n",
            uLoopIteration,
            pszTag,
            DBG_ADDR(GetControlPC(pCF->pRD)), 
            DBG_ADDR(GetRegdisplaySP(pCF->pRD)), 
            DBG_ADDR(pCF->pFrame),
            DBG_ADDR((pCF->pFrame != FRAME_TOP) ? pCF->pFrame->GetVTablePtr() : NULL)));
    }
}
#endif // _DEBUG

StackWalkAction Thread::MakeStackwalkerCallback(
    CrawlFrame* pCF, 
    PSTACKWALKFRAMESCALLBACK pCallback, 
    VOID* pData 
    DEBUG_ARG(UINT32 uLoopIteration))
{
#ifdef _DEBUG    
    DebugLogStackWalkInfo(pCF, "CALLBACK", uLoopIteration);
#endif // _DEBUG

    // Since we may be asynchronously walking another thread's stack,
    // check (frequently) for stack-buffer-overrun corruptions
    pCF->CheckGSCookies();

    StackWalkAction swa = pCallback(pCF, (VOID*)pData);

    pCF->CheckGSCookies();

#ifdef _DEBUG
    if (swa == SWA_ABORT)
    {
        LOG((LF_GCROOTS, LL_INFO10000, "STACKWALK: SWA_ABORT: callback aborted the stackwalk\n"));
    }
#endif // _DEBUG

    return swa;
}

StackWalkAction Thread::MakeExplicitFrameCallbacksForManagedFrame(
    CrawlFrame* pCF, 
    PSTACKWALKFRAMESCALLBACK pCallback, 
    VOID* pData, 
    UINT flags, 
    bool fUseCodeInfo,
    bool fMakeCallbacks
    DEBUG_ARG(UINT32 uLoopIteration))
{
    BOOL                    fHandleSkippedFrames = FALSE;
    LPVOID                  pvReferenceSP;

    StackWalkAction retval;
    
#ifdef _X86_
    fHandleSkippedFrames = (flags & HANDLESKIPPEDFRAMES); // can debugger handle skipped frames?
#endif // _X86_

    pvReferenceSP = GetRegdisplaySP(pCF->pRD);

    //
    // common case early-out
    //
    if ( !(pCF->pFrame != FRAME_TOP && (PVOID)pCF->pFrame < pvReferenceSP) )
    {
        return SWA_CONTINUE;
    }

    LOG((LF_GCROOTS, LL_EVERYTHING, "STACKWALK: MakeExplicitFrameCallbacksForManagedFrame\n"));

    // We might have skipped past some Frames
    // This happens with InlinedCallFrames and if we unwound
    // out of a finally in managed code or for ContextTransitionFrames that are
    // inserted into the managed call stack
    while (pCF->pFrame != FRAME_TOP && (PVOID)pCF->pFrame < pvReferenceSP)
    {
        if (fHandleSkippedFrames
            || InlinedCallFrame::FrameHasActiveCall(pCF->pFrame)
            )
        {
            pCF->GotoNextFrame();
#ifdef STACKWALKER_MAY_POP_FRAMES
            if (flags & POPFRAMES)
            {
                this->SetFrame(pCF->pFrame);
            }
#endif // STACKWALKER_MAY_POP_FRAMES
        }
        else
        {
            pCF->codeMgrInstance = NULL;
            pCF->isFrameless     = false;
            pCF->pFunc           = pCF->pFrame->GetFunction();

#ifdef _DEBUG
            DebugLogStackWalkInfo(pCF, "consider", uLoopIteration);
#endif // _DEBUG

            // process that frame
            // On WIN64, if we are skipping funclets, then we should not make the callback.
            if ( (pCF->pFunc || !(flags & FUNCTIONSONLY))
                  WIN64_ONLY( && fMakeCallbacks) )
            {
                END_FORBID_TYPELOAD();
                retval = MakeStackwalkerCallback(pCF, pCallback, pData DEBUG_ARG(uLoopIteration));
                BEGIN_FORBID_TYPELOAD();
                if (retval == SWA_ABORT)
                {
                    return SWA_ABORT;
                }
            }


#ifdef STACKWALKER_MAY_POP_FRAMES
            if (flags & POPFRAMES)
            {
                _ASSERTE(pCF->pFrame == GetFrame());

                // If we got here, the current frame chose not to handle the
                // exception. Give it a chance to do any termination work
                // before we pop it off.
                END_FORBID_TYPELOAD();
                pCF->pFrame->ExceptionUnwind();
                BEGIN_FORBID_TYPELOAD();

                // Pop off this frame and go on to the next one.
                pCF->GotoNextFrame();

                this->SetFrame(pCF->pFrame);
            }
            else
#endif // STACKWALKER_MAY_POP_FRAMES
            {
                // go to the next frame
                pCF->GotoNextFrame();
            }
        }
    }

    return SWA_CONTINUE;
}

StackWalkAction Thread::StackWalkFramesEx(
                    PREGDISPLAY pRD,        // virtual register set at crawl start
                    PSTACKWALKFRAMESCALLBACK pCallback,
                    VOID *pData,
                    unsigned flags,
                    Frame *pStartFrame
                )
{
    // Note: there are cases (i.e., exception handling) where we may never return from this function. This means
    // that any C++ destructors pushed in this function will never execute, and it means that this function can
    // never have a dynamic contract.
    STATIC_CONTRACT_WRAPPER;
    STATIC_CONTRACT_ENTRY_POINT;
    SCAN_IGNORE_THROW;            // see contract above
    SCAN_IGNORE_TRIGGER;          // see contract above

    _ASSERTE(pRD);
    _ASSERTE(pCallback);

    // When the LIGHTUNWIND flag is set, we use the stack walk cache.
    // On x64, accesses to the stack walk cache are synchronized by
    // a CrstStatic, which may need to call back into the host.
    _ASSERTE(CanThisThreadCallIntoHost() || (flags & LIGHTUNWIND) == 0);

    // when POPFRAMES we don't want to allow GC trigger.
    // The only method that guarantees this now is COMPlusUnwindCallback
#ifdef STACKWALKER_MAY_POP_FRAMES
    ASSERT(!(flags & POPFRAMES) || pCallback == (PSTACKWALKFRAMESCALLBACK) COMPlusUnwindCallback);
    ASSERT(!(flags & POPFRAMES) || pRD->pContextForUnwind != NULL);
#else // STACKWALKER_MAY_POP_FRAMES
    ASSERT(!(flags & POPFRAMES));
#endif // STACKWALKER_MAY_POP_FRAMES

    BEGIN_FORBID_TYPELOAD();


    IA64_ONLY(FlushRSE());


    CrawlFrame              cf;
    StackWalkAction         retVal               = SWA_FAILED;
    StackWalkAction         retValTmp            = SWA_FAILED;     // To cache helper call returns
    Frame*                  pInlinedFrame        = NULL;
    unsigned                unwindFlags          = (flags & QUICKUNWIND) ? 0 : UpdateAllRegs;
#ifdef _X86_
    BOOL                    fHandleSkippedFrames = FALSE;
#endif // _X86_
    IJitManager*            pEEJM                = ExecutionManager::FindJitMan((PBYTE)GetControlPC(pRD));
    IJitManager::ScanFlag   fJitManagerScanFlags = IJitManager::GetScanFlags();
    bool                    fMakeCallback        = true;
#ifdef _DEBUG
    UINT32                  uLoopIteration       = 0;
#endif // _DEBUG


#ifdef _X86_
    fHandleSkippedFrames = !(flags & HANDLESKIPPEDFRAMES); // can debugger handle skipped frames?
#endif // _X86_

    if (pStartFrame)
    {
        cf.pFrame = pStartFrame;
    }
    else
    {
        cf.pFrame = this->GetFrame();
    }

    cf.isProfilerDoStackSnapshot = !!(flags & PROFILER_DO_STACK_SNAPSHOT);
    if (cf.pFrame != FRAME_TOP)
        cf.SetCurGSCookie(Frame::SafeGetGSCookiePtr(cf.pFrame));

    // FRAME_TOP and NULL must be distinct values. This assert
    // will fire if someone changes this.
    C_ASSERT(FRAME_TOP != NULL);

#ifdef _DEBUG
    Frame* startFrame = cf.pFrame;
    int depth = 0;
    
    forceFrame = &depth;
    cf.pFunc = (MethodDesc*)POISONC;
#endif
    cf.pThread = this;
    cf.isFirst = true;
    cf.isInterrupted = false;
    cf.hasFaulted = false;
    cf.isIPadjusted = false;
    cf.isNativeMarker = false;

    cf.JitManagerInstance = pEEJM;
    cf.codeMgrInstance = NULL;
    cf.isFrameless = (pEEJM != NULL);
    if (cf.isFrameless)
    {
        cf.codeMgrInstance = pEEJM->GetCodeManager();
    }
    cf.pRD = pRD;
    cf.pAppDomain = GetDomain(INDEBUG(flags & PROFILER_DO_STACK_SNAPSHOT));

    cf.isCachedMethod = false;
    cf.pSecurityObject = NULL;

    LOG((LF_GCROOTS, LL_INFO10000, "STACKWALK: Begin stack walk with callback " FMT_ADDR "  SP=" FMT_ADDR "\n", DBG_ADDR(pCallback), DBG_ADDR(GetRegdisplaySP(cf.pRD))));

#if defined(ELIMINATE_FEF) //<><><><><><><><><><><><><><><><><><><><><><><><><><><><>
    // If this stack includes a fault-induced-transition from managed to unmanaged (eg,
    //  a null reference exception), it may be necessary to recover the stack walk
    //  from the exception's pContext, since there is no Frame encapsulating that
    //  kind of transition (there used to be a FaultingExceptionFrame, but no more).
    // The pContext is kept in the ExInfo structure, and there can be a chain of
    //  those structs, if there are nested exceptions.

#if defined(SIMULATE_FAULTINGEXCEPTIONFRAME)
    FrameWithCookie<FaultingExceptionFrame> fef;         // used to replay pContext into cf.pRD
#endif // defined(SIMULATE_FAULTINGEXCEPTIONFRAME)
    ExInfoWalker exInfoWalk;            // To walk the chain of ExInfos.
    exInfoWalk.Init(&(GetExceptionState()->m_currentExInfo));
    // Find the lowest (newest) pContext that is higher than the pStartFrame.
    exInfoWalk.WalkToPosition(pStartFrame, false /* don't reset UseExInfoForStackwalk*/);
#endif // ELIMINATE_FEF //<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

    for (;;)
    {
        // Since we may be asynchronously walking another thread's stack,
        // check (frequently) for stack-buffer-overrun corruptions.
        cf.CheckGSCookies();

#ifdef _DEBUG
        uLoopIteration++;
#endif // _DEBUG
        
#if defined(ELIMINATE_FEF) //<><><><><><><><><><><><><><><><><><><><><><><><><><><><>
        // Look for a fault from managed to unmanaged (ie, a fault in managed code causes
        //  a transition to the unmanaged fault handler), in which case we must
        //  simulate a frame, because no frame is created by the fault.
        if (!cf.isFrameless && exInfoWalk.GetExInfo())
        {
            // We are currently walking ("lost") in unmanaged code.  We can recover
            //  from a) the next Frame record, or b) an exception context.
            // Recover from the exception context if all of these are true:
            //  - it "returns" to managed code
            //  - if is lower (newer) than the next Frame record
            //  - the stack walk has not already passed by it
            //
            // The ExInfo walker is initialized to be higher than the pStartFrame, and
            //  as we unwind managed (frameless) functions, we keep eliminating any
            //  ExInfos that are passed in the stackwalk.
            //
            // So, here we need to find the next ExInfo that "returns" to managed code,
            //  and then choose the lower of that ExInfo and the next Frame.
            exInfoWalk.WalkToManaged();
            PTR_CONTEXT pContext = exInfoWalk.GetContext();
            TADDR pContextSP = exInfoWalk.GetSPFromContext();


            if (pContextSP)
            {
                LOG((LF_EH, LL_INFO10000, "STACKWALK: considering resync from pContext(%p), fault(%08X), sp(%p); pStartFrame(%p); cf.pFrame(%p), cf.SP(%p)\n",
                      pContext, exInfoWalk.GetFault(), pContextSP, pStartFrame, cf.pFrame, GetRegdisplaySP(cf.pRD)));

                // If the pContext is lower (newer) than the CrawlFrame's Frame*, try to use
                //  the pContext.
                // There are still a few cases in which a FaultingExceptionFrame is linked in.  If
                //  the next frame is one of them, we don't want to override it.  THIS IS PROBABLY BAD!!!
                if ( ((BYTE*)pContextSP < (BYTE*)cf.pFrame) &&
                     ((cf.GetFrame() == FRAME_TOP) || (cf.GetFrame()->GetVTablePtr() != FaultingExceptionFrame::GetFrameVtable()) ) )
                {
                    // Get the JitManager for the managed address.
                    pEEJM = ExecutionManager::FindJitMan((PBYTE)GetIP(pContext));

                    STRESS_LOG4(LF_EH, LL_INFO100, "STACKWALK: resync from pContext(%p); pStartFrame(%p), cf.pFrame(%p), cf.SP(%p)\n",
                          pContext, pStartFrame, cf.pFrame, GetRegdisplaySP(cf.pRD));

#if defined(SIMULATE_FAULTINGEXCEPTIONFRAME)
                    //<><><><><><><><><><><><><><><><><><><><><><><><><><>><><>
                    // Make a callback with a "fake" frame; fake in the sense
                    //  that it is not linked into the frame chain.
                    cf.pFunc = 0;   // Always NULL for FaultingExceptionFrames.
                    Frame *pSavedFrame = cf.pFrame;
                    // Initialize the FaultingExceptionFrame.  NOTE that it is NOT linked
                    //  into the frame chain.
                    fef.Init(pContext);
                    cf.pFrame = &fef;

                    // Do the callback, if we should call back for non-function frames.
                    if (cf.pFunc || !(flags&FUNCTIONSONLY))
                    {
                        END_FORBID_TYPELOAD();
                        retValTmp = MakeStackwalkerCallback(&cf, pCallback, pData DEBUG_ARG(uLoopIteration));
                        BEGIN_FORBID_TYPELOAD();
                        if (retValTmp == SWA_ABORT)
                        {
                            retVal = SWA_ABORT;
                            goto Cleanup;
                        }
                    }

                    // Restore CrawlFrame from fake callback.
                    cf.pFrame = pSavedFrame;
                    // End of Fake callback.
                    //<><><><><><><><><><><><><><><><><><><><><><><><><><>><><>
#endif // defined(SIMULATE_FAULTINGEXCEPTIONFRAME)

                    // Update the RegDisplay from the context info.
                    FillRegDisplay(cf.pRD, pContext);

                    // Now we know where we are, and it's "frameless", aka managed.
                    cf.isFrameless = true;

                    // Update the Jit Manager and Code Manager.
                    cf.JitManagerInstance = pEEJM;
                    cf.codeMgrInstance = pEEJM->GetCodeManager();

                    // Flags the same as from a FaultingExceptionFrame.
                    cf.isInterrupted = 1;
                    cf.hasFaulted = 1;
                    cf.isIPadjusted = 0;

#ifdef STACKWALKER_MAY_POP_FRAMES
                    // If Frames would be unlinked from the Frame chain, also reset the UseExInfoForStackwalk bit
                    //  on the ExInfo.
                    if (flags & POPFRAMES)
                    {
                        exInfoWalk.GetExInfo()->m_ExceptionFlags.ResetUseExInfoForStackwalk();
                    }
#endif // STACKWALKER_MAY_POP_FRAMES                    
                    // Done with this ExInfo.
                    exInfoWalk.WalkOne();
                }
            }
        }
#endif // ELIMINATE_FEF //<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        // If managed code, or more Frame chain, keep going.  Otherwise, break from loop.
        if (!cf.isFrameless && (cf.pFrame == FRAME_TOP))
        {
            break;
        }

        retVal = SWA_DONE;

        cf.codeManState.dwIsSet = 0;
#ifdef _DEBUG
        memset((void *)cf.codeManState.stateBuf, 0xCD,
                sizeof(cf.codeManState.stateBuf));
        depth++;
#endif

        if (cf.isFrameless)
        {
            //-----------------------------------------------------------------
            // This must be a JITed/managed native method. There is no explicit
            // frame.
            //-----------------------------------------------------------------

            cf.isCachedMethod = FALSE;
            if (cf.stackWalkCache.Enabled() && (flags & LIGHTUNWIND))
            {
                cf.isCachedMethod = cf.stackWalkCache.Lookup((UINT_PTR)GetControlPC(cf.pRD));
                _ASSERTE (cf.isCachedMethod != cf.stackWalkCache.IsEmpty ());

                cf.pSecurityObject = NULL;
#if defined(_X86_) && !defined(FJITONLY)
                if (cf.isCachedMethod && cf.stackWalkCache.m_CacheEntry.HasSecurityObject())
                {
                    // pCallback will use this to save time on GetAddrOfSecurityObject
                    StackwalkCacheUnwindInfo stackwalkCacheUnwindInfo(&cf.stackWalkCache.m_CacheEntry);
                    cf.pSecurityObject = EECodeManager::GetAddrOfSecurityObjectFromCachedInfo(
                                                cf.pRD, 
                                                &stackwalkCacheUnwindInfo);
                }
#endif // _X86_ && !FJITONLY
            }

            pEEJM->JitCodeToMethodInfo((BYTE*)GetControlPC(cf.pRD), 
                                       &(cf.pFunc), 
                                       &(cf.methodToken),
                                       (DWORD*)&(cf.relOffset), 
                                       fJitManagerScanFlags);


            
            EECodeInfo codeInfo(cf.methodToken, pEEJM, cf.pFunc);
            LPVOID methodInfo = pEEJM->GetGCInfo(cf.methodToken);

            GSCookie * pGSCookie = (GSCookie*)cf.codeMgrInstance->GetGSCookieAddr(
                                                                    cf.pRD,
                                                                    methodInfo,
                                                                    &codeInfo,
                                                                    cf.relOffset,
                                                                    &cf.codeManState);
            if (pGSCookie)
                cf.SetCurGSCookie(pGSCookie);

#ifdef _DEBUG
            DebugLogStackWalkInfo(&cf, "CONSIDER", uLoopIteration);
#endif // _DEBUG
            
            fMakeCallback = true;
            

            //
            // Stress testing for synchronized methods
            // Win64 has the JIT generate try/finallys to leave monitors
            //
#if defined(_DEBUG) && defined(_X86_) && !defined(FJITONLY) 
            // cf.GetObject() requires full unwind
            // In GC's relocate phase, objects is not verifiable
            if  (!(flags & (LIGHTUNWIND|QUICKUNWIND|ALLOW_INVALID_OBJECTS)) && 
                cf.pFunc->IsSynchronized() && !cf.pFunc->IsStatic() &&
                cf.codeMgrInstance->IsInSynchronizedRegion(cf.relOffset,
                    methodInfo, cf.GetCodeManagerFlags()))
            {
                OBJECTREF obj = cf.GetObject();

                _ASSERTE(obj != NULL);
                VALIDATEOBJECTREF(obj);

                _ASSERTE(obj->IsObjMonitorOwnedByThread(cf.pThread));
            }
#endif // _DEBUG && _X86_ && !FJITONLY

            if (fMakeCallback)
            {
                END_FORBID_TYPELOAD();
                retValTmp = MakeStackwalkerCallback(
                    &cf, 
                    pCallback, 
                    pData 
                    DEBUG_ARG(uLoopIteration));
                BEGIN_FORBID_TYPELOAD();
                if (retValTmp == SWA_ABORT)
                {
                    retVal = SWA_ABORT;
                    goto Cleanup;
                }
            }

            ////////////////////////////////////////////////////
            // update CrawlFrame state for next frame to process 
            ////////////////////////////////////////////////////
            
            // Now find out if we need to leave monitors

            //
            // win64 has the JIT generate try/finallys to leave monitors
            //
#ifdef STACKWALKER_MAY_POP_FRAMES
            if (flags & POPFRAMES)
            {
                if (cf.pFunc->IsSynchronized())
                {
                    MethodDesc    *pMD = cf.pFunc;
                    OBJECTREF      orUnwind = 0;

                    if (cf.codeMgrInstance->IsInSynchronizedRegion(cf.relOffset,
                        methodInfo, cf.GetCodeManagerFlags()))
                    {
                        if (pMD->IsStatic())
                        {
                            MethodTable *pMT = pMD->GetMethodTable();
                            orUnwind = pMT->GetManagedClassObject();
                        }
                        else
                        {
                            orUnwind = cf.codeMgrInstance->GetInstance(
                                                    cf.pRD,
                                                    methodInfo,
                                                    &codeInfo,
                                                    cf.relOffset);
                        }

                        _ASSERTE(orUnwind != NULL);
                        VALIDATEOBJECTREF(orUnwind);

                        _ASSERTE(!orUnwind->IsThunking());

                        if (orUnwind != NULL)
                        {
                            orUnwind->LeaveObjMonitorAtException();
                        }
                    }
                }
            }
#endif // STACKWALKER_MAY_POP_FRAMES

#if !defined(ELIMINATE_FEF)
            // FaultingExceptionFrame is special case where it gets
            // pushed on the stack after the frame is running
            _ASSERTE((cf.pFrame == FRAME_TOP) ||
                     (GetRegdisplaySP(cf.pRD) < (PVOID)cf.pFrame) ||
                           (cf.pFrame->GetVTablePtr() == FaultingExceptionFrame::GetMethodFrameVPtr()) ||
                           (cf.pFrame->GetVTablePtr() == ContextTransitionFrame::GetMethodFrameVPtr()));
#endif // !defined(ELIMINATE_FEF)

            // Get rid of the frame (actually, it isn't really popped)

            LOG((LF_GCROOTS, LL_EVERYTHING, "STACKWALK: about to unwind for '%s', SP:" FMT_ADDR ", IP:" FMT_ADDR "\n",
                 cf.pFunc->m_pszDebugMethodName, DBG_ADDR(GetRegdisplaySP(cf.pRD)), DBG_ADDR(GetControlPC(cf.pRD))));

#if !defined(FJITONLY)
            StackwalkCacheEntry *pCacheEntry = cf.GetStackwalkCacheEntry();
            if (pCacheEntry != NULL)
            {
                _ASSERTE(cf.stackWalkCache.Enabled() && (flags & LIGHTUNWIND));

                // lightened schema: take stack unwind info from stackwalk cache
                EECodeManager::QuickUnwindStackFrame(cf.pRD, pCacheEntry, EECodeManager::UnwindCurrentStackFrame);
            }
            else
#endif // FJITONLY
            {
                // non-optimized stack unwind schema, doesn't use StackwalkCache
                UINT_PTR curSP = (UINT_PTR)GetRegdisplaySP(cf.pRD);
                UINT_PTR curIP = (UINT_PTR)GetControlPC(cf.pRD);

                bool fInsertCacheEntry = cf.stackWalkCache.Enabled() && 
                                         (flags & LIGHTUNWIND) &&
                                         pGSCookie == NULL;

                StackwalkCacheUnwindInfo unwindInfo;                

                if (!cf.codeMgrInstance->UnwindStackFrame(
                                    cf.pRD,
                                    methodInfo,
                                    &codeInfo,
                                    unwindFlags
                                        | cf.GetCodeManagerFlags()
                                        | ((flags & PROFILER_DO_STACK_SNAPSHOT) ?
                                            SpeculativeStackwalk :
                                            0),
                                    &cf.codeManState,
                                    (fInsertCacheEntry ? &unwindInfo : NULL)))
                {
                    LOG((LF_CORPROF, LL_INFO100, "**PROF: cf.codeMgrInstance->UnwindStackFrame failure leads to SWA_FAILED.\n"));
                    retVal = SWA_FAILED;
                    goto Cleanup;
                }

                // store into hashtable if fits, otherwise just use old schema
                if (fInsertCacheEntry)
                {
                    //
                    //  information we add to cache, consists of two parts:
                    //  1. SPOffset - locals, etc. of current method, adding which to current ESP we get to retAddr ptr
                    //  2. argSize - size of pushed function arguments, the rest we need to add to get new ESP
                    //  we have to store two parts of ESP delta, since we need to update pPC also, and so require retAddr ptr
                    //
                    //  newSP = oldSP + SPOffset + sizeof(PTR) + argSize
                    //
                    UINT_PTR SPOffset = (UINT_PTR)GetRegdisplayStackMark(cf.pRD) - curSP;
                    UINT_PTR argSize  = (UINT_PTR)GetRegdisplaySP(cf.pRD) - curSP - SPOffset - sizeof(void*);

                    StackwalkCacheEntry cacheEntry;
                    if (cacheEntry.Init(
                                curIP,
                                SPOffset,
                                &unwindInfo,
                                argSize))
                    {
                        cf.stackWalkCache.Insert(&cacheEntry);
                    }
                }
            }

    #define FAIL_IF_SPECULATIVE_WALK(condition)             \
            if (flags & PROFILER_DO_STACK_SNAPSHOT)         \
            {                                               \
                if (!(condition))                           \
                {                                           \
                    LOG((LF_CORPROF, LL_INFO100, "**PROF: " #condition " failure leads to SWA_FAILED.\n")); \
                    retVal = SWA_FAILED;                    \
                    goto Cleanup;                           \
                }                                           \
            }                                               \
            else                                            \
            {                                               \
                _ASSERTE(condition);                        \
            }

            // When the stackwalk is seeded with a profiler context, the context
            // might be bogus.  Check the stack pointer and the program counter for validity here.
            // (Note that these checks are not strictly necessary since we are able 
            // to recover from AVs during profiler stackwalk.)


            FAIL_IF_SPECULATIVE_WALK(!IsBadReadPtr(LPVOID(cf.pRD->pPC), sizeof(TADDR)));
            FAIL_IF_SPECULATIVE_WALK(!IsBadReadPtr(LPVOID(GetControlPC(cf.pRD)), sizeof(TADDR)));

    #undef FAIL_IF_SPECULATIVE_WALK

            LOG((LF_GCROOTS, LL_EVERYTHING, "STACKWALK: finished unwind for '%s', SP:" FMT_ADDR ", IP:" FMT_ADDR "\n",
                 cf.pFunc->m_pszDebugMethodName, DBG_ADDR(GetRegdisplaySP(cf.pRD)), DBG_ADDR(GetControlPC(cf.pRD))));

            cf.isFirst          = FALSE;
            cf.isInterrupted    = FALSE;
            cf.hasFaulted       = FALSE;
            cf.isIPadjusted     = FALSE;

#ifdef _X86_
            if (SWA_ABORT == MakeExplicitFrameCallbacksForManagedFrame(&cf, pCallback, pData, flags, true, fMakeCallback  DEBUG_ARG(uLoopIteration)))
            {
                retVal = SWA_ABORT;
                goto Cleanup;
            }
#endif // _X86_
            

#if defined(ELIMINATE_FEF) //<><><><><><><><><><><><><><><><><><><><><><><><><><><><>
            // As with frames, we may have unwound past a ExInfo.pContext.  This 
            //  can happen when unwinding from a handler that rethrew the exception.
            //  Skip any ExInfo.pContext records that may no longer be valid.
            // If Frames would be unlinked from the Frame chain, also reset the UseExInfoForStackwalk bit
            //  on the ExInfo.
            exInfoWalk.WalkToPosition(GetRegdisplaySP(cf.pRD), (flags & POPFRAMES) /* do reset UseExInfoForStackwalk*/);
#endif // ELIMINATE_FEF //<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

            // HandleSpecialFramelessStub() may call MakeExplicitFrameCallbacksForManagedFrame(), which needs to know whether
            // it should make a callback on an explicit frame.
            if (SWA_ABORT == HandleSpecialFramelessStub(&cf, pCallback, pData, flags, fMakeCallback  DEBUG_ARG(uLoopIteration)))
            {
                retVal = SWA_ABORT;
                goto Cleanup;
            }
            pEEJM = FillInCrawlFrameFromControlPC(&cf, (PBYTE)GetControlPC(cf.pRD));

            if (!cf.isFrameless)
            {
                // Stack walker just unwound a managed frame and "got lost".
                // This means we may be in native code (we're unwinding through a U2M transition).
                // We'll resume the rest of the stackwalk based off the FrameChain
                // until Frame::GetReturnAddress() provides us an address back in managed code.
                //
                if ((flags & NOTIFY_ON_U2M_TRANSITIONS)  
                     WIN64_ONLY(&& fMakeCallback))
                {
                    cf.isNativeMarker = true;

                    _ASSERTE(cf.IsNativeMarker());

                    END_FORBID_TYPELOAD();
                    retValTmp = MakeStackwalkerCallback(&cf, pCallback, pData DEBUG_ARG(uLoopIteration));
                    BEGIN_FORBID_TYPELOAD();
                    if (retValTmp == SWA_ABORT)
                    {
                        retVal = SWA_ABORT;
                        goto Cleanup;
                    }

                    cf.isNativeMarker = false;
                }
            }            
        }
        else
        {
            //-----------------------------------------------------------------
            // We have an explicit Frame
            //-----------------------------------------------------------------

#ifdef _DEBUG
            DebugLogStackWalkInfo(&cf, "CONSIDER", uLoopIteration);
#endif // _DEBUG
            
            pInlinedFrame = NULL;

            if (InlinedCallFrame::FrameHasActiveCall(cf.pFrame))
            {
                pInlinedFrame = cf.pFrame;
            }

            cf.pFunc  = cf.pFrame->GetFunction();
            INDEBUG(cf.codeMgrInstance = NULL);


            // Are we supposed to filter non-function frames?
            if ( (cf.pFunc || !(flags & FUNCTIONSONLY)) 
                 WIN64_ONLY( && fMakeCallback ) 
               )
            {
                END_FORBID_TYPELOAD();
                retValTmp = MakeStackwalkerCallback(&cf, pCallback, pData DEBUG_ARG(uLoopIteration));
                BEGIN_FORBID_TYPELOAD();
                if (retValTmp == SWA_ABORT)
                {
                    retVal = SWA_ABORT;
                    goto Cleanup;
                }
            }

            ////////////////////////////////////////////////////
            // update CrawlFrame state for next frame to process 
            ////////////////////////////////////////////////////

            // Special resumable frames make believe they are on top of the stack
            cf.isFirst = (cf.pFrame->GetFrameAttribs() & Frame::FRAME_ATTR_RESUMABLE) != 0;

            // If the frame is a subclass of ExceptionFrame,
            // then we know this is interrupted

            cf.isInterrupted = (cf.pFrame->GetFrameAttribs() & Frame::FRAME_ATTR_EXCEPTION) != 0;

            if (cf.isInterrupted)
            {
                cf.hasFaulted   = (cf.pFrame->GetFrameAttribs() & Frame::FRAME_ATTR_FAULTED) != 0;
                cf.isIPadjusted = (cf.pFrame->GetFrameAttribs() & Frame::FRAME_ATTR_OUT_OF_LINE) != 0;
                _ASSERTE(!cf.hasFaulted || !cf.isIPadjusted); // both cant be set together
            }

            //
            // Update app domain if this frame caused a transition
            //
            AppDomain *pAppDomain = cf.pFrame->GetReturnDomain();
            if (pAppDomain != NULL)
            {
                cf.pAppDomain = pAppDomain;
            }

            SLOT adr = (SLOT)cf.pFrame->GetReturnAddress();
            _ASSERTE(adr != (LPVOID)POISONC);

            _ASSERTE(!pInlinedFrame || adr);

            if (adr)
            {
                // is caller in managed code ?
                pEEJM = FillInCrawlFrameFromControlPC(&cf, adr);

                _ASSERTE(pEEJM || !pInlinedFrame);

                if (cf.isFrameless)
                {

                    cf.pFrame->UpdateRegDisplay(cf.pRD);


                    // We are transitioning from unmanaged code to managed code... lets do some validation of our
                    // EH mechanism on platforms that we can.
#if defined(_DEBUG) && (defined(_X86_) || defined(PAL_PORTABLE_SEH))
                    VerifyValidTransitionFromManagedCode(this, &cf);
#endif
                }
                else
                {
                    // We only need to call HandleSpecialFramelessStub() if we are not in managed code.
                    if (SWA_ABORT == HandleSpecialFramelessStub(&cf, pCallback, pData, flags, fMakeCallback  DEBUG_ARG(uLoopIteration)))
                    {
                        retVal = SWA_ABORT;
                        goto Cleanup;
                    }
                    pEEJM = FillInCrawlFrameFromControlPC(&cf, (PBYTE)GetControlPC(cf.pRD));
                }
            }

            if (!pInlinedFrame)
            {
#ifdef STACKWALKER_MAY_POP_FRAMES
                if (flags & POPFRAMES)
                {
                    // If we got here, the current frame chose not to handle the
                    // exception. Give it a chance to do any termination work
                    // before we pop it off.
                    END_FORBID_TYPELOAD();
                    cf.pFrame->ExceptionUnwind();
                    BEGIN_FORBID_TYPELOAD();

                    // Pop off this frame and go on to the next one.
                    cf.GotoNextFrame();

                    this->SetFrame(cf.pFrame);
                }
                else
#endif
                {
                    /* go to the next frame */
                    cf.pFrame = cf.pFrame->Next();

#ifndef DACCESS_COMPILE
                    if (cf.pFrame != FRAME_TOP)
                        cf.SetCurGSCookie(Frame::SafeGetGSCookiePtr(cf.pFrame));
#endif // DACCESS_COMPILE
                }

            }
        }
    }

    // Try to ensure that the frame chain did not change underneath us.
    // In particular, is thread's starting frame the same as it was when we started?
    _ASSERTE(startFrame  != 0 || startFrame == this->GetFrame());

Cleanup:
    //
    // If we got here, we either couldn't even start (for whatever reason)
    // or we came to the end of the stack. In the latter case we return SWA_DONE.
    //
    END_FORBID_TYPELOAD();

#ifdef _DEBUG
    if (retVal == SWA_DONE)
    {
        LOG((LF_GCROOTS, LL_INFO10000, "STACKWALK: SWA_DONE: reached the end of the stack\n"));
    }
    else if (retVal == SWA_FAILED)
    {
        LOG((LF_GCROOTS, LL_INFO10000, "STACKWALK: SWA_FAILED: couldn't start stackwalk\n"));
    }
#endif // _DEBUG

    return retVal;
} // StackWalkAction Thread::StackWalkFramesEx()

#endif // #ifndef DACCESS_COMPILE

StackWalkAction Thread::StackWalkFrames(PSTACKWALKFRAMESCALLBACK pCallback,
                               VOID *pData,
                               unsigned flags,
                               Frame *pStartFrame)
{
    // Note: there are cases (i.e., exception handling) where we may never return from this function. This means
    // that any C++ destructors pushed in this function will never execute, and it means that this function can
    // never have a dynamic contract.
    STATIC_CONTRACT_WRAPPER;
    _ASSERTE((flags & THREAD_IS_SUSPENDED) == 0 || (flags & ALLOW_ASYNC_STACK_WALK));

    CONTEXT ctx;
    REGDISPLAY rd;
    bool fUseInitRegDisplay;

#ifndef DACCESS_COMPILE
    _ASSERTE(GetThread() == this || (flags & ALLOW_ASYNC_STACK_WALK));
    BOOL fDebuggerHasInitialContext = (GetFilterContext() != NULL);
    BOOL fProfilerHasInitialContext = (GetProfilerFilterContext() != NULL);        

    // If this walk is seeded by a profiler, then the walk better be done by the profiler
    _ASSERTE(!fProfilerHasInitialContext || (flags & PROFILER_DO_STACK_SNAPSHOT));
    
    fUseInitRegDisplay              = fDebuggerHasInitialContext || fProfilerHasInitialContext;
#else
    fUseInitRegDisplay = true;
#endif

    if(fUseInitRegDisplay)
    {
        if (GetProfilerFilterContext() != NULL)
        {
            if (!InitRegDisplay(&rd, GetProfilerFilterContext(), TRUE))
            {
                LOG((LF_CORPROF, LL_INFO100, "**PROF: InitRegDisplay(&rd, GetProfilerFilterContext() failure leads to SWA_FAILED.\n"));
                return SWA_FAILED;
            }
        }
        else
        {
            if (!InitRegDisplay(&rd, &ctx, FALSE))
            {
                LOG((LF_CORPROF, LL_INFO100, "**PROF: InitRegDisplay(&rd, &ctx, FALSE) failure leads to SWA_FAILED.\n"));
                return SWA_FAILED;
            }
        }
    }
    else
    {
#ifdef _DEBUG_IMPL
        // We don't ever want to be a suspended cooperative mode thread here.
        // All threads that were in cooperative mode before suspend should be moving
        // towards waiting in preemptive mode.
        if (this != GetThread() && m_fPreemptiveGCDisabled)
        {
            DWORD dwSuspendCount;
            SuspendThreadResult str = SuspendThread(FALSE, &dwSuspendCount);
            if (str == STR_Success)
            {
                ResumeThread();

                if (dwSuspendCount != 0 && m_fPreemptiveGCDisabled && 
                      (GCHeap::IsGCInProgress() || !IsAbortRequested()) &&
                      ((flags & THREAD_EXECUTING_MANAGED_CODE) == 0))
                {
                    STRESS_LOG2(LF_ALWAYS, LL_ALWAYS, "STACKWALK: thread %x has suspend count %x\n",
                                this, dwSuspendCount);
                    _ASSERTE(!"Thread is in an invalid state to walk stack");
                }
            }
            else
            {
                STRESS_LOG2(LF_ALWAYS, LL_ALWAYS, "STACKWALK: thread %x, str %d\n", this, str);
                _ASSERTE(str != STR_SwitchedOut || CLRTaskHosted());
            }
        }
#endif  // _DEBUG_IMPL

        SetIP(&ctx, 0);
        SetSP(&ctx, 0);
        LOG((LF_GCROOTS, LL_INFO100000, "STACKWALK    starting with partial context\n"));
        FillRegDisplay(&rd, &ctx);
    }

#ifdef STACKWALKER_MAY_POP_FRAMES
    if (flags & POPFRAMES)
        rd.pContextForUnwind = &ctx;
#endif

    return StackWalkFramesEx(&rd, pCallback, pData, flags, pStartFrame);
}

StackWalkAction StackWalkFunctions(Thread * thread,
                                   PSTACKWALKFRAMESCALLBACK pCallback,
                                   VOID * pData)
{
    // Note: there are cases (i.e., exception handling) where we may never return from this function. This means
    // that any C++ destructors pushed in this function will never execute, and it means that this function can
    // never have a dynamic contract.
    STATIC_CONTRACT_WRAPPER;

    return thread->StackWalkFrames(pCallback, pData, FUNCTIONSONLY);
}

#ifdef DACCESS_COMPILE

//******************************************************************************
//
// StackFrameIterator.
//
//
//******************************************************************************
StackFrameIterator::StackFrameIterator(void)
{
    CONTRACT_VOID
    {
        CONSTRUCTOR_CHECK;
    } CONTRACT_END;

    m_thread     = NULL;
    m_initFrame  = NULL;
    m_startFrame = NULL;
    m_frameState = SFITER_UNINITIALIZED;



#if defined(USE_DBGHELP_TO_WALK_STACK_IN_DAC)
    m_fDbgHelpInit = false;
#endif // USE_DBGHELP_TO_WALK_STACK_IN_DAC

    m_fHandlingSpecialFramelessStub = false;
} // StackFrameIterator::StackFrameIterator()

//******************************************************************************
StackFrameIterator::~StackFrameIterator(void)
{
#if defined(USE_DBGHELP_TO_WALK_STACK_IN_DAC)
    if (m_fDbgHelpInit)
    {
        m_thread->FreeDbgHelp();
    }
#endif // USE_DBGHELP_TO_WALK_STACK_IN_DAC
} // StackFrameIterator::~StackFrameIterator()

//******************************************************************************
void
StackFrameIterator::Init(
    Thread      *thread,                    // Thread on which to walk.
    Frame       *frame,                     // Frame at which to start; NULL means use thread->GetFrame().
    PREGDISPLAY regDisp,                    // Initial reg display.
    ULONG32     flags)                      // StackWalkFrames flags.
{
    CONTRACT_VOID
    {
        NOTHROW;
        PRECONDITION(CheckPointer(thread));
        PRECONDITION(CheckPointer(frame, NULL_OK));
        PRECONDITION(CheckPointer(regDisp));
    } CONTRACT_END;

#if defined(USE_DBGHELP_TO_WALK_STACK_IN_DAC)
    if (m_fDbgHelpInit)
    {
        thread->FreeDbgHelp();
        m_fDbgHelpInit = false;
    }

    if (!m_fDbgHelpInit)
    {
        m_fDbgHelpInit = thread->InitDbgHelp();
        if (!m_fDbgHelpInit)
        {
            return;
        }
    }
#endif // USE_DBGHELP_TO_WALK_STACK_IN_DAC


    // FRAME_TOP and NULL must be distinct values. This assert
    // will fire if someone changes this.
    _ASSERTE(FRAME_TOP != NULL);

    m_thread = thread;

    m_initFrame = frame;
    if (frame)
    {
        m_startFrame = frame;
    }
    else
    {
        m_startFrame = m_thread->GetFrame();
        _ASSERTE(m_startFrame != NULL);
    }

    m_flags    = flags;
    m_scanFlag = IJitManager::GetScanFlags();

    m_crawl.pFrame = m_startFrame;
    m_crawl.isFirst = true;
    m_crawl.isInterrupted = false;
    m_crawl.hasFaulted = false;
    m_crawl.isIPadjusted = false;
    m_crawl.pRD = regDisp;
    m_crawl.pAppDomain = m_thread->GetDomain();
    INDEBUG(m_crawl.pFunc = (MethodDesc*)POISONC);

#if defined(ELIMINATE_FEF)
    // Walk the ExInfo chain, past any specified starting frame.
    m_exInfoWalk.Init(&(m_thread->GetExceptionState()->m_currentExInfo));
    m_exInfoWalk.WalkToPosition(m_initFrame, false /* don't reset UseExInfoForStackwalk*/);
#endif // ELIMINATE_FEF

    UpdateRegDisp();
    ProcessIp((SLOT)GetControlPC(m_crawl.pRD));
    ProcessCurrentFrame();

    RETURN;
} // StackFrameIterator::Init()

//******************************************************************************
// Is the iterator still valid?  That is, is there more left to iterate?
//******************************************************************************
BOOL
StackFrameIterator::IsValid(void)
{
    // There is more to iterate if the stackwalk is currently in managed code,
    //  or if there are frames left.
    // If there is an ExInfo with a pContext, it may substitute for a Frame,
    //  if the ExInfo is due to an exception in managed code.
    if (!m_crawl.isFrameless && m_crawl.pFrame == FRAME_TOP)
    {
#if defined(ELIMINATE_FEF)
        m_exInfoWalk.WalkToManaged();
        if (m_exInfoWalk.GetContext())
            return TRUE;
#endif // ELIMINATE_FEF
        // Try to ensure that the frame chain did not change underneath us.
        // In particular, is thread's starting frame the same as it was when
        // we started?
        _ASSERTE( (m_initFrame != NULL) ||
                  (m_flags & POPFRAMES) ||
                  (m_startFrame == m_thread->GetFrame()) );

        return FALSE;
    }

    return TRUE;
} // StackFrameIterator::IsValid()

//******************************************************************************
void
StackFrameIterator::SetInvalid(void)
{
    LEAF_CONTRACT;

    // Is this correct for the ELIMINATE_FEF case?
    m_crawl.pFrame = PTR_Frame((TADDR)FRAME_TOP);
    m_crawl.isFrameless = false;
} // StackFrameIterator::SetInvalid()

//******************************************************************************
void
StackFrameIterator::UpdateRegDisp(void)
{
    WRAPPER_CONTRACT;

    WIN64_ONLY(SyncRegDisplayToCurrentContext(m_crawl.pRD));
} // StackFrameIterator::UpdateRegDisp()

//******************************************************************************
void
StackFrameIterator::ProcessIp(SLOT Ip)
{
    WRAPPER_CONTRACT;

    m_thread->FillInCrawlFrameFromControlPC(&m_crawl, Ip);
} // StackFrameIterator::ProcessIp()

//******************************************************************************
StackWalkAction
StackFrameIterator::ProcessCurrentFrame(void)
{
    // Check for the case of an exception in managed code, and resync the stack walk
    //  from the exception context.
#if defined(ELIMINATE_FEF) 
    if (!m_crawl.isFrameless && m_exInfoWalk.GetExInfo())
    {
        // We are currently walking ("lost") in unmanaged code.  We can recover 
        //  from a) the next Frame record, or b) an exception context.
        // Recover from the exception context if all of these are true:
        //  - it "returns" to managed code
        //  - if is lower (newer) than the next Frame record
        //  - the stack walk has not already passed by it
        //
        // The ExInfo walker is initialized to be higher than the pStartFrame, and 
        //  as we unwind managed (frameless) functions, we keep eliminating any
        //  ExInfos that are passed in the stackwalk.
        //
        // So, here we need to find the next ExInfo that "returns" to managed code, 
        //  and then choose the lower of that ExInfo and the next Frame.
        m_exInfoWalk.WalkToManaged();
        TADDR pContextSP = m_exInfoWalk.GetSPFromContext();


        if (pContextSP)
        {
            PTR_CONTEXT pContext = m_exInfoWalk.GetContext();
            
#if defined(_DEBUG)
            {
            char buf[200];
            sprintf_s(buf, sizeof(buf), "CLR: ProcessCurrentFrame: considering resync from pContext(%p), sp(%p); pStartFrame(%p), cf.pFrame(%p), cf.SP(%p)\n",
                  pContext, pContextSP, m_initFrame, PTR_HOST_TO_TADDR(m_crawl.pFrame), GetRegdisplaySP(m_crawl.pRD));
            OutputDebugStringA(buf);
            }
#endif

            // If the pContext is lower (newer) than the CrawlFrame's Frame*, try to use
            //  the pContext.
            // There are still a few cases in which a FaultingExceptionFrame is linked in.  If 
            //  the next frame is one of them, we don't want to override it.  THIS IS PROBABLY BAD!!!
            if ( (pContextSP < PTR_HOST_TO_TADDR(m_crawl.pFrame)) &&
                 ((m_crawl.GetFrame() == FRAME_TOP) || (m_crawl.GetFrame()->GetVTablePtr() != FaultingExceptionFrame::GetMethodFrameVPtr() ) ) )  
            {   
                // Get the JitManager for the managed address.
                IJitManager* pEEJM = ExecutionManager::FindJitMan((PBYTE)GetIP(pContext));
                 
#if defined(_DEBUG)
                {
                char buf[200];
                sprintf_s(buf, sizeof(buf), "CLR: ProcessCurrentFrame: resync from pContext(%p), sp(%p); cf.pFrame(%p), cf.SP(%p)\n",
                      (void*)pContext, pContextSP, PTR_HOST_TO_TADDR(m_crawl.pFrame), GetRegdisplaySP(m_crawl.pRD));
                OutputDebugStringA(buf);
                }
#endif

                // Update the RegDisplay from the context info.
                FillRegDisplay(m_crawl.pRD, pContext);

                // Now we know where we are, and it's "frameless", aka managed.
                m_crawl.isFrameless = true;

                // Update the Jit Manager and Code Manager.
                m_crawl.JitManagerInstance = pEEJM;
                m_crawl.codeMgrInstance = pEEJM->GetCodeManager();

                // Flags the same as from a FaultingExceptionFrame.
                m_crawl.isInterrupted = 1;
                m_crawl.hasFaulted = 1;
                m_crawl.isIPadjusted = 0;

                // If Frames would be unlinked from the Frame chain, also reset the UseExInfoForStackwalk bit
                //  on the ExInfo.
                if (m_flags & POPFRAMES)
                {
                    m_exInfoWalk.GetExInfo()->m_ExceptionFlags.ResetUseExInfoForStackwalk();
                }
                // Done with this ExInfo.
                m_exInfoWalk.WalkOne();
            }
        }
    }
#endif // defined(ELIMINATE_FEF)

    if (!IsValid())
    {
        return SWA_ABORT;
    }

    m_crawl.codeManState.dwIsSet = 0;
#if defined(_DEBUG)
    memset((void *)m_crawl.codeManState.stateBuf, 0xCD,
           sizeof(m_crawl.codeManState.stateBuf));
#endif // _DEBUG

    bool fStopAndReport = true;

    if (m_crawl.isFrameless)
    {
        {
            // This must be a JITed/managed native method

            m_crawl.JitManagerInstance->JitCodeToMethodInfo((SLOT)GetControlPC(m_crawl.pRD),
                                                            &(m_crawl.pFunc),
                                                            &(m_crawl.methodToken),
                                                            (DWORD*)&(m_crawl.relOffset),
                                                            m_scanFlag);


            if (fStopAndReport)
            {
                m_frameState = SFITER_FRAMELESS_METHOD;
            }
        }
    }
    else if (m_crawl.pFrame != FRAME_TOP)
    {
        m_crawl.pFunc = m_crawl.pFrame->GetFunction();
        INDEBUG(m_crawl.codeMgrInstance = NULL);

        if ( ((m_crawl.pFunc == NULL) && (m_flags & FUNCTIONSONLY)) 
             WIN64_ONLY( || (m_pMDofParent != NULL) )
           )
        {
            fStopAndReport = false;
        }

        if (fStopAndReport)
        {
            m_frameState = SFITER_FRAME_FUNCTION;
        }
    }

    StackWalkAction swaResult = SWA_CONTINUE;
    if (!fStopAndReport)
    {
        // fast foward
        //
        swaResult = Next();
    }
    return swaResult;
} // StackFrameIterator::ProcessCurrentFrame()

//******************************************************************************
// This function is conceptually the same as MakeExplicitFrameCallbacksForManagedFrame().
// The difference is that MEFCFMF() can make all the callbacks before it returns, but
// this function has to return to the caller each time it handles a frame.  So we
// use m_frameState as a key.  If the frame state is SFITER_SKIPPED_FRAME_FUNCTION,
// then we know we are still handling skipped frames.
//******************************************************************************
BOOL
StackFrameIterator::CheckForSkippedFrames(void)
{
    BOOL   fHandleSkippedFrames = FALSE;
    LPVOID pvReferenceSP;

#if defined(_X86_)
    // Can the caller handle skipped frames;
    fHandleSkippedFrames = (m_flags & HANDLESKIPPEDFRAMES);
#endif // _X86_

    pvReferenceSP = GetRegdisplaySP(m_crawl.pRD);

    if ( !( (m_crawl.pFrame != FRAME_TOP) && 
            (PTR_HOST_TO_TADDR(m_crawl.pFrame) < (TADDR)pvReferenceSP) )
       )
    {
        return FALSE;
    }

    // We might have skipped past some Frames.
    // This happens with InlinedCallFrames and if we unwound
    // out of a finally in managed code or for ContextTransitionFrames
    // that are inserted into the managed call stack.
    while ( (m_crawl.pFrame != FRAME_TOP) &&
            (PTR_HOST_TO_TADDR(m_crawl.pFrame) < (TADDR)pvReferenceSP)
          )
    {
        if (fHandleSkippedFrames
            || InlinedCallFrame::FrameHasActiveCall(m_crawl.pFrame)
            )
        {
            m_crawl.GotoNextFrame();
        }
        else
        {
            m_crawl.codeMgrInstance = NULL;
            m_crawl.isFrameless = false;
            m_crawl.pFunc = m_crawl.pFrame->GetFunction();
            m_frameState = SFITER_SKIPPED_FRAME_FUNCTION;
            return TRUE;
        }
    }

    return FALSE;
} // StackFrameIterator::CheckForSkippedFrames()

//******************************************************************************
StackWalkAction
StackFrameIterator::Next(void)
{
    bool fProcessCurrentFrameWithoutUnwinding = false;

    if (m_frameState == SFITER_SKIPPED_FRAME_FUNCTION)
    {
        // Go to the next frame.
        m_crawl.GotoNextFrame();

        if (CheckForSkippedFrames())
        {
            return SWA_CONTINUE;
        }
        else
        {
#if defined(_X86_)

#if defined(ELIMINATE_FEF) 
            // As with frames, we may have unwound past a ExInfo.pContext.  This 
            //  can happen when unwinding from a handler that rethrew the exception.
            //  Skip any ExInfo.pContext records that may no longer be valid.
            // If Frames would be unlinked from the Frame chain, also reset the UseExInfoForStackwalk bit
            //  on the ExInfo.
            m_exInfoWalk.WalkToPosition(GetRegdisplaySP(m_crawl.pRD), (m_flags & POPFRAMES) /* reset UseExInfoForStackwalk*/);
#endif // ELIMINATE_FEF 

            ProcessIp((SLOT)GetControlPC(m_crawl.pRD));

#endif // _WIN64

        }
    }

    if (!fProcessCurrentFrameWithoutUnwinding)
    {
        if (!IsValid())
        {
            return SWA_ABORT;
        }

        if (m_crawl.isFrameless)
        {
#if defined(_X86_) || defined(_WIN64)
            EECodeInfo codeInfo(m_crawl.methodToken, m_crawl.JitManagerInstance, m_crawl.pFunc);

            // Now find out if we need to leave monitors.
            LPVOID methodInfo = m_crawl.JitManagerInstance->GetGCInfo(m_crawl.methodToken);

#if !defined(ELIMINATE_FEF)
            // FaultingExceptionFrame is special case where it gets
            // pushed on the stack after the frame is running
            _ASSERTE((m_crawl.pFrame == FRAME_TOP) ||
                     ((TADDR)GetRegdisplaySP(m_crawl.pRD) < PTR_HOST_TO_TADDR(m_crawl.pFrame)) ||
                     (m_crawl.pFrame->GetVTablePtr() == FaultingExceptionFrame::GetMethodFrameVPtr()) ||
                     (m_crawl.pFrame->GetVTablePtr() == ContextTransitionFrame::GetMethodFrameVPtr()));
#endif // defined(_X86_) && !defined(ELIMINATE_FEF)

            unsigned unwindFlags = (m_flags & QUICKUNWIND) ? 0 : UpdateAllRegs;
            m_crawl.codeMgrInstance->UnwindStackFrame(m_crawl.pRD,
                                                      methodInfo,
                                                      &codeInfo,
                                                      unwindFlags | m_crawl.GetCodeManagerFlags(),
                                                      &m_crawl.codeManState,
                                                      NULL);

            m_crawl.isFirst       = FALSE;
            m_crawl.isInterrupted = FALSE;
            m_crawl.hasFaulted    = FALSE;
            m_crawl.isIPadjusted  = FALSE;

#if defined(_X86_)
            if (CheckForSkippedFrames())
            {
                return SWA_CONTINUE;
            }
#endif // _X86_

#if defined(ELIMINATE_FEF) 
            // As with frames, we may have unwound past a ExInfo.pContext.  This 
            //  can happen when unwinding from a handler that rethrew the exception.
            //  Skip any ExInfo.pContext records that may no longer be valid.
            // If Frames would be unlinked from the Frame chain, also reset the UseExInfoForStackwalk bit
            //  on the ExInfo.
            m_exInfoWalk.WalkToPosition(GetRegdisplaySP(m_crawl.pRD), (m_flags & POPFRAMES) /* reset UseExInfoForStackwalk*/);
#endif // ELIMINATE_FEF 

            ProcessIp((SLOT)GetControlPC(m_crawl.pRD));

#else  // !_X86_ && !_WIN64
            _ASSERTE(!"StackFrameIterator is NYI on this platform.");
#endif // !_X86_ && !_WIN64
        }
        else
        {   // !m_crawl.isFrameless
            Frame* inlinedFrame = NULL;

            if (InlinedCallFrame::FrameHasActiveCall(m_crawl.pFrame))
            {
                inlinedFrame = m_crawl.pFrame;
            }

            // Special resumable frames make believe they are on top of the stack.
            m_crawl.isFirst = (m_crawl.pFrame->GetFrameAttribs() & Frame::FRAME_ATTR_RESUMABLE) != 0;

            // If the frame is a subclass of ExceptionFrame,
            // then we know this is interrupted.
            m_crawl.isInterrupted = (m_crawl.pFrame->GetFrameAttribs() & Frame::FRAME_ATTR_EXCEPTION) != 0;

            if (m_crawl.isInterrupted)
            {
                m_crawl.hasFaulted = (m_crawl.pFrame->GetFrameAttribs() & Frame::FRAME_ATTR_FAULTED) != 0;
                m_crawl.isIPadjusted = (m_crawl.pFrame->GetFrameAttribs() & Frame::FRAME_ATTR_OUT_OF_LINE) != 0;
                _ASSERTE(!m_crawl.hasFaulted || !m_crawl.isIPadjusted); // both cant be set together
            }

            //
            // Update app domain if this frame caused a transition.
            //

            AppDomain *retDomain = m_crawl.pFrame->GetReturnDomain();
            if (retDomain != NULL)
            {
                m_crawl.pAppDomain = retDomain;
            }

            SLOT adr = (SLOT)m_crawl.pFrame->GetReturnAddress();
            _ASSERTE(adr != (LPVOID)POISONC);

            _ASSERTE(!inlinedFrame || adr);

            if (adr)
            {
                ProcessIp(adr);

                _ASSERTE(m_crawl.JitManagerInstance || !inlinedFrame);

                if (m_crawl.isFrameless)
                {

                    m_crawl.pFrame->UpdateRegDisplay(m_crawl.pRD);

                }
            }

            if (!inlinedFrame)
            {
                // Go to the next frame.
                m_crawl.GotoNextFrame();
            }
        }
    }

    return ProcessCurrentFrame();
} // StackFrameIterator::Next()

//******************************************************************************
void
StackFrameIterator::ResetRegDisp(PREGDISPLAY regDisp)
{
    TADDR curPc = (TADDR)GetControlPC(regDisp);
    m_crawl.pRD = regDisp;
    UpdateRegDisp();
    ProcessIp((SLOT)curPc);

    //
    // We may need to advance the current frame
    // to catch up to the new stack location.
    // The assumption is that this reset is called
    // in response to a completion of some
    // unmanaged stack walking and now may point
    // to managed code.  If so, we should find
    // a transition frame with a return address
    // that matches the current PC.
    // XXX drewb - This may not be a good enough search.
    //

    if (m_crawl.isFrameless)
    {
        // The new context refers to managed code,
        // so look for an unprocessed transition frame.
        Frame* checkFrame = m_crawl.pFrame;
        while (checkFrame && (checkFrame != FRAME_TOP))
        {
            if (checkFrame->GetReturnAddress() == curPc)
            {
                break;
            }

            checkFrame = checkFrame->Next();
        }

        // If we found a matching transition frame
        // advance the real crawl frame to that frame.
        if (checkFrame && (checkFrame != FRAME_TOP))
        {
            while (checkFrame != m_crawl.pFrame)
            {
                m_crawl.pFrame->UpdateRegDisplay(m_crawl.pRD);
                m_crawl.GotoNextFrame();
            }

            // Apply the final transition frame to get
            // the fully updated register info.
            m_crawl.pFrame->UpdateRegDisplay(m_crawl.pRD);
            m_crawl.GotoNextFrame();

            _ASSERTE(curPc == (TADDR)GetControlPC(m_crawl.pRD));
        }
    }

    ProcessCurrentFrame();
} // StackFrameIterator::ResetRegDisp()

//******************************************************************************
// Iterater based implementation of StackWalkFramesEx.  Used only by
//  DAC.
//******************************************************************************
StackWalkAction Thread::StackWalkFramesEx(
                    PREGDISPLAY pRD,        // virtual register set at crawl start
                    PSTACKWALKFRAMESCALLBACK pCallback,
                    VOID *pData,
                    unsigned flags,
                    Frame *pStartFrame
                )
{

    BEGIN_FORBID_TYPELOAD();

    StackFrameIterator frameIter;
    StackWalkAction retVal = SWA_FAILED;

    frameIter.Init(this, pStartFrame, pRD, flags);

    while (frameIter.IsValid())
    {
        retVal = SWA_DONE;

        switch(frameIter.m_frameState)
        {
        case StackFrameIterator::SFITER_FRAMELESS_METHOD:
            {
                END_FORBID_TYPELOAD();
                if (SWA_ABORT == pCallback(&frameIter.m_crawl, (VOID*)pData))
                {
                    return SWA_ABORT;
                }
                BEGIN_FORBID_TYPELOAD();
            }

            break;

        case StackFrameIterator::SFITER_FRAME_FUNCTION:

            // Fall through.

        case StackFrameIterator::SFITER_SKIPPED_FRAME_FUNCTION:
            // Are we supposed to filter non-function frames?
            if (frameIter.m_crawl.pFunc || !(flags&FUNCTIONSONLY))
            {
                END_FORBID_TYPELOAD();
                if (SWA_ABORT == pCallback(&frameIter.m_crawl, (VOID *)pData))
                    return SWA_ABORT;
                BEGIN_FORBID_TYPELOAD();
            }
            break;

        default:
            break;
        }

        frameIter.Next();
    }

#ifdef _DEBUG
    if (retVal == SWA_FAILED)
    {
        LOG((LF_CORPROF, LL_INFO100, "**PROF: StackWalkFramesEx fall through leads to SWA_FAILED.\n"));
    }
#endif

    END_FORBID_TYPELOAD();
    return retVal;
} // StackWalkAction Thread::StackWalkFramesEx()

#endif // #ifdef DACCESS_COMPILE


/*
    copies 64-bit *src to *target, atomically accessing the data
    requires 64-bit alignment for atomic load/store
*/
inline static void atomicMoveCacheEntry(UINT64* src, UINT64* target)
{
    _ASSERTE(false);
}

/*
============================================================
Here is an implementation of StackwalkCache class, used to optimize performance
of stack walking. Currently each CrawlFrame has a StackwalkCache member, which implements
functionality for caching already walked methods (see Thread::StackWalkFramesEx).
See class and corresponding types declaration at stackwalktypes.h
We do use global cache g_StackwalkCache[] with InterlockCompareExchange, fitting
each cache entry into 8 bytes.
============================================================
*/

#define LOG_NUM_OF_CACHE_ENTRIES 10
#define NUM_OF_CACHE_ENTRIES (1 << LOG_NUM_OF_CACHE_ENTRIES)

static volatile StackwalkCacheEntry g_StackwalkCache[NUM_OF_CACHE_ENTRIES]; // Global StackwalkCache

BOOL StackwalkCache::s_Enabled = FALSE;

#ifndef DACCESS_COMPILE

/*
    StackwalkCache class constructor.
    Set "enable/disable optimization" flag according to registry key.
*/
StackwalkCache::StackwalkCache()
{
    CONTRACTL {
       NOTHROW;
       GC_NOTRIGGER;
    } CONTRACTL_END;

    ClearEntry();

    static BOOL stackwalkCacheEnableChecked = FALSE;
    if (!stackwalkCacheEnableChecked)
    {
        // We can enter this block on multiple threads because of racing.
        // However, that is OK since this operation is idempotent
        
        s_Enabled = ((g_pConfig->DisableStackwalkCache() == 0) &&
                    // disable cache if for some reason it is not aligned
                    IS_ALIGNED((void*)&g_StackwalkCache[0], STACKWALK_CACHE_ENTRY_ALIGN_BOUNDARY));
        stackwalkCacheEnableChecked = TRUE;
    }
}

#endif // #ifndef DACCESS_COMPILE

// static
void StackwalkCache::Init()
{
}

/*
    Returns efficient hash table key based on provided IP.
    CPU architecture dependent.
*/
inline unsigned StackwalkCache::GetKey(UINT_PTR IP)
{
    LEAF_CONTRACT;
    return (unsigned)(((IP >> LOG_NUM_OF_CACHE_ENTRIES) ^ IP) & (NUM_OF_CACHE_ENTRIES-1));
}

/*
    Looks into cache and returns StackwalkCache entry, if current IP is cached.
    JIT team guarantees the same ESP offset for the same IPs for different call chains.
*/
BOOL StackwalkCache::Lookup(UINT_PTR IP)
{
    CONTRACTL {
       NOTHROW;
       GC_NOTRIGGER;
    } CONTRACTL_END;

#if defined(_X86_) || defined(_AMD64_)
    _ASSERTE(Enabled());
    _ASSERTE(IP);

    unsigned hkey = GetKey(IP);
    _ASSERTE(IS_ALIGNED((void*)&g_StackwalkCache[hkey], STACKWALK_CACHE_ENTRY_ALIGN_BOUNDARY));
    // Don't care about m_CacheEntry access atomicity, since it's private to this
    // stackwalk/thread
    atomicMoveCacheEntry((UINT64*)&g_StackwalkCache[hkey], (UINT64*)&m_CacheEntry);

#ifdef _DEBUG
    if (IP != m_CacheEntry.IP)
    {
        ClearEntry();
    }
#endif

    return (IP == m_CacheEntry.IP);
#else // _X86_
    return FALSE;
#endif // _X86_
}

/*
    Caches data provided for current IP.
*/
void StackwalkCache::Insert(StackwalkCacheEntry *pCacheEntry)
{
    CONTRACTL {
       NOTHROW;
       GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE(Enabled());
    _ASSERTE(pCacheEntry);

    unsigned hkey = GetKey(pCacheEntry->IP);
    _ASSERTE(IS_ALIGNED((void*)&g_StackwalkCache[hkey], STACKWALK_CACHE_ENTRY_ALIGN_BOUNDARY));
    atomicMoveCacheEntry((UINT64*)pCacheEntry, (UINT64*)&g_StackwalkCache[hkey]);
}

// static
void StackwalkCache::Invalidate(AppDomain * pDomain)
{
    CONTRACTL {
       NOTHROW;
       GC_NOTRIGGER;
    } CONTRACTL_END;

    if (!s_Enabled)
        return;

    /* Note that we could just flush the entries corresponding to
    pDomain if we wanted to get fancy. To keep things simple for now,
    we just invalidate everything
    */
    
    ZeroMemory(PVOID(&g_StackwalkCache), sizeof(g_StackwalkCache));
}

