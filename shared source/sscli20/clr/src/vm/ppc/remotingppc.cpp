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
** File:    remotingppc.cpp
**       
** Purpose: Defines various remoting related functions for the ppc architecture
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

extern "C" size_t g_dwTPStubAddr; // This is the external entrypoint - expects MethodDesc
extern "C" size_t g_dwOOContextAddr;

size_t g_dwTPStubAddr;
size_t g_dwOOContextAddr;

// External variables
extern DWORD g_dwNonVirtualThunkRemotingLabelOffset;
extern DWORD g_dwNonVirtualThunkReCheckLabelOffset;

extern "C" void __stdcall CRemotingServices__DispatchInterfaceCall(MethodDesc *pMD);

//+----------------------------------------------------------------------------
//
//  Method:     EmitCallToCheckForContextMatch     private
//
//  Synopsis:   This code generates a call to CheckForContextMatch.
//              Sets kNE on context mismatch.
//
//+----------------------------------------------------------------------------
static void EmitCallToCheckForContextMatch(CPUSTUBLINKER* psl, PPCReg regThis)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(psl));
    }
    CONTRACTL_END;

    // Move into kR0 the stub data and call the stub
    psl->PPCEmitLwz(kR0, regThis, TP_OFFSET_STUBDATA);
    psl->PPCEmitLwz(kR12, regThis, TP_OFFSET_STUB);


    // mtctr r12, bctrl
    psl->PPCEmitCallR12();


    // cmpwi r0,0
    psl->PPCEmitCmpwi(kR0, 0);
}

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

    // This must be the remoting proxy
    psl->PPCEmitLoadImm(kR12, (INT)(size_t)GetEEFuncEntryPoint(CRemotingServices__DispatchInterfaceCall));

    // mtctr r12, bctr
    psl->PPCEmitBranchR12();
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

    DWORD *pCode = (DWORD*)startaddr;

    _ASSERTE(NULL != s_pTPStub);
    UINT32 addr = (UINT32)(UINT_PTR)s_pTPStub->GetEntryPoint();

    // addis r12, 0, (addr>>16)
    *pCode++ = 0x3D800000 | (addr >> 16);
    // ori r12,r12,addr
    *pCode++ = 0x618C0000 | (UINT16)addr;

    // The high 16 bits of r11 can contain a bogus sign extension because of this
    _ASSERTE(s_dwMaxSlots <= 65536);
    _ASSERTE(dwSlot < 65536);

    // li r11, value
    *pCode++ = 0x39600000 | dwSlot;

    // mtctr r12
    *pCode++ = 0x7D8903A6;

    // bctr
    *pCode++ = 0x4E800420;

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

    PORTABILITY_WARNING("CTPMethodTable::ActivatePrecodeRemotingThunk");
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

    MetaSig msig(pMD);
    PPCReg regThis = msig.HasRetBuffArg() ? kR4 : kR3;
       
    // The thunk has not been created yet. Go ahead and create it.    

    // Generate label where a null reference exception will be thrown
    CodeLabel *pJmpAddrLabel = psl->NewCodeLabel();
    // Generate label where remoting code will execute
    CodeLabel *pRemotingLabel = psl->NewCodeLabel();

    // if this == NULL throw NullReferenceException
    psl->PPCEmitCmpwi(regThis, 0);
    psl->PPCEmitBranch(pJmpAddrLabel, PPCCondCode::kEQ);

    // Emit a label here for the debugger. A breakpoint will
    // be set at the next instruction and the debugger will
    // call CNonVirtualThunkMgr::TraceManager when the
    // breakpoint is hit with the thread's context.
    CodeLabel *pRecheckLabel = psl->NewCodeLabel();
    psl->EmitLabel(pRecheckLabel);

    // If this.MethodTable != TPMethodTable then do RemotingCall
    psl->PPCEmitLwz(kR12, regThis);
    psl->PPCEmitLoadImm(kR0, (__int32)(size_t)GetMethodTable());
    psl->PPCEmitCmpw(kR0, kR12);

    // marshalbyref case
    psl->PPCEmitBranch(pJmpAddrLabel, PPCCondCode::kNE);

    // erect stack frame before making the call

    // mflr r0
    psl->Emit32(0x7C0802A6);
    // stw r0, 8(r1)
    psl->PPCEmitStw(kR0, kR1, 8);
    // stwu r1, -local_space(r1)
    psl->PPCEmitRegRegDisp16(0x94000000, kR1, kR1, -AlignStack(sizeof(LinkageArea)));

    // sets kNE on context mismatch
    EmitCallToCheckForContextMatch(psl, regThis);

    // restore the stack frame

    // lwz r1, 0(r1)
    psl->PPCEmitLwz(kR1, kR1, 0);
    // lwz r0, 8(r1)
    psl->PPCEmitLwz(kR0, kR1, 8);
    // mtlr r0
    psl->Emit32(0x7C0803A6);

    // bne CtxMismatch
    psl->PPCEmitBranch(pRemotingLabel, PPCCondCode::kNE);

    // Exception handling and non-remoting share the 
    // same codepath
    psl->EmitLabel(pJmpAddrLabel);
    psl->PPCEmitLoadImm(kR12, (__int32)(size_t)pvAddrOfCode);

    // mtctr r12, bctr
    psl->PPCEmitBranchR12();
    
    psl->EmitLabel(pRemotingLabel);
            
    // the MethodDesc should be still in R11.  goto TPStub
    // jmp TPStub
    psl->PPCEmitLoadImm(kR12, (__int32)(size_t)g_dwTPStubAddr);
    psl->PPCEmitBranchR12();

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

    MetaSig msig(pMD);
    PPCReg regThis = msig.HasRetBuffArg() ? kR4 : kR3;

    // The thunk has not been created yet. Go ahead and create it.    
    // Compute the address of the slot         
    LPVOID pvSlot = (LPVOID)pMD->GetAddrOfSlot();

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
    psl->PPCEmitCmpwi(regThis, 0);
    psl->PPCEmitBranch(pExceptionLabel, PPCCondCode::kEQ);

    // Generate label where remoting code will execute
    CodeLabel *pRemotingLabel = psl->NewCodeLabel();

    // Emit a label here for the debugger. A breakpoint will
    // be set at the next instruction and the debugger will
    // call CNonVirtualThunkMgr::TraceManager when the
    // breakpoint is hit with the thread's context.
    CodeLabel *pRecheckLabel = psl->NewCodeLabel();
    psl->EmitLabel(pRecheckLabel);

    // If this.MethodTable != TPMethodTable then do RemotingCall
    psl->PPCEmitLwz(kR12, regThis);
    psl->PPCEmitLoadImmNonoptimized(kR0, (__int32)(size_t)GetMethodTable());
    psl->PPCEmitCmpw(kR0, kR12);

    psl->PPCEmitBranch(pRemotingLabel, PPCCondCode::kEQ);

    // Exception handling and non-remoting share the 
    // same codepath
    psl->EmitLabel(pExceptionLabel);

    // Non-RemotingCode
    // Jump to the vtable slot of the method
    psl->PPCEmitLoadImmNonoptimized(kR12, (DWORD)(size_t)pvSlot);
    psl->PPCEmitLwz(kR12, kR12);
    psl->PPCEmitBranchR12();

    // Remoting code. Note: CNonVirtualThunkMgr::TraceManager
    // relies on the arrangement of instructions around this label.
    // If you change any instructions after the pvSlot loadimm
    // above and before the MethodDesc loadimm below, update
    // CNonVirtualThunkMgr::DoTraceStub and TraceManager.
    psl->EmitLabel(pRemotingLabel);

    // Save the MethodDesc and goto TPStub
    // push MethodDesc
    psl->PPCEmitLoadImmNonoptimized(kR12, (DWORD)(size_t)g_dwTPStubAddr);
    psl->PPCEmitLoadImmNonoptimized(kR11, (DWORD)(size_t)pMD);
    psl->PPCEmitBranchR12();

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

    RETURN(pStub->GetEntryPoint());
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

        CodeLabel *OOExternal = pStubLinker->NewCodeLabel();
        CodeLabel *ThisReg = pStubLinker->NewCodeLabel();
        CodeLabel *ConvMD = pStubLinker->NewCodeLabel();
        CodeLabel *UseCode = pStubLinker->NewCodeLabel();
        CodeLabel *OOContext = pStubLinker->NewCodeLabel();
        CodeLabel *FixupMD = pStubLinker->NewCodeLabel();

        // The slot # is sign extended - get rid of the sign extension

        // clrlwi r11, r11, 16 == rlwinm r11, r11, 0, 16, 31
        pStubLinker->Emit32(0x556B043E);

        pStubLinker->EmitLabel(OOExternal);

        // EmitCallToStub(pStubLinker, OOContext);

        // erect stack frame before making the call

        // mflr r12
        pStubLinker->Emit32(0x7D8802A6);
        // stw r12, 8(r1)
        pStubLinker->PPCEmitStw(kR12, kR1, 8);
        // stwu r1, -local_space(r1)
        pStubLinker->PPCEmitRegRegDisp16(0x94000000, kR1, kR1, -AlignStack(sizeof(LinkageArea)+4));

        // get the this register: look at the callsite whether it contains
        // the magic instruction to determine the location of the this pointer

        // lwz r0, 0(r12)
        pStubLinker->PPCEmitLwz(kR0, kR12);

        // kR12 = oris r0, r0, 0
        pStubLinker->PPCEmitLoadImm(kR12, 0x64000000);
        pStubLinker->PPCEmitCmpw(kR0, kR12);

        // kR12 = this reg
        pStubLinker->PPCEmitMr(kR12, kR3);
        pStubLinker->PPCEmitBranch(ThisReg, PPCCondCode::kNE);
        pStubLinker->PPCEmitMr(kR12, kR4);
        pStubLinker->EmitLabel(ThisReg);

        // save this reg
        pStubLinker->PPCEmitStw(kR12, kR1, sizeof(LinkageArea));

        // sets kNE on context mismatch
        EmitCallToCheckForContextMatch(pStubLinker, kR12);

        // restore this reg
        pStubLinker->PPCEmitLwz(kR12, kR1, sizeof(LinkageArea));

        // restore the stack frame

        // lwz r1, 0(r1)
        pStubLinker->PPCEmitLwz(kR1, kR1, 0);
        // lwz r0, 8(r1)
        pStubLinker->PPCEmitLwz(kR0, kR1, 8);
        // mtlr r0
        pStubLinker->Emit32(0x7C0803A6);

        // bne CtxMismatch
        pStubLinker->PPCEmitBranch(OOContext, PPCCondCode::kNE);

        // The contexts match. Jump to the real address and start executing.
        EmitJumpToAddressCode(pStubLinker, ConvMD, UseCode);

        // label: OOContext
        pStubLinker->EmitLabel(OOContext);

        // CONTEXT MISMATCH CASE, call out to the real proxy to
        // dispatch

        // andis. r12,r11,0xFFFF0000
        pStubLinker->Emit32(0x756CFFFF);
        // beq FixupMD
        pStubLinker->PPCEmitBranch(FixupMD, PPCCondCode::kEQ);

        // label: FixupMD
        pStubLinker->EmitLabel(FixupMD);

        // Setup the frame
        EmitSetupFrameCode(pStubLinker);

        // Finally, create the stub
        s_pTPStub = pStubLinker->Link();

        g_dwTPStubAddr = (size_t)(s_pTPStub->GetEntryPoint() +
                                    pStubLinker->GetLabelOffset(OOExternal));

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

        // Setup the frame
        EmitSetupFrameCode(pStubLinker);

        s_pDelegateStub = pStubLinker->Link();
    }
        
    RETURN(s_pDelegateStub);
}

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

    // andis. r0,r11,0xFFFF0000
    pStubLinker->Emit32(0x7560FFFF);

    // bne ConvMD
    pStubLinker->PPCEmitBranch(ConvMD, PPCCondCode::kNE);
    
    // if (!(r11 & 0xffff0000))
    // {
    
        // ** code addr from slot case **
    
        // lwz r12, (kR12)(TPMethodTable::GetOffsetOfMT())
        pStubLinker->PPCEmitLwz(kR12, kR12, TP_OFFSET_MT);

        // slwi r11, r11, 2 == rlwinm r11, r11, 2, 0, 29
        pStubLinker->Emit32(0x556B103A);

        // add r12, r12, r11
        pStubLinker->PPCEmitRegRegReg(0x7C000214, kR12, kR12, kR11);

        // lwz r12,[r12 + MethodTable::GetVtableOffset()]
        pStubLinker->PPCEmitLwz(kR12, kR12, MethodTable::GetVtableOffset());

        // mtctr r12, bctr
        pStubLinker->PPCEmitBranchR12();
    
    // }
    // else
    // {
        // ** code addr from MethodDesc case **

        pStubLinker->EmitLabel(ConvMD);                
        
        // mr r12, r11
        pStubLinker->PPCEmitMr(kR12, kR11);

        // mtctr r12, bctr
        pStubLinker->PPCEmitBranchR12();

    // }
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

    pStubLinker->EmitMethodStubProlog(TPMethodFrame::GetMethodFrameVPtr(), FALSE);

    //  StubStackFrame::params are used for 3 params and 1 INT64 local var

    // Complete the setup of the frame by calling PreCall

    // call PreCall with the frame as an argument
    pStubLinker->PPCEmitAddi(kR3, kR1, offsetof(StubStackFrame, methodframe));  
    pStubLinker->PPCEmitCall(pStubLinker->NewExternalCodeLabel(GetEEFuncEntryPoint(PreCall)));

    ////////////////END SETUP FRAME////////////////////////////////                
    
    // Debugger patch location
    // NOTE: This MUST follow the call to emit the "PreCall" label
    // since after PreCall we know how to help the debugger in 
    // finding the actual destination address of the call.
    // See CVirtualThunkMgr::DoTraceStub.
    pStubLinker->EmitPatchLabel();

    // Call
    pStubLinker->PPCEmitAddi(kR3, kR1, offsetof(StubStackFrame, methodframe)); // new frame as ARG
    pStubLinker->PPCEmitMr(kR4, kR31); // current thread as ARG
    pStubLinker->PPCEmitAddi(kR5, kR1, offsetof(StubStackFrame, argregs) - sizeof(INT64)); // return value as ARG
    pStubLinker->PPCEmitCall(pStubLinker->NewExternalCodeLabel(GetEEFuncEntryPoint(OnCall)));

    // Epilog
    pStubLinker->EmitMethodStubEpilog(kNoTripStubStyle);
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
        LONG destAddress = 0;
        if(stubStartAddress == pThunk)
        {
            // The first two instructions contain the address
            destAddress = PPCCrackLoadImmNonoptimized((DWORD*)stubStartAddress);
        }

        // We cannot tell where the stub will end up until OnCall is reached.
        // So we tell the debugger to run till OnCall is reached and then 
        // come back and ask us again for the actual destination address of 
        // the call
    
        Stub *stub = Stub::RecoverStub((TADDR)(size_t)destAddress);

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

    // This is decoding code generated in CTPMethodTable::CreateThunkForVirtualMethod

    DWORD *pCode = (DWORD*)startaddr;

    // Future:: Try using the rangelist. This may be a problem if the code is not at least 20 bytes long
    return (startaddr &&
        PPCIsLoadImmNonoptimized(pCode+0, kR12) &&
        (PPCCrackLoadImmNonoptimized(pCode+0) == 
            (INT)(size_t)CTPMethodTable::GetTPStub()->GetEntryPoint()) &&
        ((pCode[2] >> 16) == 0x3960) && // li r11, value
        (pCode[3] == 0x7D8903A6) && // mtctr r12
        (pCode[4] == 0x4E800420)); // bctr
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

    // This is decoding code generated in CTPMethodTable::CreateThunkForVirtualMethod

    DWORD *pCode = (DWORD*)startaddr;

    RETURN pMT->GetMethodDescForSlot((UINT16)pCode[2]);
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
    
    // We need to find the methodDesc first
    DWORD stubStartAddress = pContext->Iar -
            g_dwNonVirtualThunkReCheckLabelOffset;

    DWORD* remotingLabelAddr = (DWORD *)(size_t)(stubStartAddress +
                                g_dwNonVirtualThunkRemotingLabelOffset);

    MethodDesc *pMD =
        (MethodDesc *)(size_t)PPCCrackLoadImmNonoptimized(remotingLabelAddr + 2);

    _ASSERTE(pMD != NULL);

    MetaSig msig(pMD);
    DWORD pThis = msig.HasRetBuffArg() ? pContext->Gpr4 : pContext->Gpr3;

    // Does this.MethodTable == CTPMethodTable::GetMethodTable()?
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
        
        // Find the address of the loadimm instructions that have the slot
        DWORD* pvSlotAddr = remotingLabelAddr - 5;

        // Extract the immediate value
        DWORD pvSlot = PPCCrackLoadImmNonoptimized(pvSlotAddr);

        // Dereference it to get the address
        BYTE *destAddress = *(BYTE **)(size_t)pvSlot;

        // Ask the stub manager to trace the destination address
        bRet = StubManager::TraceStub((TADDR) destAddress, trace);
    }

    // While we may have made it this far, further tracing may reveal
    // that the debugger can't continue on. Therefore, since there is
    // no frame currently pushed, we need to tell the debugger where
    // we're returning to just in case it hits such a situation.  We
    // know that the return address is in the link register
    *pRetAddr = (BYTE*)(size_t)(pContext->Lr);
    
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

    DWORD *pCode = (DWORD*)startaddr;

    // This is decoding code generated in CTPMethodTable::CreateNonVirtualThunkForVirtualMethod

    // FUTURE:: Try using rangelist, this may be a problem if the code is not long enough
    return  (startaddr &&
        ((pCode[0] == 0x2c030000) || (pCode[0] == 0x2c040000)) && // cmpwi regThis, 0
        (pCode[1] == 0x41820018) && // beq pExceptionLabel
        ((pCode[2] == 0x81830000) || (pCode[2] == 0x81840000)) && // lwz r12,regThis
        PPCIsLoadImmNonoptimized(pCode+3, kR0) &&
        (PPCCrackLoadImmNonoptimized(pCode+3) == 
            (INT)(size_t)CTPMethodTable::GetMethodTable()) &&
        (pCode[5] == 0x7c006000) && // cmpw r0,r12
        (pCode[6] == 0x41820018) && // beq pRemotingLabel
        PPCIsLoadImmNonoptimized(pCode+7, kR12) &&
        (pCode[9] == 0x818c0000) && // lwz r12,0(r12)
        (pCode[10] == 0x7d8903a6) && // mtctr r12
        (pCode[11] == 0x4e800420) && // bctr
        PPCIsLoadImmNonoptimized(pCode+12, kR12) &&
        (PPCCrackLoadImmNonoptimized(pCode+12) == 
            (INT)(size_t)g_dwTPStubAddr) &&
        PPCIsLoadImmNonoptimized(pCode+14, kR11) &&
        (pCode[16] == 0x7d8903a6) && // mtctr r12
        (pCode[17] == 0x4e800420)); // bctr
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

    DWORD *pCode = (DWORD*)startaddr;

    RETURN (MethodDesc *)(size_t)PPCCrackLoadImmNonoptimized(pCode+14);
}

#endif // HAS_REMOTING_PRECODE
