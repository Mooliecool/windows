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
// Desription:
//
//   Implementation of DBG_FlushInstructionCache, which flushes
//   the CPU cache after executable code has been modified.
//

// mnemonics for the PowerPC registers 
#define a0      r3   // First arg register.  This is also the return-value location 
#define a1      r4 
#define a2      r5
#define t0      r2
#define t1      r11

// a handy macro for declaring a public function 
.macro ASMFUNC 
  .align 2 
  .globl _$0 
_$0: 
.endmacro 

.data
// extern int CacheLineSize;
    .non_lazy_symbol_pointer
    L_CacheLineSize$non_lazy_ptr:
        .indirect_symbol _CacheLineSize
        .long 0

.text


/*++
Function:
  DBG_FlushInstructionCache

Parameters:

lpBaseAddress
       [in] Start of memory region to flush
dwSize
       [in] Length of region to flush, in bytes

Return Values:

BOOL - TRUE for success, FALSE for failure

--*/
	
ASMFUNC DBG_FlushInstructionCache
    mflr r0         // save the return address in r0
    bl 0f
0:
    mflr t1         // move link register into t1
    mtlr r0         // restore the return address back into the lr
    addis a2, t1, ha16(L_CacheLineSize$non_lazy_ptr-0b)
    lwz  a2, lo16(L_CacheLineSize$non_lazy_ptr-0b)(a2)
    lwz  a2, 0(a2)  // a2 = CacheLineSize

    add t0, a1, a0 // t0 = lpBaseAddress+dwSize
    
    neg t1, a2
    and a0, a0, t1 // a0 = lpBaseAddress&~-CacheLineSize
    
    mr a1, a0
    
    // This is based on the instruction sequence recommended 
    // in chapter "Self-Modifying Code" of "MPC7410 RISC 
    // Microprocessor User's Manual"

1:
    dcbst 0, a0     // update memory
    add  a0, a0, a2 // a0+=CacheLineSize
    cmplw a0, t0
    blt  1b         // brif a0 < dwSize
    sync            // wait for the update

2:
    icbi 0,  a1     // invalidate instruction cache
    add  a1, a1, a2 // a1+=CacheLineSize
    cmplw a1, t0
    blt  2b         // brif a1 < dwSize
    sync            // wait for the update

    isync           // discard any prefetched instructions
    li   a0, 1      // return 1 for TRUE
    blr
