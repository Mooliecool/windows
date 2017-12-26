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
/*===========================================================================
**
** File:    remotingx86.cpp
**       
** Purpose: Defines various remoting related functions for the x86 architecture
**
** Date:    Oct 12, 1999
**
=============================================================================*/

#include "common.h"
#include "excep.h"
#include "comstring.h"
#include "comdelegate.h"
#include "remoting.h"
#include "field.h"
#include "siginfo.hpp"
#include "stackbuildersink.h"
#include "threads.h"
#include "method.hpp"
#include "asmconstants.h"
#include "interoputil.h"
#include "virtualcallstub.h"


// External variables
extern DWORD g_dwNonVirtualThunkRemotingLabelOffset;
extern DWORD g_dwNonVirtualThunkReCheckLabelOffset;

extern "C" size_t g_dwTPStubAddr;
extern "C" size_t g_dwOOContextAddr;

size_t g_dwTPStubAddr;
size_t g_dwOOContextAddr;

extern "C" void __stdcall CRemotingServices__DispatchInterfaceCall(MethodDesc *pMD);

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::GenerateCheckForProxy   public
//
//  Synopsis:   This code generates a check to see if the "this" pointer
//              is a proxy. If so, the interface invoke is handled via
//              the CRemotingServices::DispatchInterfaceCall else we
//              delegate to the old path
//+----------------------------------------------------------------------------
void CRemotingServices::GenerateCheckForProxy(CPUSTUBLINKER* psl)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(psl));
    }
    CONTRACTL_END;

    // EAX contains MethodDesc* from the precode. Push it here
    // to initialize m_Datum field of the frame.
    psl->X86EmitPushReg(kEAX);

    // call CRemotingServices::DispatchInterfaceCall
    // NOTE: We pop 0 bytes of stack even though the size of the arguments is
    // 4 bytes because the MethodDesc argument gets pushed for "free" via the
    // call instruction placed just before the start of the MethodDesc.
    // See the class MethodDesc for more details.
    psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) CRemotingServices__DispatchInterfaceCall), 0);

    // never returns
}

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::CreateThunkForVirtualMethod   private
//
//  Synopsis:   Creates the thunk that pushes the supplied slot number and jumps
//              to TP Stub
//+----------------------------------------------------------------------------
void CTPMethodTable::CreateThunkForVirtualMethod(DWORD dwSlot, BYTE *startaddr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(s_pTPStub));
        PRECONDITION(CheckPointer(startaddr));
    }
    CONTRACTL_END;

    BYTE *pCode = startaddr;

    // 0000   68 67 45 23 01     PUSH dwSlot
    // 0005   E9 ?? ?? ?? ??     JMP  s_pTPStub+1
    *pCode++ = 0x68;
    *((DWORD *) pCode) = dwSlot;
    pCode += sizeof(DWORD);
    *pCode++ = 0xE9;
    // self-relative call, based on the start of the next instruction.
    *((LONG *) pCode) = (LONG)(((size_t) s_pTPStub->GetEntryPoint()) - (size_t) (pCode + sizeof(LONG)));

    _ASSERTE(CVirtualThunkMgr::IsThunkByASM(startaddr));
}



#ifdef HAS_REMOTING_PRECODE

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::ActivatePrecodeRemotingThunk    private
//
//  Synopsis:   Patch the precode remoting thunk to begin interception
//
//+----------------------------------------------------------------------------
void CTPMethodTable::ActivatePrecodeRemotingThunk()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // 0000 85 C9               test     ecx,ecx
    // 0002 74 XX               je       RemotingDone
    // 0004 81 39 XX XX XX XX   cmp      dword ptr [ecx],11111111h
    // 000A 74 XX               je       RemotingCheck

    BYTE* pCode = (BYTE*)PrecodeRemotingThunk;
    SIZE_T size = 0x000A;

    DWORD oldProtect;
    if (!ClrVirtualProtect(pCode, size, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        _ASSERTE(!"ClrVirtualProtect of code page failed");
        COMPlusThrowWin32();
    }

    // Replace placeholder value with the actual address of TP method table
    _ASSERTE(*(PVOID*)(pCode+0x0006) == (PVOID*)0x11111111);
    *(PVOID*)(pCode+0x0006) = GetMethodTable();

    FlushInstructionCache(GetCurrentProcess(), pCode, size);
}

#else // HAS_REMOTING_PRECODE

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::CreateStubForNonVirtualMethod   public
//
//  Synopsis:   Create a stub for a non virtual method
//+----------------------------------------------------------------------------

Stub* CTPMethodTable::CreateStubForNonVirtualMethod(MethodDesc* pMD, CPUSTUBLINKER* psl,
                                            LPVOID pvAddrOfCode, Stub* pInnerStub)
{
    CONTRACT (Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pMD));
        PRECONDITION(CheckPointer(psl));
        PRECONDITION(CheckPointer(pvAddrOfCode));
        PRECONDITION(CheckPointer(pInnerStub, NULL_OK));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // we need a hash table only for virtual methods
    _ASSERTE(!pMD->IsVirtual());

    // Ensure the TP MethodTable's fields have been initialized.
    EnsureFieldsInitialized();

    // The thunk has not been created yet. Go ahead and create it.
    LPVOID pvStub = (LPVOID)s_pTPStub->GetEntryPoint();

    // Generate label where a null reference exception will be thrown
    CodeLabel *pJmpAddrLabel = psl->NewCodeLabel();
    // Generate label where remoting code will execute
    CodeLabel *pRemotingLabel = psl->NewCodeLabel();

    // if this == NULL throw NullReferenceException
    // test ecx, ecx
    psl->X86EmitR2ROp(0x85, kECX, kECX);

    // je ExceptionLabel
    psl->X86EmitCondJump(pJmpAddrLabel, X86CondCode::kJE);

    // Emit a label here for the debugger. A breakpoint will
    // be set at the next instruction and the debugger will
    // call CNonVirtualThunkMgr::TraceManager when the
    // breakpoint is hit with the thread's context.
    CodeLabel *pRecheckLabel = psl->NewCodeLabel();
    psl->EmitLabel(pRecheckLabel);

    // If this.MethodTable != TPMethodTable then do RemotingCall

    // cmp [ecx], CTPMethodTable::s_pThunkTable
    psl->Emit16(0x3981);
    psl->Emit32((DWORD)(size_t)GetMethodTable());

    // jne pJmpAddrLabel
    // marshalbyref case
    psl->X86EmitCondJump(pJmpAddrLabel, X86CondCode::kJNE);

    // EAX contains MethodDesc* from the precode. Push it here
    // to initialize m_Datum field of the frame.
    psl->X86EmitPushReg(kEAX);

    // Transparent proxy case
    EmitCallToStub(psl, pRemotingLabel);

    // Throw away methoddesc
    psl->X86EmitPopReg(kEAX);

    // Exception handling and non-remoting share the
    // same codepath
    psl->EmitLabel(pJmpAddrLabel);

    // jump to the address
    psl->X86EmitNearJump(psl->NewExternalCodeLabel(pvAddrOfCode));

    psl->EmitLabel(pRemotingLabel);

    // the MethodDesc is already on top of the stack.  goto TPStub
    // jmp TPStub
    psl->X86EmitNearJump(psl->NewExternalCodeLabel(pvStub));

    // Link and produce the stub
    Stub *pStub = psl->LinkInterceptor(pMD->GetDomain()->GetStubHeap(),
                                    pInnerStub, pvAddrOfCode);

    pStub->SetRequiresMethodDescCallingConvention();

    RETURN pStub;
}

//+----------------------------------------------------------------------------
//
//  Synopsis:   Find an existing thunk or create a new one for the given
//              method descriptor. NOTE: This is used for the methods that do
//              not go through the vtable such as constructors, private and
//              final methods.
//+----------------------------------------------------------------------------
const BYTE* CTPMethodTable::CreateNonVirtualThunkForVirtualMethod(MethodDesc* pMD)
{
    CONTRACT (const BYTE*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pMD));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    CPUSTUBLINKER sl;
    CPUSTUBLINKER* psl = &sl;

    Stub *pStub = NULL;

    // The thunk has not been created yet. Go ahead and create it.
    // Compute the address of the slot
    LPVOID pvSlot = (LPVOID)pMD->GetAddrOfSlot();
    LPVOID pvStub = (LPVOID)s_pTPStub->GetEntryPoint();

    // Generate label where a null reference exception will be thrown
    CodeLabel *pExceptionLabel = psl->NewCodeLabel();

    //  !!! WARNING WARNING WARNING WARNING WARNING !!!
    //
    //  DO NOT CHANGE this code without changing the thunk recognition
    //  code in CNonVirtualThunkMgr::IsThunkByASM
    //  & CNonVirtualThunkMgr::GetMethodDescByASM
    //
    //  !!! WARNING WARNING WARNING WARNING WARNING !!!

    // if this == NULL throw NullReferenceException
    // test ecx, ecx
    psl->X86EmitR2ROp(0x85, kECX, kECX);

    // je ExceptionLabel
    psl->X86EmitCondJump(pExceptionLabel, X86CondCode::kJE);

    // Generate label where remoting code will execute
    CodeLabel *pRemotingLabel = psl->NewCodeLabel();

    // Emit a label here for the debugger. A breakpoint will
    // be set at the next instruction and the debugger will
    // call CNonVirtualThunkMgr::TraceManager when the
    // breakpoint is hit with the thread's context.
    CodeLabel *pRecheckLabel = psl->NewCodeLabel();
    psl->EmitLabel(pRecheckLabel);

    // If this.MethodTable == TPMethodTable then do RemotingCall

    // cmp [ecx], CTPMethodTable::s_pThunkTable
    psl->Emit16(0x3981);
    psl->Emit32((DWORD)(size_t)GetMethodTable());

    // je RemotingLabel
    psl->X86EmitCondJump(pRemotingLabel, X86CondCode::kJE);

    // Exception handling and non-remoting share the
    // same codepath
    psl->EmitLabel(pExceptionLabel);

    // Non-RemotingCode
    // Jump to the vtable slot of the method
    // jmp [slot]
    psl->Emit8(0xff);
    psl->Emit8(0x25);
    psl->Emit32((DWORD)(size_t)pvSlot);

    // Remoting code. Note: CNonVirtualThunkMgr::TraceManager
    // relies on this label being right after the jmp [slot]
    // instruction above. If you move this label, update
    // CNonVirtualThunkMgr::DoTraceStub.
    psl->EmitLabel(pRemotingLabel);

    // Save the MethodDesc and goto TPStub
    // push MethodDesc

    psl->X86EmitPushImm32((DWORD)(size_t)pMD);

    // jmp TPStub
    psl->X86EmitNearJump(psl->NewExternalCodeLabel(pvStub));

    // Link and produce the stub
    // FUTURE: Do we have to provide the loader heap ?
    pStub = psl->Link(SystemDomain::System()->GetHighFrequencyHeap());

    // Grab the offset of the RemotingLabel and RecheckLabel
    // for use in CNonVirtualThunkMgr::DoTraceStub and
    // TraceManager.
    g_dwNonVirtualThunkRemotingLabelOffset =
        psl->GetLabelOffset(pRemotingLabel);
    g_dwNonVirtualThunkReCheckLabelOffset =
        psl->GetLabelOffset(pRecheckLabel);

    RETURN (pStub->GetEntryPoint());
}

#endif // HAS_REMOTING_PRECODE


//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::CreateTPStub   private
//
//  Synopsis:   Creates the stub that sets up a CtxCrossingFrame and forwards the
//              call to
//+----------------------------------------------------------------------------

Stub *CTPMethodTable::CreateTPStub()
{
    CONTRACT (Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

    // Note: We are already inside a criticalsection

    if (s_pTPStub == NULL)
    {
        CPUSTUBLINKER sl;
        CPUSTUBLINKER* pStubLinker = &sl;

        CodeLabel *ConvMD = pStubLinker->NewCodeLabel();
        CodeLabel *UseCode = pStubLinker->NewCodeLabel();
        CodeLabel *OOContext = pStubLinker->NewCodeLabel();

        // before we setup a frame check if the method is being executed 
        // in the same context in which the server was created, if true,
        // we do not set up a frame and instead jump directly to the code address.
        EmitCallToStub(pStubLinker, OOContext);

        // The contexts match. Jump to the real address and start executing...
        EmitJumpToAddressCode(pStubLinker, ConvMD, UseCode);

        // label: OOContext
        pStubLinker->EmitLabel(OOContext);

        // CONTEXT MISMATCH CASE, call out to the real proxy to
        // dispatch

        // Setup the frame
        EmitSetupFrameCode(pStubLinker);

        // Finally, create the stub
        s_pTPStub = pStubLinker->Link();

        g_dwTPStubAddr = (size_t)s_pTPStub->GetEntryPoint();

        // Set the address of Out Of Context case.
        // This address is used by other stubs like interface
        // invoke to jump straight to RealProxy::PrivateInvoke
        // because they have already determined that contexts
        // don't match.
        g_dwOOContextAddr = (size_t)(s_pTPStub->GetEntryPoint() +
                                    pStubLinker->GetLabelOffset(OOContext));
    }

    // Initialize the stub manager which will aid the debugger in finding
    // the actual address of a call made through the vtable
    // Note: This function can throw, but we are guarded by a try..finally
    CVirtualThunkMgr::InitVirtualThunkManager((const BYTE *) s_pTPStub->GetEntryPoint());

    RETURN(s_pTPStub);
}

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::CreateDelegateStub   private
//
//  Synopsis:   Creates the stub that sets up a CtxCrossingFrame and forwards the
//              call to PreCall
//+----------------------------------------------------------------------------
Stub *CTPMethodTable::CreateDelegateStub()
{
    CONTRACT (Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;


    // Note: We are inside a critical section

    if (s_pDelegateStub == NULL)
    {
        CPUSTUBLINKER sl;
        CPUSTUBLINKER* pStubLinker = &sl;

        // EAX contains MethodDesc* from the precode. Push it here
        // to initialize m_Datum field of the frame.
        pStubLinker->X86EmitPushReg(kEAX);

        // Setup the frame
        EmitSetupFrameCode(pStubLinker);

        s_pDelegateStub = pStubLinker->Link();
    }

    RETURN(s_pDelegateStub);
}

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::EmitCallToStub   private
//
//  Synopsis:   Emits code to call a stub defined on the proxy.
//              The result of the call dictates whether the call should be executed in the callers
//              context or not.
//+----------------------------------------------------------------------------
VOID CTPMethodTable::EmitCallToStub(CPUSTUBLINKER* pStubLinker, CodeLabel* pCtxMismatch)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pStubLinker));
        PRECONDITION(CheckPointer(pCtxMismatch));
    }
    CONTRACTL_END;

    // Move into eax the stub data and call the stub
    // mov eax, [ecx + TP_OFFSET_STUBDATA]
    pStubLinker->X86EmitIndexRegLoad(kEAX, kECX, TP_OFFSET_STUBDATA);

    //call [ecx + TP_OFFSET_STUB]
    BYTE callStub[] = {0xff, 0x51, (BYTE)TP_OFFSET_STUB};
    pStubLinker->EmitBytes(callStub, sizeof(callStub));

    // test eax,eax
    pStubLinker->Emit16(0xc085);

    // jnz CtxMismatch
    pStubLinker->X86EmitCondJump(pCtxMismatch, X86CondCode::kJNZ);
}

// Forward declaration.
void InContextTPQuickDispatchAsmStub();

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::EmitJumpToAddressCode   private
//
//  Synopsis:   Emits the code to extract the address from the slot or the method
//              descriptor and jump to it.
//+----------------------------------------------------------------------------
VOID CTPMethodTable::EmitJumpToAddressCode(CPUSTUBLINKER* pStubLinker, CodeLabel* ConvMD,
                                           CodeLabel* UseCode)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pStubLinker));
        PRECONDITION(CheckPointer(ConvMD));
        PRECONDITION(CheckPointer(UseCode));
    }
    CONTRACTL_END;

    // UseCode:
    pStubLinker->EmitLabel(UseCode);

    // mov eax, [esp]
    BYTE loadSlotOrMD[] = {0x8B, 0x44, 0x24, 0x00};
    pStubLinker->EmitBytes(loadSlotOrMD, sizeof(loadSlotOrMD));

    // test eax, 0xffff0000
    BYTE testForSlot[] = { 0xA9, 0x00, 0x00, 0xFF, 0xFF };
    pStubLinker->EmitBytes(testForSlot, sizeof(testForSlot));

    // jnz ConvMD
    pStubLinker->X86EmitCondJump(ConvMD, X86CondCode::kJNZ);    

    // ** code addr from slot case **

    // Get the real MT
    // mov eax, [ecx + TPMethodTable::GetOffsetOfMT()]
    pStubLinker->X86EmitIndexRegLoad(kEAX, kECX, TP_OFFSET_MT);

    // Spill EBX
    // push ebx
    pStubLinker->X86EmitPushReg(kEBX);

    // Reload the slot
    // mov ebx, [esp + 4]
    BYTE loadSlot[] = {0x8B, 0x5C, 0x24, 0x04};
    pStubLinker->EmitBytes(loadSlot, sizeof(loadSlot));

    // Offset by the slot and dereference the vtable to get the target
    // mov eax,[eax + ebx*4 + MethodTable::GetVtableOffset()]
    BYTE getCodePtr[]  = {0x8B, 0x84, 0x98, 0x00, 0x00, 0x00, 0x00};
    *((DWORD *)(getCodePtr+3)) = MethodTable::GetVtableOffset();
    pStubLinker->EmitBytes(getCodePtr, sizeof(getCodePtr));

    // Restore EBX
    // pop ebx
    pStubLinker->X86EmitPopReg(kEBX);

    // Remove the slot number from the stack
    // lea esp, [esp+4]
    BYTE popNULL[] = { 0x8D, 0x64, 0x24, 0x04};
    pStubLinker->EmitBytes(popNULL, sizeof(popNULL));

    // jmp eax
    BYTE jumpToRegister[] = {0xff, 0xe0};
    pStubLinker->EmitBytes(jumpToRegister, sizeof(jumpToRegister));


    // ** code addr from MethodDesc case **
    pStubLinker->EmitLabel(ConvMD);


    pStubLinker->X86EmitPopReg(kEAX);

    pStubLinker->X86EmitNearJump(
        pStubLinker->NewExternalCodeLabel((LPVOID)ThePreStub()->GetEntryPoint()));
}

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::EmitSetupFrameCode   private
//
//  Synopsis:   Emits the code to setup a frame and call to PreCall method
//              call to PreCall
//+----------------------------------------------------------------------------
VOID CTPMethodTable::EmitSetupFrameCode(CPUSTUBLINKER *pStubLinker)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pStubLinker));
    }
    CONTRACTL_END;

    ////////////////START SETUP FRAME//////////////////////////////
    // Setup frame (partial)
    pStubLinker->EmitMethodStubProlog(TPMethodFrame::GetMethodFrameVPtr());

    pStubLinker->X86EmitSubEsp(sizeof(INT64));
    pStubLinker->X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());
    
    // Complete the setup of the frame by calling PreCall

    // push esi (push new frame as ARG)
    pStubLinker->X86EmitPushReg(kESI);
    // pop 4 bytes or args on return from call
    pStubLinker->X86EmitCall(pStubLinker->NewExternalCodeLabel((LPVOID) PreCall), 4);

    ////////////////END SETUP FRAME////////////////////////////////

    // Debugger patch location
    // NOTE: This MUST follow the call to emit the "PreCall" label
    // since after PreCall we know how to help the debugger in
    // finding the actual destination address of the call.
    // See CVirtualThunkMgr::DoTraceStub.
    pStubLinker->EmitPatchLabel();

    // OnCall

    // lea edx, [esp+4] ;; address of m_returnValue
    pStubLinker->X86EmitEspOffset(0x8d, kEDX, sizeof(GSCookie));
    pStubLinker->X86EmitPushReg(kEDX); // push edx
    pStubLinker->X86EmitPushReg(kEBX); // push ebx (push current thread as ARG)
    pStubLinker->X86EmitPushReg(kESI); // push esi (push new frame as ARG)
    // in CE pop 12 bytes or args on return from call
    pStubLinker->X86EmitCall(pStubLinker->NewExternalCodeLabel((LPVOID) OnCall), 12);

    pStubLinker->EmitCheckGSCookie(kESI, TPMethodFrame::GetOffsetOfGSCookie());
    // Tear down frame
    pStubLinker->X86EmitAddEsp(sizeof(INT64) + sizeof(GSCookie));
    pStubLinker->EmitMethodStubEpilog(CPUSTUBLINKER::ARG_SIZE_ON_STACK, kNoTripStubStyle);
}


//+----------------------------------------------------------------------------
//
//  Method:     CVirtualThunkMgr::DoTraceStub   public
//
//  Synopsis:   Traces the stub given the starting address
//+----------------------------------------------------------------------------
BOOL CVirtualThunkMgr::DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(stubStartAddress));
        PRECONDITION(CheckPointer(trace));
    }
    CONTRACTL_END;

    BOOL bIsStub = FALSE;

    // Find a thunk whose code address matching the starting address
    LPBYTE pThunk = FindThunk(stubStartAddress);
    if(NULL != pThunk)
    {
        LPBYTE pbAddr = NULL;
        LONG destAddress = 0;
        if(stubStartAddress == pThunk)
        {

            // Extract the long which gives the self relative address
            // of the destination
            pbAddr = pThunk + sizeof(BYTE) + sizeof(DWORD) + sizeof(BYTE);
            destAddress = *(LONG *)pbAddr;

            // Calculate the absolute address by adding the offset of the next
            // instruction after the call instruction
            destAddress += (LONG)(size_t)(pbAddr + sizeof(LONG));

        }

        // We cannot tell where the stub will end up until OnCall is reached.
        // So we tell the debugger to run till OnCall is reached and then
        // come back and ask us again for the actual destination address of
        // the call

        Stub *stub = Stub::RecoverStub((TADDR)destAddress);

        trace->InitForFramePush(stub->GetPatchAddress());
        bIsStub = TRUE;
    }

    return bIsStub;
}

//+----------------------------------------------------------------------------
//
//  Method:     CVirtualThunkMgr::IsThunkByASM  public
//
//  Synopsis:   Check assembly to see if this one of our thunks
//+----------------------------------------------------------------------------
BOOL CVirtualThunkMgr::IsThunkByASM(const BYTE *startaddr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(startaddr));
    }
    CONTRACTL_END;

    // Future:: Try using the rangelist. This may be a problem if the code is not at least 6 bytes long
    const BYTE *bCode = startaddr + 6;
    return (startaddr &&
            (startaddr[0] == 0x68) &&
            (startaddr[5] == 0xe9) &&
            (*((LONG *) bCode) == (LONG)((LONG_PTR)CTPMethodTable::GetTPStub()->GetEntryPoint()) - (LONG_PTR)(bCode + sizeof(LONG))));
}

//+----------------------------------------------------------------------------
//
//  Method:     CVirtualThunkMgr::GetMethodDescByASM   public
//
//  Synopsis:   Parses MethodDesc out of assembly code
//+----------------------------------------------------------------------------
MethodDesc *CVirtualThunkMgr::GetMethodDescByASM(const BYTE *startaddr, MethodTable *pMT)
{
    CONTRACT (MethodDesc*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(startaddr));
        PRECONDITION(CheckPointer(pMT));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    RETURN (pMT->GetMethodDescForSlot(*((DWORD *) (startaddr + 1))));
}


#ifndef HAS_REMOTING_PRECODE

//+----------------------------------------------------------------------------
//
//  Method:     CNonVirtualThunkMgr::TraceManager   public
//
//  Synopsis:   Traces the stub given the current context
//+----------------------------------------------------------------------------
BOOL CNonVirtualThunkMgr::TraceManager(Thread *thread,
                                       TraceDestination *trace,
                                       CONTEXT *pContext,
                                       BYTE **pRetAddr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(thread, NULL_OK));
        PRECONDITION(CheckPointer(trace));
        PRECONDITION(CheckPointer(pContext));
        PRECONDITION(CheckPointer(pRetAddr));
    }
    CONTRACTL_END;

    BOOL bRet = FALSE;

    // Does this.MethodTable ([ecx]) == CTPMethodTable::GetMethodTable()?
    DWORD pThis = pContext->Ecx;

    if ((pThis != NULL) &&
        (*(DWORD*)(size_t)pThis == (DWORD)(size_t)CTPMethodTable::GetMethodTable()))
    {
        //
        bRet = FALSE;
    }
    else
    {
        // No proxy in the way, so figure out where we're really going
        // to and let the stub manager try to pickup the trace from
        // there.
        DWORD stubStartAddress = pContext->Eip -
            g_dwNonVirtualThunkReCheckLabelOffset;

        // Extract the long which gives the address of the destination
        BYTE* pbAddr = (BYTE *)(size_t)(stubStartAddress +
                                g_dwNonVirtualThunkRemotingLabelOffset -
                                sizeof(DWORD));

        // Since we do an indirect jump we have to dereference it twice
        LONG destAddress = **(LONG **)pbAddr;

        // Ask the stub manager to trace the destination address
        bRet = StubManager::TraceStub((TADDR)(BYTE *)(size_t)destAddress, trace);
    }

    // While we may have made it this far, further tracing may reveal
    // that the debugger can't continue on. Therefore, since there is
    // no frame currently pushed, we need to tell the debugger where
    // we're returning to just in case it hits such a situtation.  We
    // know that the return address is on the top of the thread's
    // stack.
    *pRetAddr = *((BYTE**)(size_t)(pContext->Esp));

    return bRet;
}

//+----------------------------------------------------------------------------
//
//  Method:     CNonVirtualThunkMgr::DoTraceStub   public
//
//  Synopsis:   Traces the stub given the starting address
//+----------------------------------------------------------------------------
BOOL CNonVirtualThunkMgr::DoTraceStub(const BYTE *stubStartAddress,
                                      TraceDestination *trace)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(stubStartAddress));
        PRECONDITION(CheckPointer(trace));
    }
    CONTRACTL_END;

    BOOL bRet = FALSE;

    CNonVirtualThunk* pThunk = FindThunk(stubStartAddress);

    if(NULL != pThunk)
    {
        // We can either jump to
        // (1) a slot in the transparent proxy table (UNMANAGED)
        // (2) a slot in the non virtual part of the vtable
        // ... so, we need to return TRACE_MGR_PUSH with the address
        // at which we want to be called back with the thread's context
        // so we can figure out which way we're gonna go.
        if(stubStartAddress == pThunk->GetThunkCode())
        {
            trace->InitForManagerPush(
                (TADDR) (stubStartAddress + g_dwNonVirtualThunkReCheckLabelOffset),
                this);
            bRet = TRUE;
        }
    }

    return bRet;
}

//+----------------------------------------------------------------------------
//
//  Method:     CNonVirtualThunkMgr::IsThunkByASM  public
//
//  Synopsis:   Check assembly to see if this one of our thunks
//+----------------------------------------------------------------------------
BOOL CNonVirtualThunkMgr::IsThunkByASM(const BYTE *startaddr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(startaddr));
    }
    CONTRACTL_END;


    // FUTURE:: Try using rangelist, this may be a problem if the code is not long enough
    return  (startaddr &&
             startaddr[0] == 0x85 &&
             startaddr[1] == 0xc9 &&
             startaddr[2] == 0x74 &&
             (*((DWORD *)(startaddr + 6)) == (DWORD)(size_t)CTPMethodTable::GetMethodTable()));
}

//+----------------------------------------------------------------------------
//
//  Method:     CNonVirtualThunkMgr::GetMethodDescByASM   public
//
//  Synopsis:   Parses MethodDesc out of assembly code
//+----------------------------------------------------------------------------
MethodDesc *CNonVirtualThunkMgr::GetMethodDescByASM(const BYTE *startaddr)
{
    CONTRACT (MethodDesc*)
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(startaddr));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    RETURN (*((MethodDesc **) (startaddr + 19)));
}

#endif // HAS_REMOTING_PRECODE
