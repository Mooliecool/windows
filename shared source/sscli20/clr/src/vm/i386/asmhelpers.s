// ==++==
//
//  
//   Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//  
//   The use and distribution terms for this software are contained in the file
//   named license.txt, which can be found in the root of this distribution.
//   By using this software in any fashion, you are agreeing to be bound by the
//   terms of this license.
//  
//   You must not remove this notice, or any other, from this software.
//  
//
// ==--==
//
//  *** NOTE:  If you make changes to this file, propagate the changes to
//             asmhelpers.asm in this directory
//
//  This file uses AT&T i386 syntax. Search web for "AT&T Syntax versus Intel Syntax"
//  to get document describing differences between the AT&T i386 syntax and the Intel
//  syntax used by masm.

#include "asmconstants.h"

.text

#ifdef PLATFORM_UNIX
#define STDMANGLE(name,args) name
#define CMANGLE(name) name
#else
#define STDMANGLE(name,args) _##name##@##args
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

#ifdef _DEBUG

// ensure that a register contains a 4-byte-aligned pointer.  If it doesn't,
// then execute a hard-coded int3 to report the assertion failure.
#define _ASSERT_ALIGNED_4_X86(reg) \
        test $3, %##reg         ; \
        jz .+3                  ; \
        int $3                  ;

// ensure that a register contains an 8-byte-aligned pointer.  If it doesn't,
// then execute a hard-coded int3 to report the assertion failure.
#define _ASSERT_ALIGNED_8_X86(reg) \
        test $7, %##reg         ; \
        jz .+3                  ; \
        int $3                  ;

#else
#define _ASSERT_ALIGNED_4_X86(reg)
#define _ASSERT_ALIGNED_8_X86(reg)
#endif

// void [__cdecl|__stdcall] ResetCurrentContext(void);
//
// Restores the floating-point hardware to a known state, leaving
// precision and rounding unmodified.
//
ASMFUNC(STDMANGLE(ResetCurrentContext,0))
        push %ebp
        mov  %esp, %ebp
        sub  $4, %esp
        #define ctrlWord 0xfffffffc(%ebp)

        // Clear the direction flag (used for rep instructions)
        cld

        fnstcw ctrlWord
        fninit                  // reset FPU
        andw $0x0f00, ctrlWord  // preserve precision and rounding control
        orw  $0x007f, ctrlWord  // mask all exceptions
        fldcw ctrlWord          // preserve precision control

        leave
        ret
        #undef ctrlWord
ASMFUNCEND()

/*
; This is a helper that we use to raise the correct managed exception with
; the necessary frame (after taking a fault in jitted code).
;
; Inputs:
;      all registers still have the value
;      they had at the time of the fault, except
;              EIP points to this function
;              ECX contains the original EIP
;
; What it does:
;      The exception to be thrown is stored in m_pLastThrownObjectHandle.
;      We push a FaultingExcepitonFrame on the stack, and then we call
;      complus throw.
;
*/
ASMFUNC(STDMANGLE(NakedThrowHelper,0))
        // Erect a faulting Method Frame.  Layout is as follows ...
        mov %esp, %edx
        push %ebp               // ebp
        push %ebx               // ebx
        push %esi               // esi
        push %edi               // edi
        push %edx               // original esp
        push %ecx               // m_ReturnAddress (i.e. original IP)
        sub  $12,%esp           // m_dummy (trash)
                                // m_next (filled in by LinkFrameAndThrow)
                                // FaultingExceptionFrame VFP (ditto)

        mov %esp, %ecx
        push CMANGLE(s_gsCookie)// GSCookie
        push %ecx               // push cdecl argument
        call STDMANGLE(LinkFrameAndThrow,4)
        pop  %eax               // clean up after cdecl call

        pop %ebp
        ret $4
ASMFUNCEND()


// void ResumeAtJitEHHelper(CONTEXT *pContext)
//
// Restores the CPU registers from the CONTEXT record, including
// EIP.
//
ASMFUNC(STDMANGLE(ResumeAtJitEHHelper,4))
        mov     4(%esp),%edx        // edx = pContext (EHContext*)

        mov     EHContext_Eax(%edx), %eax
        mov     EHContext_Ebx(%edx), %ebx
        mov     EHContext_Esi(%edx), %esi
        mov     EHContext_Edi(%edx), %edi
        mov     EHContext_Ebp(%edx), %ebp
        mov     EHContext_Esp(%edx), %esp

        jmp     *EHContext_Eip(%edx)
ASMFUNCEND()

// int __stdcall CallJitEHFilterHelper(size_t *pShadowSP, EHContext *pContext);
//   On entry, only the pContext->Esp, Ebx, Esi, Edi, Ebp, and Eip are
//   initialized.
//
ASMFUNC(STDMANGLE(CallJitEHFilterHelper,8))
        push    %ebp
        mov     %esp, %ebp
        push    %ebx
        push    %esi
        push    %edi

        #define pShadowSP 8(%ebp)
        #define pContext  12(%ebp)

        mov     pShadowSP, %eax     // Write esp-4 to the shadowSP slot
        test    %eax, %eax
        jz      DONE_SHADOWSP_FILTER
        mov     %esp, %ebx
        sub     $4, %ebx
        or      IMM(SHADOW_SP_IN_FILTER_ASM), %ebx
        mov     %ebx, (%eax)
    DONE_SHADOWSP_FILTER:

        mov     pContext, %edx
        mov     EHContext_Eax(%edx), %eax
        mov     EHContext_Ebx(%edx), %ebx
        mov     EHContext_Esi(%edx), %esi
        mov     EHContext_Edi(%edx), %edi
        mov     EHContext_Ebp(%edx), %ebp

        call    *EHContext_Eip(%edx)
#ifdef _DEBUG
        nop  // Indicate that it is OK to call managed code directly from here
#endif
        pop     %edi
        pop     %esi
        pop     %ebx
        pop     %ebp // don't use 'leave' here, as ebp as been trashed
        ret     $8
        #undef pShadowSP
        #undef pContext
ASMFUNCEND()

// void __stdcall CallJitEHFinallyHelper(size_t *pShadowSP, EHContext *pContext);
//   on entry, only the pContext->Esp, Ebx, Esi, Edi, Ebp, and Eip are initialized
ASMFUNC(STDMANGLE(CallJitEHFinallyHelper,8))
        push    %ebp
        mov     %esp, %ebp
        push    %ebx
        push    %esi
        push    %edi

        #define pShadowSP 8(%ebp)
        #define pContext  12(%ebp)

        mov     pShadowSP, %eax     // Write esp-4 to the shadowSP slot
        test    %eax, %eax
        jz      DONE_SHADOWSP_FINALLY
        mov     %esp, %ebx
        sub     $4, %ebx
        mov     %ebx, (%eax)
    DONE_SHADOWSP_FINALLY:

        mov     pContext, %edx
        mov     EHContext_Eax(%edx), %eax
        mov     EHContext_Ebx(%edx), %ebx
        mov     EHContext_Esi(%edx), %esi
        mov     EHContext_Edi(%edx), %edi
        mov     EHContext_Ebp(%edx), %ebp
        call    *EHContext_Eip(%edx)
#ifdef _DEBUG
        nop  // Indicate that it is OK to call managed code directly from here
#endif
        pop     %edi
        pop     %esi
        pop     %ebx
        pop     %ebp // don't use 'leave' here, as ebp as been trashed
        ret     $8
        #undef pShadowSP
        #undef pContext
ASMFUNCEND()


// DWORD [__cdecl||__stdcall] GetSpecificCpuTypeAsm(void);
//
// Determines the type of x86 processor
//
ASMFUNC(STDMANGLE(GetSpecificCpuTypeAsm,0))
        push    %ebx        // ebx is trashed by the cpuid calls

        // See if the chip supports CPUID
        pushf
        pop     %ecx        // Get the EFLAGS
        mov     %ecx, %eax  // Save for later testing
        xor     $0x200000, %ecx // Invert the ID bit.
        push    %ecx
        popf                // Save the updated flags.
        pushf
        pop     %ecx        // Retrive the updated flags
        xor     %eax, %ecx  // Test if it actually changed (bit set means yes)
        push    %eax
        popf                // Restore the flags

        test    $0x200000, %ecx
        jz      Assume486

        xor     %eax, %eax
        cpuid

        test    %eax, %eax
        jz      Assume486   // brif CPUID1 not allowed

        mov     $1, %eax
        cpuid

        // filter out everything except family and model
        // Note that some multi-procs have different stepping number for each proc
        and     $0x0ff0, %eax

        jmp     CpuTypeDone

Assume486:
        mov     $0x0400, %eax   // report 486
CpuTypeDone:
        pop     %ebx
        ret
ASMFUNCEND()

//
// Determines the CPU features supported by the processor
//
// DWORD __stdcall GetSpecificCpuFeaturesAsm(DWORD *pInfo);
ASMFUNC(STDMANGLE(GetSpecificCpuFeaturesAsm,4))
        push    %ebx            // ebx is trashed by the cpuid calls

        // See if the chip supports CPUID
        pushf
        pop     %ecx            // Get the EFLAGS
        mov     %ecx, %eax      // Save for later testing
        xor     $0x200000, %ecx // Invert the ID bit.
        push    %ecx
        popf                    // Save the updated flags.
        pushf
        pop     %ecx            // Retrive the updated flags
        xor     %eax, %ecx      // Test if it actually changed (bit set means yes)
        push    %eax
        popf                    // Restore the flags

        test    $0x200000, %ecx
        jz      CpuFeaturesFail

        xor     %eax, %eax
        cpuid

        test    %eax, %eax
        jz      CpuFeaturesDone // brif CPUID1 not allowed

        mov     $1, %eax
        cpuid
        mov     %edx, %eax      // return all feature flags
        mov     8(%esp), %edx
        test    %edx, %edx
        jz      CpuFeaturesDone
        mov     %ebx, (%edx)    // return additional useful information
        jmp     CpuFeaturesDone

CpuFeaturesFail:
        xor     %eax, %eax    // Nothing to report
CpuFeaturesDone:
        pop     %ebx
        ret     $4
ASMFUNCEND()

// Atomic bit manipulations, with and without the lock prefix.  We initialize
// all consumers to go through the appropriate service at startup.

// void __stdcall OrMaskUP(DWORD volatile *p, const int msk)
ASMFUNC(STDMANGLE(OrMaskUP,8))
        mov     4(%esp), %ecx   // p
        mov     8(%esp), %edx   // msk
        _ASSERT_ALIGNED_4_X86(ecx)
        or      %edx, (%ecx)
        ret     $8
ASMFUNCEND()

// void __stdcall AndMaskUP(DWORD volatile *p, const int msk)
ASMFUNC(STDMANGLE(AndMaskUP,8))
        mov     4(%esp), %ecx   // p
        mov     8(%esp), %edx   // msk
        _ASSERT_ALIGNED_4_X86(ecx)
        and     %edx, (%ecx)
        ret     $8
ASMFUNCEND()

// LONG __stdcall ExchangeUP(LONG volatile *Target, LONG Value)
ASMFUNC(STDMANGLE(ExchangeUP,8))
        mov     4(%esp), %ecx   // Target
        mov     8(%esp), %edx   // Value
        mov     (%ecx), %eax    // attempted comparant
        _ASSERT_ALIGNED_4_X86(ecx)
LRetryExchange:
        cmpxchg %edx, (%ecx)
        jne     LRetryExchange1
        ret     $8
LRetryExchange1:
        jmp     LRetryExchange
ASMFUNCEND()


// LONG __stdcall ExchangeAddUP(LONG volatile *Target, LONG value)
ASMFUNC(STDMANGLE(ExchangeAddUP,8))
        mov     4(%esp), %ecx   // Target
        mov     8(%esp), %eax   // Value
        _ASSERT_ALIGNED_4_X86(ecx)
        xadd    %eax, (%ecx)    // Add Value to Target
        ret     $8              // result in EAX
ASMFUNCEND()


// void *__stdcall CompareExchangeUP(PVOID volatile *Destination,
//                                  PVOID Exchange, PVOID Comparand)
ASMFUNC(STDMANGLE(CompareExchangeUP,12))
        mov     4(%esp), %ecx   // Destination
        mov     8(%esp), %edx   // Exchange
        mov     12(%esp), %eax  // Comparand
        _ASSERT_ALIGNED_4_X86(ecx)
        cmpxchg %edx, (%ecx)
        ret     $12             // result in EAX
ASMFUNCEND()


// LONG __stdcall IncrementUP(LONG volatile *Target)
ASMFUNC(STDMANGLE(IncrementUP,4))
        mov     4(%esp), %ecx
        mov     $1, %eax
        _ASSERT_ALIGNED_4_X86(ecx)
        xadd    %eax, (%ecx)
        inc     %eax            // return prior value, plus 1 we added
        ret     $4
ASMFUNCEND()


// LONG __stdcall DecrementUP(LONG volatile *Target)
ASMFUNC(STDMANGLE(DecrementUP,4))
        mov     4(%esp), %ecx
        _ASSERT_ALIGNED_4_X86(ecx)
        mov     $-1, %eax
        xadd    %eax, (%ecx)
        dec     %eax            // return prior value, less 1 we removed
        ret     $4
ASMFUNCEND()

// void __stdcall OrMaskMP(DWORD volatile *p, const int msk)
ASMFUNC(STDMANGLE(OrMaskMP,8))
        mov     4(%esp), %ecx   // p
        mov     8(%esp), %edx   // msk
        _ASSERT_ALIGNED_4_X86(ecx)
  lock; or      %edx, (%ecx)
        ret     $8
ASMFUNCEND()

// void __stdcall AndMaskMP(DWORD volatile *p, const int msk)
ASMFUNC(STDMANGLE(AndMaskMP,8))
        mov     4(%esp), %ecx   // p
        mov     8(%esp), %edx   // msk
        _ASSERT_ALIGNED_4_X86(ecx)
  lock; and     %edx, (%ecx)
        ret     $8
ASMFUNCEND()

// LONG __stdcall ExchangeMP(LONG volatile *Target, LONG Value)
ASMFUNC(STDMANGLE(ExchangeMP,8))
        mov     4(%esp), %ecx   // Target
        mov     8(%esp), %edx   // Value
        mov     (%ecx), %eax    // attempted comparant
        _ASSERT_ALIGNED_4_X86(ecx)
LRetryExchangeMP:
  lock; cmpxchg %edx, (%ecx)
        jne     LRetryExchangeMP1
        ret     $8
LRetryExchangeMP1:
        jmp     LRetryExchangeMP
ASMFUNCEND()

// INT64 __stdcall ExchangeLongMP8b(INT64 volatile *Target, INT64 value)
ASMFUNC(STDMANGLE(ExchangeLongMP8b,12))

        mov     4(%esp), %ecx   // Target

        push    %esi
        push    %ebx
        mov     %ecx, %esi
        mov     16(%esp), %ebx      //EBX:ECX is the value to switch if equal
        mov     20(%esp), %ecx

LelMP8bPre:
        mov     (%esi), %eax        //EAX:EDX is the value to compare with
        mov     4(%esi), %edx

  lock; cmpxchg8b (%esi)
        jnz     LelMP8bPre          //Swap was unsuccessful try again

        pop     %ebx
        pop     %esi

        ret     $12                 // result in EAX:EDX
ASMFUNCEND()

// void * __stdcall CompareExchangeMP(PVOID volatile *Destination,
//                                    PVOID Exchange, PVOID Comparand)
ASMFUNC(STDMANGLE(CompareExchangeMP,12))
        mov     4(%esp), %ecx   // Destintation
        mov     8(%esp), %edx   // Exchange
        mov     12(%esp), %eax  // Comparand
        _ASSERT_ALIGNED_4_X86(ecx)
  lock; cmpxchg %edx, (%ecx)
        ret     $12             // result in EAX
ASMFUNCEND()

// INT64 __stdcall CompareExchangeLongMP8b(INT64 volatile *Destination,
//                                  INT64 Exchange, INT64 Comparand)
ASMFUNC(STDMANGLE(CompareExchangeLongMP8b,20))

        mov     4(%esp), %ecx   // Destination

        _ASSERT_ALIGNED_4_X86(ecx)
        push    %ebx
        push    %esi
        mov     %ecx, %esi
        mov     24(%esp), %edx
        mov     28(%esp), %eax
        mov     16(%esp), %ebx
        mov     20(%esp), %ecx

  lock; cmpxchg8b (%esi)

        pop     %esi
        pop     %ebx
        ret      $20               // result in EAX:EDX
ASMFUNCEND()

// LONG __stdcall ExchangeAddMP(LONG volatile *Target, LONG value)
ASMFUNC(STDMANGLE(ExchangeAddMP,8))
        mov     4(%esp), %ecx   // Target
        mov     8(%esp), %eax   // Value
        _ASSERT_ALIGNED_4_X86(ecx)
  lock; xadd    %eax, (%ecx)    // Add Value to Target
        ret     $8              // result in EAX
ASMFUNCEND()

// INT64 __stdcall ExchangeAddLongMP8b(INT64 volatile *Destination,
//                                  INT64 Exchange, INT64 Comparand)
ASMFUNC(STDMANGLE(ExchangeAddLongMP8b,12))

        mov     4(%esp), %ecx   // Target

        _ASSERT_ALIGNED_4_X86(ecx)

        push    %esi
        push    %ebx
        mov     %ecx, %esi

LealMP8bPre:
        mov     (%esi), %eax        //load the previous value
        mov     4(%esi), %edx

        mov     16(%esp), %ebx      //EBX:ECX is the value to add in
        mov     20(%esp), %ecx

        add     (%esi), %ebx
        adc     4(%esi), %ecx

  lock; cmpxchg8b (%esi)
        jnz     LealMP8bPre         //Someone changed the dest, Swap was unsuccessful try again

        pop     %ebx
        pop     %esi

        ret      $12
ASMFUNCEND()

// LONG __stdcall IncrementMP(LONG volatile *Target)
ASMFUNC(STDMANGLE(IncrementMP,4))
        mov     4(%esp), %ecx
        mov     $1, %eax
        _ASSERT_ALIGNED_4_X86(ecx)
  lock; xadd    %eax, (%ecx)
        inc     %eax            // return prior value, plus 1 we added
        ret     $4
ASMFUNCEND()

// UINT64 __stdcall IncrementLongMP8b(UINT64 volatile *Target)
ASMFUNC(STDMANGLE(IncrementLongMP8b,4))
        push    %esi
        push    %ebx
        mov     12(%esp), %esi  // Target


        _ASSERT_ALIGNED_4_X86(esi)

        xor     %edx, %edx
        xor     %eax, %eax
        xor     %ecx, %ecx
        mov     $1, %ebx

  lock; cmpxchg8b (%esi)
        jz LDoneInc8b

LPreemptedInc8b:
        mov     %edx, %ecx
        mov     %eax, %ebx
        add     $1, %ebx
        adc     $0, %ecx

  lock; cmpxchg8b (%esi)
        jnz     LPreemptedInc8b

LDoneInc8b:
        mov     %ecx, %edx
        mov     %ebx, %eax

        pop     %ebx
        pop     %esi
        ret     $4
ASMFUNCEND()

// LONG __stdcall DecrementMP(LONG volatile *Target)
ASMFUNC(STDMANGLE(DecrementMP,4))
        mov     4(%esp), %ecx
        _ASSERT_ALIGNED_4_X86(ecx)
        mov     $-1, %eax
  lock; xadd    %eax, (%ecx)
        dec     %eax            // return prior value, less 1 we removed
        ret     $4
ASMFUNCEND()

// UINT64 __stdcall DecrementLongMP8b(UINT64 volatile *Target)
ASMFUNC(STDMANGLE(DecrementLongMP8b,4))
        push    %esi
        push    %ebx

        mov     12(%esp), %esi  // Target

        _ASSERT_ALIGNED_4_X86(esi)

        xor     %ebx, %ebx
        xor     %eax, %eax
        mov     $-1, %ecx
        mov     %ecx, %ebx

  lock; cmpxchg8b (%esi)
        jz      LDoneDec8b

LPreemptedDec8b:
        mov     %edx, %ecx
        mov     %eax, %ebx
        sub     $1, %ebx
        sbb     $0, %ecx

  lock; cmpxchg8b (%esi)
        jnz     LPreemptedDec8b

LDoneDec8b:
        mov     %ecx, %edx
        mov     %ebx, %eax

        pop     %ebx
        pop     %esi
        ret     $4
ASMFUNCEND()

#ifdef _DEBUG
// void Frame::CheckExitFrameDebuggerCalls(void)
// NOTE: this may be called with cdecl, or stdcall
ASMFUNC(STDMANGLE(CheckExitFrameDebuggerCalls,0))
        call    STDMANGLE(PerformExitFrameChecks,0)
        jmp     *%eax
ASMFUNCEND()
#endif


//-----------------------------------------------------------------------
// The out-of-line portion of the code to enable preemptive GC.
// After the work is done, the code jumps back to the "pRejoinPoint"
// which should be emitted right after the inline part is generated.
//
// Assumptions:
//      ebx = Thread
// Preserves
//      all registers except ecx.
//
//-----------------------------------------------------------------------
ASMFUNC(CMANGLE(StubRareEnable))
        push    %eax
        push    %edx

        push    %ebx
        call    STDMANGLE(StubRareEnableWorker,4)

        pop     %edx
        pop     %eax
        ret
ASMFUNCEND()

ASMFUNC(CMANGLE(StubRareDisableHR))
        push    %edx

        push    %esi    // Frame
        push    %ebx    // Thread
        call    STDMANGLE(StubRareDisableHRWorker,8)

        pop     %edx
        ret
ASMFUNCEND()

ASMFUNC(CMANGLE(StubRareDisableTHROW))
        push    %eax
        push    %edx

        push    %esi    // Frame
        push    %ebx    // Thread
        call    STDMANGLE(StubRareDisableTHROWWorker,8)

        pop     %edx
        pop     %eax
        ret
ASMFUNCEND()

ASMFUNC(CMANGLE(StubRareDisableRETURN))
        push    %eax
        push    %edx

        push    %esi    // Frame
        push    %ebx    // Thread
        call    STDMANGLE(StubRareDisableRETURNWorker,8)

        pop     %edx
        pop     %eax
        ret
ASMFUNCEND()

// EAX -> number of caller args bytes on the stack that we must remove before going
// to the throw helper, which assumes the stack is clean.
// ECX -> exception number to throw
ASMFUNC(CMANGLE(InternalExceptionWorker))
    pop     %edx        // recover RETADDR
    add     %eax, %esp  // release caller's args
    push    %edx        // restore RETADDR
    jmp     FMANGLE(JIT_InternalThrow,12)
ASMFUNCEND()

// EAX -> number of caller arg bytes on the stack that we must remove before going
// to the throw helper, which assumes the stack is clean.
ASMFUNC(CMANGLE(ArrayOpStubNullException))
// kFactorReg and kTotalReg could not have been modified, but let's pop
// them anyway for consistency and to avoid future bugs
    pop     %esi
    pop     %edi
    mov     IMM(CORINFO_NullReferenceException_ASM), %ecx
    jmp     CMANGLE(InternalExceptionWorker)
ASMFUNCEND()

// EAX -> number of caller arg bytes on the stack that we must remove before going
// to the throw helper, which assumes the stack is clean.
ASMFUNC(CMANGLE(ArrayOpStubRangeException))
// kFactorReg and kTotalReg could not have been modified, but let's pop
// them anyway for consistency and to avoid future bugs
    pop     %esi
    pop     %edi
    mov     IMM(CORINFO_IndexOutOfRangeException_ASM), %ecx
    jmp     CMANGLE(InternalExceptionWorker)
ASMFUNCEND()

// EAX -> number of caller arg bytes on the stack that we must remove before going
// to the throw helper, which assumes the stack is clean.
ASMFUNC(CMANGLE(ArrayOpStubTypeMismatchException))
// kFactorReg and kTotalReg could not have been modified, but let's pop
// them anyway for consistency and to avoid future bugs
    pop     %esi
    pop     %edi
    mov     IMM(CORINFO_ArrayTypeMismatchException_ASM), %ecx
    jmp     CMANGLE(InternalExceptionWorker)
ASMFUNCEND()

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
ASMFUNC(STDMANGLE(CallDescrWorkerInternal,20))
#else
ASMFUNC(STDMANGLE(CallDescrWorker,20))
#endif
        push    %ebp
        mov     %esp, %ebp
        #define pSrcEnd         8(%ebp)
        #define numStackSlots   12(%ebp)
        #define pArgumentRegisters 16(%ebp)
        #define fpRetSize       20(%ebp)
        #define pTarget         24(%ebp)

        mov     pSrcEnd, %eax           // copy the stack
        mov     numStackSlots, %ecx
        test    %ecx, %ecx
        jz      donestack
        sub     $4, %eax
        push    (%eax)
        dec     %ecx
        jz      donestack
        sub     $4, %eax
        push    (%eax)
        dec     %ecx
        jz      donestack
stackloop:
        sub     $4, %eax
        push    (%eax)
        dec     %ecx
        jnz     stackloop
donestack:

        // now we must push each field of the ArgumentRegister structure
        mov     pArgumentRegisters, %eax
        mov     (%eax), %edx
        mov     4(%eax), %ecx

        call    *pTarget
#ifdef _DEBUG
        nop     // This is a tag that we use in an assert.  Fcalls expect to
                // be called from Jitted code or from certain blessed call sites like
                // this one.  (See HelperMethodFrame::InsureInit)
#endif

        // Save FP return value
        mov     fpRetSize, %ecx
        cmp     $4, %ecx
        je      ReturnsFloat
        cmp     $8, %ecx
        je      ReturnsDouble
        leave
        ret     $20

ReturnsFloat:
        fstps   fpRetSize
        mov     fpRetSize, %eax
        leave
        ret     $20

ReturnsDouble:
        fstpl   fpRetSize
        mov     fpRetSize, %eax
        mov     pTarget, %edx
        leave
        ret     $20

        #undef pSrcEnd
        #undef numStackSlots
        #undef pArgumentRegisters
        #undef fpRetSize
        #undef pTarget
ASMFUNCEND()

#ifdef _DEBUG
// int __fastcall HelperMethodFrameRestoreState(HelperMethodFrame*, struct MachState *)
ASMFUNC(STDMANGLE(HelperMethodFrameRestoreState,8))
    mov         8(%esp), %eax   // MachState*
#else
// int __fastcall HelperMethodFrameRestoreState(struct MachState *)
ASMFUNC(STDMANGLE(HelperMethodFrameRestoreState,4))
    mov         4(%esp), %eax   // MachState*
#endif
    // restore the registers from the m_MachState stucture.  Note that
    // we only do this for register that where not saved on the stack
    // at the time the machine state snapshot was taken.

    cmp         $0, MachState__pRetAddr(%eax)

#ifndef DACCESS_COMPILE
#ifdef _DEBUG
    jnz         noConfirm
    mov         4(%esp),%ecx    // HelperMethodFrame*
    push        %ebp
    push        %ebx
    push        %edi
    push        %esi
    push        %ecx            // HelperFrame*
    call        STDMANGLE(HelperMethodFrameConfirmState,20)
    // on return, eax = MachState*
    cmp         $0, MachState__pRetAddr(%eax)
noConfirm:
#endif
#endif

    jz          doRet

    lea         MachState__esi(%eax), %edx      // Did we have to spill ESI
    cmp         %edx, MachState__pEsi(%eax)
    jnz         SkipESI
    mov         (%edx), %esi                    // Then restore it
SkipESI:

    lea         MachState__edi(%eax), %edx      // Did we have to spill EDI
    cmp         %edx, MachState__pEdi(%eax)
    jnz         SkipEDI
    mov         (%edx), %edi                    // Then restore it
SkipEDI:

    lea         MachState__ebx(%eax), %edx      // Did we have to spill EBX
    cmp         %edx, MachState__pEbx(%eax)
    jnz         SkipEBX
    mov         (%edx), %ebx                    // Then restore it
SkipEBX:

    lea         MachState__ebp(%eax), %edx      // Did we have to spill EBP
    cmp         %edx, MachState__pEbp(%eax)
    jnz         SkipEBP
    mov         (%edx), %ebp                    // Then restore it
SkipEBP:

doRet:
    xor         %eax, %eax
#ifdef _DEBUG
    ret         $8
#else
    ret         $4
#endif
ASMFUNCEND()


//---------------------------------------------------------------------------
// Portable GetThread() function: used if no platform-specific optimizations apply.
// This is in assembly code because we count on edx not getting trashed on calls
// to this function.
//---------------------------------------------------------------------------
// Thread* __stdcall GetThreadGeneric(void);
ASMFUNC(STDMANGLE(GetThreadGeneric,0))
    push        %ebp
    mov         %esp, %ebp
    push        %ecx
    push        %edx

#ifdef _DEBUG
    // _ASSERTE(ThreadInited());
    cmp         $-1, CMANGLE(gThreadTLSIndex)
    jnz         ThreadInited
    int         $3
ThreadInited:
#endif

    push        CMANGLE(gThreadTLSIndex)
    call        STDMANGLE(TlsGetValue,4)

    pop         %edx
    pop         %ecx
    leave
    ret
ASMFUNCEND()

//---------------------------------------------------------------------------
// Portable GetAppDomain() function: used if no platform-specific optimizations apply.
// This is in assembly code because we count on edx not getting trashed on calls
// to this function.
//---------------------------------------------------------------------------
// Appdomain* __stdcall GetAppDomainGeneric(void);
ASMFUNC(STDMANGLE(GetAppDomainGeneric,0))
    push        %ebp
    mov         %esp, %ebp
    push        %ecx
    push        %edx

#ifdef _DEBUG
    cmp         $-1, CMANGLE(gAppDomainTLSIndex)
    jnz         AppDomainInited
    int         $3
AppDomainInited:
#endif

    push        CMANGLE(gAppDomainTLSIndex)
    call        STDMANGLE(TlsGetValue,4)

    pop         %edx
    pop         %ecx
    leave
    ret
ASMFUNCEND()




// Note that the debugger skips this entirely when doing SetIP,
// since COMPlusCheckForAbort should always return 0.  Excep.cpp:LeaveCatch
// asserts that to be true.  If this ends up doing more work, then the
// debugger may need additional support.
// void __stdcall JIT_EndCatch()//
ASMFUNC(STDMANGLE(JIT_EndCatch,0))
    xor     %eax, %eax
    push    %eax                // pSEH
    push    %eax                // pCtx
    push    %eax                // pCurThread
    call    STDMANGLE(COMPlusEndCatch,12)       // returns old esp value in eax

    mov     (%esp), %ecx        // actual return address into jitted code
    mov     %eax, %edx          // old esp value
    push    %eax                // save old esp
    push    %ebp

    push    %edx
    push    %ecx
    call    STDMANGLE(COMPlusCheckForAbort,12)  // __stdcall, returns old esp value

    pop     %ecx
    // at this point, ecx   = old esp value
    //                [esp] = return address into jitted code
    //                eax   = 0 (if no abort), address to jump to otherwise
    test    %eax, %eax
    jz      NormalEndCatch
    mov     %ecx, %esp
    jmp     *%eax

NormalEndCatch:
    pop     %eax                // Move the returnAddress into ecx
    mov     %ecx, %esp          // Reset esp to the old value
    jmp     *%eax               // Resume after the "endcatch"
ASMFUNCEND()


//---------------------------------------------------------
// Performs an N/Direct call. This is a generic version
// that can handly any N/Direct call but is not as fast
// as more specialized versions.
//---------------------------------------------------------
//
// INT64 __stdcall NDirectGenericStubWorker(Thread *pThread,
//                                          NDirectMethodFrame *pFrame);
//
ASMFUNC(STDMANGLE(NDirectGenericStubWorker,8))
    push    %ebp
    mov     %esp, %ebp
    sub     $24, %esp

    // Parameters
    #define pThread 8(%ebp)
    #define pFrame  12(%ebp)
    // Locals
    #define pHeader -4(%ebp)
    #define pvfn    -8(%ebp)
    #define pLocals -12(%ebp)
    #define pMLCode -16(%ebp)

    #define PreESP  -20(%ebp)
    #define PostESP -24(%ebp)
#if NDirectGenericWorkerFrameSize != 40
 #error asmconstants.h is incorrect
#endif

    lea     pHeader, %eax
    push    %eax         // &pHeader
    push    pFrame
    push    pThread
    call    STDMANGLE(NDirectGenericStubComputeFrameSize,12)
    // eax = cbAlloc (rounded up to next DWORD)

    add     $3, %eax     // _alloca
    and     $-4, %eax
    sub     %eax, %esp

    push    %esp        // pAlloc
    lea     pvfn, %eax
    push    %eax         // &pvfn
    lea     pLocals, %eax
    push    %eax         // &pLocals
    lea     pMLCode, %eax
    push    %eax         // &pMLCode
    push    pHeader
    push    pFrame
    push    pThread
    call    STDMANGLE(NDirectGenericStubBuildArguments,28)
    // pvfn is now the function to invoke, pLocals points to the locals, and
    // eax is zero or more MHLF_ constants.

    test    IMM(MLHF_THISCALL_ASM), %eax
    jz      doit
    test    IMM(MLHF_THISCALLHIDDENARG_ASM), %eax
    jz      regularthiscall

    // this call, with hidden arg
    pop     %eax        // pop the first argument, shrinking the alloca space
    pop     %ecx        // pop the second argument
    push    %eax
    jmp     doit

regularthiscall:
    pop     %ecx        // pop the first argument, shrinking the alloca space

doit:
    mov     %esp, PreESP
    call    *pvfn

// this label is used by the debugger, and must immediately follow the 'call'.
.globl STDMANGLE(NDirectGenericStubReturnFromCall,0)
STDMANGLE(NDirectGenericStubReturnFromCall,0):

    mov     %esp, PostESP

    push    PostESP
    push    PreESP
    push    %edx                // push high dword of the return value
    push    %eax                // push low  dword of the return value
    push    pLocals
    push    pMLCode
    push    pHeader
    push    pFrame
    push    pThread
    call    STDMANGLE(NDirectGenericStubPostCall,36)
    // return value is in edx:eax

    leave
    ret     $8
    #undef  pThread
    #undef  pFrame
    #undef  pHeader
    #undef  pvfn
    #undef  pLocals
    #undef  pMLCode
    #undef  PreESP
    #undef  PostESP
ASMFUNCEND()

ASMFUNC(STDMANGLE(DllImportForDelegateGenericStubWorker,8))

// ROTORTODO: Implement this

// this label is used by the debugger, and must immediately follow the 'call'.
.globl STDMANGLE(DllImportForDelegateGenericStubReturnFromCall,0)
STDMANGLE(DllImportForDelegateGenericStubReturnFromCall,0):

        int $3
ASMFUNCEND()

// LPVOID __stdcall CTPMethodTable__CallTargetHelper2(
//     const void *pTarget,
//     LPVOID pvFirst,
//     LPVOID pvSecond)
ASMFUNC(STDMANGLE(CTPMethodTable__CallTargetHelper2,12))
    push    %ebp
    mov     %esp, %ebp

    #define pTarget  8(%ebp)
    #define pvFirst  12(%ebp)
    #define pvSecond 16(%ebp)

    mov     pvFirst, %ecx
    mov     pvSecond, %edx

    call    *pTarget
#ifdef _DEBUG
    nop     // Mark this as a special call site that can directly
            // call managed code
#endif
    mov     %ebp, %esp
    pop     %ebp
    ret     $0xc
    #undef  pTarget
    #undef  pvFirst
    #undef  pvSecond
ASMFUNCEND()

// LPVOID __stdcall CTPMethodTable__CallTargetHelper3(
//     const void *pTarget,
//     LPVOID pvFirst,
//     LPVOID pvSecond,
//     LPVOID pvThird
ASMFUNC(STDMANGLE(CTPMethodTable__CallTargetHelper3,16))
    push    %ebp
    mov     %esp, %ebp

    #define pTarget  8(%ebp)
    #define pvFirst  12(%ebp)
    #define pvSecond 16(%ebp)
    #define pvThird  20(%ebp)

    push    pvThird

    mov     pvFirst, %ecx
    mov     pvSecond, %edx

    call    *pTarget
#ifdef _DEBUG
    nop     // Mark this as a special call site that can directly
            // call managed code
#endif
    mov     %ebp, %esp
    pop     %ebp
    ret     $0x10
    #undef  pTarget
    #undef  pvFirst
    #undef  pvSecond
    #undef  pvThird
ASMFUNCEND()

// void __stdcall setFPReturn(int fpSize, INT64 retVal)
//
// Load a floating-point return value into the FPU's top-of-stack,
// in preparation for a return from a function whose return type
// is float or double.  Note that fpSize may not be 4 or 8, meaning
// that the floating-point state must not be modified.
//
ASMFUNC(STDMANGLE(setFPReturn,12))
    mov     4(%esp), %eax
    cmp     $4, %eax
    jz      setFPReturn4

    cmp     $8, %eax
    jnz     setFPReturnNot8
    fldl    8(%esp)
setFPReturnNot8:
    ret     $12

setFPReturn4:
    flds    8(%esp)
    ret     $12
ASMFUNCEND()

// void __stdcall getFPReturn(int fpSize, INT64 *pretVal)
//
// Retrieve a floating-point return value from the FPU, after a
// call to a function whose return type is float or double.  Note
// that the fpSize may not be 4 or 8, meaning that the floating-
// point state and the contents of pretVal must not be modified.
//
ASMFUNC(STDMANGLE(getFPReturn,8))
   mov      4(%esp), %ecx
   mov      8(%esp), %eax
   cmp      $4, %ecx
   jz       getFPReturn4

   cmp      $8, %ecx
   jnz      getFPReturnNot8
   fstpl    (%eax)
getFPReturnNot8:
   ret      $8

getFPReturn4:
   fstps    (%eax)
   ret      $8
ASMFUNCEND()

// void *__stdcall UM2MThunk_WrapperHelper(void *pThunkArgs,
//                                         int argLen,
//                                         void *pAddr,
//                                         UMEntryThunk *pEntryThunk,
//                                         Thread *pThread)
//
ASMFUNC(STDMANGLE(UM2MThunk_WrapperHelper,20))
    push    %ebp
    mov     %esp, %ebp
    #define pThunkArgs 8(%ebp)
    #define argLen     12(%ebp)
    #define pAddr      16(%ebp)
    #define pEntryThunk 20(%ebp)
    #define pThread    24(%ebp)
    mov     argLen, %ecx
    test    %ecx, %ecx
    jz      UM2MThunk_ArgsComplete // brif no args

    // copy the args to the stack, they will be released when we return from
    // the thunk because we are coming from unmanaged to managed, any
    // objectref args must have already been pinned so don't have to worry
    // about them moving in the copy we make.
    _ASSERT_ALIGNED_4_X86(ecx)
    mov     pThunkArgs, %eax
UM2MThunk_CopyArg:
    push    -4(%eax,%ecx)       // push the argument on the stack
    sub     $4, %ecx            // move to the next argument
    jnz     UM2MThunk_CopyArg

UM2MThunk_ArgsComplete:
    mov     pEntryThunk, %eax
    mov     pThread, %ecx
    call    *pAddr

    // restore the stack after the call
    #undef  pThunkArgs
    #undef  argLen
    #undef  pAddr
    #undef  pEntryThunk
    #undef  pThread
    mov     %ebp, %esp
    pop     %ebp
    ret     $20
ASMFUNCEND()

// VOID __cdecl UMThunkStubRareDisable()
ASMFUNC(CMANGLE(UMThunkStubRareDisable))
    push    %eax
    push    %ecx

    push    %esi         // Push frame
    push    %eax         // Push the UMEntryThunk
    push    %ecx         // Push thread
    call    STDMANGLE(UMThunkStubRareDisableWorker,12) // __stdcall

    pop     %ecx
    pop     %eax
    ret
ASMFUNCEND()

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::CheckForContextMatch   public
//
//  Synopsis:   This code generates a check to see if the current context and
//              the context of the proxy match.
//+----------------------------------------------------------------------------
//
// returns zero if contexts match
// returns non-zero if contexts do not match
//
// UINT_PTR __stdcall CRemotingServices__CheckForContextMatch(Object* pStubData)
ASMFUNC(STDMANGLE(CRemotingServices__CheckForContextMatch,4))
    push    %ebx                // spill ebx
    mov     4(%eax), %ebx       // Get the internal context id by unboxing
                                // the stub data
    call    *CMANGLE(GetThread) // Get the current thread, assumes that the
                                // registers are preserved
    mov     Thread_m_Context(%eax), %eax // Get the current context from the
                                // thread
    sub     %ebx, %eax          // Get the pointer to the context from the
                                // proxy and compare with the current context
    pop     %ebx                // restore the value of ebx
    ret
ASMFUNCEND()

//+----------------------------------------------------------------------------
//
//  Method:     CRemotingServices::DispatchInterfaceCall   public
//
//  Synopsis:
//              Push that method desc on the stack and jump to the
//              transparent proxy stub to execute the call.
//              WARNING!! This MethodDesc is not the methoddesc in the vtable
//              of the object instead it is the methoddesc in the vtable of
//              the interface class. Since we use the MethodDesc only to probe
//              the stack via the signature of the method call we are safe.
//              If we want to get any object vtable/class specific
//              information this is not safe.
//
//+----------------------------------------------------------------------------
// void __stdcall CRemotingServices__DispatchInterfaceCall(MethodDesc *pMD)
ASMFUNC(STDMANGLE(CRemotingServices__DispatchInterfaceCall,4))
    // NOTE: At this point the stack looks like
    //
    // esp--->  return addr of stub
    //          saved MethodDesc of Interface method
    //          return addr of calling function
    //
    mov     TP_OFFSET_STUBDATA(%ecx), %eax
    call    *TP_OFFSET_STUB(%ecx)
#ifdef _DEBUG
    nop     // Mark this as a special call site that can directly
            // call managed code
#endif
    test    %eax, %eax
    jnz     CtxMismatch

    // in current context, so resolve MethodDesc to real slot no
    push    %ebx                                // spill ebx

    mov     8(%esp), %eax                       // eax <-- MethodDesc
    movzbl  MD_IndexOffset_ASM(%eax),%ebx       // get MethodTable from pMD
    neg     %ebx
    mov     MD_SkewOffset_ASM(%eax,%ebx,MethodDesc__ALIGNMENT), %eax
    mov     MethodTable_m_pEEClass(%eax), %eax  // get EEClass from pMT

    mov     EEClass_m_dwInterfaceId(%eax), %ebx // get the interface id from
                                                // the EEClass
    mov     TP_OFFSET_MT(%ecx), %eax            // get the *real* MethodTable

    mov     MethodTable_m_pEEClass(%eax), %eax  // get EEClass
    mov     EEClass_m_pInterfaceVTableMap(%eax), %eax // get interface map
    mov     (%eax,%ebx,4), %eax                 // offset map by interface id
    mov     8(%esp), %ebx                       // get MethodDesc
    movw    MethodDesc_m_wSlotNumber(%ebx), %bx
    and     $0xffff, %ebx
    mov     (%eax,%ebx,4), %eax                 // get jump addr

    pop     %ebx                                // restore ebx

    add     $0x8, %esp                          // pop off Method desc and
                                                // and stub's return addr
    jmp     *%eax

CtxMismatch:                                    // Jump to TPStub

    add     $0x4, %esp                          // pop ret addr of stub, so that the
                                                // stack and registers are now
                                                // setup exactly like they
                                                // were at the callsite

    jmp     *CMANGLE(g_dwOOContextAddr)         // jump to OOContext label in
                                                // TPStub
ASMFUNCEND()

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
ASMFUNC(STDMANGLE(CRemotingServices__CallFieldGetter,20))
    push    %ebp
    mov     %esp, %ebp
    #define pMD      8(%ebp)
    #define pThis    12(%ebp)
    #define pFirst   16(%ebp)
    #define pSecond  20(%ebp)
    #define pThird   24(%ebp)

    push    pSecond             // push the second argument on the stack
    push    pThird              // push the third argument on the stack

    mov     pThis, %ecx         // enregister pThis, the 'this' pointer
    mov     pFirst, %edx        // enregister pFirst, the first argument

    lea     retAddrFieldGetter, %eax
    push    %eax

    push    pMD                 // push the MethodDesc of object::__FieldGetter
    jmp     *CMANGLE(g_dwTPStubAddr) // jump to the TP stub

retAddrFieldGetter:
    #undef  pMD
    #undef  pThis
    #undef  pFirst
    #undef  pSecond
    #undef  pThird
    mov     %ebp, %esp
    pop     %ebp
    ret     $0x14
ASMFUNCEND()



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
ASMFUNC(STDMANGLE(CRemotingServices__CallFieldSetter,20))
    push    %ebp
    mov     %esp, %ebp
    #define pMD      8(%ebp)
    #define pThis    12(%ebp)
    #define pFirst   16(%ebp)
    #define pSecond  20(%ebp)
    #define pThird   24(%ebp)

    push    pSecond             // push the field name (second arg)
    push    pThird              // push the object (third arg) on the stack

    mov     pThis, %ecx         // enregister pThis, the 'this' pointer
    mov     pFirst, %edx        // enregister the first argument

    lea     retAddrFieldSetter, %eax // push the return address
    push    %eax

    push    pMD                 // push the MethodDesc of Object::__FieldSetter
    jmp     *CMANGLE(g_dwTPStubAddr) // jump to the TP Stub

retAddrFieldSetter:
    #undef  pMD
    #undef  pThis
    #undef  pFirst
    #undef  pSecond
    #undef  pThird
    mov     %ebp, %esp
    pop     %ebp
    ret     $0x14
ASMFUNCEND()


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
ASMFUNC(STDMANGLE(CTPMethodTable__GenericCheckForContextMatch,4))
    push    %ebp
    mov     %esp, %ebp
    #define tp    8(%ebp)

    push    %ecx
    mov     tp, %ecx
    mov     TP_OFFSET_STUBDATA(%ecx), %eax
    call    *TP_OFFSET_STUB(%ecx)
#ifdef _DEBUG
    nop     // Mark this as a special call site that can directly
            // call managed code
#endif
    test    %eax, %eax
    mov     $0, %eax
    setz    %al
    // NOTE: In the CheckForXXXMatch stubs (for URT ctx/ Ole32 ctx) eax is
    // non-zero if contexts *do not* match & zero if they do.
    pop     %ecx
    #undef  tp
    mov     %ebp, %esp
    pop     %ebp
    ret     $0x4
ASMFUNCEND()


// void __stdcall JIT_ProfilerEnterLeaveStub(UINT_PTR EEHandle, UINT_PTR ProfilerHandle, UINT_PTR FrameInfo, UINT_PTR ArgInfo)
ASMFUNC(STDMANGLE(JIT_ProfilerEnterLeaveStub,16))
    // this function must preserve all registers, including scratch
    ret     $16
ASMFUNCEND()

// void __stdcall JIT_ProfilerTailcallStub(UINT_PTR EEHandle, UINT_PTR ProfilerHandle, UINT_PTR FrameInfo)
ASMFUNC(STDMANGLE(JIT_ProfilerTailcallStub,12))
    // this function must preserve all registers, including scratch
    ret     $12
ASMFUNCEND()

//
// Used to get the current instruction pointer value
//
// LPVOID __stdcall GetCurrentIP(void);
ASMFUNC(STDMANGLE(GetCurrentIP,0))
    mov     (%esp), %eax
    ret
ASMFUNCEND()

// LPVOID __stdcall GetCurrentSP(void);
ASMFUNC(STDMANGLE(GetCurrentSP,0))
    mov     %esp, %eax
    ret
ASMFUNCEND()


// void __stdcall ProfileEnterNaked(FunctionID functionId,
//                                  void *clientData,
//                                  COR_PRF_FRAME_INFO *pFrame,
//                                  COR_PRF_ARGUMENT_INFO *pArgs);
ASMFUNC(STDMANGLE(ProfileEnterNaked,16))
    push    %esi
    push    %edi

    //
    // Push in reverse order the fields of ProfilePlatformSpecificData
    //
    push    8(%esp)         // EIP of the managed code that we return to.

    push    %esp            // Create some stack space for our parameter.
    push    %eax            // Save off EAX, because we need it to do some math.
    mov     %esp, %eax
    add     $44, %eax
    mov     %eax, 4(%esp)   // EAX is now the ESP of the managed code before it called to us.
    pop     %eax

    push    %ecx
    push    %edx
    push    %eax

    push    $0              // Create buffer space in the structure
    push    $0              // Create buffer space in the structure
    push    $0              // Create buffer space in the structure
    push    $0              // Create buffer space in the structure

    mov     48(%esp), %ecx  // The functionID that was pushed to FunctionEnter

    push    %ecx            // Push functionID into the structure.

    //
    // We are now done with the ProfilePlatformSpecificData structure and ready
    // to push the args to the _stdcall EE Enter method, which is of the following
    // declaratoin  ProfileEnter(FunctionID funcID, UINT_PTR clientData, PlatformSpecificData *pData)
    //

    push    %esp            // Push the address of the Platform structure.

    mov     60(%esp), %edx  // The clientData that was pushed to FunctionEnter

    // ecx contains the FuncId already from above.

    call    FMANGLE(ProfileEnter,16)

    add     $20, %esp       // Remove the buffer space
    pop     %eax
    pop     %edx
    pop     %ecx
    add     $8, %esp
    pop     %edi
    pop     %esi

    ret     $16
ASMFUNCEND()

// void __stdcall ProfileLeaveNaked(FunctionID functionId,
//                                  void *clientData,
//                                  COR_PRF_FRAME_INFO *pFrame,
//                                  COR_PRF_ARGUMENT_RANGE *pRetVal);
ASMFUNC(STDMANGLE(ProfileLeaveNaked,16))
    push    %ecx            // We do not strictly need to save ECX, however
                            // emitNoGChelper(CORINFO_HELP_PROF_FCN_LEAVE) returns true in the JITcompiler
    push    %edx            // Return value may be in EAX:EDX

    //
    // Push in reverse order the fields of ProfilePlatformSpecificData
    //
    push    8(%esp)         // EIP of the managed code that we return to.

    push    %esp            // Create some stack space for our parameter.
    push    %eax            // Save off EAX as return value may be in EAX:EDX
                            // and we need it to do some math.
    mov     %esp, %eax
    add     $32, %eax
    mov     %eax, 4(%esp)   // EAX is now the ESP of the managed code before it called to us.
    pop     %eax

    push    %ecx
    push    %edx
    push    %eax

                            // Save off any floating point registers we may need
    fstsw   %ax
    and     $0x3800, %ax    // Check the top-of-fp-stack bits
    cmp     $0, %ax         // If non-zero, we have something to save
    jnz     SaveFPReg

    push    $0              // else, fill the buffer space with zeros
    push    $0
    push    $0
    push    $0
    jmp     Continue

SaveFPReg:
    push    $1              // mark that a float value is present

    sub     $4, %esp        // Make room for the FP value
    fst     (%esp)          // Copy the FP value to the buffer as a float

    sub     $8, %esp        // Make room for the FP value
    fstp    (%esp)          // Copy the FP value to the buffer as a double

Continue:
    mov     48(%esp), %ecx

    push    %ecx            // Push functionID into the structure.

    //
    // Now call into the EE method for Leave, which is declared like this:
    // ProfileLeave(FunctionID funcID, UINT_PTR clientData, PlatformSpecificStructure *pData)
    //

    push    %esp            // The address of this structure.

    mov     60(%esp), %edx

    // ecx contains the function id already from above.

    call    FMANGLE(ProfileLeave,16)

    //
    // Now see if we have to restore and floating point registers
    //

    cmp     $0, 16(%esp)
    jz      NoRestore

    fld     4(%esp)

NoRestore:

    add     $20, %esp       // pop off buffer space
    pop     %eax
    add     $16, %esp
    pop     %edx
    pop     %ecx
    ret     $16
ASMFUNCEND()


// void __stdcall ProfileTailcallNaked(FunctionID functionId,
//                                     void *clientData,
//                                     COR_PRF_FRAME_INFO *pFrameInfo);
ASMFUNC(STDMANGLE(ProfileTailcallNaked,12))
    push    %ecx
    push    %edx

    //
    // Push in reverse order the fields of ProfilePlatformSpecificData
    //
    push    8(%esp)         // EIP of the managed code that we return to.

    push    %esp            // Create some stack space for our parameter.
    push    %eax            // Save off EAX, because we need it to do some math.
    mov     %esp, %eax
    add     $32, %eax
    mov     %eax, 4(%esp)   // EAX is now the ESP of the managed code before it called to us.
    pop     %eax

    push    %ecx
    push    %edx
    push    %eax

    push    $0              // Create buffer space in the structure
    push    $0              // Create buffer space in the structure
    push    $0              // Create buffer space in the structure
    push    $0              // Create buffer space in the structure

    mov     48(%esp), %ecx  // load the function ID

    push    %ecx            // Push functionID into the structure.

    //
    // Now call into the EE method for Leave, which is declared like this:
    // ProfileTailcall(FunctionID funcID, UINT_PTR clientData, PlatformSpecificStructure *pData)
    //

    push    %esp            // The address of this structure.

    mov     60(%esp), %edx  // The clientData that was pushed to FunctionEnter

    // ecx contains the function id already from above.

    call    FMANGLE(ProfileTailcall,16)

    add     $20, %esp       // Remove the buffer space
    add     $20, %esp
    pop     %edx
    pop     %ecx
    ret     $12
ASMFUNCEND()

