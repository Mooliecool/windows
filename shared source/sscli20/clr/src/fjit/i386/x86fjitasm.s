//  ==++==
//
//    
//     Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//    
//     The use and distribution terms for this software are contained in the file
//     named license.txt, which can be found in the root of this distribution.
//     By using this software in any fashion, you are agreeing to be bound by the
//     terms of this license.
//    
//     You must not remove this notice, or any other, from this software.
//    
//
//  ==--==
//
//   *** NOTE:    If you make changes to this file, propagate the changes to
//                x86fjitasm.s in this directory
//
//  This file uses AT&T i386 syntax. Search web for "AT&T Syntax versus Intel Syntax" 
//  to get document describing differences between the AT&T i386 syntax and the Intel 
//  syntax used by masm.

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

//  void __stdcall SWITCH_helper(void)
ASMFUNC(STDMANGLE(SWITCH_helper,0))
        pop   %eax      // return address
        pop   %ecx      // limit
        pop   %edx      // index
        push  %eax
        cmp   %ecx, %edx
        jbe   LRangeOK
        mov   %ecx, %edx
LRangeOK:
        lea   0x2(%edx,%edx,4), %edx // +2 is the size of the "jmp eax"
                                // instruction just before the switch table
                                // this is being done only for the
                                // convenience of the debugger, which
                                // currently cannot handle functions that do
                                // a jmp out.
        add   %edx, %eax        // since eax+edx*5 is not allowed
        ret
ASMFUNCEND()
