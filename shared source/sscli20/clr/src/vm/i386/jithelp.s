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
//
//  *** NOTE:  If you make changes to this file, propagate the changes to
//             jithelp.s in this directory
//
//  This file uses AT&T i386 syntax. Search web for "AT&T Syntax versus Intel Syntax"
//  to get document describing differences between the AT&T i386 syntax and the Intel
//  syntax used by masm.

// This contains JITinterface routines that are 100% x86 assembly

#include "asmconstants.h"


.text

#ifdef PLATFORM_UNIX
#define STDMANGLE(name,args) name
#define STDMANGLEIMM(name,args) $##name
#define CMANGLE(name) name
#else
#define STDMANGLE(name,args) _##name##@##args
#define STDMANGLEIMM(name,args) $_##name##@##args
#define CMANGLE(name) _##name
#endif

#define FMANGLE(name,args) STDMANGLE(name,args)

// a handy macro for declaring a function
#define ASMFUNC(name)             \
        .globl name             ; \
name: ;

#define ASMFUNCEND()

// immediate operand. useful for constants defined using macros
#define IMM_HELPER(x) $ ## x
#define IMM(x)        IMM_HELPER(x)

// from clr/src/vm/eecallconv.h
#define ARGUMENT_REG1 %ecx
#define ARGUMENT_REG2 %edx


//***
//JIT_WriteBarrier* - GC write barrier helper
//
//Purpose:
//   Helper calls in order to assign an object to a field
//   Enables book-keeping of the GC.
//
//Entry:
//   EDX - address of ref-field (assigned to)
//   the resp. other reg - RHS of assignment
//
//Exit:
//
//Uses:
//       EDX is destroyed.
//
//Exceptions:
//
//*****************************************************************************

#ifdef _DEBUG
 #define DO_WRITE_BARRIER_ASSERT(rg, dst) \
        push    %edx    ; \
        push    %ecx    ; \
        push    %eax    ; \
                        ; \
        push    %rg     ; \
        push    %dst    ; \
        call    STDMANGLE(WriteBarrierAssert,8) ; \
                        ; \
        pop     %eax    ; \
        pop     %ecx    ; \
        pop     %edx
#else
 #define DO_WRITE_BARRIER_ASSERT(rg, dst)
#endif // _DEBUG

 #define DO_WRITE_BARRIER_CHECK(rg, dst)

#define WriteBarrierHelper(rg)                                                \
        .align 4                                                            ; \
        /* The entry point is the fully 'safe' one in which we check if */  ; \
        /* EDX (the REF begin updated) is actually in the GC heap */        ; \
ASMFUNC(STDMANGLE(JIT_CheckedWriteBarrier##rg,0))                           ; \
        /* check in the REF being updated is in the GC heap */              ; \
        cmp     CMANGLE(g_lowest_address), %edx                             ; \
        jb      LJIT_WriteBarrierOutOfRange##rg                             ; \
        cmp     CMANGLE(g_highest_address), %edx                            ; \
        jae     LJIT_WriteBarrierOutOfRange##rg                             ; \
        /* fall through to unchecked routine */                             ; \
        /* note that its entry point also happens to be aligned */          ; \
                                                                            ; \
        /* This entry point is used when you know the REF pointer being */  ; \
        /* updated is in the GC heap */                                     ; \
.globl STDMANGLE(JIT_WriteBarrier##rg,0)                                    ; \
STDMANGLE(JIT_WriteBarrier##rg,0):                                          ; \
        DO_WRITE_BARRIER_ASSERT(rg, edx)                                    ; \
        /* ALSO update the shadow GC heap if that is enabled */             ; \
        DO_WRITE_BARRIER_CHECK(rg, edx)                                     ; \
        cmp     CMANGLE(g_ephemeral_low), %rg                               ; \
        jb      LJIT_WriteBarrierOutOfRange##rg                             ; \
        cmp     CMANGLE(g_ephemeral_high), %rg                              ; \
        jae     LJIT_WriteBarrierOutOfRange##rg                             ; \
        mov     %rg, (%edx)                                                 ; \
                                                                            ; \
        shr     $10, %edx                                                   ; \
        add     CMANGLE(g_card_table), %edx                                 ; \
        cmpb    $0xFF, (%edx)                                               ; \
        jne     LJIT_WriteBarrierSet##rg                                    ; \
        ret                                                                 ; \
LJIT_WriteBarrierSet##rg:                                                   ; \
        movb    $0xFF, (%edx)                                               ; \
        ret                                                                 ; \
LJIT_WriteBarrierOutOfRange##rg:                                            ; \
        mov     %rg, (%edx)                                                 ; \
        ret                                                                 ; \
ASMFUNCEND()

//***
//JIT_ByRefWriteBarrier* - GC write barrier helper
//
//Purpose:
//   Helper calls in order to assign an object to a byref field
//   Enables book-keeping of the GC.
//
//Entry:
//   EDI - address of ref-field (assigned to)
//   ESI - address of the data  (source)
//   ECX can be trashed
//
//Exit:
//
//Uses:
//   EDI and ESI are incremented by a DWORD
//
//Exceptions:
//
//*****************************************************************************

#define ByRefWriteBarrierHelper()                                             \
        .align 4                                                            ; \
ASMFUNC(STDMANGLE(JIT_ByRefWriteBarrier,0))                                 ; \
        /* test for dest in range */                                        ; \
        mov     (%esi), %ecx                                                ; \
                                                                            ; \
        cmp     CMANGLE(g_lowest_address), %edi                             ; \
        jb      LJIT_ByRefWriteBarrierOutOfRange                            ; \
        cmp     CMANGLE(g_highest_address), %edi                            ; \
        jae     LJIT_ByRefWriteBarrierOutOfRange                            ; \
                                                                            ; \
        /* ALSO update the shadow GC heap if that is enabled */             ; \
        DO_WRITE_BARRIER_CHECK(ecx, edi)                                    ; \
                                                                            ; \
        /* test for *src in range */                                        ; \
        cmp     CMANGLE(g_ephemeral_low), %ecx                              ; \
        jb      LJIT_ByRefWriteBarrierOutOfRange                            ; \
        cmp     CMANGLE(g_ephemeral_high), %ecx                             ; \
        jae     LJIT_ByRefWriteBarrierOutOfRange                            ; \
                                                                            ; \
        /* write barrier */                                                 ; \
        mov     %ecx, (%edi)                                                ; \
        mov     %edi, %ecx                                                  ; \
        add     $4, %esi                                                    ; \
        add     $4, %edi                                                    ; \
                                                                            ; \
        shr     $10, %ecx                                                   ; \
        add     CMANGLE(g_card_table), %ecx                                 ; \
        cmpb    $0xFF, (%ecx)                                               ; \
        jne     LJIT_ByRefWriteBarrierSet                                   ; \
        ret                                                                 ; \
LJIT_ByRefWriteBarrierSet:                                                  ; \
        movb    $0xFF, (%ecx)                                               ; \
        ret                                                                 ; \
LJIT_ByRefWriteBarrierOutOfRange:                                           ; \
        /* dest = *src */                                                   ; \
        /* increment src and dest */                                        ; \
        mov     %ecx, (%edi)                                                ; \
        add     $4,%esi                                                     ; \
        add     $4,%edi                                                     ; \
        ret                                                                 ; \
ASMFUNCEND()

//*****************************************************************************
// Write barrier wrappers with fcall calling convention
//
#define UniversalWriteBarrierHelper(name)                                   ; \
        .align 4                                                            ; \
ASMFUNC(FMANGLE(JIT_##name,12))                                             ; \
        mov     %edx, %eax                                                  ; \
        mov     %ecx, %edx                                                  ; \
        jmp     STDMANGLE(JIT_##name##EAX,0)                                ; \
ASMFUNCEND()

// WriteBarrierStart and WriteBarrierEnd are used to determine bounds of
// WriteBarrier functions so can determine if got AV in them.
ASMFUNC(STDMANGLE(JIT_WriteBarrierStart,0))
ret
ASMFUNCEND()

UniversalWriteBarrierHelper(CheckedWriteBarrier)
UniversalWriteBarrierHelper(WriteBarrier)

WriteBarrierHelper(EAX)
WriteBarrierHelper(EBX)
WriteBarrierHelper(ECX)
WriteBarrierHelper(ESI)
WriteBarrierHelper(EDI)
WriteBarrierHelper(EBP)

ByRefWriteBarrierHelper()

ASMFUNC(STDMANGLE(JIT_WriteBarrierLast,0))
ret
ASMFUNCEND()


//*********************************************************************/
//llshl - long shift left
//
//Purpose:
//   Does a Long Shift Left (signed and unsigned are identical)
//   Shifts a long left any number of bits.
//
//       NOTE:  This routine has been adapted from the Microsoft CRTs.
//
//Entry:
//   EDX:EAX - long value to be shifted
//       ECX - number of bits to shift by
//
//Exit:
//   EDX:EAX - shifted value
//
ASMFUNC(STDMANGLE(JIT_LLsh,0))
// Handle shifts of between bits 0 and 31
        cmp     $32, %ecx
        jae     LLshMORE32
        shld    %cl,%eax,%edx
        shl     %cl,%eax
        ret
// Handle shifts of between bits 32 and 63
LLshMORE32:
        // The x86 shift instructions only use the lower 5 bits.
        mov     %eax,%edx
        xor     %eax,%eax
        shl     %cl,%edx
        ret
ASMFUNCEND()


//*********************************************************************/
//LRsh - long shift right
//
//Purpose:
//   Does a signed Long Shift Right
//   Shifts a long right any number of bits.
//
//       NOTE:  This routine has been adapted from the Microsoft CRTs.
//
//Entry:
//   EDX:EAX - long value to be shifted
//       ECX - number of bits to shift by
//
//Exit:
//   EDX:EAX - shifted value
//
ASMFUNC(STDMANGLE(JIT_LRsh,0))
// Handle shifts of between bits 0 and 31
        cmp     $32, %ecx
        jae     LRshMORE32
        shrd    %cl,%edx,%eax
        sar     %cl,%edx
        ret
// Handle shifts of between bits 32 and 63
LRshMORE32:
        // The x86 shift instructions only use the lower 5 bits.
        mov     %edx,%eax
        sar     $31,%edx
        sar     %cl,%eax
        ret
ASMFUNCEND()


//*********************************************************************/
// LRsz:
//Purpose:
//   Does a unsigned Long Shift Right
//   Shifts a long right any number of bits.
//
//       NOTE:  This routine has been adapted from the Microsoft CRTs.
//
//Entry:
//   EDX:EAX - long value to be shifted
//       ECX - number of bits to shift by
//
//Exit:
//   EDX:EAX - shifted value
//
ASMFUNC(STDMANGLE(JIT_LRsz,0))
// Handle shifts of between bits 0 and 31
        cmp     $32, %ecx
        jae     LRszMORE32
        shrd    %cl,%edx,%eax
        shr     %cl,%edx
        ret
// Handle shifts of between bits 32 and 63
LRszMORE32:
        // The x86 shift instructions only use the lower 5 bits.
        mov     %edx,%eax
        xor     %edx,%edx
        shr     %cl,%eax
        ret
ASMFUNCEND()

ASMFUNC(STDMANGLE(JIT_WriteBarrierReg_PreGrow,0))
        mov     %ecx, (%edx)
        cmp     $0xF0F0F0F0, %ecx
        jb      LNoWriteBarrierPre

        shr     $10, %edx
        cmpb    $0xFF, 0xF0F0F0F0(%edx)
        jne     LWriteBarrierPre
LNoWriteBarrierPre:
        ret
LWriteBarrierPre:
        movb    $0xFF, 0xF0F0F0F0(%edx)
        ret
ASMFUNCEND()

ASMFUNC(STDMANGLE(JIT_WriteBarrierReg_PostGrow,0))
        mov     %ecx, (%edx)
        cmp     $0xF0F0F0F0, %ecx
        jb      LNoWriteBarrierPost
        cmp     $0xF0F0F0F0, %ecx
        jae     LNoWriteBarrierPost

        shr     $10, %edx
        cmpb    $0xFF, 0xF0F0F0F0(%edx)
        jne     LWriteBarrierPost
LNoWriteBarrierPost:
        ret
LWriteBarrierPost:
        movb    $0xFF, 0xF0F0F0F0(%edx)
        ret
ASMFUNCEND()


//-----------------------------------------------------------------------------
ASMFUNC(STDMANGLE(JIT_TailCall,0))

// the stack layout at this point is:
//
//  ebp+8+4*nOldStackArgs   <- end of argument destination
//   ...                       ...
//  ebp+8+                     old args (size is nOldStackArgs)
//   ...                       ...
//  ebp+8                   <- start of argument destination
//  ebp+4                   ret addr
//  ebp+0                   saved ebp
//  ebp-c                   saved ebx, esi, edi (if have callee saved regs = 1)
//
//                          other stuff (local vars) in the jitted callers' frame
//
//  esp+20+4*nNewStackArgs  <- end of argument source
//   ...                       ...
//  esp+20+                    new args (size is nNewStackArgs) to be passed to the target of the tail-call
//   ...                       ...
//  esp+20                  <- start of argument source
//  esp+16                  nOldStackArgs
//  esp+12                  nNewStackArgs
//  esp+8                   flags (1 = have callee saved regs, 2 = virtual stub dispatch)
//  esp+4                   target addr
//  esp+0                   retaddr

#define RetAddr         0
#define TargetAddr      4
#define Flags           8
#define nNewStackArgs   12
#define nOldStackArgs   16
#define NewArgs         20

//  extra space is incremented as we push things on the stack along the way
#define ExtraSpace      0

        call    *CMANGLE(GetThread)

        push    %eax        // Thread*

        // save ArgumentRegisters
        push    %ecx
        push    %edx

#undef ExtraSpace
#define ExtraSpace      12  // pThread, ecx, edx

        // For GC stress, we always need to trip for GC
        testb   $0xFF, CMANGLE(g_TailCanSkipTripForGC)
        jz      TripForGC
        
        // Trip for GC only if necessary
        testl   IMM(TS_CatchAtSafePoint_ASM), Thread_m_State(%eax)
        jz      NoTripForGC

TripForGC:

// Create a MachState struct on the stack

// return address is already on the stack, but is separated from stack
// arguments by the extra arguments of JIT_TailCall. So we cant use it directly

        push    $0xDDDDDDDD

// Esp on unwind. Not needed as we it is deduced from the target method

        push    $0xCCCCCCCC
        push    %ebp
        push    %esp        // pEbp
        push    %ebx
        push    %esp        // pEbx
        push    %esi
        push    %esp        // pEsi
        push    %edi
        push    %esp        // pEdi

        push    %esp        // TailCallArgs*

        // JIT_TailCallHelper(TailCallArgs*)
        call    STDMANGLE(JIT_TailCallHelper,4) // this is __stdcall
        // on return, eax is the new esp value to use when returning

        // Restore MachState registers
        pop     %edx        // discard pEdi
        pop     %edi        // restore edi
        pop     %ecx        // discard pEsi
        pop     %esi        // restore esi
        pop     %edx        // discard pEbx
        pop     %ebx        // restore ebx
        pop     %ecx        // discard pEbp
        pop     %ebp        // restore ebp

        pop     %edx        // discard esp
        pop     %ecx        // discard return address
NoTripForGC:

#undef ExtraSpace
#define ExtraSpace      12  // pThread, ecx, edx

        mov     ExtraSpace+Flags(%esp), %edx            // edx = flags

        mov     ExtraSpace+nOldStackArgs(%esp), %eax    // eax = nOldStackArgs
        mov     ExtraSpace+nNewStackArgs(%esp), %ecx    // ecx = nNewStackArgs

        // restore callee saved registers
        test    $1, %edx
        jz      NoCalleeSaveRegisters

        mov     -4(%ebp), %edi                  // restore edi
        mov     -8(%ebp), %esi                  // restore esi
        mov     -12(%ebp), %ebx                 // restore ebx

NoCalleeSaveRegisters:
        push    4(%ebp)     // save the original return address for later
        push    %edi
        push    %esi

#undef ExtraSpace
#define ExtraSpace      24

#define CallersEsi 0
#define CallersEdi 4
#define OrigRetAddr 8
#define pThread 20

        lea     8(%ebp,%eax,4), %edi            // edi = the end of argument destination
        lea     ExtraSpace+NewArgs(%esp,%ecx,4), %esi // esi = the end of argument source

        mov     (%ebp), %ebp        // restore ebp (do not use ebp as scratch register to get a good stack trace in debugger)

        test    $2, %edx
        jnz     VSDTailCall

        // copy the arguments to the final destination
        test    %ecx, %ecx
        jz      ArgumentsCopied
ArgumentCopyLoop:
        // At this point, this is the value of the registers :
        // edi = end of argument dest
        // esi = end of argument source
        // ecx = nNewStackArgs
        mov     -4(%esi), %eax
        sub     $4, %edi
        sub     $4, %esi
        mov     %eax, (%edi)
        dec     %ecx
        jnz     ArgumentCopyLoop
ArgumentsCopied:

        // edi = the start of argument destination

        mov     4+4(%esp), %eax                     // return address
        mov     ExtraSpace+TargetAddr(%esp), %ecx   // target address

        mov     %eax, -4(%edi)                      // return address
        mov     %ecx, -8(%edi)                      // target address

        lea     -8(%edi), %eax                      // new value for esp

        pop     %esi
        pop     %edi
        pop     %ecx        // skip original return address
        pop     %edx
        pop     %ecx

        mov     %eax, %esp

        ret                 // Will branch to targetAddr.  This matches the
                            // "call" done by JITted code, keeping the
                            // call-ret count balanced.

        //---------------------------------------------------------------------
VSDTailCall:

        //
        //


        cmpl    STDMANGLEIMM(JIT_TailCallReturnFromVSD,0), OrigRetAddr(%esp)
        jz      VSDTailCallFrameInserted_DoSlideUpArgs // There is an exiting TailCallFrame that can be reused

        // try to allocate space for the frame / check whether there is enough space
        // If there is sufficient space, we will setup the frame and then slide 
        // the arguments up the stack. Else, we first need to slide the arguments
        // down the stack to make space for the TailCallFrame
        sub     $(SIZEOF_GSCookie + SIZEOF_TailCallFrame), %edi
        cmp     %esi, %edi
        jae     VSDSpaceForFrameChecked

        // There is not sufficient space to wedge in the TailCallFrame without 
        // overwriting the new arguments.
        // We need to allocate the extra space on the stack, 
        // and slide down the new arguments

        mov     %esi, %eax
        sub     %edi, %eax
        sub     %eax, %esp

        mov     %ecx, %eax                      // to subtract the size of arguments
        mov     %ecx, %edx                      // for counter

        neg     %eax

        // copy down the arguments to the final destination, need to copy all temporary storage as well
        add     $11, %edx                       // (ExtraSpace+NewArgs)/4

        lea     -(ExtraSpace+NewArgs)(%esi,%eax,4), %esi
        lea     -(ExtraSpace+NewArgs)(%edi,%eax,4), %edi

VSDAllocFrameCopyLoop:
        mov     (%esi), %eax
        mov     %eax, (%edi)
        add     $4, %esi
        add     $4, %edi
        dec     %edx
        jnz     VSDAllocFrameCopyLoop

        // the argument source and destination are same now
        mov     %edi, %esi

VSDSpaceForFrameChecked:

        // At this point, we have enough space on the stack for the TailCallFrame,
        // and we may already have slided down the arguments

        mov     CMANGLE(s_gsCookie), %eax      // GetProcessGSCookie()
        mov     %eax, (%edi)                   // set GSCookie
        mov     CMANGLE(g_TailCallFrameVptr), %eax // vptr
        mov     OrigRetAddr(%esp), %edx
        mov     %eax, SIZEOF_GSCookie(%edi)    // TailCallFrame::vptr
        mov     %edx, SIZEOF_GSCookie+28(%edi) // TailCallFrame::m_ReturnAddress

        mov     CallersEdi(%esp), %eax         // restored edi
        mov     CallersEsi(%esp), %edx         // restored esi
        mov     %eax, SIZEOF_GSCookie+12(%edi) // TailCallFrame::m_regs::edi
        mov     %edx, SIZEOF_GSCookie+16(%edi) // TailCallFrame::m_regs::esi
        mov     %ebx, SIZEOF_GSCookie+20(%edi) // TailCallFrame::m_regs::ebx
        mov     %ebp, SIZEOF_GSCookie+24(%edi) // TailCallFrame::m_regs::ebp

        mov     pThread(%esp), %ebx            // pThread

        mov     Thread_m_pFrame(%ebx), %eax
        lea     SIZEOF_GSCookie(%edi), %edx
        mov     %eax, SIZEOF_GSCookie+4(%edi)  // TailCallFrame::m_pNext
        mov     %edx, Thread_m_pFrame(%ebx)    // hook the new frame into the chain

	// setup ebp chain
	lea     SIZEOF_GSCookie+24(%edi), %ebp // TailCallFrame::m_regs::ebp

        // Do not copy arguments again if they are in place already
        // Otherwise, we will need to slide the new arguments up the stack
        cmp     %edi, %esi
        jne     VSDTailCallFrameInserted_DoSlideUpArgs

        // At this point, we must have already previously slided down the new arguments,
        // or the TailCallFrame is a perfect fit
        // set the caller address
        mov     ExtraSpace+RetAddr(%esp), %edx  // caller address
        mov     %edx, SIZEOF_GSCookie+8(%edi)   // TailCallFrame::m_CallerAddress

        // adjust edi as it would by copying
        neg     %ecx
        lea     (%edi,%ecx,4), %edi

        jmp     VSDArgumentsCopied

VSDTailCallFrameInserted_DoSlideUpArgs:
        // set the caller address
        mov     ExtraSpace+RetAddr(%esp), %edx  // caller address
        mov     %edx, SIZEOF_GSCookie+8(%edi)   // TailCallFrame::m_CallerAddress

        // copy the arguments to the final destination
        test    %ecx, %ecx
        jz      VSDArgumentsCopied
VSDArgumentCopyLoop:
        mov     -4(%esi), %eax
        sub     $4, %edi
        sub     $4, %esi
        mov     %eax, (%edi)
        dec     %ecx
        jnz     VSDArgumentCopyLoop
VSDArgumentsCopied:

        // edi = the start of argument destination

        mov     ExtraSpace+TargetAddr(%esp), %ecx   // target address

        movl    STDMANGLEIMM(JIT_TailCallReturnFromVSD,0), -4(%edi)    // return address
        mov     %ecx, -12(%edi)             // address of indirection cell
        mov     (%ecx), %ecx
        mov     %ecx, -8(%edi)              // target address

        // skip original return address and saved esi, edi
        add     $12, %esp

        pop     %edx
        pop     %ecx

        lea     -12(%edi), %esp             // new value for esp
        pop     %eax

        ret                 // Will branch to targetAddr.  This matches the
                            // "call" done by JITted code, keeping the
                            // call-ret count balanced.

        // a fake virtual stub dispatch register indirect callsite
        nop
        nop
        nop
        call    *(%eax)

.globl STDMANGLE(JIT_TailCallReturnFromVSD,0)
STDMANGLE(JIT_TailCallReturnFromVSD,0):
#ifdef _DEBUG
        nop                         // blessed callsite
#endif
        call    VSDHelperLabel      // keep call-ret count balanced.
VSDHelperLabel:

// Stack at this point :
//    ...
// m_ReturnAddress
// m_regs
// m_CallerAddress
// m_pThread
// vtbl
// GSCookie
// &VSDHelperLabel
#define OffsetOfTailCallFrame 8

// ebx = pThread

#ifdef _DEBUG
        mov     CMANGLE(s_gsCookie), %esi       // GetProcessGSCookie()
        cmp     %esi, OffsetOfTailCallFrame-SIZEOF_GSCookie(%esp)
        je      TailCallFrameGSCookieIsValid
	call	STDMANGLE(JIT_FailFast,0)
    TailCallFrameGSCookieIsValid:
#endif
        // remove the padding frame from the chain
        mov     OffsetOfTailCallFrame+4(%esp), %esi                // edx = TailCallFrame::m_Next
        mov     %esi, Thread_m_pFrame(%ebx)

        // skip the frame
        add     $20, %esp   // &VSDHelperLabel, GSCookie, vtbl, m_Next, m_CallerAddress

        pop     %edi        // restore callee saved registers
        pop     %esi
        pop     %ebx
        pop     %ebp

        ret                 // return to m_ReturnAddress

ASMFUNCEND()

//-----------------------------------------------------------------------------

ASMFUNC(STDMANGLE(JIT_MonStart,0))
        ret
ASMFUNCEND()

ASMFUNC(STDMANGLE(JIT_MonEnterWorker,12))
        // Initialize delay value for retry with exponential backoff
        pushl   %ebx
        movl    $50, %ebx

        // We need yet another register to avoid refetching the thread object
        pushl   %esi
        
        // Check if the instance is NULL.
        testl   ARGUMENT_REG1, ARGUMENT_REG1
        jz      MonEnterFramedLockHelper

        call    *CMANGLE(GetThread)
        movl    %eax, %esi
                
        // Check if we can abort here
        movl    Thread_m_State(%esi), %eax
        andl    IMM(TS_AbortRequested_ASM), %eax
        jz      MonEnterRetryThinLock
        // go through the slow code path to initiate ThreadAbort.
        jmp     MonEnterFramedLockHelper

MonEnterRetryThinLock:
        // Fetch the object header dword
        movl    -SyncBlockIndexOffset_ASM(ARGUMENT_REG1), %eax

        // Check whether we have the "thin lock" layout, the lock is free and the spin lock bit not set
        test    IMM(SBLK_COMBINED_MASK_ASM), %eax
        jnz     MonEnterNeedMoreTests

        // Everything is fine - get the thread id to store in the lock
        movl    Thread_m_ThreadId(%esi), %edx

        // If the thread id is too large, we need a syncblock for sure
        cmpl    IMM(SBLK_MASK_LOCK_THREADID_ASM), %edx
        ja      MonEnterFramedLockHelper

        // We want to store a new value with the current thread id set in the low 10 bits
        orl     %eax, %edx
        nop
        cmpxchgl    %edx, -SyncBlockIndexOffset_ASM(ARGUMENT_REG1)
        jnz     MonEnterPrepareToWaitThinLock

        // Everything went fine and we're done
        addl    $1, Thread_m_dwLockCount(%esi)
        popl    %esi
        popl    %ebx
        ret

MonEnterNeedMoreTests:
        // Ok, it's not the simple case - find out which case it is
        testl   IMM(BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_ASM), %eax
        jnz     MonEnterHaveHashOrSyncBlockIndex

        // The header is transitioning or the lock - treat this as if the lock was taken
        testl   IMM(BIT_SBLK_SPIN_LOCK_ASM), %eax
        jnz     MonEnterPrepareToWaitThinLock

        // Here we know we have the "thin lock" layout, but the lock is not free.
        // It could still be the recursion case - compare the thread id to check
        movl    %eax, %edx
        andl    IMM(SBLK_MASK_LOCK_THREADID_ASM), %edx
        cmpl    Thread_m_ThreadId(%esi), %edx
        jne     MonEnterPrepareToWaitThinLock

        // Ok, the thread id matches, it's the recursion case.
        // Bump up the recursion level and check for overflow
        leal    SBLK_LOCK_RECLEVEL_INC_ASM(%eax), %edx
        testl   IMM(SBLK_MASK_LOCK_RECLEVEL_ASM), %edx
        jz      MonEnterFramedLockHelper

        // Try to put the new recursion level back. If the header was changed in the meantime,
        // we need a full retry, because the layout could have changed.
        nop
        cmpxchgl    %edx, -SyncBlockIndexOffset_ASM(ARGUMENT_REG1)
        jnz     MonEnterRetryHelperThinLock

        // Everything went fine and we're done
        popl    %esi
        popl    %ebx
        ret

MonEnterPrepareToWaitThinLock:
        // If we are on an MP system, we try spinning for a certain number of iterations
        cmpl    $1, SYSTEM_INFO_dwNumberOfProcessors+CMANGLE(g_SystemInfo)
        jle     MonEnterFramedLockHelper

        // exponential backoff: delay by approximately 2*ebx clock cycles (on a PIII)
        movl    %ebx, %eax
MonEnterdelayLoopThinLock:
        rep
        nop     // indicate to the CPU that we are spin waiting (useful for some Intel P4 multiprocs)
        decl    %eax
        jnz     MonEnterdelayLoopThinLock

        // next time, wait 3 times as long
        imul    $3, %ebx, %ebx

        imul    $20000, SYSTEM_INFO_dwNumberOfProcessors+CMANGLE(g_SystemInfo), %eax
        cmpl    %eax, %ebx
        jle     MonEnterRetryHelperThinLock

        jmp     MonEnterFramedLockHelper

MonEnterRetryHelperThinLock:
        jmp     MonEnterRetryThinLock

MonEnterHaveHashOrSyncBlockIndex:
        // If we have a hash code already, we need to create a sync block
        testl   IMM(BIT_SBLK_IS_HASHCODE_ASM), %eax
        jnz     MonEnterFramedLockHelper

        // Ok, we have a sync block index - just and out the top bits and grab the syncblock index
        andl    IMM(MASK_SYNCBLOCKINDEX_ASM), %eax

        // Get the sync block pointer.
        movl    CMANGLE(g_pSyncTable), ARGUMENT_REG2
        movl    SyncTableEntry_m_SyncBlock(ARGUMENT_REG2, %eax, SizeOfSyncTableEntry_ASM), ARGUMENT_REG2

        // Check if the sync block has been allocated.
        testl   ARGUMENT_REG2, ARGUMENT_REG2
        jz      MonEnterFramedLockHelper

        // Get a pointer to the lock object.
        leal    SyncBlock_m_Monitor(ARGUMENT_REG2), ARGUMENT_REG2

        // Attempt to acquire the lock.
MonEnterRetrySyncBlock:
        movl    AwareLock_m_MonitorHeld(ARGUMENT_REG2), %eax
        testl   %eax, %eax
        jne     MonEnterHaveWaiters

        // Common case, lock isn't held and there are no waiters. Attempt to
        // gain ownership ourselves.
        movl    $1, ARGUMENT_REG1
        nop
        cmpxchgl    ARGUMENT_REG1, AwareLock_m_MonitorHeld(ARGUMENT_REG2)
        jnz     MonEnterRetryHelperSyncBlock

        // Success. Save the thread object in the lock and increment the use count.
        movl    %esi, AwareLock_m_HoldingThread (ARGUMENT_REG2)
        incl    Thread_m_dwLockCount(%esi)
        incl    AwareLock_m_Recursion(ARGUMENT_REG2)

#if defined(MON_DEBUG) && defined(TRACK_SYNC)
        pushl   ARGUMENT_REG2   // AwareLock
        pushl   4(%esp)         // return address
        call    STDMANGLE(EnterSyncHelper,8)
#endif // defined(MON_DEBUG) && defined(TRACK_SYNC)

        popl    %esi
        popl    %ebx
        ret

        // It's possible to get here with waiters but no lock held, but in this
        // case a signal is about to be fired which will wake up a waiter. So
        // for fairness sake we should wait too.
        // Check first for recursive lock attempts on the same thread.
MonEnterHaveWaiters:
        // Is mutex already owned by current thread?
        cmpl    %esi, AwareLock_m_HoldingThread(ARGUMENT_REG2)
        jne     MonEnterPrepareToWait

        // Yes, bump our use count.
        incl    AwareLock_m_Recursion(ARGUMENT_REG2)
#if defined(MON_DEBUG) && defined(TRACK_SYNC)
        pushl   ARGUMENT_REG2   // AwareLock
        pushl   4(%esp)         // return address
        call    STDMANGLE(EnterSyncHelper,8)
#endif // defined(MON_DEBUG) && defined(TRACK_SYNC)
        popl    %esi
        popl    %ebx
        ret

MonEnterPrepareToWait:
        // If we are on an MP system, we try spinning for a certain number of iterations
        cmpl    $1, SYSTEM_INFO_dwNumberOfProcessors+CMANGLE(g_SystemInfo)
        jle     MonEnterHaveWaiters1

        // exponential backoff: delay by approximately 2*ebx clock cycles (on a PIII)
        movl    %ebx, %eax
MonEnterdelayLoop:
        rep
        nop         // indicate to the CPU that we are spin waiting (useful for some Intel P4 multiprocs)
        decl    %eax
        jnz     MonEnterdelayLoop

        // next time, wait 3 times as long
        imul    $3, %ebx, %ebx

        imul    $20000, SYSTEM_INFO_dwNumberOfProcessors+CMANGLE(g_SystemInfo), %eax
        cmpl    %eax, %ebx
        jle     MonEnterRetrySyncBlock

MonEnterHaveWaiters1:

        popl    %esi
        popl    %ebx

        // Place AwareLock in arg1 then call contention helper.
        movl    ARGUMENT_REG2, ARGUMENT_REG1
        jmp     STDMANGLE(JITutil_MonContention,12)

MonEnterRetryHelperSyncBlock:
        jmp     MonEnterRetrySyncBlock

        // ECX has the object to synchronize on
MonEnterFramedLockHelper:
        popl    %esi
        popl    %ebx
        jmp     STDMANGLE(JIT_MonEnterWorker_Portable,12)

ASMFUNCEND()

ASMFUNC(STDMANGLE(JIT_MonTryEnter,12))
        // Save the timeout parameter.
        pushl   ARGUMENT_REG2

        // Initialize delay value for retry with exponential backoff
        pushl   %ebx
        movl    $50, %ebx
                
        // The thin lock logic needs another register to store the thread
        pushl   %esi
        
        // Check if the instance is NULL.
        test    ARGUMENT_REG1, ARGUMENT_REG1
        jz      MonTryEnterFramedLockHelper

        // Check if the timeout looks valid
        cmpl    $-1, ARGUMENT_REG2
        jl      MonTryEnterFramedLockHelper
        
        // Get the thread right away, we'll need it in any case
        call    *CMANGLE(GetThread)
        movl    %eax, %esi

        // Check if we can abort here
        movl    Thread_m_State(%esi), %eax
        andl    IMM(TS_AbortRequested_ASM), %eax
        jz      MonTryEnterRetryThinLock
        // go through the slow code path to initiate ThreadAbort.
        jmp     MonTryEnterFramedLockHelper

MonTryEnterRetryThinLock:
        // Get the header dword and check its layout
        movl    -SyncBlockIndexOffset_ASM(ARGUMENT_REG1), %eax

        // Check whether we have the "thin lock" layout, the lock is free and the spin lock bit not set
        test    IMM(SBLK_COMBINED_MASK_ASM), %eax
        jnz     MonTryEnterNeedMoreTests

        // Ok, everything is fine. Fetch the thread id and make sure it's small enough for thin locks
        movl    Thread_m_ThreadId(%esi), %edx
        cmpl    IMM(SBLK_MASK_LOCK_THREADID_ASM), %edx
        ja      MonTryEnterFramedLockHelper

        // Try to put our thread id in there
        orl     %eax, %edx
        nop
        cmpxchgl    %edx, -SyncBlockIndexOffset_ASM(ARGUMENT_REG1)
        jnz     MonTryEnterRetryHelperThinLock

        // Got the lock - everything is fine"
        addl    $1, Thread_m_dwLockCount(%esi)
        popl    %esi

        // Delay value no longer needed
        popl    %ebx

        // Timeout parameter not needed, ditch it from the stack.
        addl    $4, %esp

        movl    $1, %eax
        ret

MonTryEnterNeedMoreTests: 
        // Ok, it's not the simple case - find out which case it is
        testl   IMM(BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_ASM), %eax
        jnz     MonTryEnterHaveSyncBlockIndexOrHash

        // The header is transitioning or the lock is taken
        testl   IMM(BIT_SBLK_SPIN_LOCK_ASM), %eax
        jnz     MonTryEnterRetryHelperThinLock

        movl    %eax, %edx
        andl    IMM(SBLK_MASK_LOCK_THREADID_ASM), %edx
        cmpl    Thread_m_ThreadId(%esi), %edx
        jne     MonTryEnterPrepareToWaitThinLock

        // Ok, the thread id matches, it's the recursion case.
        // Bump up the recursion level and check for overflow
        leal    SBLK_LOCK_RECLEVEL_INC_ASM(%eax), %edx
        testl   IMM(SBLK_MASK_LOCK_RECLEVEL_ASM), %edx
        jz      MonTryEnterFramedLockHelper

        // Try to put the new recursion level back. If the header was changed in the meantime,
        // we need a full retry, because the layout could have changed.
        nop
        cmpxchgl    %edx, -SyncBlockIndexOffset_ASM(ARGUMENT_REG1)
        jnz     MonTryEnterRetryHelperThinLock

        // Everything went fine and we're done
        popl    %esi
        popl    %ebx

        // Timeout parameter not needed, ditch it from the stack.
        addl    $4, %esp

        movl    $1, %eax
        ret

MonTryEnterPrepareToWaitThinLock:
        // If we are on an MP system, we try spinning for a certain number of iterations
        cmpl    $1, SYSTEM_INFO_dwNumberOfProcessors+CMANGLE(g_SystemInfo)
        jle     MonTryEnterFramedLockHelper

        // exponential backoff: delay by approximately 2*ebx clock cycles (on a PIII)
        movl    %ebx, %eax
MonTryEnterdelayLoopThinLock:
        rep
        nop     // indicate to the CPU that we are spin waiting (useful for some Intel P4 multiprocs)
        decl    %eax
        jnz     MonTryEnterdelayLoopThinLock

        // next time, wait 3 times as long
        imul    $3, %ebx, %ebx

        imul    $20000, SYSTEM_INFO_dwNumberOfProcessors+CMANGLE(g_SystemInfo), %eax
        cmpl    %eax, %ebx
        jle     MonTryEnterRetryHelperThinLock

        jmp     MonTryEnterWouldBlock

MonTryEnterRetryHelperThinLock: 
        jmp     MonTryEnterRetryThinLock


MonTryEnterHaveSyncBlockIndexOrHash: 
        // If we have a hash code already, we need to create a sync block
        testl   IMM(BIT_SBLK_IS_HASHCODE_ASM), %eax
        jnz     MonTryEnterFramedLockHelper

        // Just and out the top bits and grab the syncblock index
        andl    IMM(MASK_SYNCBLOCKINDEX_ASM), %eax

        // Get the sync block pointer.
        movl    CMANGLE(g_pSyncTable), ARGUMENT_REG2
        movl    SyncTableEntry_m_SyncBlock(ARGUMENT_REG2, %eax, SizeOfSyncTableEntry_ASM), ARGUMENT_REG2

        // Check if the sync block has been allocated.
        testl   ARGUMENT_REG2, ARGUMENT_REG2
        jz      MonTryEnterFramedLockHelper

        // Get a pointer to the lock object.
        leal    SyncBlock_m_Monitor(ARGUMENT_REG2), ARGUMENT_REG2

MonTryEnterRetrySyncBlock:
        // Attempt to acquire the lock.
        movl    AwareLock_m_MonitorHeld(ARGUMENT_REG2), %eax
        testl   %eax, %eax
        jne     MonTryEnterHaveWaiters

        // We need another scratch register for what follows, so save EBX now so
        // we can use it for that purpose."
        pushl   %ebx

        // Common case, lock isn't held and there are no waiters. Attempt to
        // gain ownership ourselves.
        movl    $1, %ebx
        nop
        cmpxchgl    %ebx, AwareLock_m_MonitorHeld(ARGUMENT_REG2)

        popl    %ebx

        jnz     MonTryEnterRetryHelperSyncBlock

        // Success. Save the thread object in the lock and increment the use count.
        mov     %esi, AwareLock_m_HoldingThread (ARGUMENT_REG2)
        incl    AwareLock_m_Recursion(ARGUMENT_REG2)        
        incl    Thread_m_dwLockCount(%esi)

#if defined(MON_DEBUG) && defined(TRACK_SYNC)
        pushl   ARGUMENT_REG2   // AwareLock
        pushl   4(%esp)         // return address
        call    STDMANGLE(EnterSyncHelper,8)
#endif // defined(MON_DEBUG) && defined(TRACK_SYNC)

        popl    %esi
        popl    %ebx

        // Timeout parameter not needed, ditch it from the stack.
        addl    $4, %esp

        movl    $1, %eax
        ret

        // It's possible to get here with waiters but no lock held, but in this
        // case a signal is about to be fired which will wake up a waiter. So
        // for fairness sake we should wait too.
        // Check first for recursive lock attempts on the same thread.
MonTryEnterHaveWaiters:
        // Is mutex already owned by current thread?
        cmpl    %esi, AwareLock_m_HoldingThread(ARGUMENT_REG2)
        jne     MonTryEnterPrepareToWait

        // Yes, bump our use count.
        incl    AwareLock_m_Recursion(ARGUMENT_REG2)
#if defined(MON_DEBUG) && defined(TRACK_SYNC)
        pushl   ARGUMENT_REG2   // AwareLock
        pushl   4(%esp)         // return address
        call    STDMANGLE(EnterSyncHelper,8)
#endif // defined(MON_DEBUG) && defined(TRACK_SYNC)
        popl    %esi
        popl    %ebx

        // Timeout parameter not needed, ditch it from the stack.
        addl    $4, %esp

        movl    $1, %eax
        ret

MonTryEnterPrepareToWait:
        // If we are on an MP system, we try spinning for a certain number of iterations
        cmpl    $1, SYSTEM_INFO_dwNumberOfProcessors+CMANGLE(g_SystemInfo)
        jle     MonTryEnterWouldBlock

        // exponential backoff: delay by approximately 2*ebx clock cycles (on a PIII)
        movl    %ebx, %eax
MonTryEnterdelayLoop:
        rep
        nop     // indicate to the CPU that we are spin waiting (useful for some Intel P4 multiprocs)
        decl    %eax
        jnz     MonTryEnterdelayLoop

        // next time, wait 3 times as long
        imul    $3, %ebx, %ebx

        imul    $20000, SYSTEM_INFO_dwNumberOfProcessors+CMANGLE(g_SystemInfo), %eax
        cmpl    %eax, %ebx
        jle     MonTryEnterRetrySyncBlock

        // We would need to block to enter the section. Return failure if
        // timeout is zero, else call the framed helper to do the blocking
        // form of TryEnter.
MonTryEnterWouldBlock: 
        popl    %esi
        popl    %ebx
        popl    ARGUMENT_REG2
        test    ARGUMENT_REG2, ARGUMENT_REG2
        jnz     MonTryEnterBlock
        xorl    %eax, %eax
        ret

MonTryEnterRetryHelperSyncBlock: 
        jmp     MonTryEnterRetrySyncBlock

MonTryEnterFramedLockHelper: 
        // ARGUMENT_REG1 has the object to synchronize on, must retrieve the
        // timeout parameter from the stack.
        popl    %esi
        popl    %ebx
        popl    ARGUMENT_REG2
MonTryEnterBlock:         
        jmp     STDMANGLE(JIT_MonTryEnter_Portable,12)

ASMFUNCEND()

ASMFUNC(STDMANGLE(JIT_MonExitWorker,12))

        pushl   %esi
        
        // Check if the instance is NULL.
        testl   ARGUMENT_REG1, ARGUMENT_REG1
        jz      MonExitFramedLockHelper

        call    *CMANGLE(GetThread)
        movl    %eax, %esi

MonExitRetryThinLock: 
        // Fetch the header dword and check its layout and the spin lock bit
        movl    -SyncBlockIndexOffset_ASM(ARGUMENT_REG1), %eax
        test    IMM(BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_SPIN_LOCK_ASM), %eax
        jnz     MonExitNeedMoreTests

        // Ok, we have a "thin lock" layout - check whether the thread id matches
        movl    %eax, %edx
        andl    IMM(SBLK_MASK_LOCK_THREADID_ASM), %edx
        cmpl    Thread_m_ThreadId(%esi), %edx
        jne     MonExitFramedLockHelper

        // Check the recursion level"
        testl   IMM(SBLK_MASK_LOCK_RECLEVEL_ASM), %eax
        jne     MonExitDecRecursionLevel

        // It's zero - we're leaving the lock.
        // So try to put back a zero thread id.
        // edx and eax match in the thread id bits, and edx is zero elsewhere, so the xor is sufficient
        xorl    %eax, %edx
        nop
        cmpxchgl    %edx, -SyncBlockIndexOffset_ASM(ARGUMENT_REG1)
        jnz     MonExitRetryHelperThinLock

        // We're done
        subl    $1, Thread_m_dwLockCount(%esi)
        popl    %esi
        ret

MonExitDecRecursionLevel: 
        leal    -SBLK_LOCK_RECLEVEL_INC_ASM(%eax), %edx
        nop
        cmpxchgl    %edx, -SyncBlockIndexOffset_ASM(ARGUMENT_REG1)
        jnz     MonExitRetryHelperThinLock

        // We're done
        popl    %esi
        ret

MonExitNeedMoreTests: 
        // Forward all special cases to the slow helper
        testl   IMM(BIT_SBLK_IS_HASHCODE_OR_SPIN_LOCK_ASM), %eax
        jnz     MonExitFramedLockHelper

        // Get the sync block index and use it to compute the sync block pointer
        movl    CMANGLE(g_pSyncTable), ARGUMENT_REG2
        andl    IMM(MASK_SYNCBLOCKINDEX_ASM), %eax
        movl    SyncTableEntry_m_SyncBlock(ARGUMENT_REG2, %eax, SizeOfSyncTableEntry_ASM), ARGUMENT_REG2

        // was there a sync block?
        test    ARGUMENT_REG2, ARGUMENT_REG2
        jz      MonExitFramedLockHelper

        // Get a pointer to the lock object.
        lea     SyncBlock_m_Monitor(ARGUMENT_REG2), ARGUMENT_REG2

        // Check if lock is held.
        cmpl    %esi, AwareLock_m_HoldingThread(ARGUMENT_REG2)
        jne     MonExitFramedLockHelper

#if defined(MON_DEBUG) && defined(TRACK_SYNC)
        pushl   ARGUMENT_REG1   // preserve regs
        pushl   ARGUMENT_REG2

        pushl   ARGUMENT_REG2   // AwareLock
        pushl   8(%esp)         // return address
        call    STDMANGLE(LeaveSyncHelper,8)

        pop     ARGUMENT_REG2   // restore regs
        pop     ARGUMENT_REG1
#endif // defined(MON_DEBUG) && defined(TRACK_SYNC)

        // Reduce our recursion count.
        decl    AwareLock_m_Recursion(ARGUMENT_REG2)
        jz      MonExitLastRecursion

        popl    %esi
        ret

MonExitRetryHelperThinLock: 
        jmp     MonExitRetryThinLock

MonExitFramedLockHelper: 
        popl    %esi
        jmp     STDMANGLE(JIT_MonExitWorker,12)

        // This is the last count we held on this lock, so release the lock.
MonExitLastRecursion: 
        decl    Thread_m_dwLockCount(%esi)
        movl    $0, AwareLock_m_HoldingThread(ARGUMENT_REG2)

MonExitRetry: 
        movl    AwareLock_m_MonitorHeld(ARGUMENT_REG2), %eax
        lea     -1(%eax), %esi
        nop
        cmpxchg %esi, AwareLock_m_MonitorHeld(ARGUMENT_REG1)
        jne     MonExitRetryHelper
        popl    %esi
        testl   $0xFFFFFFFE, %eax
        jne     MonExitMustSignal

        ret

MonExitMustSignal:
        mov     ARGUMENT_REG2, ARGUMENT_REG1
        jmp     STDMANGLE(JITutil_MonSignal,12)

MonExitRetryHelper: 
        jmp     MonExitRetry

ASMFUNCEND()

ASMFUNC(STDMANGLE(JIT_MonEnterStatic,12))
        // We need another scratch register for what follows, so save EBX now so
        // we can use it for that purpose.
        pushl   %ebx

        // Attempt to acquire the lock
MonEnterStaticRetry: 
        movl    AwareLock_m_MonitorHeld(ARGUMENT_REG1), %eax
        testl   %eax, %eax
        jne     MonEnterStaticHaveWaiters

        // Common case, lock isn't held and there are no waiters. Attempt to
        // gain ownership ourselves.
        movl    $1, %ebx
        nop
        cmpxchgl    %ebx, AwareLock_m_MonitorHeld(ARGUMENT_REG1)
        jnz     MonEnterStaticRetryHelper

        popl    %ebx

        // Success. Save the thread object in the lock and increment the use count.
        call    *CMANGLE(GetThread)
        mov     %eax, AwareLock_m_HoldingThread(ARGUMENT_REG1)
        incl    AwareLock_m_Recursion(ARGUMENT_REG1)
        incl    Thread_m_dwLockCount(%eax)        

#if defined(MON_DEBUG) && defined(TRACK_SYNC)
        pushl   ARGUMENT_REG1   // AwareLock
        pushl   4(%esp)         // return address
        call    STDMANGLE(EnterSyncHelper,8)
#endif // defined(MON_DEBUG) && defined(TRACK_SYNC)
        ret

        // It's possible to get here with waiters but no lock held, but in this
        // case a signal is about to be fired which will wake up a waiter. So
        // for fairness sake we should wait too.
        // Check first for recursive lock attempts on the same thread.
MonEnterStaticHaveWaiters: 
        // Get thread but preserve EAX (contains cached contents of m_MonitorHeld).
        pushl   %eax
        call    *CMANGLE(GetThread)
        movl    %eax, %ebx
        popl    %eax

        // Is mutex already owned by current thread?
        cmpl    %ebx, AwareLock_m_HoldingThread(ARGUMENT_REG1)
        jne     MonEnterStaticPrepareToWait

        // Yes, bump our use count.
        incl    AwareLock_m_Recursion(ARGUMENT_REG1)
#if defined(MON_DEBUG) && defined(TRACK_SYNC)
        pushl   ARGUMENT_REG1   // AwareLock
        pushl   4(%esp)         // return address
        call    STDMANGLE(EnterSyncHelper,8)
#endif // defined(MON_DEBUG) && defined(TRACK_SYNC)
        popl    %ebx
        ret

MonEnterStaticPrepareToWait: 
        popl    %ebx

        // ARGUMENT_REG1 should have AwareLock. Call contention helper.
        jmp     STDMANGLE(JITutil_MonContention,12)

MonEnterStaticRetryHelper: 
        jmp     MonEnterStaticRetry
ASMFUNCEND()

ASMFUNC(STDMANGLE(JIT_MonExitStatic,12))
#if defined(MON_DEBUG) && defined(TRACK_SYNC)
        pushl   ARGUMENT_REG1   // preserve regs

        pushl   ARGUMENT_REG1   // AwareLock
        pushl   8(%esp)         // return address
        call    STDMANGLE(LeaveSyncHelper,8)

        popl    ARGUMENT_REG1   // restore regs
#endif // defined(MON_DEBUG) && defined(TRACK_SYNC)

        // Check if lock is held.
        call    *CMANGLE(GetThread)
        cmpl    %eax, AwareLock_m_HoldingThread(ARGUMENT_REG1)
        jne     MonExitStaticLockError

        // Reduce our recursion count.
        decl    AwareLock_m_Recursion(ARGUMENT_REG1)
        jz      MonExitStaticLastRecursion

        ret

        // This is the last count we held on this lock, so release the lock.
MonExitStaticLastRecursion: 
        // eax must have the thread object
        decl    Thread_m_dwLockCount(%eax)
        movl    $0, AwareLock_m_HoldingThread(ARGUMENT_REG1)
        pushl   %ebx

MonExitStaticRetry: 
        movl    AwareLock_m_MonitorHeld(ARGUMENT_REG1), %eax
        leal    -1(%eax), %ebx
        nop
        cmpxchgl    %ebx, AwareLock_m_MonitorHeld(ARGUMENT_REG1)
        jne     MonExitStaticRetryHelper
        popl    %ebx
        testl   $0xFFFFFFFE, %eax
        jne     MonExitStaticMustSignal

        ret

MonExitStaticMustSignal: 
        jmp     STDMANGLE(JITutil_MonSignal,12)

MonExitStaticRetryHelper: 
        jmp     MonExitStaticRetry
        // Throw a synchronization lock exception.
MonExitStaticLockError: 
        movl    IMM(CORINFO_SynchronizationLockException_ASM), ARGUMENT_REG1
        jmp     STDMANGLE(JIT_InternalThrow,12)
ASMFUNCEND()

ASMFUNC(STDMANGLE(JIT_MonLast,0))
        ret
ASMFUNCEND()
