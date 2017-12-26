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

#include "common.h"

#include "stackcompressor.h"
#include "securitystackwalk.h"
#include "appdomainstack.inl"
#include "comdelegate.h"

//-----------------------------------------------------------
// Stack walk callback data structure for stack compress.
//-----------------------------------------------------------
typedef struct _StackCompressData
{
    void*    compressedStack;
    StackCrawlMark *    stackMark;
    DWORD               dwFlags;
    Assembly *          prevAssembly; // Previously checked assembly.
    AppDomain *         prevAppDomain;
    Frame* pCtxTxFrame;
} StackCompressData;


void TurnSecurityStackWalkProgressOn( Thread* pThread ) 
{ 
    WRAPPER_CONTRACT;
    pThread->SetSecurityStackwalkInProgress( TRUE ); 
}
void TurnSecurityStackWalkProgressOff( Thread* pThread ) 
{ 
    WRAPPER_CONTRACT;
    pThread->SetSecurityStackwalkInProgress( FALSE ); 
}
typedef Holder< Thread*, TurnSecurityStackWalkProgressOn, TurnSecurityStackWalkProgressOff > StackWalkProgressEnableHolder;



DWORD StackCompressor::GetCSInnerAppDomainOverridesCount(COMPRESSEDSTACKREF csRef)
{
    //csRef can be NULL - that implies that we set the CS, then crossed an AD. So we would already have counted the overrides when we hit the
    // ctxTxFrame. Nothing to do here
    if (csRef != NULL)
    {
        NewCompressedStack* cs = (NewCompressedStack*)csRef->GetUnmanagedCompressedStack();
        if (cs != NULL)
            return cs->GetInnerAppDomainOverridesCount();
    }
    return 0;
}
DWORD StackCompressor::GetCSInnerAppDomainAssertCount(COMPRESSEDSTACKREF csRef)
{
    //csRef can be NULL - that implies that we set the CS, then crossed an AD. So we would already have counted the overrides when we hit the
    // ctxTxFrame. Nothing to do here
    if (csRef != NULL)
    {
        NewCompressedStack* cs = (NewCompressedStack*)csRef->GetUnmanagedCompressedStack();
        if (cs != NULL)
            return cs->GetInnerAppDomainAssertCount();
    }
    return 0;
}

void* StackCompressor::SetAppDomainStack(Thread* pThread, void* curr)
{
    CONTRACTL
    {
        MODE_ANY;
        GC_NOTRIGGER;
        THROWS;
    } CONTRACTL_END;

    NewCompressedStack* unmanagedCompressedStack = (NewCompressedStack *)curr;

    AppDomainStack* pRetADStack = NULL;
       
    if (unmanagedCompressedStack != NULL)
    {
        pRetADStack = new AppDomainStack(pThread->GetAppDomainStack());
        pThread->SetAppDomainStack(unmanagedCompressedStack->GetAppDomainStack() );
    }
    else
    {
        if (!pThread->IsDefaultSecurityInfo())    /* Do nothing for the single domain/FT/no overrides case */
        {
            pRetADStack = new AppDomainStack(pThread->GetAppDomainStack());            
            pThread->ResetSecurityInfo(); 
        }
    }
    return (void*)pRetADStack;
}

void StackCompressor::RestoreAppDomainStack(Thread* pThread, void* appDomainStack)
{
    WRAPPER_CONTRACT;
    _ASSERTE(appDomainStack != NULL);
    AppDomainStack* pADStack = (AppDomainStack*)appDomainStack;
    pThread->SetAppDomainStack(*pADStack);
    delete pADStack;
}

void StackCompressor::Destroy(void *stack)
{
    WRAPPER_CONTRACT;
    _ASSERTE(stack != NULL && "Don't pass NULL");
    NewCompressedStack* ncs = (NewCompressedStack*)stack;
    ncs->Destroy();
}


/* Forward declarations of the new CS stackwalking implementation */
static void NCS_GetCompressedStackWorker(Thread *t, void *pData);
static StackWalkAction NCS_CompressStackCB(CrawlFrame* pCf, void *pData);

OBJECTREF StackCompressor::GetCompressedStack( StackCrawlMark* stackMark )
{
   
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_INTOLERANT;// Not an entry point
    } CONTRACTL_END;

    // Get the current thread that we're to walk.
    Thread * t = GetThread();    

    NewCompressedStackHolder csHolder(new NewCompressedStack());

        

    //
    // Initialize the callback data on the stack...        
    //

    StackCompressData walkData;
    
    walkData.dwFlags = 0;
    walkData.prevAssembly = NULL;
    walkData.prevAppDomain = NULL;
    walkData.stackMark = stackMark;
    walkData.pCtxTxFrame = NULL;



    walkData.compressedStack = (void*)csHolder.GetValue();
    NCS_GetCompressedStackWorker(t, &walkData);
    
    struct _gc {
        SAFEHANDLE pSafeCSHandle;
    } gc;
    gc.pSafeCSHandle = NULL;
    
    GCPROTECT_BEGIN(gc);
    // Allocate a SafeHandle here
    static MethodTable *pMT = NULL;
    if (pMT == NULL)
        pMT = g_Mscorlib.GetClass(CLASS__SAFE_CSHANDLE); 
    
    gc.pSafeCSHandle = (SAFEHANDLE) AllocateObject(pMT);
    CallDefaultConstructor(gc.pSafeCSHandle);
    gc.pSafeCSHandle->SetHandle((void*) csHolder.GetValue());
    csHolder.SuppressRelease();
    
    GCPROTECT_END();
    return (OBJECTREF) gc.pSafeCSHandle;
}

    
void NCS_GetCompressedStackWorker(Thread *t, void *pData)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    StackCompressData *pWalkData = (StackCompressData*)pData;
    NewCompressedStack* compressedStack = (NewCompressedStack*) pWalkData->compressedStack;

    _ASSERTE( t != NULL );

    {
        StackWalkProgressEnableHolder holder( t );

        //
        // Begin the stack walk...
        //
        // LIGHTUNWIND flag: allow using stackwalk cache for security stackwalks
        t->StackWalkFrames(NCS_CompressStackCB, pWalkData, LIGHTUNWIND);


        // Ignore CS (if present) when we hit a FT assert
        if (pWalkData->dwFlags & CORSEC_FT_ASSERT)
            return;
        
        // Check if there is a CS at the top of the thread
        COMPRESSEDSTACKREF csRef = (COMPRESSEDSTACKREF)t->GetCompressedStack();
        AppDomain *pAppDomain = t->GetDomain();
        Frame *pFrame = NULL;
        if (csRef == NULL)
        {
            // There may have been an AD transition and we shd look at the CB data to see if this is the case
            if (pWalkData->pCtxTxFrame != NULL)
            {
                pFrame = pWalkData->pCtxTxFrame;
                csRef = Security::GetCSFromContextTransitionFrame(pFrame);
                _ASSERTE(csRef != NULL); //otherwise we would not have saved the frame in the CB data
                pAppDomain = pWalkData->pCtxTxFrame->GetReturnDomain();
            }
        }

        if (csRef != NULL)
        {
            

            compressedStack->ProcessCS(pAppDomain, csRef, pFrame);
        }
        else
        {
            compressedStack->ProcessAppDomainTransition(); // just to update domain overrides/assert count at the end of stackwalk
        }


    }

    return;      
}

StackWalkAction NCS_CompressStackCB(CrawlFrame* pCf, void *pData)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    StackCompressData *pCBdata = (StackCompressData*)pData;
    NewCompressedStack* compressedStack = (NewCompressedStack*) pCBdata->compressedStack;

    // First check if the walk has skipped the required frames. The check
    // here is between the address of a local variable (the stack mark) and a
    // pointer to the EIP for a frame (which is actually the pointer to the
    // return address to the function from the previous frame). So we'll
    // actually notice which frame the stack mark was in one frame later. This
    // is fine for our purposes since we're always looking for the frame of the
    // caller of the method that actually created the stack mark. 
    _ASSERTE((pCBdata->stackMark == NULL) || (*pCBdata->stackMark == LookForMyCaller));
    if ((pCBdata->stackMark != NULL) &&
        !IsInCalleesFrames(pCf->GetRegisterSet(), pCBdata->stackMark))
        return SWA_CONTINUE;

    Frame *pFrame = pCf->GetFrame();

    // Save the CtxTxFrame if this is one
    if (pCBdata->pCtxTxFrame == NULL)
    {
        if (Security::IsContextTransitionFrameWithCS(pFrame))
        {
            pCBdata->pCtxTxFrame = pFrame;
        }
    }

    //  Handle AppDomain transitions:
    AppDomain *pAppDomain = pCf->GetAppDomain();
    if (pCBdata->prevAppDomain != pAppDomain)
    {
        compressedStack->ProcessAppDomainTransition();
        pCBdata->prevAppDomain = pAppDomain;
    }


    if (pCf->GetFunction() == NULL)
        return SWA_CONTINUE; // not a function frame, so we were just looking for CtxTransitionFrames. Resume the stackwalk...
        
    // Get the security object for this function...
    OBJECTREF* pRefSecDesc = pCf->GetAddrOfSecurityObject();

    MethodDesc * pFunc = pCf->GetFunction();

    _ASSERTE(pFunc != NULL); // we requested methods!

    Assembly * pAssem = pCf->GetAssembly();
    _ASSERTE(pAssem != NULL);
    PREFIX_ASSUME(pAssem != NULL);



    
    if (pRefSecDesc != NULL)
        SecurityDeclarative::DoDeclarativeSecurityAtStackWalk(pFunc, pAppDomain, pRefSecDesc);

    
    
    if (pFunc->GetMethodTable()->IsAnyDelegateClass())
    {
        DelegateEEClass* delegateCls = (DelegateEEClass*) pFunc->GetMethodTable()->GetClass();
        if (pFunc == delegateCls->m_pBeginInvokeMethod)
        {
            // Async delegate case: we may need to insert the creator frame into the CS 
            DELEGATEREF dRef = (DELEGATEREF) ((FramedMethodFrame *)pFrame)->GetThis();
            _ASSERTE(dRef);
            if (COMDelegate::IsSecureDelegate(dRef))
            {
                Assembly* pCreatorAssembly = (Assembly*) dRef->GetMethodPtrAux();
                _ASSERTE(pCreatorAssembly);
                compressedStack->ProcessFrame(pAppDomain, 
                                               pCreatorAssembly->GetSharedSecurityDescriptor(), 
                                               NULL) ; // ignore return value - No FSD being passed in.
            }
            
        }
    }
        

    DWORD retFlags = compressedStack->ProcessFrame(pAppDomain, 
                                                   pAssem->GetSharedSecurityDescriptor(), 
                                                   (FRAMESECDESCREF *) pRefSecDesc) ;
    
    pCBdata->dwFlags |= (retFlags & CORSEC_FT_ASSERT);
    // ProcessFrame returns TRUE if we should stop stackwalking
   if (retFlags != 0 || Security::IsSpecialRunFrame(pFunc))
        return SWA_ABORT;
   
    return SWA_CONTINUE;

}

