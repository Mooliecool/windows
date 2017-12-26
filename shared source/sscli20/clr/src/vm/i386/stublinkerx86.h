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
#ifndef STUBLINKERX86_H_
#define STUBLINKERX86_H_

#include "stublink.h"

struct ArrayOpScript;
struct DeclActionInfo;
class MetaSig;

//=======================================================================

#define X86_INSTR_CALL_REL32    0xE8        // call rel32
#define X86_INSTR_CALL_IND      0x15FF      // call dword ptr[addr32]
#define X86_INSTR_CALL_IND_EAX  0x10FF      // call dword ptr[eax]
#define X86_INSTR_CALL_IND_EAX_OFFSET  0x50FF  // call dword ptr[eax + offset] ; where offset follows these 2 bytes
#define X86_INSTR_CALL_EAX      0xD0FF      // call eax
#define X86_INSTR_JMP_REL32     0xE9        // jmp rel32
#define X86_INSTR_JMP_IND       0x25FF      // jmp dword ptr[addr32]
#define X86_INSTR_JMP_EAX       0xE0FF      // jmp eax
#define X86_INSTR_MOV_EAX_IMM32 0xB8        // mov eax, imm32
#define X86_INSTR_MOV_EAX_IND   0x058B      // mov eax, dword ptr[addr32]
#define X86_INSTR_MOV_EAX_ECX_IND 0x018b    // mov eax, [ecx]        
#define X86_INSTR_MOV_ECX_ECX_OFFSET 0x498B    // mov ecx, [ecx + offset] ; where offset follows these 2 bytes
#define X86_INSTR_MOV_ECX_EAX_OFFSET 0x488B    // mov ecx, [eax + offset] ; where offset follows these 2 bytes
#define X86_INSTR_CMP_IND_ECX_IMM32 0x3981  // cmp [ecx], imm32
#define X86_INSTR_MOV_RM_R      0x89        // mov r/m,reg

#define X86_INSTR_MOV_AL        0xB0        // mov al, imm8
#define X86_INSTR_JMP_REL8      0xEB        // jmp short rel8

#define X86_INSTR_NOP           0x90        // nop
#define X86_INSTR_INT3          0xCC        // int 3
#define X86_INSTR_HLT           0xF4        // hlt


//----------------------------------------------------------------------
// Encodes X86 registers. The numbers are chosen to match Intel's opcode
// encoding.
//----------------------------------------------------------------------
enum X86Reg
{
    kEAX = 0,
    kECX = 1,
    kEDX = 2,
    kEBX = 3,
    // kESP intentionally omitted because of its irregular treatment in MOD/RM
    kEBP = 5,
    kESI = 6,
    kEDI = 7,

#ifdef _X86_
    NumX86Regs = 8,
#endif // _X86_


    // We use "push ecx" instead of "sub esp, sizeof(LPVOID)"
    kDummyPushReg = kECX
};

// Use this only if you are absolutely sure that the instruction format
// handles it. This is not declared as X86Reg so that users are forced
// to add a cast and think about what exactly they are doing.
const int kESP_Unsafe = 4;

//----------------------------------------------------------------------
// Encodes X86 conditional jumps. The numbers are chosen to match
// Intel's opcode encoding.
//----------------------------------------------------------------------
class X86CondCode {
    public:
        enum cc {
            kJA   = 0x7,
            kJAE  = 0x3,
            kJB   = 0x2,
            kJBE  = 0x6,
            kJC   = 0x2,
            kJE   = 0x4,
            kJZ   = 0x4,
            kJG   = 0xf,
            kJGE  = 0xd,
            kJL   = 0xc,
            kJLE  = 0xe,
            kJNA  = 0x6,
            kJNAE = 0x2,
            kJNB  = 0x3,
            kJNBE = 0x7,
            kJNC  = 0x3,
            kJNE  = 0x5,
            kJNG  = 0xe,
            kJNGE = 0xc,
            kJNL  = 0xd,
            kJNLE = 0xf,
            kJNO  = 0x1,
            kJNP  = 0xb,
            kJNS  = 0x9,
            kJNZ  = 0x5,
            kJO   = 0x0,
            kJP   = 0xa,
            kJPE  = 0xa,
            kJPO  = 0xb,
            kJS   = 0x8,
        };
};


//----------------------------------------------------------------------
// StubLinker with extensions for generating X86 code.
//----------------------------------------------------------------------
class StubLinkerCPU : public StubLinker
{
    public:


        VOID X86EmitAddReg(X86Reg reg, INT32 imm32);
        VOID X86EmitSubReg(X86Reg reg, INT32 imm32);
        VOID X86EmitSubRegReg(X86Reg destreg, X86Reg srcReg);

        VOID X86EmitMovRegReg(X86Reg destReg, X86Reg srcReg);
        VOID X86EmitMovSPReg(X86Reg srcReg);
        VOID X86EmitMovRegSP(X86Reg destReg);
        
        VOID X86EmitPushReg(X86Reg reg);
        VOID X86EmitPopReg(X86Reg reg);
        VOID X86EmitPushRegs(unsigned regSet);
        VOID X86EmitPopRegs(unsigned regSet);
        VOID X86EmitPushImm32(UINT value);
        VOID X86EmitPushImm32(CodeLabel &pTarget);
        VOID X86EmitPushImm8(BYTE value);
        VOID X86EmitPushImmPtr(LPVOID value);
        
        VOID X86EmitCmpRegImm32(X86Reg reg, INT32 imm32); // cmp reg, imm32
        VOID X86EmitCmpRegIndexImm32(X86Reg reg, INT32 offs, INT32 imm32); // cmp [reg+offs], imm32
        VOID X86EmitZeroOutReg(X86Reg reg);        
        VOID X86EmitOffsetModRM(BYTE opcode, X86Reg altreg, X86Reg indexreg, __int32 ofs);
        VOID X86EmitOffsetModRmSIB(BYTE opcode, X86Reg opcodeOrReg, X86Reg baseReg, X86Reg indexReg, __int32 scale, __int32 ofs);
        
        VOID X86EmitTailcallWithESPAdjust(CodeLabel *pTarget, INT32 imm32);
        VOID X86EmitTailcallWithSinglePop(CodeLabel *pTarget, X86Reg reg);
        
        VOID X86EmitNearJump(CodeLabel *pTarget);
        VOID X86EmitCondJump(CodeLabel *pTarget, X86CondCode::cc condcode);
        VOID X86EmitCall(CodeLabel *target, int iArgBytes, BOOL returnLabel = FALSE);
        VOID X86EmitReturn(int iArgBytes);

        static const unsigned X86TLSFetch_TRASHABLE_REGS = (1<<kEAX) | (1<<kEDX) | (1<<kECX);
        VOID X86EmitTLSFetch(DWORD idx, X86Reg dstreg, unsigned preservedRegSet);
        VOID X86EmitCurrentThreadFetch(unsigned preservedRegSet);
        //VOID X86EmitSetupThread();
        
        VOID X86EmitIndexRegLoad(X86Reg dstreg, X86Reg srcreg, __int32 ofs);
        VOID X86EmitIndexRegStore(X86Reg dstreg, __int32 ofs, X86Reg srcreg);
        VOID X86EmitIndexPush(X86Reg srcreg, __int32 ofs);
        VOID X86EmitBaseIndexPush(X86Reg baseReg, X86Reg indexReg, __int32 scale, __int32 ofs);
        VOID X86EmitIndexPop(X86Reg srcreg, __int32 ofs);
        VOID X86EmitIndexLea(X86Reg dstreg, X86Reg srcreg, __int32 ofs);

        VOID X86EmitSPIndexPush(__int32 ofs);
        VOID X86EmitSubEsp(INT32 imm32);
        VOID X86EmitAddEsp(INT32 imm32);
        VOID X86EmitEspOffset(BYTE opcode,
                              X86Reg altreg,
                              __int32 ofs
                    AMD64_ARG(X86OperandSize OperandSize = k64BitOp)
                              );
        VOID X86EmitPushEBPframe();
        VOID X86EmitPopEBPframe(bool popReturnAddress);

        // These are used to emit calls to notify the profiler of transitions in and out of
        // managed code through COM->COM+ interop or N/Direct
        VOID EmitProfilerComCallProlog(TADDR pFrameVptr, X86Reg regFrame);
        VOID EmitProfilerComCallEpilog(TADDR pFrameVptr, X86Reg regFrame);



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
        //    

        VOID X86EmitOp(WORD    opcode,
                       X86Reg  altreg,
                       X86Reg  basereg,
                       __int32 ofs = 0,
                       X86Reg  scaledreg = (X86Reg)0,
                       BYTE    scale = 0
             AMD64_ARG(X86OperandSize OperandSize = k32BitOp)
                       );


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

        VOID X86EmitR2ROp(WORD opcode,
                          X86Reg altreg,
                          X86Reg modrmreg
                AMD64_ARG(X86OperandSize OperandSize = k64BitOp)
                          );

        VOID X86EmitRegLoad(X86Reg reg, UINT_PTR imm);

        VOID X86EmitRegSave(X86Reg altreg, __int32 ofs)
        {
            LEAF_CONTRACT;        
            X86EmitEspOffset(0x89, altreg, ofs);
            UnwindSavedReg(altreg, ofs);
        }

        VOID X86_64BitOperands ()
        {
            WRAPPER_CONTRACT;
        }

        VOID EmitEnable(CodeLabel *pForwardRef);
        VOID EmitRareEnable(CodeLabel *pRejoinPoint);

        VOID EmitDisable(CodeLabel *pForwardRef, BOOL fCallIn, X86Reg ThreadReg);
        VOID EmitRareDisable(CodeLabel *pRejoinPoint, BOOL bIsCallIn);
        VOID EmitRareDisableHRESULT(CodeLabel *pRejoinPoint, CodeLabel *pExitPoint);

        VOID X86EmitSetup(CodeLabel *pForwardRef);
        VOID EmitRareSetup(CodeLabel* pRejoinPoint, BOOL fThrow);
        VOID EmitCheckGSCookie(X86Reg frameReg, int gsCookieOffset);

#ifdef _X86_
        void EmitComMethodStubProlog(TADDR pFrameVptr, CodeLabel** rgRareLabels,
                                     CodeLabel** rgRejoinLabels, LPVOID pSEHHandler,
                                     BOOL bShouldProfile);

        void EmitEnterManagedStubEpilog(TADDR pFrameVptr, unsigned numStackBytes,
                    CodeLabel** rgRareLabels, CodeLabel** rgRejoinLabels,
                    BOOL bShouldProfile);

        void EmitComMethodStubEpilog(TADDR pFrameVptr, unsigned numStackBytes,
                            CodeLabel** rgRareLabels, CodeLabel** rgRejoinLabels,
                            LPVOID pSEHHAndler, BOOL bShouldProfile);
#endif

        //========================================================================
        //  void EmitSEHProlog(LPVOID pvFrameHandler)
        //  Prolog for setting up SEH for stubs that enter managed code from unmanaged
        //  assumptions: esi has the current frame pointer
        void EmitSEHProlog(LPVOID pvFrameHandler);

        //===========================================================================
        //  void EmitUnLinkSEH(unsigned offset)
        //  negOffset is the offset from the current frame where the next exception record
        //  pointer is stored in the stack
        //  for e.g. COM to managed frames the pointer to next SEH record is in the stack
        //          after the ComMethodFrame::NegSpaceSize() + 4 ( address of handler)
        //
        //  also assumes ESI is pointing to the current frame
        void EmitUnLinkSEH(unsigned offset);

        VOID EmitMethodStubProlog(TADDR pFrameVptr);
        enum { ARG_SIZE_ON_STACK = -1 };
        VOID EmitMethodStubEpilog(__int16 numArgBytes, StubStyle style);

        VOID EmitUnboxMethodStub(MethodDesc* pRealMD);
#if defined(FEATURE_SHARE_GENERIC_CODE)  
        VOID EmitInstantiatingMethodStub(MethodDesc* pSharedMD, void* extra);
#endif // FEATURE_SHARE_GENERIC_CODE


        LPVOID EmitSecurityWrapperStub(UINT numStackBytes, MethodDesc* pMD, BOOL fToStub, LPVOID pRealStub, DeclActionInfo *pActions);
        LPVOID EmitSecurityInterceptorStub(MethodDesc* pMD, BOOL fToStub, LPVOID pRealStub, DeclActionInfo *pActions);

        //===========================================================================
        // Computes hash code for MulticastDelegate.Invoke()
        static UINT_PTR HashMulticastInvoke(UINT32 numStackBytes, MetaSig* pSig);

        //===========================================================================
        // Emits code for Delegate.Invoke() any delegate type
        VOID EmitDelegateInvoke(BOOL bHasReturnBuffer = FALSE);

        //===========================================================================
        // Emits code for MulticastDelegate.Invoke() - sig specific
        VOID EmitMulticastInvoke(UINT numStackBytes, MetaSig* pSig);

        //===========================================================================
        // Emits code for Delegate.Invoke() on delegates that recorded creator assembly
        VOID EmitSecureDelegateInvoke(UINT numStackBytes, MetaSig* pSig);

        //===========================================================================
        // Emits code to adjust for a static delegate target.
        VOID EmitShuffleThunk(struct ShuffleEntry *pShuffleEntryArray, BOOL fRetainThis);

        //===========================================================================
        // Emits code to capture the lasterror code.
        VOID EmitSaveLastError();


        //===========================================================================
        // Emits code to do an array operation.
        VOID EmitArrayOpStub(const ArrayOpScript*);

        //===========================================================================
        // Emits code to break into debugger
        VOID EmitDebugBreak();

#if defined(_DEBUG) && (defined(_AMD64_) || defined(_X86_))
        //===========================================================================
        // Emits code to log JITHelper access
        void EmitJITHelperLoggingThunk(LPVOID pJitHelper, LPVOID helperFuncCount);
#endif

        //===========================================================================
        // Emits code to touch pages
        // Inputs:
        //   eax = first byte of data
        //   edx = first byte past end of data
        //
        // Trashes eax, edx, ecx
        //
        // Pass TRUE if edx is guaranteed to be strictly greater than eax.
        VOID EmitPageTouch(BOOL fSkipNullCheck);

#ifdef _DEBUG
        VOID X86EmitDebugTrashReg(X86Reg reg);
#endif

#if defined(_DEBUG) && defined(STUBLINKER_GENERATES_UNWIND_INFO)
        virtual VOID EmitUnwindInfoCheckWorker (CodeLabel *pCheckLabel);
        virtual VOID EmitUnwindInfoCheckSubfunction();
#endif

    private:
        VOID X86EmitSubEspWorker(INT32 imm32);

    public:
        static void Init();

};

inline TADDR rel32Decode(/*PTR_INT32*/ TADDR pRel32)
{
    LEAF_CONTRACT;
    return pRel32 + 4 + *PTR_INT32(pRel32);
}

BOOL rel32SetInterlocked(/*PINT32*/ PVOID pRel32, TADDR target, TADDR expected, MethodDesc* pMD);


//------------------------------------------------------------------------
//
// Precode definitions
//
//------------------------------------------------------------------------


EXTERN_C VOID __stdcall PrecodeFixupThunk();


EXTERN_C VOID __stdcall PrecodeRemotingThunk();

#define OFFSETOF_PRECODE_TYPE           5
#define OFFSETOF_PRECODE_TYPE_MOV_RM_R  6

#define SIZEOF_PRECODE_BASE             8



#include <pshpack1.h>

// Invalid precode type
struct InvalidPrecode {
    // int3
    static const int Type = 0xCC;
};


// Regular precode
struct StubPrecode {

    static const int Type = 0xED;
    // mov eax,pMethodDesc
    // mov ebp,ebp
    // jmp Stub

    IN_WIN64(USHORT m_movR10;)
    IN_WIN32(BYTE   m_movEAX;)
    TADDR           m_pMethodDesc;
    IN_WIN32(BYTE   m_mov_rm_r;)
    BYTE            m_type;
    BYTE            m_jmp;
    INT32           m_rel32;

    void Init(MethodDesc* pMD, BaseDomain* pDomain, int type = StubPrecode::Type, TADDR target = NULL);

    TADDR GetMethodDesc()
    {
        LEAF_CONTRACT; 
        return m_pMethodDesc;
    }

    TADDR GetTarget()
    { 
        LEAF_CONTRACT; 
        return rel32Decode(PTR_HOST_MEMBER_TADDR(StubPrecode, this, m_rel32));
    }

    BOOL SetTargetInterlocked(TADDR target, TADDR expected, BOOL fRequiresMethodDescCallingConvention)
    {
        WRAPPER_CONTRACT;
        return rel32SetInterlocked(&m_rel32, target, expected, (MethodDesc*)GetMethodDesc());
    }

};
IN_WIN64(C_ASSERT(offsetof(StubPrecode, m_movR10) == OFFSETOF_PRECODE_TYPE);)
IN_WIN64(C_ASSERT(offsetof(StubPrecode, m_type) == OFFSETOF_PRECODE_TYPE_MOV_R10);)
IN_WIN32(C_ASSERT(offsetof(StubPrecode, m_mov_rm_r) == OFFSETOF_PRECODE_TYPE);)
IN_WIN32(C_ASSERT(offsetof(StubPrecode, m_type) == OFFSETOF_PRECODE_TYPE_MOV_RM_R);)
typedef DPTR(StubPrecode) PTR_StubPrecode;


#ifdef HAS_NDIRECT_IMPORT_PRECODE

// NDirect import precode
// (This is fake precode. VTable slot does not point to it.)
struct NDirectImportPrecode : StubPrecode {

    static const int Type = 0xC0;
    // mov eax,pMethodDesc
    // mov eax,eax
    // jmp Prestub/Stub/NativeCode

    void Init(MethodDesc* pMD, BaseDomain* pDomain);

    LPVOID GetEntrypoint()
    {
        LEAF_CONTRACT;
        return this;
    }

};
typedef DPTR(NDirectImportPrecode) PTR_NDirectImportPrecode;

#endif // HAS_NDIRECT_IMPORT_PRECODE


#ifdef HAS_REMOTING_PRECODE

// Precode with embedded remoting interceptor
struct RemotingPrecode {

    static const int Type = 0x90;
    // mov eax,pMethodDesc
    // call PrecodeRemotingThunk
    // nop
    // jmp Prestub/Stub/NativeCode

    IN_WIN64(USHORT m_movR10;)
    IN_WIN32(BYTE   m_movEAX;)
    TADDR           m_pMethodDesc;
    BYTE            m_type;
    BYTE            m_call;
    INT32           m_callRel32;
    BYTE            m_jmp;
    INT32           m_rel32;

    void Init(MethodDesc* pMD, BaseDomain* pDomain);

    TADDR GetMethodDesc()
    {
        LEAF_CONTRACT; 
        return m_pMethodDesc;
    }

    TADDR GetTarget()
    { 
        LEAF_CONTRACT; 
        return rel32Decode(PTR_HOST_MEMBER_TADDR(RemotingPrecode, this, m_rel32));
    }

    BOOL SetTargetInterlocked(TADDR target, TADDR expected)
    {
        WRAPPER_CONTRACT;
        return rel32SetInterlocked(&m_rel32, target, expected, (MethodDesc*)GetMethodDesc());
    }

};
IN_WIN64(C_ASSERT(offsetof(RemotingPrecode, m_movR10) == OFFSETOF_PRECODE_TYPE);)
IN_WIN64(C_ASSERT(offsetof(RemotingPrecode, m_type) == OFFSETOF_PRECODE_TYPE_MOV_R10);)
IN_WIN32(C_ASSERT(offsetof(RemotingPrecode, m_type) == OFFSETOF_PRECODE_TYPE);)
typedef DPTR(RemotingPrecode) PTR_RemotingPrecode;

#endif // HAS_REMOTING_PRECODE


#ifdef HAS_FIXUP_PRECODE

// Fixup precode is used in ngen images when the prestub does just one time fixup.
// The fixup precode is simple jump once patched. It does not have the two instruction overhead of regular precode.
struct FixupPrecode {

    static const int TypePrestub = 0x5E;
    // The entrypoint has to be 8-byte aligned so that the "call PrecodeFixupThunk" can be patched to "jmp NativeCode" atomically.
    // call PrecodeFixupThunk
    // db TypePrestub (pop esi)
    // int3's (padding)
    // dw pMethodDesc

    static const int Type = 0x5F;
    // After it has been patched to point to native code
    // jmp NativeCode
    // db Type (pop edi)
    // int3's (padding)
    // dw pMethodDesc

    IN_WIN64(BYTE   m_type;)
    IN_WIN32(BYTE   m_op;)
    INT32           m_rel32;
    IN_WIN64(BYTE   m_padding[3];)
    IN_WIN32(BYTE   m_type;)
    IN_WIN32(BYTE   m_padding[2];)
    TADDR           m_pMethodDesc;

    void Init(MethodDesc* pMD, BaseDomain* pDomain);

    TADDR GetMethodDesc()
    {
        LEAF_CONTRACT; 
        return m_pMethodDesc;
    }

    TADDR GetTarget();

    BOOL SetTargetInterlocked(TADDR target, TADDR expected);

};
C_ASSERT(offsetof(FixupPrecode, m_type) == OFFSETOF_PRECODE_TYPE);
typedef DPTR(FixupPrecode) PTR_FixupPrecode;

#endif // HAS_FIXUP_PRECODE

#ifdef HAS_THISPTR_RETBUF_PRECODE

// Precode to stuffle this and retbuf for closed delegates over static methods with return buffer
struct ThisPtrRetBufPrecode {

    static const int Type = 0xC2;
    // mov eax,ecx
    // mov ecx,edx
    // mov edx,eax
    // nop
    // jmp EntryPoint
    // dw pMethodDesc

    WORD            m_movEAXECX;
    WORD            m_movECXEDX;
    WORD            m_movEDXEAX;
    BYTE            m_nop;
    BYTE            m_jmp;
    INT32           m_rel32;
    TADDR           m_pMethodDesc;

    void Init(MethodDesc* pMD, BaseDomain* pDomain);

    TADDR GetMethodDesc()
    {
        LEAF_CONTRACT; 
        return m_pMethodDesc;
    }

    TADDR GetTarget()
    { 
        LEAF_CONTRACT; 
        return rel32Decode(PTR_HOST_MEMBER_TADDR(ThisPtrRetBufPrecode, this, m_rel32));
    }

    BOOL SetTargetInterlocked(TADDR target, TADDR expected)
    {
        WRAPPER_CONTRACT;
        return rel32SetInterlocked(&m_rel32, target, expected, (MethodDesc*)GetMethodDesc());
    }

};
IN_WIN32(C_ASSERT(offsetof(ThisPtrRetBufPrecode, m_movEDXEAX) + 1 == OFFSETOF_PRECODE_TYPE);)
typedef DPTR(ThisPtrRetBufPrecode) PTR_ThisPtrRetBufPrecode;

#endif // HAS_THISPTR_RETBUF_PRECODE


#include <poppack.h>




#endif  // STUBLINKERX86_H_
