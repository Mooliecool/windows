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
//   Implementation of Interlocked functions for the PowerPC
//   platform. These functions are processor dependent.
//

.text
	
// mnemonics for the PowerPC registers 
#define a0      r3   // First arg register.  This is also the return-value location 
#define a1      r4 
#define a2      r5 
#define a3      r6 

// a handy macro for declaring a public function 
.macro ASMFUNC 
  .align 2 
  .globl _$0 
_$0: 
.endmacro 

/*++
Function:
  InterlockedIncrement

The InterlockedIncrement function increments (increases by one) the
value of the specified variable and checks the resulting value. The
function prevents more than one thread from using the same variable
simultaneously.

Parameters

lpAddend 
       [in/out] Pointer to the variable to increment. 

Return Values

The return value is the resulting incremented value. 

--*/
	
ASMFUNC InterlockedIncrement
    sync
    lwarx  a1, 0,  a0 
    addi   a1, a1, 1 
    stwcx. a1, 0,  a0 
    bne- _InterlockedIncrement
    sync
    mr a0,a1
    blr

/*++
Function:
  InterlockedDecrement

The InterlockedDecrement function decrements (decreases by one) the
value of the specified variable and checks the resulting value. The
function prevents more than one thread from using the same variable
simultaneously.

Parameters

lpAddend 
       [in/out] Pointer to the variable to decrement. 

Return Values

The return value is the resulting decremented value.

--*/    
ASMFUNC InterlockedDecrement
    sync
    lwarx a1, 0, a0
    addi a1, a1,-1
    stwcx. a1, 0,a0 
    bne- _InterlockedDecrement
    sync
    mr a0,a1
    blr

/*++
Function:
  InterlockedExchange

The InterlockedExchange function atomically exchanges a pair of
values. The function prevents more than one thread from using the same
variable simultaneously.

Parameters

Target 
       [in/out] Pointer to the value to exchange. The function sets
       this variable to Value, and returns its prior value.
Value 
       [in] Specifies a new value for the variable pointed to by Target. 

Return Values

The function returns the initial value pointed to by Target. 

--*/    
ASMFUNC InterlockedExchange
    sync
    lwarx a2,0,a0
    stwcx. a1,0,a0
    bne- _InterlockedExchange
    sync
    mr a0,a2
    blr

/*++
Function:
  InterlockedCompareExchange

The InterlockedCompareExchange function performs an atomic comparison
of the specified values and exchanges the values, based on the outcome
of the comparison. The function prevents more than one thread from
using the same variable simultaneously.

If you are exchanging pointer values, this function has been
superseded by the InterlockedCompareExchangePointer function.

Parameters

Destination     [in/out] Specifies the address of the destination value. The sign is ignored. 
Exchange        [in]     Specifies the exchange value. The sign is ignored. 
Comperand       [in]     Specifies the value to compare to Destination. The sign is ignored. 

Return Values

The return value is the initial value of the destination.

--*/
	   
ASMFUNC InterlockedCompareExchange  
    sync
    lwarx a3, 0, a0
    cmpw a3,a2
    bne- ContW
    stwcx. a1,0,a0
    bne- _InterlockedCompareExchange
ContW: 
    sync
    mr a0,a3
    blr
    
ASMFUNC MemoryBarrier
    sync
    blr

ASMFUNC YieldProcessor
    blr

#define CONTEXT_Gpr0     (33*8)
#define CONTEXT_Cr    (CONTEXT_Gpr0+4*32)
#define CONTEXT_Xer   (CONTEXT_Gpr0+4*33)
#define CONTEXT_Msr   (CONTEXT_Gpr0+4*34)
#define CONTEXT_Iar   (CONTEXT_Gpr0+4*35)
#define CONTEXT_Lr    (CONTEXT_Gpr0+4*36)
#define CONTEXT_Ctr   (CONTEXT_Gpr0+4*37)

/*++
Function:
  CONTEXT_CaptureContext

Captures the current thread's registers into a CONTEXT*.

Parameters

lpContext               [out]    Pointer to the CONTEXT record
fCaptureFloatingPoint   [in]     BOOL if the floating-point regs should be saved

Return Values

None

--*/
ASMFUNC CONTEXT_CaptureContext
    stmw r0, CONTEXT_Gpr0(a0)    // store all 32 Gprs into the CONTEXT
    mflr r0                      // capture the link register value
    bl 0f                        // and branch to determine our address
0:
    mfcr  r2
    stw  r2, CONTEXT_Cr(a0)
    mfxer r2
    stw  r2, CONTEXT_Xer(a0)
    addi r2, 0, 0                // "mfmsr r2" is privileged - just report zero
    stw  r2, CONTEXT_Msr(a0)
    mflr r2                      // get the address following the 'bl'
    subi r2, r2, 12              // and back up to the first opcode in the function
    stw  r2, CONTEXT_Iar(a0)     // and report it as CONTEXT.Iar
    stw  r0, CONTEXT_Lr(a0)      // store the original LR value in CONTEXT.LR
    mfctr r2
    stw  r2, CONTEXT_Ctr(a0)
    mtlr r0                      // restore the link register value

    cmpwi a1, 0
    beq 1f                       // brif fCaptureFloatingPoint==FALSE
    stfd f0, (0*8)(a0)           // store all FP regs into lpContext
    stfd f1, (1*8)(a0)
    stfd f2, (2*8)(a0)
    stfd f3, (3*8)(a0)
    stfd f4, (4*8)(a0)
    stfd f5, (5*8)(a0)
    stfd f6, (6*8)(a0)
    stfd f7, (7*8)(a0)
    stfd f8, (8*8)(a0)
    stfd f9, (9*8)(a0)
    stfd f10, (10*8)(a0)
    stfd f11, (11*8)(a0)
    stfd f12, (12*8)(a0)
    stfd f13, (13*8)(a0)
    stfd f14, (14*8)(a0)
    stfd f15, (15*8)(a0)
    stfd f16, (16*8)(a0)
    stfd f17, (17*8)(a0)
    stfd f18, (18*8)(a0)
    stfd f19, (19*8)(a0)
    stfd f20, (20*8)(a0)
    stfd f21, (21*8)(a0)
    stfd f22, (22*8)(a0)
    stfd f23, (23*8)(a0)
    stfd f24, (24*8)(a0)
    stfd f25, (25*8)(a0)
    stfd f26, (26*8)(a0)
    stfd f27, (27*8)(a0)
    stfd f28, (28*8)(a0)
    stfd f29, (29*8)(a0)
    stfd f30, (30*8)(a0)
    stfd f31, (31*8)(a0)

    mffs f0                      // bits 32...63 of f0 are set to the FPSCR
    stfd f0, (32*8)(a0)          // store it to lpConext->Fpscr

1:
    blr
