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
; ***********************************************************************
; File: JIThelp.asm
;
; ***********************************************************************
;
;  *** NOTE:  If you make changes to this file, propagate the changes to
;             jithelp.s in this directory                            

; This contains JITinterface routines that are 100% x86 assembly

; Define this on x86 to test out indirection stubs
; define USES_INDIRECT_STUB

        .586
        .model  flat

        include asmconstants.inc

        option  casemap:none
        .code
;
;

ARGUMENT_REG1           equ     ecx
ARGUMENT_REG2           equ     edx
g_ephemeral_low                 TEXTEQU <_g_ephemeral_low>
g_ephemeral_high                TEXTEQU <_g_ephemeral_high>
g_lowest_address                TEXTEQU <_g_lowest_address>
g_highest_address               TEXTEQU <_g_highest_address>
g_card_table                    TEXTEQU <_g_card_table>
WriteBarrierAssert              TEXTEQU <_WriteBarrierAssert@8>
JIT_LLsh                        TEXTEQU <_JIT_LLsh@0>
JIT_LRsh                        TEXTEQU <_JIT_LRsh@0>
JIT_LRsz                        TEXTEQU <_JIT_LRsz@0>
JIT_WriteBarrierReg_PreGrow     TEXTEQU <_JIT_WriteBarrierReg_PreGrow@0>
JIT_WriteBarrierReg_PostGrow    TEXTEQU <_JIT_WriteBarrierReg_PostGrow@0>
JIT_TailCall                    TEXTEQU <_JIT_TailCall@0>
JIT_TailCallHelper              TEXTEQU <_JIT_TailCallHelper@4>
JIT_TailCallReturnFromVSD       TEXTEQU <_JIT_TailCallReturnFromVSD@0>

EXTERN  g_ephemeral_low:DWORD
EXTERN  g_ephemeral_high:DWORD
EXTERN  g_lowest_address:DWORD
EXTERN  g_highest_address:DWORD
EXTERN  g_card_table:DWORD
ifdef _DEBUG
EXTERN  WriteBarrierAssert:PROC
endif ; _DEBUG	
EXTERN  JIT_TailCallHelper:PROC
EXTERN _GetThread:DWORD
EXTERN _g_TailCanSkipTripForGC:BYTE
EXTERN _g_TailCallFrameVptr:DWORD
EXTERN @JIT_FailFast@0:PROC
EXTERN _s_gsCookie:DWORD


EXTERN _g_dwOOContextAddr:DWORD



;***
;JIT_WriteBarrier* - GC write barrier helper
;
;Purpose:
;   Helper calls in order to assign an object to a field
;   Enables book-keeping of the GC.
;
;Entry:
;   EDX - address of ref-field (assigned to)
;   the resp. other reg - RHS of assignment
;
;Exit:
;
;Uses:
;       EDX is destroyed.
;
;Exceptions:
;
;*******************************************************************************

; The code here is tightly coupled with AdjustContextForWriteBarrier, if you change
; anything here, you might need to change AdjustContextForWriteBarrier as well
WriteBarrierHelper MACRO rg
        ALIGN 4

    ;; The entry point is the fully 'safe' one in which we check if EDX (the REF
    ;; begin updated) is actually in the GC heap

PUBLIC _JIT_CheckedWriteBarrier&rg&@0
_JIT_CheckedWriteBarrier&rg&@0 PROC
        ;; check in the REF being updated is in the GC heap
        cmp             edx, g_lowest_address
        jb              WriteBarrier_NotInHeap_&rg
        cmp             edx, g_highest_address
        jae             WriteBarrier_NotInHeap_&rg

_JIT_CheckedWriteBarrier&rg&@0 ENDP
        ;; fall through to unchecked routine
        ;; note that its entry point also happens to be aligned

    ;; This entry point is used when you know the REF pointer being updated
    ;; is in the GC heap
PUBLIC _JIT_WriteBarrier&rg&@0
_JIT_WriteBarrier&rg&@0 PROC

ifdef _DEBUG
        push    edx
        push    ecx
        push    eax

        push    rg
        push    edx
        call    WriteBarrierAssert

        pop     eax
        pop     ecx
        pop     edx
endif ;_DEBUG

        ; in the !WRITE_BARRIER_CHECK case this will be the move for all
        ; addresses in the GCHeap, addresses outside the GCHeap will get 
        ; taken care of below at WriteBarrier_NotInHeap_&rg

        mov     DWORD PTR [edx], rg

        cmp     rg, g_ephemeral_low
        jb      WriteBarrier_NotInEphemeral_&rg
        cmp     rg, g_ephemeral_high
        jae     WriteBarrier_NotInEphemeral_&rg

        shr     edx, 10
        add     edx, [g_card_table]
        cmp     BYTE PTR [edx], 0FFh
        jne     WriteBarrier_UpdateCardTable_&rg
        ret
        
WriteBarrier_UpdateCardTable_&rg:
        mov     BYTE PTR [edx], 0FFh
        ret

WriteBarrier_NotInHeap_&rg:
        ; If it wasn't in the heap then we haven't updated the dst in memory yet
        mov     DWORD PTR [edx], rg
WriteBarrier_NotInEphemeral_&rg:
        ; If it is in the GC Heap but isn't in the ephemeral range we've already
        ; updated the Heap with the Object*.
        ret
_JIT_WriteBarrier&rg&@0 ENDP



ENDM

;***
;JIT_ByRefWriteBarrier* - GC write barrier helper
;
;Purpose:
;   Helper calls in order to assign an object to a byref field
;   Enables book-keeping of the GC.
;
;Entry:
;   EDI - address of ref-field (assigned to)
;   ESI - address of the data  (source)
;   ECX can be trashed
;
;Exit:
;
;Uses:
;   EDI and ESI are incremented by a DWORD
;
;Exceptions:
;
;*******************************************************************************

; The code here is tightly coupled with AdjustContextForWriteBarrier, if you change
; anything here, you might need to change AdjustContextForWriteBarrier as well

ByRefWriteBarrierHelper MACRO
        ALIGN 4
PUBLIC _JIT_ByRefWriteBarrier@0
_JIT_ByRefWriteBarrier@0 PROC
        ;;test for dest in range
        mov     ecx, [esi] 
        cmp     edi, g_lowest_address
        jb      ByRefWriteBarrier_NotInHeap
        cmp     edi, g_highest_address
        jae     ByRefWriteBarrier_NotInHeap

        ;;write barrier
        mov     [edi],ecx

        ;;test for *src in ephemeral segement
        cmp     ecx, g_ephemeral_low
        jb      ByRefWriteBarrier_NotInEphemeral
        cmp     ecx, g_ephemeral_high
        jae     ByRefWriteBarrier_NotInEphemeral 
        
        mov     ecx, edi
        add     esi,4
        add     edi,4

        shr     ecx, 10
        add     ecx, [g_card_table]
        cmp     byte ptr [ecx], 0FFh
        jne     ByRefWriteBarrier_UpdateCardTable
        ret
ByRefWriteBarrier_UpdateCardTable:
        mov     byte ptr [ecx], 0FFh
        ret
        
ByRefWriteBarrier_NotInHeap:
        ; If it wasn't in the heap then we haven't updated the dst in memory yet
        mov     [edi],ecx
ByRefWriteBarrier_NotInEphemeral:
        ; If it is in the GC Heap but isn't in the ephemeral range we've already
        ; updated the Heap with the Object*.
        add     esi,4
        add     edi,4
        ret
_JIT_ByRefWriteBarrier@0 ENDP
ENDM

;*******************************************************************************
; Write barrier wrappers with fcall calling convention
;
UniversalWriteBarrierHelper MACRO name
        ALIGN 4
PUBLIC @JIT_&name&@8
@JIT_&name&@8 PROC
        mov eax,edx
        mov edx,ecx
        jmp _JIT_&name&EAX@0
@JIT_&name&@8 ENDP
ENDM

; WriteBarrierStart and WriteBarrierEnd are used to determine bounds of
; WriteBarrier functions so can determine if got AV in them. 
PUBLIC _JIT_WriteBarrierStart@0
_JIT_WriteBarrierStart@0 PROC
ret
_JIT_WriteBarrierStart@0 ENDP

UniversalWriteBarrierHelper <CheckedWriteBarrier>
UniversalWriteBarrierHelper <WriteBarrier>

WriteBarrierHelper <EAX>
WriteBarrierHelper <EBX>
WriteBarrierHelper <ECX>
WriteBarrierHelper <ESI>
WriteBarrierHelper <EDI>
WriteBarrierHelper <EBP>

ByRefWriteBarrierHelper

PUBLIC _JIT_WriteBarrierLast@0
_JIT_WriteBarrierLast@0 PROC
ret
_JIT_WriteBarrierLast@0 ENDP

; This is the first function outside the "keep together range". Used by BBT scripts.
PUBLIC _JIT_WriteBarrierEnd@0
_JIT_WriteBarrierEnd@0 PROC
ret
_JIT_WriteBarrierEnd@0 ENDP


;*********************************************************************/
;llshl - long shift left
;
;Purpose:
;   Does a Long Shift Left (signed and unsigned are identical)
;   Shifts a long left any number of bits.
;
;       NOTE:  This routine has been adapted from the Microsoft CRTs.
;
;Entry:
;   EDX:EAX - long value to be shifted
;       ECX - number of bits to shift by
;
;Exit:
;   EDX:EAX - shifted value
;
        ALIGN 16
PUBLIC JIT_LLsh
JIT_LLsh PROC
; Handle shifts of between bits 0 and 31
        cmp     ecx, 32
        jae     short LLshMORE32
        shld    edx,eax,cl
        shl     eax,cl
        ret
; Handle shifts of between bits 32 and 63
LLshMORE32:
        ; The x86 shift instructions only use the lower 5 bits.
        mov     edx,eax
        xor     eax,eax
        shl     edx,cl
        ret
JIT_LLsh ENDP


;*********************************************************************/
;LRsh - long shift right
;
;Purpose:
;   Does a signed Long Shift Right
;   Shifts a long right any number of bits.
;
;       NOTE:  This routine has been adapted from the Microsoft CRTs.
;
;Entry:
;   EDX:EAX - long value to be shifted
;       ECX - number of bits to shift by
;
;Exit:
;   EDX:EAX - shifted value
;
        ALIGN 16
PUBLIC JIT_LRsh
JIT_LRsh PROC
; Handle shifts of between bits 0 and 31
        cmp     ecx, 32
        jae     short LRshMORE32
        shrd    eax,edx,cl
        sar     edx,cl
        ret
; Handle shifts of between bits 32 and 63
LRshMORE32:
        ; The x86 shift instructions only use the lower 5 bits.
        mov     eax,edx
        sar     edx, 31
        sar     eax,cl
        ret
JIT_LRsh ENDP


;*********************************************************************/
; LRsz:
;Purpose:
;   Does a unsigned Long Shift Right
;   Shifts a long right any number of bits.
;
;       NOTE:  This routine has been adapted from the Microsoft CRTs.
;
;Entry:
;   EDX:EAX - long value to be shifted
;       ECX - number of bits to shift by
;
;Exit:
;   EDX:EAX - shifted value
;
        ALIGN 16
PUBLIC JIT_LRsz
JIT_LRsz PROC
; Handle shifts of between bits 0 and 31
        cmp     ecx, 32
        jae     short LRszMORE32
        shrd    eax,edx,cl
        shr     edx,cl
        ret
; Handle shifts of between bits 32 and 63
LRszMORE32:
        ; The x86 shift instructions only use the lower 5 bits.
        mov     eax,edx
        xor     edx,edx
        shr     eax,cl
        ret
JIT_LRsz ENDP



;*********************************************************************/
; This is the small write barrier thunk we use when we know the
; ephemeral generation is higher in memory than older generations.
; The 0x0F0F0F0F values are bashed by the two functions above.
; This the generic version - wherever the code says ECX, 
; the specific register is patched later into a copy
; Note: do not replace ECX by EAX - there is a smaller encoding for
; the compares just for EAX, which won't work for other registers. </STRIP>
PUBLIC JIT_WriteBarrierReg_PreGrow
JIT_WriteBarrierReg_PreGrow PROC
        mov     DWORD PTR [edx], ecx
        cmp     ecx, 0F0F0F0F0h
        jb      NoWriteBarrierPre

        shr     edx, 10
        cmp     byte ptr [edx+0F0F0F0F0h], 0FFh
        jne     WriteBarrierPre
NoWriteBarrierPre:
        ret
WriteBarrierPre:
        mov     byte ptr [edx+0F0F0F0F0h], 0FFh
        ret
JIT_WriteBarrierReg_PreGrow ENDP


PUBLIC JIT_WriteBarrierReg_PostGrow
JIT_WriteBarrierReg_PostGrow PROC
        mov     DWORD PTR [edx], ecx
        cmp     ecx, 0F0F0F0F0h
        jb      NoWriteBarrierPost
        cmp     ecx, 0F0F0F0F0h
        jae     NoWriteBarrierPost

        shr     edx, 10
        cmp     byte ptr [edx+0F0F0F0F0h], 0FFh
        jne     WriteBarrierPost
NoWriteBarrierPost:
        ret
WriteBarrierPost:
        mov     byte ptr [edx+0F0F0F0F0h], 0FFh
        ret
JIT_WriteBarrierReg_PostGrow ENDP

;*********************************************************************/

        ; a fake virtual stub dispatch register indirect callsite
        nop
        nop
        nop
        call    dword ptr [eax]


PUBLIC JIT_TailCallReturnFromVSD
JIT_TailCallReturnFromVSD:
ifdef _DEBUG
        nop                         ; blessed callsite
endif
        call    VSDHelperLabel      ; keep call-ret count balanced.
VSDHelperLabel:

; Stack at this point :
;    ...
; m_ReturnAddress
; m_regs
; m_CallerAddress
; m_pThread
; vtbl
; GSCookie
; &VSDHelperLabel
OffsetOfTailCallFrame = 8

; ebx = pThread

ifdef _DEBUG
        mov     esi, _s_gsCookie        ; GetProcessGSCookie()
        cmp     dword ptr [esp+OffsetOfTailCallFrame-SIZEOF_GSCookie], esi
        je      TailCallFrameGSCookieIsValid
        call    @JIT_FailFast@0
    TailCallFrameGSCookieIsValid:
endif
        ; remove the padding frame from the chain
        mov     esi, dword ptr [esp+OffsetOfTailCallFrame+4]    ; esi = TailCallFrame::m_Next
        mov     dword ptr [ebx + Thread_m_pFrame], esi

        ; skip the frame
        add     esp, 20     ; &VSDHelperLabel, GSCookie, vtbl, m_Next, m_CallerAddress

        pop     edi         ; restore callee saved registers
        pop     esi
        pop     ebx
        pop     ebp

        ret                 ; return to m_ReturnAddress

;------------------------------------------------------------------------------

PUBLIC JIT_TailCall
JIT_TailCall PROC

; the stack layout at this point is:
;
;   ebp+8+4*nOldStackArgs   <- end of argument destination
;    ...                       ...
;   ebp+8+                     old args (size is nOldStackArgs)
;    ...                       ...
;   ebp+8                   <- start of argument destination
;   ebp+4                   ret addr
;   ebp+0                   saved ebp
;   ebp-c                   saved ebx, esi, edi (if have callee saved regs = 1)
;
;                           other stuff (local vars) in the jitted callers' frame
;
;   esp+20+4*nNewStackArgs  <- end of argument source
;    ...                       ...
;   esp+20+                    new args (size is nNewStackArgs) to be passed to the target of the tail-call
;    ...                       ...
;   esp+20                  <- start of argument source
;   esp+16                  nOldStackArgs
;   esp+12                  nNewStackArgs
;   esp+8                   flags (1 = have callee saved regs, 2 = virtual stub dispatch)
;   esp+4                   target addr
;   esp+0                   retaddr

RetAddr         equ 0
TargetAddr      equ 4
Flags           equ 8
nNewStackArgs   equ 12
nOldStackArgs   equ 16
NewArgs         equ 20

; extra space is incremented as we push things on the stack along the way
ExtraSpace      = 0

        call    _GetThread  ; eax = Thread*
        push    eax         ; Thread*

        ; save ArgumentRegisters
        push    ecx
        push    edx

ExtraSpace      = 12    ; pThread, ecx, edx

        ; For GC stress, we always need to trip for GC
        test    _g_TailCanSkipTripForGC, 0FFh
        jz      TripForGC
        
        ; Trip for GC only if necessary
        test    dword ptr [eax+Thread_m_State], TS_CatchAtSafePoint_ASM
        jz      NoTripForGC

TripForGC:

; Create a MachState struct on the stack

; return address is already on the stack, but is separated from stack 
; arguments by the extra arguments of JIT_TailCall. So we cant use it directly

        push    0DDDDDDDDh

; Esp on unwind. Not needed as we it is deduced from the target method

        push    0CCCCCCCCh
        push    ebp 
        push    esp         ; pEbp
        push    ebx 
        push    esp         ; pEbx
        push    esi 
        push    esp         ; pEsi
        push    edi 
        push    esp         ; pEdi

        push    esp         ; TailCallArgs*

        ; JIT_TailCallHelper(TailCallArgs*)
        call    JIT_TailCallHelper  ; this is __stdcall

        ; Restore MachState registers
        pop     edx         ; discard pEdi
        pop     edi         ; restore edi
        pop     ecx         ; discard pEsi
        pop     esi         ; restore esi
        pop     edx         ; discard pEbx
        pop     ebx         ; restore ebx
        pop     ecx         ; discard pEbp
        pop     ebp         ; restore ebp

        pop     edx         ; discard esp
        pop     ecx         ; discard return address
        
NoTripForGC:

ExtraSpace      = 12    ; pThread, ecx, edx

        mov     edx, dword ptr [esp+ExtraSpace+Flags]           ; edx = flags

        mov     eax, dword ptr [esp+ExtraSpace+nOldStackArgs]   ; eax = nOldStackArgs
        mov     ecx, dword ptr [esp+ExtraSpace+nNewStackArgs]   ; ecx = nNewStackArgs

        ; restore callee saved registers
        test    edx, 1
        jz      NoCalleeSaveRegisters
        
        mov     edi, dword ptr [ebp-4]              ; restore edi
        mov     esi, dword ptr [ebp-8]              ; restore esi
        mov     ebx, dword ptr [ebp-12]             ; restore ebx

NoCalleeSaveRegisters:

        push    dword ptr [ebp+4]                   ; save the original return address for later
        push    edi
        push    esi

ExtraSpace      = 24
CallersEsi      = 0
CallersEdi      = 4
OrigRetAddr     = 8
pThread         = 20

        lea     edi, [ebp+8+4*eax]                  ; edi = the end of argument destination
        lea     esi, [esp+ExtraSpace+NewArgs+4*ecx] ; esi = the end of argument source

        mov     ebp, dword ptr [ebp]        ; restore ebp (do not use ebp as scratch register to get a good stack trace in debugger)

        test    edx, 2
        jnz     VSDTailCall

        ; copy the arguments to the final destination
        test    ecx, ecx
        jz      ArgumentsCopied
ArgumentCopyLoop:
        ; At this point, this is the value of the registers :
        ; edi = end of argument dest
        ; esi = end of argument source
        ; ecx = nNewStackArgs
        mov     eax, dword ptr [esi-4]
        sub     edi, 4
        sub     esi, 4
        mov     dword ptr [edi], eax
        dec     ecx
        jnz     ArgumentCopyLoop
ArgumentsCopied:

        ; edi = the start of argument destination

        mov     eax, dword ptr [esp+4+4]                    ; return address
        mov     ecx, dword ptr [esp+ExtraSpace+TargetAddr]  ; target address

        mov     dword ptr [edi-4], eax      ; return address
        mov     dword ptr [edi-8], ecx      ; target address

        lea     eax, [edi-8]                ; new value for esp

        pop     esi
        pop     edi
        pop     ecx         ; skip original return address
        pop     edx
        pop     ecx

        mov     esp, eax

        retn                ; Will branch to targetAddr.  This matches the
                            ; "call" done by JITted code, keeping the
                            ; call-ret count balanced.

        ;----------------------------------------------------------------------
VSDTailCall:
        
        ;
        ;


        cmp     dword ptr [esp+OrigRetAddr], JIT_TailCallReturnFromVSD
        jz      VSDTailCallFrameInserted_DoSlideUpArgs ; There is an exiting TailCallFrame that can be reused

        ; try to allocate space for the frame / check whether there is enough space
        ; If there is sufficient space, we will setup the frame and then slide 
        ; the arguments up the stack. Else, we first need to slide the arguments
        ; down the stack to make space for the TailCallFrame
        sub     edi, (SIZEOF_GSCookie + SIZEOF_TailCallFrame)
        cmp     edi, esi
        jae     VSDSpaceForFrameChecked

        ; There is not sufficient space to wedge in the TailCallFrame without 
        ; overwriting the new arguments.
        ; We need to allocate the extra space on the stack, 
        ; and slide down the new arguments
        
        mov     eax, esi
        sub     eax, edi
        sub     esp, eax

        mov     eax, ecx                        ; to subtract the size of arguments
        mov     edx, ecx                        ; for counter

        neg     eax

        ; copy down the arguments to the final destination, need to copy all temporary storage as well
        add     edx, (ExtraSpace+NewArgs)/4

        lea     esi, [esi+4*eax-(ExtraSpace+NewArgs)]
        lea     edi, [edi+4*eax-(ExtraSpace+NewArgs)]

VSDAllocFrameCopyLoop:
        mov     eax, dword ptr [esi]
        mov     dword ptr [edi], eax
        add     esi, 4
        add     edi, 4
        dec     edx
        jnz     VSDAllocFrameCopyLoop

        ; the argument source and destination are same now
        mov     esi, edi

VSDSpaceForFrameChecked:

        ; At this point, we have enough space on the stack for the TailCallFrame,
        ; and we may already have slided down the arguments
        
        mov     eax, _s_gsCookie                ; GetProcessGSCookie()
        mov     dword ptr [edi], eax            ; set GSCookie
        mov     eax, _g_TailCallFrameVptr       ; vptr
        mov     edx, dword ptr [esp+OrigRetAddr]
        mov     dword ptr [edi+SIZEOF_GSCookie], eax            ; TailCallFrame::vptr
        mov     dword ptr [edi+SIZEOF_GSCookie+28], edx         ; TailCallFrame::m_ReturnAddress

        mov     eax, dword ptr [esp+CallersEdi]         ; restored edi
        mov     edx, dword ptr [esp+CallersEsi]         ; restored esi
        mov     dword ptr [edi+SIZEOF_GSCookie+12], eax         ; TailCallFrame::m_regs::edi
        mov     dword ptr [edi+SIZEOF_GSCookie+16], edx         ; TailCallFrame::m_regs::esi
        mov     dword ptr [edi+SIZEOF_GSCookie+20], ebx         ; TailCallFrame::m_regs::ebx
        mov     dword ptr [edi+SIZEOF_GSCookie+24], ebp         ; TailCallFrame::m_regs::ebp

        mov     ebx, dword ptr [esp+pThread]            ; ebx = pThread

        mov     eax, dword ptr [ebx+Thread_m_pFrame]
        lea     edx, [edi+SIZEOF_GSCookie]
        mov     dword ptr [edi+SIZEOF_GSCookie+4], eax          ; TailCallFrame::m_pNext
        mov     dword ptr [ebx+Thread_m_pFrame], edx    ; hook the new frame into the chain

        ; setup ebp chain
        lea     ebp, [edi+SIZEOF_GSCookie+24]                   ; TailCallFrame::m_regs::ebp

        ; Do not copy arguments again if they are in place already
        ; Otherwise, we will need to slide the new arguments up the stack
        cmp     esi, edi
        jne     VSDTailCallFrameInserted_DoSlideUpArgs

        ; At this point, we must have already previously slided down the new arguments,
        ; or the TailCallFrame is a perfect fit
        ; set the caller address
        mov     edx, dword ptr [esp+ExtraSpace+RetAddr] ; caller address
        mov     dword ptr [edi+SIZEOF_GSCookie+8], edx         ; TailCallFrame::m_CallerAddress

        ; adjust edi as it would by copying
        neg     ecx
        lea     edi, [edi+4*ecx]

        jmp     VSDArgumentsCopied

VSDTailCallFrameInserted_DoSlideUpArgs:
        ; set the caller address
        mov     edx, dword ptr [esp+ExtraSpace+RetAddr] ; caller address
        mov     dword ptr [edi+SIZEOF_GSCookie+8], edx          ; TailCallFrame::m_CallerAddress

        ; copy the arguments to the final destination
        test    ecx, ecx
        jz      VSDArgumentsCopied
VSDArgumentCopyLoop:
        mov     eax, dword ptr [esi-4]
        sub     edi, 4
        sub     esi, 4
        mov     dword ptr [edi], eax
        dec     ecx
        jnz     VSDArgumentCopyLoop
VSDArgumentsCopied:

        ; edi = the start of argument destination

        mov     ecx, dword ptr [esp+ExtraSpace+TargetAddr]   ; target address

        mov     dword ptr [edi-4], JIT_TailCallReturnFromVSD ; return address
        mov     dword ptr [edi-12], ecx     ; address of indirection cell
        mov     ecx, [ecx]
        mov     dword ptr [edi-8], ecx      ; target address

        ; skip original return address and saved esi, edi
        add     esp, 12

        pop     edx
        pop     ecx

        lea     esp, [edi-12]   ; new value for esp
        pop     eax

        retn                ; Will branch to targetAddr.  This matches the
                            ; "call" done by JITted code, keeping the
                            ; call-ret count balanced.

JIT_TailCall ENDP


;------------------------------------------------------------------------------


;------------------------------------------------------------------------------

g_SystemInfo            TEXTEQU <?g_SystemInfo@@3U_SYSTEM_INFO@@A>
g_pSyncTable            TEXTEQU <?g_pSyncTable@@3PAVSyncTableEntry@@A>
JIT_MonEnterWorker_Portable	TEXTEQU <@JIT_MonEnterWorker_Portable@4>
JIT_MonTryEnter_Portable        TEXTEQU <@JIT_MonTryEnter_Portable@8>
JIT_MonExitWorker_Portable      TEXTEQU <@JIT_MonExitWorker_Portable@4>
JITutil_MonContention   TEXTEQU <@JITutil_MonContention@4>       
JITutil_MonSignal       TEXTEQU <@JITutil_MonSignal@4>
JIT_InternalThrow       TEXTEQU <@JIT_InternalThrow@4>
EXTRN	g_SystemInfo:BYTE
EXTRN	g_pSyncTable:DWORD
EXTRN	JIT_MonEnterWorker_Portable:PROC
EXTRN	JIT_MonTryEnter_Portable:PROC
EXTRN	JIT_MonExitWorker_Portable:PROC
EXTRN	JITutil_MonContention:PROC
EXTRN	JITutil_MonSignal:PROC
EXTRN	JIT_InternalThrow:PROC

ifdef MON_DEBUG
ifdef TRACK_SYNC
EnterSyncHelper TEXTEQU <_EnterSyncHelper@8>
LeaveSyncHelper TEXTEQU <_LeaveSyncHelper@8>          
EXTRN	EnterSyncHelper:PROC
EXTRN	LeaveSyncHelper:PROC
endif ;TRACK_SYNC
endif ;MON_DEBUG

; The following macro is needed because MASM returns
; "instruction prefix not allowed" error messagre for
; rep nop mnemonic
$repnop MACRO
    db 0F3h
    db 090h
ENDM

; MonStart and MonEnd are used to determine bounds of
; Monitor functions so can patch locks in them

_JIT_MonStart@0 proc public
ret
_JIT_MonStart@0 endp

; Safe ThreadAbort does not abort a thread if it is running finally or has lock counts.
; At the time we call Monitor.Enter, we initiate the abort if we can.
; We do not need to do the same for Monitor.Leave, since most of time, Monitor.Leave is called
; during finally.

;**********************************************************************
; This is a frameless helper for entering a monitor on a object.
; The object is in ARGUMENT_REG1.  This tries the normal case (no
; blocking or object allocation) in line and calls a framed helper
; for the other cases.
; ***** NOTE: if you make any changes to this routine, build with MON_DEBUG undefined
; to make sure you don't break the non-debug build. This is very fragile code.
; Also, propagate the changes to jithelp.s which contains the same helper and assembly code
; (in AT&T syntax) for gnu assembler.
@JIT_MonEnterWorker@4 proc public
        ; Initialize delay value for retry with exponential backoff
        push    ebx
        mov     ebx,50

        ; We need yet another register to avoid refetching the thread object
        push    esi
        
        ; Check if the instance is NULL.
        test    ARGUMENT_REG1, ARGUMENT_REG1
        jz      MonEnterFramedLockHelper
        
        call    _GetThread
        mov     esi,eax
        
        ; Check if we can abort here
        mov     eax, [esi+Thread_m_State]
        and     eax, TS_AbortRequested_ASM
        jz      MonEnterRetryThinLock
        ; go through the slow code path to initiate ThreadAbort.
        jmp     MonEnterFramedLockHelper

MonEnterRetryThinLock: 
        ; Fetch the object header dword
        mov     eax, [ARGUMENT_REG1-SyncBlockIndexOffset_ASM]

        ; Check whether we have the "thin lock" layout, the lock is free and the spin lock bit not set
        ; SBLK_COMBINED_MASK_ASM = BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX + BIT_SBLK_SPIN_LOCK + SBLK_MASK_LOCK_THREADID + SBLK_MASK_LOCK_RECLEVEL
        test    eax, SBLK_COMBINED_MASK_ASM
        jnz     MonEnterNeedMoreTests

        ; Everything is fine - get the thread id to store in the lock
        mov     edx, [esi+Thread_m_ThreadId]

        ; If the thread id is too large, we need a syncblock for sure
        cmp     edx, SBLK_MASK_LOCK_THREADID_ASM
        ja      MonEnterFramedLockHelper

        ; We want to store a new value with the current thread id set in the low 10 bits
        or      edx,eax
        nop
        cmpxchg dword ptr [ARGUMENT_REG1-SyncBlockIndexOffset_ASM], edx
        jnz     MonEnterPrepareToWaitThinLock

        ; Everything went fine and we're done
        add     [esi+Thread_m_dwLockCount],1
        pop     esi
        pop     ebx
        ret

MonEnterNeedMoreTests: 
        ; Ok, it's not the simple case - find out which case it is
        test    eax, BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_ASM
        jnz     MonEnterHaveHashOrSyncBlockIndex

        ; The header is transitioning or the lock - treat this as if the lock was taken
        test    eax, BIT_SBLK_SPIN_LOCK_ASM
        jnz     MonEnterPrepareToWaitThinLock

        ; Here we know we have the "thin lock" layout, but the lock is not free.
        ; It could still be the recursion case - compare the thread id to check
        mov     edx,eax
        and     edx, SBLK_MASK_LOCK_THREADID_ASM
        cmp     edx, [esi+Thread_m_ThreadId]
        jne     MonEnterPrepareToWaitThinLock

        ; Ok, the thread id matches, it's the recursion case.
        ; Bump up the recursion level and check for overflow
        lea     edx, [eax+SBLK_LOCK_RECLEVEL_INC_ASM]
        test    edx, SBLK_MASK_LOCK_RECLEVEL_ASM
        jz      MonEnterFramedLockHelper

        ; Try to put the new recursion level back. If the header was changed in the meantime,
        ; we need a full retry, because the layout could have changed.
        nop
        cmpxchg [ARGUMENT_REG1-SyncBlockIndexOffset_ASM], edx
        jnz     MonEnterRetryHelperThinLock

        ; Everything went fine and we're done
        pop     esi
        pop     ebx
        ret

MonEnterPrepareToWaitThinLock: 
        ; If we are on an MP system, we try spinning for a certain number of iterations
        cmp     dword ptr g_SystemInfo+SYSTEM_INFO_dwNumberOfProcessors,1
        jle     MonEnterFramedLockHelper

        ; exponential backoff: delay by approximately 2*ebx clock cycles (on a PIII)
        mov     eax, ebx
MonEnterdelayLoopThinLock:
        $repnop ; indicate to the CPU that we are spin waiting (useful for some Intel P4 multiprocs)
        dec     eax
        jnz     MonEnterdelayLoopThinLock

        ; next time, wait 3 times as long
        imul    ebx, ebx, 3

        imul    eax, dword ptr g_SystemInfo+SYSTEM_INFO_dwNumberOfProcessors,20000
        cmp     ebx,eax
        jle     MonEnterRetryHelperThinLock

        jmp     MonEnterFramedLockHelper

MonEnterRetryHelperThinLock: 
        jmp     MonEnterRetryThinLock

MonEnterHaveHashOrSyncBlockIndex: 
        ; If we have a hash code already, we need to create a sync block
        test    eax, BIT_SBLK_IS_HASHCODE_ASM
        jnz     MonEnterFramedLockHelper

        ; Ok, we have a sync block index - just and out the top bits and grab the syncblock index
        and     eax, MASK_SYNCBLOCKINDEX_ASM

        ; Get the sync block pointer.
        mov     ARGUMENT_REG2, dword ptr g_pSyncTable
        mov     ARGUMENT_REG2, [ARGUMENT_REG2+eax*SizeOfSyncTableEntry_ASM+SyncTableEntry_m_SyncBlock]

        ; Check if the sync block has been allocated.
        test    ARGUMENT_REG2, ARGUMENT_REG2
        jz      MonEnterFramedLockHelper

        ; Get a pointer to the lock object.
        lea     ARGUMENT_REG2, [ARGUMENT_REG2+SyncBlock_m_Monitor]

        ; Attempt to acquire the lock.
MonEnterRetrySyncBlock: 
        mov     eax, [ARGUMENT_REG2+AwareLock_m_MonitorHeld]
        test    eax,eax
        jne     MonEnterHaveWaiters

        ; Common case, lock isn't held and there are no waiters. Attempt to
        ; gain ownership ourselves.
        mov     ARGUMENT_REG1,1
        nop
        cmpxchg [ARGUMENT_REG2+AwareLock_m_MonitorHeld], ARGUMENT_REG1
        jnz     MonEnterRetryHelperSyncBlock

        ; Success. Save the thread object in the lock and increment the use count.
        mov     dword ptr [ARGUMENT_REG2+AwareLock_m_HoldingThread],esi
        inc     dword ptr [esi+Thread_m_dwLockCount]
        inc     dword ptr [ARGUMENT_REG2+AwareLock_m_Recursion]

ifdef MON_DEBUG
ifdef TRACK_SYNC
        push    ARGUMENT_REG2 ; AwareLock
        push    [esp+4]   ; return address
        call    EnterSyncHelper
endif ;TRACK_SYNC
endif ;MON_DEBUG
        pop     esi
        pop     ebx
        ret

        ; It's possible to get here with waiters but no lock held, but in this
        ; case a signal is about to be fired which will wake up a waiter. So
        ; for fairness sake we should wait too.
        ; Check first for recursive lock attempts on the same thread.
MonEnterHaveWaiters: 
        ; Is mutex already owned by current thread?
        cmp     [ARGUMENT_REG2+AwareLock_m_HoldingThread],esi
        jne     MonEnterPrepareToWait

        ; Yes, bump our use count.
        inc     dword ptr [ARGUMENT_REG2+AwareLock_m_Recursion]
ifdef MON_DEBUG
ifdef TRACK_SYNC
        push    ARGUMENT_REG2 ; AwareLock
        push    [esp+4]   ; return address
        call    EnterSyncHelper
endif ;TRACK_SYNC        
endif ;MON_DEBUG
        pop     esi
        pop     ebx
        ret

MonEnterPrepareToWait: 
        ; If we are on an MP system, we try spinning for a certain number of iterations
        cmp     dword ptr g_SystemInfo+SYSTEM_INFO_dwNumberOfProcessors,1
        jle     MonEnterHaveWaiters1

        ; exponential backoff: delay by approximately 2*ebx clock cycles (on a PIII)
        mov     eax,ebx
MonEnterdelayLoop:
        $repnop ; indicate to the CPU that we are spin waiting (useful for some Intel P4 multiprocs)
        dec     eax
        jnz     MonEnterdelayLoop

        ; next time, wait 3 times as long
        imul    ebx,ebx,3

        imul    eax, dword ptr g_SystemInfo+SYSTEM_INFO_dwNumberOfProcessors,20000
        cmp     ebx,eax
        jle     MonEnterRetrySyncBlock

MonEnterHaveWaiters1: 

        pop     esi
        pop     ebx

        ; Place AwareLock in arg1 then call contention helper.
        mov     ARGUMENT_REG1, ARGUMENT_REG2
        jmp     JITutil_MonContention

MonEnterRetryHelperSyncBlock: 
        jmp     MonEnterRetrySyncBlock

        ; ECX has the object to synchronize on
MonEnterFramedLockHelper: 
        pop     esi
        pop     ebx
        jmp     JIT_MonEnterWorker_Portable

@JIT_MonEnterWorker@4 endp

;************************************************************************
; This is a frameless helper for trying to enter a monitor on a object.
; The object is in ARGUMENT_REG1 and a timeout in ARGUMENT_REG2. This tries the
; normal case (no object allocation) in line and calls a framed helper for the
; other cases.
; ***** NOTE: if you make any changes to this routine, build with MON_DEBUG undefined
; to make sure you don't break the non-debug build. This is very fragile code.
; Also, propagate the changes to jithelp.s which contains the same helper and assembly code
; (in AT&T syntax) for gnu assembler.
@JIT_MonTryEnter@8 proc public
        ; Save the timeout parameter.
        push    ARGUMENT_REG2

        ; Initialize delay value for retry with exponential backoff
        push    ebx
        mov     ebx, 50

        ; The thin lock logic needs another register to store the thread
        push    esi
        
        ; Check if the instance is NULL.
        test    ARGUMENT_REG1, ARGUMENT_REG1
        jz      MonTryEnterFramedLockHelper

        ; Check if the timeout looks valid
        cmp     ARGUMENT_REG2,-1
        jl      MonTryEnterFramedLockHelper

        ; Get the thread right away, we'll need it in any case
        call    _GetThread
        mov     esi,eax

        ; Check if we can abort here
        mov     eax, [esi+Thread_m_State]
        and     eax, TS_AbortRequested_ASM
        jz      MonTryEnterRetryThinLock
        ; go through the slow code path to initiate ThreadAbort.
        jmp     MonTryEnterFramedLockHelper

MonTryEnterRetryThinLock: 
        ; Get the header dword and check its layout
        mov     eax, [ARGUMENT_REG1-SyncBlockIndexOffset_ASM]

        ; Check whether we have the "thin lock" layout, the lock is free and the spin lock bit not set
        ; SBLK_COMBINED_MASK_ASM = BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX + BIT_SBLK_SPIN_LOCK + SBLK_MASK_LOCK_THREADID + SBLK_MASK_LOCK_RECLEVEL
        test    eax, SBLK_COMBINED_MASK_ASM
        jnz     MonTryEnterNeedMoreTests

        ; Ok, everything is fine. Fetch the thread id and make sure it's small enough for thin locks
        mov     edx, [esi+Thread_m_ThreadId]
        cmp     edx, SBLK_MASK_LOCK_THREADID_ASM
        ja      MonTryEnterFramedLockHelper

        ; Try to put our thread id in there
        or      edx,eax
        nop
        cmpxchg [ARGUMENT_REG1-SyncBlockIndexOffset_ASM],edx
        jnz     MonTryEnterRetryHelperThinLock

        ; Got the lock - everything is fine"
        add     [esi+Thread_m_dwLockCount],1
        pop     esi

        ; Delay value no longer needed
        pop     ebx

        ; Timeout parameter not needed, ditch it from the stack.
        add     esp,4

        mov     eax,1
        ret

MonTryEnterNeedMoreTests: 
        ; Ok, it's not the simple case - find out which case it is
        test    eax, BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_ASM
        jnz     MonTryEnterHaveSyncBlockIndexOrHash

        ; The header is transitioning or the lock is taken
        test    eax, BIT_SBLK_SPIN_LOCK_ASM
        jnz     MonTryEnterRetryHelperThinLock

        mov     edx, eax
        and     edx, SBLK_MASK_LOCK_THREADID_ASM
        cmp     edx, [esi+Thread_m_ThreadId]
        jne     MonTryEnterPrepareToWaitThinLock

        ; Ok, the thread id matches, it's the recursion case.
        ; Bump up the recursion level and check for overflow
        lea     edx, [eax+SBLK_LOCK_RECLEVEL_INC_ASM]
        test    edx, SBLK_MASK_LOCK_RECLEVEL_ASM
        jz      MonTryEnterFramedLockHelper

        ; Try to put the new recursion level back. If the header was changed in the meantime,
        ; we need a full retry, because the layout could have changed.
        nop
        cmpxchg [ARGUMENT_REG1-SyncBlockIndexOffset_ASM],edx
        jnz     MonTryEnterRetryHelperThinLock

        ; Everything went fine and we're done
        pop     esi
        pop     ebx

        ; Timeout parameter not needed, ditch it from the stack.
        add     esp, 4

        mov     eax, 1
        ret

MonTryEnterPrepareToWaitThinLock:
        ; If we are on an MP system, we try spinning for a certain number of iterations
        cmp     dword ptr g_SystemInfo+SYSTEM_INFO_dwNumberOfProcessors,1
        jle     MonTryEnterFramedLockHelper

        ; exponential backoff: delay by approximately 2*ebx clock cycles (on a PIII)
        mov     eax, ebx
MonTryEnterdelayLoopThinLock:
        $repnop ; indicate to the CPU that we are spin waiting (useful for some Intel P4 multiprocs)
        dec     eax
        jnz     MonTryEnterdelayLoopThinLock

        ; next time, wait 3 times as long
        imul    ebx, ebx, 3

        imul    eax, dword ptr g_SystemInfo+SYSTEM_INFO_dwNumberOfProcessors,20000
        cmp     ebx, eax
        jle     MonTryEnterRetryHelperThinLock

        jmp     MonTryEnterWouldBlock

MonTryEnterRetryHelperThinLock: 
        jmp     MonTryEnterRetryThinLock


MonTryEnterHaveSyncBlockIndexOrHash: 
        ; If we have a hash code already, we need to create a sync block
        test    eax, BIT_SBLK_IS_HASHCODE_ASM
        jnz     MonTryEnterFramedLockHelper

        ; Just and out the top bits and grab the syncblock index
        and     eax, MASK_SYNCBLOCKINDEX_ASM

        ; Get the sync block pointer.
        mov     ARGUMENT_REG2, dword ptr g_pSyncTable
        mov     ARGUMENT_REG2, [ARGUMENT_REG2+eax*SizeOfSyncTableEntry_ASM+SyncTableEntry_m_SyncBlock]

        ; Check if the sync block has been allocated.
        test    ARGUMENT_REG2, ARGUMENT_REG2
        jz      MonTryEnterFramedLockHelper

        ; Get a pointer to the lock object.
        lea     ARGUMENT_REG2, [ARGUMENT_REG2+SyncBlock_m_Monitor]        

MonTryEnterRetrySyncBlock: 
        ; Attempt to acquire the lock.
        mov     eax, [ARGUMENT_REG2+AwareLock_m_MonitorHeld]
        test    eax,eax
        jne     MonTryEnterHaveWaiters

        ; We need another scratch register for what follows, so save EBX now so"
        ; we can use it for that purpose."
        push    ebx

        ; Common case, lock isn't held and there are no waiters. Attempt to
        ; gain ownership ourselves.
        mov     ebx,1
        nop
        cmpxchg [ARGUMENT_REG2+AwareLock_m_MonitorHeld],ebx

        pop     ebx
        
        jnz     MonTryEnterRetryHelperSyncBlock

        ; Success. Save the thread object in the lock and increment the use count.
        mov     dword ptr [ARGUMENT_REG2+AwareLock_m_HoldingThread],esi
        inc     dword ptr [ARGUMENT_REG2+AwareLock_m_Recursion]        
        inc     dword ptr [esi+Thread_m_dwLockCount]

ifdef MON_DEBUG
ifdef TRACK_SYNC
        push    ARGUMENT_REG2 ; AwareLock
        push    [esp+4]   ; return address
        call    EnterSyncHelper
endif ;TRACK_SYNC        
endif ;MON_DEBUG

        pop     esi
        pop     ebx

        ; Timeout parameter not needed, ditch it from the stack."
        add     esp,4

        mov     eax,1
        ret

        ; It's possible to get here with waiters but no lock held, but in this
        ; case a signal is about to be fired which will wake up a waiter. So
        ; for fairness sake we should wait too.
        ; Check first for recursive lock attempts on the same thread.
MonTryEnterHaveWaiters: 
        ; Is mutex already owned by current thread?
        cmp     [ARGUMENT_REG2+AwareLock_m_HoldingThread],esi
        jne     MonTryEnterPrepareToWait

        ; Yes, bump our use count.
        inc     dword ptr [ARGUMENT_REG2+AwareLock_m_Recursion]
ifdef MON_DEBUG
ifdef TRACK_SYNC
        push    ARGUMENT_REG2 ; AwareLock
        push    [esp+4]   ; return address
        call    EnterSyncHelper
endif ;TRACK_SYNC        
endif ;MON_DEBUG
        pop     esi
        pop     ebx

        ; Timeout parameter not needed, ditch it from the stack.
        add     esp,4

        mov     eax,1
        ret

MonTryEnterPrepareToWait:
        ; If we are on an MP system, we try spinning for a certain number of iterations
        cmp     dword ptr g_SystemInfo+SYSTEM_INFO_dwNumberOfProcessors,1
        jle     MonTryEnterWouldBlock

        ; exponential backoff: delay by approximately 2*ebx clock cycles (on a PIII)
        mov     eax, ebx
MonTryEnterdelayLoop:
        $repnop ; indicate to the CPU that we are spin waiting (useful for some Intel P4 multiprocs)
        dec     eax
        jnz     MonTryEnterdelayLoop

        ; next time, wait 3 times as long
        imul    ebx, ebx, 3

        imul    eax, dword ptr g_SystemInfo+SYSTEM_INFO_dwNumberOfProcessors,20000
        cmp     ebx, eax
        jle     MonTryEnterRetrySyncBlock

        ; We would need to block to enter the section. Return failure if
        ; timeout is zero, else call the framed helper to do the blocking
        ; form of TryEnter."
MonTryEnterWouldBlock: 
        pop     esi
        pop     ebx
        pop     ARGUMENT_REG2
        test    ARGUMENT_REG2, ARGUMENT_REG2
        jnz     MonTryEnterBlock
        xor     eax,eax
        ret

MonTryEnterRetryHelperSyncBlock: 
        jmp     MonTryEnterRetrySyncBlock

MonTryEnterFramedLockHelper: 
        ; ARGUMENT_REG1 has the object to synchronize on, must retrieve the
        ; timeout parameter from the stack.
        pop     esi
        pop     ebx
        pop     ARGUMENT_REG2
MonTryEnterBlock:        
        jmp     JIT_MonTryEnter_Portable

@JIT_MonTryEnter@8 endp

;**********************************************************************
; This is a frameless helper for exiting a monitor on a object.
; The object is in ARGUMENT_REG1.  This tries the normal case (no
; blocking or object allocation) in line and calls a framed helper
; for the other cases.
; ***** NOTE: if you make any changes to this routine, build with MON_DEBUG undefined
; to make sure you don't break the non-debug build. This is very fragile code.
; Also, propagate the changes to jithelp.s which contains the same helper and assembly code
; (in AT&T syntax) for gnu assembler.
@JIT_MonExitWorker@4 proc public
        push    esi
        
        ; Check if the instance is NULL.
        test    ARGUMENT_REG1, ARGUMENT_REG1
        jz      MonExitFramedLockHelper
        
        call    _GetThread
        mov     esi,eax

MonExitRetryThinLock: 
        ; Fetch the header dword and check its layout and the spin lock bit
        mov     eax, [ARGUMENT_REG1-SyncBlockIndexOffset_ASM]
        ;BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_SPIN_LOCK_ASM = BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX + BIT_SBLK_SPIN_LOCK
        test    eax, BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_SPIN_LOCK_ASM
        jnz     MonExitNeedMoreTests

        ; Ok, we have a "thin lock" layout - check whether the thread id matches
        mov     edx,eax
        and     edx, SBLK_MASK_LOCK_THREADID_ASM
        cmp     edx, [esi+Thread_m_ThreadId]
        jne     MonExitFramedLockHelper

        ; Check the recursion level
        test    eax, SBLK_MASK_LOCK_RECLEVEL_ASM
        jne     MonExitDecRecursionLevel

        ; It's zero - we're leaving the lock.
        ; So try to put back a zero thread id.
        ; edx and eax match in the thread id bits, and edx is zero elsewhere, so the xor is sufficient
        xor     edx,eax
        nop
        cmpxchg [ARGUMENT_REG1-SyncBlockIndexOffset_ASM],edx
        jnz     MonExitRetryHelperThinLock

        ; We're done
        sub     [esi+Thread_m_dwLockCount],1
        pop     esi
        ret

MonExitDecRecursionLevel: 
        lea     edx, [eax-SBLK_LOCK_RECLEVEL_INC_ASM]
        nop
        cmpxchg [ARGUMENT_REG1-SyncBlockIndexOffset_ASM],edx
        jnz     MonExitRetryHelperThinLock

        ; We're done
        pop     esi
        ret

MonExitNeedMoreTests:
        ;Forward all special cases to the slow helper
        ;BIT_SBLK_IS_HASHCODE_OR_SPIN_LOCK_ASM = BIT_SBLK_IS_HASHCODE + BIT_SBLK_SPIN_LOCK
        test    eax, BIT_SBLK_IS_HASHCODE_OR_SPIN_LOCK_ASM
        jnz     MonExitFramedLockHelper

        ; Get the sync block index and use it to compute the sync block pointer
        mov     ARGUMENT_REG2, dword ptr g_pSyncTable
        and     eax, MASK_SYNCBLOCKINDEX_ASM
        mov     ARGUMENT_REG2, [ARGUMENT_REG2+eax*SizeOfSyncTableEntry_ASM+SyncTableEntry_m_SyncBlock]        

        ; was there a sync block?
        test    ARGUMENT_REG2, ARGUMENT_REG2
        jz      MonExitFramedLockHelper

        ; Get a pointer to the lock object.
        lea     ARGUMENT_REG2, [ARGUMENT_REG2+SyncBlock_m_Monitor]

        ; Check if lock is held.
        cmp     [ARGUMENT_REG2+AwareLock_m_HoldingThread],esi
        jne     MonExitFramedLockHelper

ifdef MON_DEBUG
ifdef TRACK_SYNC
        push    ARGUMENT_REG1 ; preserve regs
        push    ARGUMENT_REG2

        push    ARGUMENT_REG2 ; AwareLock
        push    [esp+8]       ; return address
        call    LeaveSyncHelper

        pop     ARGUMENT_REG2 ; restore regs
        pop     ARGUMENT_REG1
endif ;TRACK_SYNC        
endif ;MON_DEBUG
        ; Reduce our recursion count.
        dec     dword ptr [ARGUMENT_REG2+AwareLock_m_Recursion]
        jz      MonExitLastRecursion

        pop     esi
        ret

MonExitRetryHelperThinLock: 
        jmp     MonExitRetryThinLock

MonExitFramedLockHelper: 
        pop     esi
        jmp     JIT_MonExitWorker_Portable

        ; This is the last count we held on this lock, so release the lock.
MonExitLastRecursion: 
        dec     dword ptr [esi+Thread_m_dwLockCount]
        mov     dword ptr [ARGUMENT_REG2+AwareLock_m_HoldingThread],0

MonExitRetry: 
        mov     eax, [ARGUMENT_REG2+AwareLock_m_MonitorHeld]
        lea     esi, [eax-1]
        nop
        cmpxchg [ARGUMENT_REG2+AwareLock_m_MonitorHeld], esi
        jne     MonExitRetryHelper        
        pop     esi        
        test    eax,0FFFFFFFEh
        jne     MonExitMustSignal

        ret

MonExitMustSignal:
        mov     ARGUMENT_REG1, ARGUMENT_REG2
        jmp     JITutil_MonSignal

MonExitRetryHelper: 
        jmp     MonExitRetry

@JIT_MonExitWorker@4 endp

;**********************************************************************
; This is a frameless helper for entering a static monitor on a class.
; The methoddesc is in ARGUMENT_REG1.  This tries the normal case (no
; blocking or object allocation) in line and calls a framed helper
; for the other cases.
; Note we are changing the methoddesc parameter to a pointer to the
; AwareLock.
; ***** NOTE: if you make any changes to this routine, build with MON_DEBUG undefined
; to make sure you don't break the non-debug build. This is very fragile code.
; Also, propagate the changes to jithelp.s which contains the same helper and assembly code
; (in AT&T syntax) for gnu assembler.
@JIT_MonEnterStatic@4 proc public
        ; We need another scratch register for what follows, so save EBX now so
        ; we can use it for that purpose.
        push    ebx

        ; Attempt to acquire the lock
MonEnterStaticRetry: 
        mov     eax, [ARGUMENT_REG1+AwareLock_m_MonitorHeld]
        test    eax,eax
        jne     MonEnterStaticHaveWaiters

        ; Common case, lock isn't held and there are no waiters. Attempt to
        ; gain ownership ourselves.
        mov     ebx,1
        nop
        cmpxchg [ARGUMENT_REG1+AwareLock_m_MonitorHeld],ebx
        jnz     MonEnterStaticRetryHelper

        pop     ebx

        ; Success. Save the thread object in the lock and increment the use count.
        call    _GetThread
        mov     [ARGUMENT_REG1+AwareLock_m_HoldingThread], eax
        inc     dword ptr [ARGUMENT_REG1+AwareLock_m_Recursion]
        inc     dword ptr [eax+Thread_m_dwLockCount]

ifdef MON_DEBUG
ifdef TRACK_SYNC
        push    ARGUMENT_REG1   ; AwareLock
        push    [esp+4]         ; return address
        call    EnterSyncHelper
endif ;TRACK_SYNC
endif ;MON_DEBUG
        ret

        ; It's possible to get here with waiters but no lock held, but in this
        ; case a signal is about to be fired which will wake up a waiter. So
        ; for fairness sake we should wait too.
        ; Check first for recursive lock attempts on the same thread.
MonEnterStaticHaveWaiters: 
        ; Get thread but preserve EAX (contains cached contents of m_MonitorHeld).
        push    eax
        call    _GetThread
        mov     ebx,eax
        pop     eax

        ; Is mutex already owned by current thread?
        cmp     [ARGUMENT_REG1+AwareLock_m_HoldingThread],ebx
        jne     MonEnterStaticPrepareToWait

        ; Yes, bump our use count.
        inc     dword ptr [ARGUMENT_REG1+AwareLock_m_Recursion]
ifdef MON_DEBUG
ifdef TRACK_SYNC
        push    ARGUMENT_REG1   ; AwareLock
        push    [esp+4]         ; return address
        call    EnterSyncHelper
endif ;TRACK_SYNC
endif ;MON_DEBUG
        pop     ebx
        ret

MonEnterStaticPrepareToWait: 
        pop     ebx

        ; ARGUMENT_REG1 should have AwareLock. Call contention helper.
        jmp     JITutil_MonContention

MonEnterStaticRetryHelper: 
        jmp     MonEnterStaticRetry
@JIT_MonEnterStatic@4 endp

;**********************************************************************
; A frameless helper for exiting a static monitor on a class.
; The methoddesc is in ARGUMENT_REG1.  This tries the normal case (no
; blocking or object allocation) in line and calls a framed helper
; for the other cases.
; Note we are changing the methoddesc parameter to a pointer to the
; AwareLock.
; ***** NOTE: if you make any changes to this routine, build with MON_DEBUG undefined
; to make sure you don't break the non-debug build. This is very fragile code.
; Also, propagate the changes to jithelp.s which contains the same helper and assembly code
; (in AT&T syntax) for gnu assembler.
@JIT_MonExitStatic@4 proc public

ifdef MON_DEBUG
ifdef TRACK_SYNC
        push    ARGUMENT_REG1   ; preserve regs

        push    ARGUMENT_REG1   ; AwareLock
        push    [esp+8]         ; return address
        call    LeaveSyncHelper

        pop     [ARGUMENT_REG1] ; restore regs
endif ;TRACK_SYNC
endif ;MON_DEBUG

        ; Check if lock is held.
        call    _GetThread
        cmp     [ARGUMENT_REG1+AwareLock_m_HoldingThread],eax
        jne     MonExitStaticLockError

        ; Reduce our recursion count.
        dec     dword ptr [ARGUMENT_REG1+AwareLock_m_Recursion]
        jz      MonExitStaticLastRecursion

        ret

        ; This is the last count we held on this lock, so release the lock.
MonExitStaticLastRecursion: 
        ; eax must have the thread object
        dec     dword ptr [eax+Thread_m_dwLockCount]
        mov     dword ptr [ARGUMENT_REG1+AwareLock_m_HoldingThread],0
        push    ebx

MonExitStaticRetry: 
        mov     eax, [ARGUMENT_REG1+AwareLock_m_MonitorHeld]
        lea     ebx, [eax-1]
        nop
        cmpxchg [ARGUMENT_REG1+AwareLock_m_MonitorHeld],ebx
        jne     MonExitStaticRetryHelper
        pop     ebx
        test    eax,0FFFFFFFEh
        jne     MonExitStaticMustSignal

        ret

MonExitStaticMustSignal: 
        jmp     JITutil_MonSignal

MonExitStaticRetryHelper: 
        jmp     MonExitStaticRetry
        ; Throw a synchronization lock exception.
MonExitStaticLockError: 
        mov     ARGUMENT_REG1, CORINFO_SynchronizationLockException_ASM
        jmp     JIT_InternalThrow

@JIT_MonExitStatic@4 endp


;**********************************************************************
; PrecodeRemotingThunk is patched at runtime with the address of TransparentProxy.
; Including it in the range for JIT_MonXXX helpers will make it share the same dirty page with them.

        ALIGN 16
_PrecodeRemotingThunk@0 proc public

        test    ecx,ecx
        jz      RemotingDone            ; predicted not taken

        cmp     dword ptr [ecx],11111111h ; This is going to be patched to address of the transparent proxy
        je      RemotingCheck           ; predicted not taken

RemotingDone:
        ret

RemotingCheck:
        push     eax            ; save method desc
        mov      eax, dword ptr [ecx + TP_OFFSET_STUBDATA]
        call     [ecx + TP_OFFSET_STUB]
        test     eax, eax
        jnz      RemotingCtxMismatch
        pop      eax            ; throw away method desc
        jmp      RemotingDone

RemotingCtxMismatch:
        pop      eax            ; restore method desc
        mov      [esp],eax      ; replace return address into the precode with method desc (argument for TP stub)
        jmp      [_g_dwOOContextAddr]

_PrecodeRemotingThunk@0 endp

_JIT_MonLast@0 proc public
ret
_JIT_MonLast@0 endp

; This is the first function outside the "keep together range". Used by BBT scripts.
_JIT_MonEnd@0 proc public
ret
_JIT_MonEnd@0 endp

    end
