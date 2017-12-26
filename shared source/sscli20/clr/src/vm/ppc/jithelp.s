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

// This contains JITinterface routines that are 100% PowerPC assembly

#include "asmconstants.h"


// mnemonics for the PowerPC registers
#define sp      r1

        // The stack on entry:
        // sp + 16 + 24 -- Arguments
        // sp + 12 + 24 -- sizeofCallerSpillArea (not used)
        // sp + 8  + 24 -- sizeofTargetArguments
        // sp + 4  + 24 -- flags (not used)
        // sp + 0  + 24 -- target address

ASMFUNC(JIT_TailCall)
        #define LINKAGE_AREA 24
        #define NumGPRegisters 8
        #define TargetSizeOff 8
        #define TargetArgsOff 16
        #define TargetAddrOff 0

        // sp  + 16 - arguments to target function
        // r30  + 24 - spill area of the callee
        // r12 - amount left to copy
        lwz   r12, TargetSizeOff(r1)     // r12 = sizeofTargetArguments
        cmpwi r12, NumGPRegisters+1      // Skip first 8 words of arguments
        blt   LOCAL_LABEL(ArgCopyDone)
        addi  r12, r12, -NumGPRegisters
        mtctr r12                           // initalize the counter register
        addi  r12, sp, LINKAGE_AREA + TargetArgsOff + NumGPRegisters*4  // pointer to arguments
        addi  r11, r30, LINKAGE_AREA + NumGPRegisters*4   // pointer to spill area
LOCAL_LABEL(ArgCopyLoop):
        lwz   r0, 0(r12)
        addi  r12, r12, 4
        stw   r0, 0(r11)
        addi  r11, r11, 4
        bdnz  LOCAL_LABEL(ArgCopyLoop)
LOCAL_LABEL(ArgCopyDone):

        // Move the address of the target function into the counter register
        lwz   r12, TargetAddrOff(sp)
        mtctr r12

        // Restore the state of the processor
        lwz   r29, -8(r30) // callee saved register
        lwz   r0,   8(r30) // restore the link register
        mtlr  r0
        or    sp, r30, r30 // sp = fp
        lwz   r30, -4(sp)  // fp = old_fp

        // Branch to target
        bctr

        #undef LINKAGE_AREA
        #undef NumGPRegisters
        #undef TargetSizeOff
        #undef TargetArgsOff
        #undef TargetAddrOff
