; ==++==
;
;   
;    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
;   
;    The use and distribution terms for this software are contained in the file
;    named license.txt, which can be found in the root of this distribution.
;    By using this software in any fashion, you are agreeing to be bound by the
;    terms of this license.
;   
;    You must not remove this notice, or any other, from this software.
;   
;
; ==--==
;
;  *** NOTE:  If you make changes to this file, propagate the changes to
;             asmhelpers.s in this directory

        .586
        .model  flat

include asmconstants.inc

        option  casemap:none
        .code

EXTERN @LinkFrameAndThrow@4:PROC        ; this is fastcall, with one arg in ECX
ifdef _DEBUG
EXTERN _PerformExitFrameChecks@0:PROC
EXTERN _HelperMethodFrameConfirmState@20:PROC
endif
EXTERN _StubRareEnableWorker@4:PROC
EXTERN _StubRareDisableHRWorker@8:PROC
EXTERN _StubRareDisableTHROWWorker@8:PROC
EXTERN _StubRareDisableRETURNWorker@8:PROC
EXTERN __imp__TlsGetValue@4:DWORD
TlsGetValue PROTO stdcall
EXTERN _CommonTripThread:PROC
EXTERN _COMPlusEndCatch@12:PROC
EXTERN @COMPlusCheckForAbort@12:PROC    ; this is fastcall, with args in ecx/edx and one on the stack
EXTERN __alloca_probe:PROC
EXTERN _UMThunkStubRareDisableWorker@12:PROC

; To debug that LastThrownObjectException really is EXCEPTION_COMPLUS
ifdef _DEBUG	
EXTERN __imp____CxxFrameHandler:PROC
endif

EXTERN _gThreadTLSIndex:DWORD
EXTERN _gAppDomainTLSIndex:DWORD
EXTERN _g_dwPreStubAddr:DWORD
EXTERN _g_dwOOContextAddr:DWORD
EXTERN _g_dwTPStubAddr:DWORD
EXTERN _GetThread:DWORD
EXTERN _GetAppDomain:DWORD
EXTERN _s_gsCookie:DWORD

EXTERN _InContextTPQuickDispatchAsmStub@0:PROC

EXTERN @JIT_InternalThrow@4:PROC
EXTERN @ProfileEnter@12:PROC
EXTERN @ProfileLeave@12:PROC
EXTERN @ProfileTailcall@12:PROC

FASTCALL_FUNC macro FuncName,cbArgs
FuncNameReal EQU @&FuncName&@&cbArgs
FuncNameReal proc public
endm

FASTCALL_ENDFUNC macro
FuncNameReal endp
endm

;
; This macro verifies that the pointer held in reg is 4-byte aligned
; and breaks into the debugger if the condition is not met
;
ifdef _DEBUG
_ASSERT_ALIGNED_4_X86 macro reg
    test reg, 3
    jz @f
    int 3
@@:
endm

_ASSERT_ALIGNED_8_X86 macro reg
    test reg, 7
    jz @f
    int 3
@@:
endm

else

_ASSERT_ALIGNED_4_X86 macro reg
endm

_ASSERT_ALIGNED_8_X86 macro reg
endm

endif

ResetCurrentContext PROC stdcall public
        LOCAL ctrlWord:WORD

        ; Clear the direction flag (used for rep instructions)
        cld

        fnstcw ctrlWord
        fninit                  ; reset FPU
        and ctrlWord, 0f00h     ; preserve precision and rounding control
        or  ctrlWord, 007fh     ; mask all exceptions
        fldcw ctrlWord          ; preserve precision control
        RET
ResetCurrentContext ENDP

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
NakedThrowHelper PROC stdcall public
        ; Erect a faulting Method Frame.  Layout is as follows ...
        mov edx, esp

        ; FaultingExceptionFrame::m_regs
        push ebp                ; ebp
        push ebx                ; ebx
        push esi                ; esi
        push edi                ; edi

        push edx                ; original esp for FaultingExceptionFrame::m_Esp
        push ecx                ; m_ReturnAddress (i.e. original IP)
        sub  esp,12             ; m_dummy (trash)
                                ; m_next (filled in by LinkFrameAndThrow)
                                ; FaultingExceptionFrame VFP (ditto)

        mov ecx, esp
        push _s_gsCookie        ; GSCookie
        call @LinkFrameAndThrow@4
ifdef LOP_FRIENDLY_FRAMES
        add esp, 4+36           ; GSCookie+FaultingExceptionFrame
endif
        RET
NakedThrowHelper ENDP

ifdef _DEBUG
endif ; _DEBUG


_ResumeAtJitEHHelper@4 PROC public
        mov     edx, [esp+4]     ; edx = pContext (EHContext*)

        mov     eax, [edx+EHContext_Eax]
        mov     ebx, [edx+EHContext_Ebx]
        mov     esi, [edx+EHContext_Esi]
        mov     edi, [edx+EHContext_Edi]
        mov     ebp, [edx+EHContext_Ebp]
        mov     esp, [edx+EHContext_Esp]

        jmp     dword ptr [edx+EHContext_Eip]
_ResumeAtJitEHHelper@4 ENDP

; int __stdcall CallJitEHFilterHelper(size_t *pShadowSP, EHContext *pContext);
;   on entry, only the pContext->Esp, Ebx, Esi, Edi, Ebp, and Eip are initialized
_CallJitEHFilterHelper@8 PROC public
        push    ebp
        mov     ebp, esp
        push    ebx
        push    esi
        push    edi

        pShadowSP equ [ebp+8]
        pContext  equ [ebp+12]

        mov     eax, pShadowSP      ; Write esp-4 to the shadowSP slot
        test    eax, eax
        jz      DONE_SHADOWSP_FILTER
        mov     ebx, esp
        sub     ebx, 4
        or      ebx, SHADOW_SP_IN_FILTER_ASM
        mov     [eax], ebx
    DONE_SHADOWSP_FILTER:

        mov     edx, [pContext]
        mov     eax, [edx+EHContext_Eax]
        mov     ebx, [edx+EHContext_Ebx]
        mov     esi, [edx+EHContext_Esi]
        mov     edi, [edx+EHContext_Edi]
        mov     ebp, [edx+EHContext_Ebp]

        call    dword ptr [edx+EHContext_Eip]
ifdef _DEBUG
        nop  ; Indicate that it is OK to call managed code directly from here
endif
        pop     edi
        pop     esi
        pop     ebx
        pop     ebp ; don't use 'leave' here, as ebp as been trashed
        retn    8
_CallJitEHFilterHelper@8 ENDP


; void __stdcall CallJITEHFinallyHelper(size_t *pShadowSP, EHContext *pContext);
;   on entry, only the pContext->Esp, Ebx, Esi, Edi, Ebp, and Eip are initialized
_CallJitEHFinallyHelper@8 PROC public
        push    ebp
        mov     ebp, esp
        push    ebx
        push    esi
        push    edi

        pShadowSP equ [ebp+8]
        pContext  equ [ebp+12]

        mov     eax, pShadowSP      ; Write esp-4 to the shadowSP slot
        test    eax, eax
        jz      DONE_SHADOWSP_FINALLY
        mov     ebx, esp
        sub     ebx, 4
        mov     [eax], ebx
    DONE_SHADOWSP_FINALLY:

        mov     edx, [pContext]
        mov     eax, [edx+EHContext_Eax]
        mov     ebx, [edx+EHContext_Ebx]
        mov     esi, [edx+EHContext_Esi]
        mov     edi, [edx+EHContext_Edi]
        mov     ebp, [edx+EHContext_Ebp]
        call    dword ptr [edx+EHContext_Eip]
ifdef _DEBUG
        nop  ; Indicate that it is OK to call managed code directly from here
endif
        pop     edi
        pop     esi
        pop     ebx
        pop     ebp ; don't use 'leave' here, as ebp as been trashed
        retn    8
_CallJitEHFinallyHelper@8 ENDP


_GetSpecificCpuTypeAsm@0 PROC public
        push    ebx         ; ebx is trashed by the cpuid calls

        ; See if the chip supports CPUID
        pushfd
        pop     ecx         ; Get the EFLAGS
        mov     eax, ecx    ; Save for later testing
        xor     ecx, 200000h ; Invert the ID bit.
        push    ecx
        popfd               ; Save the updated flags.
        pushfd
        pop     ecx         ; Retrive the updated flags
        xor     ecx, eax    ; Test if it actually changed (bit set means yes)
        push    eax
        popfd               ; Restore the flags

        test    ecx, 200000h
        jz      Assume486

        xor     eax, eax
        cpuid

        test    eax, eax
        jz      Assume486   ; brif CPUID1 not allowed

        mov     eax, 1
        cpuid

        ; filter out everything except family and model
        ; Note that some multi-procs have different stepping number for each proc
        and     eax, 0ff0h

        jmp     CpuTypeDone

Assume486:
        mov     eax, 0400h ; report 486
CpuTypeDone:
        pop     ebx
        retn
_GetSpecificCpuTypeAsm@0 ENDP

; DWORD __stdcall GetSpecificCpuFeaturesAsm(DWORD *pInfo);
_GetSpecificCpuFeaturesAsm@4 PROC public
        push    ebx         ; ebx is trashed by the cpuid calls

        ; See if the chip supports CPUID
        pushfd
        pop     ecx         ; Get the EFLAGS
        mov     eax, ecx    ; Save for later testing
        xor     ecx, 200000h ; Invert the ID bit.
        push    ecx
        popfd               ; Save the updated flags.
        pushfd
        pop     ecx         ; Retrive the updated flags
        xor     ecx, eax    ; Test if it actually changed (bit set means yes)
        push    eax
        popfd               ; Restore the flags

        test    ecx, 200000h
        jz      CpuFeaturesFail

        xor     eax, eax
        cpuid

        test    eax, eax
        jz      CpuFeaturesDone ; br if CPUID1 not allowed

        mov     eax, 1
        cpuid
        mov     eax, edx        ; return all feature flags
        mov     edx, [esp+8]
        test    edx, edx
        jz      CpuFeaturesDone
        mov     [edx],ebx       ; return additional useful information
        jmp     CpuFeaturesDone

CpuFeaturesFail:
        xor     eax, eax    ; Nothing to report
CpuFeaturesDone:
        pop     ebx
        retn    4
_GetSpecificCpuFeaturesAsm@4 ENDP

; Atomic bit manipulations, with and without the lock prefix.  We initialize
; all consumers to go through the appropriate service at startup.

FASTCALL_FUNC OrMaskUP,8
        _ASSERT_ALIGNED_4_X86 ecx
        or      dword ptr [ecx], edx
        ret
FASTCALL_ENDFUNC OrMaskUP

FASTCALL_FUNC AndMaskUP,8
        _ASSERT_ALIGNED_4_X86 ecx
        and     dword ptr [ecx], edx
        ret
FASTCALL_ENDFUNC AndMaskUP

FASTCALL_FUNC ExchangeUP,8
        _ASSERT_ALIGNED_4_X86 ecx
        mov     eax, [ecx]      ; attempted comparand
retry:
        cmpxchg [ecx], edx
        jne     retry1          ; predicted NOT taken
        retn
retry1:
        jmp     retry
FASTCALL_ENDFUNC ExchangeUP

FASTCALL_FUNC ExchangeAddUP,8
        _ASSERT_ALIGNED_4_X86 ecx
        xadd    [ecx], edx      ; Add Value to Target
        mov     eax, edx
        retn
FASTCALL_ENDFUNC ExchangeAddUP

FASTCALL_FUNC CompareExchangeUP,12
        _ASSERT_ALIGNED_4_X86 ecx
        mov     eax, [esp+4]    ; Comparand
        cmpxchg [ecx], edx
        retn    4               ; result in EAX
FASTCALL_ENDFUNC CompareExchangeUP

FASTCALL_FUNC IncrementUP,4
        _ASSERT_ALIGNED_4_X86 ecx
        mov     eax, 1
        xadd    [ecx], eax
        inc     eax             ; return prior value, plus 1 we added
        retn
FASTCALL_ENDFUNC IncrementUP

FASTCALL_FUNC DecrementUP,4
        _ASSERT_ALIGNED_4_X86 ecx
        mov     eax, -1
        xadd    [ecx], eax
        dec     eax             ; return prior value, less 1 we removed
        retn
FASTCALL_ENDFUNC DecrementUP

FASTCALL_FUNC OrMaskMP,8
        _ASSERT_ALIGNED_4_X86 ecx
  lock  or      dword ptr [ecx], edx
        retn
FASTCALL_ENDFUNC OrMaskMP

FASTCALL_FUNC AndMaskMP,8
        _ASSERT_ALIGNED_4_X86 ecx
  lock  and     dword ptr [ecx], edx
        retn
FASTCALL_ENDFUNC AndMaskMP

FASTCALL_FUNC ExchangeMP,8
        _ASSERT_ALIGNED_4_X86 ecx
        mov     eax, [ecx]      ; attempted comparand
retryMP:
  lock  cmpxchg [ecx], edx
        jne     retry1MP        ; predicted NOT taken
        retn
retry1MP:
        jmp     retryMP
FASTCALL_ENDFUNC ExchangeMP

FASTCALL_FUNC ExchangeLongMP8b,12
        _ASSERT_ALIGNED_4_X86 ecx

        push    esi
        push    ebx
        mov     esi, ecx
        mov     ebx, [esp+12]     ; EBX:ECX is the value to switch if equal
        mov     ecx, [esp+16]

elMP8bPre:
        mov     eax, [esi]        ; EAX:EDX is the value to compare with
        mov     edx, [esi+4]

  lock  cmpxchg8b qword ptr [esi]
        jnz      elMP8bPre        ;Swap was unsuccessful try again

        pop     ebx
        pop     esi

        retn    8
FASTCALL_ENDFUNC ExchangeLongMP8b

FASTCALL_FUNC CompareExchangeMP,12
        _ASSERT_ALIGNED_4_X86 ecx
        mov     eax, [esp+4]    ; Comparand
  lock  cmpxchg [ecx], edx
        retn    4               ; result in EAX
FASTCALL_ENDFUNC CompareExchangeMP

FASTCALL_FUNC CompareExchangeLongMP8b,20

; EAX:EDX is the value to compare with
; EBX:ECX is the value to switch if equal
;RETURN VAL
; EAX:EDX are the return
; if success then eax:edx will contain the previous
;           since esi is pointing to ecx
; if failure then cmpxchg8b will move
;           [ESI]:[ESI+4] into EAX:EDX

        _ASSERT_ALIGNED_4_X86 ecx
        push    ebx
        push    esi
        mov     esi, ecx
        mov     eax, [esp+20]
        mov     edx, [esp+24]
        mov     ebx, [esp+12]
        mov     ecx, [esp+16]

  lock  cmpxchg8b qword ptr [esi]

        pop     esi
        pop     ebx
        retn    16
FASTCALL_ENDFUNC CompareExchangeLongMP8b

FASTCALL_FUNC ExchangeAddMP,8
        _ASSERT_ALIGNED_4_X86 ecx
  lock  xadd    [ecx], edx      ; Add Value to Target
        mov     eax, edx        ; move result
        retn
FASTCALL_ENDFUNC ExchangeAddMP

FASTCALL_FUNC ExchangeAddLongMP8b,12
        _ASSERT_ALIGNED_4_X86 ecx

        push    esi
        push    ebx
        mov     esi, ecx

ealMP8bPre:
        mov     eax, [esi]        ; load the previous value
        mov     edx, [esi+4]

        mov     ebx, [esp+12]     ; EBX:ECX is the value to add with target
        mov     ecx, [esp+16]

        add     ebx, [esi]
        adc     ecx, [esi+4]

  lock  cmpxchg8b qword ptr [esi]
        jnz      ealMP8bPre       ; Someone changed the dest, Swap was unsuccessful try again

        pop     ebx
        pop     esi

        retn    8
FASTCALL_ENDFUNC ExchangeAddLongMP8b

FASTCALL_FUNC IncrementMP,4
        _ASSERT_ALIGNED_4_X86 ecx
        mov     eax, 1
  lock  xadd    [ecx], eax
        inc     eax             ; return prior value, plus 1 we added
        retn
FASTCALL_ENDFUNC IncrementMP

FASTCALL_FUNC IncrementLongMP8b,4

        _ASSERT_ALIGNED_4_X86 ecx

        push    esi
        push    ebx
        mov     esi, ecx

        xor     edx, edx
        xor     eax, eax
        xor     ecx, ecx
        mov     ebx, 1

  lock  cmpxchg8b qword ptr [esi]
        jz      done

preempted:
        mov     ecx, edx
        mov     ebx, eax
        add     ebx, 1
        adc     ecx, 0

  lock  cmpxchg8b qword ptr [esi]
        jnz     preempted

done:
        mov     edx, ecx
        mov     eax, ebx

        pop     ebx
        pop     esi
        retn
FASTCALL_ENDFUNC IncrementLongMP8b

FASTCALL_FUNC DecrementMP,4
        _ASSERT_ALIGNED_4_X86 ecx
        mov     eax, -1
  lock  xadd    [ecx], eax
        dec     eax             ; return priory value, less 1 we removed
        retn
FASTCALL_ENDFUNC DecrementMP

FASTCALL_FUNC DecrementLongMP8b,4

        _ASSERT_ALIGNED_4_X86 ecx

        push    esi
        push    ebx
        mov     esi, ecx

        xor     edx, edx
        xor     eax, eax
        mov     ecx, 0ffffffffh
        mov     ebx, 0ffffffffh

  lock  cmpxchg8b qword ptr [esi]
        jz      donedec

preempteddec:
        mov     ecx, edx
        mov     ebx, eax
        sub     ebx, 1
        sbb     ecx, 0

  lock  cmpxchg8b qword ptr [esi]
        jnz     preempteddec

donedec:
        mov     edx, ecx
        mov     eax, ebx

        pop     ebx
        pop     esi
        retn
FASTCALL_ENDFUNC DecrementLongMP8b

ifdef _DEBUG
; void Frame::CheckExitFrameDebuggerCalls(void)
; NOTE: this may be called with cdecl, stdcall, or __thiscall, but not __fastcall
CheckExitFrameDebuggerCalls PROC stdcall public
        push    ecx         ; preserve ecx, in case this is a __thiscall call
        call    _PerformExitFrameChecks@0
        pop     ecx
        jmp     eax
CheckExitFrameDebuggerCalls ENDP
endif


;-----------------------------------------------------------------------
; The out-of-line portion of the code to enable preemptive GC.
; After the work is done, the code jumps back to the "pRejoinPoint"
; which should be emitted right after the inline part is generated.
;
; Assumptions:
;      ebx = Thread
; Preserves
;      all registers except ecx.
;
;-----------------------------------------------------------------------
_StubRareEnable proc public
        push    eax
        push    edx

        push    ebx
        call    _StubRareEnableWorker@4

        pop     edx
        pop     eax
        retn
_StubRareEnable ENDP

_StubRareDisableHR proc public
        push    edx

        push    esi     ; Frame
        push    ebx     ; Thread
        call    _StubRareDisableHRWorker@8

        pop     edx
        retn
_StubRareDisableHR ENDP

_StubRareDisableTHROW proc public
        push    eax
        push    edx

        push    esi     ; Frame
        push    ebx     ; Thread
        call    _StubRareDisableTHROWWorker@8

        pop     edx
        pop     eax
        retn
_StubRareDisableTHROW endp

_StubRareDisableRETURN proc public
        push    eax
        push    edx

        push    esi     ; Frame
        push    ebx     ; Thread
        call    _StubRareDisableRETURNWorker@8

        pop     edx
        pop     eax
        retn
_StubRareDisableRETURN endp



InternalExceptionWorker proc public
        pop     edx             ; recover RETADDR
        add     esp, eax        ; release caller's args
        push    edx             ; restore RETADDR
        jmp     @JIT_InternalThrow@4
InternalExceptionWorker endp

; EAX -> number of caller arg bytes on the stack that we must remove before going
; to the throw helper, which assumes the stack is clean.
_ArrayOpStubNullException proc public
        ; kFactorReg and kTotalReg could not have been modified, but let's pop
        ; them anyway for consistency and to avoid future bugs.
        pop     esi
        pop     edi
        mov     ecx, CORINFO_NullReferenceException_ASM
        jmp     InternalExceptionWorker
_ArrayOpStubNullException endp

; EAX -> number of caller arg bytes on the stack that we must remove before going
; to the throw helper, which assumes the stack is clean.
_ArrayOpStubRangeException proc public
        ; kFactorReg and kTotalReg could not have been modified, but let's pop
        ; them anyway for consistency and to avoid future bugs.
        pop     esi
        pop     edi
        mov     ecx, CORINFO_IndexOutOfRangeException_ASM
        jmp     InternalExceptionWorker
_ArrayOpStubRangeException endp

; EAX -> number of caller arg bytes on the stack that we must remove before going
; to the throw helper, which assumes the stack is clean.
_ArrayOpStubTypeMismatchException proc public
        ; kFactorReg and kTotalReg could not have been modified, but let's pop
        ; them anyway for consistency and to avoid future bugs.
        pop     esi
        pop     edi
        mov     ecx, CORINFO_ArrayTypeMismatchException_ASM
        jmp     InternalExceptionWorker
_ArrayOpStubTypeMismatchException endp

;------------------------------------------------------------------------------
; This helper routine enregisters the appropriate arguments and makes the
; actual call.
;------------------------------------------------------------------------------
;ARG_SLOT
;#ifdef _DEBUG
;      CallDescrWorkerInternal
;#else
;      CallDescrWorker
;#endif
;                     (LPVOID                   pSrcEnd,            // points just after the last argument
;                      UINT32                   numStackSlots,      // pSrcEnd-numStackSlots is the start of the arguments
;                      const ArgumentRegisters *pArgumentRegisters,
;                      UINT32                   fpRetSize,
;                      LPVOID                   pTarget
;                     )
ifdef _DEBUG
CallDescrWorkerInternal PROC stdcall public,
                         pSrcEnd: DWORD,
                         numStackSlots: DWORD,
                         pArgumentRegisters: DWORD,
                         fpRetSize: DWORD,  ; NOTE: fpRetSize and pTarget must be adjacent, see ReturnsDouble label
                         pTarget: DWORD
else
CallDescrWorker         PROC stdcall public,
                         pSrcEnd: DWORD,
                         numStackSlots: DWORD,
                         pArgumentRegisters: DWORD,
                         fpRetSize: DWORD,
                         pTarget: DWORD
endif

        LOCAL   doubleRet:QWORD             ; Make space for a float/double return value

        mov     eax, pSrcEnd                ; copy the stack
        mov     ecx, numStackSlots
        test    ecx, ecx
        jz      donestack
        sub     eax, 4
        push    dword ptr [eax]
        dec     ecx
        jz      donestack
        sub     eax, 4
        push    dword ptr [eax]
        dec     ecx
        jz      donestack
stackloop:
        sub     eax, 4
        push    dword ptr [eax]
        dec     ecx
        jnz     stackloop
donestack:

        ; now we must push each field of the ArgumentRegister structure
        mov     eax, pArgumentRegisters
        mov     edx, dword ptr [eax]
        mov     ecx, dword ptr [eax+4]

        call    pTarget
ifdef _DEBUG
        nop     ; This is a tag that we use in an assert.  Fcalls expect to
                ; be called from Jitted code or from certain blessed call sites like
                ; this one.  (See HelperMethodFrame::InsureInit)
endif

        ; Save FP return value
        mov     ecx, fpRetSize
        cmp     ecx, 4
        je      ReturnsFloat
        cmp     ecx, 8
        je      ReturnsDouble
        jmp     Epilog

ReturnsFloat:
        fstp    dword ptr doubleRet     ; Spill the Float return value to the stack and ...
        mov     eax, dword ptr doubleRet; load it into EAX
        jmp     Epilog

ReturnsDouble:
        fstp    qword ptr doubleRet     ; Spill the Double return value to the stack and ...
        mov     eax, dword ptr doubleRet; load it into EDX:EAX
        mov     edx, dword ptr doubleRet+4

Epilog:
       RET
ifdef _DEBUG
CallDescrWorkerInternal endp
else
CallDescrWorker endp
endif

ifdef _DEBUG
; int __fastcall HelperMethodFrameRestoreState(HelperMethodFrame*, struct MachState *)
FASTCALL_FUNC HelperMethodFrameRestoreState,8
    mov         eax, edx        ; eax = MachState*
else
; int __fastcall HelperMethodFrameRestoreState(struct MachState *)
FASTCALL_FUNC HelperMethodFrameRestoreState,4
    mov         eax, ecx        ; eax = MachState*
endif
    ; restore the registers from the m_MachState stucture.  Note that
    ; we only do this for register that where not saved on the stack
    ; at the time the machine state snapshot was taken.

    cmp         [eax+MachState__pRetAddr], 0

ifndef DACCESS_COMPILE
ifdef _DEBUG
    jnz         noConfirm
    push        ebp
    push        ebx
    push        edi
    push        esi
    push        ecx     ; HelperFrame*
    call        _HelperMethodFrameConfirmState@20
    ; on return, eax = MachState*
    cmp         [eax+MachState__pRetAddr], 0
noConfirm:
endif
endif

    jz          doRet

    lea         edx, [eax+MachState__esi]       ; Did we have to spill ESI
    cmp         [eax+MachState__pEsi], edx
    jnz         SkipESI
    mov         esi, [edx]                      ; Then restore it
SkipESI:

    lea         edx, [eax+MachState__edi]       ; Did we have to spill EDI
    cmp         [eax+MachState__pEdi], edx
    jnz         SkipEDI
    mov         edi, [edx]                      ; Then restore it
SkipEDI:

    lea         edx, [eax+MachState__ebx]       ; Did we have to spill EBX
    cmp         [eax+MachState__pEbx], edx
    jnz         SkipEBX
    mov         ebx, [edx]                      ; Then restore it
SkipEBX:

    lea         edx, [eax+MachState__ebp]       ; Did we have to spill EBP
    cmp         [eax+MachState__pEbp], edx
    jnz         SkipEBP
    mov         ebp, [edx]                      ; Then restore it
SkipEBP:

doRet:
    xor         eax, eax
    retn
FASTCALL_ENDFUNC HelperMethodFrameRestoreState



;---------------------------------------------------------------------------
; Portable GetThread() function: used if no platform-specific optimizations apply.
; This is in assembly code because we count on edx not getting trashed on calls
; to this function.
;---------------------------------------------------------------------------
; Thread* __stdcall GetThreadGeneric(void);
GetThreadGeneric PROC stdcall public USES ecx edx

ifdef _DEBUG
    cmp         dword ptr [_gThreadTLSIndex], -1
    jnz         @F
    int         3
@@:
endif

    push        dword ptr [_gThreadTLSIndex]
    call        dword ptr [__imp__TlsGetValue@4]
    ret
GetThreadGeneric ENDP

;---------------------------------------------------------------------------
; Portable GetAppdomain() function: used if no platform-specific optimizations apply.
; This is in assembly code because we count on edx not getting trashed on calls
; to this function.
;---------------------------------------------------------------------------
; Appdomain* __stdcall GetAppDomainGeneric(void);
GetAppDomainGeneric PROC stdcall public USES ecx edx

ifdef _DEBUG
    cmp         dword ptr [_gAppDomainTLSIndex], -1
    jnz         @F
    int         3
@@:
endif

    push        dword ptr [_gAppDomainTLSIndex]
    call        dword ptr [__imp__TlsGetValue@4]
    ret
GetAppDomainGeneric ENDP





; Note that the debugger skips this entirely when doing SetIP,
; since COMPlusCheckForAbort should always return 0.  Excep.cpp:LeaveCatch
; asserts that to be true.  If this ends up doing more work, then the
; debugger may need additional support.
; void __stdcall JIT_EndCatch();
JIT_EndCatch PROC stdcall public
    xor     eax, eax
    push    eax         ; pSEH
    push    eax         ; pCtx
    push    eax         ; pCurThread
    call    _COMPlusEndCatch@12 ; returns old esp value in eax

    mov     ecx, [esp]  ; actual return address into jitted code
    mov     edx, eax    ; old esp value
    push    eax         ; save old esp
    push    ebp
    call    @COMPlusCheckForAbort@12  ; returns old esp value
    pop     ecx
    ; at this point, ecx   = old esp value
    ;                [esp] = return address into jitted code
    ;                eax   = 0 (if no abort), address to jump to otherwise
    test    eax, eax
    jz      NormalEndCatch
    mov     esp, ecx
    jmp     eax

NormalEndCatch:
    pop     eax         ; Move the returnAddress into ecx
    mov     esp, ecx    ; Reset esp to the old value
    jmp     eax         ; Resume after the "endcatch"
JIT_EndCatch ENDP


NDirectGenericStubWorkerImpl macro NDirectFlavor

EXTERN _&NDirectFlavor&ComputeFrameSize@12:PROC
EXTERN _&NDirectFlavor&BuildArguments@28:PROC
EXTERN _&NDirectFlavor&PostCall@36:PROC

;---------------------------------------------------------
; Performs an N/Direct call. This is a generic version
; that can handly any N/Direct call but is not as fast
; as more specialized versions.
;---------------------------------------------------------
;
; INT64 __stdcall NDirectGenericStubWorker(Thread *pThread,
;                                          NDirectMethodFrame *pFrame);
NDirectFlavor&Worker proc stdcall public,
                         pThread : DWORD,
                         pFrame  : DWORD
    local pHeader : DWORD
    local pvfn    : DWORD
    local pLocals : DWORD
    local pMLCode : DWORD
    local PreESP  : DWORD
    local PostESP : DWORD
.erre NDirectGenericWorkerFrameSize eq 40, "asmconstants.h is incorrect"

    lea     eax, pHeader
    push    eax         ; &pHeader
    push    pFrame
    push    pThread
    call    _&NDirectFlavor&ComputeFrameSize@12
    ; eax = cbAlloc (rounded up to next DWORD)

    call    __alloca_probe ; __alloca, where the amt to allocate is in eax

    push    esp         ; pAlloc
    lea     eax, pvfn
    push    eax         ; &pvfn
    lea     eax, pLocals
    push    eax         ; &pLocals
    lea     eax, pMLCode
    push    eax         ; &pMLCode
    push    pHeader
    push    pFrame
    push    pThread
    call    _&NDirectFlavor&BuildArguments@28
    ; pvfn is now the function to invoke, pLocals points to the locals, and
    ; eax is zero or more MHLF_ constants.

    test    eax, MLHF_THISCALL_ASM
    jz      doit_&NDirectFlavor
    test    eax, MLHF_THISCALLHIDDENARG_ASM
    jz      regularthiscall_&NDirectFlavor

    ; this call, with hidden arg
    pop     eax         ; pop the first argument, shrinking the alloca space
    pop     ecx         ; pop the second argument
    push    eax
    jmp     doit_&NDirectFlavor

regularthiscall_&NDirectFlavor:
    pop     ecx         ; pop the first argument, shrinking the alloca space

doit_&NDirectFlavor:
    mov     [PreESP], esp
    call    dword ptr [pvfn]

; this label is used by the debugger, and must immediately follow the 'call'.
public _&NDirectFlavor&ReturnFromCall@0
_&NDirectFlavor&ReturnFromCall@0:

    mov     [PostESP], esp

    push    PostESP
    push    PreESP
    push    edx         ; push high dword of the return value
    push    eax         ; push low  dword of the return value
    push    pLocals
    push    pMLCode
    push    pHeader
    push    pFrame
    push    pThread
    call    _&NDirectFlavor&PostCall@36

    ; return value is in edx:eax
    ret
NDirectFlavor&Worker endp

    endm

NDirectGenericStubWorkerImpl NDirectGenericStub
NDirectGenericStubWorkerImpl DllImportForDelegateGenericStub




; LPVOID __stdcall CTPMethodTable__CallTargetHelper2(
;     const void *pTarget,
;     LPVOID pvFirst,
;     LPVOID pvSecond)
CTPMethodTable__CallTargetHelper2 proc stdcall public,
                                  pTarget : DWORD,
                                  pvFirst : DWORD,
                                  pvSecond : DWORD
    mov     ecx, pvFirst
    mov     edx, pvSecond

    call    pTarget
ifdef _DEBUG
    nop                         ; Mark this as a special call site that can
                                ; directly call unmanaged code
endif
    ret
CTPMethodTable__CallTargetHelper2 endp

; LPVOID __stdcall CTPMethodTable__CallTargetHelper3(
;     const void *pTarget,
;     LPVOID pvFirst,
;     LPVOID pvSecond,
;     LPVOID pvThird)
CTPMethodTable__CallTargetHelper3 proc stdcall public,
                                  pTarget : DWORD,
                                  pvFirst : DWORD,
                                  pvSecond : DWORD,
                                  pvThird : DWORD
    push    pvThird

    mov     ecx, pvFirst
    mov     edx, pvSecond

    call    pTarget
ifdef _DEBUG
    nop                         ; Mark this as a special call site that can
                                ; directly call unmanaged code
endif
    ret
CTPMethodTable__CallTargetHelper3 endp


; void __stdcall setFPReturn(int fpSize, INT64 retVal)
_setFPReturn@12 proc public
    mov     eax, [esp+4]
    cmp     eax, 4
    jz      setFPReturn4

    cmp     eax, 8
    jnz     setFPReturnNot8
    fld     qword ptr [esp+8]
setFPReturnNot8:
    retn    12

setFPReturn4:
    fld     dword ptr [esp+8]
    retn    12
_setFPReturn@12 endp

; void __stdcall getFPReturn(int fpSize, INT64 *pretVal)
_getFPReturn@8 proc public
   mov     ecx, [esp+4]
   mov     eax, [esp+8]
   cmp     ecx, 4
   jz      getFPReturn4

   cmp     ecx, 8
   jnz     getFPReturnNot8
   fstp    qword ptr [eax]
getFPReturnNot8:
   retn    8

getFPReturn4:
   fstp    dword ptr [eax]
   retn    8
_getFPReturn@8 endp

; void *__stdcall UM2MThunk_WrapperHelper(void *pThunkArgs,
;                                         int argLen,
;                                         void *pAddr,
;                                         UMEntryThunk *pEntryThunk,
;                                         Thread *pThread)
UM2MThunk_WrapperHelper proc stdcall public,
                        pThunkArgs : DWORD,
                        argLen : DWORD,
                        pAddr : DWORD,
                        pEntryThunk : DWORD,
                        pThread : DWORD

    mov     ecx, argLen
    test    ecx, ecx
    jz      UM2MThunk_ArgsComplete ; brif no args

    ; copy the args to the stack, they will be released when we return from
    ; the thunk because we are coming from unmanaged to managed, any
    ; objectref args must have already been pinned so don't have to worry
    ; about them moving in the copy we make.
    _ASSERT_ALIGNED_4_X86 ecx
    mov     eax, pThunkArgs
UM2MThunk_CopyArg:
    push    dword ptr [eax+ecx-4]          ; push the argument on the stack
    sub     ecx, 4               ; move to the next argument
    jnz     UM2MThunk_CopyArg

UM2MThunk_ArgsComplete:
    mov     eax, pEntryThunk
    mov     ecx, pThread
    call    pAddr

    ret
UM2MThunk_WrapperHelper endp

; VOID __cdecl UMThunkStubRareDisable()
_UMThunkStubRareDisable proc public
    push    eax
    push    ecx

    push    esi          ; Push frame
    push    eax          ; Push the UMEntryThunk
    push    ecx          ; Push thread
    call    _UMThunkStubRareDisableWorker@12

    pop     ecx
    pop     eax
    retn
_UMThunkStubRareDisable endp


;+----------------------------------------------------------------------------
;
;  Method:     CRemotingServices::CheckForContextMatch   public
;
;  Synopsis:   This code generates a check to see if the current context and
;              the context of the proxy match.
;+----------------------------------------------------------------------------
;
; returns zero if contexts match
; returns non-zero if contexts do not match
;
; UINT_PTR __stdcall CRemotingServices__CheckForContextMatch(Object* pStubData)
_CRemotingServices__CheckForContextMatch@4 proc public
    push    ebx                  ; spill ebx
    mov     ebx, [eax+4]         ; Get the internal context id by unboxing
                                 ; the stub data
    call    [_GetThread]         ; Get the current thread, assumes that the
                                 ; registers are preserved
    mov     eax, [eax+Thread_m_Context] ; Get the current context from the
                                 ; thread
    sub     eax, ebx             ; Get the pointer to the context from the
                                 ; proxy and compare with the current context
    pop     ebx                  ; restore the value of ebx
    retn
_CRemotingServices__CheckForContextMatch@4 endp

;+----------------------------------------------------------------------------
;
;  Method:     CRemotingServices::DispatchInterfaceCall   public
;
;  Synopsis:
;              Push that method desc on the stack and jump to the
;              transparent proxy stub to execute the call.
;              WARNING!! This MethodDesc is not the methoddesc in the vtable
;              of the object instead it is the methoddesc in the vtable of
;              the interface class. Since we use the MethodDesc only to probe
;              the stack via the signature of the method call we are safe.
;              If we want to get any object vtable/class specific
;              information this is not safe.
;
;+----------------------------------------------------------------------------
; void __stdcall CRemotingServices__DispatchInterfaceCall(MethodDesc *pMD)
_CRemotingServices__DispatchInterfaceCall@4 proc public
    ; NOTE: At this point the stack looks like
    ;
    ; esp--->  return addr of stub
    ;          saved MethodDesc of Interface method
    ;          return addr of calling function
    ;
    mov      eax, [ecx + TP_OFFSET_STUBDATA]
    call    [ecx + TP_OFFSET_STUB]
ifdef _DEBUG
    nop     ; Mark this as a special call site that can directly
            ; call managed code
endif
    test    eax, eax
    jnz     CtxMismatch


    add     esp,4
    jmp     _InContextTPQuickDispatchAsmStub@0


CtxMismatch:                                     ; Jump to TPStub

    add     esp, 4                               ; pop ret addr of stub so that the
                                                 ; stack and registers are now
                                                 ; setup exactly like they
                                                 ; were at the callsite

    jmp     [_g_dwOOContextAddr]                 ; jump to OOContext label in
                                                 ; TPStub
_CRemotingServices__DispatchInterfaceCall@4 endp


;+----------------------------------------------------------------------------
;
;  Method:     CRemotingServices::CallFieldGetter   private
;
;  Synopsis:   Calls the field getter function (Object::__FieldGetter) in
;              managed code by setting up the stack and calling the target
;
;+----------------------------------------------------------------------------
; void __stdcall CRemotingServices__CallFieldGetter(
;    MethodDesc *pMD,
;    LPVOID pThis,
;    LPVOID pFirst,
;    LPVOID pSecond,
;    LPVOID pThird)
CRemotingServices__CallFieldGetter proc stdcall public,
                                   pMD : DWORD,
                                   pThis : DWORD,
                                   pFirst : DWORD,
                                   pSecond : DWORD,
                                   pThird : DWORD

    push    [pSecond]           ; push the second argument on the stack
    push    [pThird]            ; push the third argument on the stack

    mov     ecx, [pThis]        ; enregister pThis, the 'this' pointer
    mov     edx, [pFirst]       ; enregister pFirst, the first argument

    lea     eax, retAddrFieldGetter
    push    eax

    push    [pMD]               ; push the MethodDesc of object::__FieldGetter
    jmp     [_g_dwTPStubAddr]   ; jump to the TP stub

retAddrFieldGetter:
    ret
CRemotingServices__CallFieldGetter endp


;+----------------------------------------------------------------------------
;
;  Method:     CRemotingServices::CallFieldSetter   private
;
;  Synopsis:   Calls the field setter function (Object::__FieldSetter) in
;              managed code by setting up the stack and calling the target
;
;+----------------------------------------------------------------------------
; void __stdcall CRemotingServices__CallFieldSetter(
;    MethodDesc *pMD,
;    LPVOID pThis,
;    LPVOID pFirst,
;    LPVOID pSecond,
;    LPVOID pThird)
CRemotingServices__CallFieldSetter proc stdcall public,
                                   pMD : DWORD,
                                   pThis : DWORD,
                                   pFirst : DWORD,
                                   pSecond : DWORD,
                                   pThird : DWORD

    push    [pSecond]           ; push the field name (second arg)
    push    [pThird]            ; push the object (third arg) on the stack

    mov     ecx, [pThis]        ; enregister pThis, the 'this' pointer
    mov     edx, [pFirst]       ; enregister the first argument

    lea     eax, retAddrFieldSetter ; push the return address
    push    eax

    push    [pMD]               ; push the MethodDesc of Object::__FieldSetter
    jmp     [_g_dwTPStubAddr]   ; jump to the TP Stub

retAddrFieldSetter:
    ret
CRemotingServices__CallFieldSetter endp


;+----------------------------------------------------------------------------
;
;  Method:     CTPMethodTable::GenericCheckForContextMatch private
;
;  Synopsis:   Calls the stub in the TP & returns TRUE if the contexts
;              match, FALSE otherwise.
;
;  Note:       1. Called during FieldSet/Get, used for proxy extensibility
;+----------------------------------------------------------------------------
; BOOL __stdcall CTPMethodTable__GenericCheckForContextMatch(Object* orTP)
CTPMethodTable__GenericCheckForContextMatch proc stdcall public uses ecx, tp : DWORD

    mov     ecx, [tp]
    mov     eax, [ecx+TP_OFFSET_STUBDATA]
    call    [ecx + TP_OFFSET_STUB]
ifdef _DEBUG
    nop     ; Mark this as a special call site that can directly
            ; call managed code
endif
    test    eax, eax
    mov     eax, 0
    setz    al
    ; NOTE: In the CheckForXXXMatch stubs (for URT ctx/ Ole32 ctx) eax is
    ; non-zero if contexts *do not* match & zero if they do.
    ret
CTPMethodTable__GenericCheckForContextMatch endp


; void __stdcall JIT_ProfilerEnterLeaveStub(UINT_PTR EEHandle, UINT_PTR ProfilerHandle, UINT_PTR FrameInfo, UINT_PTR ArgInfo)
_JIT_ProfilerEnterLeaveStub@16 proc public
    ; this function must preserve all registers, including scratch
    retn    16
_JIT_ProfilerEnterLeaveStub@16 endp

; void __stdcall JIT_ProfilerTailcallStub(UINT_PTR EEHandle, UINT_PTR ProfilerHandle, UINT_PTR FrameInfo)
_JIT_ProfilerTailcallStub@12 proc public
    ; this function must preserve all registers, including scratch
    retn    12
_JIT_ProfilerTailcallStub@12 endp


;
; Used to get the current instruction pointer value
;
; UINT_PTR __stdcall GetCurrentIP(void);
_GetCurrentIP@0 proc public
    mov     eax, [esp]
    retn
_GetCurrentIP@0 endp

; LPVOID __stdcall GetCurrentSP(void);
_GetCurrentSP@0 proc public
    mov     eax, esp
    retn
_GetCurrentSP@0 endp

; void __stdcall ProfileEnterNaked(FunctionID functionId,
;                                  UINT_PTR clientData,
;                                  COR_PRF_FRAME_INFO *pFrameInfo,   // ignored
;                                  COR_PRF_FUNCTION_ARGUMENT_INFO *pArgInfo); // ignored
_ProfileEnterNaked@16 proc public
    push    esi
    push    edi

    ;
    ; Push in reverse order the fields of ProfilePlatformSpecificData
    ;
    push    dword ptr [esp+8] ; EIP of the managed code that we return to.

    push    esp        ; Create some stack space for our parameter.
    push    eax        ; Save off EAX, because we need it to do some math.
    mov     eax, esp
    add     eax, 44    ; Skip past the return IP, straight to the stack args.
    mov     [esp+4], eax ; EAX is now the ESP of the managed code before it called to us.
    pop     eax

    push    ecx
    push    edx
    push    eax
    push    0          ; Create buffer space in the structure
    push    0          ; Create buffer space in the structure
    push    0          ; Create buffer space in the structure
    push    0          ; Create buffer space in the structure

    mov     ecx, [esp + 48] ; The functionID that was pushed to FunctionEnter

    push    ecx        ; Push functionID into the structure.

    ;
    ;  We are now done with the ProfilePlatformSpecificData structure and ready
    ;  to push the args to the _stdcall EE Enter method, which is of the following
    ;  declaratoin  ProfileEnter(FunctionID funcID, UINT_PTR clientData, PlatformSpecificData *pData)
    ;

    push    esp        ; Push the address of the Platform structure.

    mov     edx, [esp + 60] ; The clientData that was pushed to FunctionEnter

    ; ecx contains the FuncId already from above.

    call    @ProfileEnter@12

    add     esp, 20      ; Remove the buffer space
    pop     eax
    pop     edx
    pop     ecx
    add     esp, 8
    pop     edi
    pop     esi

    retn    16
_ProfileEnterNaked@16 endp

; void __stdcall ProfileLeaveNaked(FunctionID functionId,
;                                  UINT_PTR clientData,
;                                  COR_PRF_FRAME_INFO *pFrameInfo,   // ignored
;                                  COR_PRF_FUNCTION_ARGUMENT_RANGE *pArgInfo); // ignored
_ProfileLeaveNaked@16 proc public
    push    ecx       ; We do not strictly need to save ECX, however
                      ; emitNoGChelper(CORINFO_HELP_PROF_FCN_LEAVE) returns true in the JITcompiler
    push    edx       ; Return value may be in EAX:EDX

    ;
    ; Push in reverse order the fields of ProfilePlatformSpecificData
    ;
    push    dword ptr [esp+8] ; EIP of the managed code that we return to.

    push    esp        ; Create some stack space for our parameter.
    push    eax        ; Save off EAX as return value may be in EAX:EDX
                       ; and we need it to do some math.
    mov     eax, esp
    add     eax, 40
    mov     [esp+4], eax ; EAX is now the ESP of the managed code before it called to us.
    pop     eax

    push    ecx
    push    edx
    push    eax

                       ; Save off any floating point registers we may need

    fstsw   ax
    and     ax, 3800h  ; Check the top-of-fp-stack bits
    cmp     ax, 0      ; If non-zero, we have something to save
    jnz     SaveFPReg

    push    0          ; else, fill the buffer space with zeros
    push    0
    push    0
    push    0
    jmp     Continue

SaveFPReg:
    push    1          ; mark that a float value is present

    sub     esp, 4     ; Make room for the FP value
    fst     dword ptr [esp] ; Copy the FP value to the buffer as a float

    sub     esp, 8     ; Make room for the FP value
    fstp    qword ptr [esp] ; Copy the FP value to the buffer as a double

Continue:
    mov     ecx, [esp + 48]

    push    ecx        ; Push functionID into the structure.

    ;
    ; Now call into the EE method for Leave, which is declared like this:
    ;  ProfileLeave(FunctionID funcID, UINT_PTR clientData, PlatformSpecificStructure *pData)
    ;

    push    esp        ; The address of this structure.

    mov     edx, [esp + 60]

    ; ecx contains the function id already from above.

    call    @ProfileLeave@12

    ;
    ; Now see if we have to restore and floating point registers
    ;

    cmp     [esp + 16], 0
    jz      NoRestore

    fld     qword ptr [esp + 4]

NoRestore:

    add     esp, 20    ; pop off buffer space
    pop     eax
    add     esp, 16
    pop     edx
    pop     ecx
    retn    16
_ProfileLeaveNaked@16 endp


; void __stdcall ProfileTailcallNaked(FunctionID functionId,
;                                     UINT_PTR clientData,
;                                     COR_PRF_FRAME_INFO *pFrameInfo);   // ignored
_ProfileTailcallNaked@12 proc public
    push    ecx
    push    edx

    ;
    ; Push in reverse order the fields of ProfilePlatformSpecificData
    ;
    push    dword ptr [esp+8] ; EIP of the managed code that we return to.

    push    esp        ; Create some stack space for our parameter.
    push    eax        ; Save off EAX, because we need it to do some math.
    mov     eax, esp
    add     eax, 32
    mov     [esp+4], eax ; EAX is now the ESP of the managed code before it called to us.
    pop     eax

    push    ecx
    push    edx
    push    eax
    push    0          ; Create buffer space in the structure
    push    0          ; Create buffer space in the structure
    push    0          ; Create buffer space in the structure
    push    0          ; Create buffer space in the structure

    mov     ecx, [esp + 48] ; load the functionID

    push    ecx        ; Push functionID into the structure.

    ;
    ; Now call into the EE method for Leave, which is declared like this:
    ;  ProfileTailcall(FunctionID funcID, UINT_PTR clientData, PlatformSpecificStructure *pData)
    ;

    push    esp        ; The address of this structure.

    mov     edx, [esp + 60]

    ; ecx contains the function id already from above.

    call    @ProfileTailcall@12

    add     esp, 20      ; Remove buffer space
    add     esp, 20
    pop     edx
    pop     ecx
    retn    12
_ProfileTailcallNaked@12 endp

    end
