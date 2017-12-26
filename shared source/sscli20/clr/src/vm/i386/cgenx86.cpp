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
// CGENX86.CPP -
//
// Various helper routines for generating x86 assembly code.
//
//

// Precompiled Header
#include "common.h"

#include "field.h"
#include "stublink.h"
#include "cgensys.h"
#include "frames.h"
#include "excep.h"
#include "dllimport.h"
#include "comdelegate.h"
#include "log.h"
#include "security.h"
#include "comdelegate.h"
#include "array.h"
#include "jitinterface.h"
#include "codeman.h"
#include "remoting.h"
#include "dbginterface.h"
#include "eeprofinterfaces.h"
#include "eeconfig.h"
#include "comobject.h"
#include "asmconstants.h"
#include "class.h"
#include "virtualcallstub.h"
#include "mdaassistantsptr.h"


#include "stublink.inl"

extern "C" DWORD __stdcall GetSpecificCpuTypeAsm(void);
extern "C" DWORD __stdcall GetSpecificCpuFeaturesAsm(DWORD *pInfo);

extern "C" size_t g_dwPreStubAddr;
size_t g_dwPreStubAddr;

// NOTE on Frame Size C_ASSERT usage in this file 
// if the frame size changes then the stubs have to be revisited for correctness
// kindly revist the logic and then update the constants so that the C_ASSERT will again fire
// if someone changes the frame size.  You are expected to keep this hard coded constant
// up to date so that changes in the frame size trigger errors at compile time if the code is not altered

void generate_noref_copy (unsigned nbytes, StubLinkerCPU* sl);

#ifndef DACCESS_COMPILE

//---------------------------------------------------------------
// Returns the type of CPU (the value of x of x86)
// (Please note, that it returns 6 for P5-II)
//---------------------------------------------------------------
void __stdcall GetSpecificCpuInfo(CORINFO_CPU * cpuInfo)
{
    LEAF_CONTRACT

    static CORINFO_CPU val = { 0, 0, 0 };

    if (val.dwCPUType)
    {
        *cpuInfo = val;
        return;
    }

    CORINFO_CPU tempVal;
    tempVal.dwCPUType = GetSpecificCpuTypeAsm();  // written in ASM & doesn't participate in contracts
    _ASSERTE(tempVal.dwCPUType);
    
#ifdef _DEBUG

    /* Set Family+Model+Stepping string (eg., x690 for Banias, or xF30 for P4 Prescott)
     * instead of Family only
     */
     
    const DWORD cpuDefault = 0xFFFFFFFF;
    static ConfigDWORD cpuFamily;
    DWORD configCpuFamily = cpuFamily.val(L"CPUFamily", cpuDefault);
    if (configCpuFamily != cpuDefault)
    {
        assert((configCpuFamily & 0xFFF) == configCpuFamily);
        tempVal.dwCPUType = (tempVal.dwCPUType & 0xFFFF0000) | configCpuFamily;
    }

#endif

    tempVal.dwFeatures = GetSpecificCpuFeaturesAsm(&tempVal.dwExtendedFeatures);  // written in ASM & doesn't participate in contracts

#ifdef _DEBUG

    /* Set the 32-bit feature mask
     */
    
    const DWORD cpuFeaturesDefault = 0xFFFFFFFF;
    static ConfigDWORD cpuFeatures;
    DWORD configCpuFeatures = cpuFeatures.val(L"CPUFeatures", cpuFeaturesDefault);
    if (configCpuFeatures != cpuFeaturesDefault)
    {
        tempVal.dwFeatures = configCpuFeatures;
    }

#endif

    val = *cpuInfo = tempVal;
}


#endif // #ifndef DACCESS_COMPILE

/*
    This method is dependent on the StubProlog, therefore it's implementation
    is done right next to it.
*/
void FramedMethodFrame::UpdateRegDisplay(const PREGDISPLAY pRD)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

    CalleeSavedRegisters* regs = GetCalleeSavedRegisters();
    MethodDesc * pFunc = GetFunction();


    // reset pContext; it's only valid for active (top-most) frame

    pRD->pContext = NULL;

    pRD->pEdi = (DWORD*) &regs->edi;
    pRD->pEsi = (DWORD*) &regs->esi;
    pRD->pEbx = (DWORD*) &regs->ebx;
    pRD->pEbp = (DWORD*) &regs->ebp;
    pRD->PCTAddr = GetReturnAddressPtr();
    pRD->pPC  = PTR_SLOT(pRD->PCTAddr);
    pRD->Esp  = (DWORD)(pRD->PCTAddr + sizeof(TADDR));



    if (pFunc)
    {
        pRD->Esp += (DWORD) pFunc->CbStackPop();
    }

    RETURN;
}

void DelegateTransitionFrame::UpdateRegDisplay(const PREGDISPLAY pRD)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

    CalleeSavedRegisters* regs = GetCalleeSavedRegisters();
    MethodDesc * pFunc = GetInvokeFunctionUnsafe();


    // reset pContext; it's only valid for active (top-most) frame

    pRD->pContext = NULL;

    pRD->pEdi = (DWORD*) &regs->edi;
    pRD->pEsi = (DWORD*) &regs->esi;
    pRD->pEbx = (DWORD*) &regs->ebx;
    pRD->pEbp = (DWORD*) &regs->ebp;
    pRD->PCTAddr = GetReturnAddressPtr();
    pRD->pPC  = PTR_SLOT(pRD->PCTAddr);
    pRD->Esp  = (DWORD)(pRD->PCTAddr + sizeof(TADDR));

    if (pFunc)
    {
        pRD->Esp += MetaSig(pFunc).CbStackPop(TRUE);
    }

    RETURN;
}


void SecurityFrame::UpdateRegDisplay(const PREGDISPLAY pRD)
{
    WRAPPER_CONTRACT;
    FramedMethodFrame::UpdateRegDisplay(pRD);
}


void HelperMethodFrame::UpdateRegDisplay(const PREGDISPLAY pRD)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(m_MachState->isValid());               // InsureInit has been called
    }
    CONTRACT_END;

    ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

    // reset pContext; it's only valid for active (top-most) frame
    pRD->pContext = NULL;

#ifdef DACCESS_COMPILE

    //
    // In the dac case we may have gotten here
    // without the frame being initialized, so
    // try and initialize on the fly.
    //

    if (!m_MachState->isValid())
    {
        MachState unwindState;

        pRD->PCTAddr = InsureInit(false, &unwindState);
        pRD->pPC = PTR_SLOT(pRD->PCTAddr);
        pRD->Esp = unwindState._esp;

        // Get some special host instance memory
        // so we have a place to point to.
        // This host memory has no target address
        // and so won't be looked up or used for
        // anything else.
        MachState* thisState = (MachState*)
            DacAllocHostOnlyInstance(sizeof(*thisState), true);

        thisState->_edi = unwindState._edi;
        pRD->pEdi = (DWORD *)&thisState->_edi;
        thisState->_esi = unwindState._esi;
        pRD->pEsi = (DWORD *)&thisState->_esi;
        thisState->_ebx = unwindState._ebx;
        pRD->pEbx = (DWORD *)&thisState->_ebx;
        thisState->_ebp = unwindState._ebp;
        pRD->pEbp = (DWORD *)&thisState->_ebp;

        // InsureInit always sets m_RegArgs to zero
        // in the real code.  I'm not sure exactly
        // what should happen in the on-the-fly case,
        // but go with what would happen from an InsureInit.
        RETURN;
    }

#endif // #ifdef DACCESS_COMPILE
    
    // DACCESS: The MachState pointers are kept as PTR_TADDR so
    // the host pointers here refer to the appropriate size and
    // these casts are not a problem.
    pRD->pEdi = (DWORD*) m_MachState->pEdi();
    pRD->pEsi = (DWORD*) m_MachState->pEsi();
    pRD->pEbx = (DWORD*) m_MachState->pEbx();
    pRD->pEbp = (DWORD*) m_MachState->pEbp();
    pRD->pPC  = (SLOT *)m_MachState->pRetAddr();
    pRD->PCTAddr = PTR_HOST_TO_TADDR(m_MachState->pRetAddr());
    pRD->Esp  = (DWORD) m_MachState->esp();

    if (m_RegArgs == 0)
        RETURN;

    // If we are promoting arguments, then we should do what the signature
    // tells us to do, instead of what the epilog tells us to do.
    // This is because the helper (and thus the epilog) may be shared and
    // can not pop off the correct number of arguments
    
    CONSISTENCY_CHECK(GetFunction() != NULL || IsRefDataFrame());
    
    UINT cbStackPop = 0;
    if (GetFunction() != NULL)
    {
        cbStackPop = GetFunction()->CbStackPop();
    }

    else if (IsRefDataFrame())
    {
        cbStackPop = NativeDelayFixupObjRefData(m_wRefData).CbStackPop();
    }

    pRD->Esp  = (DWORD)pRD->PCTAddr + sizeof(TADDR) + cbStackPop;

    RETURN;
}

#ifdef _DEBUG_IMPL
// Confirm that if the machine state was not initialized, then
// any unspilled callee saved registers did not change
EXTERN_C MachState* __stdcall HelperMethodFrameConfirmState(HelperMethodFrame* frame, void* esiVal, void* ediVal, void* ebxVal, void* ebpVal) {

    CONTRACT(MachState*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        DEBUG_ONLY;
    }
    CONTRACT_END;

    MachState* state = NULL; 
    // probe to avoid a kazillion violations in the code that follows.
    BEGIN_DEBUG_ONLY_CODE;
    
    state = frame->MachineState();
    if (!state->isValid())
    {
        frame->InsureInit(false, NULL);
        _ASSERTE(state->_pEsi != &state->_esi || state->_esi  == (TADDR)esiVal);
        _ASSERTE(state->_pEdi != &state->_edi || state->_edi  == (TADDR)ediVal);
        _ASSERTE(state->_pEbx != &state->_ebx || state->_ebx  == (TADDR)ebxVal);
        _ASSERTE(state->_pEbp != &state->_ebp || state->_ebp  == (TADDR)ebpVal);
    }

    END_DEBUG_ONLY_CODE;
    RETURN(state);
}
#endif

void FaultingExceptionFrame::UpdateRegDisplay(const PREGDISPLAY pRD)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    CalleeSavedRegisters* regs = GetCalleeSavedRegisters();

    // reset pContext; it's only valid for active (top-most) frame
    pRD->pContext = NULL;

    pRD->pEdi = (DWORD*) &regs->edi;
    pRD->pEsi = (DWORD*) &regs->esi;
    pRD->pEbx = (DWORD*) &regs->ebx;
    pRD->pEbp = (DWORD*) &regs->ebp;
    pRD->PCTAddr = GetReturnAddressPtr();
    pRD->pPC  = PTR_SLOT(pRD->PCTAddr);
    pRD->Esp = m_Esp;
    RETURN;
}

void InlinedCallFrame::UpdateRegDisplay(const PREGDISPLAY pRD)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        // We should skip over InlinedCallFrame if it is not active.
        // It will be part of a JITed method's frame, and the stack-walker
        // can handle such a case.
        PRECONDITION(CORProfilerStackSnapshotEnabled() || InlinedCallFrame::FrameHasActiveCall(this));
        MODE_ANY;
    }
    CONTRACT_END;

    if (!InlinedCallFrame::FrameHasActiveCall(this))
    {
        LOG((LF_CORDB, LL_ERROR, "WARNING: InlinedCallFrame::UpdateRegDisplay called on inactive frame %p\n", this));
        return;
    }
    
    DWORD * savedRegs = (DWORD*) &m_pCalleeSavedRegisters;
    DWORD stackArgSize = (DWORD) PTR_TO_TADDR(m_Datum);   

    if (stackArgSize & ~0xFFFF)
    {
        NDirectMethodDesc * pMD = PTR_NDirectMethodDesc(m_Datum);

        /* if this is not an NDirect frame, something is really wrong */

        _ASSERTE(pMD->SanityCheck() && pMD->IsNDirect());

        stackArgSize = pMD->GetStackArgumentSize();
    }

    // reset pContext; it's only valid for active (top-most) frame
    pRD->pContext = NULL;


    pRD->pEdi = savedRegs++;
    pRD->pEsi = savedRegs++;
    pRD->pEbx = savedRegs++;
    pRD->pEbp = savedRegs++;

    /* The return address is just above the "ESP" */
    pRD->PCTAddr = PTR_HOST_MEMBER_TADDR(InlinedCallFrame, this,
                                         m_pCallerReturnAddress);
    pRD->pPC = PTR_SLOT(pRD->PCTAddr);                                         

    /* Now we need to pop off the outgoing arguments */
    pRD->Esp  = (DWORD)(size_t) m_pCallSiteSP + stackArgSize;
    RETURN;
}


void UnmanagedToManagedFrame::UpdateRegDisplay(const PREGDISPLAY pRD)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    DWORD *savedRegs = (DWORD *)(PTR_HOST_TO_TADDR(this) -
                                 (sizeof(CalleeSavedRegisters)));

    // reset pContext; it's only valid for active (top-most) frame

    pRD->pContext = NULL;

    BYTE offsRetAddr = GetOffsetOfReturnAddress();
    
    pRD->pEdi = savedRegs++;
    pRD->pEsi = savedRegs++;
    pRD->pEbx = savedRegs++;
    pRD->pEbp = savedRegs++;
    pRD->pPC  = (SLOT*)((BYTE*)this + offsRetAddr);
    pRD->PCTAddr  = PTR_HOST_TO_TADDR(this) + offsRetAddr;
    pRD->Esp  = (DWORD)(pRD->PCTAddr + sizeof(void*));

    pRD->Esp += (DWORD) GetNumCallerStackBytes();

    RETURN;
}

void TailCallFrame::UpdateRegDisplay(const PREGDISPLAY pRD)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACT_END;

    // reset pContext; it's only valid for active (top-most) frame
    pRD->pContext = NULL;

    pRD->pEdi = (DWORD*)&m_regs.edi;
    pRD->pEsi = (DWORD*)&m_regs.esi;
    pRD->pEbx = (DWORD*)&m_regs.ebx;
    pRD->pEbp = (DWORD*)&m_regs.ebp;

    pRD->PCTAddr = GetReturnAddressPtr();
    pRD->pPC  = PTR_SLOT(pRD->PCTAddr);
    pRD->Esp  = (DWORD)(pRD->PCTAddr + sizeof(TADDR));

    RETURN;
}

#ifndef DACCESS_COMPILE

#ifdef _DEBUG
//-------------------------------------------------------------------------
// This is a function which checks the debugger stub tracing capability
// when calling out to unmanaged code.
//
// IF YOU SEE STRANGE ERRORS CAUSED BY THIS CODE, it probably means that
// you have changed some exit stub logic, and not updated the corresponding
// debugger helper routines.  The debugger helper routines need to be able
// to determine
//
//      (a) the unmanaged address called by the stub
//      (b) the return address which the unmanaged code will return to
//      (c) the size of the stack pushed by the stub
//
// This information is necessary to allow the COM+ debugger to hand off
// control properly to an unmanaged debugger & manage the boundary between
// managed & unmanaged code.
//
// These are in XXXFrame::GetUnmanagedCallSite. (Usually
// with some help from the stub linker for generated stubs.)
//-------------------------------------------------------------------------

extern "C" void * __stdcall PerformExitFrameChecks();

void * __stdcall PerformExitFrameChecks()
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;
    STATIC_CONTRACT_DEBUG_ONLY;

    Thread *thread = GetThread();
    Frame *frame = thread->GetFrame();

    TADDR ip, returnIP, returnSP;

    BEGIN_DEBUG_ONLY_CODE;

    frame->GetUnmanagedCallSite(&ip, &returnIP, &returnSP);

    _ASSERTE(*PTR_TADDR(returnSP) == returnIP);

    END_DEBUG_ONLY_CODE;
    
    return (void*)ip;
}

#endif


//
BOOL Runtime_Test_For_SSE2()
{
    return FALSE;   // No support for __asm on Rotor builds, so return false
}

//===========================================================================
// create prestub

Stub * GeneratePrestub()
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;

        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    CPUSTUBLINKER sl;
    CPUSTUBLINKER *psl = &sl;

    // EAX contains MethodDesc* from the precode. Push it here
    // to initialize m_Datum field of the frame.
    psl->X86EmitPushReg(kEAX);

    psl->EmitMethodStubProlog(PrestubMethodFrame::GetMethodFrameVPtr());

    psl->X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());

    // push the new frame as an argument and call PreStubWorker.
    psl->X86EmitPushReg(kESI);
    psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) PreStubWorker), 4);

    // eax now contains replacement stub. PreStubWorker will never return
    // NULL (it throws an exception if stub creation fails.)

    // Debugger patch location
    psl->EmitPatchLabel();

    // mov [ebx + Thread.GetFrame()], edi   ;; restore previous frame
    psl->X86EmitIndexRegStore(kEBX, Thread::GetOffsetOfCurrentFrame(), kEDI);

    // mov ecx,[esi + Frame.GetOffsetOfDatum()] ;; ECX = MethodDesc*
    psl->X86EmitIndexRegLoad(kECX, kESI, PrestubMethodFrame::GetOffsetOfDatum());
    // mov [esi + Frame.GetOffsetOfDatum],eax   ;; overwrite datum with new target
    psl->X86EmitIndexRegStore(kESI, PrestubMethodFrame::GetOffsetOfDatum(), kEAX);
    // mov eax, ecx                             ;; EAX = MethodDesc*
    psl->Emit8(0x8b);
    psl->Emit8(0300 | kEAX << 3 | kECX);

    // !!! From here on, mustn't trash eax

    psl->EmitCheckGSCookie(kESI, PrestubMethodFrame::GetOffsetOfGSCookie());
    
    // Pop off the GSCookie and all of the frame until the argument registers
    psl->X86EmitAddEsp(PrestubMethodFrame::GetNegSpaceSize() + 
                       PrestubMethodFrame::GetOffsetOfArgumentRegisters());
    
    // Pop ArgumentRegisters structure, while restoring the actual
    // machine registers.
    #define DEFINE_ARGUMENT_REGISTER_BACKWARD(regname) psl->X86EmitPopReg(k##regname);
    #include "eecallconv.h"

    // !!! From here on, mustn't trash eax, ecx or edx.

    // Pop the VC5Frame
    psl->X86EmitPopEBPframe(true);

    //--------------------------------------------------------------------------
    // Pop CalleeSavedRegisters structure, while restoring the actual machine registers.
    //--------------------------------------------------------------------------
    psl->X86EmitPopReg(kEDI);
    psl->X86EmitPopReg(kESI);
    psl->X86EmitPopReg(kEBX);
    psl->X86EmitPopReg(kEBP);

    //--------------------------------------------------------------------------
    //!!! From here on, can't trash ANY register other than esp & eip.
    //--------------------------------------------------------------------------

    // Pop off the Frame structure *except* for the "datum" field
    // which has been overwritten with the new address to jump to.
    psl->X86EmitAddEsp(PrestubMethodFrame::GetOffsetOfDatum());

    // Now, jump to the new address.
    psl->X86EmitReturn(0);

    Stub* pStub = psl->Link();

    g_dwPreStubAddr = (size_t)pStub->GetEntryPoint();

    RETURN pStub;
}

//-------------------------------------------------------------------------
// One-time creation of special prestub to initialize UMEntryThunks.
//-------------------------------------------------------------------------
Stub *GenerateUMThunkPrestub()
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;

        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    CPUSTUBLINKER sl;
    CPUSTUBLINKER *psl = &sl;

    CodeLabel* rgRareLabels[] = { psl->NewCodeLabel(),
                                  psl->NewCodeLabel(),
                                  psl->NewCodeLabel()
                                };


    CodeLabel* rgRejoinLabels[] = { psl->NewCodeLabel(),
                                    psl->NewCodeLabel(),
                                    psl->NewCodeLabel()
                                };


    CodeLabel *pWrapLabel = psl->NewCodeLabel();

    //    push eax   // push UMEntryThunk
    psl->X86EmitPushReg(kEAX);

    //    push ecx      (in case this is a _thiscall: need to protect this register)
    psl->X86EmitPushReg(kECX);

    // Wrap puts a fake return address and a duplicate copy
    // of pUMEntryThunk on the stack, then falls thru to the regular
    // stub prolog. The stub prolog is fooled into thinkin this duplicate
    // copy is the real return address and UMEntryThunk.
    //
    //    call wrap. 
    psl->X86EmitCall(pWrapLabel, 0);


    //    pop  ecx
    psl->X86EmitPopReg(kECX);

    // Now we've executed the prestub and fixed up UMEntryThunk. The
    // duplicate data has been popped off.
    //
    //    pop eax   // pop UMEntryThunk
    psl->X86EmitPopReg(kEAX);

    //    lea eax, [eax + UMEntryThunk.m_code]  // point to fixedup UMEntryThunk
    psl->X86EmitOp(0x8d, kEAX, kEAX, 
                   UMEntryThunk::GetCodeOffset() + UMEntryThunkCode::GetEntryPointOffset());

    //    jmp eax //reexecute!
    psl->X86EmitR2ROp(0xff, (X86Reg)4, kEAX);

    psl->EmitLabel(pWrapLabel);

    // Wrap:
    //
    //   push [esp+8]  //repush UMEntryThunk
    psl->X86EmitEspOffset(0xff, (X86Reg)6, 8);

    // emit the initial prolog
    psl->EmitComMethodStubProlog(UMThkCallFrame::GetUMThkCallFrameVPtr(), rgRareLabels, rgRejoinLabels,
                               (LPVOID) UMThunkPrestubHandler, FALSE /*Don't profile*/);

    // mov ecx, [esi+UMThkCallFrame.pUMEntryThunk]
    psl->X86EmitIndexRegLoad(kECX, kESI, UMThkCallFrame::GetOffsetOfUMEntryThunk());

    // The call conv is a __stdcall   
    psl->X86EmitPushReg(kECX);

    // call UMEntryThunk::RuntimeInit
    psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID)UMEntryThunk::DoRunTimeInit), 4);

    psl->EmitComMethodStubEpilog(UMThkCallFrame::GetUMThkCallFrameVPtr(), 0, rgRareLabels, rgRejoinLabels,
                               (LPVOID) UMThunkPrestubHandler, FALSE /*Don't profile*/);

    RETURN psl->Link();
}

/*static*/
void NDirect::CreateGenericNDirectStubSys(CPUSTUBLINKER *psl, UINT numStackBytes)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(sizeof(CleanupWorkList) == sizeof(LPVOID));
    }
    CONTRACT_END;

    // EAX contains MethodDesc* from the precode. Push it here
    // to initialize m_Datum field of the frame.
    psl->X86EmitPushReg(kEAX);

    psl->EmitMethodStubProlog(NDirectMethodFrameGeneric::GetMethodFrameVPtr());

    // See NOTE on Frame Size C_ASSERT usage in this file 
    C_ASSERT(sizeof(NDirectMethodFrame::NegInfo)  - sizeof(FramedMethodFrame::NegInfo) == 4);

    // push 00000000    // pushes a marker for LeaveRuntime
    psl->X86EmitPushImm32(FLAG_ENTER_RUNTIME_NOT_REQUIRED);

    // See NOTE on Frame Size C_ASSERT usage in this file 
    C_ASSERT(sizeof(NDirectMethodFrameEx::NegInfo)  - sizeof(NDirectMethodFrame::NegInfo) == 12);

    // push room for the checkpoint object

    psl->X86EmitPushImm32(0);
    psl->X86EmitPushImm32(0);

    // push 00000000    ;; pushes a CleanupWorkList.
    psl->X86EmitPushImm32(0);

    // push g_GSCookie
    psl->X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());

    // Frame is ready to be inspected by debugger for patch location
    psl->EmitPatchLabel();

    psl->X86EmitPushReg(kESI);       // push esi (push new frame as ARG)
    psl->X86EmitPushReg(kEBX);       // push ebx (push current thread as ARG)
    psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) NDirectGenericStubWorker), 8); // in CE pop 8 bytes or args on return from call

    psl->EmitCheckGSCookie(kESI, NDirectMethodFrameGeneric::GetOffsetOfGSCookie());
    
    // pops CleanupWorkList and marker for LeaveRuntime
    psl->X86EmitAddEsp(NDirectMethodFrameGeneric::GetNegSpaceSize() - FramedMethodFrame::GetRawNegSpaceSize());

    psl->EmitMethodStubEpilog(numStackBytes, kNoTripStubStyle);

    RETURN;
}


// Creates the runtime "DllImport" stub for a delegate -> unmanaged call
Stub* COMDelegate::CreateGenericDllImportStubForDelegate(StubLinker *pstublinker, UINT numStackBytes, DelegateEEClass* pClass)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(CheckPointer(pstublinker));
        PRECONDITION(CheckPointer(pClass));
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    MethodDesc* pMD = pClass->m_pInvokeMethod;

    // Add caching per CreateGenericNDirectStub method

    CPUSTUBLINKER *psl = (CPUSTUBLINKER*)pstublinker;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Shuffle the arguments to remove the this pointer.
    //
    g_IBCLogger.LogMethodDescAccess(pMD);

    size_t nEntries = 3 + pMD->SizeOfVirtualFixedArgStack() / STACK_ELEM_SIZE;

#ifndef _DEBUG
    // This allocsize prediction is easy to break, so in retail, add
    // some fudge to be safe.
    nEntries += 3;
#endif

    size_t allocsize = sizeof(ShuffleEntry) * nEntries; 

    ShuffleEntry *pShuffleEntryArray = (ShuffleEntry*)_alloca(allocsize);

#ifdef _DEBUG
    FillMemory(pShuffleEntryArray, allocsize, 0xcc);
#endif

    GenerateShuffleArray(pMD, NULL, pShuffleEntryArray, nEntries);

    psl->EmitShuffleThunk(pShuffleEntryArray, TRUE);

    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Create the frame
    //

    // See NOTE on Frame Size C_ASSERT usage in this file 
    C_ASSERT(sizeof(DelegateTransitionFrame::NegInfo)  - sizeof(FramedMethodFrame::NegInfo) == 12);

    // Push the 'Delegate.Aux' field which holds the unmanaged callsite...this will live in m_Datum.
    psl->X86EmitIndexPush(SCRATCH_REGISTER_X86REG, DelegateObject::GetOffsetOfMethodPtrAux());

    // Emit the frame's VTable pointer
    psl->EmitMethodStubProlog(DelegateTransitionFrame::GetMethodFrameVPtr());

    // push 00000000    // pushes a marker for LeaveRuntime
    psl->X86EmitPushImm32(FLAG_ENTER_RUNTIME_NOT_REQUIRED);

    // push 00000000    ;; pushes a CleanupWorkList.
    psl->X86EmitPushImm32(0);

    // push pClass      // pushes the DelegateEEClass
    psl->X86EmitPushImmPtr(pClass);

    psl->X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());
    
    // Frame is ready to be inspected by debugger for patch location
    psl->EmitPatchLabel();

    // Push the args for the worker.
    psl->X86EmitPushReg(kESI);       // push esi (push new frame as ARG)
    psl->X86EmitPushReg(kEBX);       // push ebx (push current thread as ARG)    
    // Call the worker.
    psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) DllImportForDelegateGenericStubWorker), 8); // in CE pop 8 bytes or args on return from call

    psl->EmitCheckGSCookie(kESI, DelegateTransitionFrame::GetOffsetOfGSCookie());
    
    // pops CleanupWorkList, marker for LeaveRuntime, and the DelegateEEClass.
    psl->X86EmitAddEsp(DelegateTransitionFrame::GetNegSpaceSize() - FramedMethodFrame::GetRawNegSpaceSize());

    psl->EmitMethodStubEpilog(numStackBytes, kNoTripStubStyle);

    Stub* pCandidate = psl->Link(pClass->GetStubHeap());
    RETURN pCandidate;
}


/*static*/ Stub* CreateGenericNExportStub(UMThunkMLStub* pHeader)
{
    CPUSTUBLINKER sl;
    CPUSTUBLINKER *psl = &sl;
    
    CodeLabel* rgRareLabels[] = { psl->NewCodeLabel(),
                                    psl->NewCodeLabel(),
                                    psl->NewCodeLabel()
                                };


    CodeLabel* rgRejoinLabels[] = { psl->NewCodeLabel(),
                                    psl->NewCodeLabel(),
                                    psl->NewCodeLabel()
                                };

    if (pHeader->m_wFlags & umtmlThisCall)
    {
        if (pHeader->m_wFlags & umtmlThisCallHiddenArg)
        {
            // pop off the return address
            psl->X86EmitPopReg(kEDX);

            // exchange ecx (this "this") with the hidden structure return buffer
            //  xchg ecx, [esp]
            psl->X86EmitOp(0x87, kECX, (X86Reg)4 /*ESP*/);

            // push ecx
            psl->X86EmitPushReg(kECX);

            // push edx
            psl->X86EmitPushReg(kEDX);
        }
        else
        {
            // pop off the return address
            psl->X86EmitPopReg(kEDX);

            // jam ecx (the "this" param onto stack. Now it looks like a normal stdcall.)
            psl->X86EmitPushReg(kECX);

            // repush
            psl->X86EmitPushReg(kEDX);
        }
    }

    // push UMEntryThunk
    psl->X86EmitPushReg(kEAX);

    // emit the initial prolog
    psl->EmitComMethodStubProlog(UMThkCallFrame::GetUMThkCallFrameVPtr(), rgRareLabels, rgRejoinLabels,
                                    (LPVOID) UMThunkPrestubHandler, TRUE /*Profile*/);

    psl->X86EmitPushReg(kESI);       // push frame as an ARG
    psl->X86EmitPushReg(kEBX);       // push ebx (push current thread as ARG)

    psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) DoUMThunkCall), 8); // on CE pop 8 bytes or args on return

    psl->EmitComMethodStubEpilog(UMThkCallFrame::GetUMThkCallFrameVPtr(), pHeader->m_cbRetPop, rgRareLabels,
                                    rgRejoinLabels, (LPVOID) UMThunkPrestubHandler, TRUE /*Profile*/);

    return psl->Link();
}


// Here's the support for the interlocked operations.  The external view of 
// them is declared in util.hpp.

// These are implemented in assembly code, jithelp.asm/jithelp.s
extern "C" {
    void __fastcall OrMaskUP(DWORD volatile *Target, const int Bits);
    void __fastcall OrMaskMP(DWORD volatile *Target, const int Bits);
    void __fastcall AndMaskUP(DWORD volatile *Target, const int Bits);
    void __fastcall AndMaskMP(DWORD volatile *Target, const int Bits);
    LONG __fastcall ExchangeUP(LONG volatile *Target, LONG Value);
    LONG __fastcall ExchangeMP(LONG volatile *Target, LONG Value);
    INT64 __fastcall ExchangeLongMP8b(INT64 volatile *Target, INT64 Value);    
    PVOID __fastcall CompareExchangeUP(PVOID volatile *Target, PVOID Exchange, PVOID Comperand);
    PVOID __fastcall CompareExchangeMP(PVOID volatile *Target, PVOID Exchange, PVOID Comperand);
    INT64 __fastcall CompareExchangeLongMP8b(INT64 volatile *Target, INT64 Exchange, INT64 Comperand);
    LONG __fastcall ExchangeAddUP(LONG volatile *Target, LONG Value);
    LONG __fastcall ExchangeAddMP(LONG volatile *Target, LONG Value);
    INT64 __fastcall ExchangeAddLongMP8b(INT64 volatile *Target, INT64 Value);
    LONG __fastcall IncrementUP(LONG volatile *Target);
    LONG __fastcall IncrementMP(LONG volatile *Target);
    LONG __fastcall DecrementUP(LONG volatile *Target);
    LONG __fastcall DecrementMP(LONG volatile *Target);
    UINT64 __fastcall IncrementLongMP8b(UINT64 volatile *Target);
    UINT64 __fastcall DecrementLongMP8b(UINT64 volatile *Target);
}

BitFieldOps FastInterlockOr = OrMaskUP;
BitFieldOps FastInterlockAnd = AndMaskUP;

XchgOps         FastInterlockExchange = ExchangeUP;
XchgLongOps         FastInterlockExchangeLong = (XchgLongOps)ExchangeLongMP8b;
CmpXchgOps      FastInterlockCompareExchange = (CmpXchgOps)CompareExchangeUP;
CmpXchgLongOps      FastInterlockCompareExchangeLong = (CmpXchgLongOps)CompareExchangeLongMP8b;
XchngAddOps     FastInterlockExchangeAdd = ExchangeAddUP;
XchgAddLongOps     FastInterlockExchangeAddLong = (XchgAddLongOps)ExchangeAddLongMP8b;

IncDecOps       FastInterlockIncrement = IncrementUP;
IncDecOps       FastInterlockDecrement = DecrementUP;
IncDecLongOps   FastInterlockIncrementLong = IncrementLongMP8b;
IncDecLongOps   FastInterlockDecrementLong = DecrementLongMP8b;

// Adjust the generic interlocked operations for any platform specific ones we
// might have.
void InitFastInterlockOps()
{
    /* disabled because allocations in DllMain are not allowed
    CONTRACT_VOID
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(g_SystemInfo.dwNumberOfProcessors != 0);
    }
    CONTRACT_END;
    */

    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;

    if (g_SystemInfo.dwNumberOfProcessors != 1)
    {
        FastInterlockOr  = OrMaskMP;
        FastInterlockAnd = AndMaskMP;

        FastInterlockExchange = ExchangeMP;
        FastInterlockCompareExchange = (CmpXchgOps)CompareExchangeMP;
        FastInterlockExchangeAdd = ExchangeAddMP;
        FastInterlockIncrement = IncrementMP;
        FastInterlockDecrement = DecrementMP;

    }
    //RETURN;
}




//---------------------------------------------------------
// Handles Cleanup for a standalone stub that returns a gcref
//---------------------------------------------------------
LPVOID STDMETHODCALLTYPE DoCleanupWithGcProtection(NDirectMethodFrameEx* pFrame, Object *objectUNSAFE)
{

    CONTRACT(LPVOID)
    {
        THROWS;
        ENTRY_POINT;
        DISABLED(GC_TRIGGERS);  // see below
        MODE_COOPERATIVE;
    }
    CONTRACT_END;

    LPVOID pvret = NULL;

    OBJECTREF oref = ObjectToOBJECTREF(objectUNSAFE);

    INSTALL_UNWIND_AND_CONTINUE_HANDLER; // called by managed code by way of a stub

    GCPROTECT_BEGIN(oref);
    TRIGGERSGC();

    pFrame->GetCleanupWorkList()->Cleanup(FALSE);
    GetThread()->m_MarshalAlloc.Collapse(&pFrame->GetNegInfo()->m_checkpoint);
    *(OBJECTREF*)&pvret = oref;

    GCPROTECT_END();

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;


    RETURN pvret;
}


//---------------------------------------------------------
// Wrapper arround an instance call to ML_CSTR_C2N_SR::DoConversion(STRINGREF, CleanupWorkList*)
//---------------------------------------------------------
  
LPSTR STDMETHODCALLTYPE DoConversionWrapper_CSTR(ML_CSTR_C2N_SR * obj, StringObject * s, UINT32 fBestFitMapping, UINT32 fThrowOnUnmappableChar, CleanupWorkList* l)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        ENTRY_POINT;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    LPSTR str = NULL;
    // DoConversion has probe
    //BEGIN_ENTRYPOINT_THROWS;

    str = obj->DoConversion( ObjectToSTRINGREF(s), fBestFitMapping, fThrowOnUnmappableChar, l );
    //END_ENTRYPOINT_THROWS;
    return str;
}


//---------------------------------------------------------
// Wrapper around an instance call to VOID CleanupWorkList::Cleanup(BOOL fBecauseOfException)
//---------------------------------------------------------
static void STDMETHODCALLTYPE CleanupWrapper(NDirectMethodFrameEx* pFrame)
{
    WRAPPER_CONTRACT;
    
    CleanupWorkList *pList = pFrame->GetCleanupWorkList();

    if (pList->IsTrivial())
    {
        GetThread()->m_MarshalAlloc.Collapse(&pFrame->GetNegInfo()->m_checkpoint);
    }
    else
    {
        INSTALL_UNWIND_AND_CONTINUE_HANDLER; // called by managed code by way of a stub

        pList->Cleanup(FALSE);
        GetThread()->m_MarshalAlloc.Collapse(&pFrame->GetNegInfo()->m_checkpoint);

        UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
    }
}

static void __stdcall LeaveRuntimeHelperWithFrame (Thread *pThread, size_t target, Frame *pFrame)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        ENTRY_POINT;
    }
    CONTRACTL_END;
    
    Thread::LeaveRuntimeThrowComplus(target);
    GCX_COOP_THREAD_EXISTS(pThread); 
    pFrame->Push(pThread);

}

static void __stdcall EnterRuntimeHelperWithFrame (Thread *pThread, Frame *pFrame)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_PREEMPTIVE;
        ENTRY_POINT;
    }
    CONTRACTL_END;
    
    HRESULT hr = Thread::EnterRuntimeNoThrow();
    GCX_COOP_THREAD_EXISTS(pThread);
    if (FAILED(hr))
    {
        INSTALL_UNWIND_AND_CONTINUE_HANDLER;
        ThrowHR (hr);
        UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
    }

    pFrame->Pop(pThread);

}

// "ip" is the return address
// This function disassembles the code at the return address to determine
// how many arguments to pop off.
// Returns the number of DWORDs that should be popped off on return.

static int __stdcall GetStackSizeForVarArgCall(BYTE* ip)
{

    LEAF_CONTRACT;

    int retValue = 0;
    //BEGIN_ENTRYPOINT_VOIDRET;

    if (ip[0] == 0x81 && ip[1] == 0xc4)
    {
        // add esp, imm32
        retValue = (*(int*)&ip[2])/4;
    }
    else if (ip[0] == 0x83 && ip[1] == 0xc4)
    {
        // add esp, imm8
        retValue = ip[2]/4;
    }
    else if (ip[0] == 0x59)
    {
        // pop ecx
        retValue = 1;
    }
    else
    {
        retValue = 0;
    }
    //END_ENTRYPOINT_VOIDRET;
    return retValue;
}

//-----------------------------------------------------------------------------
// Hosting stub for calls from CLR code to unmanaged code
//
// We push a LeaveRuntimeFrame, and then re-push all the arguments.
// Note that we have to support all the different native calling conventions
// viz. stdcall, thiscall, cdecl, varargs


//-----------------------------------------------------------------------------
// This the layout of the frame of the stub

struct StubForHostStackFrame
{
    LPVOID                  m_outgingArgs[1];
    ArgumentRegisters       m_argumentRegisters;
    GSCookie                m_gsCookie;
    LeaveRuntimeFrame       m_LeaveRuntimeFrame;
    CalleeSavedRegisters    m_calleeSavedRegisters;
    LPVOID                  m_retAddr;
    LPVOID                  m_incomingArgs[1];

public:

    // Where does the FP/EBP point to?
    static INT32 GetFPpositionOffset()
    {
        LEAF_CONTRACT;
        return offsetof(StubForHostStackFrame, m_calleeSavedRegisters) + 
               offsetof(CalleeSavedRegisters, ebp); 
    }

    static INT32 GetFPrelOffsOfArgumentRegisters() 
    { 
        LEAF_CONTRACT;
        return offsetof(StubForHostStackFrame, m_argumentRegisters) - GetFPpositionOffset(); 
    }

    static INT32 GetFPrelOffsOfCalleeSavedRegisters() 
    { 
        LEAF_CONTRACT;
        return offsetof(StubForHostStackFrame, m_calleeSavedRegisters) - GetFPpositionOffset(); 
    }

    static INT32 GetFPrelOffsOfRetAddr() 
    { 
        LEAF_CONTRACT;
        return offsetof(StubForHostStackFrame, m_retAddr) - GetFPpositionOffset(); 
    }

    static INT32 GetFPrelOffsOfIncomingArgs() 
    { 
        LEAF_CONTRACT;
        return offsetof(StubForHostStackFrame, m_incomingArgs) - GetFPpositionOffset(); 
    }
};

//-----------------------------------------------------------------------------

Stub* NDirectMethodDesc::GenerateStubForHost(LPVOID pNativeTarget)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    // We need to call LeaveRuntime before the target, and EnterRuntime after the target
    CPUSTUBLINKER sl;

    sl.X86EmitPushEBPframe();
    
    // save EBX, ESI, EDI
    sl.X86EmitPushReg(kEBX);
    sl.X86EmitPushReg(kESI);
    sl.X86EmitPushReg(kEDI);

    // Frame
    sl.X86EmitPushReg(kDummyPushReg); // m_Next
    sl.X86EmitPushImm32((UINT)(size_t)LeaveRuntimeFrame::GetLeaveRuntimeFrameVPtr());

    // mov esi, esp;  esi is Frame
    sl.X86EmitMovRegSP(kESI);

    sl.X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());

    // Save outgoing arguments on the stack
    sl.X86EmitPushReg(kECX);
    sl.X86EmitPushReg(kEDX);

    if (IsVarArgs()) {
        // Re-push the return address as an argument to GetStackSizeForVarArgCall()
        // This will return the number of stack arguments (in DWORDs)
        sl.X86EmitIndexPush(kEBP, StubForHostStackFrame::GetFPrelOffsOfRetAddr());
        sl.X86EmitCall(sl.NewExternalCodeLabel((LPVOID)GetStackSizeForVarArgCall), 4);

        // We generate the following code sequence to re-push all the arguments
        //
        // Note that we cannot use "sub ESP, EAX" as ESP might jump past the
        // stack guard-page.
        //
        //      cmp EAX, 0
        // LoopTop:
        //      jz LoopDone
        //      push dword ptr[EBP + EAX*4 + 4]
        //      sub EAX, 1
        //      jmp LoopTop
        // LoopDone:
        //      ...

        sl.X86EmitCmpRegImm32(kEAX, 0);
        CodeLabel * pLoopTop = sl.EmitNewCodeLabel();
        CodeLabel * pLoopDone = sl.NewCodeLabel();
        sl.X86EmitCondJump(pLoopDone, X86CondCode::kJZ);
        sl.X86EmitBaseIndexPush(kEBP, kEAX, 4, StubForHostStackFrame::GetFPrelOffsOfIncomingArgs() - sizeof(LPVOID));
        sl.X86EmitSubReg(kEAX, 1);
        sl.X86EmitNearJump(pLoopTop);
        sl.EmitLabel(pLoopDone);
    }
    else {
        INT32 offs = StubForHostStackFrame::GetFPrelOffsOfIncomingArgs() + GetStackArgumentSize();

        int numStackSlots = GetStackArgumentSize() / sizeof(LPVOID);
        for (int i = 0; i < numStackSlots; i++) {
            offs -= sizeof(LPVOID);
            sl.X86EmitIndexPush(kEBP, offs);
        }
    }

    //-------------------------------------------------------------------------
    
    // EBX has Thread*
    // X86TLSFetch_TRASHABLE_REGS will get trashed
    sl.X86EmitCurrentThreadFetch(0);

    // push Frame
    sl.X86EmitPushReg(kESI);
    // push target
    sl.X86EmitPushImm32((UINT)(size_t)pNativeTarget);
    // push Thread
    sl.X86EmitPushReg(kEBX);
    sl.X86EmitCall(sl.NewExternalCodeLabel((LPVOID)LeaveRuntimeHelperWithFrame), 0xc);

    //-------------------------------------------------------------------------
    // call NDirect
    // See diagram above to see what the stack looks like at this point

    // Restore outgoing arguments
    unsigned offsToArgRegs = StubForHostStackFrame::GetFPrelOffsOfArgumentRegisters();
    sl.X86EmitIndexRegLoad(kECX, kEBP, offsToArgRegs + offsetof(ArgumentRegisters, ECX));
    sl.X86EmitIndexRegLoad(kEDX, kEBP, offsToArgRegs + offsetof(ArgumentRegisters, EDX));
    
    sl.X86EmitCall(sl.NewExternalCodeLabel((LPVOID)pNativeTarget),
                   (IsStdCall() || IsThisCall()) ? GetStackArgumentSize()/4 : 0);

    //-------------------------------------------------------------------------
    // Save return value registers and call EnterRuntimeHelperWithFrame
    //
    
    sl.X86EmitPushReg(kEAX);
    sl.X86EmitPushReg(kEDX);

    // push Frame
    sl.X86EmitPushReg(kESI);
    // push Thread
    sl.X86EmitPushReg(kEBX);
    // call EnterRuntime
    sl.X86EmitCall(sl.NewExternalCodeLabel((LPVOID)EnterRuntimeHelperWithFrame), 8);

    sl.X86EmitPopReg(kEDX);
    sl.X86EmitPopReg(kEAX);

    //-------------------------------------------------------------------------
    // Tear down the frame
    //
    
    sl.EmitCheckGSCookie(kESI, LeaveRuntimeFrame::GetOffsetOfGSCookie());
    
    // lea esp, [ebp - offsToCalleeSavedRegs]
    unsigned offsToCalleeSavedRegs = StubForHostStackFrame::GetFPrelOffsOfCalleeSavedRegisters();
    sl.X86EmitIndexLea((X86Reg)kESP_Unsafe, kEBP, offsToCalleeSavedRegs);

    sl.X86EmitPopReg(kEDI);
    sl.X86EmitPopReg(kESI);
    sl.X86EmitPopReg(kEBX);

    sl.X86EmitPopEBPframe(false);

    unsigned argsToPop = (IsStdCall() || IsThisCall()) ? GetStackArgumentSize() : 0;
    // ret [argsToPop]
    sl.X86EmitReturn(argsToPop);

    Module* pModule = GetModule();
    PREFIX_ASSUME(pModule != NULL);
    Stub *pCandidate = sl.Link(pModule->GetDomain()->GetStubHeap());

    return pCandidate;
}

//---------------------------------------------------------
// Handles system specfic portion of fully optimized NDirect stub creation
//
// Results:
//     TRUE     - was able to create a standalone asm stub (generated into
//                psl)
//     FALSE    - decided not to create a standalone asm stub due to
//                to the method's complexity. Stublinker remains empty!
//
//     COM+ exception - error - don't trust state of stublinker.
//-----------------------------------------------------------------------------

/*static*/ BOOL NDirect::CreateStandaloneNDirectStubSys(const MLHeader *pheader, CPUSTUBLINKER *psl, BOOL fDoComInterop)
{
    CONTRACT(BOOL)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pheader));
        PRECONDITION(CheckPointer(psl));
    }
    CONTRACT_END;


    // Must first scan the ML stream to see if this method qualifies for
    // a standalone stub. Can't wait until we start generating because we're
    // supposed to leave psl empty if we return FALSE.
    if (0 != (pheader->m_Flags & ~(MLHF_SETLASTERROR|MLHF_THISCALL|MLHF_64BITMANAGEDRETVAL|
                                   MLHF_64BITUNMANAGEDRETVAL|MLHF_MANAGEDRETVAL_TYPECAT_MASK|
                                   MLHF_UNMANAGEDRETVAL_TYPECAT_MASK|MLHF_NATIVERESULT|
                                   MLHF_LOP_FRIENDLY_FRAMES_CALLER_POPS_ARGS)))
    {
        RETURN FALSE;
    }

    BOOL fNeedsCleanup = FALSE;
    const MLCode *pMLCode = pheader->GetMLCode();
    MLCode mlcode;
    while (ML_INTERRUPT != (mlcode = *(pMLCode++)))
    {
        switch (mlcode)
        {
            case ML_COPY4: //intentional fallthru
            case ML_COPY8: //intentional fallthru
            case ML_PINNEDUNISTR_C2N: //intentional fallthru
            case ML_BLITTABLELAYOUTCLASS_C2N:
            case ML_CBOOL_C2N:
            case ML_COPYPINNEDGCREF:
                break;
            case ML_BUMPSRC:
            case ML_PINNEDISOMORPHICARRAY_C2N_EXPRESS:
                pMLCode += 2;
                break;
            case ML_REFBLITTABLEVALUECLASS_C2N:
                pMLCode += 4;
                break;

            case ML_CSTR_C2N:
                pMLCode += 2; //go past bestfitmapping & throwonunmappable char vars.
                break;


            case ML_PUSHRETVALBUFFER1: //fallthru
            case ML_PUSHRETVALBUFFER2: //fallthru
            case ML_PUSHRETVALBUFFER4:
                break;

            case ML_HANDLEREF_C2N:
                break;

            case ML_CREATE_MARSHALER_CSTR: //fallthru
                pMLCode += (sizeof(UINT8) * 2);  //go past bestfitmapping & throwonunmappable char vars.
                
            case ML_CREATE_MARSHALER_WSTR: //fallthru

            if (*pMLCode == ML_PRERETURN_C2N_RETVAL)
            {
                    pMLCode++;
                    break;
            }
            else
            {
                RETURN FALSE;
            }

            case ML_CSTRBUILDER_C2N:
                pMLCode += (sizeof(UINT8) * 2); // go past bestfitmapping & throw char vars.
                break;
                

            case ML_REFSAFEHANDLE_C2N:
                pMLCode += 1 + sizeof(MethodTable*);
                break;

            case ML_SAFEHANDLE_C2N:
                break;

            case ML_RETVALSAFEHANDLE_C2N:
                pMLCode += sizeof(MethodTable*);
                break;

            case ML_RETSAFEHANDLE_C2N:
                pMLCode += sizeof(MethodTable*);
                break;

            case ML_REFCRITICALHANDLE_C2N:
                pMLCode += 1 + sizeof(MethodTable*);
                break;

            case ML_CRITICALHANDLE_C2N:
                break;

            case ML_RETVALCRITICALHANDLE_C2N:
                pMLCode += sizeof(MethodTable*);
                break;

            case ML_RETCRITICALHANDLE_C2N:
                pMLCode += sizeof(MethodTable*);
                break;

            default:
            RETURN FALSE;
        }

        if (gMLInfo[mlcode].m_frequiresCleanup)
        {
            fNeedsCleanup = TRUE;
        }
    }



    if (*(pMLCode) == ML_THROWIFHRFAILED)
    {
        return FALSE;
    }

    if (*(pMLCode) == ML_SETSRCTOLOCAL)
    {
        pMLCode += 3;
    }

    while (ML_END != (mlcode = *(pMLCode++)))
    {
        switch (mlcode)
        {
            case ML_RETURN_C2N_RETVAL:
            case ML_CSTRBUILDER_C2N_POST:
            case ML_REFSAFEHANDLE_C2N_POST:
            case ML_RETVALSAFEHANDLE_C2N_POST:
            case ML_RETSAFEHANDLE_C2N_POST:
            case ML_REFCRITICALHANDLE_C2N_POST:
            case ML_RETVALCRITICALHANDLE_C2N_POST:
            case ML_RETCRITICALHANDLE_C2N_POST:
                pMLCode += 2;
                break;

            case ML_COPY4:
            case ML_COPY8:
            case ML_COPYI1:
            case ML_COPYU1:
            case ML_COPYI2:
            case ML_COPYU2:
            case ML_COPYI4:
            case ML_COPYU4:
            case ML_CBOOL_N2C:
            case ML_BOOL_N2C:
                break;

            default:
            RETURN FALSE;
        }
    }

    //-----------------------------------------------------------------------
    // Qualification stage done. If we've gotten this far, we MUST return
    // TRUE or throw an exception.
    //-----------------------------------------------------------------------

    // EAX contains MethodDesc* from the precode. Push it here
    // to initialize m_Datum field of the frame.
    psl->X86EmitPushReg(kEAX);

    //-----------------------------------------------------------------------
    // Generate the standard prolog
    //-----------------------------------------------------------------------

    TADDR frameVptr;
    unsigned sizeOfNegSpace;
    unsigned offsetOfFlags;
    
    {
        if (fNeedsCleanup)
        {
            frameVptr = NDirectMethodFrameStandaloneCleanup::GetMethodFrameVPtr();
            sizeOfNegSpace = NDirectMethodFrameStandaloneCleanup::GetNegSpaceSize();
            offsetOfFlags = NDirectMethodFrameStandaloneCleanup::GetOffsetOfLeaveRuntimeFlags();
        }
        else
        {
            frameVptr = NDirectMethodFrameStandalone::GetMethodFrameVPtr();
            sizeOfNegSpace = NDirectMethodFrameStandalone::GetNegSpaceSize();
            offsetOfFlags = NDirectMethodFrameStandalone::GetOffsetOfLeaveRuntimeFlags();
        }
    }

    psl->EmitMethodStubProlog(frameVptr);

    // See NOTE on Frame Size C_ASSERT usage in this file 
    C_ASSERT(sizeof(NDirectMethodFrame::NegInfo)  - sizeof(FramedMethodFrame::NegInfo) == 4);

    // a flag that is set after LeaveRuntime has been called if hosted,
    // and reset after EnterRuntime has been called.
    psl->X86EmitPushImm32(FLAG_ENTER_RUNTIME_NOT_REQUIRED);

    //------------------------------------------------------------------------
    // If needs cleanup, reserve space for cleanup pointer.
    //------------------------------------------------------------------------
    if (fNeedsCleanup)
    {
        // if the frame size were to change then the stubs have to be revisited for correctness
        // kindly revist this logic and then update the constant so that the C_ASSERT will again fire
        // if someone were to change the frame size.  You are expected to keep this hard coded constant
        // up to date so that changes in the frame size trigger errors at compile time if the code is not altered

        // See NOTE on Frame Size C_ASSERT usage in this file 
        C_ASSERT(sizeof(NDirectMethodFrameEx::NegInfo)  - sizeof(NDirectMethodFrame::NegInfo) == 12);
    
        // push  to make room for the Checkpoint in the neg info
        psl->X86EmitPushImm32(0);
        psl->X86EmitPushImm32(0);

        // push to make room for the cleanup worker list
        psl->X86EmitPushImm32(0);
    }
    
    psl->X86EmitPushImm32(GetProcessGSCookie());

    // Frame is ready to be inspected by debugger for patch location
    psl->EmitPatchLabel();

    //-----------------------------------------------------------------------
    // Add space for locals
    //-----------------------------------------------------------------------
    
    psl->X86EmitSubEsp(pheader->m_cbLocals);

    if (fNeedsCleanup)
    {
        // push ebx // thread
        psl->X86EmitPushReg(kEBX);
        // push esi // frame
        psl->X86EmitPushReg(kESI);

        // call DoCheckPointForCleanup
        psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) DoCheckPointForCleanup), 8); 
    }

    INT32 locbase = 0 - (sizeOfNegSpace + pheader->m_cbLocals);

    INT32  locofs = locbase;
    UINT32 ofs;
    ofs = 0;
                   
    UINT32 fBestFitMapping;
    UINT32 fThrowOnUnmappableChar;

#ifdef _X86_
#endif

    // The following variables track the local offset of the context structures used by SafeHandles and CriticalHandles when
    // returned directly. They're used to directly inline return processing for these items (essentially back propagating the native
    // handle into the managed handle wrapper) in order to ensure we do this before letting a pending thread abort in.
    INT32 retSafeHandleLoc = 0x7ffffff;
    INT32 retCriticalHandleLoc = 0x7ffffff;

    //-----------------------------------------------------------------------
    // Generate code to marshal each parameter.
    //-----------------------------------------------------------------------
    pMLCode = pheader->GetMLCode();
    while (ML_INTERRUPT != (mlcode = *(pMLCode++)))
    {
        switch (mlcode)
        {
            case ML_COPY4:
            case ML_COPYPINNEDGCREF:
                psl->X86EmitIndexPush(kESI, ofs);
                ofs += 4;
                break;

            case ML_COPY8:
                psl->X86EmitIndexPush(kESI, ofs+4);
                psl->X86EmitIndexPush(kESI, ofs);
                ofs += 8;
                break;

            case ML_HANDLEREF_C2N:
                psl->X86EmitIndexPush(kESI, ofs+4);
                ofs += 8;
                break;

            case ML_CBOOL_C2N:
                {
                    //    mov eax,[esi+ofs]
                    psl->X86EmitIndexRegLoad(kEAX, kESI, ofs);
                    //    xor  ecx,ecx
                    //    test al,al
                    //    setne cl
                    static const BYTE code[] = {0x33,0xc9,0x84,0xc0,0x0f,0x95,0xc1};
                    psl->EmitBytes(code, sizeof(code));
                    //    push ecx
                    psl->X86EmitPushReg(kECX);
                    ofs += 4;
                }
                break;

            case ML_REFBLITTABLEVALUECLASS_C2N:
                {
                    // mov eax, [esi+ofs]
                    psl->X86EmitOp(0x8b, kEAX, kESI, ofs);

                    // push eax
                    psl->X86EmitPushReg(kEAX);

                    ofs += sizeof(LPVOID);
                }
                break;


            case ML_PINNEDUNISTR_C2N: {


                // mov eax, [esi+OFS]
                psl->X86EmitIndexRegLoad(kEAX, kESI, ofs);
                // test eax,eax
                psl->Emit16(0xc085);
                CodeLabel *plabel = psl->NewCodeLabel();
                // jz LABEL
                psl->X86EmitCondJump(plabel, X86CondCode::kJZ);
                // add eax, BUFOFS
                psl->X86EmitAddReg(kEAX, (UINT8)(StringObject::GetBufferOffset()));

                psl->EmitLabel(plabel);

                // push eax
                psl->X86EmitPushReg(kEAX);

                ofs += 4;
                }
                break;


            case ML_BLITTABLELAYOUTCLASS_C2N: {


                // mov eax, [esi+OFS]
                psl->X86EmitIndexRegLoad(kEAX, kESI, ofs);
                // test eax,eax
                psl->Emit16(0xc085);
                CodeLabel *plabel = psl->NewCodeLabel();
                psl->X86EmitCondJump(plabel, X86CondCode::kJZ);

                // lea eax, [eax+DATAPTR]
                psl->X86EmitOp(0x8d, kEAX, kEAX, Object::GetOffsetOfFirstField());

                // LABEL:
                psl->EmitLabel(plabel);
                psl->X86EmitPushReg(kEAX);


                ofs += 4;
            }
            break;

            case ML_CSTR_C2N:
            {
                    fBestFitMapping = (UINT32) ((*(UINT8*)pMLCode == 0) ? 0 : 1);
                    pMLCode += sizeof(UINT8);
                    fThrowOnUnmappableChar = (UINT32) ((*(UINT8*)pMLCode == 0) ? 0 : 1);
                    pMLCode += sizeof(UINT8);

                    // push cleanup worklist
                    //   lea eax, [esi + NDirectMethodFrameEx.CleanupWorklist]
                    psl->X86EmitOp(0x8d, kEAX, kESI, NDirectMethodFrameEx::GetOffsetOfCleanupWorkList());
                    //   push eax
                    psl->X86EmitPushReg(kEAX);

                    //  push    fThrowOnUnmappableChar
                    //  push    fBestFitMapping
                    psl->X86EmitPushImm32(fThrowOnUnmappableChar);
                    psl->X86EmitPushImm32(fBestFitMapping);

                    //   push [esi + OFS]
                    psl->X86EmitIndexPush(kESI, ofs);

                    //   lea ecx, [esi + locofs]
                    psl->X86EmitOp(0x8d, kECX, kESI, locofs);
                    //   push ecx
                    psl->X86EmitPushReg(kECX); 
                   
                    //   call ML_CSTR_C2N_SR::DoConversion(STRINGREF, UINT32, UINT32, CleanupWorkList*) via a static wrapper
                    psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) DoConversionWrapper_CSTR), 20);

                    //   push eax
                    psl->X86EmitPushReg(kEAX);
                    ofs += 4;
                    locofs += sizeof(ML_CSTR_C2N_SR);
            }
            break;


            case ML_BUMPSRC:
                ofs += *( (INT16*)pMLCode );
                pMLCode += 2;
                break;

            case ML_PINNEDISOMORPHICARRAY_C2N_EXPRESS:
                {
                    UINT16 dataofs = *( (INT16*)pMLCode );
                    pMLCode += 2;
                    _ASSERTE(dataofs);

                    // mov eax,[esi+ofs]
                    psl->X86EmitIndexRegLoad(kEAX, kESI, ofs);
                    // test eax,eax
                    psl->Emit16(0xc085);
                    CodeLabel *plabel = psl->NewCodeLabel();
                    // jz LABEL
                    psl->X86EmitCondJump(plabel, X86CondCode::kJZ);
                    // lea eax, [eax + dataofs]
                    psl->X86EmitOp(0x8d, kEAX, kEAX, (UINT32)dataofs);
    
                    psl->EmitLabel(plabel);
                    // push eax
                    psl->X86EmitPushReg(kEAX);

    
                    ofs += 4;

                }
                break;


            case ML_PUSHRETVALBUFFER1: //fallthru
            case ML_PUSHRETVALBUFFER2: //fallthru
            case ML_PUSHRETVALBUFFER4:
                // lea eax, [esi+locofs]
                // mov [eax],0
                // push eax

                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locofs);
                psl->X86EmitOffsetModRM(0xc7, (X86Reg)0, kEAX, 0);
                psl->Emit32(0);
                psl->X86EmitPushReg(kEAX);

                locofs += 4;
                break;

            case ML_CREATE_MARSHALER_CSTR:
                fBestFitMapping = (UINT32) ((*(UINT8*)pMLCode == 0) ? 0 : 1);
                pMLCode += sizeof(UINT8);
                fThrowOnUnmappableChar = (UINT32) ((*(UINT8*)pMLCode == 0) ? 0 : 1);
                pMLCode += sizeof(UINT8);

                _ASSERTE(*pMLCode == ML_PRERETURN_C2N_RETVAL);

                //  push    fThrowOnUnmappableChar
                //  push    fBestFitMapping
                psl->X86EmitPushImm32(fThrowOnUnmappableChar);
                psl->X86EmitPushImm32(fBestFitMapping);
                
                //  lea     eax, [esi+locofs]
                //  push    eax       ;; push plocalwalk
                //  lea     eax, [esi + Frame.CleanupWorkList]
                //  push    eax       ;; push CleanupWorkList
                //  push    esi       ;; push Frame
                //  call    DoMLCreateMarshaler?Str

                //  push    edx       ;; push garbage (this will be overwritten by actual argument)
                //  push    esp       ;; push address of the garbage we just pushed
                //  lea     eax, [esi+locofs]
                //  push    eax       ;; push marshaler
                //  call    DoMLPrereturnC2N

                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locofs);
                psl->X86EmitPushReg(kEAX);

                psl->X86EmitOp(0x8d, kEAX, kESI, NDirectMethodFrameEx::GetOffsetOfCleanupWorkList());
                psl->X86EmitPushReg(kEAX);

                psl->X86EmitPushReg(kESI);

                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) DoMLCreateMarshalerCStr), 20);

                psl->X86EmitPushReg(kEDX);
                psl->X86EmitPushReg((X86Reg)4 /*kESP*/);
                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locofs);
                psl->X86EmitPushReg(kEAX);
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) DoMLPrereturnC2N), 8);

                locofs += gMLInfo[mlcode].m_cbLocal;
                pMLCode++;

                break;


            case ML_CREATE_MARSHALER_WSTR:
                _ASSERTE(*pMLCode == ML_PRERETURN_C2N_RETVAL);

                //  lea     eax, [esi+locofs]
                //  push    eax       ;; push plocalwalk
                //  lea     eax, [esi + Frame.CleanupWorkList]
                //  push    eax       ;; push CleanupWorkList
                //  push    esi       ;; push Frame
                //  call    DoMLCreateMarshaler?Str

                //  push    edx       ;; push garbage (this will be overwritten by actual argument)
                //  push    esp       ;; push address of the garbage we just pushed
                //  lea     eax, [esi+locofs]
                //  push    eax       ;; push marshaler
                //  call    DoMLPrereturnC2N

                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locofs);
                psl->X86EmitPushReg(kEAX);

                psl->X86EmitOp(0x8d, kEAX, kESI, NDirectMethodFrameEx::GetOffsetOfCleanupWorkList());
                psl->X86EmitPushReg(kEAX);

                psl->X86EmitPushReg(kESI);
                switch (mlcode)
                {
                    case ML_CREATE_MARSHALER_WSTR:
                        psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) DoMLCreateMarshalerWStr), 12);
                        break;
                    default:
                        _ASSERTE(0);
                }
                psl->X86EmitPushReg(kEDX);
                psl->X86EmitPushReg((X86Reg)4 /*kESP*/);
                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locofs);
                psl->X86EmitPushReg(kEAX);
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) DoMLPrereturnC2N), 8);

                locofs += gMLInfo[mlcode].m_cbLocal;
                pMLCode++;

                break;

            case ML_CSTRBUILDER_C2N:
                fBestFitMapping = (UINT32) ((*(UINT8*)pMLCode == 0) ? 0 : 1);
                pMLCode += sizeof(UINT8);
                fThrowOnUnmappableChar = (UINT32) ((*(UINT8*)pMLCode == 0) ? 0 : 1);
                pMLCode += sizeof(UINT8);

                // push cleanup worklist
                //   lea eax, [esi + NDirectMethodFrameEx.CleanupWorklist]
                psl->X86EmitOp(0x8d, kEAX, kESI, NDirectMethodFrameEx::GetOffsetOfCleanupWorkList());
                //   push eax
                psl->X86EmitPushReg(kEAX);

                //  push    fThrowOnUnmappableChar
                //  push    fBestFitMapping
                psl->X86EmitPushImm32(fThrowOnUnmappableChar);
                psl->X86EmitPushImm32(fBestFitMapping);

                //   push the StringBufferRef reference
                //   lea eax, [esi + ofs]
                //   push eax
                psl->X86EmitOp(0x8d, kEAX, kESI, ofs);
                psl->X86EmitPushReg(kEAX);
                ofs += sizeof(STRINGBUFFERREF);                

                // Setup 'this' to point to local ML_CSTRBUILDER_C2N_SR structure
                //   lea ecx, [esi + locofs]
                //   push ecx
                psl->X86EmitOp(0x8d, kECX, kESI, locofs);
                psl->X86EmitPushReg(kECX);
                locofs += sizeof(ML_CSTRBUILDER_C2N_SR);
               
                //   call ML_CSTRBUILDER_C2N_SR::DoConversionStatic(ML_CSTRBUILDER_C2N_SR, STRINGBUFFERREF, UINT32, UINT32, CleanupWorkList*)
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ML_CSTRBUILDER_C2N_SR::DoConversionStatic), 20);

                //   push eax
                psl->X86EmitPushReg(kEAX);
                break;


            case ML_REFSAFEHANDLE_C2N:
            {
                // Get SafeHandle subtype
                MethodTable *pMT = *((MethodTable**)pMLCode);
                pMLCode += sizeof(MethodTable*);

                // Get IN/OUT flags
                UINT8 bFlags = *((UINT8*)pMLCode);
                pMLCode += sizeof(UINT8);

                // Push cleanup worklist
                //   lea eax, [esi + NDirectMethodFrameEx.CleanupWorklist]
                //   push eax
                psl->X86EmitOp(0x8d, kEAX, kESI, NDirectMethodFrameEx::GetOffsetOfCleanupWorkList());
                psl->X86EmitPushReg(kEAX);

                // Push IN/OUT flags
                //   push bFlags
                psl->X86EmitPushImm8(bFlags);

                // Push SafeHandle subtype (MethodTable*)
                //   push pMT
                psl->X86EmitPushImm32((UINT32)(size_t)pMT);

                // Push SafeHandle reference
                //   push [esi+ofs]
                psl->X86EmitIndexPush(kESI, ofs);
                ofs += sizeof(SAFEHANDLE*);

                // Setup 'this' to point to local ML_REFSAFEHANDLE_C2N_SR structure
                //   lea eax, [esi+locofs]
                //   push eax
                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locofs);
                psl->X86EmitPushReg(kEAX);
                locofs += sizeof(ML_REFSAFEHANDLE_C2N_SR);

                // Call ML_REFSAFEHANDLE_C2N_SR::DoConversion(SafeHandle**, MethodTable*, UINT8, CleanupWorkList*)
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ML_REFSAFEHANDLE_C2N_SR::DoConversionStatic), 20);

                // Push result (native handle reference)
                //   push eax
                psl->X86EmitPushReg(kEAX);

                break;
            }

            case ML_SAFEHANDLE_C2N:
                // Push cleanup worklist
                //   lea eax, [esi + NDirectMethodFrameEx.CleanupWorklist]
                //   push eax
                psl->X86EmitOp(0x8d, kEAX, kESI, NDirectMethodFrameEx::GetOffsetOfCleanupWorkList());
                psl->X86EmitPushReg(kEAX);

                // Push SafeHandle reference
                //   lea eax, [esi + ofs]
                //   push eax
                psl->X86EmitOp(0x8d, kEAX, kESI, ofs);
                psl->X86EmitPushReg(kEAX);
                ofs += sizeof(SAFEHANDLE);

                // Call SafeHandleC2NHelper(SAFEHANDLE*, CleanupWorkList*)
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) SafeHandleC2NHelper), 8);

                // Push result (native handle)
                //   push eax
                psl->X86EmitPushReg(kEAX);

                break;

            case ML_RETVALSAFEHANDLE_C2N:
            {
                // Get SafeHandle subtype
                MethodTable *pMT = *((MethodTable**)pMLCode);
                pMLCode += sizeof(MethodTable*);

                // Push cleanup worklist
                //   lea eax, [esi + NDirectMethodFrameEx.CleanupWorklist]
                //   push eax
                psl->X86EmitOp(0x8d, kEAX, kESI, NDirectMethodFrameEx::GetOffsetOfCleanupWorkList());
                psl->X86EmitPushReg(kEAX);

                // Push IN/OUT flags (retval is OUT only)
                //   push ML_OUT
                psl->X86EmitPushImm8(ML_OUT);

                // Push SafeHandle subtype (MethodTable*)
                //   push pMT
                psl->X86EmitPushImm32((UINT32)(size_t)pMT);

                // Push NULL SafeHandle reference (since this is an OUT parameter)
                //   push 0
                psl->X86EmitPushImm8(0);

                // Setup 'this' to point to local ML_REFSAFEHANDLE_C2N_SR structure
                //   lea eax, [esi+locofs]
                //   push eax
                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locofs);
                psl->X86EmitPushReg(kEAX);
                locofs += sizeof(ML_REFSAFEHANDLE_C2N_SR);

                // Call ML_REFSAFEHANDLE_C2N_SR::DoConversion(SafeHandle**, MethodTable*, UINT8, CleanupWorkList*)
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ML_REFSAFEHANDLE_C2N_SR::DoConversionStatic), 20);

                // Push result (native handle reference)
                //   push eax
                psl->X86EmitPushReg(kEAX);

                break;
            }

            case ML_RETSAFEHANDLE_C2N:
            {
                // Get SafeHandle subtype
                MethodTable *pMT = *((MethodTable**)pMLCode);
                pMLCode += sizeof(MethodTable*);

                // Push cleanup worklist
                //   lea eax, [esi + NDirectMethodFrameEx.CleanupWorklist]
                //   push eax
                psl->X86EmitOp(0x8d, kEAX, kESI, NDirectMethodFrameEx::GetOffsetOfCleanupWorkList());
                psl->X86EmitPushReg(kEAX);

                // Push SafeHandle subtype (MethodTable*)
                //   push pMT
                psl->X86EmitPushImm32((UINT32)(size_t)pMT);

                // Setup 'this' to point to local ML_RETSAFEHANDLE_C2N_SR structure
                //   lea eax, [esi+locofs]
                //   push eax
                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locofs);
                psl->X86EmitPushReg(kEAX);
                retSafeHandleLoc = locofs;
                locofs += sizeof(ML_RETSAFEHANDLE_C2N_SR);

                // Call ML_RETSAFEHANDLE_C2N_SR::PreReturn(MethodTable*, CleanupWorkList*)
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ML_RETSAFEHANDLE_C2N_SR::PreReturnStatic), 12);

                break;
            }

            case ML_REFCRITICALHANDLE_C2N:
            {
                // Get CriticalHandle subtype
                MethodTable *pMT = *((MethodTable**)pMLCode);
                pMLCode += sizeof(MethodTable*);

                // Get IN/OUT flags
                UINT8 bFlags = *((UINT8*)pMLCode);
                pMLCode += sizeof(UINT8);

                // Push cleanup worklist
                //   lea eax, [esi + NDirectMethodFrameEx.CleanupWorklist]
                //   push eax
                psl->X86EmitOp(0x8d, kEAX, kESI, NDirectMethodFrameEx::GetOffsetOfCleanupWorkList());
                psl->X86EmitPushReg(kEAX);

                // Push IN/OUT flags
                //   push bFlags
                psl->X86EmitPushImm8(bFlags);

                // Push CriticalHandle subtype (MethodTable*)
                //   push pMT
                psl->X86EmitPushImm32((UINT32)(size_t)pMT);

                // Push CriticalHandle reference
                //   push [esi+ofs]
                psl->X86EmitIndexPush(kESI, ofs);
                ofs += sizeof(CRITICALHANDLE*);

                // Setup 'this' to point to local ML_REFCRITICALHANDLE_C2N_SR structure
                //   lea eax, [esi+locofs]
                //   push eax
                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locofs);
                psl->X86EmitPushReg(kEAX);
                locofs += sizeof(ML_REFCRITICALHANDLE_C2N_SR);

                // Call ML_REFCRITICALHANDLE_C2N_SR::DoConversion(CriticalHandle**, MethodTable*, UINT8, CleanupWorkList*)
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ML_REFCRITICALHANDLE_C2N_SR::DoConversionStatic), 20);

                // Push result (native handle reference)
                //   push eax
                psl->X86EmitPushReg(kEAX);

                break;
            }

            case ML_CRITICALHANDLE_C2N:
                // Grab handle value from CriticalHandle and push it
                //   mov eax, [esi + ofs]
                //   push [eax + CriticalHandle.m_handle]
                psl->X86EmitIndexRegLoad(kEAX, kESI, ofs);
                psl->X86EmitIndexPush(kEAX, (DWORD)CriticalHandle::GetHandleOffset());
                ofs += sizeof(SAFEHANDLE);

                break;

            case ML_RETVALCRITICALHANDLE_C2N:
            {
                // Get CriticalHandle subtype
                MethodTable *pMT = *((MethodTable**)pMLCode);
                pMLCode += sizeof(MethodTable*);

                // Push cleanup worklist
                //   lea eax, [esi + NDirectMethodFrameEx.CleanupWorklist]
                //   push eax
                psl->X86EmitOp(0x8d, kEAX, kESI, NDirectMethodFrameEx::GetOffsetOfCleanupWorkList());
                psl->X86EmitPushReg(kEAX);

                // Push IN/OUT flags (retval is OUT only)
                //   push ML_OUT
                psl->X86EmitPushImm8(ML_OUT);

                // Push CriticalHandle subtype (MethodTable*)
                //   push pMT
                psl->X86EmitPushImm32((UINT32)(size_t)pMT);

                // Push NULL CriticalHandle reference (since this is an OUT parameter)
                //   push 0
                psl->X86EmitPushImm8(0);

                // Setup 'this' to point to local ML_REFCRITICALHANDLE_C2N_SR structure
                //   lea eax, [esi+locofs]
                //   push eax
                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locofs);
                psl->X86EmitPushReg(kEAX);
                locofs += sizeof(ML_REFCRITICALHANDLE_C2N_SR);

                // Call ML_REFCRITICALHANDLE_C2N_SR::DoConversion(CriticalHandle**, MethodTable*, UINT8, CleanupWorkList*)
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ML_REFCRITICALHANDLE_C2N_SR::DoConversionStatic), 20);

                // Push result (native handle reference)
                //   push eax
                psl->X86EmitPushReg(kEAX);

                break;
            }

            case ML_RETCRITICALHANDLE_C2N:
            {
                // Get CriticalHandle subtype
                MethodTable *pMT = *((MethodTable**)pMLCode);
                pMLCode += sizeof(MethodTable*);

                // Push cleanup worklist
                //   lea eax, [esi + NDirectMethodFrameEx.CleanupWorklist]
                //   push eax
                psl->X86EmitOp(0x8d, kEAX, kESI, NDirectMethodFrameEx::GetOffsetOfCleanupWorkList());
                psl->X86EmitPushReg(kEAX);

                // Push CriticalHandle subtype (MethodTable*)
                //   push pMT
                psl->X86EmitPushImm32((UINT32)(size_t)pMT);

                // Setup 'this' to point to local ML_RETCRITICALHANDLE_C2N_SR structure
                //   lea eax, [esi+locofs]
                //   push eax
                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locofs);
                psl->X86EmitPushReg(kEAX);
                retCriticalHandleLoc = locofs;
                locofs += sizeof(ML_RETCRITICALHANDLE_C2N_SR);

                // Call ML_RETCRITICALHANDLE_C2N_SR::PreReturn(MethodTable*, CleanupWorkList*)
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ML_RETCRITICALHANDLE_C2N_SR::PreReturnStatic), 12);

                break;
            }

            default:
                _ASSERTE(0);
        }
    }
    UINT32 numStackBytes;
    numStackBytes = pheader->m_cbStackPop;






    CodeLabel *pRareEnable,  *pEnableRejoin;
    CodeLabel *pRareDisable, *pDisableRejoin;
    pRareEnable    = psl->NewCodeLabel();
    pEnableRejoin  = psl->NewCodeLabel();
    pRareDisable   = psl->NewCodeLabel();
    pDisableRejoin = psl->NewCodeLabel();

    //-----------------------------------------------------------------------
    // Generate the inline part of enabling preemptive GC
    //-----------------------------------------------------------------------
    psl->EmitEnable(pRareEnable);
    psl->EmitLabel(pEnableRejoin);

#ifdef PROFILING_SUPPORTED

    // Notify the profiler of a call out of managed code
    if (CORProfilerTrackTransitions())
    {
        // Save registers
        psl->X86EmitPushReg(kEAX);
        psl->X86EmitPushReg(kECX);
        psl->X86EmitPushReg(kEDX);

        psl->X86EmitPushImm32(COR_PRF_TRANSITION_CALL);     // Reason
        psl->X86EmitPushReg(kESI);                          // Frame*
        psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ProfilerManagedToUnmanagedTransition), 8);

        // Restore registers
        psl->X86EmitPopReg(kEDX);
        psl->X86EmitPopReg(kECX);
        psl->X86EmitPopReg(kEAX);
    }
#endif // PROFILING_SUPPORTED

    {
        //-----------------------------------------------------------------------
        // Invoke the DLL target.
        //-----------------------------------------------------------------------

        if (pheader->m_Flags & MLHF_THISCALL)
        {
            if (pheader->m_Flags & MLHF_THISCALLHIDDENARG)
            {
                // pop eax
                psl->X86EmitPopReg(kEAX);
                // pop ecx
                psl->X86EmitPopReg(kECX);
                // push eax
                psl->X86EmitPushReg(kEAX);
            }
            else
            {
                // pop ecx
                psl->X86EmitPopReg(kECX);
            }
        }

        //  mov eax, [CURFRAME.MethodDesc]
        psl->X86EmitIndexRegLoad(kEAX, kESI, FramedMethodFrame::GetOffsetOfMethod());

        if (NDirect::IsHostHookEnabled())
        {
            // Do we need a hook?

            CodeLabel *labelNoHook = psl->NewCodeLabel();

            // test byte ptr [eax+offsetof(NDirectMethodDesc, ndirect.m_wFlags)], kNeedHostHook
            psl->Emit16(0x40f6);
            psl->Emit8(offsetof(NDirectMethodDesc, ndirect.m_wFlags));
            _ASSERTE(FitsInI1(NDirectMethodDesc::kNeedHostHook));
            psl->Emit8(NDirectMethodDesc::kNeedHostHook);

            psl->X86EmitCondJump(labelNoHook, X86CondCode::kJZ);

            // save eax, ecx, edx
            psl->X86EmitPushReg(kEAX);
            psl->X86EmitPushReg(kECX);
            psl->X86EmitPushReg(kEDX);

            //  push [eax + MethodDesc.NDirectTarget]
            psl->X86EmitIndexPush(kEAX, NDirectMethodDesc::GetOffsetofNativeNDirectTarget());
            // call Thread::LeaveRuntime
            psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID)Thread::LeaveRuntimeThrowComplus), 4);
            
            // restore eax, ecx, edx
            psl->X86EmitPopReg(kEDX);
            psl->X86EmitPopReg(kECX);
            psl->X86EmitPopReg(kEAX);

            // mark LeaveRuntime has been called
            // mov byte ptr [esi+NDirectMethodFrame::m_flags], FLAG_ENTER_RUNTIME_REQUIRED
            psl->X86EmitOffsetModRM(0xc6, (X86Reg)0, kESI, NDirectMethodFrame::GetOffsetOfLeaveRuntimeFlags());
            psl->Emit8(FLAG_ENTER_RUNTIME_REQUIRED);

            psl->EmitLabel(labelNoHook);

#if _DEBUG
            // Call through debugger logic to make sure it works
            psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) CheckExitFrameDebuggerCalls), 0, TRUE);
#else

            //  call [eax + MethodDesc.NDirectTarget]
            psl->X86EmitOffsetModRM(0xff, (X86Reg)2, kEAX, NDirectMethodDesc::GetOffsetofNativeNDirectTarget());
            psl->EmitReturnLabel();
#endif
        }
        else
        {
#if _DEBUG
            // Call through debugger logic to make sure it works
            psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) CheckExitFrameDebuggerCalls), 0, TRUE);
#else

            //  mov eax, [eax + NDirectMethodDesc.ndirect.m_pWriteableData]
            psl->X86EmitIndexRegLoad(kEAX, kEAX, offsetof(NDirectMethodDesc, ndirect.m_pWriteableData));
            //  call [eax + NDirectWriteableData.m_pNDirectTarget]
            psl->X86EmitOffsetModRM(0xff, (X86Reg)2, kEAX, offsetof(NDirectWriteableData, m_pNDirectTarget));
            psl->EmitReturnLabel();
#endif
        }

        if (pheader->m_Flags & MLHF_SETLASTERROR)
        {
            psl->EmitSaveLastError();
        }
    }

#ifdef PROFILING_SUPPORTED
    // Notify the profiler of a return from a managed->unmanaged call
    if (CORProfilerTrackTransitions())
    {
        // Save registers
        psl->X86EmitPushReg(kEAX);
        psl->X86EmitPushReg(kECX);
        psl->X86EmitPushReg(kEDX);

        psl->X86EmitPushImm32(COR_PRF_TRANSITION_RETURN);   // Reason
        psl->X86EmitPushReg(kESI);                          // FrameID
        psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ProfilerUnmanagedToManagedTransition), 8);

        // Restore registers
        psl->X86EmitPopReg(kEDX);
        psl->X86EmitPopReg(kECX);
        psl->X86EmitPopReg(kEAX);
    }
#endif // PROFILING_SUPPORTED

    if (NDirect::IsHostHookEnabled())
    {
        int offset;
        {
            offset = NDirectMethodFrame::GetOffsetOfLeaveRuntimeFlags();
        }
        CodeLabel *pNoHook = psl->NewCodeLabel();

        // test byte ptr [esi+offset], 1
        psl->X86EmitOffsetModRM(0xf6, (X86Reg)0, kESI, offset);
        psl->Emit8(FLAG_ENTER_RUNTIME_REQUIRED);
        // jne
        psl->X86EmitCondJump(pNoHook, X86CondCode::kJZ);

        // save regs
        psl->X86EmitPushReg(kEAX);
        psl->X86EmitPushReg(kEDX);

        // call EnterRuntimeHelper
        psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID)Thread::EnterRuntime), 0);

        // restore regs
        psl->X86EmitPopReg(kEDX);
        psl->X86EmitPopReg(kEAX);

        // UnMark flag for LeaveRuntime
        psl->X86EmitOffsetModRM(0xc6, (X86Reg)0, kESI, offset);
        psl->Emit8(FLAG_ENTER_RUNTIME_NOT_REQUIRED);

        psl->EmitLabel(pNoHook);
    }
    
    //-----------------------------------------------------------------------
    // Generate the inline part of disabling preemptive GC
    // If we're returning a SafeHandle or CriticalHandle we set a flag on the frame to indicate that we shouldn't check for thread
    // abort just yet (or we'd leak the native handle just returned).
    //-----------------------------------------------------------------------
    if (retSafeHandleLoc != 0x7ffffff || retCriticalHandleLoc != 0x7ffffff)
    {
        //  or byte ptr [esi + offsetOfFlags], FLAG_SKIP_HANDLE_THREAD_ABORT
        psl->X86EmitOffsetModRM(0x80, (X86Reg)1, kESI, offsetOfFlags);
        psl->Emit8(FLAG_SKIP_HANDLE_THREAD_ABORT);
    }
    psl->EmitDisable(pRareDisable, /*fCallIn=*/FALSE, kEBX);
    psl->EmitLabel(pDisableRejoin);

    // If we're returning a SafeHandle or CriticalHandle we have to do this before any other output marshalling to ensure a thread
    // abort doesn't get in and leak the native handle.
    INT32 iContextOffsetFromFrame = 0;
    if (retSafeHandleLoc != 0x7ffffff || retCriticalHandleLoc != 0x7ffffff)
    {
        // SafeHandles and CriticalHandles are treated much the same, they differ only in some key offsets.        
        INT32 iHandleOffsetInObject;
        if (retSafeHandleLoc != 0x7ffffff)
        {
            _ASSERTE(retCriticalHandleLoc == 0x7ffffff);

            iContextOffsetFromFrame = retSafeHandleLoc + ML_RETSAFEHANDLE_C2N_SR::OffsetOfHandleRef();
            iHandleOffsetInObject = (INT32)SafeHandle::GetHandleOffset();
        }
        else
        {
            _ASSERTE(retCriticalHandleLoc != 0x7ffffff);
            _ASSERTE(retSafeHandleLoc == 0x7ffffff);

            iContextOffsetFromFrame = retCriticalHandleLoc + ML_RETCRITICALHANDLE_C2N_SR::OffsetOfHandleRef();
            iHandleOffsetInObject = (INT32)CriticalHandle::GetHandleOffset();
        }

        // Preserve a work register.
        //   push ecx
        psl->X86EmitPushReg(kECX);

        // Grab the pointer to the context structure from locals and from there extract the pointer to the handle reference.
        //   mov ecx, [esi + iContextOffsetFromFrame]
        psl->X86EmitIndexRegLoad(kECX, kESI, iContextOffsetFromFrame);

        // Dereference to get the actual handle object.
        //   mov ecx, [ecx]
        psl->X86EmitIndexRegLoad(kECX, kECX, 0);

        // Plug the native handle value into the object.
        //   mov [ecx + iHandleOffsetInObject], eax
        psl->X86EmitIndexRegStore(kECX, iHandleOffsetInObject, kEAX);

        // The result we return is the managed wrapper rather than the raw handle value.
        //   mov eax, ecx
        psl->X86EmitMovRegReg(kEAX, kECX);

        // Restore the work register.
        //   pop ecx
        psl->X86EmitPopReg(kECX);
    }

    //-----------------------------------------------------------------------
    // Marshal the return value
    //-----------------------------------------------------------------------


    if (*pMLCode == ML_SETSRCTOLOCAL)
    {
        pMLCode++;
        UINT16 bufidx = *((UINT16*)(pMLCode));
        pMLCode += 2;
        // mov eax, [esi + locbase + bufidx]
        psl->X86EmitIndexRegLoad(kEAX, kESI, locbase+bufidx);

    }


    while (ML_END != (mlcode = *(pMLCode++)))
    {
        switch (mlcode)
        {
            case ML_BOOL_N2C:
            {
                //    xor  ecx,ecx
                //    test eax,eax
                //    setne cl
                //    mov  eax,ecx

                static const BYTE code[] = {0x33,0xc9,0x85,0xc0,0x0f,0x95,0xc1,0x8b,0xc1};
                psl->EmitBytes(code, sizeof(code));
            }
            break;

            case ML_CBOOL_N2C:
            {
                //    xor  ecx,ecx
                //    test al,al
                //    setne cl
                //    mov  eax,ecx
    
                static const BYTE code[] = {0x33,0xc9,0x84,0xc0,0x0f,0x95,0xc1,0x8b,0xc1};
                psl->EmitBytes(code, sizeof(code));
            }
            break;

            case ML_COPY4: //fallthru
            case ML_COPY8: //fallthru
            case ML_COPYI4: //fallthru
            case ML_COPYU4:
            case ML_END:
                //do nothing
                break;

            case ML_COPYU1:
                // movzx eax,al
                psl->Emit8(0x0f);
                psl->Emit16(0xc0b6);
                break;

            case ML_COPYI1:
                // movsx eax,al
                psl->Emit8(0x0f);
                psl->Emit16(0xc0be);
                break;

            case ML_COPYU2:
                // movzx eax,ax
                psl->Emit8(0x0f);
                psl->Emit16(0xc0b7);
                break;

            case ML_COPYI2:
                // movsx eax,ax
                psl->Emit8(0x0f);
                psl->Emit16(0xc0bf);
                break;

            case ML_CSTRBUILDER_C2N_POST:
                {
                    UINT16 locidx = *((UINT16*)(pMLCode));
                    pMLCode += sizeof(UINT16);

                    // Must preserve EAX (since it contains the return value)
                    //    push eax
                    psl->X86EmitPushReg(kEAX);

                    // Setup 'this' to point to the local structure
                    //   lea eax, [esi+locbase+locidx]
                    //   push eax
                    psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locbase+locidx);
                    psl->X86EmitPushReg(kEAX);

                    // Call ML_CSTRBUILDER_C2N_SR::BackPropagateStatic()
                    psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ML_CSTRBUILDER_C2N_SR::BackPropagateStatic), 4);

                    // Restore return value
                    //   pop eax
                    psl->X86EmitPopReg(kEAX);
                }
                break;


            case ML_REFSAFEHANDLE_C2N_POST:
            {
                // Get offset of local ML_REFSAFEHANDLE_C2N_SR structure
                UINT16 locidx = *((UINT16*)(pMLCode));
                pMLCode += sizeof(UINT16);

                // Must preserve EAX (since it contains the return value)
                //    push eax
                psl->X86EmitPushReg(kEAX);

                // Setup 'this' to point to the local structure
                //   lea eax, [esi+locbase+locidx]
                //   push eax
                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locbase+locidx);
                psl->X86EmitPushReg(kEAX);

                // Call ML_REFSAFEHANDLE_C2N_SR::BackPropagate()
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ML_REFSAFEHANDLE_C2N_SR::BackPropagateStatic), 4);

                // Restore return value
                //   pop eax
                psl->X86EmitPopReg(kEAX);

                break;
            }

            case ML_RETVALSAFEHANDLE_C2N_POST:
            {
                // Get offset of local ML_REFSAFEHANDLE_C2N_SR structure
                UINT16 locidx = *((UINT16*)(pMLCode));
                pMLCode += sizeof(UINT16);

                // Setup 'this' to point to the local structure
                //   lea eax, [esi+locbase+locidx]
                //   push eax
                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locbase+locidx);
                psl->X86EmitPushReg(kEAX);

                // Call ML_REFSAFEHANDLE_C2N_SR::DoReturn()
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ML_REFSAFEHANDLE_C2N_SR::DoReturnStatic), 4);

                // SafeHandle object reference is now in EAX. The handle value
                // will be initialized by cleanup list based back propagation.

                break;
            }

            case ML_RETSAFEHANDLE_C2N_POST:
            {
                // Nothing to do, this should have been handled inline immediately after the native call.
                _ASSERTE(retSafeHandleLoc != 0x7ffffff);
                pMLCode += sizeof(UINT16);
                break;
            }

            case ML_REFCRITICALHANDLE_C2N_POST:
            {
                // Get offset of local ML_REFCRITICALHANDLE_C2N_SR structure
                UINT16 locidx = *((UINT16*)(pMLCode));
                pMLCode += sizeof(UINT16);

                // Must preserve EAX (since it contains the return value)
                //    push eax
                psl->X86EmitPushReg(kEAX);

                // Setup 'this' to point to the local structure
                //   lea eax, [esi+locbase+locidx]
                //   push eax
                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locbase+locidx);
                psl->X86EmitPushReg(kEAX);

                // Call ML_REFCRITICALHANDLE_C2N_SR::BackPropagate()
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ML_REFCRITICALHANDLE_C2N_SR::BackPropagateStatic), 4);

                // Restore return value
                //   pop eax
                psl->X86EmitPopReg(kEAX);

                break;
            }

            case ML_RETVALCRITICALHANDLE_C2N_POST:
            {
                // Get offset of local ML_REFCRITICALHANDLE_C2N_SR structure
                UINT16 locidx = *((UINT16*)(pMLCode));
                pMLCode += sizeof(UINT16);

                // Setup 'this' to point to the local structure
                //   lea eax, [esi+locbase+locidx]
                //   push eax
                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locbase+locidx);
                psl->X86EmitPushReg(kEAX);

                // Call ML_REFCRITICALHANDLE_C2N_SR::DoReturn()
                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) ML_REFCRITICALHANDLE_C2N_SR::DoReturnStatic), 4);

                // CriticalHandle object reference is now in EAX. The handle value
                // will be initialized by cleanup list based back propagation.

                break;
            }

            case ML_RETCRITICALHANDLE_C2N_POST:
            {
                // Nothing to do, this should have been handled inline immediately after the native call.
                _ASSERTE(retCriticalHandleLoc != 0x7ffffff);
                pMLCode += sizeof(UINT16);
                break;
            }

            case ML_RETURN_C2N_RETVAL:
            {
                UINT16 locidx = *((UINT16*)(pMLCode));
                pMLCode += 2;

                // lea        eax, [esi + locidx + locbase]
                // push       eax  //push marshaler
                // call       DoMLReturnC2NRetVal   ;; returns oref in eax
                //


                psl->X86EmitOffsetModRM(0x8d, kEAX, kESI, locbase+locidx);
                psl->X86EmitPushReg(kEAX);

                psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) DoMLReturnC2NRetVal), 4);
            }
            break;


            default:
                _ASSERTE(!"Can't get here.");
        }

    }

    if (fNeedsCleanup)
    {
        if ( pheader->GetManagedRetValTypeCat() == MLHF_TYPECAT_GCREF )
        {
            // push eax  // object
            // push esi  // frame
            // call DoCleanupWithGcProtection
            // ;; (possibly promoted) objref left in eax
            psl->X86EmitPushReg(kEAX);
            psl->X86EmitPushReg(kESI);

            psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) DoCleanupWithGcProtection), 8);            
        }
        else
        {
            // Do cleanup
    
            // push eax             //save EAX
            psl->X86EmitPushReg(kEAX);
    
            // push edx             //save EDX
            psl->X86EmitPushReg(kEDX);   

            // push esi             //NDirectMethodFrameEx*
            psl->X86EmitPushReg(kESI); 

            // call CleanupWorkList::Cleanup(NDirectMethodFrameEx*) via a static wrapper
            psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) CleanupWrapper), 4);
   
            // pop edx
            psl->X86EmitPopReg(kEDX);
    
            // pop eax
            psl->X86EmitPopReg(kEAX);
        }
    }

    // must restore esp explicitly since we don't know whether the target
    // popped the args.
    psl->EmitCheckGSCookie(kESI, 0 - sizeOfNegSpace);
    // lea esp, [ebp-distFromRawNegSpaceToEbp]
    UINT32 distFromRawNegSpaceToEbp = offsetof(FramedMethodFrame::NegInfo, m_vc5Frame) + 
                                      offsetof(VC5Frame, m_savedebp);
    psl->X86EmitIndexLea((X86Reg)kESP_Unsafe, kEBP, 0 - distFromRawNegSpaceToEbp);

    //-----------------------------------------------------------------------
    // Epilog
    //-----------------------------------------------------------------------
    psl->EmitMethodStubEpilog(numStackBytes, kNoTripStubStyle);

    //-----------------------------------------------------------------------
    // The out-of-line portion of enabling preemptive GC - rarely executed
    //-----------------------------------------------------------------------
    psl->EmitLabel(pRareEnable);
    psl->EmitRareEnable(pEnableRejoin);

    //-----------------------------------------------------------------------
    // The out-of-line portion of disabling preemptive GC - rarely executed
    //-----------------------------------------------------------------------
    psl->EmitLabel(pRareDisable);
    psl->EmitRareDisable(pDisableRejoin, /*bIsCallIn=*/FALSE);


    RETURN TRUE;
}



extern "C" VOID __stdcall StubRareEnableWorker(Thread *pThread)
{
    WRAPPER_CONTRACT;

    //printf("RareEnable\n");
    pThread->RareEnablePreemptiveGC();
}


// Disable when calling into managed code from a place that fails via HRESULT
extern "C" HRESULT __stdcall StubRareDisableHRWorker(Thread *pThread, Frame *pFrame)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;

    HRESULT hr = S_OK;
    

    BEGIN_CONTRACT_VIOLATION(SOToleranceViolation);


    // WARNING!!!!
    // when we start executing here, we are actually in cooperative mode.  But we
    // haven't synchronized with the barrier to reentry yet.  So we are in a highly
    // dangerous mode.  If we call managed code, we will potentially be active in
    // the GC heap, even as GC's are occuring!

    // Check for ShutDown scenario.  This happens only when we have initiated shutdown 
    // and someone is trying to call in after the CLR is suspended.  In that case, we
    // must either raise an unmanaged exception or return an HRESULT, depending on the
    // expectations of our caller.
    if (!CanRunManagedCode())
    {
        hr = E_PROCESS_SHUTDOWN_REENTRY;
    }
    else
    {
    // We must do the following in this order, because otherwise we would be constructing
    // the exception for the abort without synchronizing with the GC.  Also, we have no
    // CLR SEH set up, despite the fact that we may throw a ThreadAbortException.
    pThread->RareDisablePreemptiveGC();
    EX_TRY
    {
        pThread->HandleThreadAbort();
    }
    EX_CATCH
    {
        hr = GET_EXCEPTION()->GetHR();
    }
    EX_END_CATCH(SwallowAllExceptions);

#ifdef DEBUGGING_SUPPORTED

    // If the debugger is attached, we use this opprotunity to see if
    // we're disabling preemptive GC on the way into the runtime from
    // unmanaged code. We end up here because
    // Increment/DecrementTraceCallCount() will bump
    // g_TrapReturningThreads for us.
    if (CORDebuggerTraceCall())
        g_pDebugInterface->PossibleTraceCall(NULL, pFrame);
#endif // DEBUGGING_SUPPORTED
    }

    // should always be in coop mode here
    _ASSERTE(pThread->PreemptiveGCDisabled());

    END_CONTRACT_VIOLATION;
    
    return hr;
}


// Disable when calling into managed code from a place that fails via Exceptions
extern "C" VOID __stdcall StubRareDisableTHROWWorker(Thread *pThread, Frame *pFrame)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    // Do not add a CONTRACT here.  We haven't set up SEH.  We rely
    // on HandleThreadAbort and COMPlusThrowBoot dealing with this situation properly.

    // WARNING!!!!
    // when we start executing here, we are actually in cooperative mode.  But we
    // haven't synchronized with the barrier to reentry yet.  So we are in a highly
    // dangerous mode.  If we call managed code, we will potentially be active in
    // the GC heap, even as GC's are occuring!

    // Check for ShutDown scenario.  This happens only when we have initiated shutdown 
    // and someone is trying to call in after the CLR is suspended.  In that case, we
    // must either raise an unmanaged exception or return an HRESULT, depending on the
    // expectations of our caller.
    if (!CanRunManagedCode())
    {
        // DO NOT IMPROVE THIS EXCEPTION!  It cannot be a managed exception.  It
        // cannot be a real exception object because we cannot execute any managed
        // code here.
        pThread->m_fPreemptiveGCDisabled = 0;
        COMPlusThrowBoot(E_PROCESS_SHUTDOWN_REENTRY);
    }

    // We must do the following in this order, because otherwise we would be constructing
    // the exception for the abort without synchronizing with the GC.  Also, we have no
    // CLR SEH set up, despite the fact that we may throw a ThreadAbortException.
    pThread->RareDisablePreemptiveGC();

    // Handling thread abort here is bad if we are returning a SafeHandle or CriticalHandle (because at the point this is called we
    // haven't yet marshalled the native handle into the SafeHandle and will consequently leak it if we inject an exception). We
    // could check for thread abort separately at the end of the marshalling but that would impact a highly optimized code path that
    // is very sensitive to even a couple of extra instructions. Instead we'll look for a bit in the frame that indicates that the
    // check should be skipped. We'll set this just for the SafeHandle/CriticalHandle case and it doesn't impact the semantic too
    // much even there (the abort will eventually happen, just not this precise instant, the aborting thread will have to poll).
    // Note we only have to do this for the standalone stub case, the slower paths simply hold the prevent abort count around the
    // entire unmarshaling process.
    TADDR frameType = pFrame->GetVTablePtr();
    BOOL fHandleThreadAbort = TRUE;
    if (frameType == NDirectMethodFrameStandalone::GetMethodFrameVPtr())
        fHandleThreadAbort = (((NDirectMethodFrame*)pFrame)->GetNegInfo()->m_flags & FLAG_SKIP_HANDLE_THREAD_ABORT) == 0;
    else if (frameType == NDirectMethodFrameStandaloneCleanup::GetMethodFrameVPtr())
        fHandleThreadAbort = (((NDirectMethodFrame*)pFrame)->GetNegInfo()->m_flags & FLAG_SKIP_HANDLE_THREAD_ABORT) == 0;

    if (fHandleThreadAbort)
        pThread->HandleThreadAbort();

#ifdef DEBUGGING_SUPPORTED

    // If the debugger is attached, we use this opprotunity to see if
    // we're disabling preemptive GC on the way into the runtime from
    // unmanaged code. We end up here because
    // Increment/DecrementTraceCallCount() will bump
    // g_TrapReturningThreads for us.
    if (CORDebuggerTraceCall())
        g_pDebugInterface->PossibleTraceCall(NULL, pFrame);
#endif // DEBUGGING_SUPPORTED

}

// Disable when calling from a place that is returning to managed code, not calling
// into it.
extern "C" VOID __stdcall StubRareDisableRETURNWorker(Thread *pThread, Frame *pFrame)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_SO_TOLERANT;

    // Do not add a CONTRACT here.  We haven't set up SEH.  We rely
    // on HandleThreadAbort dealing with this situation properly.

    // WARNING!!!!
    // when we start executing here, we are actually in cooperative mode.  But we
    // haven't synchronized with the barrier to reentry yet.  So we are in a highly
    // dangerous mode.  If we call managed code, we will potentially be active in
    // the GC heap, even as GC's are occuring!

    // Don't check for ShutDown scenario.  We are returning to managed code, not
    // calling into it.  The best we can do during shutdown is to deadlock and allow
    // the WatchDogThread to terminate the process on timeout.

    // We must do the following in this order, because otherwise we would be constructing
    // the exception for the abort without synchronizing with the GC.  Also, we have no
    // CLR SEH set up, despite the fact that we may throw a ThreadAbortException.
    pThread->RareDisablePreemptiveGC();
    pThread->HandleThreadAbort();

#ifdef DEBUGGING_SUPPORTED

    // If the debugger is attached, we use this opprotunity to see if
    // we're disabling preemptive GC on the way into the runtime from
    // unmanaged code. We end up here because
    // Increment/DecrementTraceCallCount() will bump
    // g_TrapReturningThreads for us.
    if (CORDebuggerTraceCall())
        g_pDebugInterface->PossibleTraceCall(NULL, pFrame);
#endif // DEBUGGING_SUPPORTED
}

// Disable from a place that is calling into managed code via a UMEntryThunk.
extern "C" VOID __stdcall UMThunkStubRareDisableWorker(Thread *pThread, UMEntryThunk *pUMEntryThunk, Frame *pFrame)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    // Do not add a CONTRACT here.  We haven't set up SEH.  We rely
    // on HandleThreadAbort dealing with this situation properly.

    // WARNING!!!!
    // when we start executing here, we are actually in cooperative mode.  But we
    // haven't synchronized with the barrier to reentry yet.  So we are in a highly
    // dangerous mode.  If we call managed code, we will potentially be active in
    // the GC heap, even as GC's are occuring!

    // Check for ShutDown scenario.  This happens only when we have initiated shutdown 
    // and someone is trying to call in after the CLR is suspended.  In that case, we
    // must either raise an unmanaged exception or return an HRESULT, depending on the
    // expectations of our caller.
    if (!CanRunManagedCode())
    {
        // DO NOT IMPROVE THIS EXCEPTION!  It cannot be a managed exception.  It
        // cannot be a real exception object because we cannot execute any managed
        // code here.
        pThread->m_fPreemptiveGCDisabled = 0;
        COMPlusThrowBoot(E_PROCESS_SHUTDOWN_REENTRY);
    }

    // We must do the following in this order, because otherwise we would be constructing
    // the exception for the abort without synchronizing with the GC.  Also, we have no
    // CLR SEH set up, despite the fact that we may throw a ThreadAbortException.
    pThread->RareDisablePreemptiveGC();
    pThread->HandleThreadAbort();

    #ifdef DEBUGGING_SUPPORTED
    // If the debugger is attached, we use this opprotunity to see if
    // we're disabling preemptive GC on the way into the runtime from
    // unmanaged code. We end up here because
    // Increment/DecrementTraceCallCount() will bump
    // g_TrapReturningThreads for us.
    if (CORDebuggerTraceCall())
        g_pDebugInterface->PossibleTraceCall(pUMEntryThunk, pFrame);
    #endif // DEBUGGING_SUPPORTED
}



//---------------------------------------------------------
// Performs a slim N/Direct call. This form can handle most
// common cases and is faster than the full generic version.
//---------------------------------------------------------

#define NDIRECT_SLIM_CBDSTMAX 32

struct NDirectSlimLocals
{
    Thread               *pThread;
    NDirectMethodFrameEx *pFrame;
    UINT32                savededi;

    NDirectMethodDesc    *pMD;
    const MLCode         *pMLCode;
    CleanupWorkList      *pCleanup;
    BYTE                 *pLocals;

    INT64                 nativeRetVal;
};

LPVOID __stdcall NDirectSlimStubWorker1(NDirectSlimLocals *pNSL)
{
    CONTRACT(LPVOID)
    {
        THROWS;
        GC_TRIGGERS;
        ENTRY_POINT;
        MODE_COOPERATIVE;
    }
    CONTRACT_END;

    LPVOID pNativeTarget = NULL;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER; // called by managed code by way of a stub

    pNSL->pMD                 = (NDirectMethodDesc*)(pNSL->pFrame->GetFunction());
    MLHeader *pheader         = pNSL->pMD->GetMLHeader();
    UINT32 cbLocals           = pheader->m_cbLocals;
    BYTE *pdst                = ((BYTE*)pNSL) - NDIRECT_SLIM_CBDSTMAX - cbLocals;
    pNSL->pLocals             = pdst + NDIRECT_SLIM_CBDSTMAX;
    VOID *psrc                = (VOID*)(pNSL->pFrame);
    pNSL->pCleanup            = pNSL->pFrame->GetCleanupWorkList();

    LOG((LF_STUBS, LL_INFO1000, "Calling NDirectSlimStubWorker1 %s::%s \n", pNSL->pMD->m_pszDebugClassName, pNSL->pMD->m_pszDebugMethodName));

    if (pNSL->pCleanup)
    {
        // Checkpoint the current thread's fast allocator (used for temporary
        // buffers over the call) and schedule a collapse back to the checkpoint in
        // the cleanup list. Note that if we need the allocator, it is
        // guaranteed that a cleanup list has been allocated.
        void* cp = pNSL->pThread->m_MarshalAlloc.GetCheckpoint();
        pNSL->pCleanup->ScheduleFastFree(cp);
        pNSL->pCleanup->IsVisibleToGc();
    }

#ifdef _DEBUG
    FillMemory(pdst, NDIRECT_SLIM_CBDSTMAX+cbLocals, 0xcc);
#endif

    pNSL->pMLCode = RunML(pheader->GetMLCode(),
                          psrc,
                          pdst + pheader->m_cbDstBuffer,
                          (UINT8*const)(pNSL->pLocals),
                          pNSL->pCleanup);


    pNSL->pThread->EnablePreemptiveGC();

    if (pNSL->pMD->NeedHostHook()) {
        Thread::LeaveRuntime((size_t)pNSL->pMD->GetNativeNDirectTarget());
        pNSL->pFrame->MarkEnterRuntimeRequired();
    }

#ifdef PROFILING_SUPPORTED

    // Notify the profiler of transitions out of the runtime
    if (CORProfilerTrackTransitions())
    {
        PROFILER_CALL;

    
        g_profControlBlock.pProfInterface->
            ManagedToUnmanagedTransition((CodeID) pNSL->pMD,
                                               COR_PRF_TRANSITION_CALL);
    }
#endif // PROFILING_SUPPORTED

    pNativeTarget = pNSL->pMD->GetNativeNDirectTarget();

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

    RETURN pNativeTarget;
}


INT64 __stdcall NDirectSlimStubWorker2(const NDirectSlimLocals *pNSL)
{

    CONTRACT(INT64)
    {
        THROWS;
        GC_TRIGGERS;
        ENTRY_POINT;
        MODE_PREEMPTIVE;
    }
    CONTRACT_END;

    INT64 returnValue = 0;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER;  // called by managed code by way of a stub

    {
        // Ensure we don't respond to a thread abort request until after we
        // have marshaled our return result(s). In particular native handles
        // need to be back propagated into safe handles before it's safe to
        // take a thread abort.
        ThreadPreventAsyncHolder preventAsync(TRUE);

        LOG((LF_STUBS, LL_INFO1000, "Calling NDirectSlimStubWorker2 %s::%s \n", pNSL->pMD->m_pszDebugClassName, pNSL->pMD->m_pszDebugMethodName));

#ifdef PROFILING_SUPPORTED

        // Notify the profiler of transitions out of the runtime
        if (CORProfilerTrackTransitions())
        {
            PROFILER_CALL;
            g_profControlBlock.pProfInterface->
                UnmanagedToManagedTransition((CodeID) pNSL->pMD,
                                             COR_PRF_TRANSITION_RETURN);
        }
#endif // PROFILING_SUPPORTED

        if (pNSL->pFrame->EnterRuntimeRequired()) {
            Thread::EnterRuntime();
            pNSL->pFrame->UnMarkEnterRuntimeRequired();
        }

        pNSL->pThread->DisablePreemptiveGC();


        RunML(pNSL->pMLCode,
              &(pNSL->nativeRetVal),
              ((BYTE*)&returnValue) + 4, // We don't slimstub 64-bit returns
              (UINT8*const)(pNSL->pLocals),
              pNSL->pFrame->GetCleanupWorkList());
    }

    pNSL->pThread->HandleThreadAbort();

    if (pNSL->pCleanup)
    {
        pNSL->pCleanup->Cleanup(FALSE);
    }

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;  
    RETURN returnValue;
}


//---------------------------------------------------------
// Creates the slim NDirect stub.
//---------------------------------------------------------
/* static */
Stub* NDirect::CreateSlimNDirectStub(StubLinker *pstublinker, NDirectMethodDesc *pMD, UINT numStackBytes)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;

        PRECONDITION(!pMD->IsVarArg());
    }
    CONTRACT_END;

    BOOL fSaveLastError = FALSE;

    // Putting this in a local block to prevent the code below from seeing
    // the header. Since we sharing stubs based on the return value, we can't
    // customize based on the header.
    {
        {
            MLHeader *pheader    = pMD->GetMLHeader();

            if ( !(((pheader->m_Flags & MLHF_MANAGEDRETVAL_TYPECAT_MASK) 
                    != MLHF_TYPECAT_GCREF) &&
                   0 == (pheader->m_Flags & ~(MLHF_SETLASTERROR)) &&
                   pheader->m_cbDstBuffer <= NDIRECT_SLIM_CBDSTMAX &&
                   pheader->m_cbLocals + pheader->m_cbDstBuffer   <= 0x1000-100) )
            {
                RETURN NULL;
            }

            if (pheader->m_Flags & MLHF_SETLASTERROR)
            {
                fSaveLastError = TRUE;
            }
        }
    }

    //printf("Generating slim.\n");


    UINT key           = numStackBytes << 1;
    if (fSaveLastError)
    {
        key |= 1;
    }
#ifdef LOP_FRIENDLY_FRAMES
    DWORD useLOPFriendlyFrames = g_LOPFriendlyFrames.val(L"LOPFriendlyFrames", 0);
    if (useLOPFriendlyFrames)
    {
        // In this case, the stub also depends on these values, so they need to be part of the key.
        // We will get problems if there are more than one 1024 bytes of arguments or more than 4096 bytes of
        // locals.
        key |= (pMD->GetMLHeader()->m_cbDstBuffer << 10) + (pMD->GetMLHeader()->m_cbLocals << 20);
    }
#endif //LOP_FRIENDLY_FRAMES
    Stub *pStub = m_pNDirectSlimStubCache->GetStub(key);
    if (pStub)
    {
        RETURN pStub;
    }
    else
    {
        CPUSTUBLINKER *psl = (CPUSTUBLINKER*)pstublinker;

        // EAX contains MethodDesc* from the precode. Push it here
        // to initialize m_Datum field of the frame.
        psl->X86EmitPushReg(kEAX);

        psl->EmitMethodStubProlog(NDirectMethodFrameSlim::GetMethodFrameVPtr());
        
        // See NOTE on Frame Size C_ASSERT usage in this file 
        C_ASSERT(sizeof(NDirectMethodFrame::NegInfo)  - sizeof(FramedMethodFrame::NegInfo) == 4);

        // pushes a marker for LeaveRuntime call
        psl->X86EmitPushImm32(FLAG_ENTER_RUNTIME_NOT_REQUIRED);

        // See NOTE on Frame Size C_ASSERT usage in this file 
        C_ASSERT(sizeof(NDirectMethodFrameEx::NegInfo)  - sizeof(NDirectMethodFrame::NegInfo) == 12);

        // push room for the checkpoint object

        psl->X86EmitPushImm32(0);
        psl->X86EmitPushImm32(0);

        // pushes a CleanupWorkList.
        psl->X86EmitPushImm32(0);

        psl->X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());
        
        // Frame is ready to be inspected by debugger for patch location
        psl->EmitPatchLabel();

        // Reserve space for NDirectSlimLocals (note this actually reserves
        // more space than necessary.)
        psl->X86EmitSubEsp(sizeof(NDirectSlimLocals));

        // Allocate & initialize leading NDirectSlimLocals fields
        psl->X86EmitPushReg(kEDI); _ASSERTE(8==offsetof(NDirectSlimLocals, savededi));
        psl->X86EmitPushReg(kESI); _ASSERTE(4==offsetof(NDirectSlimLocals, pFrame));
        psl->X86EmitPushReg(kEBX); _ASSERTE(0==offsetof(NDirectSlimLocals, pThread));

        // Save pointer to NDirectSlimLocals in edi.
        // mov edi,esp
        psl->X86EmitMovRegSP(kEDI);

        // Save space for destination & ML local buffer.
        //  mov edx, [CURFRAME.MethodDesc]
        psl->X86EmitIndexRegLoad(kEDX, kESI, FramedMethodFrame::GetOffsetOfMethod());

        //  mov ecx, [edx + NDirectMethodDesc.ndirect.m_pWriteableData]
        psl->X86EmitIndexRegLoad(kECX, kEDX, offsetof(NDirectMethodDesc, ndirect.m_pWriteableData));
        //  mov ecx, [ecx + NDirectWriteableData.m_pMLHeader]
        psl->X86EmitIndexRegLoad(kECX, kECX, offsetof(NDirectWriteableData, m_pMLHeader));

        _ASSERTE(2 == sizeof(((MLHeader*)0)->m_cbLocals));
        //  movzx eax, word ptr [ecx + Stub.m_cbLocals]
        psl->Emit8(0x0f);
        psl->X86EmitOffsetModRM(0xb7, kEAX, kECX, offsetof(MLHeader,m_cbLocals));

        //  add eax, NDIRECT_SLIM_CBDSTMAX
        psl->Emit8(0x05);
        psl->Emit32(NDIRECT_SLIM_CBDSTMAX);

        psl->Push(NDIRECT_SLIM_CBDSTMAX);

        //  sub esp, eax
        psl->Emit16(0xe02b);

        // Invoke the first worker, passing it the address of NDirectSlimLocals.
        // This will marshal the parameters into the dst buffer and enable gc.
        psl->X86EmitPushReg(kEDI);
        psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) NDirectSlimStubWorker1), 4);

#ifdef LOP_FRIENDLY_FRAMES
        if (useLOPFriendlyFrames)
        {
            // Emit a fake return for LOP

            // The story is that if LOP catches the program in NDirectSlimStubWorker1 or one of
            // its callees, it won't know how the routine called indirectly affects the stack, so
            // it can't find our callee.

            // This fake return will let LOP figure out where the return address is on the stack.

            // cmp eax, eax
            psl->Emit16(0xc03b);
            CodeLabel* plabel = psl->NewCodeLabel();
            // jnz label
            psl->X86EmitCondJump(plabel, X86CondCode::kJZ);

            // add esp, xxx
            // use X86EmitAddReg instead of X86EmitAddEsp, because we don't want the bookkeeping for esp
            // to be updated - this code is never going to be executed, after all.
            psl->X86EmitAddReg((X86Reg)4 /*kESP*/, NDirectMethodFrameSlim::GetNegSpaceSize() +
                                                   sizeof(NDirectSlimLocals) + 
                                                   12 + NDIRECT_SLIM_CBDSTMAX + 0x18 + 12 + 
                                                   pMD->GetMLHeader()->m_cbLocals);
            // ret xxx
            psl->Emit8(0xc2);
            psl->Emit16(numStackBytes);

            //label:
            psl->EmitLabel(plabel);
        }
#endif //LOP_FRIENDLY_FRAMES

#if _DEBUG
        // Invoke the DLL target.
        //  mov eax, [CURFRAME.MethodDesc]
        psl->X86EmitIndexRegLoad(kEAX, kESI, FramedMethodFrame::GetOffsetOfMethod());

        // Call through debugger logic to make sure it works
        psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) CheckExitFrameDebuggerCalls), 0, TRUE);
#else

        // call eax
        psl->Emit16(0xd0ff);
        psl->EmitReturnLabel();
#endif

        // Emit our call site return label

        if (fSaveLastError)
        {
            psl->EmitSaveLastError();
        }

        // Save away the raw return value
        psl->X86EmitIndexRegStore(kEDI, offsetof(NDirectSlimLocals, nativeRetVal), kEAX);
        psl->X86EmitIndexRegStore(kEDI, offsetof(NDirectSlimLocals, nativeRetVal) + 4, kEDX);

        // Invoke the second worker, passing it the address of NDirectSlimLocals.
        // This will marshal the return value into eax, and redisable gc.
        psl->X86EmitPushReg(kEDI);
        psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) NDirectSlimStubWorker2), 4);

        // DO NOT TRASH EAX FROM HERE OUT.

        // Restore edi.
        // mov edi, [edi + savededi]
        psl->X86EmitIndexRegLoad(kEDI, kEDI, offsetof(NDirectSlimLocals, savededi));

        // must restore esp explicitly since we don't know whether the target
        // popped the args.
        psl->EmitCheckGSCookie(kESI, NDirectMethodFrameSlim::GetOffsetOfGSCookie());
        // lea esp, [ebp-distFromRawNegSpaceToEbp]
        UINT32 distFromRawNegSpaceToEbp = offsetof(FramedMethodFrame::NegInfo, m_vc5Frame) + 
                                          offsetof(VC5Frame, m_savedebp);
        psl->X86EmitIndexLea((X86Reg)kESP_Unsafe, kEBP, 0 - distFromRawNegSpaceToEbp);

        // Tear down frame and exit.
        psl->EmitMethodStubEpilog(numStackBytes, kNoTripStubStyle);

        // The cache is per-process using a key based on number of stack bytes,
        // last error handling, etc.  These stubs are never unloaded.

        Stub *pCandidate = psl->Link(SystemDomain::System()->GetStubHeap());
        Stub *pWinner = m_pNDirectSlimStubCache->AttemptToSetStub(key,pCandidate);
        pCandidate->DecRef();
        if (!pWinner)
        {
            COMPlusThrowOM();
        }
        RETURN pWinner;
    }

}

VOID __cdecl PopSEHRecords(LPVOID pTargetSP)
{
    // No CONTRACT here, because we can't run the risk of it pushing any SEH into the
    // current method.

    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    PPAL_EXCEPTION_REGISTRATION pReg = PAL_GetBottommostRegistration();

    while ((PVOID)pReg < pTargetSP)
    {
        pReg = pReg->Next;
    }

    PAL_SetBottommostRegistration(pReg);
}

//////////////////////////////////////////////////////////////////////////////
//
// JITInterface
//
//////////////////////////////////////////////////////////////////////////////

/*********************************************************************/



void UMEntryThunkCode::Encode(BYTE* pTargetCode, void* pvSecretParam)
{
    LEAF_CONTRACT;

#ifdef _DEBUG
    m_alignpad[0] = X86_INSTR_INT3;
    m_alignpad[1] = X86_INSTR_INT3;
#endif // _DEBUG
    m_movEAX     = X86_INSTR_MOV_EAX_IMM32;
    m_uet        = pvSecretParam;
    m_jmp        = X86_INSTR_JMP_REL32;
    m_execstub   = (BYTE*) ((pTargetCode) - (4+((BYTE*)&m_execstub)));

    FlushInstructionCache(GetCurrentProcess(),GetEntryPoint(),sizeof(UMEntryThunkCode));

    // Ensure that IsStub stays in sync w/ the encoding.
    _ASSERTE(IsStub(&m_movEAX, pTargetCode, pvSecretParam));
}

UMEntryThunk* UMEntryThunk::Decode(LPVOID pCallback)
{
    LEAF_CONTRACT;

    if (*((BYTE*)pCallback) != X86_INSTR_MOV_EAX_IMM32 ||
        ( ((size_t)pCallback) & 3) != 2) {
        return NULL;
    }
    return *(UMEntryThunk**)( 1 + (BYTE*)pCallback );
}

#endif // #ifndef DACCESS_COMPILE


typedef DPTR(UMEntryThunkCode) PTR_UMEntryThunkCode;

// Called by IJWNOADThunkStubManager::CheckIsStub to decide
// if an address is the beginning of the executable portion of an IJWNOADThunk stub.
// Return true if it is, else false.
// pAddress, pTargetCode, and pvSecretParam are all target addresses.
bool UMEntryThunkCode::IsStub(const BYTE* pAddress, BYTE* pTargetCode, void* pvSecretParam)
{
    LEAF_CONTRACT;

    // A null address is not one of our stubs.
    if (pAddress == NULL)
        return false;

    // This should only be called from a debugger.
    // If we return true, that tells the debugger that pAddress is a stub owned
    // by the clr. If we return false, that implies this is native code.
    // We should err on the side of returning false.

    // Note, we have to be careful. We don't know that this is a stub yet,
    // and so we don't want to access memory off the end of a page and AV.
    // To be safe, start at the front and keep working through the stub.
    // A stub won't end until we hit some sort of branch instruction (call/jmp),
    // so if we only look 1 instruction ahead at a time, we'll be safe.
        
    // Implement by inspecting the contents
    // Get a pointer to the start of this struct
    PTR_UMEntryThunkCode StubPtr =
        PTR_UMEntryThunkCode((TADDR)pAddress - GetEntryPointOffset());
    
#ifdef DACCESS_COMPILE
    // IsStub can be called on completely random addresses, so
    // protect this code against garbage.
    if (!StubPtr.IsValid())
    {
        return false;
    }
#endif
    
    UMEntryThunkCode * pStub = StubPtr;

    // The input address comes from a public API and so it could be anything.
    // If somebody set the address to GetEntryPointOffset(), then pStub 
    // would actually be null.
    if (pStub == NULL)
    {
        return false;
    }
    
    // What if an unmanaged debugger placed a breakpoint on this stub?
    // Then we'll have some int3 in our instruction stream on the start of an
    // instruction. There's not really anything we can do about that.
    
    // Check by comparing contents.
    if (pStub->m_movEAX != X86_INSTR_MOV_EAX_IMM32)
        return false;
    
    if (pStub->m_uet != pvSecretParam)
        return false;

    if (pStub->m_jmp != X86_INSTR_JMP_REL32)
        return false;
        
    const BYTE* p  = (BYTE*) ((pTargetCode) - (4+((BYTE*)PTR_HOST_MEMBER_TADDR(UMEntryThunkCode, pStub, m_execstub))));        
    if (pStub->m_execstub != p)
        return false;

    return true;

}

#ifndef DACCESS_COMPILE
BOOL DoesSlotCallPrestub(const BYTE *pCode)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pCode));
        PRECONDITION(pCode != (BYTE *) ThePreStub()->GetEntryPoint());
    } CONTRACTL_END;

    TADDR pTarget = (TADDR)pCode;

    // x86 has the following possible sequences for prepad logic:
    // 1. slot -> callRel32 (prepad) -> prestub
    // 2. slot -> callRel32 (prepad) -> jumpRel32 (NGEN case) -> prestub

#ifdef HAS_COMPACT_ENTRYPOINTS
    if (MethodDescChunk::GetMethodDescFromCompactEntryPoint(pTarget, TRUE) != NULL)
    {
        return TRUE;
    }
#endif // HAS_COMPACT_ENTRYPOINTS

    if (!IS_ALIGNED(pTarget, PRECODE_ALIGNMENT))
    {
        return FALSE;
    }

#ifdef HAS_FIXUP_PRECODE
    if (*PTR_BYTE(pTarget) == X86_INSTR_CALL_REL32)
    {
        // Note that call could have been patched to jmp in the meantime
        pTarget = rel32Decode(pTarget+1);

        // NGEN case
        if (*PTR_BYTE(pTarget) == X86_INSTR_JMP_REL32) {
            pTarget = rel32Decode(pTarget+1);
        }

        return pTarget == (TADDR)PrecodeFixupThunk;
    }
#endif

    if (*PTR_BYTE(pTarget) != X86_INSTR_MOV_EAX_IMM32 ||
        *PTR_BYTE(pTarget+5) != X86_INSTR_MOV_RM_R ||
        *PTR_BYTE(pTarget+7) != X86_INSTR_JMP_REL32)
    {
        return FALSE;
    }
    pTarget = rel32Decode(pTarget+8);

    // NGEN case
    if (*PTR_BYTE(pTarget) == X86_INSTR_JMP_REL32) {
        pTarget = rel32Decode(pTarget+1);
    }

    return pTarget == (TADDR) ThePreStub()->GetEntryPoint();
}

// Create a stub that jumps to the given stub simulating being called through
// the prepad (in that the dispatch method desc pointer is passed as an
// additional argument to the stub). Currently only used for a remoting edge
// case where an fcall stub that has caught a transparent proxy needs to
// redispatch to the TP stub for correct processing.
Stub *CreatePrepadSimulationThunk(CPUSTUBLINKER *psl, MethodDesc *pMD, const BYTE *pTarget)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    psl->X86EmitPushImmPtr(pMD);
    psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID)pTarget), 0);
    psl->X86EmitReturn(0);

    return psl->Link(pMD->GetDomain()->GetStubHeap());
}

#endif // !DACCESS_COMPILE

//------------------------------------------------------------------------
// This is declared as returning WORD instead of PRD_TYPE because of
// header issues with cgencpu.h including dbginterface.h.
WORD GetUnpatchedCodeData(LPCBYTE pAddr)
{
#ifndef _X86_
#error Make sure this works before porting to platforms other than x86.
#endif
    // PRD_TYPE is currently typedef'ed to be a DWORD_PTR. If this changes
    // (it could become a BYTE) then we'll need to reimplement this method
    // so that it can cope with this change.
    C_ASSERT(sizeof(WORD) <= sizeof(PRD_TYPE));
    CONTRACT(WORD) {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CORDebuggerAttached());
        PRECONDITION(CheckPointer(pAddr));
        SO_TOLERANT;
    } CONTRACT_END;

    PRD_TYPE ret = *((PRD_TYPE *)pAddr);

#ifndef DACCESS_COMPILE
    // Need to make sure that the code we're reading is free of breakpoint patches.
    PRD_TYPE unpatchedOpcode;
    if (g_pDebugInterface->CheckGetPatchedOpcode((CORDB_ADDRESS_TYPE *)pAddr,
                                                 &unpatchedOpcode))
    {
        ret = unpatchedOpcode;
    }
#endif

    RETURN (WORD)ret;
}

