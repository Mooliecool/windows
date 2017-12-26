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
#ifndef __REGDISP_H
#define __REGDISP_H




#if defined(_X86_)

typedef struct _REGDISPLAY {
    PCONTEXT pContext;

    PCONTEXT pContextForUnwind; // scratch context for unwinding
                                // used to preserve context saved in the frame that 
                                // could be otherwise wiped by the unwinding

    DWORD * pEdi;
    DWORD * pEsi;
    DWORD * pEbx;
    DWORD * pEdx;
    DWORD * pEcx;
    DWORD * pEax;

    DWORD * pEbp;
    DWORD   Esp;                // (Esp) Stack Pointer
    SLOT *  pPC;                // processor neutral name
    TADDR   PCTAddr;

} REGDISPLAY;

inline LPVOID GetRegdisplaySP(REGDISPLAY *display) {
    LEAF_CONTRACT;

    return (LPVOID)(size_t)display->Esp;
}

inline void SetRegdisplaySP(REGDISPLAY *display, LPVOID sp ) {
    LEAF_CONTRACT;

    (display->Esp) = (DWORD)(size_t)sp;
}

inline LPVOID GetRegdisplayFP(REGDISPLAY *display) {
    LEAF_CONTRACT;

    return (LPVOID)(size_t)*(display->pEbp);
}

inline LPVOID GetRegdisplayFPAddress(REGDISPLAY *display) {
    LEAF_CONTRACT;
    
    return (LPVOID)display->pEbp;
}

// XXX drewb - Eventually all users of GetControlPC should
// be converted to using TADDR and this duplication can
// go away.
inline TADDR taGetControlPC(REGDISPLAY* display) {
    return (TADDR)(*(display->pPC));
}
inline LPVOID GetControlPC(REGDISPLAY *display) {
    LEAF_CONTRACT;

    return (LPVOID)(*(display->pPC));
}
inline void SetControlPC(REGDISPLAY* display, LPVOID ip)
{
    LEAF_CONTRACT;

#ifndef DACCESS_COMPILE
    *(display->pPC) = (SLOT)ip;
#else
    DacError(E_UNEXPECTED);
#endif
}
// This function tells us if the given stack pointer is in one of the frames of the functions called by the given frame
inline BOOL IsInCalleesFrames(REGDISPLAY *display, LPVOID stackPointer) {
    LEAF_CONTRACT;

    return (TADDR)stackPointer < display->PCTAddr;
}
inline TADDR GetRegdisplayStackMark(REGDISPLAY *display) {
    LEAF_CONTRACT;

    return display->PCTAddr;
}

#elif defined(_PPC_)

#ifndef NUM_CALLEESAVED_REGISTERS
#define NUM_CALLEESAVED_REGISTERS 19
#endif
#ifndef NUM_FLOAT_CALLEESAVED_REGISTERS
#define NUM_FLOAT_CALLEESAVED_REGISTERS 18
#endif

typedef struct _REGDISPLAY {
    PCONTEXT pContext;

    PCONTEXT pContextForUnwind; // scratch context for unwinding
                                // used to preserve context saved in the frame that 
                                // could be otherwise wiped by the unwinding

    DWORD   * pR[NUM_CALLEESAVED_REGISTERS];        // r13 .. r31
    DOUBLE  * pF[NUM_FLOAT_CALLEESAVED_REGISTERS];  // fpr14 .. fpr31

    DWORD     CR;                                   // cr

    DWORD     SP;
    SLOT    * pPC;              // processor neutral name
    TADDR   PCTAddr;

} REGDISPLAY;

inline LPVOID GetRegdisplaySP(REGDISPLAY *display) {
    LEAF_CONTRACT;
    return (LPVOID)(size_t)display->SP;
}

inline void SetRegdisplaySP(REGDISPLAY *display, LPVOID sp ) {
    LEAF_CONTRACT;
    display->SP = (DWORD)(size_t)sp;
}

inline LPVOID GetRegdisplayFP(REGDISPLAY *display) {
    LEAF_CONTRACT;
    return (LPVOID)(size_t)*(display->pR[30 - 13]);
}

inline LPVOID GetRegdisplayFPAddress(REGDISPLAY *display) {
    LEAF_CONTRACT;
    return (LPVOID)display->pR[30 - 13];
}

// XXX drewb - Eventually all users of GetControlPC should
// be converted to using TADDR and this duplication can
// go away.
inline TADDR taGetControlPC(REGDISPLAY* display) {
    return (TADDR)(*(display->pPC));
}
inline LPVOID GetControlPC(REGDISPLAY *display) {
    LEAF_CONTRACT;
    return (LPVOID)(*(display->pPC));
}
inline void SetControlPC(REGDISPLAY* display, LPVOID ip)
{
    LEAF_CONTRACT;
    *(display->pPC) = (SLOT)ip;
}

// This function tells us if the given stack pointer is in one of the frames of the functions called by the given frame
inline BOOL IsInCalleesFrames(REGDISPLAY *display, LPVOID stackPointer) {
    LEAF_CONTRACT;
    return stackPointer < ((LPVOID)display->pPC);
}

inline LPVOID GetRegdisplayStackMark(REGDISPLAY *display) {
    LEAF_CONTRACT;
    return (LPVOID)(size_t)display->SP;
}

#else // none of the above processors

PORTABILITY_WARNING("RegDisplay functions are not implemented on this platform.")

typedef struct _REGDISPLAY {
    PCONTEXT pContext;          // points to current Context
    size_t   SP;
    size_t * FramePtr;
    SLOT   * pPC;
} REGDISPLAY;

// XXX drewb - Eventually all users of GetControlPC should
// be converted to using TADDR and this duplication can
// go away.
inline TADDR taGetControlPC(REGDISPLAY* display) {
    return (TADDR) NULL;
}

inline LPVOID GetControlPC(REGDISPLAY *display) {
    LEAF_CONTRACT;
    return (LPVOID) NULL;
}

inline LPVOID GetRegdisplaySP(REGDISPLAY *display) {
    LEAF_CONTRACT;
    return (LPVOID)display->SP;
}

inline void SetRegdisplaySP(REGDISPLAY *display, LPVOID sp ) {
    LEAF_CONTRACT;
    display->SP = (DWORD)(size_t)sp;
}

inline LPVOID GetRegdisplayFP(REGDISPLAY *display) {
    LEAF_CONTRACT;
    return (LPVOID)(size_t)*(display->FramePtr);
}

inline BOOL IsInCalleesFrames(REGDISPLAY *display, LPVOID stackPointer) {
    LEAF_CONTRACT;
    return FALSE;
}
inline LPVOID GetRegdisplayStackMark(REGDISPLAY *display) {
    LEAF_CONTRACT;
    return (LPVOID)display->SP;
}

#endif

typedef REGDISPLAY *PREGDISPLAY;


inline void FillRegDisplay(const PREGDISPLAY pRD, PCONTEXT pctx, PCONTEXT pCallerCtx = NULL)
{
    WRAPPER_CONTRACT;

#ifdef _X86_
    pRD->pContext = pctx;
    pRD->pContextForUnwind = NULL;
    pRD->pEdi = &(pctx->Edi);
    pRD->pEsi = &(pctx->Esi);
    pRD->pEbx = &(pctx->Ebx);
    pRD->pEbp = &(pctx->Ebp);
    pRD->pEax = &(pctx->Eax);
    pRD->pEcx = &(pctx->Ecx);
    pRD->pEdx = &(pctx->Edx);
    pRD->Esp  = pctx->Esp;
    pRD->pPC  = (SLOT*)&(pctx->Eip);
    pRD->PCTAddr = (TADDR)&(pctx->Eip);
#elif defined(_PPC_)

    int i;

    pRD->pContext = pctx;
    pRD->CR  = pctx->Cr;

    for (i = 0; i < NUM_CALLEESAVED_REGISTERS; i++) {
        pRD->pR[i] = (DWORD*)&(pctx->Gpr13) + i;
    }

    for (i = 0; i < NUM_FLOAT_CALLEESAVED_REGISTERS; i++) {
        pRD->pF[i] = (DOUBLE*)&(pctx->Fpr14) + i;
    }

    pRD->SP  = pctx->Gpr1;
    pRD->pPC  = (SLOT*)&(pctx->Iar);

#else
    PORTABILITY_ASSERT("@NYI Platform - InitRegDisplay (Threads.cpp)");
#endif
}

// Initialize a new REGDISPLAY/CONTEXT pair from an existing valid REGDISPLAY.
inline void CopyRegDisplay(const PREGDISPLAY pInRD, PREGDISPLAY pOutRD, CONTEXT *pOutCtx)
{
    WRAPPER_CONTRACT;

    // The general strategy is to extract the register state from the input REGDISPLAY 
    // into the new CONTEXT then simply call FillRegDisplay.

    CONTEXT* pOutCallerCtx = NULL;

#ifdef _X86_
    pOutCtx->Edi = *pInRD->pEdi;
    pOutCtx->Esi = *pInRD->pEsi;
    pOutCtx->Ebx = *pInRD->pEbx;
    pOutCtx->Ebp = *pInRD->pEbp;
    pOutCtx->Eax = *pInRD->pEax;
    pOutCtx->Ecx = *pInRD->pEcx;
    pOutCtx->Edx = *pInRD->pEdx;
    pOutCtx->Esp = pInRD->Esp;
    pOutCtx->Eip = *(DWORD*)pInRD->pPC;
#elif defined(_PPC_)

    int i;

    pOutCtx->Cr = pInRD->CR;

    for (i = 0; i < NUM_CALLEESAVED_REGISTERS; i++)
        *(&pOutCtx->Gpr13 + i) = *(ULONG*)pInRD->pR[i];

    for (i = 0; i < NUM_FLOAT_CALLEESAVED_REGISTERS; i++)
        *(&pOutCtx->Fpr14 + i) = *(double*)pInRD->pF[i];

    pOutCtx->Gpr1 = pInRD->SP;
    pOutCtx->Iar = *(ULONG*)(pInRD->pPC);
#else
    PORTABILITY_ASSERT("@NYI Platform - CopyRegDisplay (Threads.cpp)");
#endif

    if (pOutRD)
        FillRegDisplay(pOutRD, pOutCtx, pOutCallerCtx);
}

// Get address of a register in a CONTEXT given the reg number. For X86, 
// the reg number is the R/M number from ModR/M byte or base in SIB byte
inline size_t * getRegAddr (unsigned regNum, PTR_CONTEXT regs)
{
#ifdef _X86_
    switch (regNum)
    {
    case 0:
        return (size_t *)&regs->Eax;
        break;
    case 1:
        return (size_t *)&regs->Ecx;
        break;
    case 2:
        return (size_t *)&regs->Edx;
        break;
    case 3:
        return (size_t *)&regs->Ebx;
        break;
    case 4:
        return (size_t *)&regs->Esp;
        break;
    case 5:
        return (size_t *)&regs->Ebp;
        break;
    case 6:
        return (size_t *)&regs->Esi;
        break;
    case 7:
        return (size_t *)&regs->Edi;
        break;
    default:
        _ASSERTE (!"unknown regNum");
    }
#else
    _ASSERTE(!"@TODO Port - getRegAddr (Regdisp.h)");
#endif
    return(0);
}

#endif  // __REGDISP_H


