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


#include "eecallconv.h"
#include "asmconstants.h"

// mnemonics for the PowerPC registers
#define sp      r1

#define RED_ZONE 224
#define LINKAGE_AREA 24
// offset 0 in the linkage area is the caller's saved SP
// offset 4 is the saved CR
// offset 8 as the saved LR
// offsets 12, 16 and 20 are reserved.

TOC_DATA_SECTION
EXTERN_GLOBAL(iSpinLock)
EXTERN_GLOBAL(g_dwTPStubAddr)
EXTERN_GLOBAL(g_dwOOContextAddr)
EXTERN_GLOBAL(GetThread)

DATA_SECTION
EXTERN_FUNC(LinkFrameAndThrow)
#ifdef _DEBUG
EXTERN_FUNC(PerformExitFrameChecks)
EXTERN_FUNC(HelperMethodFrameConfirmState)
#endif
EXTERN_FUNC(COMPlusEndCatch)
EXTERN_FUNC(COMPlusCheckForAbort)
EXTERN_FUNC(NDirectGenericStubComputeFrameSize)
EXTERN_FUNC(NDirectGenericStubBuildArguments)
EXTERN_FUNC(NDirectGenericStubPostCall)

TEXT_SECTION

// void [__cdecl|__stdcall] ResetCurrentContext(void);
//
// Restores the floating-point hardware to a known state, leaving
// precision and rounding unmodified.
//
ASMFUNC(ResetCurrentContext)
    stfd  f0, -8(sp)   // preserve the current value in f0
    mffs  f0            // move fpscr to the upper 32 bits of f0
    stfd  f0, -16(sp)   // store f0 to the stack
    lwz   r3, -12(sp)   // load one word from it... the fpscr value
    addis r4, 0, 0xc010 // clear all bits except 30/31 (RN, rounding control)
    and   r3, r3, r4    //  and 20 (reserved)
    stw   r3, -12(sp)   // store the new fpscr value to memory
    lfd   f0,-16(sp)    // load f0 with the update fpscr value
    mtfsf 7, f0         // move it to fpscr
    lfd   f0, -8(sp)    // restore f0
    blr


/*
; This is a helper that we use to raise the correct managed exception with
; the necessary frame (after taking a fault in jitted code).
;
; Inputs:
;      all registers still have the value
;      they had at the time of the fault, except
;              Iar points to this function
;              r0 contains the original EIP
;
; What it does:
;      The exception to be thrown is stored in m_pLastThrownObjectHandle.
;      We push a FaultingExcepitonFrame on the stack, and then we call
;      complus throw.
;
*/
ASMFUNC(NakedThrowHelper)

        // skip the reserved red zone & create caller linkage area
        stwu sp, -roundup(LINKAGE_AREA+RED_ZONE, 16)(sp)

        // save the return address
        stw r0, 8(sp)

        // allocate the rest of FaultingExceptionFrame and callee linkage area
    #if (LINKAGE_AREA+8 + FaultingExceptionFrame_m_SavedSP) % 16 != 0
    #error Stack frame not alligned
    #endif
        stwu sp, -(LINKAGE_AREA+8 + FaultingExceptionFrame_m_SavedSP)(sp)

        // save cr
        mfcr r0
        stw r0,(LINKAGE_AREA+8 + FaultingExceptionFrame_m_regs+CalleeSavedRegisters_cr)(sp)

        // save general regs
        stmw r13,(LINKAGE_AREA+8 + FaultingExceptionFrame_m_regs+CalleeSavedRegisters_r)(sp)

        // save float regs
        CALLEESAVED_REGISTERS_FP(stfd,(LINKAGE_AREA+8 + FaultingExceptionFrame_m_regs+CalleeSavedRegisters_f),sp)

        // r3 = pFrame
        la r3, (LINKAGE_AREA+8)(sp)

        bl EXTERN_LABEL(LinkFrameAndThrow)

        // this should never return, but...
        lwz sp, 0(sp)
        lwz r0, 8(sp)
        lwz sp, 0(sp) // skip the reserved red zone
        mtlr r0
        blr


// void ResumeAtJitEHHelper(CONTEXT *pContext)
//
// Restores the CPU registers from the CONTEXT record, including
// EIP.
//
ASMFUNC(ResumeAtJitEHHelper)

        // load cr
        lwz r0,(EHContext_CR)(r3)
        mtcr r0

        // load float regs
        CALLEESAVED_REGISTERS_FP(lfd,EHContext_F,r3)

        // load general regs
        lmw r4,(EHContext_R+4*4)(r3)
        lwz r0,(EHContext_R+3*4)(r3) // save r3 in r0, so it can be used in the meantime
        lwz r3,(EHContext_R+4)(r3)

        // allocate linkage area
        addi r3,r3,-LINKAGE_AREA

        // align the stack
        clrrwi r3,r3,4

        // try to fill the linkage area with a somehow meaningful content
        stw r30,0(r3)

        // set the new sp
        mr sp,r3
        mr r3,r0

        // make the jump
        mtctr r12
        bctr

// int __stdcall CallJitEHFilterHelper(size_t *pShadowSP, EHContext *pContext);
//   Restores only r3..r31 from the EHContext
//
ASMFUNC(CallJitEHFilterHelper)

        // prolog
        mflr r0
        stw r0, 8(sp)
        stwu sp, -roundup(LINKAGE_AREA + 4*NUM_CALLEESAVED_REGISTERS, 16)(sp)

        // Write sp to the shadowSP slot
        cmpwi r3, 0
        beq LOCAL_LABEL(DONE_SHADOWSP_FILTER)
        ori r5, sp, SHADOW_SP_IN_FILTER_ASM
        stw r5, 0(r3)
    LOCAL_LABEL(DONE_SHADOWSP_FILTER):

        // store callee saved regs
        stmw r13, (LINKAGE_AREA)(sp)

        // load general regs
        mr r3, r4
        lmw r4,(EHContext_R+4*4)(r3)
        lwz r3,(EHContext_R+3*4)(r3)

        // make the call
        mtctr r12
        bctrl

        // restore callee saved regs
        lmw r13, (LINKAGE_AREA)(sp)

        // epilog
        lwz sp, 0(sp)
        lwz r0, 8(sp)
        mtlr r0
        blr


// void __stdcall CallJitEHFinallyHelper(size_t *pShadowSP, EHContext *pContext);
//   Restores only r3..r31 from the EHContext
ASMFUNC(CallJitEHFinallyHelper)

        // prolog
        mflr r0
        stw r0, 8(sp)
        stwu sp, -roundup(LINKAGE_AREA + 4*NUM_CALLEESAVED_REGISTERS, 16)(sp)

        // Write sp to the shadowSP slot
        cmpwi r3, 0
        beq LOCAL_LABEL(DONE_SHADOWSP_FINALLY)
        stw sp, 0(r3)
    LOCAL_LABEL(DONE_SHADOWSP_FINALLY):

        // store callee saved regs
        stmw    r13, (LINKAGE_AREA)(sp)

        // load general regs
        mr r3, r4
        lmw r4,(EHContext_R+4*4)(r3)
        lwz r3,(EHContext_R+3*4)(r3)

        // make the call
        mtctr r12
        bctrl

        // restore callee saved regs
        lmw     r13, (LINKAGE_AREA)(sp)

        // epilog
        lwz sp, 0(sp)
        lwz r0, 8(sp)
        mtlr r0
        blr



// Atomic bit manipulations.  On x86 there are UP and MP versions,
// but for PowerPC, one version, generic (GN) handles both cases.


// Enter a spinlock.
//  First argument is the spinlock variable's name
// On return, r10 must be preserved until the SPINLOCK_EXIT is complete,
// trashes r9 and r0
#define SPINLOCK_ENTER(name, label) \
    mflr r0     /* save the return address in r0 */  \n \
    LOAD_EXTERNAL_VAR(name, r10) \n \
    mtlr r0     /* restore the return address back into the lr */ \n \
    /* r10 now holds the address of the spinlock variable */ \n \
    li r9, 1 \n \
LOCAL_LABEL(label##1):    \n \
    sync \n \
    lwarx   r0, 0, r10  /* load the current spinlock value */ \n \
    cmpwi   r0, 0      /* check if it is zero */ \n \
    bne-    LOCAL_LABEL(label##1) /* retry if the value isn't zero - the lock is held */ \n \
    stwcx.  r9, 0, r10  /* store 1 to lock it */ \n \
    bne-    LOCAL_LABEL(label##1) /* store failed - someone else beat us to it*/ \n \
    sync 

// Leave a spinlock.  It assumes r10 still points to the spinlock variable,
// trashes r9 and r0
#define SPINLOCK_EXIT(label) \
    li r9, 0           \n \
LOCAL_LABEL(label##2): \n \
    lwarx   r0, 0, r10  \n \
    stwcx.  r9, 0, r10  \n \
    bne-    LOCAL_LABEL(label##2) \n \
    sync

// void __stdcall OrMaskGN(DWORD volatile *p, const int msk)
ASMFUNC(OrMaskGN)
        sync
        lwarx   r5, 0, r3
        or      r5, r5, r4
        stwcx.  r5, 0, r3
        bne-    GLOBAL_LABEL(OrMaskGN)
        sync
        blr

// void __stdcall AndMaskGN(DWORD volatile *p, const int msk)
ASMFUNC(AndMaskGN)
        sync
        lwarx   r5, 0, r3
        and     r5, r5, r4
        stwcx.  r5, 0, r3
        bne-    GLOBAL_LABEL(AndMaskGN)
        sync
        blr

// LONG __stdcall ExchangeGN(LONG volatile *Target, LONG Value)
// Returns the previous value of *Target
ASMFUNC(ExchangeGN)
        sync
        lwarx   r5, 0, r3
        stwcx.  r4, 0, r3
        bne-    GLOBAL_LABEL(ExchangeGN)
        sync
        mr      r3, r5
        blr

// INT64  __stdcall ExchangeLongGN(INT64 volatile *Target, INT64 Value)
// Returns the previous value of *Target
ASMFUNC(ExchangeLongGN)
        mr      r6, r3      // result will be in r4:r3
        mr      r7, r4
        // sync implied by SPINLOCK_ENTER
        SPINLOCK_ENTER(iSpinLock, ExchangeLongGN)
        lwz     r4, 4(r6)   // load the low word
        lwz     r3, 0(r6)   // load the high word
        stw     r5, 4(r6)   // store the new low word
        stw     r7, 0(r6)   // store the new high word
        SPINLOCK_EXIT(ExchangeLongGN)
        // sync implied by SPINLOCK_EXIT
        blr

// LONG __stdcall ExchangeAddGN(LONG volatile *Target, LONG value)
// Returns the previous value of *Target
ASMFUNC(ExchangeAddGN)
        sync
        lwarx   r5, 0, r3
        add     r6, r5, r4
        stwcx.  r6, 0, r3
        bne-    GLOBAL_LABEL(ExchangeAddGN)
        sync
        mr      r3, r5
        blr

// INT64 __stdcall ExchangeAddLongGN(INT64 volatile *Target, INT64 value)
// Returns the previous value of *Target
ASMFUNC(ExchangeAddLongGN)
        // sync implied by SPINLOCK_ENTER
        SPINLOCK_ENTER(iSpinLock, ExchangeAddLongGN)
        lwz     r7, 4(r3)   // load the low word
        lwz     r6, 0(r3)   // load the high word
        addc    r7, r7, r5  // increment, and record carry
        adde    r6, r6, r4  // increment, with carry
        stw     r7, 4(r3)   // store the new low word
        stw     r6, 0(r3)   // store the new high word
        SPINLOCK_EXIT(ExchangeAddLongGN)
        // sync implied by SPINLOCK_EXIT
        mr      r4, r7
        mr      r3, r6
        blr

// void *__stdcall CompareExchangeGN(PVOID volatile *Destination,
//                                  PVOID Exchange, PVOID Comparand)
ASMFUNC(CompareExchangeGN)
        sync
        lwarx   r6, 0, r3
        cmpw    r6, r5
        bne-    LOCAL_LABEL(CompareExchangeGNMismatch)
        stwcx.  r4, 0, r3
        bne-    GLOBAL_LABEL(CompareExchangeGN)
    LOCAL_LABEL(CompareExchangeGNMismatch):
        sync
        mr      r3, r6
        blr

// INT64 __stdcall CompareExchangeLongGN(PVOID volatile *Destination,
//                                  INT64 Exchange, INT64 Comparand)
ASMFUNC(CompareExchangeLongGN)
        // sync implied by SPINLOCK_ENTER
        SPINLOCK_ENTER(iSpinLock, CompareExchangeLongGN)
        lwz     r9, 4(r3)   // load the low word
        lwz     r8, 0(r3)   // load the high word
        cmpw    r9, r7      // compare low word
        bne-    LOCAL_LABEL(CompareExchangeLongGNMismatch)
        cmpw    r8, r6      // compare high word
        bne-    LOCAL_LABEL(CompareExchangeLongGNMismatch)
        stw     r5, 4(r3)   // store the new low word
        stw     r4, 0(r3)   // store the new high word
LOCAL_LABEL(CompareExchangeLongGNMismatch):
        mr      r4, r9      // r9 is trashed by SPINLOCK_EXIT
        SPINLOCK_EXIT(CompareExchangeLongGN)
        // sync implied by SPINLOCK_EXIT
        mr      r3, r8
        blr

// LONG __stdcall IncrementGN(LONG volatile *Target)
ASMFUNC(IncrementGN)
        sync
        lwarx   r4, 0, r3
        addi    r4, r4, 1
        stwcx.  r4, 0, r3
        bne-    GLOBAL_LABEL(IncrementGN)
        sync
        mr      r3, r4
        blr

// LONG __stdcall DecrementGN(LONG volatile *Target)
ASMFUNC(DecrementGN)
        sync
        lwarx   r4, 0, r3
        addi    r4, r4, -1
        stwcx.  r4, 0, r3
        bne-    GLOBAL_LABEL(DecrementGN)
        sync
        mr      r3, r4
        blr

// UINT64 __stdcall IncrementLongGN(UINT64 volatile *Target)
ASMFUNC(IncrementLongGN)
        mr r5, r3       // result will be in r4:r3
        // sync implied by SPINLOCK_ENTER
        SPINLOCK_ENTER(iSpinLock, IncrementLongGN)
        lwz r4, 4(r5)   // load the low word
        lwz r3, 0(r5)   // load the high word
        addic r4, r4, 1 // increment, and record carry in XER[CA]
        addze r3, r3    // add the value of XER[CA] (a bit)
        stw r4, 4(r5)   // store the new low word back to Target
        stw r3, 0(r5)   // store the new high word back to Target
        SPINLOCK_EXIT(IncrementLongGN)
        // sync implied by SPINLOCK_EXIT
        blr

// UINT64 __stdcall DecrementLongGN(UINT64 volatile *Target)
ASMFUNC(DecrementLongGN)
        mr r5, r3       // result will be in r4:r3
        // sync implied by SPINLOCK_ENTER
        SPINLOCK_ENTER(iSpinLock, DecrementLongGN)
        lwz r4, 4(r5)   // load the low word
        lwz r3, 0(r5)   // load the high word
        addic r4, r4, -1// decrement, and record carry in XER[CA]
        addme r3, r3    // subtract the value of XER[CA] (a bit)
        stw r4, 4(r5)   // store the new low word back to Target
        stw r3, 0(r5)   // store the new high word back to Target
        SPINLOCK_EXIT(DecrementLongGN)
        // sync implied by SPINLOCK_EXIT
        blr


#ifdef _DEBUG
// void Frame::CheckExitFrameDebuggerCalls(void)
ASMFUNC(CheckExitFrameDebuggerCalls)
        // Spill the argument and link registers to the stack
        mflr r0
        stw  r0, 8(sp)

        stwu sp, -roundup(LINKAGE_AREA+32, 16)(sp)

        ARGUMENT_REGISTERS_GP(stw,LINKAGE_AREA,sp)

        // call void* PerformExitFrameChecks(void)
        bl EXTERN_LABEL(PerformExitFrameChecks)
        mr r12, r3

        // Restore the argument and link registers back
        ARGUMENT_REGISTERS_GP(lwz,LINKAGE_AREA,sp)

        lwz sp, 0(sp)
        lwz r0, 8(sp)
        mtlr r0

        // jump to the returned function pointer
        mtctr r12
        bctr
#endif //_DEBUG

//-----------------------------------------------------------------------------
// This helper routine enregisters the appropriate arguments and makes the
// actual call.
//-----------------------------------------------------------------------------
//ARG_SLOT
//__stdcall
//#ifdef _DEBUG
//      CallDescrWorkerInternal
//#else
//      CallDescrWorker
//#endif
//                     (LPVOID                   pSrcEnd,
//                      UINT32                   numStackSlots,
//                      const ArgumentRegisters *pArgumentRegisters,
//                      UINT32                   fpRetSize,
//                      LPVOID                   pTarget
//                     )
#ifdef _DEBUG
ASMFUNC(CallDescrWorkerInternal)
#else
ASMFUNC(CallDescrWorker)
#endif
        // prolog
        mflr r0
        stw r0, 8(sp)

        // deregister fpRetSize
        stw r6, (LINKAGE_AREA+3*4)(sp)

        // allocate frame with size roundup(4*numStackSlots+LINKAGE_AREA,16)
        slwi r8, r4, 2 // r8 = size of arguments
        addi r9, r8, LINKAGE_AREA+15
        clrrwi r9, r9, 4
        neg r9, r9
        stwux sp, sp, r9

        // copy the stack over
        sub r9, r3, r8 // r9 = src
        addi r8, sp, LINKAGE_AREA // r8 = dst

        cmpwi r4, 0
        mtctr r4
        beq- LOCAL_LABEL(ArgCopyDone)
LOCAL_LABEL(ArgCopyLoop):
        lwz r3, 0(r9)
        addi r9, r9, 4
        stw r3, 0(r8)
        addi r8, r8, 4
        bdnz LOCAL_LABEL(ArgCopyLoop)
LOCAL_LABEL(ArgCopyDone):

        // r12 = pArgumentRegisters (it is not possible to use r0 as index in lwz)
        mr r12, r5        
        // r0 = pTarget
        mr r0, r7

        // enregister float registers
        ARGUMENT_REGISTERS_FP(lfd,ArgumentRegisters_f,r12)
        // enregister integer registers
        ARGUMENT_REGISTERS_GP(lwz,ArgumentRegisters_r,r12)

        // make the call
        mr r12, r0

        // The lowest bit indicates the type of callsite to use
        andi. r0, r12, 1
        bne LOCAL_LABEL(ReturnBuffer)

        mtctr r12
        bctrl
        b LOCAL_LABEL(CallDone)

LOCAL_LABEL(ReturnBuffer):
        clrrwi r12, r12, 1
        mtctr r12
        bctrl
        oris r0, r0, 0

LOCAL_LABEL(CallDone):
        // epilog
        lwz sp, 0(sp)

        // enregister fpRetSize
        lwz r6, (LINKAGE_AREA+3*4)(sp)

        cmpwi   r6, 4
        bne+    LOCAL_LABEL(CallDescrFPReturnNot4)
        stfs    f1, (LINKAGE_AREA)(sp)
        lwz     r3, (LINKAGE_AREA)(sp)
LOCAL_LABEL(CallDescrFPReturnNot4):

        cmpwi   r6, 8
        bne+    LOCAL_LABEL(CallDescrFPReturnNot8)
        stfd    f1, (LINKAGE_AREA)(sp)
        lwz     r3, (LINKAGE_AREA)(sp)
        lwz     r4, (LINKAGE_AREA+4)(sp)
LOCAL_LABEL(CallDescrFPReturnNot8):

        lwz r0, 8(sp)
        mtlr r0
        blr

#ifdef _DEBUG
// int __fastcall HelperMethodFrameRestoreState(HelperMethodFrame * pFrame,
//                                              struct MachState * pState)
  #define pFrame r3
  #define pState r4
#else
// int __fastcall HelperMethodFrameRestoreState(struct MachState * pState)
  #define pState r3
#endif
ASMFUNC(HelperMethodFrameRestoreState)
        // restore the registers from the m_MachState stucture.  Note that
        // we only do this for register that were not saved on the stack
        // at the time the machine state snapshot was taken.

        lwz r5, MachState__pRetAddr(pState)
        cmpwi r5, 0

#ifdef _DEBUG
        bne+  LOCAL_LABEL(NoConfirm)
        mflr r0
        stw r0, 8(sp)

        stwu sp, -(LINKAGE_AREA + 8 + 20 * 4)(sp) // build a stack frame

        stmw r13, (LINKAGE_AREA + 8)(sp)

        // r3 == pFrame (this was passed to HelperMethodFrameRestoreState
        la r4, (LINKAGE_AREA + 8)(sp)
        bl EXTERN_LABEL(HelperMethodFrameConfirmState)

        // on return, r3 == MachState*, move back into pState
        mr pState,r3
        lwz r5, MachState__pRetAddr(pState)
        cmpwi r5, 0

        lwz sp, 0(sp)                      // restore the stack pointer
        lwz r0, 8(sp)                      // restore the link register
        mtlr r0
LOCAL_LABEL(NoConfirm):
#endif
        beq+ LOCAL_LABEL(DoRet)

        // Reload any spilled registers
        addi r5, pState, MachState__Regs // r5=&pState->_Regs[0]
        addi r6, pState, MachState__pRegs// r6=&pState->_pRegs[0]

    // Reload a register if it was spilled:
    //  if (pState->_pRegs[rnum-13] == &pState->_Regs[rnum-13]) {
    //    Reload register rnum from pState->_Regs[rnum]
    //  }
    #define ReloadRegister(reg)\
        lwz  r5, MachState__pRegs+(reg-13)*4(pState) \n \
        addi r6, pState, MachState__Regs+reg-13      \n \
        cmpw  r5, r6                                 \n \
        bne+ LOCAL_LABEL(reload_reg_##reg)           \n \
        lwz  r##reg, MachState__Regs+(reg-13)*4(pState) \n \
    LOCAL_LABEL(reload_reg_##reg):

        ReloadRegister(13)
        ReloadRegister(14)
        ReloadRegister(15)
        ReloadRegister(16)
        ReloadRegister(17)
        ReloadRegister(18)
        ReloadRegister(19)
        ReloadRegister(20)
        ReloadRegister(21)
        ReloadRegister(22)
        ReloadRegister(23)
        ReloadRegister(24)
        ReloadRegister(25)
        ReloadRegister(26)
        ReloadRegister(27)
        ReloadRegister(28)
        ReloadRegister(29)
        ReloadRegister(30)
        ReloadRegister(31)

LOCAL_LABEL(DoRet):
        li r3, 0
        blr
#undef pFrame
#undef pState


// Note that the debugger skips this entirely when doing SetIP,
// since COMPlusCheckForAbort should always return 0.  Excep.cpp:LeaveCatch
// asserts that to be true.  If this ends up doing more work, then the
// debugger may need additional support.
// void __stdcall JIT_EndCatch()
ASMFUNC(JIT_EndCatch)

#define FRAME_SIZE roundup(LINKAGE_AREA + 4*3, 16)

    // prolog
    mflr r0
    stw r0, 8(sp)
    stwu sp, -FRAME_SIZE(sp)

    li r3,0             // pCurThread
    li r4,0             // pCtx
    li r5,0             // pSEH
    bl EXTERN_LABEL(COMPlusEndCatch) // returns old esp value in r3

    stw r3, (FRAME_SIZE)(sp) // save old esp

    mr r4,r3        // esp
    mr r5,r30       // ebp
    lwz r3, (FRAME_SIZE + 8)(sp) // return address
    bl EXTERN_LABEL(COMPlusCheckForAbort) // returns old esp value

    // at this point, (FRAME_SIZE)(sp) = old esp value
    //                (FRAME_SIZE + 8)(sp) = return address into jitted code
    //                r3 = 0 (if no abort), address to jump to otherwise

    cmpwi r3, 0
    beq LOCAL_LABEL(NormalEndCatch)

    mr r12, r3
    lwz sp, (FRAME_SIZE)(sp)

    // make the jump
    mtctr r12
    bctr

LOCAL_LABEL(NormalEndCatch):
    lwz r0, (FRAME_SIZE + 8)(sp)
    lwz sp, (FRAME_SIZE)(sp)

    // return as usual
    mtlr r0
    blr

#undef FRAME_SIZE


//---------------------------------------------------------
// Performs an N/Direct call. This is a generic version
// that can handly any N/Direct call but is not as fast
// as more specialized versions.
//---------------------------------------------------------
//
// INT64 __stdcall NDirectGenericStubWorker(Thread *pThread,
//                                          NDirectMethodFrame *pFrame);
//
ASMFUNC(NDirectGenericStubWorker)

    // prolog
    mflr r0
    stw r31, -4(sp)
    stw r0, 8(sp)
    mr r31, sp

    #define LOCALS  (6*4) // local variables
    #define ARGS    (8*4) // space to pass arguments to worker methods

    // Locals
    #define pHeader (-LOCALS + 0)(r31)
    #define pvfn    (-LOCALS + 4)(r31)
    #define pLocals (-LOCALS + 8)(r31)
    #define pMLCode (-LOCALS + 12)(r31)
    // pad
    // saved r31     (-LOCALS + 28)(r31)

    // Parameters
    #define pThread (LINKAGE_AREA + 0)(r31)
    #define pFrame  (LINKAGE_AREA + 4)(r31)

#if NDirectGenericWorkerFrameSize != (LINKAGE_AREA + LOCALS)
 #error asmconstants.h is incorrect
#endif

#if (LINKAGE_AREA + LOCALS + ARGS) % 16 != 0
 #error Stack frame not alligned
#endif

    stwu sp, -(LINKAGE_AREA + LOCALS + ARGS)(sp)

    stw r3, pThread
    stw r4, pFrame

    la r5, pHeader
    bl EXTERN_LABEL(NDirectGenericStubComputeFrameSize)

    // r3 = AlignStack(cbAlloc)

    // alloca
    neg r3, r3
    stwux r31, sp, r3

    lwz r3, pThread
    lwz r4, pFrame
    lwz r5, pHeader
    la r6, pMLCode
    la r7, pLocals
    la r8, pvfn
    addi r9, sp, LINKAGE_AREA + ARGS // pAlloc
    bl EXTERN_LABEL(NDirectGenericStubBuildArguments)

#if ARGS % 16 != 0
 #error Stack frame not alligned
#endif

    // free the space for worker arguments
    stwu r31, (ARGS)(sp)

    // pvfn is now the function to invoke, pLocals points to the locals, and
    // r3 is flags that are ignored on PPC
    lwz r12, pvfn

    // enregister float registers
    ARGUMENT_REGISTERS_FP(lfd,-(LOCALS+8*13),r31)
    // enregister integer registers
    ARGUMENT_REGISTERS_GP(lwz,LINKAGE_AREA,sp)


    // make the call
    mtctr r12
    bctrl

// this label is used by the debugger, and must immediately follow the 'call'.
LOCAL_LABEL(NDirectGenericStubReturnFromCallLocal):


    // reclaim the space for worker arguments
    stwu r31, (-ARGS)(sp)

    // pass the return value
    mr r8, r3
    mr r9, r4

    lwz r3, pThread
    lwz r4, pFrame
    lwz r5, pHeader
    lwz r6, pMLCode
    lwz r7, pLocals
    bl EXTERN_LABEL(NDirectGenericStubPostCall)
    // return value is in r3:r4

    // epilog
    lwz sp, 0(sp)
    lwz r0, 8(sp)
    lwz r31, -4(sp)
    mtlr r0
    blr

    #undef  pThread
    #undef  pFrame
    #undef  pHeader
    #undef  pvfn
    #undef  pLocals
    #undef  pMLCode

    #undef ARGS
    #undef LOCALS

DATA_SECTION
#if defined(__APPLE__)
    .globl GLOBAL_LABEL(NDirectGenericStubReturnFromCall)
    .align 2
GLOBAL_LABEL(NDirectGenericStubReturnFromCall):
    .long LOCAL_LABEL(NDirectGenericStubReturnFromCallLocal)
#else
    .globl NDirectGenericStubReturnFromCall
    .align 2
NDirectGenericStubReturnFromCall:
    .long LOCAL_LABEL(NDirectGenericStubReturnFromCallLocal)
#endif
TEXT_SECTION

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::CallFieldGetter   private
//
//  Synopsis:   Calls the field getter function (Object::__FieldGetter) in
//              managed code by setting up the stack and calling the target
//
//+----------------------------------------------------------------------------
// void __stdcall CRemotingServices__CallFieldGetter(
//    MethodDesc *pMD,
//    LPVOID pThis,
//    LPVOID pFirst,
//    LPVOID pSecond,
//    LPVOID pThird)
ASMFUNC(CRemotingServices__CallFieldGetter)

    // prolog
    mflr r0
    stw r0, 8(sp)
    stwu sp, -roundup(LINKAGE_AREA + 4*4, 16)(sp)

    mr r11, r3 // setup the method desc ptr for the stub
    mr r3, r4 // pThis
    mr r4, r5 // pFirst
    mr r5, r6 // pSecond
    mr r6, r7 // pThird

    LOAD_EXTERNAL_VAR(g_dwTPStubAddr, r12)
    lwz r12, 0(r12)

    // call the TP stub
    mtctr r12
    bctrl

    // epilog
    lwz sp, 0(sp)
    lwz r0, 8(sp)
    mtlr r0
    blr

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::CallFieldSetter   private
//
//  Synopsis:   Calls the field setter function (Object::__FieldSetter) in
//              managed code by setting up the stack and calling the target
//
//+----------------------------------------------------------------------------
// void __stdcall CRemotingServices__CallFieldSetter(
//    MethodDesc *pMD,
//    LPVOID pThis,
//    LPVOID pFirst,
//    LPVOID pSecond,
//    LPVOID pThird)
ASMFUNC(CRemotingServices__CallFieldSetter)
    // prolog
    mflr r0
    stw r0, 8(sp)
    stwu sp, -roundup(LINKAGE_AREA + 3*4, 16)(sp)

    mr r11, r3 // setup the method desc ptr for the stub
    mr r3, r4 // pThis
    mr r4, r5 // pFirst
    mr r5, r6 // pSecond
    mr r6, r7 // pThird

    LOAD_EXTERNAL_VAR(g_dwTPStubAddr, r12)
    lwz r12, 0(r12)

    // call the TP stub
    mtctr r12
    bctrl

    // epilog
    lwz sp, 0(sp)
    lwz r0, 8(sp)
    mtlr r0
    blr

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::CheckForContextMatch   public
//
//  Synopsis:   This code generates a check to see if the current context and
//              the context of the proxy match.
//
//          returns zero if contexts match
//              returns non-zero if contexts do not match
//
// UINT_PTR __stdcall CRemotingServices__CheckForContextMatch(Object* pStubData)
//_CRemotingServices__CheckForContextMatch@4 proc public
//
//+----------------------------------------------------------------------------
ASMFUNC(CRemotingServices__CheckForContextMatch)
    // kR0 = stub data
    // result will be in kR0
    // must preserve kR3..kR11
    
    // prolog
    mflr r12
    stw r12, 8(sp)
    stwu sp, -roundup(LINKAGE_AREA + 4*12, 16)(sp)

    stw r0, (LINKAGE_AREA + 0*4)(sp)

    ARGUMENT_REGISTERS_GP(stw,(LINKAGE_AREA + 3*4),sp)

    stw r11, (LINKAGE_AREA + 11*4)(sp)

    // GetThread won't hopefully destroy the floating point registers

    LOAD_EXTERNAL_VAR(GetThread, r12)
    lwz r12, 0(r12)

    // Get the current thread
    CALL_FNCPTR_R12

    lwz r4, (LINKAGE_AREA + 0*4)(sp)    // Get the pointer to the context from the proxy

    lwz r3, Thread_m_Context(r3)        // Get the current context from the thread
    lwz r4, 4(r4)                       // Get the internal context id by unboxing the stub data

    sub r0, r3, r4

    ARGUMENT_REGISTERS_GP(lwz,(LINKAGE_AREA + 3*4),sp)

    lwz r11, (LINKAGE_AREA + 11*4)(sp)

    // epilog
    lwz sp, 0(sp)
    lwz r12, 8(sp)
    mtlr r12
    blr

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::DispatchInterfaceCall   public
//
//+----------------------------------------------------------------------------
ASMFUNC(CRemotingServices__DispatchInterfaceCall)
    // NOTE: At this point r11 contains the MethodDesc*

    mflr r12
    stw r12, 8(sp)
    stwu sp, -roundup(LINKAGE_AREA+4, 16)(sp)

    // get the this register: look at the callsite whether it contains
    // the magic instruction to determine the location of the this pointer

    lwz r0, 0(r12)

    // r12 = oris r0, r0, 0
    addis r12, 0, 0x6400
    cmpw r12, r0

    // r12 = this reg
    mr r12, r3
    bne+ LOCAL_LABEL(ThisRegDone)
    mr r12, r4
LOCAL_LABEL(ThisRegDone):
    stw r12, (LINKAGE_AREA + 0*4)(sp) // save this reg

    // Move into kR0 the stub data and call the stub
    lwz r0, TP_OFFSET_STUBDATA(r12)
    lwz r12, TP_OFFSET_STUB(r12)

    CALL_FNCPTR_R12

    cmpwi r0,0

    bne LOCAL_LABEL(CtxMismatch)

    // ROTORTODO: implement
    trap

LOCAL_LABEL(CtxMismatch):                       // Jump to TPStub
    // jump to OOContext label in TPStub
    LOAD_EXTERNAL_VAR(g_dwOOContextAddr, r12)
    lwz r12, 0(r12)
    
    // restore the stack frame
    lwz sp, 0(sp)
    lwz r0, 8(sp)
    mtlr r0

    mtctr r12
    bctr


// LPVOID __stdcall CTPMethodTable__CallTargetHelper2(
//     const void *pTarget,
//     LPVOID pvFirst,
//     LPVOID pvSecond)
ASMFUNC(CTPMethodTable__CallTargetHelper2)
    // prolog
    mflr r0
    stw r0, 8(sp)
    stwu sp, -roundup(LINKAGE_AREA + 8, 16)(sp)

    mr r12, r3
    mr r3, r4
    mr r4, r5

    // make the call
    mtctr r12
    bctrl

    // epilog
    lwz sp, 0(sp)
    lwz r0, 8(sp)
    mtlr r0
    blr

// LPVOID __stdcall CTPMethodTable__CallTargetHelper3(
//     const void *pTarget,
//     LPVOID pvFirst,
//     LPVOID pvSecond,
//     LPVOID pvThird
ASMFUNC(CTPMethodTable__CallTargetHelper3)
    // prolog
    mflr r0
    stw r0, 8(sp)
    stwu sp, -roundup(LINKAGE_AREA + 12, 16)(sp)

    mr r12, r3
    mr r3, r4
    mr r4, r5
    mr r5, r6

    // make the call
    mtctr r12
    bctrl

    // epilog
    lwz sp, 0(sp)
    lwz r0, 8(sp)
    mtlr r0
    blr

//+----------------------------------------------------------------------------
//
//  Method:     CTPMethodTable::GenericCheckForContextMatch private
//
//  Synopsis:   Calls the stub in the TP & returns TRUE if the contexts
//              match, FALSE otherwise.
//
//  Note:       1. Called during FieldSet/Get, used for proxy extensibility
//+----------------------------------------------------------------------------
// BOOL __stdcall CTPMethodTable__GenericCheckForContextMatch(Object* orTP)
ASMFUNC(CTPMethodTable__GenericCheckForContextMatch)

    // prolog
    mflr r0
    stw r0, 8(sp)
    stwu sp, -roundup(LINKAGE_AREA, 16)(sp)

    lwz r12, TP_OFFSET_STUB(r3)
    lwz r0, TP_OFFSET_STUBDATA(r3)

    CALL_FNCPTR_R12

    // NOTE: In the CheckForXXXMatch stubs (for URT ctx/ Ole32 ctx) eax is
    // non-zero if contexts *do not* match & zero if they do.
    subfic r3,r0,0
    adde r3,r3,r0

    // epilog
    lwz sp, 0(sp)
    lwz r0, 8(sp)
    mtlr r0
    blr

// void __stdcall getFPReturn(int fpSize, INT64 *pretVal)
//
// Retrieve a floating-point return value from the FPU, after a
// call to a function whose return type is float or double.  Note
// that the fpSize may not be 4 or 8, meaning that the floating-
// point state and the contents of pretVal must not be modified.
//
ASMFUNC(getFPReturn)
    cmpwi   r3, 4
    beq-    LOCAL_LABEL(GetFPReturn4)
    cmpwi   r3, 8
    bne+    LOCAL_LABEL(GetFPReturnNot8)
    stfd    f1, 0(r4)
LOCAL_LABEL(GetFPReturnNot8):
    blr
LOCAL_LABEL(GetFPReturn4):
    stfs    f1, 0(r4)
    blr

// void __stdcall setFPReturn(int fpSize, INT64 retVal)
//
// Load a floating-point return value into the FPU's top-of-stack,
// in preparation for a return from a function whose return type
// is float or double.  Note that fpSize may not be 4 or 8, meaning
// that the floating-point state must not be modified.
//
ASMFUNC(setFPReturn)
    stw     r4, (LINKAGE_AREA+1*4)(sp)
    stw     r5, (LINKAGE_AREA+2*4)(sp)
    cmpwi   r3, 4
    beq-    LOCAL_LABEL(SetFPReturn4)
    cmpwi   r3, 8
    bne+    LOCAL_LABEL(SetFPReturnNot8)
    lfd     f1, (LINKAGE_AREA+1*4)(sp)
LOCAL_LABEL(SetFPReturnNot8):
    blr
LOCAL_LABEL(SetFPReturn4):
    lfs     f1, (LINKAGE_AREA+1*4)(sp)
    blr

// LPVOID __stdcall GetCurrentIP(void);
ASMFUNC(GetCurrentIP)
    mflr r3
    blr

// LPVOID __stdcall GetCurrentSP(void);
ASMFUNC(GetCurrentSP)
    mr r3, sp
    blr

// INT64 __stdcall DllImportForDelegateGenericStubWorker(Thread *pThread,
//                     DelegateTransitionFrame *pFrame);
ASMFUNC(DllImportForDelegateGenericStubWorker)
    // ROTORTODO: implement
    trap

// this label is used by the debugger, and must immediately follow the 'call'.
LOCAL_LABEL(DllImportForDelegateGenericStubReturnFromCallLocal):
    blr

DATA_SECTION
#if defined(__APPLE__)
    .globl GLOBAL_LABEL(DllImportForDelegateGenericStubReturnFromCall)
    .align 2
GLOBAL_LABEL(DllImportForDelegateGenericStubReturnFromCall):
   .long LOCAL_LABEL(DllImportForDelegateGenericStubReturnFromCallLocal)
#else
    .globl DllImportForDelegateGenericStubReturnFromCall
    .align 2
DllImportForDelegateGenericStubReturnFromCall:
    .long LOCAL_LABEL(DllImportForDelegateGenericStubReturnFromCallLocal)
#endif
TEXT_SECTION

