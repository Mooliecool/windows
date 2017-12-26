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
//*****************************************************************************
// File: ppcwalker.cpp
//
// ppc instruction decoding/stepping logic
//
//*****************************************************************************

#include "stdafx.h"

#include "walker.h"

#include "frames.h"
#include "openum.h"


#ifdef _PPC_

#define R_VALUE(inst, s, e )     ((((unsigned)inst) << s) >> (31-(e-s)) )
#define R_BIT(inst, p)           ((((unsigned)inst) << p) >> 31 )
#define R_PPC_OPCODE(inst)       (((unsigned)inst) >> 26 )

// y: hint bit
// x: ignore the bit
#define BO_0 0  // 0000y Decrement counter, branch if counter is not 0 and condition is false
#define BO_1 2  // 0001y Decrement counter, branch if counter is 0 and condition is false
#define BO_2 4  // 001xy Branch if condition is false
#define BO_3 8  // 0100y Decrement counter, branch if counter is not 0 and condition is true
#define BO_4 10 // 0101y Decrement counter, branch if counter is 0 and condition is true
#define BO_5 12 // 011xy Branch if condition is true
#define BO_6 16 // 1x00y Decrement counter, branch if counter is not 0
#define BO_7 18 // 1x01x Decrement counter, branch if counter is 0
#define BO_8 20 // 1x1xx Branch always

// Evaluate whether we can enough information to predict the branch or not
// We can predict if we have all registers or if the branch is a branch always
#define CANPREDICT(regs, instr)                         \
    ((regs != NULL) ||                                  \
     ((R_VALUE(instr, 6, 10) & 0x14) == 0x14))

// Evaluate whether the branch will be taken or not:
bool NativeWalker::TakeBranch(DWORD instruction)
{
    // Get the BO field, ignore the hint bit
    ULONG BO = R_VALUE(instruction, 6, 10);
    ULONG BI = R_VALUE(instruction, 11, 15);

    if ((BO & 0x14) == 0x14) {
        LOG((LF_CORDB, LL_INFO100000, "NW:TakeBranch: BO:0x%x, branch always\n", BO));
        return true;
    }

    // If the instruction isn't a branch always, m_registers must be non null
    _ASSERTE(m_registers);
    LOG((LF_CORDB, LL_INFO100000, "NW:TakeBranch: BO:0x%x, BI:0x%x, Ctr:0x%x, Cr:0x%x\n", BO, BI, m_registers->pContext->Ctr, m_registers->pContext->Cr));

    if ((BO & 0x4) == 0) {
        ULONG decrementedCounter = m_registers->pContext->Ctr - 1;
        if ((BO & 0x2) != 0) {
            if (decrementedCounter != 0) return false;
        }
        else {
            if (decrementedCounter == 0) return false;
        }
    }
     
    if ((BO & 0x10) == 0) {
        bool CrValue = R_BIT(m_registers->pContext->Cr, BI);
        if ((BO & 0x8) != 0) {
            if (CrValue) return false;
        }
        else {
            if (!CrValue) return false;
        }
    }

    return true;
}

//
// The ppc walker is currently pretty minimal.  
// It only recognizes branchs.  The rest is treated as unknown.
//
void NativeWalker::Decode()
{
    LOG((LF_CORDB, LL_INFO100000, "NW:Decode: m_ip:0x%x\n", m_ip));

    BYTE *ip = (BYTE *)m_ip;
    DWORD opcode;

    // Read the opcode
    m_opcode = (*(DWORD *)ip);

    ip = ip + sizeof(DWORD);
    if (m_opcode == CORDbg_BREAK_INSTRUCTION)
        m_opcode = DebuggerController::GetPatchedOpcode((CORDB_ADDRESS_TYPE *)m_ip);

    m_type = WALK_UNKNOWN;
    m_isAbsoluteBranch = false;
    // Initialize the defaults
    // default for m_nextIP must be NULL
    // callers use that to detect that we can't predict the next instruction
    m_skipIP = ip;
    m_nextIP = NULL;
    opcode = R_PPC_OPCODE(m_opcode);

    LOG((LF_CORDB, LL_INFO100000, "NW:Decode: m_opcode:0x%x, opcode:0x%x\n", m_opcode, opcode));

    // Analyze what we can of the opcode
    switch (opcode)
    {
        // Branch unconditiaonlly
        case 0x12: // bX 0x12 | LI[6-29] | AA[30] | LK[31]
        {
            bool lk = !!R_BIT(m_opcode, 31);
            if (lk)
                m_type = WALK_CALL;
            else
                m_type = WALK_BRANCH;

            DWORD address;
            address = (unsigned)( R_VALUE(m_opcode, 6, 29 ) << 2);
            // sign extend if neccessary
            if (address & 0x00800000)
            {
                address |= 0xFF000000;
            }
            m_isAbsoluteBranch = !!R_BIT(m_opcode, 30);
            if (m_isAbsoluteBranch)
            {
                m_nextIP = (BYTE*) address;
            }
            else
            {
                m_nextIP = (BYTE*) (m_ip + address);
            }
            break;
        }     

        // Branch conditionally
        case 0x10: // bcX
        case 0x13: // bcctrX, bclrX
        {
            bool lk = !!R_BIT(m_opcode, 31);

            if (lk)
                m_type = WALK_CALL;
            else
                m_type = WALK_BRANCH;

            switch (opcode)
            {
            case 0x10: // Branch contidionally to target address
                       // bcX 0x10 | BO[6-10] | BI[11-15] | BD[16-29] | AA[30] | LK[31]
                {
                    DWORD address;
                    address = (signed)(short)( R_VALUE(m_opcode, 16, 29 ) <<2);
                    if (CANPREDICT(m_registers, m_opcode)) // 
                    {
                        if (TakeBranch(m_opcode))
                        {
                            m_isAbsoluteBranch = !!R_BIT(m_opcode, 30);
                            if (m_isAbsoluteBranch)
                            {
                                m_nextIP = (BYTE*) address;
                            }
                            else
                            {
                                m_nextIP = (BYTE*) (m_ip + address);
                            }
                        } else // we know branch is not taken
                            m_nextIP = ip;
                    }
                }
                break;

                
            case 0x13: // Branch conditionally to count register
                       //   bcctrX 0x13 | BO[6-10] | BI[11-15] | 00000 | 0x210[21-30] | LK[31]
                       // Branch conditionally to link register
                       //   bclrX  0x13 | BO[6-10] | BI[11-15] | 00000 | 0x010[21-30] | LK[31]
                if (R_BIT(m_opcode, 21))
                {
                    // bcctrX
                    // Branch conditionally to count register
                    if (CANPREDICT(m_registers, m_opcode))
                    {
                        if (TakeBranch(m_opcode)) // Branch Always
                        {
                            if (m_registers)
                            {
                                m_nextIP = (BYTE *)(unsigned)m_registers->pContext->Ctr;
                                m_isAbsoluteBranch = true;
                            }
                        } else // we know branch is not taken
                            m_nextIP = ip;
                    }
                }
                else
                {
                    // bclrX - Branch conditionally to link register 
                    if (CANPREDICT(m_registers, m_opcode))
                    {
                        if (TakeBranch(m_opcode))
                        {
                            if (m_registers)
                            {
                                m_nextIP = (BYTE *)(unsigned)m_registers->pContext->Lr;
                                LOG((LF_CORDB, LL_INFO100000, "NW:Decode: *Lr:0x%x, *Lr+1:0x%x, *Lr+2\n", *(DWORD *)m_nextIP, *((DWORD *)m_nextIP+1), *((DWORD *)m_nextIP+1)));
                                m_isAbsoluteBranch = true;
                            }
                        } else // we know branch is not taken
                            m_nextIP = ip;
                    }

                    // Check to see if we're returning from this function
                    if (!lk)
                    {
                        m_type = WALK_RETURN;
                    }
                }
                break;
            default:
                _ASSERTE(!"NYI");
                break;
            }

            break;
        }

    default:
        m_nextIP = m_skipIP;
        break;
    }
    LOG((LF_CORDB, LL_INFO100000, "NW:Decode: m_nextIP=0x%x, m_skipIP=0x%x\n", m_nextIP, m_skipIP));

}

void NativeWalker::DecodeInstructionForPatchSkip(const BYTE *address, BOOL *pFIsCall, BOOL *pFIsAbsBranch)
{
    LOG((LF_CORDB, LL_INFO10000, "Patch decode: "));

    if (pFIsCall == NULL || pFIsAbsBranch == NULL)
    {
        return;
    }

    //
    // Look at opcode to tell if it's a call or an
    // absolute branch.
    //

    *pFIsCall = FALSE;
    *pFIsAbsBranch = FALSE;

    DWORD instruction = (*(DWORD *)address);
    DWORD opcode = R_PPC_OPCODE(instruction);

    LOG((LF_CORDB, LL_INFO10000, "inst 0x%x opcode 0x%x ", instruction, opcode));
    
    switch (opcode)
    {
        // Branch unconditiaonlly
        case 0x12: // bX 0x12 | LI[6-29] | AA[30] | LK[31]
        {
            if (R_BIT(instruction, 31))
            {
                *pFIsCall = TRUE;
            }

            if (R_BIT(instruction, 30))
            {
                *pFIsAbsBranch = TRUE;
            }
            break;
        }     

        // Branch conditionally
        case 0x10: // bcX
        case 0x13: // bcctrX, bclrX
        {
            if (R_BIT(instruction, 31))
            {
                *pFIsCall = TRUE;
            }
            
            switch (opcode)
            {
            case 0x10: // Branch contidionally to target address
                       // bcX 0x10 | BO[6-10] | BI[11-15] | BD[16-29] | AA[30] | LK[31]
                {
                    if (CANPREDICT(NULL, instruction))
                    {
                        if (R_BIT(instruction, 30))
                        {
                            *pFIsAbsBranch = TRUE;
                        }
                    }
                }
                break;
                
            case 0x13: // Branch conditionally to count register
                       //   bcctrX 0x13 | BO[6-10] | BI[11-15] | 00000 | 0x210[21-30] | LK[31]
                       // Branch conditionally to link register
                       //   bclrX  0x13 | BO[6-10] | BI[11-15] | 00000 | 0x010[21-30] | LK[31]
                if (R_BIT(instruction, 21))
                {
                    // bcctrX
                    // Branch conditionally to count register
                    if (CANPREDICT(NULL, instruction))
                    {
                        *pFIsAbsBranch = TRUE;
                    }
                }
                else
                {
                    // bclrX - Branch conditionally to link register 
                    if (CANPREDICT(NULL, instruction))
                    {
                        *pFIsAbsBranch = TRUE;
                    }
                }
                break;
            default:
                _ASSERTE(!"NYI");
                break;
            }

            break;
        }
        
        default:
            break;
    }

    LOG((LF_CORDB, LL_INFO10000, "call: %i abs_branch: %i", *pFIsCall, *pFIsAbsBranch));

}

#endif // _PPC_
