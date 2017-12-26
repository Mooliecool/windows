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


// NOTE on Frame Size C_ASSERT usage in this file 
// if the frame size changes then the stubs have to be revisited for correctness
// kindly revist the logic and then update the constants so that the C_ASSERT will again fire
// if someone changes the frame size.  You are expected to keep this hard coded constant
// up to date so that changes in the frame size trigger errors at compile time if the code is not altered

// Precompiled Header
#include "common.h"

#include "field.h"
#include "stublink.h"
#include "tls.h"
#include "frames.h"
#include "excep.h"
#include "dllimport.h"
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
#include "securitydeclarative.h"
#ifdef _X86_
#include "asmconstants.h"
#endif // _X86_
#include "class.h"
#include "stublink.inl"


#if defined(_DEBUG) && defined(STUBLINKER_GENERATES_UNWIND_INFO)
#include <psapi.h>
#endif

#ifndef DACCESS_COMPILE

extern "C" void __stdcall JIT_ByRefWriteBarrier();
extern "C" VOID __cdecl StubRareEnable(Thread *pThread);
extern "C" HRESULT __cdecl StubRareDisableHR(Thread *pThread, Frame *pFrame);
extern "C" VOID __cdecl StubRareDisableRETURN(Thread *pThread, Frame *pFrame);
extern "C" VOID __cdecl StubRareDisableTHROW(Thread *pThread, Frame *pFrame);
extern "C" VOID __cdecl ArrayOpStubNullException(void);
extern "C" VOID __cdecl ArrayOpStubRangeException(void);
extern "C" VOID __cdecl ArrayOpStubTypeMismatchException(void);


// Presumably this code knows what it is doing with TLS.  If we are hiding these
// services from normal code, reveal them here.
#ifdef TlsGetValue
#undef TlsGetValue
#endif

// fwd decl
Thread* __stdcall CreateThreadBlockThrow();





//-----------------------------------------------------------------------
// InstructionFormat for near Jump and short Jump
//-----------------------------------------------------------------------
class X86NearJump : public InstructionFormat
{
    public:
        X86NearJump() : InstructionFormat(  InstructionFormat::k8|InstructionFormat::k32
                                          )
        {
            LEAF_CONTRACT;
        }

        virtual UINT GetSizeOfInstruction(UINT refsize, UINT variationCode)
        {
            LEAF_CONTRACT
            switch (refsize)
            {
                case k8:
                    return 2;

                case k32:
                    return 5;
                default:
                    _ASSERTE(!"unexpected refsize");
                    return 0;

            }
        }

        virtual VOID EmitInstruction(UINT refsize, __int64 fixedUpReference, BYTE *pOutBuffer, UINT variationCode, BYTE *pDataBuffer)
        {
            LEAF_CONTRACT
            if (k8 == refsize)
            {
                pOutBuffer[0] = 0xeb;
                *((__int8*)(pOutBuffer+1)) = (__int8)fixedUpReference;
            }
            else if (k32 == refsize)
            {
                pOutBuffer[0] = 0xe9;
                *((__int32*)(pOutBuffer+1)) = (__int32)fixedUpReference;
            }
            else
            {
                _ASSERTE(!"unreached");
            }
        }

        virtual BOOL CanReach(UINT refsize, UINT variationCode, BOOL fExternal, INT_PTR offset)
        {
            STATIC_CONTRACT_NOTHROW;
            STATIC_CONTRACT_GC_NOTRIGGER;
            STATIC_CONTRACT_FORBID_FAULT;


            if (fExternal)
            {
                switch (refsize)
                {
                case InstructionFormat::k8:
                    // For external, we don't have enough info to predict
                    // the offset.
                    return FALSE;

                case InstructionFormat::k32:
                    return sizeof(PVOID) <= sizeof(UINT32);

                case InstructionFormat::kAllowAlways:
                    return TRUE;

                default:
                    _ASSERTE(0);
                    return FALSE;
                }
            }
            else
            {
                switch (refsize)
                {
                case InstructionFormat::k8:
                    return FitsInI1(offset);

                case InstructionFormat::k32:
                    return TRUE;

                case InstructionFormat::kAllowAlways:
                    return TRUE;
                default:
                    _ASSERTE(0);
                    return FALSE;
                }
            }
        }
};


//-----------------------------------------------------------------------
// InstructionFormat for conditional jump. Set the variationCode
// to members of X86CondCode.
//-----------------------------------------------------------------------
class X86CondJump : public InstructionFormat
{
    public:
        X86CondJump(UINT allowedSizes) : InstructionFormat(allowedSizes)
        {
            LEAF_CONTRACT;
        }

        virtual UINT GetSizeOfInstruction(UINT refsize, UINT variationCode)
        {
        LEAF_CONTRACT
            return (refsize == k8 ? 2 : 6);
        }

        virtual VOID EmitInstruction(UINT refsize, __int64 fixedUpReference, BYTE *pOutBuffer, UINT variationCode, BYTE *pDataBuffer)
        {
        LEAF_CONTRACT
        if (refsize == k8)
        {
                pOutBuffer[0] = 0x70|variationCode;
                *((__int8*)(pOutBuffer+1)) = (__int8)fixedUpReference;
        }
        else
        {
                pOutBuffer[0] = 0x0f;
                pOutBuffer[1] = 0x80|variationCode;
                *((__int32*)(pOutBuffer+2)) = (__int32)fixedUpReference;
            }
        }
};


//-----------------------------------------------------------------------
// InstructionFormat for near call.
//-----------------------------------------------------------------------
class X86Call : public InstructionFormat
{
    public:
        X86Call ()
            : InstructionFormat(  InstructionFormat::k32
                                )
        {
            LEAF_CONTRACT;
        }

        virtual UINT GetSizeOfInstruction(UINT refsize, UINT variationCode)
        {
            LEAF_CONTRACT;

            switch (refsize)
            {
            case k32:
                return 5;


            default:
                _ASSERTE(!"unexpected refsize");
                return 0;
            }
        }

        virtual VOID EmitInstruction(UINT refsize, __int64 fixedUpReference, BYTE *pOutBuffer, UINT variationCode, BYTE *pDataBuffer)
        {
            LEAF_CONTRACT

            switch (refsize)
            {
            case k32:
                pOutBuffer[0] = 0xE8;
                *((__int32*)(1+pOutBuffer)) = (__int32)fixedUpReference;
                break;


            default:
                _ASSERTE(!"unreached");
                break;
            }
        }

// For x86, the default CanReach implementation will suffice.  It only needs
// to handle k32.
};


//-----------------------------------------------------------------------
// InstructionFormat for push imm32.
//-----------------------------------------------------------------------
class X86PushImm32 : public InstructionFormat
{
    public:
        X86PushImm32(UINT allowedSizes) : InstructionFormat(allowedSizes)
        {
            LEAF_CONTRACT;
        }

        virtual UINT GetSizeOfInstruction(UINT refsize, UINT variationCode)
        {
            LEAF_CONTRACT;

            return 5;
        }

        virtual VOID EmitInstruction(UINT refsize, __int64 fixedUpReference, BYTE *pOutBuffer, UINT variationCode, BYTE *pDataBuffer)
        {
            LEAF_CONTRACT;

            pOutBuffer[0] = 0x68;
            // only support absolute pushimm32 of the label address. The fixedUpReference is
            // the offset to the label from the current point, so add to get address
            *((__int32*)(1+pOutBuffer)) = (__int32)(fixedUpReference);
        }
};


static BYTE gX86NearJump[sizeof(X86NearJump)];
static BYTE gX86CondJump[sizeof(X86CondJump)];
static BYTE gX86Call[sizeof(X86Call)];
static BYTE gX86PushImm32[sizeof(X86PushImm32)];

/* static */ void StubLinkerCPU::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    new (gX86NearJump) X86NearJump();
    new (gX86CondJump) X86CondJump( InstructionFormat::k8|InstructionFormat::k32);
    new (gX86Call) X86Call();
    new (gX86PushImm32) X86PushImm32(InstructionFormat::k32);

}

//---------------------------------------------------------------
// Emits:
//    mov destReg, srcReg
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitMovRegReg(X86Reg destReg, X86Reg srcReg)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    Emit8(0x89);
    Emit8(0xC0 | (srcReg << 3) | destReg);
}

//---------------------------------------------------------------

VOID StubLinkerCPU::X86EmitMovSPReg(X86Reg srcReg)
{
    WRAPPER_CONTRACT;
    const X86Reg kESP = (X86Reg)4;
    X86EmitMovRegReg(kESP, srcReg);
}

VOID StubLinkerCPU::X86EmitMovRegSP(X86Reg destReg)
{
    WRAPPER_CONTRACT;
    const X86Reg kESP = (X86Reg)4;
    X86EmitMovRegReg(destReg, kESP);
}


//---------------------------------------------------------------
// Emits:
//    PUSH <reg32>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitPushReg(X86Reg reg)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    X86Reg origReg = reg;
#endif

    Emit8(0x50+reg);

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    if (IsPreservedReg(origReg))
    {
        UnwindPushedReg(origReg);
    }
    else
#endif
    {
        Push(sizeof(void*));
    }
}


//---------------------------------------------------------------
// Emits:
//    POP <reg32>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitPopReg(X86Reg reg)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    Emit8(0x58+reg);
    Pop(sizeof(void*));
}


//---------------------------------------------------------------
// Emits:
//    PUSH <imm32>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitPushImm32(UINT32 value)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    Emit8(0x68);
    Emit32(value);
    Push(sizeof(void*));
}


//---------------------------------------------------------------
// Emits:
//    PUSH <imm32>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitPushImm32(CodeLabel &target)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    EmitLabelRef(&target, reinterpret_cast<X86PushImm32&>(gX86PushImm32), 0);
}


//---------------------------------------------------------------
// Emits:
//    PUSH <imm8>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitPushImm8(BYTE value)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    Emit8(0x6a);
    Emit8(value);
    Push(sizeof(void*));
}


//---------------------------------------------------------------
// Emits:
//    PUSH <ptr>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitPushImmPtr(LPVOID value)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    X86EmitPushImm32((UINT_PTR) value);
}


//---------------------------------------------------------------
// Emits:
//    XOR <reg32>,<reg32>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitZeroOutReg(X86Reg reg)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    Emit8(0x33);
    Emit8( 0xc0 | (reg << 3) | reg );
}


//---------------------------------------------------------------
// Emits:
//    CMP <reg32>,imm32
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitCmpRegImm32(X86Reg reg, INT32 imm32)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION((int) reg < NumX86Regs);
    }
    CONTRACTL_END;


    if (FitsInI1(imm32)) {
        Emit8(0x83);
        Emit8(0xF8 | reg);
        Emit8((INT8)imm32);
    } else {
        Emit8(0x81);
        Emit8(0xF8 | reg);
        Emit32(imm32);
    }
}

//---------------------------------------------------------------
// Emits:
//    CMP [reg+offs], imm32
//    CMP [reg], imm32
//---------------------------------------------------------------
VOID StubLinkerCPU:: X86EmitCmpRegIndexImm32(X86Reg reg, INT32 offs, INT32 imm32)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION((int) reg < NumX86Regs);
    }
    CONTRACTL_END;

    //
    // The binary representation of "cmp [mem], imm32" is :
    // 1000-00sw  mod11-1r/m
    //
    
    unsigned wBit = (FitsInI1(imm32) ? 0 : 1);
    Emit8(0x80 | wBit);
    
    unsigned modBits;
    if (offs == 0)
        modBits = 0;
    else if (FitsInI1(offs))
        modBits = 1;
    else
        modBits = 2;
    
    Emit8((modBits << 6) | 0x38 | reg );

    if (offs)
    {
        if (FitsInI1(offs))
            Emit8((INT8)offs);
        else
            Emit32(offs);
    }
    
    if (FitsInI1(imm32))
        Emit8((INT8)imm32);
    else
        Emit32(imm32);
}

//---------------------------------------------------------------
// Emits:
//  add     rsp, imm32
//  jmp     <target>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitTailcallWithESPAdjust(CodeLabel *pTarget, INT32 imm32)
{
    WRAPPER_CONTRACT;
    X86EmitAddEsp(imm32);
    X86EmitNearJump(pTarget);
}

//---------------------------------------------------------------
// Emits:
//  pop     reg
//  jmp     <target>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitTailcallWithSinglePop(CodeLabel *pTarget, X86Reg reg)
{
    WRAPPER_CONTRACT;
    X86EmitPopReg(reg);
    X86EmitNearJump(pTarget);
}

//---------------------------------------------------------------
// Emits:
//    JMP <ofs8>   or
//    JMP <ofs32}
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitNearJump(CodeLabel *target)
{
    WRAPPER_CONTRACT
    EmitLabelRef(target, reinterpret_cast<X86NearJump&>(gX86NearJump), 0);
}


//---------------------------------------------------------------
// Emits:
//    Jcc <ofs8> or
//    Jcc <ofs32>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitCondJump(CodeLabel *target, X86CondCode::cc condcode)
{
    WRAPPER_CONTRACT
    EmitLabelRef(target, reinterpret_cast<X86CondJump&>(gX86CondJump), condcode);
}


//---------------------------------------------------------------
// Emits:
//    call <ofs32>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitCall(CodeLabel *target, int iArgBytes,
                                BOOL returnLabel)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    EmitLabelRef(target, reinterpret_cast<X86Call&>(gX86Call), 0);

    if (returnLabel)
        EmitReturnLabel();

    INDEBUG(Emit8(0x90));   // Emit a nop after the call in debug so that
                            // we know that this is a call that can directly call
                            // managed code
    Pop(iArgBytes);
}


//---------------------------------------------------------------
// Emits:
//    ret n
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitReturn(int iArgBytes)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

    }
    CONTRACTL_END;

    if (iArgBytes == 0)
        Emit8(0xc3);
    else
    {
        Emit8(0xc2);
        Emit16(iArgBytes);
    }

    Pop(iArgBytes);
}


VOID StubLinkerCPU::X86EmitPushRegs(unsigned regSet)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    for (X86Reg r = kEAX; r <= NumX86Regs; r = (X86Reg)(r+1))
        if (regSet & (1U<<r))
            X86EmitPushReg(r);
}


VOID StubLinkerCPU::X86EmitPopRegs(unsigned regSet)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    for (X86Reg r = NumX86Regs; r >= kEAX; r = (X86Reg)(r-1))
        if (regSet & (1U<<r))
            X86EmitPopReg(r);
}


//---------------------------------------------------------------
// Emit code to store the current Thread structure in dstreg
// preservedRegSet is a set of registers to be preserved
// TRASHES  EAX, EDX, ECX unless they are in preservedRegSet.
// RESULTS  dstreg = current Thread
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitTLSFetch(DWORD idx, X86Reg dstreg, unsigned preservedRegSet)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

    // It doesn't make sense to have the destination register be preserved
        PRECONDITION((preservedRegSet & (1<<dstreg)) == 0);
        AMD64_ONLY(PRECONDITION(dstreg < 8)); // code below doesn't support high registers
    }
    CONTRACTL_END;

    TLSACCESSMODE mode = GetTLSAccessMode(idx);

#ifdef _DEBUG
    {
        static BOOL f = TRUE;
        f = !f;
        if (f)
        {
           mode = TLSACCESS_GENERIC;
        }
    }
#endif

    switch (mode)
    {

        case TLSACCESS_GENERIC:

            X86EmitPushRegs(preservedRegSet & ((1<<kEAX)|(1<<kEDX)|(1<<kECX)));

            X86EmitPushImm32(idx);
            // call TLSGetValue
            X86EmitCall(NewExternalCodeLabel((LPVOID) TlsGetValue), sizeof(void*));

            // mov [dstreg], eax
            X86_64BitOperands();
            Emit8(0x89);
            Emit8(0xc0 + dstreg);

            X86EmitPopRegs(preservedRegSet & ((1<<kEAX)|(1<<kEDX)|(1<<kECX)));

            break;

        default:
            _ASSERTE(0);
    }

#ifdef _DEBUG
    // Trash caller saved regs that we were not told to preserve, and that aren't the dstreg.
    preservedRegSet |= 1<<dstreg;
    if (!(preservedRegSet & (1<<kEAX)))
        X86EmitDebugTrashReg(kEAX);
    if (!(preservedRegSet & (1<<kEDX)))
        X86EmitDebugTrashReg(kEDX);
    if (!(preservedRegSet & (1<<kECX)))
        X86EmitDebugTrashReg(kECX);
#endif

}


//---------------------------------------------------------------
// Emit code to store the current Thread structure in ebx.
// TRASHES  eax ecx edx unless specified in preservedRegSet
// RESULTS  ebx = current Thread
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitCurrentThreadFetch(unsigned preservedRegSet)
{
    WRAPPER_CONTRACT
    X86EmitTLSFetch(GetThreadTLSIndex(), kEBX, preservedRegSet);
}




//---------------------------------------------------------------
// Emits:
//    mov <dstreg>, [<srcreg> + <ofs>]
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitIndexRegLoad(X86Reg dstreg,
                                        X86Reg srcreg,
                                        __int32 ofs)
{
    WRAPPER_CONTRACT
    X86EmitOffsetModRM(0x8b, dstreg, srcreg, ofs);
}


//---------------------------------------------------------------
// Emits:
//    mov [<dstreg> + <ofs>],<srcreg>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitIndexRegStore(X86Reg dstreg,
                                         __int32 ofs,
                                         X86Reg srcreg)
{
    WRAPPER_CONTRACT
    X86EmitOffsetModRM(0x89, srcreg, dstreg, ofs);
}


//---------------------------------------------------------------
// Emits:
//    push dword ptr [<srcreg> + <ofs>]
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitIndexPush(X86Reg srcreg, __int32 ofs)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    X86EmitOffsetModRM(0xff, (X86Reg)0x6, srcreg, ofs);
    Push(sizeof(void*));
}

//---------------------------------------------------------------
// Emits:
//    push dword ptr [<baseReg> + <indexReg>*<scale> + <ofs>]
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitBaseIndexPush(
        X86Reg baseReg,
        X86Reg indexReg,
        __int32 scale,
        __int32 ofs)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    X86EmitOffsetModRmSIB(0xff, (X86Reg)0x6, baseReg, indexReg, scale, ofs);
    Push(sizeof(void*));
}

//---------------------------------------------------------------
// Emits:
//    push dword ptr [ESP + <ofs>]
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitSPIndexPush(__int32 ofs)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    __int8 ofs8 = (__int8) ofs;
    if (ofs == (__int32) ofs8)
    {
        // The offset can be expressed in a byte (can use the byte
        // form of the push esp instruction)

        BYTE code[] = {0xff, 0x74, 0x24, ofs8};
        EmitBytes(code, sizeof(code));   
    }
    else
    {
        // The offset requires 4 bytes (need to use the long form
        // of the push esp instruction)
 
        BYTE code[] = {0xff, 0xb4, 0x24, 0x0, 0x0, 0x0, 0x0};
        *(__int32 *)(&code[3]) = ofs;        
        EmitBytes(code, sizeof(code));
    }
    
    Push(sizeof(void*));
}


//---------------------------------------------------------------
// Emits:
//    pop dword ptr [<srcreg> + <ofs>]
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitIndexPop(X86Reg srcreg, __int32 ofs)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    X86EmitOffsetModRM(0x8f, (X86Reg)0x0, srcreg, ofs);
    Pop(sizeof(void*));
}

//---------------------------------------------------------------
// Emits:
//    lea <dstreg>, [<srcreg> + <ofs>
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitIndexLea(X86Reg dstreg, X86Reg srcreg, __int32 ofs)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION((int) dstreg < NumX86Regs);
        PRECONDITION((int) srcreg < NumX86Regs);
    }
    CONTRACTL_END;

    X86EmitOffsetModRM(0x8d, dstreg, srcreg, ofs);
}

//---------------------------------------------------------------
// Emits:
//   sub esp, IMM
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitSubEsp(INT32 imm32)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    if (imm32 < 0x1000-100)
    {
        // As long as the esp size is less than 1 page plus a small
        // safety fudge factor, we can just bump esp.
        X86EmitSubEspWorker(imm32);
    }
    else
    {
        // Otherwise, must touch at least one byte for each page.
        while (imm32 >= 0x1000)
        {

            X86EmitSubEspWorker(0x1000-4);
            X86EmitPushReg(kEAX);

            imm32 -= 0x1000;
        }
        if (imm32 < 500)
        {
            X86EmitSubEspWorker(imm32);
        }
        else
        {
            // If the remainder is large, touch the last byte - again,
            // as a fudge factor.
            X86EmitSubEspWorker(imm32-4);
            X86EmitPushReg(kEAX);
        }
    }
}


//---------------------------------------------------------------
// Emits:
//   sub esp, IMM
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitSubEspWorker(INT32 imm32)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

    // On Win32, stacks must be faulted in one page at a time.
        PRECONDITION(imm32 < 0x1000);
    }
    CONTRACTL_END;

    if (!imm32)
    {
        // nop
    }
    else
    {
        X86_64BitOperands();

        if (FitsInI1(imm32))
        {
            Emit16(0xec83);
            Emit8((INT8)imm32);
        }
        else
        {
            Emit16(0xec81);
            Emit32(imm32);
        }

        Push(imm32);
    }
}


//---------------------------------------------------------------
// Emits:
//   add esp, IMM
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitAddEsp(INT32 imm32)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (!imm32)
    {
        // nop
    }
    else
    {
        X86_64BitOperands();

        if (FitsInI1(imm32))
        {
            Emit16(0xc483);
            Emit8((INT8)imm32);
        }
        else
        {
            Emit16(0xc481);
            Emit32(imm32);
        }
    }
    Pop(imm32);
}



VOID StubLinkerCPU::X86EmitAddReg(X86Reg reg, INT32 imm32)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION((int) reg < NumX86Regs);
    }
    CONTRACTL_END;


    if (FitsInI1(imm32)) {
        Emit8(0x83);
        Emit8(0xC0 | reg);
        Emit8((INT8)imm32);
    } else {
        Emit8(0x81);
        Emit8(0xC0 | reg);
        Emit32(imm32);
    }
}


VOID StubLinkerCPU::X86EmitSubReg(X86Reg reg, INT32 imm32)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION((int) reg < NumX86Regs);
    }
    CONTRACTL_END;


    if (FitsInI1(imm32)) {
        Emit8(0x83);
        Emit8(0xE8 | reg);
        Emit8((INT8)imm32);
    } else {
        Emit8(0x81);
        Emit8(0xE8 | reg);
        Emit32(imm32);
    }
}

//---------------------------------------------------------------
// Emits: sub destReg, srcReg
//---------------------------------------------------------------

VOID StubLinkerCPU::X86EmitSubRegReg(X86Reg destReg, X86Reg srcReg)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION((int) destReg < NumX86Regs);
        PRECONDITION((int) srcReg < NumX86Regs);
    }
    CONTRACTL_END;

    Emit8(0x2b);
    Emit8(0xC0 | (srcReg << 3) | destReg);
}



//---------------------------------------------------------------
// Emits a MOD/RM for accessing a dword at [<indexreg> + ofs32]
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitOffsetModRM(BYTE opcode, X86Reg opcodereg, X86Reg indexreg, __int32 ofs)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    BYTE    codeBuffer[7];
    BYTE*   code    = codeBuffer;
    int     nBytes  = 0;
    code[0] = opcode;
    nBytes++;
    BYTE modrm = (opcodereg << 3) | indexreg;
    if (ofs == 0 && indexreg != kEBP)
    {
        code[1] = modrm;
        nBytes++;
        EmitBytes(codeBuffer, nBytes);
    }
    else if (FitsInI1(ofs))
    {
        code[1] = 0x40|modrm;
        code[2] = (BYTE)ofs;
        nBytes += 2;
        EmitBytes(codeBuffer, nBytes);
    }
    else
    {
        code[1] = 0x80|modrm;
        *((__int32*)(2+code)) = ofs;
        nBytes += 5;
        EmitBytes(codeBuffer, nBytes);
    }
}

//---------------------------------------------------------------
// Emits a MOD/RM for accessing a dword at [<baseReg> + <indexReg>*<scale> + ofs32]
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitOffsetModRmSIB(BYTE opcode, X86Reg opcodeOrReg, X86Reg baseReg, X86Reg indexReg, __int32 scale, __int32 ofs)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(scale == 1 || scale == 2 || scale == 4 || scale == 8);
        PRECONDITION(indexReg != kESP_Unsafe);
    }
    CONTRACTL_END;

    BYTE    codeBuffer[8];
    BYTE*   code    = codeBuffer;
    int     nBytes  = 0;

    code[0] = opcode;
    nBytes++;

    BYTE scaleEnc = 0;
    switch(scale) 
    {
        case 1: scaleEnc = 0; break;
        case 2: scaleEnc = 1; break;
        case 4: scaleEnc = 2; break;
        case 8: scaleEnc = 3; break;
        default: _ASSERTE(!"Unexpected");
    }

    BYTE sib = (scaleEnc << 6) | (indexReg << 3) | baseReg;

    if (FitsInI1(ofs))
    {
        code[1] = 0x44 | (opcodeOrReg << 3);
        code[2] = sib;
        code[3] = (BYTE)ofs;
        nBytes += 3;
        EmitBytes(codeBuffer, nBytes);
    }
    else
    {
        code[1] = 0x84 | (opcodeOrReg << 3);
        code[2] = sib;
        *(__int32*)(&code[3]) = ofs;
        nBytes += 6;
        EmitBytes(codeBuffer, nBytes);
    }
}



VOID StubLinkerCPU::X86EmitRegLoad(X86Reg reg, UINT_PTR imm)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (!imm)
    {
        X86EmitZeroOutReg(reg);
        return;
    }

    UINT cbimm = sizeof(void*);

    Emit8(0xB8 | (BYTE)reg);
    EmitBytes((BYTE*)&imm, cbimm);
}


//---------------------------------------------------------------
// Emits the most efficient form of the operation:
//
//    opcode   altreg, [basereg + scaledreg*scale + ofs]
//
// or
//
//    opcode   [basereg + scaledreg*scale + ofs], altreg
//
// (the opcode determines which comes first.)
//
//
// Limitations:
//
//    scale must be 0,1,2,4 or 8.
//    if scale == 0, scaledreg is ignored.
//    basereg and altreg may be equal to 4 (ESP) but scaledreg cannot
//    for some opcodes, "altreg" may actually select an operation
//      rather than a second register argument.
//    if basereg is EBP, scale must be 0.
//
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitOp(WORD    opcode,
                              X86Reg  altreg,
                              X86Reg  basereg,
                              __int32 ofs /*=0*/,
                              X86Reg  scaledreg /*=0*/,
                              BYTE    scale /*=0*/
                    AMD64_ARG(X86OperandSize OperandSize /*= k32BitOp*/))
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

        // All 2-byte opcodes start with 0x0f.
        PRECONDITION(!(opcode >> 8) || (opcode & 0xff) == 0x0f);

        PRECONDITION(scale == 0 || scale == 1 || scale == 2 || scale == 4 || scale == 8);
        PRECONDITION(scaledreg != (X86Reg)4);
        PRECONDITION(!(basereg == kEBP && scale != 0));

        PRECONDITION( ((UINT)basereg)   < NumX86Regs );
        PRECONDITION( ((UINT)scaledreg) < NumX86Regs );
        PRECONDITION( ((UINT)altreg)    < NumX86Regs );
    }
    CONTRACTL_END;


    BYTE modrmbyte = altreg << 3;
    BOOL fNeedSIB  = FALSE;
    BYTE SIBbyte = 0;
    BYTE ofssize;
    BYTE scaleselect= 0;

    if (ofs == 0 && basereg != kEBP)
    {
        ofssize = 0; // Don't change this constant!
    }
    else if (FitsInI1(ofs))
    {
        ofssize = 1; // Don't change this constant!
    }
    else
    {
        ofssize = 2; // Don't change this constant!
    }

    switch (scale)
    {
        case 1: scaleselect = 0; break;
        case 2: scaleselect = 1; break;
        case 4: scaleselect = 2; break;
        case 8: scaleselect = 3; break;
    }

    if (scale == 0 && basereg != (X86Reg)4 /*ESP*/)
    {
        // [basereg + ofs]
        modrmbyte |= basereg | (ofssize << 6);
    }
    else if (scale == 0)
    {
        // [esp + ofs]
        _ASSERTE(basereg == (X86Reg)4);
        fNeedSIB = TRUE;
        SIBbyte  = 0044;

        modrmbyte |= 4 | (ofssize << 6);
    }
    else
    {

        //[basereg + scaledreg*scale + ofs]

        modrmbyte |= 0004 | (ofssize << 6);
        fNeedSIB = TRUE;
        SIBbyte = ( scaleselect << 6 ) | (scaledreg << 3) | (basereg);

    }

    //Some sanity checks:
    _ASSERTE(!(fNeedSIB && basereg == kEBP)); // EBP not valid as a SIB base register.
    _ASSERTE(!( (!fNeedSIB) && basereg == (X86Reg)4 )) ; // ESP addressing requires SIB byte

    Emit8((BYTE)opcode);

    if (opcode >> 8)
        Emit8(opcode >> 8);

    Emit8(modrmbyte);
    if (fNeedSIB)
    {
        Emit8(SIBbyte);
    }
    switch (ofssize)
    {
        case 0: break;
        case 1: Emit8( (__int8)ofs ); break;
        case 2: Emit32( ofs ); break;
        default: _ASSERTE(!"Can't get here.");
    }
}


// Emits
//
//    opcode altreg, modrmreg
//
// or
//
//    opcode modrmreg, altreg
//
// (the opcode determines which one comes first)
//
// For single-operand opcodes, "altreg" actually selects
// an operation rather than a register.

VOID StubLinkerCPU::X86EmitR2ROp (WORD opcode,
                                  X86Reg altreg,
                                  X86Reg modrmreg
                        AMD64_ARG(X86OperandSize OperandSize /*= k64BitOp*/)
                                  )
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

        // All 2-byte opcodes start with 0x0f.
        PRECONDITION(!(opcode >> 8) || (opcode & 0xff) == 0x0f);

        PRECONDITION( ((UINT)altreg) < NumX86Regs );
        PRECONDITION( ((UINT)modrmreg) < NumX86Regs );
    }
    CONTRACTL_END;


    Emit8((BYTE)opcode);

    if (opcode >> 8)
        Emit8(opcode >> 8);

    Emit8(0300 | (altreg << 3) | modrmreg);
}


//---------------------------------------------------------------
// Emits:
//   op altreg, [esp+ofs]
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitEspOffset(BYTE opcode,
                                     X86Reg altreg,
                                     __int32 ofs
                           AMD64_ARG(X86OperandSize OperandSize /*= k64BitOp*/)
                                     )
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    BYTE    codeBuffer[8];
    BYTE   *code = codeBuffer;
    int     nBytes;

    {
        nBytes = 0;
    }

    code[0] = opcode;
    BYTE modrm = (altreg << 3) | 004;
    if (ofs == 0)
    {
        code[1] = modrm;
        code[2] = 0044;
        EmitBytes(codeBuffer, 3 + nBytes);
    }
    else if (FitsInI1(ofs))
    {
        code[1] = 0x40|modrm;
        code[2] = 0044;
        code[3] = (BYTE)ofs;
        EmitBytes(codeBuffer, 4 + nBytes);
    }
    else
    {
        code[1] = 0x80|modrm;
        code[2] = 0044;
        *((__int32*)(3+code)) = ofs;
        EmitBytes(codeBuffer, 7 + nBytes);
    }

}

//---------------------------------------------------------------

VOID StubLinkerCPU::X86EmitPushEBPframe()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    
    //  push ebp
    X86EmitPushReg(kEBP);
    // mov ebp,esp
    X86EmitMovRegSP(kEBP);
}

// popReturnAddress=true is used for popping off VCFrame::m_returnaddress
// since there will be no "ret" instruction

VOID StubLinkerCPU::X86EmitPopEBPframe(bool popReturnAddress)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
    
    //  mov esp, ebp
    X86EmitMovSPReg(kEBP);

    // pop ebp
    X86EmitPopReg(kEBP);

    if (popReturnAddress)
    {
        // add ESP, 4
        X86EmitAddEsp(sizeof(void*));
    }
}

//===========================================================================
// Emits code to capture the lasterror code.
VOID StubLinkerCPU::EmitSaveLastError()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // push eax (must save return value)
    X86EmitPushReg(kEAX);

    // call GetLastError
    X86EmitCall(NewExternalCodeLabel((LPVOID) GetLastError), 0);

    // mov [ebx + Thread.m_dwLastError], eax
    X86EmitIndexRegStore(kEBX, offsetof(Thread, m_dwLastError), kEAX);

    // pop eax (restore return value)
    X86EmitPopReg(kEAX);
}

#if defined(_DEBUG) && (defined(_AMD64_) || defined(_X86_))
void StubLinkerCPU::EmitJITHelperLoggingThunk(LPVOID pJitHelper, LPVOID helperFuncCount)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    VMHELPCOUNTDEF* pHelperFuncCount = (VMHELPCOUNTDEF*)helperFuncCount;
/*
        push        rcx
        mov         rcx, &(pHelperFuncCount->count)
   lock inc        [rcx]
        pop         rcx
#ifdef _AMD64_
        mov         rax, <pJitHelper>
        jmp         rax
#else
        jmp         <pJitHelper>
#endif
*/

    // push     rcx
    // mov      rcx, &(pHelperFuncCount->count)
    X86EmitPushReg(kECX);
    X86EmitRegLoad(kECX, (UINT_PTR)(&(pHelperFuncCount->count)));

    // lock inc [rcx]
    BYTE lock_inc_RCX[] = { 0xf0, 0xff, 0x01 };
    EmitBytes(lock_inc_RCX, sizeof(lock_inc_RCX));

    // pop      rcx
    // jmp      <pJitHelper>
    X86EmitTailcallWithSinglePop(NewExternalCodeLabel(pJitHelper), kECX);
}
#endif

//========================================================================
//  void StubLinkerCPU::EmitSEHProlog(LPVOID pvFrameHandler)
//  Prolog for setting up SEH for stubs that enter managed code from unmanaged
//  assumptions: esi has the current frame pointer
void StubLinkerCPU::EmitSEHProlog(LPVOID pvFrameHandler)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    // fixup for ComToManagedExRecord::m_tct and allocate SEH frame
    X86EmitSubEsp(sizeof(ThrowCallbackType) + sizeof(EXCEPTION_REGISTRATION_RECORD));

    // mov eax,esp
    X86EmitMovRegSP(kEAX);

    // save registers
    X86EmitPushReg(kEDX);
    X86EmitPushReg(kECX);

    // mov [eax]EXCEPTION_REGISTRATION_RECORD.Handler, pvFrameHandler
    X86EmitOffsetModRM(0xc7, (X86Reg)0, kEAX, offsetof(EXCEPTION_REGISTRATION_RECORD, Handler));
    Emit32((INT32)(size_t)pvFrameHandler);

    // mov [eax]EXCEPTION_REGISTRATION_RECORD.pvFilterParameter, eax
    X86EmitOffsetModRM(0x89, kEAX, kEAX, offsetof(EXCEPTION_REGISTRATION_RECORD, pvFilterParameter));

    // mov [eax]EXCEPTION_REGISTRATION_RECORD.dwFlags, PAL_EXCEPTION_FLAGS_UNWINDONLY
    X86EmitOffsetModRM(0xc7, (X86Reg)0, kEAX, offsetof(EXCEPTION_REGISTRATION_RECORD, dwFlags));
    Emit32(PAL_EXCEPTION_FLAGS_UNWINDONLY);

    // mov [edx]EXCEPTION_REGISTRATION_RECORD.typeOfHandler, PALExceptFilter
    X86EmitOffsetModRM(0xc7, (X86Reg)0, kEAX, offsetof(EXCEPTION_REGISTRATION_RECORD, typeOfHandler));
    Emit32((INT32)PALExceptFilter);

    // call PAL_TryHelper
    X86EmitPushReg(kEAX);
    X86EmitCall(NewExternalCodeLabel((LPVOID) PAL_TryHelper), sizeof(void*));

    // restore registers
    X86EmitPopReg(kECX);
    X86EmitPopReg(kEDX);

}


//===========================================================================
//  void StubLinkerCPU::EmitUnLinkSEH(unsigned offset)
//  offsetOfExceptionRegistrationRecord is the offset from the current frame 
//  where the next exception record  pointer is stored in the stack
//  for e.g. COM to managed frames the pointer to next SEH record is in the stack
//          after the ComMethodFrame::NegSpaceSize() + 4 ( address of handler)
//
//  also assumes ESI is pointing to the current frame

void StubLinkerCPU::EmitUnLinkSEH(unsigned offsetOfExceptionRegistrationRecord)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    // lea ecx,[esi+offsetOfExceptionRegistrationRecord]  ;;pointer to the exception record
    X86EmitOffsetModRM(0x8d, kECX, kESI, offsetOfExceptionRegistrationRecord);

    // save registers
    X86EmitPushReg(kEAX);
    X86EmitPushReg(kEDX);

    // call PAL_EndTryHelper(ecx, 0)
    X86EmitPushImm32(0);
    X86EmitPushReg(kECX);
    X86EmitCall(NewExternalCodeLabel((LPVOID) PAL_EndTryHelper), 2*sizeof(void*));

    // restore registers
    X86EmitPopReg(kEDX);
    X86EmitPopReg(kEAX);

}


//========================================================================
#ifdef _X86_
//========================================================================
//  Prolog for entering managed code from COM
//  pushes the appropriate frame ptr
//  sets up a thread and returns a label that needs to be emitted by the caller
//  At the end:
//  ESI will hold the pointer to the ComMethodFrame or UMThkCallFrame
//  EBX will hold the result of GetThread()
//  EDI will hold the previous Frame ptr
//  ESP will point to UnmanagedToManagedCallFrame::m_vc5Frame

void StubLinkerCPU::EmitComMethodStubProlog(TADDR pFrameVptr,
                                            CodeLabel** rgRareLabels,
                                            CodeLabel** rgRejoinLabels,
                                            LPVOID pSEHHandler,
                                            BOOL bShouldProfile)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(rgRareLabels != NULL);
        PRECONDITION(rgRareLabels[0] != NULL && rgRareLabels[1] != NULL && rgRareLabels[2] != NULL);
        PRECONDITION(rgRejoinLabels != NULL);
        PRECONDITION(rgRejoinLabels[0] != NULL && rgRejoinLabels[1] != NULL && rgRejoinLabels[2] != NULL);
        PRECONDITION(pFrameVptr == ComMethodFrame::GetMethodFrameVPtr() ||
                     pFrameVptr == UMThkCallFrame::GetUMThkCallFrameVPtr());
    }
    CONTRACTL_END;

    // push edx ;leave room for m_next (edx is an arbitrary choice)
    X86EmitPushReg(kEDX);

    // push IMM32 ; push Frame vptr
    X86EmitPushImmPtr((LPVOID) pFrameVptr);

    // push ebp     ;; save callee-saved register
    // push ebx     ;; save callee-saved register
    // push esi     ;; save callee-saved register
    // push edi     ;; save callee-saved register
    X86EmitPushReg(kEBP);
    X86EmitPushReg(kEBX);
    X86EmitPushReg(kESI);
    X86EmitPushReg(kEDI);

    // lea esi, [esp+0x10]  ;; set ESI -> new frame
    static const BYTE code10[] = {0x8d, 0x74, 0x24, sizeof(CalleeSavedRegisters) };
    EmitBytes(code10 ,sizeof(code10));

    //======================================================================
    // Set up just enough of a standard frame so that
    // the debugger can stacktrace through stubs.
    //======================================================================

    _ASSERTE(VC5FRAME_SIZE == (2 * sizeof(void*)));
    //  push [esi+Frame.retaddr]
    X86EmitIndexPush(kESI, UnmanagedToManagedFrame::GetOffsetOfReturnAddress());
    X86EmitPushEBPframe();

    // Emit Setup thread
    X86EmitSetup(rgRareLabels[0]);  // rareLabel for rare setup
    EmitLabel(rgRejoinLabels[0]); // rejoin label for rare setup

    // push auxilary information

    // xor eax, eax
    X86EmitZeroOutReg(kEAX);

    // push eax ;push NULL for protected Marshalers
    X86EmitPushReg(kEAX);

    // push eax ;push null for m_GCInfo
    _ASSERTE(sizeof(UnmanagedToManagedCallGCInfo) == sizeof(LPVOID));
    X86EmitPushReg(kEAX);

    // push eax ;push null for m_pArgs
    X86EmitPushReg(kEAX);

    // push eax ;push NULL for m_pReturnContext
    X86EmitPushReg(kEAX);

    // push eax ;push NULL for CleanupWorkList->m_pnode
    X86EmitPushReg(kEAX);

    X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());

    //-----------------------------------------------------------------------
    // Generate the inline part of disabling preemptive GC.  It is critical
    // that this part happen before we link in the frame.  That's because
    // we won't be able to unlink the frame from preemptive mode.  And during
    // shutdown, we cannot switch to cooperative mode under some circumstances
    //-----------------------------------------------------------------------
    EmitDisable(rgRareLabels[1], /*fCallIn=*/TRUE, kEBX); // rare disable gc
    EmitLabel(rgRejoinLabels[1]);                         // rejoin for rare disable gc

    // If we take an SO after installing the new frame but before getting the exception
    // handlers in place, we will have a corrupt frame stack.  So probe-by-touch first for 
    // sufficient stack space to erect the handler.  Because we know we will be touching
    // that stack right away when install the handler, this probe-by-touch will not incur
    // unnecessary cache misses.   And this allows us to do the probe with one instruction.
    
    // Note that for Win64, the personality routine will handle unlinking the frame, so
    // we don't need to probe in the Win64 stubs.  The exception is ComToCLRWorker
    // where we don't setup a personality routine.  However, we push the frame inside
    // that function and it is probe-protected with an entry point probe first, so we are
    // OK there too.
 
    // We push two registers to setup the EH handler and none to setup the frame
    // so probe for double that to give ourselves a small margin for error.
    // mov eax, [esp+n] ;; probe for sufficient stack to setup EH

     // mov edi,[ebx + Thread.GetFrame()]  ;; get previous frame
    X86EmitIndexRegLoad(kEDI, kEBX, Thread::GetOffsetOfCurrentFrame());

    // mov [esi + Frame.m_next], edi
    X86EmitIndexRegStore(kESI, Frame::GetOffsetOfNextLink(), kEDI);

    // mov [ebx + Thread.GetFrame()], esi
    X86EmitIndexRegStore(kEBX, Thread::GetOffsetOfCurrentFrame(), kESI);

#if _DEBUG
    if (Frame::ShouldLogTransitions())
    {
        // call LogTransition
        X86EmitPushReg(kESI);
        X86EmitCall(NewExternalCodeLabel((LPVOID) Frame::LogTransition), sizeof(void*));
    }
#endif

    if (pSEHHandler)
    {
        EmitSEHProlog(pSEHHandler);
    }

 #ifdef PROFILING_SUPPORTED
    // If profiling is active, emit code to notify profiler of transition
    // Must do this before preemptive GC is disabled, so no problem if the
    // profiler blocks.
    if (CORProfilerTrackTransitions() && bShouldProfile)
    {
        EmitProfilerComCallProlog(pFrameVptr, /*Frame*/ kESI);
    }
#endif // PROFILING_SUPPORTED

}

//========================================================================
//  void StubLinkerCPU::EmitEnterManagedStubEpilog(unsigned numStackBytes,
//                      CodeLabel** rgRareLabels, CodeLabel** rgRejoinLabels)
//  Epilog for stubs that enter managed code from unmanaged
//
//  On entry, ESI points to the Frame
//  ESP points to below UnmanagedToManagedCallFrame::m_vc5Frame
//  EBX hold GetThread()
//  EDI holds the previous Frame

void StubLinkerCPU::EmitEnterManagedStubEpilog(TADDR pFrameVptr, unsigned numStackBytes,
                        CodeLabel** rgRareLabel, CodeLabel** rgRejoinLabel,
                        BOOL bShouldProfile)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(rgRareLabel != NULL);
        PRECONDITION(rgRareLabel[0] != NULL && rgRareLabel[1] != NULL && rgRareLabel[2] != NULL);
        PRECONDITION(rgRejoinLabel != NULL);
        PRECONDITION(rgRejoinLabel[0] != NULL && rgRejoinLabel[1] != NULL && rgRejoinLabel[2] != NULL);
        PRECONDITION(pFrameVptr == ComMethodFrame::GetMethodFrameVPtr() ||
                     pFrameVptr == UMThkCallFrame::GetUMThkCallFrameVPtr());
    }
    CONTRACTL_END;

    // mov [ebx + Thread.GetFrame()], edi  ;; restore previous frame
    X86EmitIndexRegStore(kEBX, Thread::GetOffsetOfCurrentFrame(), kEDI);

    //-----------------------------------------------------------------------
    // Generate the inline part of disabling preemptive GC
    //-----------------------------------------------------------------------
    EmitEnable(rgRareLabel[2]); // rare gc
    EmitLabel(rgRejoinLabel[2]);        // rejoin for rare gc

#ifdef PROFILING_SUPPORTED

    // If profiling is active, emit code to notify profiler of transition
    if (CORProfilerTrackTransitions() && bShouldProfile)
    {
        EmitProfilerComCallEpilog(pFrameVptr, kESI);
    }
#endif // PROFILING_SUPPORTED

#if defined(_X86_)
    // Pop the VC5Frame
    X86EmitPopEBPframe(true);
#endif

    // pop edi        ; restore callee-saved registers
    // pop esi
    // pop ebx
    // pop ebp
    X86EmitPopReg(kEDI);
    X86EmitPopReg(kESI);
    X86EmitPopReg(kEBX);
    X86EmitPopReg(kEBP);

    // add esp, popstack     ; deallocate frame + MethodDesc
    unsigned popStack = sizeof(Frame) + sizeof(MethodDesc*);
    X86EmitAddEsp(popStack);

    // ESP is now pointing to m_ReturnAddress
    //    retn n
    X86EmitReturn(numStackBytes);

    //-----------------------------------------------------------------------
    // The out-of-line portion of enabling preemptive GC - rarely executed
    //-----------------------------------------------------------------------
    EmitLabel(rgRareLabel[2]);  // label for rare enable gc
    EmitRareEnable(rgRejoinLabel[2]); // emit rare enable gc

    //-----------------------------------------------------------------------
    // The out-of-line portion of disabling preemptive GC - rarely executed
    //-----------------------------------------------------------------------
    EmitLabel(rgRareLabel[1]);  // label for rare disable gc
    EmitRareDisable(rgRejoinLabel[1], /*bIsCallIn=*/TRUE); // emit rare disable gc

    //-----------------------------------------------------------------------
    // The out-of-line portion of setup thread - rarely executed
    //-----------------------------------------------------------------------
    EmitLabel(rgRareLabel[0]);  // label for rare setup thread
    EmitRareSetup(rgRejoinLabel[0], /*fThrow*/ TRUE); // emit rare setup thread
}

//========================================================================
#endif // _X86_
//========================================================================
//========================================================================
#ifdef _X86_
//========================================================================
//  Epilog for stubs that enter managed code from COM
//
//  At this point of the stub, the state should be as follows:
//  ESI holds the ComMethodFrame or UMThkCallFrame ptr
//  EBX holds the result of GetThread()
//  EDI holds the previous Frame ptr
//  ESP points to GSCookieFor<UnmanagedToManagedCallFrame::NegInfo>
//

void StubLinkerCPU::EmitComMethodStubEpilog(TADDR pFrameVptr,
                                            unsigned numStackBytes,
                                            CodeLabel** rgRareLabels,
                                            CodeLabel** rgRejoinLabels,
                                            LPVOID pSEHHandler,
                                            BOOL bShouldProfile)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(pFrameVptr == UMThkCallFrame::GetUMThkCallFrameVPtr());
    }
    CONTRACTL_END;

    EmitCheckGSCookie(kESI, UnmanagedToManagedCallFrame::GetOffsetOfGSCookie());
    
    if (!pSEHHandler)
    {
        unsigned sizeToPop = UnmanagedToManagedCallFrame::GetNegSpaceSize();
#ifdef _X86_
        sizeToPop += UnmanagedToManagedCallFrame::GetOffsetOfVC5Frame();
#endif
        X86EmitAddEsp(sizeToPop);
    }
    else
    {
        // oh well, if we are using exceptions, unlink the SEH and
        // just reset the esp to where EnterManagedStubEpilog likes it to be

        unsigned sizeOfEH = sizeof(EXCEPTION_REGISTRATION_RECORD);
        // fixup for ComToManagedExRecord::m_tct
        sizeOfEH += sizeof(ThrowCallbackType);

        unsigned offsetOfEH = 0 - (UnmanagedToManagedCallFrame::GetNegSpaceSize() + sizeOfEH);
        EmitUnLinkSEH(offsetOfEH);

        EmitCheckGSCookie(kESI, UnmanagedToManagedCallFrame::GetOffsetOfGSCookie());
    }

    EmitEnterManagedStubEpilog(pFrameVptr, numStackBytes,
                              rgRareLabels, rgRejoinLabels, bShouldProfile);
}

//========================================================================
#endif // _X86_
/*==============================================================================
    Pushes a FramedMethodFrame on the stack
    If you make any changes to the prolog instruction sequence, be sure
    to update UpdateRegdisplay, too!!  This service should only be called from
    within the runtime.  It should not be called for any unmanaged -> managed calls in.

    At the end of the generated prolog stub code:
    pFrame is in ESI/RSI.
    the previous pFrame is in EDI/RDI
    The current Thread* is in EBX/RBX.
    For x86, ESP points to FramedMethodFrame::NegInfo
    For amd64, ESP points to the space reserved for the outgoing argument registers
*/

VOID StubLinkerCPU::EmitMethodStubProlog(TADDR pFrameVptr)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;


    // push edx ;leave room for m_next (edx is an arbitrary choice)
    X86EmitPushReg(kEDX);

    // push Frame vptr
    X86EmitPushImmPtr((LPVOID) pFrameVptr);

#ifdef _X86_
    // push ebp     ;; save callee-saved register
    // push ebx     ;; save callee-saved register
    // push esi     ;; save callee-saved register
    // push edi     ;; save callee-saved register
    X86EmitPushReg(kEBP);
    X86EmitPushReg(kEBX);
    X86EmitPushReg(kESI);
    X86EmitPushReg(kEDI);
#endif

    // lea esi, [esp + 0x10]            ; load the Frame VPTR* into esi
    static const BYTE code10[] =       { 0x8d, 0x74, 0x24, 0x10 };
    EmitBytes(code10 ,sizeof(code10));

#ifdef _X86_
    //======================================================================
    // Set up just enough of a standard frame so that
    // a debugger can stacktrace through stubs.
    //======================================================================

    _ASSERTE(VC5FRAME_SIZE == (2 * sizeof(void*)));
    //  push dword ptr [esi+Frame.retaddr]
    X86EmitIndexPush(kESI, FramedMethodFrame::GetOffsetOfReturnAddress());
    X86EmitPushEBPframe();
    
    // Push & initialize ArgumentRegisters
#define DEFINE_ARGUMENT_REGISTER(regname) X86EmitPushReg(k##regname);
#include "eecallconv.h"

#else // _AMD64_

    // Save ArgumentRegisters
#define DEFINE_ARGUMENT_REGISTER(regname) X86EmitRegSave(k##regname, sizeof(TransitionFrame) + FIELD_OFFSET(ArgumentRegisters, regname));
#include "eecallconv.h"

    _ASSERTE(((Frame*)&pFrameVptr)->GetGSCookiePtr() == PTR_GSCookie(PBYTE(&pFrameVptr) - sizeof(GSCookie)));
    X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());

    // sub rsp, 4*sizeof(void*)           ;; allocate callee scratch area and ensure rsp is 16-byte-aligned
    size_t padding = (sizeof(FramedMethodFrame) % (2 * sizeof(LPVOID))) ? 0 : sizeof(LPVOID);
    X86EmitSubEsp(sizeof(ArgumentRegisters) + padding);
#endif // _AMD64_

    // ebx <-- GetThread()
    // Trashes X86TLSFetch_TRASHABLE_REGS
    X86EmitCurrentThreadFetch(0);

#if _DEBUG

    // call ObjectRefFlush
    X86EmitPushReg(kEBX);                   // arg on stack
    X86EmitCall(NewExternalCodeLabel((LPVOID) Thread::ObjectRefFlush), sizeof(void*));

#endif // _DEBUG

    // mov edi,[ebx + Thread.GetFrame()]    ;; get previous frame
    X86EmitIndexRegLoad(kEDI, kEBX, Thread::GetOffsetOfCurrentFrame());

    // mov [esi + Frame.m_next], edi
    X86EmitIndexRegStore(kESI, Frame::GetOffsetOfNextLink(), kEDI);

    // mov [ebx + Thread.GetFrame()], esi
    X86EmitIndexRegStore(kEBX, Thread::GetOffsetOfCurrentFrame(), kESI);

#if _DEBUG

    if (Frame::ShouldLogTransitions())
    {
        // call LogTransition
        X86EmitPushReg(kESI);                   // arg on stack

        X86EmitCall(NewExternalCodeLabel((LPVOID) Frame::LogTransition), sizeof(void*));

    }

#endif // _DEBUG

    // For x86, the patch label can be specified only after the GSCookie is pushed
    // Otherwise the debugger will see a Frame without a valid GSCookie

    RETURN;
}

/*==============================================================================
 EmitMethodStubEpilog generates the part of the stub that will pop off the
 Frame, except if popDatum==false or style==kInterceptorStubStyle
 
 restoreArgRegs - indicates whether the argument registers need to be
                  restored from m_argumentRegisters
 popDatum - indicates whether to pop m_datum or not.
 
 At this point of the stub:
    pFrame is in ESI/RSI.
    the previous pFrame is in EDI/RDI
    The current Thread* is in EBX/RBX.
    For x86, ESP points to the FramedMethodFrame::NegInfo
*/

VOID StubLinkerCPU::EmitMethodStubEpilog(__int16 numArgBytes, StubStyle style)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(style == kNoTripStubStyle ||
             style == kInterceptorStubStyle);       // the only ones this code knows about.
    }
    CONTRACTL_END;

    // mov [ebx + Thread.GetFrame()], edi  ;; restore previous frame
    X86EmitIndexRegStore(kEBX, Thread::GetOffsetOfCurrentFrame(), kEDI);

#ifdef _X86_
    X86EmitPopEBPframe(true);
#endif // _AMD64_

    // pop edi        ; restore callee-saved registers
    // pop esi
    // pop ebx
    // pop ebp
    X86EmitPopReg(kEDI);
    X86EmitPopReg(kESI);
    X86EmitPopReg(kEBX);
    X86EmitPopReg(kEBP);


#ifdef _X86_
    if (numArgBytes == ARG_SIZE_ON_STACK)
    {
        // This stub is called for methods with varying numbers of bytes on the
        // stack.  The correct number to pop is expected to now be sitting on
        // the stack.
        //
        // shift the retaddr & stored EDX:EAX return value down on the stack
        // and then toss the variable number of args pushed by the caller.
        // Of course, the slide must occur backwards.

        // add     esp,8                ; deallocate frame
        // pop     ecx                  ; scratch register gets delta to pop
        // push    eax                  ; running out of registers!
        // mov     eax, [esp+4]         ; get retaddr
        // mov     [esp+ecx+4], eax     ; put it where it belongs
        // pop     eax                  ; restore retval
        // add     esp, ecx             ; pop all the args
        // ret

        X86EmitAddEsp(sizeof(Frame));

        X86EmitPopReg(kECX);
        X86EmitPushReg(kEAX);

        static const BYTE arbCode1[] =
        {
                  0x8b, 0x44, 0x24, 0x04, // mov eax, [esp+4]
                  0x89, 0x44, 0x0c, 0x04, // mov [esp+ecx+4], eax
        };

        EmitBytes(arbCode1, sizeof(arbCode1));
        X86EmitPopReg(kEAX);

        static const BYTE arbCode2[] = {
            0x03, 0xe1,                   // add esp, ecx
            0xc3,                         // ret
        };

        EmitBytes(arbCode2, sizeof(arbCode2));
    }
    else
#endif // _X86_
    {
        _ASSERTE(numArgBytes >= 0);


        if (style == kInterceptorStubStyle)
        {
            // add esp,8      ; deallocate frame
            X86EmitAddEsp(sizeof(Frame));

            // Restore MethodDesc*
            X86EmitPopReg(kEAX);
        }
        else
        {
            // add esp,12     ; deallocate frame + MethodDesc
            X86EmitAddEsp(sizeof(Frame) + sizeof(MethodDesc*));
        }

        if (style != kInterceptorStubStyle)
        {
            X86EmitReturn(numArgBytes);
        }
    }
}


VOID StubLinkerCPU::EmitRareSetup(CodeLabel *pRejoinPoint, BOOL fThrow)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    _ASSERTE(fThrow);
    X86EmitCall(NewExternalCodeLabel((LPVOID) CreateThreadBlockThrow), 0);

    // mov ebx,eax
    Emit16(0xc389);
    X86EmitNearJump(pRejoinPoint);
}



// On entry, ESI should be pointing to the Frame

VOID StubLinkerCPU::EmitCheckGSCookie(X86Reg frameReg, int gsCookieOffset)
{
#ifdef _DEBUG
    // cmp dword ptr[frameReg-gsCookieOffset], gsCookie
#ifdef _X86_
    X86EmitCmpRegIndexImm32(frameReg, gsCookieOffset, GetProcessGSCookie());
#else
    X64EmitCmp32RegIndexImm32(frameReg, gsCookieOffset, (INT32)GetProcessGSCookie());
#endif
    
    CodeLabel * pLabel = NewCodeLabel();
    X86EmitCondJump(pLabel, X86CondCode::kJE);
    
    X86EmitCall(NewExternalCodeLabel((LPVOID) JIT_FailFast), 0);

    EmitLabel(pLabel);
#endif
}

//---------------------------------------------------------------
// Emit code to store the setup current Thread structure in eax.
// TRASHES  eax,ecx&edx.
// RESULTS  ebx = current Thread
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitSetup(CodeLabel *pForwardRef)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    DWORD idx = GetThreadTLSIndex();
    TLSACCESSMODE mode = GetTLSAccessMode(idx);

#ifdef _DEBUG
    {
        static BOOL f = TRUE;
        f = !f;
        if (f)
        {
           mode = TLSACCESS_GENERIC;
        }
    }
#endif

    switch (mode)
    {

        case TLSACCESS_GENERIC:
            X86EmitPushImm32(idx);

            // call TLSGetValue
            X86EmitCall(NewExternalCodeLabel((LPVOID) TlsGetValue), sizeof(void*));
            // mov ebx,eax
            Emit16(0xc389);
            break;
        default:
            _ASSERTE(0);
    }

    // cmp ebx, 0
    static const BYTE b[] = { 0x83, 0xFB, 0x0};

    EmitBytes(b, sizeof(b));

    // jz RarePath
    X86EmitCondJump(pForwardRef, X86CondCode::kJZ);

#ifdef _DEBUG
    X86EmitDebugTrashReg(kECX);
    X86EmitDebugTrashReg(kEDX);
#endif

}


// This method unboxes the THIS pointer and then calls pRealMD
// If it's shared code for a method in a generic value class, then also extract the vtable pointer
// and pass it as an extra argument.  Thus this stub generator really covers both
//   - Unboxing, non-instantiating stubs
//   - Unboxing, method-table-instantiating stubs
VOID StubLinkerCPU::EmitUnboxMethodStub(MethodDesc* pUnboxMD)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(!pUnboxMD->IsStatic());
    }
    CONTRACT_END;

    if (pUnboxMD->RequiresInstMethodTableArg())
    {
        int regnum = -1;

        SigTypeContext typeContext(pUnboxMD, TypeHandle());

        Module*         pModule     = pUnboxMD->GetModule();
        PCCOR_SIGNATURE szMetaSig;
        DWORD           cbMetaSigSize;

        pUnboxMD->GetSig(&szMetaSig, &cbMetaSigSize);

        BOOL            fIsStatic   = pUnboxMD->IsStatic();
            
        AMD64_ONLY( UINT cbStack = )
            MetaSig::SizeOfActualFixedArgStack(pModule, szMetaSig, cbMetaSigSize, fIsStatic, &typeContext, TRUE, &regnum);
        
      // The extra parameter is passed on the stack, so replace return address by return address + vtable pointer from this object
      //@nice GENERICS: this x86 code sequence probably isn't optimal so an expert might want to tweak it
      if (regnum == -1)
      {
          // Pop return address into AX
          X86EmitPopReg(kEAX);
  
          // Put it back with a copy
          X86EmitPushReg(kEDX);  // this is just a placeholder - value doesn't matter
          X86EmitPushReg(kEAX);  // repush return address
  
          // Extract the vtable pointer from "this"
          X86EmitIndexRegLoad(kEAX, kECX, 0);
  
          // Move it into place above the return address
          X86EmitEspOffset(0x89, kEAX, 4);   // mov [esp+4], eax
  
      }
      // It's in a register; can only be DX because CX is always used for "this" pointer
      else
      {
          _ASSERTE(regnum == 1);

          // Just extract the vtable pointer from "this"
          X86EmitIndexRegLoad(kEDX, kECX, 0);
      }
    }

    // unboxing a value class simply means adding sizeof(void*) to the THIS pointer
    X86Reg thisReg = THIS_kREG;


    X86EmitAddReg(thisReg, sizeof(void*));

    Emit16(X86_INSTR_MOV_EAX_IND);                  // mov eax, dword ptr[]
    EmitPtr(pUnboxMD->GetAddrOfSlot());
    Emit16(X86_INSTR_JMP_EAX);                          // JMP EAX
    RETURN;
}


#if defined(FEATURE_SHARE_GENERIC_CODE)  
// The stub generated by this method passes an extra dictionary argument before jumping to 
// shared-instantiation generic code.
//
// pSharedMD is either
//    * An InstantiatedMethodDesc for a generic method whose code is shared across instantiations.
//      In this case, the extra argument is the InstantiatedMethodDesc for the instantiation-specific stub itself.
// or * A MethodDesc for a static method in a generic class whose code is shared across instantiations.
//      In this case, the extra argument is the MethodTable pointer of the instantiated type.
VOID StubLinkerCPU::EmitInstantiatingMethodStub(MethodDesc* pSharedMD, void* extra)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(pSharedMD->RequiresInstMethodTableArg() || pSharedMD->RequiresInstMethodDescArg());
    }
    CONTRACTL_END;

    int regnum = -1;

    SigTypeContext typeContext(pSharedMD, TypeHandle());

    Module*         pModule     = pSharedMD->GetModule();
    PCCOR_SIGNATURE szMetaSig;
    DWORD           cbMetaSigSize;

    pSharedMD->GetSig(&szMetaSig, &cbMetaSigSize);

    BOOL            fIsStatic   = pSharedMD->IsStatic();
        
    AMD64_ONLY( UINT_PTR cbStack = )
        (UINT_PTR)MetaSig::SizeOfActualFixedArgStack(pModule, szMetaSig, cbMetaSigSize, fIsStatic, &typeContext, TRUE, &regnum);


    // It's on the stack
    if (regnum == -1)
    {
        // Pop return address into AX
        X86EmitPopReg(kEAX);

        // Push extra dictionary argument
        X86EmitPushImmPtr(extra);

        // Put return address back
        X86EmitPushReg(kEAX);
    }
    // It's in a register
    else
    {
        X86EmitPushImmPtr(extra);
        if (regnum == 0)
            X86EmitPopReg(kECX);
        else
            X86EmitPopReg(kEDX);
    }

    // If it is an ECall, m_CodeOrIL does not reflect the correct address to
    // call to (which is an ECall stub).  Rather, it reflects the actual ECall
    // implementation.  Naturally, ECalls must always hit the stub first.
    // Along the same lines, perhaps the method isn't JITted yet.  The easiest
    // way to handle all this is to simply dispatch through the top of the MD.

    X86_64BitOperands();
    Emit16(X86_INSTR_MOV_EAX_IND);              // mov eax, dword ptr[]
    EmitPtr(pSharedMD->GetAddrOfSlot());
    Emit16(X86_INSTR_JMP_EAX);                  // JMP EAX

}
#endif // FEATURE_SHARE_GENERIC_CODE


//
// SecurityWrapper
//
// Wraps a real stub do some security work before the stub and clean up after. Before the
// real stub is called a security frame is added and declarative checks are performed. The
// Frame is added so declarative asserts and denies can be maintained for the duration of the
// real call. At the end the frame is simply removed and the wrapper cleans up the stack. The
// registers are restored.
//
LPVOID StubLinkerCPU::EmitSecurityWrapperStub(
        UINT numStackBytes, 
        MethodDesc* pMD, 
        BOOL fToStub, 
        LPVOID pRealStub, 
        DeclActionInfo *pActions)
{
    CONTRACT(LPVOID)
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;


    // EAX contains MethodDesc* from the precode. Push it here
    // to initialize m_Datum field of the frame.
    X86EmitPushReg(kEAX);

    EmitMethodStubProlog(InterceptorFrame::GetMethodFrameVPtr());

    UINT32 negSpaceSize = InterceptorFrame::GetRawNegSpaceSize() -
                                          FramedMethodFrame::GetRawNegSpaceSize();

    // See NOTE on Frame Size C_ASSERT usage in this file 
    C_ASSERT(sizeof(InterceptorFrame::NegInfo)  - sizeof(SecurityFrame::NegInfo) == 0);
    C_ASSERT(sizeof(SecurityFrame::NegInfo)  - sizeof(FramedMethodFrame::NegInfo) == 4);

    // the runtime computation better be the same as the compile time computation....
    _ASSERTE(negSpaceSize == 4);

    // make room for negspace fields of IFrame
    X86EmitPushImm32(0);    // m_securityData should be null as it has an object reference in it
    
    X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());
    negSpaceSize += sizeof(GSCookie);
    
    // Frame is ready to be inspected by debugger for patch location
    EmitPatchLabel();

    if (pActions->dwDeclAction == DECLSEC_UNMNGD_ACCESS_DEMAND && pActions->pNext == NULL)
    {
        X86EmitCall(NewExternalCodeLabel((LPVOID) DoSpecialUnmanagedCodeDemand), 0);
    }
    else
    {
        // push arguments for DoDeclaritiveSecurity(MethodDesc*, DeclActionInfo*, InterceptorFrame*);
        X86EmitPushReg(kESI);            // push esi (push new frame as ARG)

        X86EmitPushImmPtr(pActions);

        X86EmitPushImmPtr(pMD);

        X86EmitCall(NewExternalCodeLabel((LPVOID) DoDeclarativeSecurity), 3*sizeof(void*));
    }

    // Copy the arguments, calculate the offset
    //     terminology:  opt.          - Optional
    //                   Sec. Desc.    - Security Descriptor
    //                   desc          - Descriptor
    //                   rtn           - Return
    //                   addr          - Address
    //
    //          method desc    <-- copied from below
    //         ------------
    //           rtn addr      <-- points back into the wrapper stub
    //         ------------
    //            copied       <-- copied from below
    //             args
    //         ------------   -|
    //          Sec. Desc      |
    //         ------------   -|
    //          Arg. Registers |
    //         ------------   -|
    //          S. Stack Ptr   |
    //         ------------   -|
    //             EBP (opt.)  |
    //             EAX (opt.)  |
    //         ------------    |- Security Negitive Space (for frame)
    //             EDI         |
    //             ESI         |
    //             EBX         |
    //             EBP         |
    //         ------------ -----                -
    //            vtable                         |
    //         ------------                      |
    //            next                           |-- Security Frame
    //         ------------   --                 |
    //          method desc    |                 |
    //         ------------    |                 |
    //           rtn addr <-|  |--Original Stack |
    //         ------------ |  |                 -
    //         | original | |  :
    //         |  args    | |
    //                      ------- Points to the real return addr.
    //
    //
    //

    // Offset from original args to new args. (see above). We are copying from
    // the bottom of the stack to top. The offset is calculated to repush the
    // the arguments on the stack.
    //
    // offset = Negitive Space + return addr + method des + next + vtable + size of args - 4
    // The 4 is subtracted because the esp is one slot below the start of the copied args.
    UINT32 offset = InterceptorFrame::GetNegSpaceSize() + sizeof(InterceptorFrame) - 4 + numStackBytes;

    // Convert bytes to number of slots
    int args  = numStackBytes >> 2;

    // Emit the required number of pushes to copy the arguments
    while (args)
    {
        X86EmitSPIndexPush(offset);
        args--;
    }

    // Add a jmp to the main call, this adds our current EIP+4 to the stack
    CodeLabel* mainCall;
    mainCall = NewCodeLabel();
    X86EmitCall(mainCall, 0);

    // Jump past the call into the real stub we have already been there
    // The return addr into the stub points to this jump statement
    //
    CodeLabel* continueCall;
    continueCall = NewCodeLabel();
    X86EmitNearJump(continueCall);

    // Main Call label attached to the real stubs call
    EmitLabel(mainCall);

    // push the address of the method descriptor for the interpreted case only
    //  push dword ptr [esp+offset] and add four bytes for that case
    if (fToStub)
    {
        X86EmitSPIndexPush(offset);

        // Restore MethodDesc* for the real stub
        X86EmitPopReg(kEAX);
    }

    // Set up for arguments in stack, offset is 8 bytes below base of frame.
    // Call GetOffsetOfArgumentRegisters to move back from base of frame
    offset = offset - 8 + InterceptorFrame::GetOffsetOfArgumentRegisters();

    // Move to the last register in the space used for registers
    offset += NUM_ARGUMENT_REGISTERS * sizeof(UINT32) - 4;

    // Push the args on the stack, as esp is incremented and the
    // offset stays the same all the register values are pushed on
    // the correct registers
    for(int i = 0; i < NUM_ARGUMENT_REGISTERS; i++)
        X86EmitSPIndexPush(offset);

    // This generates the appropriate pops into the registers specified in eecallconv.h
#define DEFINE_ARGUMENT_REGISTER_BACKWARD(regname) X86EmitPopReg(k##regname);
#include "eecallconv.h"

    // Add a jump to the real stub, we will return to
    // the jump statement added  above
    X86EmitNearJump(NewExternalCodeLabel(pRealStub));

    // we will continue on past the real stub
    EmitLabel(continueCall);

    EmitCheckGSCookie(kESI, 0 + InterceptorFrame::GetOffsetOfGSCookie());

    // deallocate negspace fields of InterceptorFrame
    X86EmitAddEsp(negSpaceSize);

    // Return poping of the same number of bytes that
    // the real stub would have popped.
    EmitMethodStubEpilog(numStackBytes, kNoTripStubStyle);

    RETURN pRealStub;
}


//
// Security Filter, if no security frame is required because there are no declarative asserts or denies
// then the arguments do not have to be copied. This interceptor creates a temporary Security frame
// calls the declarative security return, cleans up the stack to the same state when the inteceptor
// was called and jumps into the real routine.
//
LPVOID StubLinkerCPU::EmitSecurityInterceptorStub(MethodDesc* pMD, BOOL fToStub, LPVOID pRealStub, DeclActionInfo *pActions)
{
    CONTRACT(LPVOID)
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;


    // EAX contains MethodDesc* from the precode. Push it here
    // to initialize m_Datum field of the frame.
    X86EmitPushReg(kEAX);

    if (pMD->IsComPlusCall())
    {
        // Generate label where non-remoting code will start executing
        CodeLabel *pPrologStart = NewCodeLabel();

        // mov eax, [ecx]
        X86EmitIndexRegLoad(kEAX, kECX, 0);

        // cmp eax, CTPMethodTable::s_pThunkTable
        Emit8(0x3b);
        Emit8(0x05);
        void* p = (void*) CTPMethodTable::GetMethodTableAddr();
        EmitPtr(p);

        // jne pPrologStart
        X86EmitCondJump(pPrologStart, X86CondCode::kJNE);

        // Restore MethodDesc* for the real stub
        X86EmitPopReg(kEAX);

        // Add a jump to the real stub thus bypassing the security stack walk
        X86EmitNearJump(NewExternalCodeLabel(pRealStub));

        // emit label for non remoting case
        EmitLabel(pPrologStart);
    }

    EmitMethodStubProlog(InterceptorFrame::GetMethodFrameVPtr());

    // See NOTE on Frame Size C_ASSERT usage in this file 
    C_ASSERT(sizeof(InterceptorFrame::NegInfo)  - sizeof(SecurityFrame::NegInfo) == 0);
    C_ASSERT(sizeof(SecurityFrame::NegInfo)  - sizeof(FramedMethodFrame::NegInfo) == 4);

    // Reserve space for m_securityState
    X86EmitPushImm32(0);    // m_securityData should be null as it has an object reference in it

    // gsCookie
    X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());

    // Frame is ready to be inspected by debugger for patch location
    EmitPatchLabel();

    if (pActions->dwDeclAction == DECLSEC_UNMNGD_ACCESS_DEMAND && pActions->pNext == NULL)
    {
        X86EmitCall(NewExternalCodeLabel((LPVOID) DoSpecialUnmanagedCodeDemand), 0);
    }
    else
    {
        // push method descriptor for DoDeclaritiveSecurity(MethodDesc*, InterceptorFrame*);
        X86EmitPushReg(kESI);            // push esi (push new frame as ARG)
        X86EmitPushImmPtr(pActions);
        X86EmitPushImmPtr(pMD);
        
        X86EmitCall(NewExternalCodeLabel((LPVOID) DoDeclarativeSecurity), 3*sizeof(void*));
    }

    // Get the number arguments stored in registers. For now we are doing the
    // simple approach of saving off all the registers and then poping them off.
    // This needs to be cleaned up for the real stubs or when CallDescr is done
    // correctly

    // Prototype for: push dword ptr[esp + offset], The last number will be the offset
    // At this point the esp should be pointing at top of the Interceptor frame
    UINT32 offset = InterceptorFrame::GetNegSpaceSize() + 
                    InterceptorFrame::GetOffsetOfArgumentRegisters() - 
                    sizeof(void*);

    offset += NUM_ARGUMENT_REGISTERS * sizeof(void*);

    // Push the args on the stack and then pop them into
    // the correct registers
    for(int i = 0; i < NUM_ARGUMENT_REGISTERS; i++)
        X86EmitSPIndexPush(offset);

    // This generates the appropriate pops into the registers specified in eecallconv.h
#define DEFINE_ARGUMENT_REGISTER_BACKWARD(regname) X86EmitPopReg(k##regname);
#include "eecallconv.h"

    EmitCheckGSCookie(kESI, InterceptorFrame::GetOffsetOfGSCookie());
    // Clean up security frame, this rips off the MD and gets the real return addr at the top of the stack
    X86EmitAddEsp(InterceptorFrame::GetNegSpaceSize() - FramedMethodFrame::GetRawNegSpaceSize());
    EmitMethodStubEpilog(0, kInterceptorStubStyle);

    // Add a jump to the real stub
    X86EmitNearJump(NewExternalCodeLabel(pRealStub));

    RETURN pRealStub;
}



#ifdef _DEBUG
//---------------------------------------------------------------
// Emits:
//     mov <reg32>,0xcccccccc
//---------------------------------------------------------------
VOID StubLinkerCPU::X86EmitDebugTrashReg(X86Reg reg)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    Emit8(0xb8|reg);
    Emit32(0xcccccccc);
}
#endif //_DEBUG


#if defined(_DEBUG) && defined(STUBLINKER_GENERATES_UNWIND_INFO)

typedef BOOL GetModuleInformationProc(
  HANDLE hProcess,
  HMODULE hModule,
  LPMODULEINFO lpmodinfo,
  DWORD cb
);

GetModuleInformationProc *g_pfnGetModuleInformation = NULL;

extern "C" VOID __cdecl DebugCheckStubUnwindInfoWorker (CONTEXT *pStubContext)
{
    BEGIN_ENTRYPOINT_VOIDRET;

    LOG((LF_STUBS, LL_INFO1000000, "checking stub unwind info:\n"));

    //
    // Make a copy of the CONTEXT.  RtlVirtualUnwind will modify this copy.
    // DebugCheckStubUnwindInfo will need to restore registers from the
    // original CONTEXT.
    //
    CONTEXT ctx = *pStubContext;
    ctx.ContextFlags = (CONTEXT_CONTROL | CONTEXT_INTEGER);

    //
    // Find the upper bound of the stack and address range of KERNEL32.  This
    // is where we expect the unwind to stop.
    //
    void *pvStackTop = GetThread()->GetCachedStackBase();

    if (!g_pfnGetModuleInformation)
    {
        HMODULE hmodPSAPI = WszGetModuleHandle(L"PSAPI.DLL");

        if (!hmodPSAPI)
        {
            hmodPSAPI = WszLoadLibrary(L"PSAPI.DLL");
            if (!hmodPSAPI)
            {
                _ASSERTE(!"unable to load PSAPI.DLL");
                goto ErrExit;
            }
        }

        g_pfnGetModuleInformation = (GetModuleInformationProc*)GetProcAddress(hmodPSAPI, "GetModuleInformation");
        if (!g_pfnGetModuleInformation)
        {
            _ASSERTE("can't find PSAPI!GetModuleInformation");
            goto ErrExit;
        }

        // Intentionally leak hmodPSAPI.  We don't want to
        // LoadLibrary/FreeLibrary every time, this is slow + produces lots of
        // debugger spew.  This is just debugging code after all...
    }

    HMODULE hmodKERNEL32 = WszGetModuleHandle(L"KERNEL32");
    _ASSERTE(hmodKERNEL32);

    MODULEINFO modinfoKERNEL32;
    if (!g_pfnGetModuleInformation(GetCurrentProcess(), hmodKERNEL32, &modinfoKERNEL32, sizeof(modinfoKERNEL32)))
    {
        _ASSERTE(!"unable to get bounds of KERNEL32");
        goto ErrExit;
    }

    //
    // Unwind until IP is 0, sp is at the stack top, and callee IP is in kernel32.
    //

    for (;;)
    {
        ULONG64 ControlPc = (ULONG64)GetIP(&ctx);

        LOG((LF_STUBS, LL_INFO1000000, "pc %p, sp %p\n", ControlPc, GetSP(&ctx)));

        ULONG64 ImageBase;
        RUNTIME_FUNCTION *pFunctionEntry = RtlLookupFunctionEntry(
                ControlPc,
                &ImageBase,
                NULL);
        if (pFunctionEntry)
        {
            PVOID HandlerData;
            ULONG64 EstablisherFrame;

            RtlVirtualUnwind(
                    0,
                    ImageBase,
                    ControlPc,
                    pFunctionEntry,
                    &ctx,
                    &HandlerData,
                    &EstablisherFrame,
                    NULL);

            ULONG64 NewControlPc = (ULONG64)GetIP(&ctx);

            LOG((LF_STUBS, LL_INFO1000000, "function %p, image %p, new pc %p, new sp %p\n", pFunctionEntry, ImageBase, NewControlPc, GetSP(&ctx)));

            if (!NewControlPc)
            {
                if (GetSP(&ctx) < (BYTE*)pvStackTop - 0x100)
                {
                    _ASSERTE(!"SP did not end up at top of stack");
                    goto ErrExit;
                }

                if (!(   ControlPc > (ULONG64)modinfoKERNEL32.lpBaseOfDll
                      && ControlPc < (ULONG64)modinfoKERNEL32.lpBaseOfDll + modinfoKERNEL32.SizeOfImage))
                {
                    _ASSERTE(!"PC did not end up in KERNEL32");
                    goto ErrExit;
                }

                break;
            }
        }
        else
        {
            // Nested functions that do not use any stack space or nonvolatile
            // registers are not required to have unwind info (ex.
            // USER32!ZwUserCreateWindowEx).
            ctx.Rip = *(ULONG64*)(ctx.Rsp);
            ctx.Rsp += sizeof(ULONG64);
        }
    }
ErrExit:
    
    END_ENTRYPOINT_VOIDRET;
    return;
}

//virtual
VOID StubLinkerCPU::EmitUnwindInfoCheckWorker (CodeLabel *pCheckLabel)
{
    X86EmitCall(pCheckLabel, 0);
}

//virtual
VOID StubLinkerCPU::EmitUnwindInfoCheckSubfunction()
{

    X86EmitNearJump(NewExternalCodeLabel(DebugCheckStubUnwindInfo));
}

#endif // defined(_DEBUG) && defined(STUBLINKER_GENERATES_UNWIND_INFO)


//-----------------------------------------------------------------------
// Generates the inline portion of the code to enable preemptive GC. Hopefully,
// the inline code is all that will execute most of the time. If this code
// path is entered at certain times, however, it will need to jump out to
// a separate out-of-line path which is more expensive. The "pForwardRef"
// label indicates the start of the out-of-line path.
//
// Assumptions:
//      ebx = Thread
// Preserves
//      all registers except ecx.
//
//-----------------------------------------------------------------------
VOID StubLinkerCPU::EmitEnable(CodeLabel *pForwardRef)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(4 == sizeof( ((Thread*)0)->m_State ));
        PRECONDITION(4 == sizeof( ((Thread*)0)->m_fPreemptiveGCDisabled ));
    }
    CONTRACTL_END;

    // move byte ptr [ebx + Thread.m_fPreemptiveGCDisabled],0
    X86EmitOffsetModRM(0xc6, (X86Reg)0, kEBX, Thread::GetOffsetOfGCFlag());
    Emit8(0);

    _ASSERTE(FitsInI1(Thread::TS_CatchAtSafePoint));

    // test byte ptr [ebx + Thread.m_State], TS_CatchAtSafePoint
    X86EmitOffsetModRM(0xf6, (X86Reg)0, kEBX, Thread::GetOffsetOfState());
    Emit8(Thread::TS_CatchAtSafePoint);

    // jnz RarePath
    X86EmitCondJump(pForwardRef, X86CondCode::kJNZ);

#ifdef _DEBUG
    X86EmitDebugTrashReg(kECX);
#endif

}


//-----------------------------------------------------------------------
// Generates the out-of-line portion of the code to enable preemptive GC.
// After the work is done, the code jumps back to the "pRejoinPoint"
// which should be emitted right after the inline part is generated.
//
// Assumptions:
//      ebx = Thread
// Preserves
//      all registers except ecx.
//
//-----------------------------------------------------------------------
VOID StubLinkerCPU::EmitRareEnable(CodeLabel *pRejoinPoint)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

#ifdef _X86_
    X86EmitCall(NewExternalCodeLabel((LPVOID) StubRareEnable), 0);
#ifdef _DEBUG
    X86EmitDebugTrashReg(kECX);
#endif
    if (pRejoinPoint)
    {
        X86EmitNearJump(pRejoinPoint);
    }
#else // _X86_
    _ASSERTE(!"NYI amd64");
#endif // _X86_

}


//-----------------------------------------------------------------------
// Generates the inline portion of the code to disable preemptive GC. Hopefully,
// the inline code is all that will execute most of the time. If this code
// path is entered at certain times, however, it will need to jump out to
// a separate out-of-line path which is more expensive. The "pForwardRef"
// label indicates the start of the out-of-line path.
//
// Assumptions:
//      ebx = Thread
// Preserves
//      all registers except ecx.
//
//-----------------------------------------------------------------------
VOID StubLinkerCPU::EmitDisable(CodeLabel *pForwardRef, BOOL fCallIn, X86Reg ThreadReg)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(4 == sizeof( ((Thread*)0)->m_fPreemptiveGCDisabled ));
        PRECONDITION(4 == sizeof(g_TrapReturningThreads));
    }
    CONTRACTL_END;



    // move byte ptr [ebx + Thread.m_fPreemptiveGCDisabled],1
    X86EmitOffsetModRM(0xc6, (X86Reg)0, ThreadReg, Thread::GetOffsetOfGCFlag());
    Emit8(1);

    // cmp dword ptr g_TrapReturningThreads, 0
    Emit16(0x3d83);
    EmitPtr((void *)&g_TrapReturningThreads);
    Emit8(0);

    // jnz RarePath
    X86EmitCondJump(pForwardRef, X86CondCode::kJNZ);


#ifdef _DEBUG
    if (ThreadReg != kECX)
        X86EmitDebugTrashReg(kECX);
#endif

}


//-----------------------------------------------------------------------
// Generates the out-of-line portion of the code to disable preemptive GC.
// After the work is done, the code jumps back to the "pRejoinPoint"
// which should be emitted right after the inline part is generated.  However,
// if we cannot execute managed code at this time, an exception is thrown
// which cannot be caught by managed code.
//
// Assumptions:
//      ebx = Thread
// Preserves
//      all registers except ecx, eax.
//
//-----------------------------------------------------------------------
VOID StubLinkerCPU::EmitRareDisable(CodeLabel *pRejoinPoint, BOOL bIsCallIn)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

#ifdef _X86_
    if (bIsCallIn)
        X86EmitCall(NewExternalCodeLabel((LPVOID) StubRareDisableTHROW), 0);
    else
        X86EmitCall(NewExternalCodeLabel((LPVOID) StubRareDisableRETURN), 0);

#ifdef _DEBUG
    X86EmitDebugTrashReg(kECX);
#endif
    X86EmitNearJump(pRejoinPoint);
#else
    _ASSERTE(!"nyi amd64");
#endif

}


//-----------------------------------------------------------------------
// Generates the out-of-line portion of the code to disable preemptive GC.
// After the work is done, the code normally jumps back to the "pRejoinPoint"
// which should be emitted right after the inline part is generated.  However,
// if we cannot execute managed code at this time, an HRESULT is returned
// via the ExitPoint.
//
// Assumptions:
//      ebx = Thread
// Preserves
//      all registers except ecx, eax.
//
//-----------------------------------------------------------------------
VOID StubLinkerCPU::EmitRareDisableHRESULT(CodeLabel *pRejoinPoint, CodeLabel *pExitPoint)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;
#ifdef _X86_
    X86EmitCall(NewExternalCodeLabel((LPVOID) StubRareDisableHR), 0);

#ifdef _DEBUG
    X86EmitDebugTrashReg(kECX);
#endif

    // test eax, eax  ;; test the result of StubRareDisableHR
    Emit16(0xc085);

    // JZ pRejoinPoint
    X86EmitCondJump(pRejoinPoint, X86CondCode::kJZ);

    X86EmitNearJump(pExitPoint);
#else
    _ASSERTE(!"nyi amd64");
#endif // _X86_

}


// Get X86Reg indexes of argument registers based on offset into ArgumentRegister
X86Reg GetX86ArgumentRegisterFromOffset(size_t ofs)
{
    CONTRACT(X86Reg)
    {
        NOTHROW;
        GC_NOTRIGGER;

    }
    CONTRACT_END;
#define DEFINE_ARGUMENT_REGISTER(reg) if (ofs == offsetof(ArgumentRegisters, reg)) RETURN  k##reg ;
#include "eecallconv.h"
    _ASSERTE(0);//Can't get here.
    RETURN kEBP;
}


VOID StubLinkerCPU::EmitShuffleThunk(ShuffleEntry *pShuffleEntryArray, BOOL fRetainThis)
{
    CONTRACT_VOID
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;


    UINT espadjust = 0;
    BOOL haveMemMemMove = FALSE;

    ShuffleEntry *pWalk = NULL;
    for (pWalk = pShuffleEntryArray; pWalk->srcofs != ShuffleEntry::SENTINEL; pWalk++)
    {
        if (!(pWalk->dstofs & ShuffleEntry::REGMASK) &&
            !(pWalk->srcofs & ShuffleEntry::REGMASK) &&
              pWalk->srcofs != pWalk->dstofs)
        {
            haveMemMemMove = TRUE;
            espadjust = sizeof(void*);
            break;
        }
    }

    if (haveMemMemMove)
    {
        // push ecx
        X86EmitPushReg(THIS_kREG);
    }
    else
    {
        // mov eax, ecx
        Emit8(0x8b);
        Emit8(0300 | SCRATCH_REGISTER_X86REG << 3 | THIS_kREG);
    }
    
    UINT16 emptySpot = 0x4 | ShuffleEntry::REGMASK;

    while (true)
    {
        for (pWalk = pShuffleEntryArray; pWalk->srcofs != ShuffleEntry::SENTINEL; pWalk++)
            if (pWalk->dstofs == emptySpot)
                break;
            
        if (pWalk->srcofs == ShuffleEntry::SENTINEL)
            break;
        
        if ((pWalk->dstofs & ShuffleEntry::REGMASK))
        {
            if (pWalk->srcofs & ShuffleEntry::REGMASK)
            {
                // mov <dstReg>,<srcReg>
                Emit8(0x8b);
                Emit8(0300 |
                        (GetX86ArgumentRegisterFromOffset( pWalk->dstofs & ShuffleEntry::OFSMASK ) << 3) |
                        (GetX86ArgumentRegisterFromOffset( pWalk->srcofs & ShuffleEntry::OFSMASK )));
            }
            else
            {
                X86EmitEspOffset(0x8b, GetX86ArgumentRegisterFromOffset( pWalk->dstofs & ShuffleEntry::OFSMASK ), pWalk->srcofs+espadjust);
            }
        }
        else
        {
            // if the destination is not a register, the source shouldn't be either.
            _ASSERTE(!(pWalk->srcofs & ShuffleEntry::REGMASK));
            if (pWalk->srcofs != pWalk->dstofs)
            {
               X86EmitEspOffset(0x8b, kEAX, pWalk->srcofs+espadjust);
               X86EmitEspOffset(0x89, kEAX, pWalk->dstofs+espadjust);
            }
        }
        emptySpot = pWalk->srcofs;
    }
    
    // Capture the stacksizedelta while we're at the end of the list.
    _ASSERTE(pWalk->srcofs == ShuffleEntry::SENTINEL);

    if (haveMemMemMove)
        X86EmitPopReg(SCRATCH_REGISTER_X86REG);

    if (pWalk->stacksizedelta)
        X86EmitAddEsp(pWalk->stacksizedelta);

    if (!fRetainThis)
    {
        // Now jump to real target
        //   JMP [SCRATCHREG]
        // we need to jump indirect so that for virtual delegates eax contains a pointer to the indirection cell
        X86EmitAddReg(SCRATCH_REGISTER_X86REG, DelegateObject::GetOffsetOfMethodPtrAux());
        static const BYTE bjmpeax[] = { 0xff, 0x20 };
        EmitBytes(bjmpeax, sizeof(bjmpeax));
    }

    // if (fRetainThis)
    //     We now have the arguments correctly shuffled and the 'this' pointer in EAX.
    

    RETURN;
}

//===========================================================================
// Computes hash code for MulticastDelegate.Invoke()
UINT_PTR StubLinkerCPU::HashMulticastInvoke(UINT32 numStackBytes, MetaSig* pSig)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;


    // check if the function is returning a float, in which case the stub has to take
    // care of popping the floating point stack except for the last invocation

    _ASSERTE(!(numStackBytes & 3));

    UINT hash = numStackBytes;

    if (CorTypeInfo::IsFloat(pSig->GetReturnType()))
    {
        hash |= 2;
    }

    return hash;
}

//===========================================================================
// Emits code for MulticastDelegate.Invoke()
VOID StubLinkerCPU::EmitDelegateInvoke(BOOL bHasReturnBuffer)
{
    CONTRACT_VOID
    {
        THROWS;
        DISABLED(GC_NOTRIGGER); // contract violation w/ MetaSig::HasRetBuffArg, called by GetOffsetOfThis
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(THIS_kREG == kECX); //If this macro changes, have to change hardcoded emit below
    }
    CONTRACT_END;

    CodeLabel *pNullLabel = NewCodeLabel();

    X86Reg kThisReg = THIS_kREG;

    
    // test THISREG, THISREG
    X86EmitR2ROp(0x85, kThisReg, kThisReg);

    // jz null
    X86EmitCondJump(pNullLabel, X86CondCode::kJZ);

    // mov SCRATCHREG, [THISREG + Delegate.FP]  ; Save target stub in register
    X86EmitIndexRegLoad(SCRATCH_REGISTER_X86REG, kThisReg, DelegateObject::GetOffsetOfMethodPtr());

    // mov THISREG, [THISREG + Delegate.OR]  ; replace "this" pointer
    X86EmitIndexRegLoad(kThisReg, kThisReg, DelegateObject::GetOffsetOfTarget());


    // jmp SCRATCHREG
    Emit16(0xe0ff | (SCRATCH_REGISTER_X86REG<<8));


    // Do a null throw
    EmitLabel(pNullLabel);

    // mov ECX, CORINFO_NullReferenceException
    Emit8(0xb8+kECX);
    Emit32(CORINFO_NullReferenceException);
    X86EmitCall(NewExternalCodeLabel(GetEEFuncEntryPoint(JIT_InternalThrowFromHelper)), 0);

    X86EmitReturn(0);

    RETURN;
}

VOID StubLinkerCPU::EmitMulticastInvoke(UINT numStackBytes, MetaSig* pSig)
{
    CONTRACT_VOID
    {
        THROWS;
        DISABLED(GC_NOTRIGGER); // contract violation w/ MetaSig::HasRetBuffArg, called by GetOffsetOfThis
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(THIS_kREG == kECX); //If this macro changes, have to change hardcoded emit below
    }
    CONTRACT_END;

    X86Reg kThisReg = THIS_kREG;

#ifdef _X86_
    BOOL fReturnFloat = CorTypeInfo::IsFloat(pSig->GetReturnType());
#endif


    // push the methoddesc on the stack
    // mov eax, [ecx + offsetof(_methodAuxPtr)]
    // NOTICE: the amd64 StubLinkerManager will decode this instruction in order to determine
    //         whether the delegate lives in rcx or rdx
    X86EmitIndexRegLoad(SCRATCH_REGISTER_X86REG, kThisReg, DelegateObject::GetOffsetOfMethodPtrAux());
    X86EmitPushReg(SCRATCH_REGISTER_X86REG);
    
    // Push a MulticastFrame on the stack.
    EmitMethodStubProlog(MulticastFrame::GetMethodFrameVPtr());

#ifdef _X86_
    X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());
    // Frame is ready to be inspected by debugger for patch location
    EmitPatchLabel();
#else // _AMD64_

    // Save register arguments in their home locations.
    // Non-FP registers are already saved by EmitMethodStubProlog.
    // (Assumes Sig.NextArg() does not enum RetBuffArg or "this".)

    int            argNum      = 0;
    __int32        argOfs      = MulticastFrame::GetOffsetOfArgs();
    CorElementType argTypes[4];
    CorElementType argType;

    if (fHasRetBuffArg)
    {
        argOfs += sizeof(void*);
        argTypes[argNum] = ELEMENT_TYPE_I8;
        argNum++;
    }

    // 'this'
    argOfs += sizeof(void*);
    argTypes[argNum] = ELEMENT_TYPE_I8;
    argNum++;

    do
    {
        argType = pSig->NextArg();
        
        switch (argType)
        {
        case ELEMENT_TYPE_R4:
        {
            // movss dword ptr [rsp + argOfs], xmm?
            Emit8(0xf3);
            X86EmitOp(0x110f, (X86Reg)argNum, kRSI, argOfs);
            break;
        }
        case ELEMENT_TYPE_R8:
        {
            // movsd qword ptr [rsp + argOfs], xmm?
            Emit8(0xf2);
            X86EmitOp(0x110f, (X86Reg)argNum, kRSI, argOfs);
            break;
        }
        } // switch

        argOfs += sizeof(void*);
        argTypes[argNum] = argType;
        argNum++;
    }
    while (argNum < 4 && ELEMENT_TYPE_END != argType);

    _ASSERTE(4 == argNum || ELEMENT_TYPE_END == argTypes[argNum-1]);

#endif


    X86EmitPushReg(kEDI);

    // xor edi,edi  ;; Loop counter: EDI=0,1,2...
    X86EmitZeroOutReg(kEDI);

    CodeLabel *pLoopLabel = NewCodeLabel();
    CodeLabel *pEndLoopLabel = NewCodeLabel();

    EmitLabel(pLoopLabel);

    // Entry:
    //   EDI == iteration counter

    // push eax     ;; during loop iterations eax contains the return value. Need to save it
    X86EmitPushReg(SCRATCH_REGISTER_X86REG);

    // mov SCRATCHREG, [esi + this]     ;; get delegate
    X86EmitIndexRegLoad(SCRATCH_REGISTER_X86REG, kESI, ArgIterator::GetThisOffset(pSig));

    // cmp edi,[SCRATCHREG]._invocationCount
    X86EmitOp(0x3b, kEDI, SCRATCH_REGISTER_X86REG, DelegateObject::GetOffsetOfInvocationCount());

    // je ENDLOOP
    X86EmitCondJump(pEndLoopLabel, X86CondCode::kJZ);
    // pop eax from the stack
    X86EmitAddEsp(sizeof(void*));


    //    ..repush & reenregister args..
    INT32 ofs = numStackBytes + MulticastFrame::GetOffsetOfArgs();
    while (ofs != MulticastFrame::GetOffsetOfArgs())
    {
        ofs -= sizeof(void*);
        X86EmitIndexPush(kESI, ofs);
    }
    #define DEFINE_ARGUMENT_REGISTER_BACKWARD_WITH_OFFSET(regname, regofs) if (k##regname != THIS_kREG) { X86EmitIndexRegLoad(k##regname, kESI, regofs + MulticastFrame::GetOffsetOfArgumentRegisters()); }
    #include "eecallconv.h"

    //    mov SCRATCHREG, [SCRATCHREG+Delegate._invocationList]  ;;fetch invocation list
    X86EmitIndexRegLoad(SCRATCH_REGISTER_X86REG, SCRATCH_REGISTER_X86REG, DelegateObject::GetOffsetOfInvocationList());

    //    mov SCRATCHREG, [SCRATCHREG+m_Array+edi*4]    ;; index into invocation list
    X86EmitOp(0x8b, kEAX, SCRATCH_REGISTER_X86REG, PtrArray::GetDataOffset(), kEDI, sizeof(void*));

    //    mov THISREG, [SCRATCHREG+Delegate.object]  ;;replace "this" pointer
    X86EmitIndexRegLoad(THIS_kREG, SCRATCH_REGISTER_X86REG, DelegateObject::GetOffsetOfTarget());

    //    call [SCRATCHREG+Delegate.target] ;; call current subscriber
    X86EmitOffsetModRM(0xff, (X86Reg)2, SCRATCH_REGISTER_X86REG, DelegateObject::GetOffsetOfMethodPtr());
    INDEBUG(Emit8(0x90));       // Emit a nop after the call in debug so that
                                // we know that this is a call that can directly call
                                // managed code

    //    inc edi
    Emit8(0x47);

    if (fReturnFloat)
    {
        // if the return value is a float/double check if we just did the last call - if not,
        // emit the pop of the float stack

        // mov SCRATCHREG, [esi + this]     ;; get delegate
        X86EmitIndexRegLoad(SCRATCH_REGISTER_X86REG, kESI, ArgIterator::GetThisOffset(pSig));

        // cmp edi,[SCRATCHREG]._invocationCount
        X86EmitOffsetModRM(0x3b, kEDI, SCRATCH_REGISTER_X86REG, DelegateObject::GetOffsetOfInvocationCount());

        CodeLabel *pNoFloatStackPopLabel = NewCodeLabel();

        // je NOFLOATSTACKPOP
        X86EmitCondJump(pNoFloatStackPopLabel, X86CondCode::kJZ);

        // fstp 0
        Emit16(0xd8dd);

        // NoFloatStackPopLabel:
        EmitLabel(pNoFloatStackPopLabel);
    }


    // The debugger may need to stop here, so grab the offset of this code.
    EmitPatchLabel();

    // jmp LOOP
    X86EmitNearJump(pLoopLabel);

    //ENDLOOP:
    EmitLabel(pEndLoopLabel);
    
    // remove the MethodDesc* from the stack
    //X86EmitAddEsp(sizeof(void*));

    // pop eax      ;; restore eax
    X86EmitPopReg(SCRATCH_REGISTER_X86REG);
    // pop edi     ;; Restore edi
    X86EmitPopReg(kEDI);

    EmitCheckGSCookie(kESI, MulticastFrame::GetOffsetOfGSCookie());
#ifdef _X86_
    X86EmitAddEsp(sizeof(GSCookie));
#endif
    // Epilog
    EmitMethodStubEpilog(numStackBytes, kNoTripStubStyle);

    RETURN;
}

VOID StubLinkerCPU::EmitSecureDelegateInvoke(UINT numStackBytes, MetaSig* pSig)
{
    CONTRACT_VOID
    {
        THROWS;
        DISABLED(GC_NOTRIGGER); // contract violation w/ MetaSig::HasRetBuffArg, called by GetOffsetOfThis
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(THIS_kREG == kECX); //If this macro changes, have to change hardcoded emit below
    }
    CONTRACT_END;

    X86Reg kThisReg = THIS_kREG;


    // push the methoddesc on the stack
    // mov eax, [ecx + offsetof(_invocationCount)]
    // NOTICE: the amd64 StubLinkerManager will decode this instruction in order to determine
    //         whether the delegate lives in rcx or rdx
    X86EmitIndexRegLoad(SCRATCH_REGISTER_X86REG, kThisReg, DelegateObject::GetOffsetOfInvocationCount());
    X86EmitPushReg(SCRATCH_REGISTER_X86REG);
    
    // Push a SecureDelegateFrame on the stack.
    EmitMethodStubProlog(SecureDelegateFrame::GetMethodFrameVPtr());

#ifdef _X86_
    X86EmitPushImmPtr((LPVOID)GetProcessGSCookie());
    // Frame is ready to be inspected by debugger for patch location
    EmitPatchLabel();
#else // _AMD64_

    // Save register arguments in their home locations.
    // Non-FP registers are already saved by EmitMethodStubProlog.
    // (Assumes Sig.NextArg() does not enum RetBuffArg or "this".)

    int            argNum      = 0;
    __int32        argOfs      = MulticastFrame::GetOffsetOfArgs();
    CorElementType argTypes[4];
    CorElementType argType;

    if (fHasRetBuffArg)
    {
        argOfs += sizeof(void*);
        argTypes[argNum] = ELEMENT_TYPE_I8;
        argNum++;
    }

    // 'this'
    argOfs += sizeof(void*);
    argTypes[argNum] = ELEMENT_TYPE_I8;
    argNum++;

    do
    {
        argType = pSig->NextArg();
        
        switch (argType)
        {
        case ELEMENT_TYPE_R4:
        {
            // movss dword ptr [rsp + argOfs], xmm?
            Emit8(0xf3);
            X86EmitOp(0x110f, (X86Reg)argNum, kRSI, argOfs);
            break;
        }
        case ELEMENT_TYPE_R8:
        {
            // movsd qword ptr [rsp + argOfs], xmm?
            Emit8(0xf2);
            X86EmitOp(0x110f, (X86Reg)argNum, kRSI, argOfs);
            break;
        }
        } // switch

        argOfs += sizeof(void*);
        argTypes[argNum] = argType;
        argNum++;
    }
    while (argNum < 4 && ELEMENT_TYPE_END != argType);

    _ASSERTE(4 == argNum || ELEMENT_TYPE_END == argTypes[argNum-1]);

#endif // _AMD64_

    // mov SCRATCHREG, [esi + this]     ;; get delegate
    X86EmitIndexRegLoad(SCRATCH_REGISTER_X86REG, kESI, ArgIterator::GetThisOffset(pSig));

    // I don't think I need this.
    // pop eax from the stack
    //X86EmitAddEsp(sizeof(void*));


    //    ..repush & reenregister args..
    INT32 ofs = numStackBytes + MulticastFrame::GetOffsetOfArgs();
    while (ofs != MulticastFrame::GetOffsetOfArgs())
    {
        ofs -= sizeof(void*);
        X86EmitIndexPush(kESI, ofs);
    }
    #define DEFINE_ARGUMENT_REGISTER_BACKWARD_WITH_OFFSET(regname, regofs) if (k##regname != THIS_kREG) { X86EmitIndexRegLoad(k##regname, kESI, regofs + MulticastFrame::GetOffsetOfArgumentRegisters()); }
    #include "eecallconv.h"

    //    mov SCRATCHREG, [SCRATCHREG+Delegate._invocationList]  ;;fetch invocation list
    X86EmitIndexRegLoad(SCRATCH_REGISTER_X86REG, SCRATCH_REGISTER_X86REG, DelegateObject::GetOffsetOfInvocationList());

    //    mov THISREG, [SCRATCHREG+Delegate.object]  ;;replace "this" pointer
    X86EmitIndexRegLoad(THIS_kREG, SCRATCH_REGISTER_X86REG, DelegateObject::GetOffsetOfTarget());

    //    call [SCRATCHREG+Delegate.target] ;; call current subscriber
    X86EmitOffsetModRM(0xff, (X86Reg)2, SCRATCH_REGISTER_X86REG, DelegateObject::GetOffsetOfMethodPtr());
    INDEBUG(Emit8(0x90));       // Emit a nop after the call in debug so that
                                // we know that this is a call that can directly call
                                // managed code


    // The debugger may need to stop here, so grab the offset of this code.
    EmitPatchLabel();

    EmitCheckGSCookie(kESI, MulticastFrame::GetOffsetOfGSCookie());
#ifdef _X86_
    X86EmitAddEsp(sizeof(GSCookie));
#endif
    // Epilog
    EmitMethodStubEpilog(numStackBytes, kNoTripStubStyle);

    RETURN;
}

// Little helper to generate code to move nbytes bytes of non Ref memory

void generate_noref_copy (unsigned nbytes, StubLinkerCPU* sl)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // If the size is pointer-aligned, we'll use movsd
    if (IS_ALIGNED(nbytes, sizeof(void*)))
    {
        // If there are less than 4 pointers to copy, "unroll" the "rep movsd"
        if (nbytes <= 3*sizeof(void*))
        {
            while (nbytes > 0)
            {
                // movsd
                sl->X86_64BitOperands();
                sl->Emit8(0xa5);

                nbytes -= sizeof(void*);
            }
        }
        else
        {
            // mov ECX, size / 4
            sl->Emit8(0xb8+kECX);
            sl->Emit32(nbytes / sizeof(void*));

            // repe movsd
            sl->Emit8(0xf3);
            sl->X86_64BitOperands();
            sl->Emit8(0xa5);
        }
    }
    else
    {
        // mov ECX, size
        sl->Emit8(0xb8+kECX);
        sl->Emit32(nbytes);

        // repe movsb
        sl->Emit16(0xa4f3);
    }
}


X86Reg LoadArrayOpArg (
        BYTE fInReg,
        UINT32 idxloc,
        StubLinkerCPU *psl,
        X86Reg kRegIfFromMem,
        UINT ofsadjust
        AMD64_ARG(StubLinkerCPU::X86OperandSize OperandSize = StubLinkerCPU::k64BitOp)
        )
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (fInReg)
        return GetX86ArgumentRegisterFromOffset(idxloc);

    psl->X86EmitEspOffset(0x8b, kRegIfFromMem, idxloc + ofsadjust AMD64_ARG(OperandSize));
    return kRegIfFromMem;
}


X86Reg LoadArrayOpArg2 (
        INT regloc,
        UINT32 idxloc,
        StubLinkerCPU *psl,
        X86Reg kRegIfFromMem,
        UINT ofsadjust
        AMD64_ARG(StubLinkerCPU::X86OperandSize OperandSize = StubLinkerCPU::k64BitOp)
        )
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (regloc != -1)
        return GetX86ArgumentRegisterFromOffset(regloc*sizeof(size_t));

    psl->X86EmitEspOffset(0x8b, kRegIfFromMem, idxloc + ofsadjust AMD64_ARG(OperandSize));
    return kRegIfFromMem;
}


//===========================================================================
// Emits code to do an array operation.
VOID StubLinkerCPU::EmitArrayOpStub(const ArrayOpScript* pArrayOpScript)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    // This is the offset to the parameters/what's already pushed on the stack:
    // return address.
    const UINT  locsize     = sizeof(void*);

    // ArrayOpScript's stack offsets are built using ArgIterator, which
    // assumes a FramedMethodFrame has been pushed, which is not the case
    // here.  rsp + ofsadjust should point at the first argument.  Any further
    // stack modifications below need to adjust ofsadjust appropriately.
    // baseofsadjust needs to be the stack adjustment at the entry point -
    // this is used further below to compute how much stack space was used.
    
    const UINT baseofsadjust = locsize - sizeof(FramedMethodFrame);
    UINT ofsadjust = baseofsadjust;

    // Register usage
    //
    //                                          x86                 AMD64
    // Inputs:
    //  managed array                           THIS_kREG (ecx)     THIS_kREG (rcx)
    //  index 0                                 edx                 rdx
    //  index 1/value                           <stack>             r8
    //  index 2/value                           <stack>             r9
    //  expected element type for LOADADDR      eax                 rax                 rdx
    // Working registers:
    //  total (accumulates unscaled offset)     edi                 r10
    //  factor (accumulates the slice factor)   esi                 r11
    X86Reg kArrayRefReg = THIS_kREG;
    const X86Reg kTotalReg    = kEDI;
    const X86Reg kFactorReg   = kESI;


    X86Reg       kValueReg;

    CodeLabel *Epilog = NewCodeLabel();
    CodeLabel *Inner_nullexception = NewCodeLabel();
    CodeLabel *Inner_rangeexception = NewCodeLabel();
    CodeLabel *Inner_typeMismatchexception = 0;

    //
    // Set up the stack frame.
    //
    //
    // x86:
    //          value
    //          <index n-1>
    //          ...
    //          <index 1>
    //          return address
    //          saved edi
    // esp ->   saved esi
    //
    //
    // AMD64:
    //          value, if rank > 2
    //          ...
    // + 0x48   more indices
    // + 0x40   r9 home
    // + 0x38   r8 home
    // + 0x30   rdx home
    // + 0x28   rcx home
    // + 0x20   return address
    // + 0x18   scratch area (callee's r9)
    // + 0x10   scratch area (callee's r8)
    // +    8   scratch area (callee's rdx)
    // rsp ->   scratch area (callee's rcx)
    //
    // If the element type is a value class w/ object references, then rsi
    // and rdi will also be saved above the scratch area:
    //
    // ...
    // + 0x28   saved rsi
    // + 0x20   saved rdi
    // + 0x18   scratch area (callee's r9)
    // + 0x10   scratch area (callee's r8)
    // +    8   scratch area (callee's rdx)
    // rsp ->   scratch area (callee's rcx)
    //
    // And if no call or movsb is necessary, then the scratch area sits
    // directly under the MethodDesc*.

    BOOL fSavedESI = FALSE;
    BOOL fSavedEDI = FALSE;

    // Preserve the callee-saved registers
    // NOTE: if you change the sequence of these pushes, you must also update:
    //  ArrayOpStubNullException
    //  ArrayOpStubRangeException
    //  ArrayOpStubTypeMismatchException
    _ASSERTE(      kTotalReg == kEDI);
    X86EmitPushReg(kTotalReg);
    _ASSERTE(      kFactorReg == kESI);
    X86EmitPushReg(kFactorReg);

    fSavedESI = fSavedEDI = TRUE;

    ofsadjust += 2*sizeof(void*);

    // Check for null.
    X86EmitR2ROp(0x85, kArrayRefReg, kArrayRefReg);             //   TEST ECX, ECX
    X86EmitCondJump(Inner_nullexception, X86CondCode::kJZ);     //   jz  Inner_nullexception

    // Do Type Check if needed
    if (pArrayOpScript->m_flags & ArrayOpScript::NEEDSTYPECHECK)
    {
        // throw exception if failed
        Inner_typeMismatchexception = NewCodeLabel();

        if (pArrayOpScript->m_op == ArrayOpScript::STORE)
        {
            // Get the value to be stored.
            kValueReg = LoadArrayOpArg(pArrayOpScript->m_fValInReg, pArrayOpScript->m_fValLoc, this, kEAX, ofsadjust);

            X86EmitR2ROp(0x85, kValueReg, kValueReg);                   // TEST kValueReg, kValueReg
            CodeLabel *CheckPassed = NewCodeLabel();
            X86EmitCondJump(CheckPassed, X86CondCode::kJZ);             // storing NULL is OK

            X86EmitOp(0x8b, kEAX, kValueReg, 0 AMD64_ARG(k64BitOp));    // mov EAX, [kValueReg]  ; possibly trashes kValueReg
                                                                        // cmp EAX, [kArrayRefReg+m_ElementType]
            X86_64BitOperands();
            X86EmitOp(0x3b, kEAX, kArrayRefReg, offsetof(PtrArray, m_ElementType));
            X86EmitCondJump(CheckPassed, X86CondCode::kJZ);             // Exact match is OK

            X86EmitRegLoad(kEAX, (UINT_PTR)g_pObjectClass);             // mov EAX, g_pObjectMethodTable
                                                                        // cmp EAX, [kArrayRefReg+m_ElementType]
            X86_64BitOperands();
            X86EmitOp(0x3b, kEAX, kArrayRefReg, offsetof(PtrArray, m_ElementType));
            X86EmitCondJump(CheckPassed, X86CondCode::kJZ);             // Assigning to array of object is OK


            X86EmitPushReg(kEDX);      // Save EDX
            X86EmitPushReg(kECX);      // pass array object

            // get address of value to store
            _ASSERTE(!(pArrayOpScript->m_fValInReg)); // on x86, value will never get a register: so too lazy to implement that case
            X86EmitEspOffset(0x8d, kECX, pArrayOpScript->m_fValLoc + ofsadjust + 2*sizeof(void*));      // lea ECX, [ESP+offs]
            // get address of 'this'
            X86EmitEspOffset(0x8d, kEDX, 0);    // lea EDX, [ESP]       (address of ECX)

            AMD64_ONLY(_ASSERTE(fNeedScratchArea));
            X86EmitCall(NewExternalCodeLabel((LPVOID) ArrayStoreCheck), 0);

            X86EmitPopReg(kECX);        // restore regs
            X86EmitPopReg(kEDX);

            X86EmitR2ROp(0x3B, kEAX, kEAX);                             //   CMP EAX, EAX
            X86EmitCondJump(Epilog, X86CondCode::kJNZ);         // This branch never taken, but epilog walker uses it

            X86EmitR2ROp(0x84, kEAX, kEAX AMD64_ARG(k32BitOp));         //   TEST AL, AL ;; test the result of ArrayStoreCheck
            X86EmitCondJump(Inner_typeMismatchexception, X86CondCode::kJZ);

            EmitLabel(CheckPassed);
        }
        else
        {
            _ASSERTE(pArrayOpScript->m_op == ArrayOpScript::LOADADDR);

            // Load up the hidden type parameter into 'typeReg'
            X86Reg typeReg = LoadArrayOpArg2(pArrayOpScript->m_typeParamReg, pArrayOpScript->m_typeParamOffs, this, kEAX, ofsadjust);

            // 'typeReg' holds the typeHandle for the ARRAY.  This must be a ArrayTypeDesc*, so
            // mask off the low two bits to get the TypeDesc*
            X86EmitR2ROp(0x83, (X86Reg)4, typeReg);    //   AND typeReg, 0xFFFFFFFC
            Emit8(0xFC);

            // If 'typeReg' is NULL then we're executing the readonly ::Address and no type check is
            // needed.
            CodeLabel *Inner_passedTypeCheck = NewCodeLabel();

            X86EmitCondJump(Inner_passedTypeCheck, X86CondCode::kJZ);
            
            // Get the parameter of the parameterize type
            // mov typeReg, [typeReg.m_Arg]
            X86EmitOp(0x8b, typeReg, typeReg, offsetof(ParamTypeDesc, m_Arg) AMD64_ARG(k64BitOp));
            
            // Compare this against the element type of the array.
            // cmp typeReg, [ECX+m_ElementType];
            X86EmitOp(0x3b, typeReg, kECX, offsetof(PtrArray, m_ElementType) AMD64_ARG(k64BitOp));
            
            // Throw error if not equal
            X86EmitCondJump(Inner_typeMismatchexception, X86CondCode::kJNZ);
            EmitLabel(Inner_passedTypeCheck);
        }
    }

    CodeLabel* DoneCheckLabel = 0;
    if (pArrayOpScript->m_rank == 1 && pArrayOpScript->m_fHasLowerBounds)
    {
        DoneCheckLabel = NewCodeLabel();
        CodeLabel* NotSZArrayLabel = NewCodeLabel();

        // for rank1 arrays, we might actually have two different layouts depending on
        // if we are ELEMENT_TYPE_ARRAY or ELEMENT_TYPE_SZARRAY.

            // mov EAX, [ARRAY]          // EAX holds the method table
        X86_64BitOperands();
        X86EmitOp(0x8b, kEAX, kArrayRefReg);

            // mov EAX, [EAX + m_pEEClass]  // EAX points to the EEClass
        X86_64BitOperands();
        X86EmitOp(0x8b, kEAX, kEAX, MethodTable::GetOffsetOfEEClass());

        // cmp BYTE [EAX+m_NormType], ELEMENT_TYPE_SZARRAY
        X86EmitOp(0x80, (X86Reg)7, kEAX, EEClass::GetOffsetOfInternalType());
        Emit8(ELEMENT_TYPE_SZARRAY);

            // jz NotSZArrayLabel
        X86EmitCondJump(NotSZArrayLabel, X86CondCode::kJNZ);

            //Load the passed-in index into the scratch register.
        const ArrayOpIndexSpec *pai = pArrayOpScript->GetArrayOpIndexSpecs();
        X86Reg idxReg = LoadArrayOpArg(pai->m_freg, pai->m_idxloc, this, SCRATCH_REGISTER_X86REG, ofsadjust);

            // cmp idxReg, [kArrayRefReg + LENGTH]
        X86EmitOp(0x3b, idxReg, kArrayRefReg, ArrayBase::GetOffsetOfNumComponents());

            // jae Inner_rangeexception
        X86EmitCondJump(Inner_rangeexception, X86CondCode::kJAE);

        X86EmitR2ROp(0x8b, kTotalReg, idxReg AMD64_ARG(k32BitOp));

            // sub ARRAY. 8                  // 8 is accounts for the Lower bound and Dim count in the ARRAY
        X86EmitSubReg(kArrayRefReg, 8);      // adjust this pointer so that indexing works out for SZARRAY

        X86EmitNearJump(DoneCheckLabel);
        EmitLabel(NotSZArrayLabel);
    }

    // For each index, range-check and mix into accumulated total.
    UINT idx = pArrayOpScript->m_rank;
    BOOL firstTime = TRUE;
    while (idx--)
    {
        const ArrayOpIndexSpec *pai = pArrayOpScript->GetArrayOpIndexSpecs() + idx;

        //Load the passed-in index into the scratch register.
        X86Reg srcreg = LoadArrayOpArg(pai->m_freg, pai->m_idxloc, this, SCRATCH_REGISTER_X86REG, ofsadjust AMD64_ARG(k32BitOp));
        if (SCRATCH_REGISTER_X86REG != srcreg)
            X86EmitR2ROp(0x8b, SCRATCH_REGISTER_X86REG, srcreg AMD64_ARG(k32BitOp));

        // sub SCRATCH, dword ptr [kArrayRefReg + LOWERBOUND]
        if (pArrayOpScript->m_fHasLowerBounds)
        {
            X86EmitOp(0x2b, SCRATCH_REGISTER_X86REG, kArrayRefReg, pai->m_lboundofs);
        }

        // cmp SCRATCH, dword ptr [kArrayRefReg + LENGTH]
        X86EmitOp(0x3b, SCRATCH_REGISTER_X86REG, kArrayRefReg, pai->m_lengthofs);

        // jae Inner_rangeexception
        X86EmitCondJump(Inner_rangeexception, X86CondCode::kJAE);


        // SCRATCH == idx - LOWERBOUND
        //
        // imul SCRATCH, FACTOR
        if (!firstTime)
        {
            //Can skip the first time since FACTOR==1
            X86EmitR2ROp(0xaf0f, SCRATCH_REGISTER_X86REG, kFactorReg AMD64_ARG(k32BitOp));
        }

        // TOTAL += SCRATCH
        if (firstTime)
        {
            // First time, we must zero-init TOTAL. Since
            // zero-initing and then adding is just equivalent to a
            // "mov", emit a "mov"
            //    mov  TOTAL, SCRATCH
            X86EmitR2ROp(0x8b, kTotalReg, SCRATCH_REGISTER_X86REG AMD64_ARG(k32BitOp));
        }
        else
        {
            //    add  TOTAL, SCRATCH
            X86EmitR2ROp(0x03, kTotalReg, SCRATCH_REGISTER_X86REG AMD64_ARG(k32BitOp));
        }

        // FACTOR *= [kArrayRefReg + LENGTH]
        if (idx != 0)
        {
            // No need to update FACTOR on the last iteration
            //  since we won't use it again

            if (firstTime)
            {
                // must init FACTOR to 1 first: hence,
                // the "imul" becomes a "mov"
                // mov FACTOR, [kArrayRefReg + LENGTH]
                X86EmitOp(0x8b, kFactorReg, kArrayRefReg, pai->m_lengthofs);
            }
            else
            {
                // imul FACTOR, [kArrayRefReg + LENGTH]
                X86EmitOp(0xaf0f, kFactorReg, kArrayRefReg, pai->m_lengthofs);
            }
        }

        firstTime = FALSE;
    }

    if (DoneCheckLabel != 0)
        EmitLabel(DoneCheckLabel);

    // Pass these values to X86EmitArrayOp() to generate the element address.
    X86Reg elemBaseReg   = kArrayRefReg;
    X86Reg elemScaledReg = kTotalReg;
    UINT32 elemScale     = pArrayOpScript->m_elemsize;
    UINT32 elemOfs       = pArrayOpScript->m_ofsoffirst;

    if (!(elemScale == 1 || elemScale == 2 || elemScale == 4 || elemScale == 8))
    {
        switch (elemScale)
        {
            // No way to express this as a SIB byte. Fold the scale
            // into TOTAL.

            case 16:
                // shl TOTAL,4
                X86EmitR2ROp(0xc1, (X86Reg)4, kTotalReg AMD64_ARG(k32BitOp));
                Emit8(4);
                break;

            case 32:
                // shl TOTAL,5
                X86EmitR2ROp(0xc1, (X86Reg)4, kTotalReg AMD64_ARG(k32BitOp));
                Emit8(5);
                break;

            case 64:
                // shl TOTAL,6
                X86EmitR2ROp(0xc1, (X86Reg)4, kTotalReg AMD64_ARG(k32BitOp));
                Emit8(6);
                break;

            default:
                // imul TOTAL, elemScale
                X86EmitR2ROp(0x69, kTotalReg, kTotalReg AMD64_ARG(k32BitOp));
                Emit32(elemScale);
                break;
        }
        elemScale = 1;
    }

    // Now, do the operation:

    switch (pArrayOpScript->m_op)
    {
        case pArrayOpScript->LOADADDR:
            // lea eax, ELEMADDR
            X86EmitOp(0x8d, kEAX, elemBaseReg, elemOfs, elemScaledReg, elemScale AMD64_ARG(k64BitOp));
            break;

        case pArrayOpScript->LOAD:
            if (pArrayOpScript->m_flags & pArrayOpScript->HASRETVALBUFFER)
            {
                // Ensure that these registers have been saved!
                _ASSERTE(fSavedESI && fSavedEDI);

                //lea esi, ELEMADDR
                X86EmitOp(0x8d, kESI, elemBaseReg, elemOfs, elemScaledReg, elemScale AMD64_ARG(k64BitOp));

                _ASSERTE(pArrayOpScript->m_fRetBufInReg);
                // mov edi, retbufptr
                X86EmitR2ROp(0x8b, kEDI, GetX86ArgumentRegisterFromOffset(pArrayOpScript->m_fRetBufLoc));

COPY_VALUE_CLASS:
                {
                    size_t size = pArrayOpScript->m_elemsize;
                    size_t total = 0;
                    if(pArrayOpScript->m_gcDesc)
                    {
                        CGCDescSeries* cur = pArrayOpScript->m_gcDesc->GetHighestSeries();
                        if ((cur->startoffset-elemOfs) > 0)
                            generate_noref_copy ((unsigned) (cur->startoffset - elemOfs), this);
                        total += cur->startoffset - elemOfs;

                        SSIZE_T cnt = (SSIZE_T) pArrayOpScript->m_gcDesc->GetNumSeries();
                        // special array encoding
                        _ASSERTE(cnt < 0);

                        for (SSIZE_T __i = 0; __i > cnt; __i--)
                        {
                            HALF_SIZE_T skip =  cur->val_serie[__i].skip;
                            HALF_SIZE_T nptrs = cur->val_serie[__i].nptrs;
                            total += nptrs*sizeof (DWORD*);
                            do
                            {
                                AMD64_ONLY(_ASSERTE(fNeedScratchArea));

                                X86EmitCall(NewExternalCodeLabel((LPVOID) JIT_ByRefWriteBarrier), 0);
                            } while (--nptrs);
                            if (skip > 0)
                            {
                                //check if we are at the end of the series
                                if (__i == (cnt + 1))
                                    skip = skip - (HALF_SIZE_T)(cur->startoffset - elemOfs);
                                if (skip > 0)
                                    generate_noref_copy (skip, this);
                            }
                            total += skip;
                        }

                        _ASSERTE (size == total);
                    }
                    else
                    {
                        // no ref anywhere, just copy the bytes.
                        _ASSERTE (size);
                        generate_noref_copy ((unsigned)size, this);
                    }
                }
            }
            else
            {
                switch (pArrayOpScript->m_elemsize)
                {
                case 1:
                    // mov[zs]x eax, byte ptr ELEMADDR
                    X86EmitOp(pArrayOpScript->m_signed ? 0xbe0f : 0xb60f, kEAX, elemBaseReg, elemOfs, elemScaledReg, elemScale);
                    break;

                case 2:
                    // mov[zs]x eax, word ptr ELEMADDR
                    X86EmitOp(pArrayOpScript->m_signed ? 0xbf0f : 0xb70f, kEAX, elemBaseReg, elemOfs, elemScaledReg, elemScale);
                    break;

                case 4:
                    if (pArrayOpScript->m_flags & pArrayOpScript->ISFPUTYPE)
                    {
                        // fld dword ptr ELEMADDR
                        X86EmitOp(0xd9, (X86Reg)0, elemBaseReg, elemOfs, elemScaledReg, elemScale);
                    }
                    else
                    {
                        // mov eax, ELEMADDR
                        X86EmitOp(0x8b, kEAX, elemBaseReg, elemOfs, elemScaledReg, elemScale);
                    }
                    break;

                case 8:
                    if (pArrayOpScript->m_flags & pArrayOpScript->ISFPUTYPE)
                    {
                        // fld qword ptr ELEMADDR
                        X86EmitOp(0xdd, (X86Reg)0, elemBaseReg, elemOfs, elemScaledReg, elemScale);
                    }
                    else
                    {
                        // mov eax, ELEMADDR
                        X86EmitOp(0x8b, kEAX, elemBaseReg, elemOfs, elemScaledReg, elemScale AMD64_ARG(k64BitOp));
#ifdef _X86_
                        // mov edx, ELEMADDR + 4
                        X86EmitOp(0x8b, kEDX, elemBaseReg, elemOfs + 4, elemScaledReg, elemScale);
#endif
                    }
                    break;

                default:
                    _ASSERTE(0);
                }
            }

            break;

        case pArrayOpScript->STORE:

            switch (pArrayOpScript->m_elemsize)
            {
            case 1:
                // mov SCRATCH, [esp + valoffset]
                kValueReg = LoadArrayOpArg(pArrayOpScript->m_fValInReg, pArrayOpScript->m_fValLoc, this, SCRATCH_REGISTER_X86REG, ofsadjust);
                // mov byte ptr ELEMADDR, SCRATCH.b
                X86EmitOp(0x88, kValueReg, elemBaseReg, elemOfs, elemScaledReg, elemScale);
                break;
            case 2:
                // mov SCRATCH, [esp + valoffset]
                kValueReg = LoadArrayOpArg(pArrayOpScript->m_fValInReg, pArrayOpScript->m_fValLoc, this, SCRATCH_REGISTER_X86REG, ofsadjust);
                // mov word ptr ELEMADDR, SCRATCH.w
                Emit8(0x66);
                X86EmitOp(0x89, kValueReg, elemBaseReg, elemOfs, elemScaledReg, elemScale);
                break;
            case 4:
                if (pArrayOpScript->m_flags & pArrayOpScript->NEEDSWRITEBARRIER)
                {
                    // mov SCRATCH, [esp + valoffset]
                    kValueReg = LoadArrayOpArg(pArrayOpScript->m_fValInReg, pArrayOpScript->m_fValLoc, this, SCRATCH_REGISTER_X86REG, ofsadjust);

                    _ASSERTE(SCRATCH_REGISTER_X86REG == kEAX); // value to store is already in EAX where we want it.
                    // lea edx, ELEMADDR
                    X86EmitOp(0x8d, kEDX, elemBaseReg, elemOfs, elemScaledReg, elemScale);

                    // call JIT_Writeable_Thunks_Buf.WriteBarrierReg[0] (== EAX)
                    X86EmitCall(NewExternalCodeLabel((LPVOID) &JIT_Writeable_Thunks_Buf.WriteBarrierReg[0]), 0);
                }
                else
                {
                    // mov SCRATCH, [esp + valoffset]
                    kValueReg = LoadArrayOpArg(pArrayOpScript->m_fValInReg, pArrayOpScript->m_fValLoc, this, SCRATCH_REGISTER_X86REG, ofsadjust AMD64_ARG(k32BitOp));

                    // mov ELEMADDR, SCRATCH
                    X86EmitOp(0x89, kValueReg, elemBaseReg, elemOfs, elemScaledReg, elemScale);
                }
                break;

            case 8:

                if (!(pArrayOpScript->m_flags & ArrayOpScript::NEEDSWRITEBARRIER))
                {
                    _ASSERTE(!(pArrayOpScript->m_fValInReg)); // on x86, value will never get a register: so too lazy to implement that case
                    // mov SCRATCH, [esp + valoffset]
                    X86EmitEspOffset(0x8b, SCRATCH_REGISTER_X86REG, pArrayOpScript->m_fValLoc + ofsadjust);
                    // mov ELEMADDR, SCRATCH
                    X86EmitOp(0x89, SCRATCH_REGISTER_X86REG, elemBaseReg, elemOfs, elemScaledReg, elemScale);

                    _ASSERTE(!(pArrayOpScript->m_fValInReg)); // on x86, value will never get a register: so too lazy to implement that case
                    // mov SCRATCH, [esp + valoffset + 4]
                    X86EmitEspOffset(0x8b, SCRATCH_REGISTER_X86REG, pArrayOpScript->m_fValLoc + ofsadjust + 4);
                    // mov ELEMADDR+4, SCRATCH
                    X86EmitOp(0x89, SCRATCH_REGISTER_X86REG, elemBaseReg, elemOfs+4, elemScaledReg, elemScale);
                    break;
                }
                    // FALL THROUGH (on x86)
            default:
                // Ensure that these registers have been saved!
                _ASSERTE(fSavedESI && fSavedEDI);

                _ASSERTE(!pArrayOpScript->m_fValInReg);
                // lea esi, [esp + valoffset]
                X86EmitEspOffset(0x8d, kESI, pArrayOpScript->m_fValLoc + ofsadjust);

                // lea edi, ELEMADDR
                X86EmitOp(0x8d, kEDI, elemBaseReg, elemOfs, elemScaledReg, elemScale AMD64_ARG(k64BitOp));
                goto COPY_VALUE_CLASS;
            }
            break;

        default:
            _ASSERTE(0);
    }

    EmitLabel(Epilog);

    // Restore the callee-saved registers
    X86EmitPopReg(kFactorReg);
    X86EmitPopReg(kTotalReg);

    // ret N
    X86EmitReturn(pArrayOpScript->m_cbretpop);

    // Exception points must clean up the stack for all those extra args.
    // kFactorReg and kTotalReg will be popped by the jump targets.

    void *pvExceptionThrowFn;


    EmitLabel(Inner_nullexception);
    pvExceptionThrowFn = (LPVOID)ArrayOpStubNullException;
    Emit8(0xb8);        // mov EAX, <stack cleanup>
    Emit32(pArrayOpScript->m_cbretpop);
    X86EmitNearJump(NewExternalCodeLabel(pvExceptionThrowFn));

    EmitLabel(Inner_rangeexception);
    pvExceptionThrowFn = (LPVOID)ArrayOpStubRangeException;
    Emit8(0xb8);        // mov EAX, <stack cleanup>
    Emit32(pArrayOpScript->m_cbretpop);
    X86EmitNearJump(NewExternalCodeLabel(pvExceptionThrowFn));

    if (pArrayOpScript->m_flags & ArrayOpScript::NEEDSTYPECHECK)
    {
        EmitLabel(Inner_typeMismatchexception);
        pvExceptionThrowFn = (LPVOID)ArrayOpStubTypeMismatchException;
        Emit8(0xb8);        // mov EAX, <stack cleanup>
        Emit32(pArrayOpScript->m_cbretpop);
        X86EmitNearJump(NewExternalCodeLabel(pvExceptionThrowFn));
    }
}


//===========================================================================
// Emits code to break into debugger
VOID StubLinkerCPU::EmitDebugBreak()
{
    // int3
    Emit8(0xCC);
}


//===========================================================================
// Emits code to touch pages
// Inputs:
//   eax = first byte of data
//   edx = first byte past end of data
//
// Trashes eax, edx, ecx
//
// Pass TRUE if edx is guaranteed to be strictly greater than eax.
VOID StubLinkerCPU::EmitPageTouch(BOOL fSkipNullCheck)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    CodeLabel *pEndLabel = NewCodeLabel();
    CodeLabel *pLoopLabel = NewCodeLabel();

    if (!fSkipNullCheck)
    {
        // cmp eax,edx
        X86EmitR2ROp(0x3b, kEAX, kEDX);

        // jnb EndLabel
        X86EmitCondJump(pEndLabel, X86CondCode::kJNB);
    }

    _ASSERTE(0 == (PAGE_SIZE & (PAGE_SIZE-1)));

    // and eax, ~(PAGE_SIZE-1)
    Emit8(0x25);
    Emit32( ~( ((UINT32)PAGE_SIZE) - 1 ));

    EmitLabel(pLoopLabel);
    // mov cl, [eax]
    X86EmitOp(0x8a, kECX, kEAX);
    // add eax, PAGESIZE
    X86_64BitOperands();
    Emit8(0x05);
    Emit32(PAGE_SIZE);
    // cmp eax, edx
    X86EmitR2ROp(0x3b, kEAX, kEDX);
    // jb LoopLabel
    X86EmitCondJump(pLoopLabel, X86CondCode::kJB);

    EmitLabel(pEndLabel);
}


VOID StubLinkerCPU::EmitProfilerComCallProlog(TADDR pFrameVptr, X86Reg regFrame)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;


    if (pFrameVptr == UMThkCallFrame::GetUMThkCallFrameVPtr())
    {
        // Save registers
        X86EmitPushReg(kEAX);
        X86EmitPushReg(kECX);
        X86EmitPushReg(kEDX);

        // Load the methoddesc into ECX (UMThkCallFrame->m_pvDatum->m_pMD)
        X86EmitIndexRegLoad(kECX, regFrame, UMThkCallFrame::GetOffsetOfDatum());
        X86EmitIndexRegLoad(kECX, kECX, UMEntryThunk::GetOffsetOfMethodDesc());

#ifdef PROFILING_SUPPORTED

        // Push arguments and notify profiler
        X86EmitPushImm32(COR_PRF_TRANSITION_CALL);      // Reason
        X86EmitPushReg(kECX);                           // MethodDesc*
        X86EmitCall(NewExternalCodeLabel((LPVOID) ProfilerUnmanagedToManagedTransitionMD), 2*sizeof(void*));
#endif // PROFILING_SUPPORTED

        // Restore registers
        X86EmitPopReg(kEDX);
        X86EmitPopReg(kECX);
        X86EmitPopReg(kEAX);
    }


    // Unrecognized frame vtbl
    else
    {
        _ASSERTE(!"Unrecognized vtble passed to EmitComMethodStubProlog with profiling turned on.");
    }
}


VOID StubLinkerCPU::EmitProfilerComCallEpilog(TADDR pFrameVptr, X86Reg regFrame)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(pFrameVptr == UMThkCallFrame::GetUMThkCallFrameVPtr() ||
                     pFrameVptr == ComMethodFrame::GetMethodFrameVPtr());
    }
    CONTRACTL_END;


    if (pFrameVptr == UMThkCallFrame::GetUMThkCallFrameVPtr())
    {
        // Save registers
        X86EmitPushReg(kEAX);
        X86EmitPushReg(kECX);
        X86EmitPushReg(kEDX);

        // Load the methoddesc into ECX (UMThkCallFrame->m_pvDatum->m_pMD)
        X86EmitIndexRegLoad(kECX, regFrame, UMThkCallFrame::GetOffsetOfDatum());
        X86EmitIndexRegLoad(kECX, kECX, UMEntryThunk::GetOffsetOfMethodDesc());

#ifdef PROFILING_SUPPORTED

        // Push arguments and notify profiler
        X86EmitPushImm32(COR_PRF_TRANSITION_RETURN);    // Reason
        X86EmitPushReg(kECX);                           // MethodDesc*
        X86EmitCall(NewExternalCodeLabel((LPVOID) ProfilerManagedToUnmanagedTransitionMD), 2*sizeof(void*));
#endif // PROFILING_SUPPORTED

        // Restore registers
        X86EmitPopReg(kEDX);
        X86EmitPopReg(kECX);
        X86EmitPopReg(kEAX);
    }


    // Unrecognized frame vtbl
    else
    {
        _ASSERTE(!"Unrecognized vtble passed to EmitComMethodStubEpilog with profiling turned on.");
    }
}

// This is the exception code to report SetupThread failure to caller of reverse pinvoke
#define EXCEPTION_EXX 0xE0455858  // 0xe0000000 | 'EXX'

Thread* __stdcall CreateThreadBlockThrow()
{

    WRAPPER_CONTRACT;

    CONTRACT_VIOLATION(ThrowsViolation); // WON'T FIX - This enables catastrophic failure exception in reverse P/Invoke - the only way we can communicate an error to legacy code.
    Thread* pThread = NULL;
    BEGIN_ENTRYPOINT_THROWS;

    EX_TRY
    {
        pThread = SetupThread();
    }
    EX_CATCH
    {
        // Creating Thread failed, and we need to throw an exception to report status.  
        // If we use our internal C++ exception, we will leak the memory for our C++ exception object.  
        // It is misleading to use our COM+ exception code, since this is not a managed exception.  
        // In the end, I picked e0455858 (EXX).

        ULONG_PTR arg = GET_EXCEPTION()->GetHR();
        RaiseException(EXCEPTION_EXX,0,1,&arg);
    }
    EX_END_CATCH(SwallowAllExceptions);
    END_ENTRYPOINT_THROWS;

    return pThread;
}


#endif // !DACCESS_COMPILE



#ifdef HAS_FIXUP_PRECODE
TADDR FixupPrecode::GetTarget()
{ 
    LEAF_CONTRACT;
    return (m_type == FixupPrecode::TypePrestub) ? 
        (TADDR)ThePreStub()->GetEntryPoint() : rel32Decode(PTR_HOST_MEMBER_TADDR(FixupPrecode, this, m_rel32));
}
#endif

#ifndef DACCESS_COMPILE

BOOL rel32SetInterlocked(/*PINT32*/ PVOID pRel32, TADDR target, TADDR expected, MethodDesc* pMD)
{
    CONTRACTL
    {
        THROWS;         // Creating a JumpStub could throw OutOfMemory
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    BYTE* callAddrAdj = (BYTE*)pRel32 + 4;
    INT32 expectedRel32 = (BYTE*)expected - callAddrAdj;

    INT32 targetRel32 = rel32UsingJumpStub((INT32*)pRel32, target, pMD);

    _ASSERTE(IS_ALIGNED(pRel32, sizeof(INT32)));
    return FastInterlockCompareExchange((LONG*)pRel32, (LONG)targetRel32, (LONG)expectedRel32) == (LONG)expectedRel32;
}

void StubPrecode::Init(MethodDesc* pMD, BaseDomain* pDomain, int type /*=StubPrecode::Type*/, TADDR target /*=NULL*/)
{
    WRAPPER_CONTRACT;

    IN_WIN64(m_movR10 = X86_INSTR_MOV_R10_IMM64);   // mov r10, pMethodDesc
    IN_WIN32(m_movEAX = X86_INSTR_MOV_EAX_IMM32);   // mov eax, pMethodDesc
    m_pMethodDesc = (TADDR)pMD;
    IN_WIN32(m_mov_rm_r = X86_INSTR_MOV_RM_R);      // mov reg,reg
    m_type = type;
    m_jmp = X86_INSTR_JMP_REL32;        // jmp rel32

    if (pDomain != NULL)
    {
        if (target == NULL)
            target = (TADDR)ThePreStub()->GetEntryPoint();
        m_rel32 = rel32UsingJumpStub(&m_rel32, target, pMD, pDomain);
    }
}



#ifdef HAS_NDIRECT_IMPORT_PRECODE

void NDirectImportPrecode::Init(MethodDesc* pMD, BaseDomain* pDomain)
{
    WRAPPER_CONTRACT;
    StubPrecode::Init(pMD, pDomain, NDirectImportPrecode::Type, (TADDR)GetEEFuncEntryPoint(NDirectImportThunk));
}


#endif // HAS_NDIRECT_IMPORT_PRECODE


#ifdef HAS_REMOTING_PRECODE

void RemotingPrecode::Init(MethodDesc* pMD, BaseDomain* pDomain)
{
    WRAPPER_CONTRACT;

    IN_WIN64(m_movR10 = X86_INSTR_MOV_R10_IMM64);   // mov r10, pMethodDesc
    IN_WIN32(m_movEAX = X86_INSTR_MOV_EAX_IMM32);   // mov eax, pMethodDesc
    m_pMethodDesc = (TADDR)pMD;
    m_type = PRECODE_REMOTING;          // nop
    m_call = X86_INSTR_CALL_REL32;
    m_jmp = X86_INSTR_JMP_REL32;        // jmp rel32

    if (pDomain != NULL)
    {
        m_callRel32 = rel32UsingJumpStub(&m_callRel32,
            (TADDR)GetEEFuncEntryPoint(PrecodeRemotingThunk), pMD, pDomain);
        m_rel32 = rel32UsingJumpStub(&m_rel32,
            (TADDR)ThePreStub()->GetEntryPoint(), pMD, pDomain);
    }
}


#endif // HAS_REMOTING_PRECODE


#ifdef HAS_FIXUP_PRECODE
void FixupPrecode::Init(MethodDesc* pMD, BaseDomain* pDomain)
{
    WRAPPER_CONTRACT;

    IN_WIN32(m_op = X86_INSTR_CALL_REL32;)  // call PrecodeFixupThunk
    m_type = FixupPrecode::TypePrestub;
    m_padding[0] = X86_INSTR_INT3;
    m_padding[1] = X86_INSTR_INT3;
    IN_WIN64(m_padding[2] = X86_INSTR_INT3;)
    m_pMethodDesc = (TADDR)pMD;

    if (pDomain != NULL)
    {
        m_rel32 = rel32UsingJumpStub(&m_rel32,
            (TADDR)GetEEFuncEntryPoint(PrecodeFixupThunk), pMD, pDomain);
    }
}

BOOL FixupPrecode::SetTargetInterlocked(TADDR target, TADDR expected)
{
    CONTRACTL
    {
        THROWS;         // Creating a JumpStub could throw OutOfMemory
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    INT64 oldValue = *(INT64*)this;
    BYTE* pOldValue = (BYTE*)&oldValue;

    if (pOldValue[OFFSETOF_PRECODE_TYPE] != FixupPrecode::TypePrestub)
        return FALSE;
    
    INT64 newValue = oldValue;
    BYTE* pNewValue = (BYTE*)&newValue;

    pNewValue[OFFSETOF_PRECODE_TYPE] = FixupPrecode::Type;

    pOldValue[0] = X86_INSTR_CALL_REL32;
    pNewValue[0] = X86_INSTR_JMP_REL32;

    *(INT32*)(&pNewValue[1]) = rel32UsingJumpStub(&m_rel32, target, (MethodDesc*)GetMethodDesc());

    _ASSERTE(IS_ALIGNED(this, sizeof(INT64)));
    return FastInterlockCompareExchangeLong((INT64*)this, newValue, oldValue) == oldValue;
}


#endif // HAS_FIXUP_PRECODE

#ifdef HAS_THISPTR_RETBUF_PRECODE

void ThisPtrRetBufPrecode::Init(MethodDesc* pMD, BaseDomain* pDomain)
{
    WRAPPER_CONTRACT;

    m_movEAXECX = 0xC889;               // mov eax,ecx
    m_movECXEDX = 0xD189;               // mov ecx,edx
    m_movEDXEAX = 0xC289;               // mov edx,eax
    m_nop = X86_INSTR_NOP;              // nop
    m_jmp = X86_INSTR_JMP_REL32;        // jmp rel32
    m_pMethodDesc = (TADDR)pMD;

    if (pDomain != NULL)
    {
        m_rel32 = rel32UsingJumpStub(&m_rel32,
            (TADDR)ThePreStub()->GetEntryPoint(), pMD, pDomain);
    }
}

#endif // HAS_THISPTR_RETBUF_PRECODE

#endif // !DACCESS_COMPILE

