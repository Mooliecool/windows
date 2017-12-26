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


#include "threads.h"
#include "excep.h"
#include "object.h"
#include "dllimportcallback.h"
#include "mlgen.h"
#include "ml.h"
#include "mlinfo.h"
#include "mlcache.h"
#include "comdelegate.h"
#include "ceeload.h"
#include "eeconfig.h"
#include "dbginterface.h"
#include "stubgen.h"
#include "mdaassistantsptr.h"
#include "memoryreport.h"
#include "appdomain.inl"


struct UM2MThunk_Args
{
    UMEntryThunk *pEntryThunk;
    void *pAddr;
    void *pThunkArgs;
    int argLen;
    void *retVal;
};

struct DoUMThunkCall_Args
{
    Thread*         pThread;
    UMThkCallFrame* pFrame;
    INT64*          retVal;
};

static class UMThunkStubCache *g_pUMThunkStubCache = NULL;
static class ArgBasedStubRetainer *g_pUMThunkInterpretedStubCache = NULL;

EXTERN_C void *__stdcall UM2MThunk_WrapperHelper(void *pThunkArgs,
                                                 int argLen,
                                                 void *pAddr,
                                                 UMEntryThunk *pEntryThunk,
                                                 Thread *pThread);

#ifdef _X86_
EXTERN_C VOID __cdecl UMThunkStubRareDisable();
extern Thread* __stdcall CreateThreadBlockThrow();
#endif // _X86_
  

EXTERN_C void __fastcall ReverseEnterRuntimeHelper(Thread *pThread)

{
    WRAPPER_CONTRACT;
    // ReverseEnterRuntimeThrowComplus probes.
    //BEGIN_ENTRYPOINT_THROWS;
    
    _ASSERTE (pThread == GetThread());

    pThread->ReverseEnterRuntimeThrowComplus();
    //END_ENTRYPOINT_THROWS
}

EXTERN_C void __fastcall ReverseLeaveRuntimeHelper(Thread *pThread)
{
    WRAPPER_CONTRACT;
    
    _ASSERTE (pThread == GetThread());
    pThread->ReverseLeaveRuntime();
}

void * UM2MThunk_WrapperWorker (UM2MThunk_Args * pArgs, Thread * pThread)
{
    WRAPPER_CONTRACT;
    
    void * result = NULL;
    BEGIN_SO_TOLERANT_CODE (pThread);
    result = UM2MThunk_WrapperHelper(pArgs->pThunkArgs,
                                     pArgs->argLen,
                                     pArgs->pAddr,
                                     pArgs->pEntryThunk,
                                     pThread);    

    // We made the call in cooperative mode, but the epilog will return us to preemptive
    // on exit.
    pThread->DisablePreemptiveGC();
    END_SO_TOLERANT_CODE;
    
    return result;
}

// This is used as target of callback from DoADCallBack. It sets up the environment and effectively
// calls back into the thunk that needed to switch ADs. 
void UM2MThunk_Wrapper(LPVOID ptr) // UM2MThunk_Args
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ptr, NULL_OK));
    }
    CONTRACTL_END;
    
    UM2MThunk_Args *pArgs = (UM2MThunk_Args *) ptr;
    Thread* pThread = GetThread();

    pArgs->retVal = UM2MThunk_WrapperWorker (pArgs, pThread);
}

EXTERN_C void * __stdcall UM2MDoADCallBack(UMEntryThunk *pEntryThunk, void *pAddr, void *pArgs, int argLen)
{
    CONTRACT (void*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pEntryThunk));
        PRECONDITION(CheckPointer(pAddr));
        PRECONDITION(CheckPointer(pArgs));        
        DISABLED(POSTCONDITION(CheckPointer(RETVAL)));
    }
    CONTRACT_END;

    UM2MThunk_Args args = { pEntryThunk, pAddr, pArgs, argLen };


    INSTALL_UNWIND_AND_CONTINUE_HANDLER;

    GetThread()->DoADCallBack(pEntryThunk->GetDomainId(), UM2MThunk_Wrapper, &args);

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

    
    RETURN args.retVal;
}


ARG_SLOT ForwardCallToManagedMethod(const BYTE* pTargetMethodCode, ARG_SLOT* pArgStack, UINT32 sizeOfActualArgStack, BOOL fIsStatic,    // these are required
        MetaSig* pMetaSig, ArgumentRegisters* pArgRegs, void* pFPArgRegs, BYTE* pRetBuff, UINT fpReturnSize, UnmanagedToManagedCallGCInfo* pGCInfo);   // these are optional

void DoCleanupWorkList(CleanupWorkList *pCleanup)
{
    if (pCleanup)
    {
        pCleanup->Cleanup(FALSE);
    }
}

void DoUMThkFrameUnprotectArgs (UMThkCallFrame * pFrame)
{
   CONTRACTL
   {
       WRAPPER_CONTRACT;
       PRECONDITION(CheckPointer(pFrame));
       PRECONDITION(CheckPointer(pFrame->GetGCInfoPtr ()));
   }
   CONTRACTL_END;

   pFrame->GetGCInfoPtr()->DisableArgsGCProtection();
}


//--------------------------------------------------------------------------
// Worker to marshal the arguments and dispatch the call for 
// non-compiled UMThunk calls.
//--------------------------------------------------------------------------
PlatformDefaultReturnType __stdcall DoUMThunkCallWorker(Thread *pThread, UMThkCallFrame *pFrame, 
                                    const UMEntryThunk *pUMEntryThunk, UMThunkMarshInfo *pUMThunkMarshInfo, UMThunkMLStub *pheader
                                    IA64_ARG(BYTE* pHFAReturnBuffer))
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pThread));
        PRECONDITION(CheckPointer(pFrame));
    }
    CONTRACTL_END;
    
    PlatformDefaultReturnType   nativeRetVal;
    ZeroMemory(&nativeRetVal, sizeof(nativeRetVal));


    CleanupWorkList* pCleanup               = pFrame->GetCleanupWorkList();
    BOOL             fIsStatic              = !!(pheader->m_wFlags & umtmlIsStatic);
    UINT             sizeOfActualArgStack   = pUMThunkMarshInfo->GetCbActualArgSize();

    UINT cbDstBuffer = 0;
    UINT cbAlloc = 0;

    if (!ClrSafeInt<UINT>::addition(FramedMethodFrame::GetRawNegSpaceSize(), sizeof(FramedMethodFrame), cbDstBuffer))
        COMPlusThrow(kNotSupportedException);
    if (!ClrSafeInt<UINT>::addition(cbDstBuffer, sizeOfActualArgStack, cbDstBuffer))
        COMPlusThrow(kNotSupportedException);
    if (!ClrSafeInt<UINT>::addition(cbDstBuffer, pheader->m_cbLocals, cbAlloc))
        COMPlusThrow(kNotSupportedException);

    BYTE *pAlloc = (BYTE*) _alloca(cbAlloc);

    // Must zero-initialize since pFrame gcscan's part of this array.
    FillMemory(pAlloc, cbAlloc, 0);

    // We need to protect the delegate while the call is in progress.
    OBJECTHANDLE hndThisDelegate = pUMEntryThunk->GetObjectHandle();
    OBJECTREF    ThisDelegate    = hndThisDelegate ? ObjectFromHandle(hndThisDelegate) : NULL;
    GCPROTECT_BEGIN(ThisDelegate)
    {
        Holder<CleanupWorkList*,DoNothing<CleanupWorkList*>,DoCleanupWorkList> cleanupHolder(pCleanup);

        if (pCleanup) 
        {
            // Checkpoint the current thread's fast allocator (used for temporary
            // buffers over the call) and schedule a collapse back to the checkpoint in
            // the cleanup list. Note that if we need the allocator, it is
            // guaranteed that a cleanup list has been allocated.
            void* cp = pThread->m_MarshalAlloc.GetCheckpoint();
            pCleanup->ScheduleFastFree(cp);
            pCleanup->IsVisibleToGc();
        }

        BYTE *pDst = pAlloc + FramedMethodFrame::GetRawNegSpaceSize();
        BYTE *pLoc = pAlloc + cbDstBuffer;

        pFrame->SetDstArgsPointer(pDst);

        pFrame->GetGCInfoPtr()->EnableArgsGCProtection();
        const MLCode *pMLCode = NULL;
        MetaSig* pMetaSig = NULL;
        {
            // arg GC protection needs to be disabled when we finish marshaling, or when exception happens
            Holder<UMThkCallFrame *, DoNothing<UMThkCallFrame *>, DoUMThkFrameUnprotectArgs> frameArgsUnprotectHolder (pFrame);
#ifdef _PPC_
            ArgumentRegisters* pCallerArgRegs = 
                ((ArgumentRegisters*)((BYTE*)pFrame
                    - (offsetof(UMStubStackFrame, umframe) - offsetof(UMStubStackFrame, argregs))));

            memcpy((void*)pFrame->GetPointerToArguments(), pCallerArgRegs->r, 
                min(pheader->m_cbSrcStack, NUM_ARGUMENT_REGISTERS * sizeof(INT32)));
#endif

            pMLCode = pheader->GetMLCode();
            pMLCode = RunML(pMLCode,
                            (LPVOID)pFrame->GetPointerToArguments(),
                            pDst,
                            (UINT8*)pLoc,
                            pCleanup
                            PPC_ARG(pCallerArgRegs->f)
                            WIN64_ARG((DOUBLE*)pFrame->GetFPSpillBase())        // This is really a FPSPILL_SLOT*.
                            );

#if (THISPTR_LOCATION > 0) || defined(ENREGISTERED_RETURNTYPE_MAXSIZE) || defined(CALLDESCR_REGTYPEMAP) || defined(CALLDESCR_RETBUFMARK)
            SigTypeContext typeContext; // An empty SigTypeContext is OK - loads the generic type
            PCCOR_SIGNATURE pSig;
            DWORD cbSigSize;
            pSig = pUMThunkMarshInfo->GetSig(&cbSigSize);
            MetaSig msig(pSig, cbSigSize, pUMThunkMarshInfo->GetModule(), &typeContext);
            pMetaSig = &msig;
#endif

            if (!fIsStatic) 
            {
                BYTE* ppThis = pDst + ArgIterator::GetThisOffset(pMetaSig);
                *((OBJECTREF*) (ppThis) ) = ObjectFromHandle(pUMEntryThunk->GetObjectHandle());
            }

            frameArgsUnprotectHolder.SuppressRelease();  // this gets done inside ForwardCallToManagedMethod
        } // frameArgsUnprotectHolder

        LOG((LF_IJW, LL_INFO1000, "UM transition to 0x%p\n", (size_t)(pUMEntryThunk->GetManagedTarget())));;

        const BYTE*         pTargetMethod   = pUMEntryThunk->GetManagedTarget();
        ARG_SLOT*           pArgStack       = (ARG_SLOT*) (pDst + sizeof(FramedMethodFrame));
        ArgumentRegisters*  pArgRegs        = NULL; 
        void*               pFPArgRegs      = NULL;
        BYTE*               pRetBuff        = NULL;
        INT64               ComPlusRetVal;
        void*               pvManagedRetVal = &ComPlusRetVal;

#ifdef CALLDESCR_ARGREGS
        pArgRegs = (ArgumentRegisters*)(pDst + FramedMethodFrame::GetOffsetOfArgumentRegisters());
#endif // CALLDESCR_ARGREGS
#ifdef CALLDESCR_REGTYPEMAP
        pFPArgRegs = (void*)pFrame->GetFPSpillBase();
#endif // CALLDESCR_REGTYPEMAP
#ifdef CALLDESCR_RETBUF
        BYTE    SmallVCBuff[ENREGISTERED_RETURNTYPE_MAXSIZE];

        if (pMetaSig->HasRetBuffArg())
        {
#ifdef RETBUF_ARG_SPECIAL_PARAM
            pRetBuff = (BYTE*)pFrame->GetRetBuffArg();
#endif // RETBUF_ARG_SPECIAL_PARAM            
        }
        else if (ELEMENT_TYPE_VALUETYPE == pMetaSig->GetReturnTypeNormalized())
        {
            pRetBuff = (BYTE*)&SmallVCBuff;
            pvManagedRetVal = &SmallVCBuff;
        }
#endif // CALLDESCR_RETBUF


        ComPlusRetVal = ForwardCallToManagedMethod(pTargetMethod, pArgStack, sizeOfActualArgStack, 
                                                fIsStatic, pMetaSig, pArgRegs, pFPArgRegs, pRetBuff,
                                                pheader->m_wFlags & umtmlFpu ? pheader->m_cbRetValSize : 0, 
                                                pFrame->GetGCInfoPtr());

        BYTE* pRetValDst = (BYTE*)&nativeRetVal;
#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
        pRetValDst += pheader->m_cbRetValSize;
#endif


        if (pheader->m_wFlags & umtmlRetValRequiredGCProtect)
        {
            GCPROTECT_BEGIN( *(OBJECTREF*)&ComPlusRetVal );
            RunML(pMLCode,
                pvManagedRetVal,
                pRetValDst,
                (UINT8*)pLoc,
                NULL);
            GCPROTECT_END();
        } 
        else
        {
            RunML(pMLCode,
                pvManagedRetVal,
                pRetValDst,
                (UINT8*)pLoc,
                NULL);
        }
    }
    GCPROTECT_END();    // can trash FP return value

    return nativeRetVal;
}


//--------------------------------------------------------------------------
// For non-compiled UMThunk calls, this C routine does most of the work.
//--------------------------------------------------------------------------
PlatformDefaultReturnType __stdcall DoUMThunkCall(Thread *pThread, UMThkCallFrame *pFrame)
{
    // We should be in cooperative mode on entry.  
    // On all platforms, the various stubs should have transitioned into cooperative mode before calling this function.

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        ENTRY_POINT;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pThread));
        PRECONDITION(CheckPointer(pFrame));
    }
    CONTRACTL_END;



    PlatformDefaultReturnType nativeRetVal;
    ZeroMemory(&nativeRetVal, sizeof(nativeRetVal));


    const UMEntryThunk *pUMEntryThunk   = NULL;
    UMThunkMarshInfo* pUMThunkMarshInfo = NULL;
    UMThunkMLStub*    pheader           = NULL;


    // this method is called by stubs which are called by managed code,
    // so we need an unwind and continue handler so that our internal 
    // exceptions don't leak out into managed code.
    INSTALL_UNWIND_AND_CONTINUE_HANDLER; 

    ReverseEnterRuntimeHolder REHolder(TRUE);

    // These operations must be protected by the unwind and continue handler, which probes for SO.
    pUMEntryThunk     = pFrame->GetUMEntryThunk();
    pUMThunkMarshInfo = pUMEntryThunk->GetUMThunkMarshInfo();


    pheader           = (UMThunkMLStub*)(pUMThunkMarshInfo->GetMLStub()->GetEntryPoint());


    // Verify we are in the correct app domain
    BOOL fSwitchDomains = pThread->GetDomain()->GetId() != pUMEntryThunk->GetDomainId();
    if (fSwitchDomains)
    {
        ENTER_DOMAIN_ID(pUMEntryThunk->GetDomainId())
        {
            ContextTransitionFrame *pCtxTransitionFrame = GET_CTX_TRANSITION_FRAME();
            pCtxTransitionFrame->SetOwnedUMCallFrame(pFrame);        

            // Call the worker to do the actual work.            
            nativeRetVal = DoUMThunkCallWorker(pThread, pFrame, pUMEntryThunk, pUMThunkMarshInfo, pheader IA64_ARG(HFAReturnBuffer));
        }
        END_DOMAIN_TRANSITION;
    }
    else
    {
        // Call the worker to do the actual work.            
        nativeRetVal = DoUMThunkCallWorker(pThread, pFrame, pUMEntryThunk, pUMThunkMarshInfo, pheader IA64_ARG(HFAReturnBuffer));   
    }
    

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER; 

    // We must do this outside of the UNINSTALL_UNWIND_AND_CONTINUE_HANDLER macro, since it trashes
    // the floating point registers.
    if (pheader->m_wFlags & umtmlFpu)
    {
        setFPReturn(pheader->m_cbRetValSize, *(INT64*)&nativeRetVal);
    }



    return nativeRetVal;
}


ARG_SLOT ForwardCallToManagedMethod(const BYTE* pTargetMethodCode, ARG_SLOT* pArgStack, UINT32 sizeOfActualArgStack, BOOL fIsStatic,    // these are required
        MetaSig* pMetaSig, ArgumentRegisters* pArgRegs, void* pFPArgRegs, BYTE* pRetBuff, UINT fpReturnSize, UnmanagedToManagedCallGCInfo* pGCInfo)   // these are optional
{
    ARG_SLOT ManagedRetVal = 0;

    DWORD   arg = 0;
#ifdef CALLDESCR_REGTYPEMAP
    int     regArgNum = 0;
#endif

#ifdef DEBUGGING_SUPPORTED
    // Notify the debugger, if present, that we're calling into
    // managed code.
    if (CORDebuggerTraceCall())
        g_pDebugInterface->TraceCall(pTargetMethodCode);
#endif // DEBUGGING_SUPPORTED

    ARG_SLOT*   pArguments = pArgStack;
#ifdef CALLDESCR_RETBUF
    UINT64      cbRetBuff  = 0;
#endif // CALLDESCR_RETBUF

    if (!fIsStatic)
    {
        arg++;
#ifdef CALLDESCR_REGTYPEMAP
        regArgNum++;
#endif
    }

#ifdef CALLDESCR_RETBUFMARK
    if (pMetaSig->HasRetBuffArg())
    {
        // the CallDescrWorker callsite for methods with return buffer is 
        //  different for RISC CPUs - we pass this information along by setting 
        //  the lowest bit in pTarget
        pTargetMethodCode = (const BYTE*)((UINT_PTR)pTargetMethodCode | 0x1);
    }
#endif // CALLDESCR_RETBUFMARK

#ifdef ENREGISTERED_RETURNTYPE_MAXSIZE
    if (pMetaSig->HasRetBuffArg()) 
    {
#ifdef CALLDESCR_RETBUF
        cbRetBuff = pMetaSig->GetReturnTypeSize();
#endif
#if defined(CALLDESCR_REGTYPEMAP) && !defined(RETBUF_ARG_SPECIAL_PARAM)
        regArgNum++;
#endif
    }
#ifdef CALLDESCR_RETBUF
    else if (ELEMENT_TYPE_VALUETYPE == pMetaSig->GetReturnTypeNormalized())
    {
        ZeroMemory(pRetBuff, ENREGISTERED_RETURNTYPE_MAXSIZE);
        cbRetBuff = pMetaSig->GetReturnTypeSize();

        _ASSERTE(cbRetBuff <= ENREGISTERED_RETURNTYPE_MAXSIZE);
    }
#endif // CALLDESCR_RETBUF
#endif // ENREGISTERED_RETURNTYPE_MAXSIZE
    
#ifdef CALLDESCR_REGTYPEMAP
    UINT64      dwRegTypeMap    = 0;
    BYTE*       pMap            = (BYTE*)&dwRegTypeMap;

    //
    // this argit stuff is probably a bit too heavy-weight for what we need
    ArgIterator argit(NULL, pMetaSig, fIsStatic);

    //
    // Create a map of the first 8 argument types.  This is used 
    // in CallDescrWorkerInternal to load args into general registers
    // or floating point registers.
    //
    BYTE    typ;
    UINT32  structSize;
    int     fparg = 0;
    NOT_IA64(fparg = regArgNum);
    while ((regArgNum < NUM_ARGUMENT_REGISTERS) && (fparg < NUM_FP_ARG_REGISTERS) && 0 != (argit.GetNextOffsetFaster(&typ, &structSize)))
    {
        LOG((LF_INTEROP, LL_INFO100, "REGMAP: regArgNum: %d fparg: %d typ: %d structSize: %d\n", regArgNum, fparg, typ, structSize));
        
        int startRegArgNum = regArgNum;
        FillInRegTypeMap(&argit, pMap, &regArgNum, &typ, structSize);

        if ((ELEMENT_TYPE_R4 == typ))
        {
            LOG((LF_INTEROP, LL_INFO100, "REGMAP:     ELEMENT_TYPE_R4: startRegArgNum: %d fparg: %d\n", startRegArgNum, fparg));
            pArguments[startRegArgNum] = FPSpillToR4(&(((FPSPILL_SLOT*)pFPArgRegs)[fparg]));
            IA64_ONLY(fparg++);
        }
        else if ((ELEMENT_TYPE_R8 == typ))
        {
            LOG((LF_INTEROP, LL_INFO100, "REGMAP:     ELEMENT_TYPE_R8: startRegArgNum: %d fparg: %d\n", startRegArgNum, fparg));
            pArguments[startRegArgNum] = FPSpillToR8(&(((FPSPILL_SLOT*)pFPArgRegs)[fparg]));
            IA64_ONLY(fparg++);
        }
        else
        {
            LOG((LF_INTEROP, LL_INFO100, "REGMAP:     INTEGER\n"));
        }
        
        NOT_IA64(fparg++);
    }
#endif // CALLDESCR_REGTYPEMAP

#ifdef _PPC_
    FramedMethodFrame::Enregister(((BYTE*)pArguments) - sizeof(FramedMethodFrame),
        pMetaSig, fIsStatic, sizeOfActualArgStack);
#endif

    if (pGCInfo)
    {
        pGCInfo->DisableArgsGCProtection();
    }

    ManagedRetVal = CallDescrWorkerWithHandler(((BYTE*)pArguments)
#ifndef CALLDESCR_BOTTOMUP
                         + sizeOfActualArgStack
#endif
                         ,
                         sizeOfActualArgStack / STACK_ELEM_SIZE,
#ifdef CALLDESCR_ARGREGS
                         pArgRegs,
#endif
#ifdef CALLDESCR_REGTYPEMAP
                         dwRegTypeMap,
#endif
#ifdef CALLDESCR_RETBUF
                         pRetBuff,
                         cbRetBuff,
#endif
                         fpReturnSize,
                         (LPVOID)pTargetMethodCode,
                         FALSE);

    return ManagedRetVal;
}




static Stub * CreateUMThunkMLStubWorker(InteropStubLinker* psl,
                                        InteropStubLinker* pslPost,
                                        InteropStubLinker* pslRet,
                                        PInvokeStaticSigInfo* pSigInfo,
                                        const SigTypeContext *pTypeContext)
{
    CONTRACT (Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pSigInfo));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    Stub* pstub = NULL;

    MetaSig msig(pSigInfo->GetSig(), pSigInfo->GetSigCount(), pSigInfo->GetModule(), pTypeContext);
    MetaSig msig2(pSigInfo->GetSig(), pSigInfo->GetSigCount(), pSigInfo->GetModule(), pTypeContext);
    ArgIterator argit(NULL, &msig2, pSigInfo->IsStatic());
    UMThunkMLStub header;

    UINT numargs = msig.NumFixedArgs();


    header.m_cbRetPop    = 0;
    header.m_cbSrcStack  = 0;
    UINT cbDstStack = msig.SizeOfActualFixedArgStack(pSigInfo->IsStatic());
    if (cbDstStack != (UINT16)cbDstStack)
        COMPlusThrow(kMarshalDirectiveException, IDS_EE_SIGTOOCOMPLEX);

    header.m_cbDstStack  = (UINT16) cbDstStack;
    header.m_cbLocals    = 0;
    header.m_wFlags = 0;

    if (pSigInfo->IsStatic())
        header.m_wFlags |= umtmlIsStatic;

    if (pSigInfo->GetCallConv() == pmCallConvThiscall)
        header.m_wFlags |= umtmlThisCall;

    if (msig.HasRetBuffArg())
        header.m_wFlags |= umtmlHasReturnBuffer;

    if (msig.IsObjectRefReturnType())
        header.m_wFlags |= umtmlRetValRequiredGCProtect;

    if (pSigInfo->GetCallConv() != pmCallConvCdecl &&
        pSigInfo->GetCallConv() != pmCallConvStdcall &&
        pSigInfo->GetCallConv() != pmCallConvThiscall)
    {
        COMPlusThrow(kNotSupportedException, IDS_INVALID_PINVOKE_CALLCONV);
    }


    // Now, grab the param tokens if any. We'll get NATIVE_TYPE_* and [in,out] information
    // this way.
    IMDInternalImport *pInternalImport = pSigInfo->GetModule()->GetMDImport();

    mdParamDef *params = (mdParamDef*)_alloca( (numargs + 1) * sizeof(mdParamDef));
    CollateParamTokens(pInternalImport, pSigInfo->GetMethodToken(), numargs, params);

    if (!psl->EmitsIL())
    {
        // Emit space for the header. We'll go back and fill it in later.
        psl->MLEmitSpace(sizeof(header));
    }

    int curofs = 0;

    MarshalInfo::MarshalType marshaltype = (MarshalInfo::MarshalType) 0xcccccccc;

    MarshalInfo*    pReturnMLInfo = NULL;
    CorElementType  elemTypeReturnValue = msig.GetReturnType();

    if (elemTypeReturnValue != ELEMENT_TYPE_VOID) 
    {
        MarshalInfo mlinfo(pSigInfo->GetModule(),
                           msig.GetReturnProps(),
                           params[0],
                           MarshalInfo::MARSHAL_SCENARIO_NDIRECT,
                           pSigInfo->GetCharSet(),
                           0,FALSE,0, 
                            pSigInfo->GetBestFitMapping(), 
                            pSigInfo->GetThrowOnUnmappableChar(),
                           NULL); // wants MethodDesc*
        marshaltype = mlinfo.GetMarshalType();
        pReturnMLInfo = &mlinfo;

    }


    if (marshaltype > MarshalInfo::MARSHAL_TYPE_DOUBLE && IsUnsupportedValueTypeReturn(msig))
    {
        if (MarshalInfo::MARSHAL_TYPE_BLITTABLEVALUECLASS == marshaltype)
        {
            CONSISTENCY_CHECK_MSG(!psl->EmitsIL(), "IL STUBS NYI UMThunk returns blittable valuetype");
            
            MethodTable *pMT = msig.GetRetTypeHandleThrowing().AsMethodTable();
            UINT         managedSize = msig.GetRetTypeHandleThrowing().GetSize();
            UINT         unmanagedSize = pMT->GetNativeSize();

            if (header.m_wFlags & umtmlThisCall)
                header.m_wFlags |= umtmlThisCallHiddenArg;
            
            if (IsManagedValueTypeReturnedByRef(managedSize) && 
                ((header.m_wFlags & umtmlThisCall) || IsUnmanagedValueTypeReturnedByRef(unmanagedSize)))
            {
                int desiredofs = argit.GetRetBuffArgOffset();
#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
                desiredofs += StackElemSize(sizeof(LPVOID));
#endif
                if (curofs != desiredofs) 
                {
                    if (!psl->EmitsIL())
                    {
                        psl->MLEmit(ML_BUMPDST);
                        psl->Emit16( (INT16)(desiredofs - curofs) );
                    }
                    curofs = desiredofs;
                }

                if (!psl->EmitsIL())
                {
#ifdef RETBUF_ARG_SPECIAL_PARAM
                    INT32 cbFixup = UMThkCallFrame::GetOffsetOfRetBuffArg() - sizeof(UMThkCallFrame);
                    CONSISTENCY_CHECK(FitsInI2(cbFixup));
                    psl->Emit8(ML_BUMPSRC);
                    psl->Emit16((INT16)cbFixup);
#endif // RETBUF_ARG_SPECIAL_PARAM
                    
                    // propagate the hidden retval buffer pointer argument
                    psl->MLEmit(ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_N2C);

#ifdef RETBUF_ARG_SPECIAL_PARAM
                    psl->Emit8(ML_BUMPSRC);
                    psl->Emit16((INT16)-cbFixup - StackElemSize(sizeof(LPVOID)));
#endif // RETBUF_ARG_SPECIAL_PARAM

                    pslPost->MLEmit(ML_MARSHAL_RETVAL_LGBLITTABLEVALUETYPE_N2C_POST);
                    pslPost->Emit16(psl->MLNewLocal(sizeof(LPVOID)));
                }
#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
                curofs -= StackElemSize(sizeof(LPVOID));
#else
                curofs += StackElemSize(sizeof(LPVOID));
#endif

                header.m_cbSrcStack += StackElemSize(sizeof(LPVOID));
        
            }
        }
        else
        {
            COMPlusThrow(kMarshalDirectiveException, IDS_EE_NDIRECT_UNSUPPORTED_SIG);
        }
    }

    MarshalInfo *pMarshalInfo = (MarshalInfo*)_alloca(sizeof(MarshalInfo) * numargs);

    CorElementType mtype;
    UINT argidx = 0;
    while (ELEMENT_TYPE_END != (mtype = (msig.NextArg()))) 
    {
        UINT32 comargsize = 0;
        if (!psl->EmitsIL())
        {
            BYTE   type;
            int desiredofs = argit.GetNextOffset(&type, &comargsize);

#ifdef ENREGISTERED_PARAMTYPE_MAXSIZE
            if (comargsize > ENREGISTERED_PARAMTYPE_MAXSIZE)
                comargsize = sizeof(void*);
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE

#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
            desiredofs += StackElemSize(comargsize);
#endif
            if (curofs != desiredofs) 
            {
                if (!psl->EmitsIL())
                {
                    psl->MLEmit(ML_BUMPDST);
                    psl->Emit16( (INT16)(desiredofs - curofs) );
                }
                curofs = desiredofs;
            }
        }
        
        new (pMarshalInfo + argidx) MarshalInfo(pSigInfo->GetModule(),
                                                msig.GetArgProps(),
                                                params[argidx+1],
                                                MarshalInfo::MARSHAL_SCENARIO_NDIRECT,
                                                pSigInfo->GetCharSet(),
                                                0, TRUE, argidx+1, 
                                                pSigInfo->GetBestFitMapping(), 
                                                pSigInfo->GetThrowOnUnmappableChar(),
                                                NULL); // wants MethodDesc*

        pMarshalInfo[argidx].GenerateArgumentML(
                psl,
                pslPost,
                GENARGML_STACK_ARGUMENT,
                FALSE);

        header.m_cbSrcStack += pMarshalInfo[argidx].GetNativeArgSize();

        if (!psl->EmitsIL())
        {
#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
            curofs -= StackElemSize(comargsize);
#else
            curofs += StackElemSize(comargsize);
#endif
        }
        argidx++;
    }

    if (ELEMENT_TYPE_VOID == elemTypeReturnValue) 
    {
        header.m_cbRetValSize = 0;
    }
    else
    {
        if (marshaltype > MarshalInfo::MARSHAL_TYPE_DOUBLE && IsUnsupportedValueTypeReturn(msig)) 
        {
            if (marshaltype == MarshalInfo::MARSHAL_TYPE_BLITTABLEVALUECLASS)
            {
                CONSISTENCY_CHECK_MSG(!psl->EmitsIL(), "IL STUBS NYI UMThunk returns blittable valuetype");
                
                MethodTable *pMT = msig.GetRetTypeHandleThrowing().AsMethodTable();
                UINT         managedSize = msig.GetRetTypeHandleThrowing().GetSize();
                UINT         unmanagedSize = pMT->GetNativeSize();

                if (!(pMT->IsBlittable()))
                    COMPlusThrow(kTypeLoadException, IDS_EE_NDIRECT_UNSUPPORTED_SIG);

            
                CONSISTENCY_CHECK_MSG(managedSize == unmanagedSize, "blittable value type doesn't have the same size in managed and unmanaged!");

#if defined(ENREGISTERED_RETURNTYPE_MAXSIZE)
#if ENREGISTERED_RETURNTYPE_MAXSIZE > 8     // 8 is the max primitive type size

                if (!IsManagedValueTypeReturnedByRef(managedSize) && 
                    !IsUnmanagedValueTypeReturnedByRef(unmanagedSize))
                {
                    if (!psl->EmitsIL())
                    {
                        pslPost->MLEmit(ML_COPY_SMBLITTABLEVALUETYPE);
                        pslPost->Emit32(managedSize);
                    }

                    header.m_cbRetValSize = StackElemSize(unmanagedSize);
                }
                else
#endif // ENREGISTERED_RETURNTYPE_MAXSIZE > 8     // 8 is the max primitive type size
#endif // defined(ENREGISTERED_RETURNTYPE_MAXSIZE)
                if (IsManagedValueTypeReturnedByRef(managedSize) && 
                    ((header.m_wFlags & umtmlThisCall) || IsUnmanagedValueTypeReturnedByRef(unmanagedSize)))
                {
                    header.m_cbRetValSize = StackElemSize(sizeof(LPVOID));
                    // do nothing here: we propagated the hidden pointer argument above
                } 
                else if (IsManagedValueTypeReturnedByRef(managedSize) && 
                    !((header.m_wFlags & umtmlThisCall) || IsUnmanagedValueTypeReturnedByRef(unmanagedSize)))
                {
                    int desiredofs = argit.GetRetBuffArgOffset();
#ifdef STACK_GROWS_DOWN_ON_ARGS_WALK
                    desiredofs += StackElemSize(sizeof(LPVOID));
#endif
                    if (curofs != desiredofs) 
                    {
                        if (!psl->EmitsIL())
                        {
                            psl->MLEmit(ML_BUMPDST);
                            psl->Emit16( (INT16)(desiredofs - curofs) );
                        }
                        curofs = desiredofs;
                    }
                    if (!psl->EmitsIL())
                    {
                        // Push a return buffer large enough to hold the largest possible valuetype returned as
                        // a normal return value.
                        psl->MLEmit(ML_PUSHRETVALBUFFER8);
                    }
                    _ASSERTE(managedSize <= 8);
                    curofs -= StackElemSize(sizeof(LPVOID));

                    if (!psl->EmitsIL())
                    {
                        pslPost->MLEmit(ML_MARSHAL_RETVAL_SMBLITTABLEVALUETYPE_N2C);
                        pslPost->Emit32(managedSize);
                        pslPost->Emit16(psl->MLNewLocal(8));
                    }

                    header.m_cbRetValSize = StackElemSize(unmanagedSize);
                }
                else
                {
                    COMPlusThrow(kTypeLoadException, IDS_EE_NDIRECT_UNSUPPORTED_SIG);
                }
            }
            else
            {
                COMPlusThrow(kTypeLoadException, IDS_EE_NDIRECT_UNSUPPORTED_SIG);
            }
        } 
        else 
        {
            pReturnMLInfo->GenerateReturnML(psl, pslPost, FALSE, FALSE);
            header.m_cbRetValSize = pReturnMLInfo->GetNativeSize();
            if (pReturnMLInfo->IsFpuReturn())
            {
                header.m_wFlags |= umtmlFpu;
            }
        }

    } 

    if (msig.IsVarArg())
    {
        _ASSERTE(!psl->EmitsIL() && "NYI: nexport vararg");
        
        if (!psl->EmitsIL())
        {
            psl->MLEmit(ML_PUSHVASIGCOOKIEEX);
            psl->Emit16(header.m_cbDstStack);
            psl->MLNewLocal(sizeof(VASigCookieEx));
        }
    }

    if (!psl->EmitsIL())
    {
        psl->MLEmit(ML_INTERRUPT);
        pslPost->MLEmit(ML_END);
    }

    StubHolder<Stub> pStubPost(pslPost->Link());
    psl->EmitBytes(pStubPost->GetEntryPoint(), MLStreamLength((const UINT8 *)(pStubPost->GetEntryPoint())) - 1);

    psl->MLEmit(ML_END);
    pstub = psl->Link();
    header.m_cbLocals = psl->GetLocalSize();
  
#if defined(_X86_) || defined(_AMD64_)
    if (pSigInfo->GetCallConv() == pmCallConvCdecl) 
        header.m_cbRetPop = 0;
    else 
        header.m_cbRetPop = header.m_cbSrcStack;
#else
    header.m_cbRetPop = 0;
#endif

    *((UMThunkMLStub *)(pstub->GetEntryPoint())) = header;
    PatchMLStubForSizeIs( sizeof(header) + (BYTE*)pstub->GetEntryPoint(),
                          numargs,
                          pMarshalInfo);
    RETURN pstub;
}



//---------------------------------------------------------
// Creates a new stub for a N/Export call. Return refcount is 1.
// If failed, returns NULL and sets *ppException to an exception
// object.
//---------------------------------------------------------
Stub * CreateUMThunkMLStub(PInvokeStaticSigInfo* pSigInfo, const SigTypeContext *pTypeContext)
{
    CONTRACT (Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pSigInfo));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;
    
    InteropStubLinker sl(StubTypeML);
    InteropStubLinker slPost(StubTypeML);
    InteropStubLinker slRet(StubTypeML);

    RETURN CreateUMThunkMLStubWorker(&sl, &slPost, &slRet, pSigInfo, pTypeContext);
}

//--------------------------------------------------------------------------
// Cache ML & compiled stubs for UMThunks.
//--------------------------------------------------------------------------
class UMThunkStubCache : public MLStubCache
{
public:
    UMThunkStubCache() : MLStubCache()
    {
        WRAPPER_CONTRACT;
    }
    
    private:
        virtual MLStubCompilationMode CompileMLStub(const BYTE *pRawMLStub,
                                                    StubLinker *psl,
                                                    void *callerContext)
        {
            CONTRACTL
            {
                THROWS;
                GC_NOTRIGGER;
                MODE_ANY;
                INJECT_FAULT(COMPlusThrowOM());
                PRECONDITION(CheckPointer(pRawMLStub));
                PRECONDITION(CheckPointer(psl));
            }
            CONTRACTL_END;

#ifdef _X86_
            UMThunkMLStub *pheader = (UMThunkMLStub *)pRawMLStub;
            CPUSTUBLINKER *pcpusl = (CPUSTUBLINKER*)psl;
            UINT           psrcofsregs[NUM_ARGUMENT_REGISTERS];
            UINT          *psrcofs = (UINT*)_alloca(sizeof(UINT) * (pheader->m_cbDstStack/STACK_ELEM_SIZE));

#ifdef _DEBUG
            if (LoggingEnabled() && (g_pConfig->GetConfigDWORD(L"LogFacility",0) & LF_IJW))
               return INTERPRETED;
#endif
            if (pheader->m_wFlags & umtmlFpu)
                return INTERPRETED;

#ifdef DEBUGGING_SUPPORTED
            // Always use the slow stubs if a debugger is attached. This provides for optimal stepping and stack tracing
            // behavior, since these optimized stubs don't push nice Frame objects like the debugger needs.
            if (CORDebuggerAttached())
                return INTERPRETED;
#endif

            for (int i = 0; i < NUM_ARGUMENT_REGISTERS; i++)
                psrcofsregs[i] = (UINT)(-1);

            const MLCode  *pMLCode = pheader->GetMLCode();

            MLCode opcode;
            int    dstofs = 0;
            int    srcofs = 0;
            while ((opcode = *(pMLCode++)) != ML_INTERRUPT)
            {
                switch (opcode)
                {
                    case ML_COPY4:
                    case ML_COPYPINNEDGCREF:
                        dstofs -= StackElemSize(4);
                        if (dstofs > 0)
                            psrcofs[(dstofs - sizeof(FramedMethodFrame))/STACK_ELEM_SIZE] = srcofs;
                        else
                        {
                            psrcofsregs[ (dstofs - FramedMethodFrame::GetOffsetOfArgumentRegisters()) / STACK_ELEM_SIZE ] = srcofs;
                        }
                        srcofs += StackElemSize(4);
                        break;

                    case ML_BUMPDST:
                        dstofs += *( ((INT16*&)pMLCode)++ );
                        break;

                    default:
                        return INTERPRETED;
                }
            }
            
            if (pMLCode[0] == ML_END || (pMLCode[0] == ML_COPY4 && pMLCode[1] == ML_END))
            {
                // continue onward
            }
            else
            {
                return INTERPRETED;
            }

            CodeLabel* pSetupThreadLabel    = pcpusl->NewCodeLabel();
            CodeLabel* pRejoinThreadLabel   = pcpusl->NewCodeLabel();
            CodeLabel* pDisableGCLabel      = pcpusl->NewCodeLabel();
            CodeLabel* pRejoinGCLabel       = pcpusl->NewCodeLabel();
            CodeLabel* pDoADCallBackLabel = pcpusl->NewCodeLabel();
            CodeLabel* pDoneADCallBackLabel = pcpusl->NewCodeLabel();
            CodeLabel* pDoADCallBackTargetLabel = pcpusl->NewAbsoluteCodeLabel();
            CodeLabel* pDoADCallBackStartLabel = pcpusl->NewCodeLabel();

            // We come into this code with UMEntryThunk in EAX
            const X86Reg kEAXentryThunk = kEAX;

#ifdef _DEBUG
            // Save incoming registers
            pcpusl->X86EmitPushReg(kEAXentryThunk); // UMEntryThunk
            pcpusl->X86EmitPushReg(kECX); // "this" pointer for a ThisCall calling convention
            
            pcpusl->X86EmitPushReg(kEAXentryThunk);
            pcpusl->X86EmitCall(pcpusl->NewExternalCodeLabel((LPVOID) LogUMTransition), 4);

            // Restore registers
            pcpusl->X86EmitPopReg(kECX);
            pcpusl->X86EmitPopReg(kEAXentryThunk);
#endif

            // For ThisCall, we make it look like a normal stdcall so that
            // the rest of the code (like repushing the arguments) does not
            // have to worry about it.

            if (pheader->m_wFlags & umtmlThisCall)
            {
                // pop off the return address into EDX
                pcpusl->X86EmitPopReg(kEDX);
                
                if (pheader->m_wFlags & umtmlThisCallHiddenArg)
                {
                    // exchange ecx ( "this") with the hidden structure return buffer
                    //  xchg ecx, [esp]
                    pcpusl->X86EmitOp(0x87, kECX, (X86Reg)4 /*ESP*/);
                }

                // jam ecx (the "this" param onto stack. Now it looks like a normal stdcall.)
                pcpusl->X86EmitPushReg(kECX);

                // push edx - repush the return address
                pcpusl->X86EmitPushReg(kEDX);
            }

            // Setup the EBP frame
            pcpusl->X86EmitPushEBPframe();
            
            // Load thread descriptor into ECX
            const X86Reg kECXthread = kECX;
            pcpusl->X86EmitTLSFetch(GetThreadTLSIndex(), kECXthread, (1 << kEAXentryThunk));

            // test ecx,ecx
            pcpusl->Emit16(0xc985);

            // jz SetupThread
            pcpusl->X86EmitCondJump(pSetupThreadLabel, X86CondCode::kJZ);
            pcpusl->EmitLabel(pRejoinThreadLabel);

#ifdef PROFILING_SUPPORTED

            X86Reg kEBXprofMD = NumX86Regs;

            // Notify profiler of transition into runtime, before we disable preemptive GC
            if (CORProfilerTrackTransitions())
            {
                // Save EBX and use it to hold on to the MethodDesc
                pcpusl->X86EmitPushReg(kEBX);
                kEBXprofMD = kEBX;

                // Load the methoddesc into EBX (UMEntryThunk->m_pMD)
                pcpusl->X86EmitIndexRegLoad(kEBXprofMD, kEAXentryThunk, UMEntryThunk::GetOffsetOfMethodDesc());

                // Save registers
                pcpusl->X86EmitPushReg(kEAXentryThunk); // UMEntryThunk
                pcpusl->X86EmitPushReg(kECXthread); // pCurThread

                // Push arguments and notify profiler
                pcpusl->X86EmitPushImm32(COR_PRF_TRANSITION_CALL);    // Reason
                pcpusl->X86EmitPushReg(kEBXprofMD);          // MethodDesc*
                pcpusl->X86EmitCall(pcpusl->NewExternalCodeLabel((LPVOID)ProfilerUnmanagedToManagedTransitionMD), 8);

                // Restore registers
                pcpusl->X86EmitPopReg(kECXthread);
                pcpusl->X86EmitPopReg(kEAXentryThunk);
            }
#endif // PROFILING_SUPPORTED

            pcpusl->EmitDisable(pDisableGCLabel, TRUE, kECXthread);

            pcpusl->EmitLabel(pRejoinGCLabel);

            // It's important that the "restart" after an AppDomain switch will skip
            // the check for g_TrapReturningThreads.  That's because, during shutdown,
            // we can only go through the UMThunkStubRareDisable pathway if we have
            // not yet pushed a frame.  (Once pushed, the frame cannot be popped
            // without coordinating with the GC.  During shutdown, such coordination
            // would deadlock).
            pcpusl->EmitLabel(pDoADCallBackStartLabel);

            // push [ECX]Thread.m_pFrame - corresponding to FrameHandlerExRecord::m_pEntryFrame
            pcpusl->X86EmitOffsetModRM(0xff, (X86Reg)6, kECXthread, offsetof(Thread, m_pFrame));                


            // allocate SEH frame
            pcpusl->X86EmitSubEsp(sizeof(FrameHandlerExRecord) - 
                                  sizeof((FrameHandlerExRecord*)NULL)->m_pEntryFrame);

            // mov edx,esp
            pcpusl->Emit16(0xd48b);

            // save registers
            pcpusl->X86EmitPushReg(kEAXentryThunk); // UMThunk
            pcpusl->X86EmitPushReg(kECXthread);

            // mov [edx]EXCEPTION_REGISTRATION_RECORD.Handler, FastNExportExceptHandler
            pcpusl->X86EmitOffsetModRM(0xc7, (X86Reg)0, kEDX, offsetof(FrameHandlerExRecord, m_ExReg.Handler));
            pcpusl->Emit32((INT32)(size_t)FastNExportExceptHandler);

            // mov [edx]EXCEPTION_REGISTRATION_RECORD.pvFilterParameter, edx
            pcpusl->X86EmitOffsetModRM(0x89, kEDX, kEDX, offsetof(FrameHandlerExRecord, m_ExReg.pvFilterParameter));

            // mov [edx]EXCEPTION_REGISTRATION_RECORD.dwFlags, PAL_EXCEPTION_FLAGS_UNWINDONLY
            pcpusl->X86EmitOffsetModRM(0xc7, (X86Reg)0, kEDX, offsetof(FrameHandlerExRecord, m_ExReg.dwFlags));
            pcpusl->Emit32(PAL_EXCEPTION_FLAGS_UNWINDONLY);

            // mov [edx]EXCEPTION_REGISTRATION_RECORD.typeOfHandler, PALExceptFilter
            pcpusl->X86EmitOffsetModRM(0xc7, (X86Reg)0, kEDX, offsetof(FrameHandlerExRecord, m_ExReg.typeOfHandler));
            pcpusl->Emit32((INT32)PALExceptFilter);

            // call PAL_TryHelper
            pcpusl->X86EmitPushReg(kEDX);
            pcpusl->X86EmitCall(pcpusl->NewExternalCodeLabel((LPVOID) PAL_TryHelper), 4);
       
            // restore registers
            pcpusl->X86EmitPopReg(kECXthread);
            pcpusl->X86EmitPopReg(kEAXentryThunk);
            

            // save the thread pointer
            pcpusl->X86EmitPushReg(kECXthread);

            // Load pThread->m_pDomain into edx
            // mov edx,[ecx + offsetof(Thread, m_pAppDomain)]
            pcpusl->X86EmitIndexRegLoad(kEDX, kECXthread, Thread::GetOffsetOfAppDomain());

            // Load pThread->m_pAppDomain->m_dwId into edx
            // mov edx,[edx + offsetof(AppDomain, m_dwId)]
            pcpusl->X86EmitIndexRegLoad(kEDX, kEDX, AppDomain::GetOffsetOfId());

            // check if the app domain of the thread matches that of delegate
            // cmp edx,[eax + offsetof(UMEntryThunk, m_dwDomainId))]
            pcpusl->X86EmitOffsetModRM(0x3b, kEDX, kEAXentryThunk, offsetof(UMEntryThunk, m_dwDomainId));

            pcpusl->X86EmitCondJump(pDoADCallBackLabel, X86CondCode::kJNE);

            if (NDirect::IsHostHookEnabled())
            {
                // We call ReverseEnterRuntimeHelper before we link a frame.
                // So we know that when exception unwinds through our ReverseEnterRuntimeFrame,
                // we need call ReverseLeaveRuntime.
                
                // save UMEntryThunk
                pcpusl->X86EmitPushReg(kEAXentryThunk);
                
                // ecx still has Thread
                // ReverseEnterRuntimeHelper is a fast call
                pcpusl->X86EmitCall(pcpusl->NewExternalCodeLabel((LPVOID)ReverseEnterRuntimeHelper), 0);
                
                // restore EAX
                pcpusl->X86EmitPopReg(kEAXentryThunk);
                
                // The thread pointer has been saved on the stack. However, we want to push
                // the ReverseEnterRuntimeHelper. So we pop it off, and will re-push it
                pcpusl->X86EmitPopReg(kECXthread);

                // push reg; leave room for m_next
                pcpusl->X86EmitPushReg(kDummyPushReg);

                // push IMM32 ; push Frame vptr
                pcpusl->X86EmitPushImm32((UINT32)(size_t)ReverseEnterRuntimeFrame::GetReverseEnterRuntimeFrameVPtr());
                
                // mov edx, esp  ;; set EDX -> new frame
                pcpusl->X86EmitMovRegSP(kEDX);

                // push IMM32  ; push gsCookie
                pcpusl->X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());
                
                // Re-push the thread pointer (now that the ReverseEnterRuntimeHelper has been pushed)
                pcpusl->X86EmitPushReg(kECXthread);
                
                // save UMEntryThunk
                pcpusl->X86EmitPushReg(kEAXentryThunk);
                
                // mov eax,[ecx + Thread.GetFrame()]  ;; get previous frame
                pcpusl->X86EmitIndexRegLoad(kEAXentryThunk, kECXthread, Thread::GetOffsetOfCurrentFrame());
            
                // mov [edx + Frame.m_next], eax
                pcpusl->X86EmitIndexRegStore(kEDX, Frame::GetOffsetOfNextLink(), kEAX);
            
                // mov [ecx + Thread.GetFrame()], edx
                pcpusl->X86EmitIndexRegStore(kECXthread, Thread::GetOffsetOfCurrentFrame(), kEDX);
                
                // restore EAX
                pcpusl->X86EmitPopReg(kEAXentryThunk);
            }
  
            // repush any stack arguments
            int arg = pheader->m_cbDstStack/STACK_ELEM_SIZE;
            
            int argStartOfs = sizeof(void*) + // return address
                            sizeof(PCONTEXT(NULL)->Ebp) + //  caller's EBP
#ifdef PROFILING_SUPPORTED
                            (CORProfilerTrackTransitions() ? sizeof(PCONTEXT(NULL)->Ebx) : 0) + // caller's saved EBX
#endif // PROFILING_SUPPORTED
                            sizeof(FrameHandlerExRecord) + // exception frame
                            sizeof(Thread*); // thread
                            
            int argOfs = argStartOfs +
                // If we are hosted, we have installed a frame on stack
                (NDirect::IsHostHookEnabled() ? 
                    (ReverseEnterRuntimeFrame::GetNegSpaceSize() + sizeof(ReverseEnterRuntimeFrame)) : 0);

            while (arg--)
            {
                // push dword ptr [esp + ofs]
                pcpusl->X86EmitEspOffset(0xff, (X86Reg)6, argOfs + psrcofs[arg]);
                argOfs += 4;
            }

            // load register arguments
            int regidx = 0;
        
#define DEFINE_ARGUMENT_REGISTER_BACKWARD(regname) \
            if (psrcofsregs[regidx] != (UINT)(-1)) \
            { \
                pcpusl->X86EmitEspOffset(0x8b, k##regname, argOfs + psrcofsregs[regidx]); \
            } \
            regidx++; 

#include "eecallconv.h"

            if (!(pheader->m_wFlags & umtmlIsStatic))
            {
                //
                // This is call on delegate
                //

                // mov THIS, [EAX + UMEntryThunk.m_pObjectHandle]
                pcpusl->X86EmitOp(0x8b, THIS_kREG, kEAXentryThunk, offsetof(UMEntryThunk, m_pObjectHandle));

                // mov THIS, [THIS]
                pcpusl->X86EmitOp(0x8b, THIS_kREG, THIS_kREG);

                //
                // Inline Delegate.Invoke for perf
                //

                // mov SCRATCHREG, [THISREG + Delegate.FP]  ; Save target stub in register
                pcpusl->X86EmitIndexRegLoad(SCRATCH_REGISTER_X86REG, THIS_kREG, DelegateObject::GetOffsetOfMethodPtr());

                // mov THISREG, [THISREG + Delegate.OR]  ; replace "this" pointer
                pcpusl->X86EmitIndexRegLoad(THIS_kREG, THIS_kREG, DelegateObject::GetOffsetOfTarget());

                // call SCRATCHREG
                pcpusl->Emit16(0xd0ff);
                INDEBUG(pcpusl->Emit8(X86_INSTR_NOP)); // Emit a NOP so we know that we can call managed code
            }
            else
            {
                //
                // Call the managed method
                //
            
                // call [SCRATCH+UMEntryThunk.m_pManagedTarget]
                pcpusl->X86EmitOp(0xff, (X86Reg)2, kEAXentryThunk, offsetof(UMEntryThunk, m_pManagedTarget));
                INDEBUG(pcpusl->Emit8(X86_INSTR_NOP)); // Emit a NOP so we know that we can call managed code
            }

            // restore the thread pointer
            pcpusl->X86EmitPopReg(kECXthread);

            if (NDirect::IsHostHookEnabled()) 
            {
#ifdef _DEBUG
                // lea edx, [esp + sizeof(GSCookie)] ; edx <- current Frame
                pcpusl->X86EmitEspOffset(0x8d, kEDX, sizeof(GSCookie));
                pcpusl->EmitCheckGSCookie(kEDX, ReverseEnterRuntimeFrame::GetOffsetOfGSCookie());
#endif
                
                // Remove our frame
                // Get the previous frame into EDX
                // mov edx, [esp + GSCookie + Frame.m_next]
                static const BYTE initArg1[] = { 0x8b, 0x54, 0x24, 0x08 }; // mov edx, [esp+8]
                _ASSERTE(ReverseEnterRuntimeFrame::GetNegSpaceSize() + Frame::GetOffsetOfNextLink() == 0x8);
                pcpusl->EmitBytes(initArg1, sizeof(initArg1));

                // mov [ecx + Thread.GetFrame()], edx
                pcpusl->X86EmitIndexRegStore(kECXthread, Thread::GetOffsetOfCurrentFrame(), kEDX);

                // pop off stack
                // add esp, 8
                pcpusl->X86EmitAddEsp(sizeof(GSCookie) + sizeof(ReverseEnterRuntimeFrame));

                // Save EAX for the return value
                pcpusl->X86EmitPushReg(kEAX);
                // Save pThread
                pcpusl->X86EmitPushReg(kECXthread);

                // ReverseEnterRuntimeHelper is a fast call
                pcpusl->X86EmitCall(pcpusl->NewExternalCodeLabel((LPVOID)ReverseLeaveRuntimeHelper), 0);

                // Restore pThread
                pcpusl->X86EmitPopReg(kECXthread);
                // Restore EAX
                pcpusl->X86EmitPopReg(kEAX);
            }
            
            // move byte ptr [ecx + Thread.m_fPreemptiveGCDisabled],0
            pcpusl->X86EmitOffsetModRM(0xc6, (X86Reg)0, kECXthread, Thread::GetOffsetOfGCFlag());
            pcpusl->Emit8(0);

            CodeLabel *pRareEnable;//,  *pEnableRejoin;
            pRareEnable    = pcpusl->NewCodeLabel();
            
            // test byte ptr [ecx + Thread.m_State], TS_CatchAtSafePoint
            pcpusl->X86EmitOffsetModRM(0xf6, (X86Reg)0, kECXthread, Thread::GetOffsetOfState());
            pcpusl->Emit8(Thread::TS_CatchAtSafePoint);

            pcpusl->X86EmitCondJump(pRareEnable,X86CondCode::kJNZ);

            pcpusl->EmitLabel(pDoneADCallBackLabel);

            // *** unhook SEH frame
            // mov edx,esp
            pcpusl->Emit16(0xd48b);

            // save registers
            pcpusl->X86EmitPushReg(kEAX); // return value
            pcpusl->X86EmitPushReg(kECXthread);

            // call PAL_EndTryHelper(edx, 0)
            pcpusl->X86EmitPushImm32(0);
            pcpusl->X86EmitPushReg(kEDX);
            pcpusl->X86EmitCall(pcpusl->NewExternalCodeLabel((LPVOID) PAL_EndTryHelper), 8);
            
            // restore registers
            pcpusl->X86EmitPopReg(kECXthread);
            pcpusl->X86EmitPopReg(kEAX);

            // deallocate SEH frame
            pcpusl->X86EmitAddEsp(sizeof(FrameHandlerExRecord));

#ifdef PROFILING_SUPPORTED
            if (CORProfilerTrackTransitions())
            {
                // if ebx is 0, then we're here on the inner part of the AD transition callback
                // so don't want to track the transition as aren't leaving.
                // test ebx,ebx
                pcpusl->Emit16(0xdb85);

                // jz SetupThread
                CodeLabel* pSkipOnInnerADCallback = pcpusl->NewCodeLabel();
                _ASSERTE(pSkipOnInnerADCallback);
                pcpusl->X86EmitCondJump(pSkipOnInnerADCallback, X86CondCode::kJZ);

                // Save registers
                pcpusl->X86EmitPushReg(kEAX); // return value
                pcpusl->X86EmitPushReg(kECXthread);

                // Push arguments and notify profiler
                pcpusl->X86EmitPushImm32(COR_PRF_TRANSITION_RETURN);    // Reason
                pcpusl->X86EmitPushReg(kEBXprofMD); // MethodDesc*
                pcpusl->X86EmitCall(pcpusl->NewExternalCodeLabel((LPVOID)ProfilerManagedToUnmanagedTransitionMD), 8);

                // Restore registers
                pcpusl->X86EmitPopReg(kECXthread);
                pcpusl->X86EmitPopReg(kEAX);

                pcpusl->EmitLabel(pSkipOnInnerADCallback);

                // Restore EBX, which was saved in prolog
                pcpusl->X86EmitPopReg(kEBXprofMD);
            }
#endif // PROFILING_SUPPORTED

            pcpusl->X86EmitPopEBPframe(false);

            //retn n
            pcpusl->X86EmitReturn(pheader->m_cbRetPop);

            //-------------------------------------------------------------
            // coming here if the thread is not set up yet
            //
            
            pcpusl->EmitLabel(pSetupThreadLabel);

            // save UMEntryThunk
            pcpusl->X86EmitPushReg(kEAXentryThunk);

            // call CreateThreadBlock
            pcpusl->X86EmitCall(pcpusl->NewExternalCodeLabel((LPVOID) CreateThreadBlockThrow), 0);

            // mov ecx,eax
            pcpusl->Emit16(0xc189);

            // restore UMEntryThunk
            pcpusl->X86EmitPopReg(kEAXentryThunk);

            // jump back into the main code path
            pcpusl->X86EmitNearJump(pRejoinThreadLabel);

            //-------------------------------------------------------------
            // coming here if g_TrapReturningThreads was true
            //
            
            pcpusl->EmitLabel(pDisableGCLabel);

            // call UMThunkStubRareDisable.  This may throw if we are not allowed
            // to enter.  Note that we have not set up our SEH yet (deliberately).
            // This is important to handle the case where we cannot enter the CLR
            // during shutdown and cannot coordinate with the GC because of
            // deadlocks.
            pcpusl->X86EmitCall(pcpusl->NewExternalCodeLabel((LPVOID) UMThunkStubRareDisable), 0);

            // jump back into the main code path
            pcpusl->X86EmitNearJump(pRejoinGCLabel);

            //-------------------------------------------------------------
            // coming here if appdomain didn't match
            //
            
            pcpusl->EmitLabel(pDoADCallBackLabel);
                            
            // we will call DoADCallBack which calls into managed code to switch ADs and then calls us
            // back. So when come in the second time the ADs will match and just keep processing.
            // So we need to setup the parms to pass to DoADCallBack one of which is an address inside
            // the stub that will branch back to the top of the stub to start again. Need to setup
            // the parms etc so that when we return from the 2nd call we pop things properly.

            // push values for UM2MThunk_Args

            // mov edx, esp ; get stack pointer
            pcpusl->Emit16(0xD48b);
           
            // push address of args
            pcpusl->X86EmitAddReg(kEDX, argStartOfs);    

            // size of args
            pcpusl->X86EmitPushImm32(pheader->m_cbSrcStack);

            // address of args
            pcpusl->X86EmitPushReg(kEDX);
            
            // addr to call
            pcpusl->X86EmitPushImm32(*pDoADCallBackTargetLabel);

            // UMEntryThunk
            pcpusl->X86EmitPushReg(kEAXentryThunk);

            // call UM2MDoADCallBack
            pcpusl->X86EmitCall(pcpusl->NewExternalCodeLabel((LPVOID) UM2MDoADCallBack), 8);

            // restore the thread pointer
            pcpusl->X86EmitPopReg(kECXthread);

            // move byte ptr [ecx + Thread.m_fPreemptiveGCDisabled],0
            pcpusl->X86EmitOffsetModRM(0xc6, (X86Reg)0, kECXthread, Thread::GetOffsetOfGCFlag());
            pcpusl->Emit8(0);

            // test byte ptr [ecx + Thread.m_State], TS_CatchAtSafePoint
            pcpusl->X86EmitOffsetModRM(0xf6, (X86Reg)0, kECXthread, Thread::GetOffsetOfState());
            pcpusl->Emit8(Thread::TS_CatchAtSafePoint);

            pcpusl->X86EmitCondJump(pDoneADCallBackLabel, X86CondCode::kJZ);

            pcpusl->EmitLabel(pRareEnable);

            // Thread object is expected to be in EBX. So first save caller's EBP
            pcpusl->X86EmitPushReg(kEBX);
            // mov ebx, ecx
            pcpusl->X86EmitMovRegReg(kEBX, kECXthread);

            pcpusl->EmitRareEnable(NULL);
            
            // restore ebx
            pcpusl->X86EmitPopReg(kEBX);

            // return to mainline of function
            pcpusl->X86EmitNearJump(pDoneADCallBackLabel);

            //-------------------------------------------------------------
            // coming here on DoADCallBack
            //
            
            pcpusl->EmitLabel(pDoADCallBackTargetLabel);

            // Push an EBP frame, just like the prolog of this stub does.
            pcpusl->X86EmitPushEBPframe();

#ifdef PROFILING_SUPPORTED
            if (CORProfilerTrackTransitions())
            {
                // Save EBX and set it to null so know that when return
                // we should not track profiler call - leave it until the outer return
                // code assumes that EBX has been saved already anyway
                pcpusl->X86EmitPushReg(kEBXprofMD);
                // xor ebx, ebx
                pcpusl->X86EmitZeroOutReg(kEBXprofMD);
            }
#endif // PROFILING_SUPPORTED

            // eax will contain the UMThunkEntry
            pcpusl->X86EmitNearJump(pDoADCallBackStartLabel);

            return STANDALONE;
#endif // _X86_
        }
        
        virtual UINT Length(const BYTE *pRawMLStub)
        {
            WRAPPER_CONTRACT;
            UMThunkMLStub *pmlstub = (UMThunkMLStub *)pRawMLStub;
            return sizeof(UMThunkMLStub) + MLStreamLength(pmlstub->GetMLCode());
        }
};


void RunTimeInit_Wrapper(LPVOID /* UMThunkMarshInfo * */ ptr)
{
    WRAPPER_CONTRACT;

    UMEntryThunk::DoRunTimeInit((UMEntryThunk*)ptr);
}


// asm entrypoint
void __stdcall UMEntryThunk::DoRunTimeInit(UMEntryThunk* pUMEntryThunk)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        ENTRY_POINT;
        PRECONDITION(CheckPointer(pUMEntryThunk));
    }
    CONTRACTL_END;

    // this method is called by stubs which are called by managed code,
    // so we need an unwind and continue handler so that our internal 
    // exceptions don't leak out into managed code.
    INSTALL_UNWIND_AND_CONTINUE_HANDLER; 

    // The thread object is guaranteed to have been set up at this point.
    Thread *pThread = GetThread();

    if (pThread->GetDomain()->GetId() != pUMEntryThunk->GetDomainId()) 
    {
        // call ourselves again through DoCallBack with a domain transition
        pThread->DoADCallBack(pUMEntryThunk->GetDomainId(), RunTimeInit_Wrapper, pUMEntryThunk);
    }
    else
    {
        pUMEntryThunk->RunTimeInit();
    }
    
    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER; 
}

UMEntryThunk* UMEntryThunk::CreateUMEntryThunk()
{
    CONTRACT (UMEntryThunk*)
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    MEMORY_REPORT_CONTEXT_SCOPE("UMThunk");

    UMEntryThunk *p = new (executable) UMEntryThunk;
    memset (p, 0, sizeof(*p));
    RETURN p;
}


VOID UMEntryThunk::FreeUMEntryThunk(UMEntryThunk* p)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(p));
    }
    CONTRACTL_END;

    {
        GCX_COOP();
        DeleteExecutable(p);
    }
}

UMThunkMarshInfo::~UMThunkMarshInfo()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE(IsAtLeastLoadTimeInited() || m_state == 0);

    if (m_pMLStub)
        m_pMLStub->DecRef();

    if (m_pExecStub)
        m_pExecStub->DecRef();

#ifdef _DEBUG
    FillMemory(this, sizeof(*this), 0xcc);
#endif
}


//----------------------------------------------------------
// This initializer can be called during load time.
// It does not do any ML stub initialization or sigparsing.
// The RunTimeInit() must be called subsequently before this
// can safely be used.
//----------------------------------------------------------
VOID UMThunkMarshInfo::LoadTimeInit(MethodDesc* pMD)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(!IsCompletelyInited());
    }
    CONTRACTL_END;
    
    FillMemory(this, sizeof(UMThunkMarshInfo), 0); // Prevent problems with partial deletes
    
    PInvokeStaticSigInfo sigInfo(pMD, PInvokeStaticSigInfo::NO_THROW_ON_ERROR);
    SigTypeContext::InitTypeContext(pMD, &m_typeContext);
    
    _LoadTimeInit(&sigInfo);

}
                                    
VOID UMThunkMarshInfo::LoadTimeInit(PInvokeStaticSigInfo* pSigInfo)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(!IsCompletelyInited());
    }
    CONTRACTL_END;
    
    FillMemory(this, sizeof(UMThunkMarshInfo), 0); // Prevent problems with partial deletes
    _LoadTimeInit(pSigInfo);
}

VOID UMThunkMarshInfo::_LoadTimeInit(PInvokeStaticSigInfo* pSigInfo)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(!IsCompletelyInited());
    }
    CONTRACTL_END;

    m_sigInfo = *pSigInfo;

    // These fields must be explicitly NULL'd out for the atomic
    // FastInterlockCompareExchange update to work during the runtime init.
    m_pMLStub   = NULL;
    m_pExecStub = NULL;

#ifdef _DEBUG
    m_cbRetPop        = 0xcccccccc;
    m_cbActualArgSize = 0xcccccccc;
#endif

    // Must be the last thing we set!
    m_state        = kLoadTimeInited;
}


//----------------------------------------------------------
// This initializer finishes the init started by LoadTimeInit.
// It does all the ML stub creation, and can throw a COM+
// exception.
//
// It can safely be called multiple times and by concurrent
// threads.
//----------------------------------------------------------
VOID UMThunkMarshInfo::RunTimeInit()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(IsAtLeastLoadTimeInited());
    }
    CONTRACTL_END;

    // this function is called by CLR to native assembly stubs which are called by 
    // managed code as a result, we need an unwind and continue handler to translate 
    // any of our internal exceptions into managed exceptions.
    INSTALL_UNWIND_AND_CONTINUE_HANDLER;

    if (m_state != kRunTimeInited)
    {
        m_sigInfo.ReportErrors();
        

        Stub *pFinalMLStub = NULL;
        Stub *pFinalExecStub = NULL;


        //
        // m_cbActualArgSize gets the number of arg bytes for the MANAGED signature 
        //
        
        //@todo GENERICS: instantiations? </STRIP>
        m_cbActualArgSize = MetaSig::SizeOfFrameArgumentArray(
            m_sigInfo.GetModule(), 
            m_sigInfo.GetSig(), 
            m_sigInfo.GetSigCount(),
            m_sigInfo.IsStatic(), 
            &m_typeContext);

        Stub     *pMLStream;
        pMLStream = CreateUMThunkMLStub(&m_sigInfo, &m_typeContext);
    
        m_cbRetPop = ( (UMThunkMLStub*)(pMLStream->GetEntryPoint()) )->m_cbRetPop;
    
        Stub *pCanonicalStub;
        MLStubCache::MLStubCompilationMode mode;
        pCanonicalStub = g_pUMThunkStubCache->Canonicalize(
                                    (const BYTE *)(pMLStream->GetEntryPoint()),
                                    &mode);
        pMLStream->DecRef();
    
        switch (mode)
        {
            case MLStubCache::INTERPRETED:
                pFinalMLStub = pCanonicalStub;
                {
                    UMThunkMLStub* pHeader = (UMThunkMLStub*)(pFinalMLStub->GetEntryPoint());

                    UINT hash = pHeader->m_cbRetPop;

                    _ASSERTE(0 == (hash & 0x3)); // We reserve the lower two bits for flags

                    enum
                    {
                        kHashThisCallAdjustment   = 0x1,
                        kHashThisCallHiddenArg = 0x2
                    };

                    if (pHeader->m_wFlags & umtmlThisCall)
                    {
                        hash |= kHashThisCallAdjustment;
                        if (pHeader->m_wFlags & umtmlThisCallHiddenArg)
                            hash |= kHashThisCallHiddenArg;
                    }

                    Stub *pStub = g_pUMThunkInterpretedStubCache->GetStub(hash);
                    if (!pStub) {
                        StubHolder<Stub> pCandidate(NULL);

                        pCandidate = CreateGenericNExportStub(pHeader);
                        Stub *pWinner = g_pUMThunkInterpretedStubCache->AttemptToSetStub(hash, pCandidate);

                        if (!pWinner)
                            COMPlusThrowOM();

                        pStub = pWinner;
                    }
    
                    pFinalExecStub = pStub;
                }
                break;
    
    
            case MLStubCache::STANDALONE:
                pFinalMLStub = NULL;
                pFinalExecStub = pCanonicalStub;
                break;
    
            default:
                _ASSERTE(0);
        }
    
   
        if (FastInterlockCompareExchangePointer((void*volatile*) &m_pMLStub,
                                          pFinalMLStub,
                                          NULL) != NULL)
        {
    
            // Some thread swooped in and set us. Our stub is now a
            // duplicate, so throw it away.
            if (pFinalMLStub)
                pFinalMLStub->DecRef();
        }
    
    
        if (FastInterlockCompareExchangePointer((void*volatile*) &m_pExecStub,
                                          pFinalExecStub,
                                          NULL) != NULL)
        {
    
            // Some thread swooped in and set us. Our stub is now a
            // duplicate, so throw it away.
            if (pFinalExecStub)
                pFinalExecStub->DecRef();
        }
    
        // Must be the last thing we set!
        m_state        = kRunTimeInited;
    }

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
}


//--------------------------------------------------------------------------
// Onetime Init
//--------------------------------------------------------------------------
void UMThunkInit()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    MEMORY_REPORT_CONTEXT_SCOPE("UMThunk");

    g_pUMThunkStubCache = new UMThunkStubCache();
    g_pUMThunkInterpretedStubCache = new ArgBasedStubRetainer();

    return;
}


#ifdef _DEBUG
void __stdcall LogUMTransition(UMEntryThunk* thunk) 
{
    CONTRACTL
    {
        NOTHROW;
        DEBUG_ONLY;
        GC_NOTRIGGER;
        ENTRY_POINT;
        if (GetThread()) MODE_PREEMPTIVE; else MODE_ANY;
        DEBUG_ONLY;
        PRECONDITION(CheckPointer(thunk));
    }
    CONTRACTL_END;

    BEGIN_ENTRYPOINT_VOIDRET;
    
    void** retESP = ((void**) &thunk) + 4;

    MethodDesc* method = thunk->GetMethod();
    if (method)
    {
        LOG((LF_STUBS, LL_INFO1000000, "UNMANAGED -> MANAGED Stub To Method = %s::%s SIG %s Ret Address ESP = 0x%x ret = 0x%x\n", 
            method->m_pszDebugClassName,
            method->m_pszDebugMethodName,
            method->m_pszDebugMethodSignature, retESP, *retESP));
    }

    if(GetThread() != NULL)
        _ASSERTE(!GetThread()->PreemptiveGCDisabled());

    END_ENTRYPOINT_VOIDRET;

    }
#endif
