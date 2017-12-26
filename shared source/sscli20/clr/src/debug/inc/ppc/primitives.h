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
// File: primitives.h
//
// Platform-specific debugger primitives
//
//*****************************************************************************

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#define CORDB_ADDRESS_TYPE const DWORD

//This is an abstraction to keep x86/ia64 patch data separate
#define PRD_TYPE                 DWORD_PTR

#define MAX_INSTRUCTION_LENGTH   sizeof(DWORD)

#define CORDbg_BREAK_INSTRUCTION_SIZE sizeof(DWORD)
#define CORDbg_BREAK_INSTRUCTION (DWORD)0x7ef00008

inline CORDB_ADDRESS GetPatchEndAddr(CORDB_ADDRESS patchAddr)
{
    return patchAddr + CORDbg_BREAK_INSTRUCTION_SIZE;
}


#define InitializePRDToBreakInst(_pPRD)       *(_pPRD) = CORDbg_BREAK_INSTRUCTION
#define PRDIsBreakInst(_pPRD)                 (*(_pPRD) == CORDbg_BREAK_INSTRUCTION)

#define CORDbgGetInstructionEx(_buffer, _requestedAddr, _patchAddr, _dummy1, _dummy2)                          \
    CORDbgGetInstruction((CORDB_ADDRESS_TYPE *)((_buffer) + ((_patchAddr) - (_requestedAddr))));
                    
#define CORDbgSetInstructionEx(_buffer, _requestedAddr, _patchAddr, _opcode, _dummy2)                          \
    CORDbgSetInstruction((CORDB_ADDRESS_TYPE *)((_buffer) + ((_patchAddr) - (_requestedAddr))), (_opcode));

#define CORDbgInsertBreakpointEx(_buffer, _requestedAddr, _patchAddr, _dummy1, _dummy2)                        \
    CORDbgInsertBreakpoint((CORDB_ADDRESS_TYPE *)((_buffer) + ((_patchAddr) - (_requestedAddr))));


static CorDebugRegister g_JITToCorDbgReg[] =
{
    REGISTER_STACK_POINTER,
    REGISTER_FRAME_POINTER
};

//
// Mapping from ICorDebugInfo register numbers to CorDebugRegister
// numbers. Note: this must match the order in corinfo.h.
//
inline CorDebugRegister ConvertRegNumToCorDebugRegister(ICorJitInfo::RegNum reg)
{
    return g_JITToCorDbgReg[reg];
}


//
// inline function to access/modify the CONTEXT
//
inline LPVOID CORDbgGetIP(CONTEXT *context) {
    LEAF_CONTRACT;

    return (LPVOID)(size_t)context->Iar;
}

inline void CORDbgSetIP(CONTEXT *context, LPVOID eip) {
    LEAF_CONTRACT;

    context->Iar = (ULONG)(size_t)eip;
}

inline LPVOID CORDbgGetSP(CONTEXT *context) {
    LEAF_CONTRACT;

    return (LPVOID)(size_t)context->Gpr1;
}

inline void CORDbgSetSP(CONTEXT *context, LPVOID esp) {
    LEAF_CONTRACT;

    context->Gpr1 = (ULONG)(size_t)esp;
}

inline LPVOID CORDbgGetFP(PCONTEXT context) {
    LEAF_CONTRACT;

    return (LPVOID)(UINT_PTR)context->Gpr30;
}

inline void CORDbgSetFP(PCONTEXT context, LPVOID ebp) {
    LEAF_CONTRACT;

    context->Gpr30 = (ULONG)(size_t)ebp;
}

/* ========================================================================= */
//
// Routines used by debugger support functions such as codepatch.cpp or
// exception handling code.
//
// GetInstruction, InsertBreakpoint, and SetInstruction all operate on
// a _single_ byte of memory on _x86_. This is really important. If you only
// save one byte from the instruction stream before placing a breakpoint,
// you need to make sure to only replace one byte later on.
//

inline PRD_TYPE CORDbgGetInstruction(UNALIGNED CORDB_ADDRESS_TYPE* address)
{
    LEAF_CONTRACT;

    return *(PRD_TYPE*)address; // retrieving only one byte is important
  
}

inline void CORDbgInsertBreakpoint(CORDB_ADDRESS_TYPE* address)
{
    WRAPPER_CONTRACT;

    *((PRD_TYPE*)address) = CORDbg_BREAK_INSTRUCTION;

    FlushInstructionCache(GetCurrentProcess(),
                          address,
                          sizeof(PRD_TYPE));

}

inline void CORDbgSetInstruction(CORDB_ADDRESS_TYPE* address,
                                 PRD_TYPE instruction)
{
    WRAPPER_CONTRACT;

    *((PRD_TYPE*)address) = instruction;

    FlushInstructionCache(GetCurrentProcess(),
                          address,
                          sizeof(PRD_TYPE));

}

inline void CORDbgAdjustPCForBreakInstruction(CONTEXT* pContext)
{
    // The instruction is already stopped at the correct place
    return;
}

inline bool AddressIsBreakpoint(CORDB_ADDRESS_TYPE* address)
{
    LEAF_CONTRACT;

    return *address == CORDbg_BREAK_INSTRUCTION;
}

inline void SetSSFlag(CONTEXT *context) 
{
    _ASSERTE(context != NULL);
    context->Msr |= 0x400UL;
}

inline void UnsetSSFlag(CONTEXT *context) 
{
    _ASSERTE(context != NULL);
    context->Msr &= ~0x400UL;
}
inline bool IsSSFlagEnabled(CONTEXT * context)
{
    _ASSERTE(context != NULL);
    return (context->Msr & 0x400UL) != 0;
}


inline bool PRDIsEqual(PRD_TYPE p1, PRD_TYPE p2)
{
    LEAF_CONTRACT;

    return p1 == p2;
}

inline void InitializePRD(PRD_TYPE *p1)
{
    LEAF_CONTRACT;

    *p1 = 0;
}

inline bool PRDIsEmpty(PRD_TYPE p1) {
    LEAF_CONTRACT;

    return p1 == 0;
}

#endif // PRIMITIVES_H_
