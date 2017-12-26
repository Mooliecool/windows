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
/**************************************************************/
/*                       gmsppc.cpp                           */
/**************************************************************/

#include "common.h"
#include "gmscpu.h"

// Macros to retrieve rA and rD fields from an opcode
#define get_rA(instr) (((instr)>>16) & 0x1f)
#define get_rD(instr) (((instr)>>21) & 0x1f)


/***************************************************************/
/* setMachState figures out what the state of the CPU will be
   when the function that calls 'setMachState' returns.  It stores
   this information in 'frame'

   setMachState works by simulating the execution of the
   instructions starting at the instruction following the
   call to 'setMachState' and continuing until a return instruction
   is simulated.  To avoid having to process arbitrary code, the
   call to 'setMachState' should be called as follows

      if (machState.setMachState != 0) return;

   setMachState is guarnenteed to return 0 (so the return
   statement will never be executed), but the expression above
   insures insures that there is a 'quick' path to epilog
   of the function.  This insures that setMachState will only
   have to parse a limited number of X86 instructions.   */


/***************************************************************/

#ifndef DACCESS_COMPILE

void MachState::Init(TADDR** prVal, TADDR aSp, TADDR* aPRetAddr)
{
    LEAF_CONTRACT;

#ifdef _DEBUG
    memset(_Regs, 0xCC, sizeof(_Regs));
#endif
    _sp = aSp;
    _pRetAddr = aPRetAddr;
    memcpy(_pRegs, prVal, sizeof(_Regs));
}

#endif // #ifndef DACCESS_COMPILE

/***************************************************************/
TADDR LazyMachState::unwindLazyState(LazyMachState* baseState,
                                     MachState* lazyState,
                                     int funCallDepth,
                                     TestFtn testFtn)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    int i;
    int rA;
    int rD = 0;
    int d;
    BYTE* dp;
    unsigned __int32 Gprs[32];
    unsigned __int32 *pGprs[32];
    unsigned __int32 Cr;
    unsigned __int32 Lr;
    unsigned __int32 *pLr;
    unsigned __int32 *ip;
    unsigned __int32 *pRetAddr = NULL;
    BOOL bFirstCondJmp = TRUE;

    // Currently we only do this for depth 1 through 4
    _ASSERTE(1 <= funCallDepth && funCallDepth <= 4);

    // Work with a copy so that we only write the values once.
    // This avoids race conditions.  Unlike the x86 version, this
    // one must track all 32 general purpose registers as it
    // interprets the code, since the return address must be loaded
    // from the stack into a GPR, and moved from a GPR to the LR.
    memset(Gprs, 0, sizeof(Gprs));
    memcpy(&Gprs[13], baseState->_Regs, sizeof(baseState->_Regs));
    for (i=0; i<32; ++i) {
        pGprs[i] = &Gprs[i];
    }
    Cr = baseState->_cr;

    Gprs[1] = (unsigned __int32)(PBYTE)PTR_BYTE(baseState->captureSp); // r1 is the stack pointer
    pGprs[1] = NULL;
    Lr = 0;
    pLr = &Lr;
    ip = (unsigned __int32*)(PBYTE)PTR_BYTE(baseState->capturePC);

#ifdef _DEBUG
    int count = 0;
#endif
    for (;;) {

        // we should never walk more than 1000 instructions!
        _ASSERTE(count++ < 1000);

#ifdef _DEBUG
// marks the register as uninitialized
#define MARK_UNINITIALIZED(_r) { int __r = _r; pGprs[__r] = &Gprs[__r]; Gprs[__r] = 0x1234; }
#else
#define MARK_UNINITIALIZED(r)
#endif

        switch (*ip >> 26) { // switch on the 6-bit opcode
        case 11:        // cmpwi crfD,L,rA,SIMM
            break;

        case 14:        // addi rD,rA,SIMM (also li rD,value)
            // note that this is used to rollback the stack in the optimized code
            rD = get_rD(*ip);
            rA = get_rA(*ip);
            Gprs[rD] = Gprs[rA] + (__int32)(__int16)(*ip);
            break;

        case 15:        // addis rD,rA,SIMM
            MARK_UNINITIALIZED(get_rD(*ip));
            break;

        case 16:        // bcx BO,BI,target_addr (ie. beq, bne, ...)
            _ASSERTE((*ip & 0x2) == 0); // only pc-relative jumps supported
            if (bFirstCondJmp) {
                bFirstCondJmp = FALSE;

                // this should better be branch on zero flag
                _ASSERTE(((*ip >> 16) & 3) == 2);

                // follow the non-zero path
                if ((*ip & (1<<24)) == 0) {
                    ip += ((__int16)*ip) >> 2; // follow the branch
                    continue; // the new ip has been computed already
                }
            }
            else {
                // Except the first jump, we always follow forward jump to avoid possible looping.
                if (((*ip & (1<<15)) == 0)) { // sign target_addr is positive
                    ip += ((__int16)*ip) >> 2; // follow the branch
                    continue; // the new ip has been computed already
                }
            }
            break;

        case 18:        // bx (ie. bl)
            switch (*ip & 3) {
            case 0:     // b
                ip += ((__int32)(*ip << 6)) >> 8;
                continue; // the new ip has been computed already

            case 1:     // bl
                break;

            case 2:     // ba
            case 3:     // bla
                goto badOpcode;  // absolute addresses not supported
            }
            break;

        case 19:        // bcctrx
            if (*ip == 0x4e800020) {    // blr
                _ASSERTE(pLr != &Lr);
                ip = (unsigned __int32*)Lr;
                pRetAddr = pLr;
                --funCallDepth;
                if (funCallDepth <= 0 || 
                    (testFtn != 0 && (*testFtn)((void*)*pRetAddr))) {
                    goto done;
                }
                bFirstCondJmp = TRUE;
                continue; // the new ip has been computed already
            }
            break;

        case 21:        // rlwnmx rA,rS,rB,MB,ME
            MARK_UNINITIALIZED(get_rA(*ip));
            break;

        case 24:        // ori rA,rS,UIMM
        case 25:        // oris rA,rS,UIMM
        case 26:        // xori rA,rS,UIMM
        case 27:        // xoris rA,rS,UIMM
        case 28:        // andi rA,rS,UIMM
        case 29:        // andis rA,rS,UIMM
            MARK_UNINITIALIZED(get_rA(*ip));
            break;

        case 31:
            switch ((*ip >> 1) & 0x1ff) {
            case 0:     // cmp/cmpw
                break;

            case 4:
                *((int *)0)=1;
                break;

            case 19:    // mfcr rD
                MARK_UNINITIALIZED(get_rD(*ip));
                break;

            case 32:    // cmpl CrfD,L,rA,rB
                break;

            case 40:    // subf rD,rA,rB
                MARK_UNINITIALIZED(get_rD(*ip));
                break;

            case 104:   // neg rD,rA
                MARK_UNINITIALIZED(get_rD(*ip));
                break;

            case 144:   // mtcrf CRM,rS
                {
                _ASSERTE(pGprs[rD] != &Gprs[rD]);

                int mask1 = *ip >> 12;
                int mask2 = 0;

                for (i = 0; i < 8; i++) {
                    mask2 |= (mask1 & (1 << i)) ? (0xF << (4*i)) : 0;
                }                

                rD = get_rD(*ip);                
                Cr = (Cr & ~mask2) | (Gprs[rD] & mask2);
                }
                break;

            case 266:   // add rD,rA,rB
                MARK_UNINITIALIZED(get_rD(*ip));
                break;

            case 444:   // orx rA, rS, rB (also mr rA, rS)
                MARK_UNINITIALIZED(get_rA(*ip));
                break;

            case 467:   // mtspr SPR,rS (also mtlr rD, mtctr rD)
                if ((*ip & 0x7e0fffff) == 0x7c0803a6) { // mtlr rD
                    rD = get_rD(*ip);
                    _ASSERTE(pGprs[rD] != &Gprs[rD]);
                    Lr = Gprs[rD];
                    pLr = pGprs[rD];
                }
                break;

            default:
                goto badOpcode;
            }
            break;

        case 32:        // lwz rD,d(rA)
            rA = get_rA(*ip);
            if (rA == 1) {     // lwz rD,d(sp)
                rD = get_rD(*ip);

                d = (int)(short)*ip;
                dp = (BYTE *)Gprs[rA]+d;

                if (dp == (BYTE*)baseState->captureSp) {
                    pGprs[rD] = NULL;
                    Gprs[rD] = (unsigned __int32)baseState->captureSp2;
                }
                else {
                    pGprs[rD] = (unsigned __int32 *)dp;
                    Gprs[rD] = *pGprs[rD];
                }
            }
            else {
                MARK_UNINITIALIZED(get_rD(*ip));
            }
            break;

        case 36:        // stw rS,d(rA)
            break;

        case 38:        // stb rS,d(rA)
            break;

        case 40:        // lhz rD,d(rA)
            MARK_UNINITIALIZED(get_rD(*ip));
            break;

        case 44:        // sth rS,d(rA)
            break;

        case 46:        // lmw rD,d(rA)
            rA = get_rA(*ip);
            if (rA == 1) {    // lmw rD,d(sp)
                rD = get_rD(*ip);
                d = (int)(short)*ip;
                dp = (BYTE *)Gprs[rA]+d;

                while (rD < 32) {
                    pGprs[rD] = (unsigned __int32 *)dp;
                    Gprs[rD] = *pGprs[rD];
                    rD++;
                    dp+=4;
                }
            } else {
                goto badOpcode;
            }
            break;

        case 48:        // lfs rD,d(rA)
        case 50:        // lfd rD,d(rA)
        case 52:        // stfs rD,d(rA)                        
        case 54:        // stfd rS,d(rA)
            break;

        default:
            // fall through
badOpcode:
            _ASSERTE(!"Bad opcode");
            // Cause an access violation (Free Build assert)
            *((unsigned __int32**) 0) = ip;
            goto done;

        }
        ip++;
    }
done:

    // At this point the fields in 'frame' correspond exactly to the register
    // state when the the helper returns to its caller.

    memcpy(lazyState->_Regs, &Gprs[13], sizeof(lazyState->_Regs));
    for (i=0; i<NUM_CALLEESAVED_REGISTERS; ++i) {
        if (pGprs[13+i] == &Gprs[13+i]) {
            lazyState->_pRegs[i] = (PTR_TADDR)(TADDR)&baseState->_Regs[i];
        } else {
            lazyState->_pRegs[i] = (PTR_TADDR)(TADDR)pGprs[13+i];
        }
    }
    lazyState->_cr = Cr;
    lazyState->_sp = (TADDR)Gprs[1];
    lazyState->_pRetAddr = (PTR_TADDR)(TADDR)pRetAddr;

    return (TADDR)pRetAddr;
}
