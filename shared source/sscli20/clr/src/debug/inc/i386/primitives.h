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


#define CORDB_ADDRESS_TYPE                     const BYTE

//This is an abstraction to keep x86/ia64 patch data separate
#define PRD_TYPE                               DWORD_PTR

#define MAX_INSTRUCTION_LENGTH 4+2+1+1+4+4

#define CORDbg_BREAK_INSTRUCTION_SIZE 1
#define CORDbg_BREAK_INSTRUCTION (BYTE)0xCC

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
    REGISTER_X86_EAX,
    REGISTER_X86_ECX,
    REGISTER_X86_EDX,
    REGISTER_X86_EBX,
    REGISTER_X86_ESP,
    REGISTER_X86_EBP,
    REGISTER_X86_ESI,
    REGISTER_X86_EDI
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

    return (LPVOID)(size_t)(context->Eip);
}

inline void CORDbgSetIP(CONTEXT *context, LPVOID eip) {
    LEAF_CONTRACT;

    context->Eip = (UINT32)(size_t)eip;
}

inline LPVOID CORDbgGetSP(CONTEXT *context) {
    LEAF_CONTRACT;

    return (LPVOID)(size_t)(context->Esp);
}

inline void CORDbgSetSP(CONTEXT *context, LPVOID esp) {
    LEAF_CONTRACT;

    context->Esp = (UINT32)(size_t)esp;
}

inline void CORDbgSetFP(CONTEXT *context, LPVOID ebp) {
    LEAF_CONTRACT;

    context->Ebp = (UINT32)(size_t)ebp;
}
inline LPVOID CORDbgGetFP(CONTEXT* context)
{
    LEAF_CONTRACT;

    return (LPVOID)(UINT_PTR)context->Ebp;
}

/* ========================================================================= */
//
// Routines used by debugger support functions such as codepatch.cpp or
// exception handling code.
//
// GetInstruction, InsertBreakpoint, and SetInstruction all operate on
// a _single_ byte of memory. This is really important. If you only
// save one byte from the instruction stream before placing a breakpoint,
// you need to make sure to only replace one byte later on.
//


inline PRD_TYPE CORDbgGetInstruction(UNALIGNED CORDB_ADDRESS_TYPE* address)
{
    LEAF_CONTRACT;

    return *address; // retrieving only one byte is important
  
}

inline void CORDbgInsertBreakpoint(UNALIGNED CORDB_ADDRESS_TYPE *address)
{
    LEAF_CONTRACT;

    *((unsigned char*)address) = 0xCC; // int 3 (single byte patch)
}

inline void CORDbgSetInstruction(CORDB_ADDRESS_TYPE* address,
                                 DWORD instruction)
{
    LEAF_CONTRACT;

    *((unsigned char*)address)
          = (unsigned char) instruction; // setting one byte is important
}

// After a breakpoint exception, the CPU points to _after_ the break instruction.
// Adjust the IP so that it points at the break instruction. This lets us patch that
// opcode and re-excute what was underneath the bp.
inline void CORDbgAdjustPCForBreakInstruction(CONTEXT* pContext)
{
    LEAF_CONTRACT;

    pContext->Eip -= 1;
}

inline bool AddressIsBreakpoint(CORDB_ADDRESS_TYPE* address)
{
    LEAF_CONTRACT;

    return *address == CORDbg_BREAK_INSTRUCTION;
}

inline BOOL IsRunningOnWin95()
{
    return RunningOnWin95();

}

// Set the hardware trace flag.
inline void SetSSFlag(CONTEXT *context) 
{
    _ASSERTE(context != NULL);
    context->EFlags |= 0x100;
}

// Unset the hardware trace flag.
inline void UnsetSSFlag(CONTEXT *context) 
{
    _ASSERTE(context != NULL);
    context->EFlags &= ~0x100;
}

// return true if the hardware trace flag applied.
inline bool IsSSFlagEnabled(CONTEXT * context)
{
    _ASSERTE(context != NULL);
    return (context->EFlags & 0x100) != 0;
}



inline CORDB_ADDRESS_TYPE GetPrevInst(CORDB_ADDRESS_TYPE address) {
//TODO: Write These
    return address;
}
inline bool PRDIsEqual(PRD_TYPE p1, PRD_TYPE p2){
    return p1 == p2;
}

// On x86 opcode 0 is an 8-bit version of ADD.  Do we really want to use 0 to mean empty? (see bug 366221).
inline void InitializePRD(PRD_TYPE *p1) {
    *p1 = 0;
}
inline bool PRDIsEmpty(PRD_TYPE p1) {
    LEAF_CONTRACT;

    return p1 == 0;
}


#endif // PRIMITIVES_H_
