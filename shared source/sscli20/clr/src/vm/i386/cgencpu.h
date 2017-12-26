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
// CGENX86.H -
//
// Various helper routines for generating x86 assembly code.
//
// DO NOT INCLUDE THIS FILE DIRECTLY - ALWAYS USE CGENSYS.H INSTEAD
//


#ifndef _X86_
#error Should only include "cgenx86.h" for X86 builds
#endif // _X86_

#ifndef __cgenx86_h__
#define __cgenx86_h__

#include "utilcode.h"

// preferred alignment for data
#define DATA_ALIGNMENT 4

class MethodDesc;
class FramedMethodFrame;
class Module;
class ComCallMethodDesc;
class BaseDomain;

// default return value type
typedef INT64 PlatformDefaultReturnType;

// CPU-dependent functions
Stub * GeneratePrestub();

BOOL Runtime_Test_For_SSE2();

#define GetEEFuncEntryPoint(pfn) ((LPVOID)(pfn))

//**********************************************************************
// To be used with GetSpecificCpuInfo()

#define CPU_X86_FAMILY(cpuType)     (((cpuType) & 0x0F00) >> 8)
#define CPU_X86_MODEL(cpuType)      (((cpuType) & 0x00F0) >> 4)
// Stepping is masked out by GetSpecificCpuInfo()
// #define CPU_X86_STEPPING(cpuType)   (((cpuType) & 0x000F)     )

#define CPU_X86_USE_CMOV(cpuFeat)   ((cpuFeat & 0x00008001) == 0x00008001)
#define CPU_X86_USE_SSE2(cpuFeat)  (((cpuFeat & 0x04000000) == 0x04000000) && Runtime_Test_For_SSE2())

// Values for CPU_X86_FAMILY(cpuType)
#define CPU_X86_486                 4
#define CPU_X86_PENTIUM             5
#define CPU_X86_PENTIUM_PRO         6
#define CPU_X86_PENTIUM_4           0xF

// Values for CPU_X86_MODEL(cpuType) for CPU_X86_PENTIUM_PRO
#define CPU_X86_MODEL_PENTIUM_PRO_BANIAS    9 // Pentium M (Mobile PPro with P4 feautres)

#define METHOD_ENTRY_CHUNKS                     0   // 1 if we place method entry points in chunka
#define PREPAD_IS_CALLABLE                      1   // 1 if we place a method entry point in the prepad

#define METHOD_PREPAD                           0

#define METHOD_ALIGN                            8   // required alignment for StubCallInstrs
#define COMMETHOD_PREPAD                        8   // # extra bytes to allocate in addition to sizeof(ComCallMethodDesc)

#define STACK_ALIGN_SIZE                        4

#define JUMP_ALLOCATE_SIZE                      8   // # bytes to allocate for a jump instruction
#define IND_JUMP_ALLOCATE_SIZE                  6   // # bytes to allocate for an indirect jump instruction
#define BACK_TO_BACK_JUMP_ALLOCATE_SIZE         8   // # bytes to allocate for a back to back jump instruction
#define METHOD_DESC_CHUNK_ALIGNPAD_BYTES        0   // # bytes required to pad MethodDescChunk to correct size
#define METHOD_ENTRY_CHUNK_ALIGNPAD_BYTES       0   // # bytes required to pad MethodDescChunk to correct size
#define COMPLUSCALL_METHOD_DESC_ALIGNPAD_BYTES  3   // # bytes required to pad ComPlusCallMethodDesc to correct size

#define HAS_COMPACT_ENTRYPOINTS                 1


// Do not use advanced types of precodes with FJIT to verify portability
#ifndef FJITONLY
#define HAS_REMOTING_PRECODE                    1
#endif

// ThisPtrRetBufPrecode one is necessary for closed delegates over static methods with return buffer
#define HAS_THISPTR_RETBUF_PRECODE              1

#define CODE_SIZE_ALIGN                         4
#define CACHE_LINE_SIZE                         32  // As per Intel Optimization Manual the cache line size is 32 bytes
#define LOG2SLOT                                LOG2_PTRSIZE

#define THISPTR_LOCATION                        -1  // This pointer is passed in designated register
#define CALLDESCR_ARGREGS                       1   // CallDescrWorker has ArgumentRegister parameter

#define UNMANAGED_CALLCONV_CAN_DIFFER_FROM_MANAGED_CALLCONV

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



#include "stublinkerx86.h"



// !! This expression assumes STACK_ELEM_SIZE is a power of 2.
#define StackElemSize(parmSize) (((parmSize) + STACK_ELEM_SIZE - 1) & ~((ULONG)(STACK_ELEM_SIZE - 1)))


inline BYTE *getStubJumpAddr(BYTE *pBuf) {
    LEAF_CONTRACT;

    // We have allocated 8 bytes,
    // and we require that the 4-byte jump displacement be 4-byte aligned
    // so go in 3 to find jmp instr point

    return ((BYTE*)pBuf) + 3;
}

//**********************************************************************
// Frames
//**********************************************************************
//--------------------------------------------------------------------
// This represents some of the FramedMethodFrame fields that are
// stored at negative offsets.
//--------------------------------------------------------------------
typedef DPTR(struct CalleeSavedRegisters) PTR_CalleeSavedRegisters;
struct CalleeSavedRegisters {
    INT32       edi;
    INT32       esi;
    INT32       ebx;
    INT32       ebp;
};

//--------------------------------------------------------------------
// This represents the arguments that are stored in volatile registers.
// This should not overlap the CalleeSavedRegisters since those are already
// saved separately and it would be wasteful to save the same register twice.
// If we do use a non-volatile register as an argument, then the ArgIterator
// will probably have to communicate this back to the PromoteCallerStack
// routine to avoid a double promotion.
//
//--------------------------------------------------------------------
typedef DPTR(struct ArgumentRegisters) PTR_ArgumentRegisters;
struct ArgumentRegisters {

#define DEFINE_ARGUMENT_REGISTER_BACKWARD(regname)  INT32 regname;
#include "eecallconv.h"

};

// forward decl
typedef struct _REGDISPLAY REGDISPLAY, *PREGDISPLAY;

// Sufficient context for Try/Catch restoration.
struct EHContext {
    INT32       Eax;
    INT32       Ebx;
    INT32       Ecx;
    INT32       Edx;
    INT32       Esi;
    INT32       Edi;
    INT32       Ebp;
    INT32       Esp;
    INT32       Eip;

    void Setup(LPVOID resumePC, PREGDISPLAY regs);

    inline LPVOID GetSP() {
        LEAF_CONTRACT;
        return (LPVOID)(UINT_PTR)Esp;
    }
    inline void SetSP(LPVOID esp) {
        LEAF_CONTRACT;
        Esp = (INT32)(size_t)esp;
    }

    inline LPVOID GetFP() {
        LEAF_CONTRACT;
        return (LPVOID)(UINT_PTR)Ebp;
    }

    inline void SetArg(LPVOID arg) {
        LEAF_CONTRACT;
        Eax = (INT32)(size_t)arg;
    }
};

#define ARGUMENTREGISTERS_SIZE sizeof(ArgumentRegisters)

//-----------------------------------------------------------------------
// Stubs always push 8 additional bytes of info in order to
// allow debuggers to stacktrace through stubs. This info
// is pushed right after the callee-saved-registers. The stubs
// also must keep ebp pointed to this structure. Note that this
// precludes the use of ebp by the stub itself.
//-----------------------------------------------------------------------
struct VC5Frame
{
    INT32      m_savedebp;
    INT32      m_returnaddress;
};
#define VC5FRAME_SIZE   sizeof(VC5Frame)

//**********************************************************************
// Exception handling
//**********************************************************************

inline LPVOID GetIP(CONTEXT *context) {
    LEAF_CONTRACT;

    return (LPVOID)(size_t)(context->Eip);
}

inline void SetIP(CONTEXT *context, LPVOID eip) {
    LEAF_CONTRACT;

    context->Eip = (INT32)(size_t)eip;
}

inline LPVOID GetSP(CONTEXT *context) {
    LEAF_CONTRACT;

    return (LPVOID)(size_t)(context->Esp);
}

extern "C" LPVOID __stdcall GetCurrentSP();

inline void SetSP(CONTEXT *context, LPVOID esp) {
    LEAF_CONTRACT;

    context->Esp = (INT32)(size_t)esp;
}

inline void SetFP(CONTEXT *context, LPVOID ebp) {
    LEAF_CONTRACT;

    context->Ebp = (INT32)(size_t)ebp;
}

inline LPVOID GetFP(CONTEXT* context)
{
    LEAF_CONTRACT;

    return (LPVOID)(INT_PTR)context->Ebp;
}

// Get Rel32 destination, emit jumpStub if necessary
inline INT32 rel32UsingJumpStub(INT32 UNALIGNED * pRel32, TADDR target, MethodDesc *pMethod = NULL, BaseDomain *pDomain = NULL)
{
    // We do not need jump stubs on i386
    LEAF_CONTRACT;

    BYTE* baseAddr = (BYTE*)pRel32 + 4;
    return (INT32)((BYTE*)target - baseAddr);
}

inline void emitCall(LPBYTE pBuffer, LPVOID target)
{
    LEAF_CONTRACT;

    pBuffer[0] = X86_INSTR_CALL_REL32; //CALLNEAR32
    *((LPVOID*)(1+pBuffer)) = (LPVOID) (((LPBYTE)target) - (pBuffer+5));
}


//------------------------------------------------------------------------
WORD GetUnpatchedCodeData(LPCBYTE pAddr);

//------------------------------------------------------------------------
inline WORD GetUnpatchedOpcodeWORD(LPCBYTE pAddr)
{
    WRAPPER_CONTRACT;
    if (CORDebuggerAttached())
    {
        return GetUnpatchedCodeData(pAddr);
    }
    else
    {
        return *((WORD *)pAddr);
    }
}

//------------------------------------------------------------------------
inline BYTE GetUnpatchedOpcodeBYTE(LPCBYTE pAddr)
{
    WRAPPER_CONTRACT;
    if (CORDebuggerAttached())
    {
        return (BYTE) GetUnpatchedCodeData(pAddr);
    }
    else
    {
        return *pAddr;
    }
}

 //------------------------------------------------------------------------
// The following must be a distinguishable set of instruction sequences for
// various stub dispatch calls.
//
// An x86 JIT which uses full stub dispatch must generate only
// the following stub dispatch calls:
//
// (1) isCallRelativeIndirect:
//        call dword ptr [rel32]  ;  FF 15 ---rel32----
// (2) isCallRelative:
//        call abc                ;     E8 ---rel32----
// (3) isCallRegisterIndirect:
//     nop
//     nop
//     nop 
//     call dword ptr [eax]       ;     90 90 90  FF 10
//
// NOTE: You must be sure that pRetAddr is a true return address for
// a stub dispatch call.

BOOL isCallRelativeIndirect(const BYTE *pRetAddr);
BOOL isCallRelative(const BYTE *pRetAddr);
BOOL isCallRegisterIndirect(const BYTE *pRetAddr);

inline BOOL isCallRelativeIndirect(const BYTE *pRetAddr)
{
    LEAF_CONTRACT;

    BOOL fRet = (GetUnpatchedOpcodeWORD(&pRetAddr[-6]) == X86_INSTR_CALL_IND);
    _ASSERTE(!fRet || !isCallRelative(pRetAddr));
    _ASSERTE(!fRet || !isCallRegisterIndirect(pRetAddr));
    return fRet;
}

inline BOOL isCallRelative(const BYTE *pRetAddr)
{
    LEAF_CONTRACT;

    BOOL fRet = (GetUnpatchedOpcodeBYTE(&pRetAddr[-5]) == X86_INSTR_CALL_REL32);
    _ASSERTE(!fRet || !isCallRelativeIndirect(pRetAddr));
    _ASSERTE(!fRet || !isCallRegisterIndirect(pRetAddr));
    return fRet;
}

inline BOOL isCallRegisterIndirect(const BYTE *pRetAddr)
{
    LEAF_CONTRACT;

    BOOL fRet = (GetUnpatchedOpcodeBYTE(&pRetAddr[-5]) == X86_INSTR_NOP)
             && (GetUnpatchedOpcodeBYTE(&pRetAddr[-4]) == X86_INSTR_NOP)
             && (GetUnpatchedOpcodeBYTE(&pRetAddr[-3]) == X86_INSTR_NOP)
             && (GetUnpatchedOpcodeWORD(&pRetAddr[-2]) == X86_INSTR_CALL_IND_EAX);
    _ASSERTE(!fRet || !isCallRelative(pRetAddr));
    _ASSERTE(!fRet || !isCallRelativeIndirect(pRetAddr));
    return fRet;
}

//------------------------------------------------------------------------
inline void emitJump(LPBYTE pBuffer, LPVOID target)
{
    LEAF_CONTRACT;

    pBuffer[0] = X86_INSTR_JMP_REL32; //JUMPNEAR32
    *((LPVOID*)(1+pBuffer)) = (LPVOID) (((LPBYTE)target) - (pBuffer+5));
}

//------------------------------------------------------------------------
//  Given the same pBuffer that was used by emitJump this method
//  decodes the instructions and returns the jump target
inline BYTE* decodeJump(const BYTE* pBuffer)
{
    LEAF_CONTRACT;
    CONSISTENCY_CHECK(pBuffer[0] == X86_INSTR_JMP_REL32);
    return (BYTE*)rel32Decode((TADDR)&(pBuffer[1]));
}

//
// On IA64 back to back jumps should be separated by a nop bundle to get
// the best performance from the hardware's branch prediction logic.
// For all other platforms back to back jumps don't require anything special
// That is why we have these two wrapper functions that call emitJump and decodeJump
//

//------------------------------------------------------------------------
inline void emitBackToBackJump(LPBYTE pBuffer, LPVOID target)
{
    WRAPPER_CONTRACT;
    emitJump(pBuffer, target);
}

//------------------------------------------------------------------------
inline BYTE* decodeBackToBackJump(BYTE* pBuffer)
{
    WRAPPER_CONTRACT;
    return decodeJump(pBuffer);
}

inline void emitIndJump(LPBYTE pBuffer, LPVOID targetptr)
{
    LEAF_CONTRACT;

    C_ASSERT(X86_INSTR_JMP_IND >> 8);
    pBuffer[0] = (X86_INSTR_JMP_IND & 0xff);
    pBuffer[1] = (X86_INSTR_JMP_IND >> 8);
    *((LPVOID*)(2+pBuffer)) = targetptr;
}

EXTERN_C void __stdcall setFPReturn(int fpSize, INT64 retVal);
EXTERN_C void __stdcall getFPReturn(int fpSize, INT64 *pretval);

// Adjust the generic interlocked operations for any platform specific ones we
// might have.
void InitFastInterlockOps();


// SEH info forward declarations

struct ComToManagedExRecord; // defined in cgenx86.cpp

inline BOOL IsUnmanagedValueTypeReturnedByRef(UINT sizeofvaluetype) 
{
    LEAF_CONTRACT;

    // odd-sized small structures are not 
    //  enregistered e.g. struct { char a,b,c; }
    return (sizeofvaluetype > 8) ||
        (sizeofvaluetype & (sizeofvaluetype-1)); // check that the size is power of two
}

inline BOOL IsManagedValueTypeReturnedByRef(UINT sizeofvaluetype) 
{
    LEAF_CONTRACT;

    return TRUE;
}

#include <pshpack1.h>
DECLSPEC_ALIGN(4) struct UMEntryThunkCode
{
    BYTE            m_alignpad[2];  // used to guarantee alignment of backpactched portion
    BYTE            m_movEAX;   //MOV EAX,imm32
    LPVOID          m_uet;      // pointer to start of this structure
    BYTE            m_jmp;      //JMP NEAR32
    const BYTE *    m_execstub; // pointer to destination code  // make sure the backpatched portion is dword aligned.

    void Encode(BYTE* pTargetCode, void* pvSecretParam);

    LPCBYTE GetEntryPoint() const
    {
        LEAF_CONTRACT;

        return (LPCBYTE)&m_movEAX;
    }

    static int GetEntryPointOffset()
    {
        LEAF_CONTRACT;

        return 2;
    }

    // Called by IJWNOADThunkStubManager::CheckIsStub to decide
    // if an address is a IJWNOADThunk stub.
    // Return true if it is, else false. 
    static bool IsStub(const BYTE* pAddress, BYTE* pTargetCode, void* pvSecretParam);
    
};
#include <poppack.h>

// ClrFlushInstructionCache is used when we want to call FlushInstructionCache
// for a specific architecture in the common code, but not for other architectures.
// On IA64 ClrFlushInstructionCache calls the Kernel FlushInstructionCache function
// to flush the instruction cache. 
// We call ClrFlushInstructionCache whenever we create or modify code in the heap. 
// Currently ClrFlushInstructionCache has no effect on X86
//

inline BOOL ClrFlushInstructionCache(LPCVOID pCodeAddr, size_t sizeOfCode)
{
    // FlushInstructionCache(GetCurrentProcess(), pCodeAddr, sizeOfCode);
    return TRUE;
}


//
// JIT HELPER ALIASING FOR PORTABILITY.
//
// Create alias for optimized implementations of helpers provided on this platform
//

#define JIT_MonEnterWorker  JIT_MonEnterWorker
#define JIT_MonTryEnter     JIT_MonTryEnter
#define JIT_MonExitWorker   JIT_MonExitWorker
#define JIT_MonEnterStatic  JIT_MonEnterStatic
#define JIT_MonExitStatic   JIT_MonExitStatic

// optimized static helpers generated dynamically at runtime
// #define JIT_GetSharedGCStaticBase
// #define JIT_GetSharedNonGCStaticBase
// #define JIT_GetSharedGCStaticBaseNoCtor
// #define JIT_GetSharedNonGCStaticBaseNoCtor



#endif // __cgenx86_h__
