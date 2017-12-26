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
//*****************************************************************************
// File: thread.cpp
//
// Debugger thread routines
//
//*****************************************************************************

#include "stdafx.h"
#include "common.h"
#include "../../vm/threads.h"


/* ------------------------------------------------------------------------- *
 * DebuggerThread routines
 * ------------------------------------------------------------------------- */



//
// Struct used to pass data to the two stack walk callback functions.
//
struct _RefreshStackFramesData
{
    static StackWalkAction StackWalkCount(FrameInfo *pInfo, VOID* data)
    {
        _RefreshStackFramesData * pThis = (_RefreshStackFramesData*) data;
        return pThis->StackWalkCountEx(pInfo);
    }

    unsigned int GetTotalFrames() { return totalFrames; }
    unsigned int GetTotalChains() { return totalChains; }

    Thread*               thread;
    DebuggerRCThread*     rcThread;


    bool                  needChainRegisters;
    REGDISPLAY            chainRegisters;


public:
    void Init()
    {
        this->iWhich  = IPC_TARGET_OUTOFPROC;
        DebuggerIPCEvent *pEvent = g_pRCThread->GetIPCEventSendBuffer(iWhich);

        this->eventMaxSize = CorDBIPC_BUFFER_SIZE;
        this->currentSTRData = &(pEvent->StackTraceResultData.traceData);
        this->eventSize = (UINT_PTR)(this->currentSTRData) - (UINT_PTR)(pEvent);


        this->totalFrames = 0;
        this->totalChains = 0;
        this->m_f1stPassHasUMLeafChain = false;
        this->totalEntries = 0;

#ifdef _DEBUG
        this->m_dbgTotalTraceCount = 0;
        this->m_dbgTotalFramesSent = 0;
        this->m_dbgTotalChainsSent = 0;
        this->m_dbgHasValidEntry = false;
#endif
    };

    // Start a new entry
    DebuggerIPCE_STRData* BeginEntry()
    {
#ifdef _DEBUG
        _ASSERTE(!this->m_dbgHasValidEntry);
        this->m_dbgHasValidEntry = true;
#endif

        HRESULT hr = PreFlushIPCBufferIfNeeded();
        if (FAILED(hr))
        {
            return NULL;
        }

        return currentSTRData;
    }

    // Valid inbetween Begin & Finish entry.
    DebuggerIPCE_STRData* GetCurrentEntry()
    {
        _ASSERTE(this->m_dbgHasValidEntry);

        return currentSTRData;
    }


    // Queue, possibly send this entry to the RS.
    // Move to the next entry.
    HRESULT FinishEntry()
    {
#ifdef _DEBUG
        _ASSERTE(this->m_dbgHasValidEntry);
        this->m_dbgHasValidEntry = false;

        // Track counting.
        if (currentSTRData->eType == DebuggerIPCE_STRData::cChain)
        {
            this->m_dbgTotalChainsSent++;
            _ASSERTE(this->m_dbgTotalChainsSent <= totalChains);
        }
        else
        {
            this->m_dbgTotalFramesSent++;
            _ASSERTE(this->m_dbgTotalFramesSent <= totalFrames);
        }
#endif
        DebuggerIPCEvent *pEvent = rcThread->GetIPCEventSendBuffer(this->iWhich);

        // This is a sad hacky workaround. See m_f1stPassHasUMLeafChain for details.
        if (this->totalEntries == 0)
        {
            bool f2ndPassHasUMLeafChain =
                (currentSTRData->eType == DebuggerIPCE_STRData::cChain) &&
                (currentSTRData->u.chainReason == CHAIN_ENTER_UNMANAGED);

            if (f2ndPassHasUMLeafChain != m_f1stPassHasUMLeafChain)
            {
                STRESS_LOG0(LF_CORDB, LL_INFO1000, "RSFD::FE - 1st and 2nd pass differ.\n");

                if (m_f1stPassHasUMLeafChain && !f2ndPassHasUMLeafChain)
                {
                    // 2nd pass doesn't have a UM leaf chain, so decrement the counter
                    _ASSERTE(this->totalChains > 0);
                    this->totalChains--;
                } else {
                    // 2nd pass will send an extra UM leaf chain so inc the counter.
                    this->totalChains++;
                }

                // Since we haven't sent a real event out yet, we still have time
                // to adjust the count in the 1st event.
                pEvent->StackTraceResultData.totalChainCount = this->GetTotalChains();
            }
        }
        this->totalEntries++;



        // Move to the next one. BeginEntry() pre-flushes to guarantee that we'll
        // have enough room here.

        pEvent->StackTraceResultData.traceCount++;
        this->currentSTRData++;
        this->eventSize += sizeof(DebuggerIPCE_STRData);
        _ASSERTE(this->eventSize <= this->eventMaxSize);

        return S_OK;
    }

    // Send any remaining IPC entries.
    HRESULT FlushRemainingEntries()
    {
        // Shouldn't be flushing in the middle of an entry.
        _ASSERTE(!this->m_dbgHasValidEntry);

        HRESULT hr = SendEvent();

        _ASSERTE(this->m_dbgTotalFramesSent == totalFrames);
        _ASSERTE(this->m_dbgTotalChainsSent == totalChains);
        _ASSERTE(this->m_dbgTotalTraceCount == totalFrames + totalChains);
        _ASSERTE(this->totalEntries == this->m_dbgTotalTraceCount);
        return hr;
    }

private:

    StackWalkAction StackWalkCountEx(FrameInfo *pInfo)
    {
        LEAF_CONTRACT;


        // Record if we're sending a leaf UM chain.
        // (eg, the first thing we see is a UM chain)
        if (this->totalChains + this->totalFrames == 0)
        {
            if (pInfo->HasChainMarker() && (pInfo->chainReason == CHAIN_ENTER_UNMANAGED))
            {
                _ASSERTE(!this->m_f1stPassHasUMLeafChain);
                this->m_f1stPassHasUMLeafChain = true;
            }
        }

        if (pInfo->HasChainMarker())
        {
            this->totalChains++;
        }

        if (pInfo->HasMethodFrame() || pInfo->HasStubFrame())
        {
            this->totalFrames++;
        }

        return SWA_CONTINUE;
    }


    // Helper to flush the IPC buffer if we need more room for the next event.
    HRESULT PreFlushIPCBufferIfNeeded()
    {
        HRESULT hr = S_OK;
        Thread *t = this->thread;

        DebuggerIPCEvent *pEvent = rcThread->GetIPCEventSendBuffer(this->iWhich);
        //
        // If we've filled this event, send it off to the Right Side
        // before continuing the walk.
        //
        if ((this->eventSize + sizeof(DebuggerIPCE_STRData)) >= this->eventMaxSize)
        {
            //
            //
            pEvent->StackTraceResultData.threadUserState = g_pDebugger->GetFullUserState(t);


            hr = SendEvent();

            //
            // Reset for the next set of frames.
            //
            pEvent->StackTraceResultData.traceCount = 0;
            this->currentSTRData = &(pEvent->StackTraceResultData.traceData);
            this->eventSize = (UINT_PTR)(this->currentSTRData) - (UINT_PTR)(pEvent);
        }
        return hr;
    }

    // Helper to send the event, and do consistency checking.
    HRESULT SendEvent()
    {
        _ASSERTE(this->iWhich == IPC_TARGET_OUTOFPROC);

#ifdef _DEBUG
        DebuggerIPCEvent *pEvent = rcThread->GetIPCEventSendBuffer(this->iWhich);

        // Check invariants w/ TraceCount
        this->m_dbgTotalTraceCount += pEvent->StackTraceResultData.traceCount;

        // Ensure that the entry ptr is where we think it ought to be.
        BYTE* pExpectedEntry = (BYTE*) &(pEvent->StackTraceResultData.traceData) +
            sizeof(DebuggerIPCE_STRData) * pEvent->StackTraceResultData.traceCount;
        _ASSERTE(pExpectedEntry == (BYTE*) currentSTRData);
#endif

        // Now actually send the event.
        HRESULT hr = this->rcThread->SendIPCEvent(this->iWhich);
        return hr;
    }


    unsigned int          totalFrames;
    unsigned int          totalChains;

    bool                  m_f1stPassHasUMLeafChain;


    IpcTarget             iWhich;

    // Track where in the event buffer we're at.
    DebuggerIPCE_STRData* currentSTRData; // current entry we're writing.
    unsigned int          eventSize;
    unsigned int          eventMaxSize;

    unsigned int          totalEntries;

#ifdef _DEBUG
    // Keep track of the total entries set. IT had better be totalFrames + totalChains
    unsigned int          m_dbgTotalTraceCount;
    unsigned int          m_dbgTotalFramesSent;
    unsigned int          m_dbgTotalChainsSent;

    // Track BeginEntry & FinishEntry calls.
    bool                  m_dbgHasValidEntry;
#endif

};


BYTE *GetValueOfRegisterJit(ICorDebugInfo::RegNum reg, FrameInfo * pInfo)
{
    LEAF_CONTRACT;

    REGDISPLAY* rd;
    rd = &pInfo->registers;

    BYTE *ret = NULL;
#ifdef _X86_
    switch(reg)
    {
        case ICorDebugInfo::REGNUM_EAX:
        {
            ret = *(BYTE**)rd->pEax;
            break;
        }
        case ICorDebugInfo::REGNUM_ECX:
        {
            ret = *(BYTE**)rd->pEcx;
            break;
        }
        case ICorDebugInfo::REGNUM_EDX:
        {
            ret = *(BYTE**)rd->pEdx;
            break;
        }
        case ICorDebugInfo::REGNUM_EBX:
        {
            ret = *(BYTE**)rd->pEbx;
            break;
        }
        case ICorDebugInfo::REGNUM_ESP:
        {
            ret = *(BYTE**)(&(rd->Esp));
            break;
        }
        case ICorDebugInfo::REGNUM_EBP:
        {
            ret = *(BYTE**)rd->pEbp;
            break;
        }
        case ICorDebugInfo::REGNUM_ESI:
        {
            ret = *(BYTE**)rd->pEsi;
            break;
        }
        case ICorDebugInfo::REGNUM_EDI:
        {
            ret = *(BYTE**)rd->pEdi;
            break;
        }
        case ICorDebugInfo::REGNUM_AMBIENT_SP:
        {
            ret = (BYTE*) pInfo->ambientSP;
            break;
        }
        default:
        {
            _ASSERTE(!"Unrecognized reg type");
            ret = NULL;
            break;
        }
    }
#elif defined(_PPC_)
    switch(reg)
    {
    case ICorDebugInfo::REGNUM_SP:
        {
            ret = (BYTE*)rd->SP;
            break;
        }
    case ICorDebugInfo::REGNUM_FP:
        {
            ret = *(BYTE**)rd->pR[30-13];
            break;
        }
    // Cases for any other registers (REGNUM_R3 .. R10) default
    // REGDISPLAY structure has no info on those
    default:
        {
            ret = NULL;
            break;
        }
    }
#else
    PORTABILITY_ASSERT("GetValueOfRegisterJit (Thread.cpp) is not implemented on this platform.");
#endif

    return ret;
}

BYTE *GetPtrFromValue(ICorJitInfo::NativeVarInfo *pJITInfo,
                      FrameInfo * pInfo)
{
    BYTE *pAddr = NULL;
    BYTE *pRegAddr = NULL;

    // Currently this method is only used to retrieve the vararg handle.  If you want to use this method
    // to retrieve other internal params such as the generics secret param, make sure you test it
    // first before relaxing/removing the following assertion.
    _ASSERTE(pJITInfo->varNumber == (DWORD)ICorDebugInfo::VARARGS_HND_ILNUM);

    switch (pJITInfo->loc.vlType)
    {
        case ICorJitInfo::VLT_REG:
                _ASSERTE(!"GPFV: The variable arguments are not pushed onto the stack yet!");
            break;

        case ICorJitInfo::VLT_STK:
                pRegAddr = GetValueOfRegisterJit(pJITInfo->loc.vlStk.vlsBaseReg, pInfo);
                pAddr = pRegAddr + pJITInfo->loc.vlStk.vlsOffset;
            break;

        case ICorJitInfo::VLT_REG_REG:
        case ICorJitInfo::VLT_REG_STK:
        case ICorJitInfo::VLT_STK_REG:
        case ICorJitInfo::VLT_STK2:
        case ICorJitInfo::VLT_FPSTK:
            _ASSERTE(!"GPFV: Can't convert multi-part values into a ptr!");
            break;

        case ICorJitInfo::VLT_FIXED_VA:
            _ASSERTE(!"GPFV: VLT_FIXED_VA is an invalid value!");
            break;

        case ICorJitInfo::VLT_REG_BYREF:
        case ICorJitInfo::VLT_STK_BYREF:
            _ASSERTE(!"GPFV: we should not be getting VLT_{REG|STK}_BYREF for internal params");
            break;

        default:
            _ASSERTE(!"GPFV: invalid VarLocType for internal params");
            break;
    }

    LOG((LF_CORDB,LL_INFO100000, "GPFV: Derived ptr 0x%x from type "
        "0x%x\n", pAddr, pJITInfo->loc.vlType));
    return pAddr;
}

// Get Variable-Arg info.
void GetVAInfo(bool *pfVarArgs,
               void **ppSig,
               SIZE_T *pcbSig,
               void **ppFirstArg,
               MethodDesc *pMD,
               FrameInfo * pInfo,
               SIZE_T relOffset)
{
    REGDISPLAY* rd = &pInfo->registers;
    PCCOR_SIGNATURE pSig;
    DWORD cbSigSize;

    ULONG callConv = 0;

    pMD->GetSig(&pSig, &cbSigSize);

    if (pSig != NULL)
    {
        SigParser sigParser(pSig, cbSigSize);
        HRESULT hr = sigParser.GetCallingConv(&callConv);

        if (FAILED(hr))
        {
            // Is this ever bad....
            (*pfVarArgs) = true;
            (*ppSig) = NULL;
            (*pcbSig) = 0;
            (*ppFirstArg) = NULL;
            return;
        }
    }

    if ( (callConv & IMAGE_CEE_CS_CALLCONV_MASK)&
         IMAGE_CEE_CS_CALLCONV_VARARG)
    {
        LOG((LF_CORDB,LL_INFO100000, "GVAI: %s::%s is a varargs fnx!\n",
             pMD->m_pszDebugClassName,pMD->m_pszDebugMethodName));

#if defined(_X86_) || defined(_PPC_) || defined(_WIN64)
        HRESULT hr = S_OK;
        ICorJitInfo::NativeVarInfo *pNativeInfo;

        // This is a VARARGs function, so pass over the instance-specific info
        DebuggerJitInfo *dji=g_pDebugger->GetJitInfo(pMD, (BYTE*)GetControlPC(rd));
        if (dji != NULL)
        {
            hr = FindNativeInfoInILVariableArray((unsigned)ICorDebugInfo::VARARGS_HND_ILNUM,
                                                 relOffset,
                                                 &pNativeInfo,
                                                 dji->GetVarNativeInfoCount(),
                                                 dji->GetVarNativeInfo());
        }

        // For optimized code, we may not get the desired information for the VASigCookie.  
        // For example, we may get a NativeVarInfo for the VASigCookie when the cookie is still 
        // in the register and not yet pushed onto the stack.  Other variables will be in the 
        // other argument registers or on the stack according to the calling convention.  
        // In that case, we can't easily enumerate arguments, so we just return a NULL VASigCookie.
        if ( (dji == NULL) || FAILED(hr) || (pNativeInfo == NULL) || 
             (pNativeInfo->loc.vlType == ICorJitInfo::VLT_REG))
        {
#ifdef _DEBUG
            if (dji == NULL)
            {
                LOG((LF_CORDB, LL_INFO1000, "GVAI: varargs? no DJI\n"));
            }
            else if (CORDBG_E_IL_VAR_NOT_AVAILABLE == hr)
            {
                LOG((LF_CORDB, LL_INFO1000, "GVAI: varargs? No VARARGS_HND_ILNUM found!\n"));
            }
            else if (FAILED(hr))
            {
                LOG((LF_CORDB, LL_INFO1000, "GVAI: varargs? Failed with hr:0x%x\n", hr));
            }
            else if (pNativeInfo == NULL)
            {
                LOG((LF_CORDB, LL_INFO1000, "GVAI: varargs? No native info\n"));
            }
            else
            {
                _ASSERTE( (pNativeInfo != NULL) && (pNativeInfo->loc.vlType == ICorJitInfo::VLT_REG) );
                LOG((LF_CORDB, LL_INFO1000, "GVAI: varargs? VASigCookie is not yet pushed onto the stack\n"));
            }
#endif //_DEBUG

            // Is this ever bad....
            (*pfVarArgs) = true;
            (*ppSig) = NULL;
            (*pcbSig) = 0;
            (*ppFirstArg) = NULL;
            return;
        }

        BYTE *pvStart = GetPtrFromValue(pNativeInfo, pInfo);
        _ASSERTE(pvStart != NULL);
        VASigCookie *vasc = *(VASigCookie**)pvStart;

        (*pfVarArgs) = true;
        (*ppSig) = (void *)vasc->mdVASig;
        (*pcbSig) = SigParser::LengthOfSig(vasc->mdVASig);

#if !(defined(STACK_GROWS_DOWN_ON_ARGS_WALK)^defined(STACK_GROWS_UP_ON_ARGS_WALK))
#error One and only one between STACK_GROWS_DOWN_ON_ARGS_WALK and STACK_GROWS_UP_ON_ARGS_WALK must be defined!
#endif

        // Note: the first arg is relative to the start of the VASigCookie on the stack
#if defined(STACK_GROWS_DOWN_ON_ARGS_WALK)
        (*ppFirstArg) = (void *)(pvStart - sizeof(void *) + vasc->sizeOfArgs);
#else
        (*ppFirstArg) = (void *)(pvStart + sizeof(VASigCookie*));
#endif // STACK_GROWS_DOWN_ON_ARGS_WALK

        LOG((LF_CORDB,LL_INFO10000, "GVAI: Base Ptr for args is 0x%x\n", (*ppFirstArg)));

#else
        PORTABILITY_ASSERT("GetVAInfo is not implemented on this platform.");
#endif // _X86_ || _PPC_ || _WIN64
    }
    else
    {
        LOG((LF_CORDB,LL_INFO100000, "GVAI: %s::%s NOT VarArg!\n",
             pMD->m_pszDebugClassName,pMD->m_pszDebugMethodName));

        (*pfVarArgs) = false;

        (*ppFirstArg) = (void *)0;
        (*ppSig) = (void *)0;
        (*pcbSig) = (SIZE_T)0;

    }
}

// Helper to copy Hot-Cold info from a code-region to a JITFuncData
void InitJITFuncDataFromCodeRegion(DebuggerIPCE_JITFuncData * pData, CodeRegionInfo * pInfo)
{
    _ASSERTE(pData != NULL);
    _ASSERTE(pInfo != NULL);

    pData->nativeSize            =  pInfo->getSizeOfTotalCode();
    pData->nativeStartAddressPtr =  pInfo->getAddrOfHotCode();

    pData->nativeStartAddressColdPtr = pInfo->getAddrOfColdCode();
    pData->nativeColdSize            = pInfo->getSizeOfColdCode();
}


//-----------------------------------------------------------------------------
// This function attempts to load the value of the hidden generic context argmuent
// by searching the variable lifetime information for the TYPECTXT_ILNUM variable.
//-----------------------------------------------------------------------------
LPVOID GetExactGenericArgsFromVarInfo(DebuggerJitInfo* dji, FrameInfo* pInfo)
{
    _ASSERTE(dji       != NULL);
    _ASSERTE(pInfo->md != NULL);

    DWORD argIndex = 0;
    if (pInfo->md->AcquiresInstMethodTableFromThis())
    {
        argIndex = 0;
    }
    else
    {
        _ASSERTE(pInfo->md->RequiresInstMethodTableArg() || pInfo->md->RequiresInstMethodDescArg());
        argIndex = (DWORD) ICorDebugInfo::TYPECTXT_ILNUM;
    }

    CONTEXT* pCtx = NULL;
#if defined(_X86_)
    CONTEXT context;
    INDEBUG(memset(&context, 0xCC, sizeof(context)));
    context.Eax = (pInfo->registers.pEax != NULL ? *(SIZE_T*)(pInfo->registers.pEax) : NULL);
    context.Ecx = (pInfo->registers.pEcx != NULL ? *(SIZE_T*)(pInfo->registers.pEcx) : NULL);
    context.Edx = (pInfo->registers.pEdx != NULL ? *(SIZE_T*)(pInfo->registers.pEdx) : NULL);
    context.Ebx = (pInfo->registers.pEbx != NULL ? *(SIZE_T*)(pInfo->registers.pEbx) : NULL);
    context.Esp = pInfo->registers.Esp;
    context.Ebp = (pInfo->registers.pEbp != NULL ? *(SIZE_T*)(pInfo->registers.pEbp) : NULL);
    context.Esi = (pInfo->registers.pEsi != NULL ? *(SIZE_T*)(pInfo->registers.pEsi) : NULL);
    context.Edi = (pInfo->registers.pEdi != NULL ? *(SIZE_T*)(pInfo->registers.pEdi) : NULL);
    pCtx = &context;
#else
    PORTABILITY_ASSERT("NYI: foo() for this platform");
#endif

    LPVOID result = NULL;
    UINT varNativeInfoCount       = dji->GetVarNativeInfoCount();
    PTR_NativeVarInfo pNativeInfo = dji->GetVarNativeInfo();

    for (UINT i = 0; i < varNativeInfoCount; i++)
    {
        if ((pNativeInfo[i].startOffset <= pInfo->relOffset)            &&
            (pNativeInfo[i].endOffset   >  pInfo->relOffset)            &&
            (pNativeInfo[i].varNumber   == argIndex))
        {
            _ASSERTE( (pNativeInfo[i].loc.vlType == ICorDebugInfo::VLT_REG) ||
                      (pNativeInfo[i].loc.vlType == ICorDebugInfo::VLT_STK) );

            // This is to guard ourself against bad context. GetNativeVarVal may
            // dereference value on stack without checking for NULL
            //
            PAL_CPP_TRY
            {
                AVInRuntimeImplOkayHolder AVOkay(TRUE);

                const ICorDebugInfo::VarLoc& loc = pNativeInfo[i].loc;
                if( (loc.vlType == ICorDebugInfo::VLT_STK) &&
                    (loc.vlStk.vlsBaseReg  ==   ICorDebugInfo::REGNUM_AMBIENT_SP) )
                {
                    result = *(LPVOID*)( (BYTE*)(pInfo->ambientSP) + loc.vlStk.vlsOffset );
                }
                else
                {
                    GetNativeVarVal(loc,
                                    pCtx,
                                    (SIZE_T*)&result,
                                    NULL
                                    WIN64_ARG(sizeof(SIZE_T)));
                }

                if (argIndex == 0 && result != NULL)
                {
                    if (GCHeap::GetGCHeap()->IsHeapPointer(result) && ((Object*)result)->GetMethodTable()->SanityCheck())
                        result = (LPVOID)(ObjectToOBJECTREF((Object*)result)->GetMethodTable());
                }
            }
            PAL_CPP_CATCH_ALL
            {
                STRESS_LOG0(LF_CORDB,LL_INFO1000,"GetExactGenericArgsFromVarInfo AV due to bad context!\n");
            }
            PAL_CPP_ENDTRY

            break;
        }
    }
    return result;
}

//-----------------------------------------------------------------------------
// Helper to init IPC event for a method.
// This is called from TASSC.
// It marshals the information from the pInfo + pDModule objects into
// the rsfd's current entry.
//-----------------------------------------------------------------------------
void InitMethodFrameHelper(
    FrameInfo *pInfo,
    DebuggerModule* pDModule,
    _RefreshStackFramesData *rsfd
)
{
    _ASSERTE(pInfo->HasMethodFrame());
    _ASSERTE(pDModule != NULL);
    MethodDesc* fd = pInfo->md;

    Thread *t;
    t = rsfd->thread;
    DebuggerREGDISPLAY* drd;
    drd = &(rsfd->GetCurrentEntry()->rd);

    SIZE_T realNativeOffset = 0;

    DebuggerIPCE_FuncData* currentFuncData = &rsfd->GetCurrentEntry()->v.funcData;
    DebuggerIPCE_JITFuncData* currentJITFuncData = &rsfd->GetCurrentEntry()->v.jitFuncData;
    _ASSERTE(fd != NULL);

    GetVAInfo(&(rsfd->GetCurrentEntry()->v.varargs.fVarArgs),
              &(rsfd->GetCurrentEntry()->v.varargs.rpSig),
              &(rsfd->GetCurrentEntry()->v.varargs.cbSig),
              &(rsfd->GetCurrentEntry()->v.varargs.rpFirstArg),
              fd,
              pInfo,
              pInfo->relOffset);

    LOG((LF_CORDB, LL_INFO10000, "DT::TASSC: good frame for %s::%s\n",
         fd->m_pszDebugClassName,
         fd->m_pszDebugMethodName));

    //
    // Fill in information about the function that goes with this
    // frame.
    //
    {
        // This may call into the metadata which may call new.
        SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
        currentFuncData->funcRVA = g_pEEInterface->MethodDescGetRVA(fd);
    }
    _ASSERTE (t != NULL);

    currentFuncData->funcDebuggerModuleToken.Set(pDModule);

    currentFuncData->funcDebuggerAssemblyToken.Set(
        g_pEEInterface->MethodDescGetModule(fd)->GetClassLoader()->GetAssembly());
    currentFuncData->funcMetadataToken = fd->GetMemberDef();

    currentFuncData->classMetadataToken = fd->GetMethodTable()->GetCl();

    currentJITFuncData->isInstantiatedGeneric = fd->HasClassOrMethodInstantiation();

    rsfd->GetCurrentEntry()->v.ambientESP = (void*) pInfo->ambientSP;

    if (fd->IsSharedByGenericInstantiations())
    {
        rsfd->GetCurrentEntry()->v.exactGenericArgsToken = pInfo->exactGenericArgsToken;
    }
    else
    {
        rsfd->GetCurrentEntry()->v.exactGenericArgsToken = NULL;
    }

    // initialize some fields
    currentFuncData->localVarSigToken = mdSignatureNil;
    currentFuncData->ilStartAddress = NULL;
    currentFuncData->ilSize = 0;
    currentFuncData->currentEnCVersion = CorDB_DEFAULT_ENC_FUNCTION_VERSION;
    rsfd->GetCurrentEntry()->v.ILIP = NULL;

    currentJITFuncData->nativeStartAddressPtr = NULL;
    currentJITFuncData->nativeSize = 0;

    currentJITFuncData->nativeStartAddressColdPtr = NULL;
    currentJITFuncData->nativeColdSize = 0;


    currentJITFuncData->nativeCodeJITInfoToken.Set(NULL);
    currentJITFuncData->ilToNativeMapAddr = NULL;
    currentJITFuncData->ilToNativeMapSize = 0;
    currentJITFuncData->enCVersion = CorDB_DEFAULT_ENC_FUNCTION_VERSION;

    // Pass back the local var signature token.
    COR_ILMETHOD *CorILM = g_pEEInterface->MethodDescGetILHeader(fd);

    if (CorILM != NULL )
    {
        COR_ILMETHOD_DECODER ILHeader(CorILM);

        if (ILHeader.LocalVarSigTok != 0)
            currentFuncData->localVarSigToken = ILHeader.GetLocalVarSigTok();
        //
        //
        currentFuncData->ilStartAddress = const_cast<BYTE*>(ILHeader.Code);
        currentFuncData->ilSize = ILHeader.GetCodeSize();

        DebuggerJitInfo *jitInfo;

        jitInfo = pInfo->GetJitInfoFromFrame();
        currentJITFuncData->nativeCodeMethodDescToken.Set(fd);

        if (jitInfo == NULL)
        {
            // if have EnC then must have the code. We don't support code pitching with EnC

            rsfd->GetCurrentEntry()->v.ILIP = NULL;

            // If we're not tracking then we still return information about the
            // native code.
            //
            // Note: always send back the size of the method. This
            // allows us to get the code, even when we haven't
            // been tracking. (Handling of the GetCode message
            // knows how to find the start address of the code, or
            // how to respond if is been pitched.)

            CodeRegionInfo info = CodeRegionInfo::GetCodeRegionInfo(NULL, fd);
            InitJITFuncDataFromCodeRegion(currentJITFuncData, &info);
            _ASSERTE(g_pEEInterface->GetFunctionSize(fd) == currentJITFuncData->nativeSize);

            //
            // We've got to update the native offset, even in the case where
            // there's no jit info.
            //
            // Bug 128182

            realNativeOffset = (SIZE_T)pInfo->relOffset;
        }
        else
        {
            LOG((LF_CORDB,LL_INFO10000,"DeTh::TASSC: Code: 0x%x Got DJI "
                 "0x%x, from 0x%x to 0x%x\n",(const BYTE*)(DWORD_PTR)drd->PC,jitInfo,
                 jitInfo->m_addrOfCode, jitInfo->m_addrOfCode +
                 jitInfo->m_sizeOfCode));

            // Pass back the pointers to the sequence point map so
            // that the RIght Side can copy it out if needed.
            //_ASSERTE(jitInfo->m_sequenceMapSorted);

            // pInfo->exactGenericArgsToken is NULL when GetExactGenericArgsToken() returns NULL
            // while we are doing the stack walk.  We can make that method try harder, but since
            // it's a member of the CrawlFrame, it doesn't and shouldn't have any information
            // about variable lifetime.
#if defined(_X86_) || defined(_WIN64)
            if (fd->IsSharedByGenericInstantiations() && (pInfo->exactGenericArgsToken == NULL))
            {
                rsfd->GetCurrentEntry()->v.exactGenericArgsToken = GetExactGenericArgsFromVarInfo(jitInfo, pInfo);
            }
#endif

            currentFuncData->currentEnCVersion = jitInfo->m_methodInfo->GetCurrentEnCVersion();
            currentJITFuncData->ilToNativeMapAddr = jitInfo->GetSequenceMap();
            currentJITFuncData->ilToNativeMapSize = jitInfo->GetSequenceMapCount();
            currentJITFuncData->enCVersion = jitInfo->m_encVersion;

            if (!jitInfo->m_codePitched)
            {
                // It's there & life is groovy
                InitJITFuncDataFromCodeRegion(currentJITFuncData, &jitInfo->m_codeRegionInfo);

                currentJITFuncData->nativeCodeJITInfoToken.Set(jitInfo);
            }

            realNativeOffset = (SIZE_T)pInfo->relOffset;

            SIZE_T whichIrrelevant;
            rsfd->GetCurrentEntry()->v.ILIP = const_cast<BYTE*>(ILHeader.Code)
                + jitInfo->MapNativeOffsetToIL(realNativeOffset,
                                               &rsfd->GetCurrentEntry()->v.mapping,
                                               (DWORD *)&whichIrrelevant);

            LOG((LF_CORDB,LL_INFO10000,"Sending native Ver:0x%p JITok:0x%p in stack trace!\n",
                 currentJITFuncData->enCVersion,
                 currentJITFuncData->nativeCodeJITInfoToken.UnWrap()));
        }
    }

    currentJITFuncData->nativeOffset = realNativeOffset;

}

//
// Callback for walking a thread's stack. Sends required frame data to the
// DI as send buffers fill up.
//
StackWalkAction DebuggerThread::TraceAndSendStackCallback(FrameInfo *pInfo, VOID* data)
{
    _RefreshStackFramesData *rsfd = (_RefreshStackFramesData*) data;
    Thread *t;
    t = rsfd->thread;

    LOG((LF_CORDB, LL_EVERYTHING, "GetIPCEventSendBuffer called in TraceAndSendStackCallback\n"));


    // Record registers for the start of the next chain, if appropriate.
    if (rsfd->needChainRegisters)
    {
        rsfd->needChainRegisters = false;
        CopyREGDISPLAY(&(rsfd->chainRegisters), &(pInfo->registers));
    }

    // Only report frames which are chain boundaries, or are not marked internal, or are stubs.
    if (!pInfo->HasChainMarker() && !pInfo->HasMethodFrame() && !pInfo->HasStubFrame())
    {
        return SWA_CONTINUE;
    }



#ifdef LOGGING
    if( pInfo->quickUnwind == true )
        LOG((LF_CORDB, LL_INFO10000, "DT::TASSC: rsfd => Doing quick unwind\n"));
#endif

    MethodDesc* fd = pInfo->md;

    if (pInfo->HasMethodFrame() || pInfo->HasStubFrame())
    {
        DebuggerIPCE_STRData* pEntry = rsfd->BeginEntry();
        if (pEntry == NULL)
        {
            return SWA_ABORT;
        }

        //
        // Send a frame
        //


        // Do common initialization for both Methods & Stubs.
        pEntry->fp = pInfo->fp;
        pEntry->quicklyUnwound = pInfo->quickUnwind;

        // Pass the appdomain that this thread was in when it was executing this frame to the Right Side.
        pEntry->currentAppDomainToken.Set(pInfo->currentAppDomain);

        pEntry->eType = DebuggerIPCE_STRData::cMethodFrame;

        REGDISPLAY* rd = &pInfo->registers;
        DebuggerREGDISPLAY* drd = &(pEntry->rd);

        // On IA64, we really want the unadjusted relOffset for funclets here (i.e. relative to the
        // beginning of the funclet, not to the beginning of the parent method).
        SetDebuggerREGDISPLAYFromREGDISPLAY(drd, rd
                                            IA64_ARG(pInfo->fIsLeaf)
                                            IA64_ARG(pInfo->relOffset));

        // Calculate some shared info,
        Module *pRuntimeModule = NULL;
        DebuggerModule* pDModule = NULL;
        AppDomain *pAppDomain = pInfo->currentAppDomain;

        if (fd != NULL)
        {
            pRuntimeModule = g_pEEInterface->MethodDescGetModule(fd);
            // Here we assume any MethodDesc in the frame is relative to the same domain as the frame itself.
            // This is currently not true in at least one place: cross-AD COM->managed transition frames.
            // We should probably store a targetAppDomain (in addition to currentAppDomain) in FrameInfo.
            pDModule = g_pDebugger->LookupModule(pRuntimeModule, pAppDomain);
        }


        // Init the rest of the event depending if this is a Stub or a Frame.
        if (pInfo->HasStubFrame())
        {
            pEntry->eType = DebuggerIPCE_STRData::cStubFrame;
            pEntry->stubFrame.frameType = pInfo->eStubFrameType;

            // We may or may not have an associated function.
            pEntry->stubFrame.funcMetadataToken =
                (fd == NULL) ? (0) : fd->GetMemberDef();
            pEntry->stubFrame.funcDebuggerModuleToken.Set(pDModule);

            if( pInfo->eStubFrameType == STUBFRAME_FUNC_EVAL )
            {
                _ASSERTE( pInfo->frame->GetFrameType() == Frame::TYPE_FUNC_EVAL );
                _ASSERTE( fd == NULL );     // func eval frames don't have a MethodDesc hint
                
                FuncEvalFrame* fef = static_cast<FuncEvalFrame*>(pInfo->frame);
                DebuggerEval* pDE = fef->GetDebuggerEval();

                // Note that these could be zero/null for non-code func-evals (new string/array)
                pEntry->stubFrame.funcMetadataToken = pDE->m_methodToken;
                pEntry->stubFrame.funcDebuggerModuleToken.Set(pDE->m_debuggerModule);                
            }

        }
        else
        {
            // If this assert fires, it most likely means that the Loader has failed to notify
            // the debugger of a module via a LoadModule() callback.
            CONSISTENCY_CHECK_MSGF((pDModule != NULL), ("Unidentified Module on stack!\nRuntime module*=0x%p, name=%s\n"
                "Method=%s::%s 0x%p\n", 
                pRuntimeModule, pRuntimeModule->GetSimpleName(), fd->m_pszDebugClassName, fd->m_pszDebugMethodName,fd));
                
            InitMethodFrameHelper(pInfo, pDModule, rsfd);

        }


        if (FAILED(rsfd->FinishEntry()))
        {
            return SWA_ABORT;
        }
    }

    //
    // Send a chain boundary if we have one.
    //
    if (pInfo->HasChainMarker())
    {
        DebuggerIPCE_STRData* pEntry = rsfd->BeginEntry();
        if (pEntry == NULL)
        {
            return SWA_ABORT;
        }

        pEntry->eType = DebuggerIPCE_STRData::cChain;
        pEntry->u.chainReason = pInfo->chainReason;
        pEntry->u.managed = pInfo->managed;
        pEntry->u.context = pInfo->context;
        pEntry->fp = pInfo->fp;
        pEntry->quicklyUnwound = pInfo->quickUnwind;

        //_ASSERTE(rsfd->pEntry->fp != NULL);

        REGDISPLAY* rd = &rsfd->chainRegisters;
        DebuggerREGDISPLAY* drd = &(pEntry->rd);

        _ASSERTE(!rsfd->needChainRegisters); // should already have them.
        SetDebuggerREGDISPLAYFromREGDISPLAY(drd, rd);

        rsfd->needChainRegisters = true;

        //
        // Bump our pointers to the next space for the next frame.
        //
        if (FAILED(rsfd->FinishEntry()))
        {
            return SWA_ABORT;
        }
    }
    return SWA_CONTINUE;
}


// Filter to assert
LONG TASSFilter(LPEXCEPTION_POINTERS ep, PVOID pv)
{
    STRESS_LOG2(LF_CORDB,LL_INFO1000,"DT::TASSFilter exception .cxr=%p, code=%x\n", ep->ContextRecord, ep->ExceptionRecord->ExceptionCode);

#ifdef _DEBUG
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
    SString sStack;
    StackScratchBuffer buffer;
    GetStackTraceAtContext(sStack, ep->ContextRecord);
    const CHAR *string = NULL;

    EX_TRY
    {
        string = sStack.GetANSI(buffer);
    }
    EX_CATCH
    {
        string = "*Could not retrieve stack*";
    }
    EX_END_CATCH(RethrowTerminalExceptions);

    CONSISTENCY_CHECK_MSGF(false, ("Exception in TASS, code=0x%08x, stack=\n%s\n", ep->ExceptionRecord->ExceptionCode, string));
#endif
    return EXCEPTION_EXECUTE_HANDLER;
}


//
// TraceAndSendStack unwinds the thread's stack and sends all needed data
// back to the DI for processing and storage. Nothing is kept on the RC side.
//
// Note: this method must work while the RC is in restricted mode.
//
// Note: the right side is waiting for a response, so if an error occurs,
// you MUST send a reply back telling what happened.  Otherwise we'll deadlock.
// Note also that the HRESULT this function returns is for HandleIPCEvent's use,
// and is otherwise dropped on the floor.
//
HRESULT DebuggerThread::TraceAndSendStack(Thread *thread,
                                          DebuggerRCThread* rcThread,
                                          IpcTarget iWhich)
{
    struct _RefreshStackFramesData rsfd;

    _ASSERTE(iWhich == IPC_TARGET_OUTOFPROC);

    HRESULT hr = S_OK;
    BOOL fAlreadySent = FALSE;
    LOG((LF_CORDB, LL_EVERYTHING, "GetIPCEventSendBuffer called in TraceAndSendStack\n"));

    memset((void *)&rsfd, 0, sizeof(rsfd));

    // Initialize the event that we'll be sending back to the right side.
    // The same event is sent over and over, depending on how many frames
    // there are. The frameCount is simply reset each time the event is sent.
    DebuggerIPCEvent *pEvent = rcThread->GetIPCEventSendBuffer(iWhich);
    pEvent->type = DB_IPCE_STACK_TRACE_RESULT;
    pEvent->processId = GetCurrentProcessId();
    pEvent->threadId = Debugger::GetThreadIdHelper(thread);
    pEvent->hr = S_OK;
    pEvent->StackTraceResultData.traceCount = 0;

    Thread::ThreadState ts = thread->GetSnapshotState();

    // In shutdown case, threads are disappearing on us, so we can't possibly get a stack trace.
    if (//g_fProcessDetach ||
        (ts & Thread::TS_Dead) ||
        (ts & Thread::TS_Unstarted) ||
        (ts & Thread::TS_Detached))
    {
        pEvent->hr =  CORDBG_E_BAD_THREAD_STATE;

        if (iWhich == IPC_TARGET_OUTOFPROC)
        {
            return rcThread->SendIPCEvent(iWhich);
        }
        else
            return CORDBG_E_BAD_THREAD_STATE;
    }

    LOG((LF_CORDB,LL_INFO1000, "thread id:0x%x userThreadState:0x%x \n",
        Debugger::GetThreadIdHelper(thread), pEvent->StackTraceResultData.threadUserState));

    //EEIface will set this to NULL if we're not in an exception, and to the
    //address of the proper context (which isn't the current context) otherwise
    pEvent->StackTraceResultData.pContext.Set(GetManagedStoppedCtx(thread));

    //
    // Setup data to be passed to the stack trace callback.
    //
    rsfd.Init();
    rsfd.thread = thread;
    rsfd.rcThread = rcThread;
    rsfd.needChainRegisters = true;

    // Default the registers to zero.
    memset((void *)&rsfd.chainRegisters, 0, sizeof(rsfd.chainRegisters));

    LOG((LF_CORDB, LL_INFO10000, "DT::TASS: tracking stack...\n"));

    PAL_TRY
    {
        //
        // If the hardware context of this thread is set, then we've hit
        // a native breakpoint for this thread. We need to initialize
        // or walk with the context of the thread when it faulted, not with
        // its current context.
        //
        CONTEXT *pContext = g_pEEInterface->GetThreadFilterContext(thread);
        CONTEXT ctx;

        BOOL contextValid = (pContext != NULL);

        if (!contextValid)
            pContext = &ctx;

        StackWalkAction res = DebuggerWalkStack(thread, LEAF_MOST_FRAME,
                                                pContext, contextValid,
                                                _RefreshStackFramesData::StackWalkCount,
                                                (VOID*)(&rsfd),
                                                TRUE, iWhich);
        if (res == SWA_FAILED)
        {
            STRESS_LOG0(LF_CORDB,LL_INFO1000,"DT::TASS failed 1\n");

            pEvent->hr =  E_FAIL;
            if (iWhich == IPC_TARGET_OUTOFPROC)
            {
                HRESULT hrTemp;
                hrTemp = rcThread->SendIPCEvent(iWhich);
            }
        }
        else if (res == SWA_ABORT)
        {
            STRESS_LOG0(LF_CORDB,LL_INFO1000,"DT::TASS abort 1\n");
            hr = E_FAIL; // Note that we'll have already sent off the error message.
            fAlreadySent = TRUE;
            goto LExit;
        }

        pEvent->StackTraceResultData.totalFrameCount = rsfd.GetTotalFrames();
        pEvent->StackTraceResultData.totalChainCount = rsfd.GetTotalChains();
        pEvent->StackTraceResultData.threadUserState =
            g_pDebugger->GetFullUserState(thread);

        LOG((LF_CORDB, LL_INFO10000, "DT::TASS: found %d frames & %d chains.\n",
             rsfd.GetTotalFrames(), rsfd.GetTotalChains()));

        //
        // If there are any frames, walk again and send the detailed info about
        // each one.
        //
        if (rsfd.GetTotalFrames() > 0 || rsfd.GetTotalChains() > 0)
        {
            res = DebuggerWalkStack(thread, LEAF_MOST_FRAME,
                                    pContext, contextValid,
                                    DebuggerThread::TraceAndSendStackCallback,
                                    (VOID*)(&rsfd), TRUE, iWhich);
            if (res == SWA_FAILED)
            {
                STRESS_LOG0(LF_CORDB,LL_INFO1000,"DT::TASS failed 2\n");

                pEvent->hr =  E_FAIL;
                if (iWhich == IPC_TARGET_OUTOFPROC)
                {
                    HRESULT hrTemp;
                    hrTemp = rcThread->SendIPCEvent(iWhich);
                }
            }
            else if (res == SWA_ABORT)
            {
                STRESS_LOG0(LF_CORDB,LL_INFO1000,"DT::TASS abort 2\n");

                hr = E_FAIL; // Note that we'll have already sent off the error message.
                fAlreadySent = TRUE;
                goto LExit;
            }
        }

LExit: ;
    }
    PAL_EXCEPT_FILTER(TASSFilter, NULL)
    {
        _ASSERTE(!"Exception");
        hr = pEvent->hr = CORDBG_E_BAD_THREAD_STATE;
    }
    PAL_ENDTRY

    if ((iWhich == IPC_TARGET_OUTOFPROC) && !fAlreadySent)
    {
        hr = rsfd.FlushRemainingEntries();
    }

    return hr;
}


