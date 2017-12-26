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
//             gmsasm.asm in this directory
//
//  This file uses AT&T i386 syntax. Search web for "AT&T Syntax versus Intel Syntax" 
//  to get document describing differences between the AT&T i386 syntax and the Intel 
//  syntax used by masm.

#include "asmconstants.h"

.text

#ifdef PLATFORM_UNIX
#define STDMANGLE(name,args) name
#else
#define STDMANGLE(name,args) _##name##@##args
#endif

// a handy macro for declaring a function
#define ASMFUNC(name)             \
        .globl name             ; \
name: ;

#define ASMFUNCEND()

// int __stdcall LazyMachStateCaptureState(struct LazyMachState *pState);
ASMFUNC(STDMANGLE(LazyMachStateCaptureState,4))
        mov     4(%esp), %ecx           //  get pState into ecx
        movl    $0, MachState__pRetAddr(%ecx) // marks that this is not yet valid
        mov     %edi,MachState__edi(%ecx) // remember register values
        mov     %esi,MachState__esi(%ecx)
        mov     %ebx,MachState__ebx(%ecx)
        mov     %ebp,LazyMachState_captureEbp(%ecx)
        lea     4(%esp), %eax           // capture esp, but simulate the
                                        // pop of the pState arg, to be
                                        // compatible with the __fastcall
                                        // Win32 version of this function
        mov     %eax, LazyMachState_captureEsp(%ecx)

        mov     (%esp), %eax            // capture return address
        mov     %eax, LazyMachState_captureEip(%ecx)
        xor     %eax, %eax
        ret     $4
ASMFUNCEND()
