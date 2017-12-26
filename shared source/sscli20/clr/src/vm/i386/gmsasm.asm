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
;             gmsasm.s in this directory                            

	.586
	.model	flat

include asmconstants.inc

	option	casemap:none
	.code

; int __fastcall LazyMachStateCaptureState(struct LazyMachState *pState);
@LazyMachStateCaptureState@4 proc public
        mov [ecx+MachState__pRetAddr], 0 ; marks that this is not yet valid
        mov [ecx+MachState__edi], edi    ; remember register values
	mov [ecx+MachState__esi], esi 
        mov [ecx+MachState__ebx], ebx
	mov [ecx+LazyMachState_captureEbp], ebp
	mov [ecx+LazyMachState_captureEsp], esp

        mov eax, [esp]                   ; capture return address
	mov [ecx+LazyMachState_captureEip], eax
	xor eax, eax
	retn
@LazyMachStateCaptureState@4 endp

end
