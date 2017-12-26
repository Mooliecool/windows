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
// CGENPPC.H -
//
// Various helper routines for generating ppc assembly code.
//
// DO NOT INCLUDE THIS FILE DIRECTLY - ALWAYS USE CGENSYS.H INSTEAD
//


#ifndef _PPC_
#error Should only include "cgenppc.h" for PPC builds
#endif

#ifndef __cgenppc_h__
#define __cgenppc_h__


#define INSTRFMT_K26

#include "stublink.h"

// preferred alignment for data
#define DATA_ALIGNMENT 4

class MethodDesc;
class FramedMethodFrame;
class Module;
struct ArrayOpScript;
struct DeclActionInfo;
class MetaSig;

// default return value type
typedef INT64 PlatformDefaultReturnType;

// CPU-dependent functions
Stub * GeneratePrestub();

#define GetEEFuncEntryPoint(pfn) ((LPVOID)(pfn))

#define METHOD_ENTRY_CHUNKS                     0 // 1 if we place method entry points in chunka
#define PREPAD_IS_CALLABLE                      1 // 1 if we place a method entry point in the prepad

#define METHOD_PREPAD                           24 // # extra bytes to allocate in addition to sizeof(Method)
#define METHOD_ALIGN                            8 // required alignment for StubCallInstrs

#define STACK_ALIGN_SIZE                        16

#define JUMP_ALLOCATE_SIZE                      20 // # bytes to allocate for a jump instruction
#define IND_JUMP_ALLOCATE_SIZE                  16 // # bytes to allocate for an indirect jump instrucation
#define BACK_TO_BACK_JUMP_ALLOCATE_SIZE         JUMP_ALLOCATE_SIZE  // # bytes to allocate for a back to back jump instruction
#define METHOD_DESC_CHUNK_ALIGNPAD_BYTES        0 // # bytes required to pad MethodDescChunk to correct size
#define METHOD_ENTRY_CHUNK_ALIGNPAD_BYTES       0 // # bytes required to pad MethodDescChunk to correct size

#define CODE_SIZE_ALIGN                         4
#define CACHE_LINE_SIZE                         32
#define LOG2SLOT                                LOG2_PTRSIZE

#define TRACK_FLOATING_POINT_REGISTERS

#define THISPTR_LOCATION                        1   // This pointer is passed after the return buffer
#define CALLDESCR_ARGREGS                       1   // CallDescrWorker has ArgumentRegister parameter
#define CALLDESCR_RETBUFMARK                    1   // lowest bit of target is ret buf flag in CallDescrWorker

//=======================================================================
// IMPORTANT: This value is used to figure out how much to allocate
// for a fixed array of FieldMarshaler's. That means it must be at least
// as large as the largest FieldMarshaler subclass. This requirement
// is guarded by an assert.
//=======================================================================
#define MAXFIELDMARSHALERSIZE               24

//**********************************************************************
// Parameter size
//**********************************************************************

typedef INT32 StackElemType;
#define STACK_ELEM_SIZE sizeof(StackElemType)


// !! This expression assumes STACK_ELEM_SIZE is a power of 2.
#define StackElemSize(parmSize) (((parmSize) + STACK_ELEM_SIZE - 1) & ~((ULONG)(STACK_ELEM_SIZE - 1)))

// The stack should be aligned on 16 bytes
#define AlignStack(stack)  (((stack)+(STACK_ALIGN_SIZE-1))&~(STACK_ALIGN_SIZE-1))

inline BYTE *getStubJumpAddr(BYTE *pBuf) {
    LEAF_CONTRACT;

    return pBuf;
}

//**********************************************************************
// Frames
//**********************************************************************

#include "eecallconv.h"

//--------------------------------------------------------------------
// This represents some of the FramedMethodFrame fields that are
// stored at negative offsets.
//--------------------------------------------------------------------
typedef DPTR(struct CalleeSavedRegisters) PTR_CalleeSavedRegisters;
struct CalleeSavedRegisters {
    INT32   cr;                                 // cr
    INT32   r[NUM_CALLEESAVED_REGISTERS];       // r13 .. r31
    DOUBLE  f[NUM_FLOAT_CALLEESAVED_REGISTERS]; // fpr14 .. fpr31
};

//--------------------------------------------------------------------
// This represents the arguments that are stored in volatile registers.
// This should not overlap the CalleeSavedRegisters since those are already
// saved separately and it would be wasteful to save the same register twice.
// If we do use a non-volatile register as an argument, then the ArgIterator
// will probably have to communicate this back to the PromoteCallerStack
// routine to avoid a double promotion.
//--------------------------------------------------------------------
typedef DPTR(struct ArgumentRegisters) PTR_ArgumentRegisters;
struct ArgumentRegisters {
    INT32   r[NUM_ARGUMENT_REGISTERS];          // r3 .. r10
    DOUBLE  f[NUM_FLOAT_ARGUMENT_REGISTERS];    // fpr1 .. fpr13
};

//--------------------------------------------------------------------
// PPC linkage area
//--------------------------------------------------------------------
typedef DPTR(struct LinkageArea) PTR_LinkageArea;
struct LinkageArea {
    INT32 SavedSP; // stack pointer
    INT32 SavedCR; // flags
    INT32 SavedLR; // return address
    INT32 Reserved1;
    INT32 Reserved2;
    INT32 Reserved3;
};

// forward decl
typedef struct _REGDISPLAY REGDISPLAY, *PREGDISPLAY;

// Sufficient context for Try/Catch restoration.
struct EHContext {
    // R1 = SP
    // R12 = IP
    // R30 = FP

    INT32       R[32];
    DOUBLE      F[NUM_FLOAT_CALLEESAVED_REGISTERS]; // fpr14 .. fpr31
    INT32       CR;

    void Setup(LPVOID resumePC, PREGDISPLAY regs);

    inline LPVOID GetSP() {
        LEAF_CONTRACT;

        return (LPVOID)(UINT_PTR)R[1];
    }
    inline void SetSP(LPVOID esp) {
        LEAF_CONTRACT;

        R[1] = (ULONG)(size_t)esp;
    }

    inline LPVOID GetFP() {
        LEAF_CONTRACT;

        return (LPVOID)(UINT_PTR)R[30];
    }

    inline void SetArg(LPVOID arg) {
        LEAF_CONTRACT;

        R[11] = (UINT32)(size_t)arg;
    }
};


#define ARGUMENTREGISTERS_SIZE sizeof(ArgumentRegisters)


//**********************************************************************
// Exception handling
//**********************************************************************

inline LPVOID GetIP(CONTEXT *context) {
    LEAF_CONTRACT;

    return (LPVOID)(size_t)context->Iar;
}

inline void SetIP(CONTEXT *context, LPVOID eip) {
    LEAF_CONTRACT;

    context->Iar = (ULONG)(size_t)eip;
}

inline LPVOID GetSP(CONTEXT *context) {
    LEAF_CONTRACT;

    return (LPVOID)(size_t)context->Gpr1;
}

extern "C" LPVOID GetCurrentSP();

inline void SetSP(CONTEXT *context, LPVOID esp) {
    LEAF_CONTRACT;

    context->Gpr1 = (ULONG)(size_t)esp;
}

inline LPVOID GetFP(PCONTEXT context) {
    LEAF_CONTRACT;

    return (LPVOID)(UINT_PTR)context->Gpr30;
}

inline void SetFP(PCONTEXT context, LPVOID ebp) {
    LEAF_CONTRACT;

    context->Gpr30 = (ULONG)(size_t)ebp;
}

inline void emitCall(LPBYTE pBuffer, LPVOID target)
{
    WRAPPER_CONTRACT;

    UINT32* p = (UINT32*)pBuffer;
    p[0] = 0x7D8B6378; // mr r11, r12
    p[1] = 0x818C0010; // lwz r12, 16(r12)
    p[2] = 0x7D8903A6; // mtctr r12
    p[3] = 0x4E800420; // bctr
    p[4] = (UINT32)(size_t)target; // <actual code address>
    FlushInstructionCache(GetCurrentProcess(), p, 16);
}

inline LPVOID getCallTarget(const BYTE *pCall)
{
    LEAF_CONTRACT;

    UINT32* pCode = (UINT32*)pCall;
    _ASSERTE(pCode[0] == 0x7D8B6378);
    _ASSERTE(pCode[1] == 0x818C0010);
    _ASSERTE(pCode[2] == 0x7D8903A6);
    _ASSERTE(pCode[3] == 0x4E800420);
    return (LPVOID)(size_t)(pCode[4]);
}

inline void emitJump(LPBYTE pBuffer, LPVOID target)
{
    WRAPPER_CONTRACT;

    emitCall(pBuffer, target);
}

//  Given the same pBuffer that was used by emitJump this method
//  decodes the instructions and returns the jump target
inline BYTE* decodeJump(BYTE* pBuffer)
{
    LEAF_CONTRACT;

    UINT32* p = (UINT32*)pBuffer;
    return (BYTE*)(size_t)(UINT32)p[4];
}

inline void emitBackToBackJump(LPBYTE pBuffer, LPVOID target)
{
    WRAPPER_CONTRACT;

    emitJump(pBuffer, target);
}

inline BYTE* decodeBackToBackJump(BYTE* pBuffer)
{
    WRAPPER_CONTRACT;
    
    return decodeJump(pBuffer);
}

inline void emitIndJump(LPBYTE pBuffer, LPVOID targetptr)
{
    LEAF_CONTRACT;

    UINT32 ha16 = ((1<<15) + (UINT32)(size_t)targetptr) >> 16;

    UINT32* p = (UINT32*)pBuffer;
    p[0] = 0x3D800000 | ha16; // addis r12, 0, ha16(targetptr)
    p[1] = 0x818C0000 | (UINT16)((size_t)targetptr - (ha16 << 16)); // lwz r12, lo16(targetptr)(r12)
    p[2] = 0x7D8903A6; // mtctr r12
    p[3] = 0x4E800420; // bctr
}

EXTERN_C void __stdcall setFPReturn(int fpSize, INT64 retVal);
EXTERN_C void __stdcall getFPReturn(int fpSize, INT64 *pretval);

//----------------------------------------------------------------------
// Encodes PPC registers.
//----------------------------------------------------------------------
enum PPCReg {
    kR0 = 0,
    kR1 = 1,
    kR2 = 2,
    kR3 = 3,
    kR4 = 4,
    kR5 = 5,
    kR6 = 6,
    kR7 = 7,
    kR8 = 8,
    kR9 = 9,
    kR10 = 10,
    kR11 = 11,
    kR12 = 12,
    kR13 = 13,
    kR14 = 14,
    kR15 = 15,
    kR16 = 16,
    kR17 = 17,
    kR18 = 18,
    kR19 = 19,
    kR20 = 20,
    kR21 = 21,
    kR22 = 22,
    kR23 = 23,
    kR24 = 24,
    kR25 = 25,
    kR26 = 26,
    kR27 = 27,
    kR28 = 28,
    kR29 = 29,
    kR30 = 30,
    kR31 = 31,
};




//----------------------------------------------------------------------
// Encodes PPC conditional jumps.
//----------------------------------------------------------------------
class PPCCondCode {
    public:
        enum cc {
            kGE     = 0x00800000,
            kLT     = 0x01800000,

            kLE     = 0x00810000,
            kGT     = 0x01810000,

            kNE     = 0x00820000,
            kEQ     = 0x01820000,

            kAlways = 0x02800000
        };
};


//----------------------------------------------------------------------
// StubLinker with extensions for generating PPC code.
//----------------------------------------------------------------------
class StubLinkerCPU : public StubLinker
{
    public:
        // helper for emitting instructions with reg, reg, disp16 format
        VOID PPCEmitRegRegDisp16(UINT32 code, PPCReg r1, PPCReg r2, int disp16);

        // helper for emitting instructions with reg, reg, reg format
        VOID PPCEmitRegRegReg(UINT32 code, PPCReg r1, PPCReg r2, PPCReg r3);

        // rD = *(rA+d)
        VOID PPCEmitLwz(PPCReg rD, PPCReg rA, int d = 0);

        // rD = rA+d
        VOID PPCEmitAddi(PPCReg rD, PPCReg rA, int d = 0);

        // cr = (rA - d)
        VOID PPCEmitCmpwi(PPCReg rA, int d = 0);

        // cr = (rA - rB)
        VOID PPCEmitCmpw(PPCReg rA, PPCReg rB);

        // *(rA+d) = rS
        VOID PPCEmitStw(PPCReg rS, PPCReg rA, int d = 0);

        // rA = rS
        VOID PPCEmitMr(PPCReg rA, PPCReg rS);

        // rD = value
        VOID PPCEmitLoadImm(PPCReg rD, INT value);

        // rD = value
        // special flavor of LoadImm for stubs that have to have predictable layout
        VOID PPCEmitLoadImmNonoptimized(PPCReg rD, INT value);

        // call target
        VOID PPCEmitCall(CodeLabel *target);

        // conditional branch
        VOID PPCEmitBranch(CodeLabel *target, PPCCondCode::cc cond = PPCCondCode::kAlways);

        // indirect branch
        VOID PPCEmitBranchR12();

        // indirect call
        VOID PPCEmitCallR12();

        // rD = GetThread()
        VOID PPCEmitCurrentThreadFetch(PPCReg rD);

        // *(ArgumentRegisters*)(basereg + ofs) = <machine state>
        VOID EmitArgumentRegsSave(PPCReg basereg, int ofs);
        // <machine state> = *(ArgumentRegisters*)(basereg + ofs)
        VOID EmitArgumentRegsRestore(PPCReg basereg, int ofs);

        // *(CalleeSavedRegisters*)(basereg + ofs) = <machine state>
        VOID EmitCalleeSavedRegsSave(PPCReg basereg, int ofs);
        // <machine state> = *(CalleeSavedRegisters*)(basereg + ofs)
        VOID EmitCalleeSavedRegsRestore(PPCReg basereg, int ofs);

        // These are used to emit calls to notify the profiler of transitions in and out of
        // managed code through COM->COM+ interop or N/Direct
        VOID EmitProfilerComCallProlog(TADDR pFrameVptr);
        VOID EmitProfilerComCallEpilog(TADDR pFrameVptr);

        void EmitComMethodStubProlog(TADDR pFrameVptr, LPVOID pSEHHandler,
                                     BOOL bShouldProfile);

        void EmitComMethodStubEpilog(TADDR pFrameVptr,
                            LPVOID pSEHHAndler, BOOL bShouldProfile, BOOL bReloop);

        VOID EmitMethodStubProlog(TADDR pFrameVptr, BOOL fDeregister = TRUE);
        VOID EmitMethodStubEpilog(StubStyle style);

        VOID EmitUnboxMethodStub(MethodDesc* pRealMD);

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
        // Emits code to do an array operation.
        VOID EmitArrayOpStub(const ArrayOpScript*);

        //===========================================================================
        // Emits code to break into debugger
        VOID EmitDebugBreak();

    public:
        static void Init();

    private:
        PPCReg ArrayOpEnregisterFromOffset(UINT offset, PPCReg regScratch);
};

// helper function for decodable stubs
void PPCEmitLoadImmNonoptimized(DWORD *pCode, PPCReg r, INT value);
BOOL PPCIsLoadImmNonoptimized(DWORD *pCode, PPCReg r);
INT PPCCrackLoadImmNonoptimized(DWORD *pCode);


// Adjust the generic interlocked operations for any platform specific ones we
// might have.
void InitFastInterlockOps();



// SEH info forward declarations

struct ComToManagedExRecord; // defined in cgenppc.cpp

inline BOOL IsUnmanagedValueTypeReturnedByRef(UINT sizeofvaluetype)
{
    LEAF_CONTRACT;

    return TRUE;
}

inline BOOL IsManagedValueTypeReturnedByRef(UINT sizeofvaluetype)
{
    LEAF_CONTRACT;

    return TRUE;
}

struct UMEntryThunkCode
{

    DWORD           m_code[5];
    const BYTE *    m_execstub; // pointer to destination code

    void Encode(BYTE* pTargetCode, void* pvSecretParam);

    LPCBYTE GetEntryPoint() const
    {
        LEAF_CONTRACT;

        return (LPCBYTE)this;
    }

    static int GetEntryPointOffset()
    {
        LEAF_CONTRACT;

        return 0;
    }

    static int GetExecutableCodeOffset()
    {
        LEAF_CONTRACT;

        return offsetof(UMEntryThunkCode, m_code);
    }
};

// ClrFlushInstructionCache is used when we want to call FlushInstructionCache
// for a specific architecture in the common code, but not for other architectures.
// On IA64 ClrFlushInstructionCache calls the Kernel FlushInstructionCache function
// to flush the instruction cache. 
// We call ClrFlushInstructionCache whenever we create or modify code in the heap. 
// Currently ClrFlushInstructionCache has no effect on PPC
//

inline BOOL ClrFlushInstructionCache(LPCVOID pCodeAddr, size_t sizeOfCode)
{
    // FlushInstructionCache(GetCurrentProcess(), pCodeAddr, sizeOfCode);
    return TRUE;
}

//------------------------------------------------------------------------
//
// Precode definitions
//
//------------------------------------------------------------------------

#define PRECODE_ALIGNMENT           CODE_SIZE_ALIGN

#define SIZEOF_PRECODE_BASE         CODE_SIZE_ALIGN

#include <pshpack1.h>

// Invalid precode type
struct InvalidPrecode {
    static const int Type = 0;
};

struct StubPrecode {

    static const int Type = 1;

    // lwz r12, 16(r12)
    // lwz r11, 20(r12)
    // mtctr r12
    // bctr

    DWORD       m_code[4];

    TADDR       m_pTarget;
    TADDR       m_pMethodDesc;

    void Init(MethodDesc* pMD, BaseDomain* pDomain);

    TADDR GetMethodDesc()
    {
        LEAF_CONTRACT; 
        return m_pMethodDesc;
    }

    TADDR GetTarget()
    {
        LEAF_CONTRACT;
        return m_pTarget;
    }

    BOOL SetTargetInterlocked(TADDR target, TADDR expected, BOOL fRequiresMethodDescCallingConvention);
};
typedef DPTR(StubPrecode) PTR_StubPrecode;


#include <poppack.h>

#endif // __cgenppc_h__
