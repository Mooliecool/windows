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

#include <eecallconv.h>
#include <asmconstants.h>

// int LazyMachStateCaptureState(struct LazyMachState *pState)
ASMFUNC(LazyMachStateCaptureState)
    #define pState r3
    li r0, 0
    stw r0, MachState__pRetAddr(pState) // marks that this is not yet valid
    stmw r13, MachState__Regs(pState)   // save r13...r30 in Regs[]
    mfcr r0                             // save cr
    stw r0, MachState__cr(pState)
    lwz  r0, 0(r1)
    stw  r1, LazyMachState_captureSp(pState)
    stw  r0, LazyMachState_captureSp2(pState)   
    mflr r0                             // capture return address
    stw  r0, LazyMachState_capturePC(pState)
    li r3, 0
    blr
