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
#include "strike.h"
#include "util.h"
#include "disasm.h"
#include <dbghelp.h>

#include "..\..\..\inc\corhdr.h"
#include "..\..\..\inc\cor.h"
#include "..\..\..\inc\dacprivate.h"


enum RegIndex
{
    BRANCH_REG,
    INTEGER_REG = BRANCH_REG+8,
    R9 = INTEGER_REG + 9,
    NONE = INTEGER_REG+128,
};

const int NumReg = NONE;

BOOL IsBranchReg (RegIndex reg)
{
    return reg >= BRANCH_REG && reg < BRANCH_REG + 8;
}

BOOL IsIntegerReg (RegIndex reg)
{
    return reg >= INTEGER_REG && reg < INTEGER_REG + 128;
}

struct Register
{
    UINT64 value;
    BOOL bValid;
    BOOL bPossiblePrepadLoad;         // is lhs reg in ld8 reg=[r9], 8
    UINT64 sourceaddr;
};


void KillReg (Register *regs, RegIndex regnum)
{
    ZeroMemory(&regs[regnum], sizeof(Register));
}


// Find the index for a register name
inline RegIndex FindReg (__in __in_z char *ptr, __out __out_opt int *plen = NULL)
{
    if ('b' == ptr[0] && isdigit(ptr[1]))
    {
        RegIndex regnum = (RegIndex)(BRANCH_REG + atoi(ptr+1));
        if (plen)
        {
            if (regnum < 10)
                *plen = 2;
            else if (regnum < 100)
                *plen = 3;
            else
                *plen = 4;
        }
        return regnum;
    }
    else if ('r' == ptr[0] && isdigit(ptr[1]))
    {
        RegIndex regnum = (RegIndex)(INTEGER_REG + atoi(ptr+1));
        if (plen)
        {
            if (regnum < 10)
                *plen = 2;
            else if (regnum < 100)
                *plen = 3;
            else
                *plen = 4;
        }
        return regnum;
    }
    else if (!strncmp (ptr, "gp", 2))
    {
        if (plen)
            *plen = 2;
        return (RegIndex)(INTEGER_REG + 1);
    }
    else if (!strncmp (ptr, "ret0", 4))
    {
        if (plen)
            *plen = 4;
        return (RegIndex)(INTEGER_REG + 8);
    }
    else if (!strncmp (ptr, "ret1", 4))
    {
        if (plen)
            *plen = 4;
        return (RegIndex)(INTEGER_REG + 9);
    }
    else if (!strncmp (ptr, "ret2", 4))
    {
        if (plen)
            *plen = 4;
        return (RegIndex)(INTEGER_REG + 10);
    }
    else if (!strncmp (ptr, "ret3", 4))
    {
        if (plen)
            *plen = 4;
        return (RegIndex)(INTEGER_REG + 11);
    }
    else if (!strncmp (ptr, "sp", 2))
    {
        if (plen)
            *plen = 2;
        return (RegIndex)(INTEGER_REG + 12);
    }
    else if (!strncmp (ptr, "rp", 2))
    {
        if (plen)
            *plen = 2;
        return (RegIndex)(BRANCH_REG + 0);
    }
    else
    {
        return NONE;
    }
}

// Find the value of an expression.
inline BOOL FindSrc (__in __in_z char *ptr, __in Register *reg, INT_PTR &value, BOOL &bDigit)
{
    // 'b7' is a valid hex number *and* a register name.  Anything that looks
    // like a register name probably isn't an interesting hex value, so we'll
    // look for register names first.

    BOOL bValid = FALSE;
    BOOL bByRef = IsByRef (ptr);
    bDigit = FALSE;

    int regnamelen;
    RegIndex index = FindReg (ptr, &regnamelen);
    if (index != NONE)
    {
        if (reg[index].bValid)
        {
            value = reg[index].value;
            char *sepptr = ptr + regnamelen;

            if ((IsTermSep (sepptr[0]) && !bByRef)
                || (sepptr[0] == ']' && bByRef))
            {
                bValid = TRUE;
                if (bByRef)
                    SafeReadMemory (value, &value, sizeof(value), NULL);
            }
        }
    }

    if (!bValid && GetValueFromExpr (ptr, value))
    {
        bValid = TRUE;
        bDigit = TRUE;
    }

    return bValid;
}


// Return 0 for non-managed call.  Otherwise return MD address.
DWORD_PTR MDForCall (DWORD_PTR callee)
{
    // call managed code?
    JitType jitType;
    DWORD_PTR methodDesc;
    DWORD_PTR IP = callee;
    DWORD_PTR gcinfoAddr;

// NYI

    IP2MethodDesc (IP, methodDesc, jitType, gcinfoAddr);
    if (methodDesc)
    {
        return methodDesc;
    }

    return 0;
}


// Handle a call instruction.
BOOL HandleCall (DWORD_PTR callee, Register *reg)
{
    // call managed code?
    DWORD_PTR methodDesc = MDForCall (callee);
    if (methodDesc)
    {        
        DacpMethodDescData MethodDescData;
        if (MethodDescData.Request(g_clrData,(CLRDATA_ADDRESS)methodDesc) == S_OK)
        {
        NameForMD_s (methodDesc, g_mdName,mdNameLen);
        ExtOut (" (%S, mdToken: %08x)", g_mdName, MethodDescData.MDToken);
        return TRUE;
    }
    }

    // A jump thunk?

    CONTEXT ctx = {0};

    ctx.ContextFlags = (CONTEXT_IA64 | CONTEXT_CONTROL | CONTEXT_INTEGER);

    UINT64 slot = (((UINT64)callee) & (IA64_BUNDLE_SIZE-1)) >> 2;
    ctx.StIIP = (ULONGLONG)callee & ~((DWORD_PTR)(IA64_BUNDLE_SIZE-1));
    ctx.StIPSR &= ~((UINT64)3 << PSR_RI);
    ctx.StIPSR |= (slot << PSR_RI);

    unsigned regNum;

    for (regNum = INTEGER_REG+1; regNum < INTEGER_REG+32; regNum++)
    {
        if (reg[regNum].bValid)
        {
            *(&ctx.IntGp + regNum - (INTEGER_REG+1)) = reg[regNum].value;
        }
    }

    for (regNum = BRANCH_REG; regNum < BRANCH_REG+8; regNum++)
    {
        if (reg[regNum].bValid)
        {
            *(&ctx.BrRp + regNum - BRANCH_REG) = reg[regNum].value;
        }
    }

    DacpJumpThunkData thunk;
    if (S_OK == thunk.GetJumpThunkTarget(g_clrData, &ctx))
    {
        if (thunk.TargetMethodDesc)
        {
            DacpMethodDescData MethodDescData;
            if (MethodDescData.Request(g_clrData, thunk.TargetMethodDesc) == S_OK)
            {
            NameForMD_s(thunk.TargetMethodDesc, g_mdName,mdNameLen);
            ExtOut (" (%S, mdToken: %08x)", g_mdName, MethodDescData.MDToken);
            return TRUE;
        }
        }
        
        if (thunk.Target != callee)
        {
            return HandleCall(thunk.Target, reg);
        }
    }

    // A JitHelper?
    const char* name = HelperFuncName(callee);
    if (name) {
        ExtOut (" (JitHelp: %s)", name);
        return TRUE;
    }

    // call unmanaged code?
    char Symbol[1024];
    if (SUCCEEDED(g_ExtSymbols->GetNameByOffset(callee, Symbol, 1024,
                                                NULL, NULL)
        ))
    {
        if (Symbol[0] != '\0')
        {
            ExtOut (" (%s)", Symbol);
            return TRUE;
        }
    }

    return FALSE;
}

// Determine if a value is MT/MD/Obj
void HandleValue(DWORD_PTR value)
{
    // A MethodTable?
    if (IsMethodTable(value))
    {
        NameForMT_s (value, g_mdName,mdNameLen);
        ExtOut (" (MT: %S)", g_mdName);
        return;
    }
    
    // A Managed Object?
    DWORD_PTR dwMTAddr;
    move (dwMTAddr, value);
    if (IsStringObject(value))
    {
        ExtOut (" (\"");
        StringObjectContent (value, TRUE);
        ExtOut ("\")");
        return;
    }
    else if (IsMethodTable(dwMTAddr))
    {
        NameForMT_s (dwMTAddr, g_mdName,mdNameLen);
        ExtOut (" (Object: %S)", g_mdName);
        return;
    }
    
    // A MethodDesc?
    if (IsMethodDesc(value))
    {        
        NameForMD_s (value, g_mdName,mdNameLen);
        ExtOut (" (MD: %S)", g_mdName);
        return;
    }

    // A JitHelper?
    const char* name = HelperFuncName(value);
    if (name) {
        ExtOut (" (JitHelp: %s)", name);
        return;
    }
}


void TrashVolatileRegs (Register *reg)
{
    KillReg(reg, (RegIndex)(INTEGER_REG + 2));
    KillReg(reg, (RegIndex)(INTEGER_REG + 3));
    KillReg(reg, (RegIndex)(INTEGER_REG + 8));
    KillReg(reg, (RegIndex)(INTEGER_REG + 9));
    KillReg(reg, (RegIndex)(INTEGER_REG + 1));
    KillReg(reg, (RegIndex)(INTEGER_REG + 1));

    for (int regnum = INTEGER_REG + 14; regnum < INTEGER_REG + 32; regnum++)
        KillReg(reg, (RegIndex)regnum);

    KillReg(reg, (RegIndex)(BRANCH_REG + 0));
    KillReg(reg, (RegIndex)(BRANCH_REG + 6));
    KillReg(reg, (RegIndex)(BRANCH_REG + 7));
}


/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    Unassembly a managed code.  Translating managed object,           *  
*    call.                                                             *
*                                                                      *
\**********************************************************************/
void Unassembly (DWORD_PTR IPBegin, DWORD_PTR IPEnd, DWORD_PTR IPAskedFor, 
    DWORD_PTR GCStressCodeCopy, 
    GCEncodingInfo *pGCEncodingInfo, 
    SOSEHInfo *pEHInfo)
{
    ULONG_PTR IP = IPBegin;
    char line[1024];
    Register reg[NumReg];
    RegIndex dest;
    INT_PTR value;
    BOOL bDigit;
    INT_PTR value2;
    BOOL bDigit2;
    char *ptr;

    ZeroMemory(&reg[0], sizeof(Register) * NumReg);

    // r0 is always 0
    reg[0].bValid = TRUE;

    while (IP < IPEnd)
    {
        if (IsInterrupt())
            return;

        //
        // Print out any GC information corresponding to the current instruction offset.
        //

        if (pGCEncodingInfo)
        {
            SIZE_T curOffset = (IP - IPBegin) + pGCEncodingInfo->hotSizeToAdd;
            while (   !pGCEncodingInfo->fDoneDecoding
                   && pGCEncodingInfo->ofs <= curOffset)            
            {
                ExtOut(pGCEncodingInfo->buf);
                ExtOut("\n");

                SwitchToFiber(pGCEncodingInfo->pvGCTableFiber);
            }
        }

        if (pEHInfo)
        {
            pEHInfo->FormatForDisassembly(IP - IPBegin);
        }
        
        if (IP == IPAskedFor)
        {
            ExtOut (">>> ");
        }

        // Retrieve a single instruction
        ULONG_PTR InstrAddr = IP;
        DisasmAndClean(IP, line, _countof(line));

        // Parse something like
        // 000006fb`fb37f498 (p14)  ld8    r15=[ret1], 8 ;;
        ptr = line;

        // Skip instruction address
        NextTerm(ptr);

        // Skip optional predicate
        if ('(' == *ptr)
            NextTerm(ptr);

        //
        // If there is gcstress info for this method, and this is a 'break.?
        // 8001b' instruction, then gcstress probably put the 'break' there.
        // Look up the original instruction and print it instead.
        //

        bool fIsGCStressInstr = false;

        if (GCStressCodeCopy && !strncmp (ptr, "break.", 6))
        {
            char *breakptr = ptr;

            NextTerm(breakptr);

            if (!strncmp(breakptr, "8001b", 5))
                fIsGCStressInstr = true;
        }

        char *pBundleEndNewLine;

        SSIZE_T cbIPOffset = 0;

        if (fIsGCStressInstr)
        {
            //
            // Compute address into saved copy of the code, and
            // disassemble the original instruction
            //
            
            ULONG_PTR OrigInstrAddr = GCStressCodeCopy + (InstrAddr - IPBegin);
            ULONG_PTR OrigIP = OrigInstrAddr;

            DisasmAndClean(OrigIP, line, _countof(line));

            //
            // Increment the real IP based on the size of the unmodifed
            // instruction
            //

            IP = InstrAddr + (OrigIP - OrigInstrAddr);

            cbIPOffset = IP - OrigIP;

            //
            // Print out real code address in place of the copy address
            //

            ExtOut("%08x`%08x ", (ULONG)(InstrAddr >> 32), (ULONG)InstrAddr);

            ptr = line;

            // The last instruction in a bundle will have an extra newline.  We'll
            // print out this extra newline later.
            pBundleEndNewLine = strrchr(line, '\n');
            if (pBundleEndNewLine)
                *pBundleEndNewLine = '\0';

            // Skip instruction address
            NextTerm(ptr);

            //
    

            ExtOut(ptr);

            //
            // Add an indicator that this address has not executed yet
            //

            ExtOut(" (gcstress)");
        }
        else
        {
            // The last instruction in a bundle will have an extra newline.  We'll
            // print out this extra newline later.
            pBundleEndNewLine = strrchr(line, '\n');
            if (pBundleEndNewLine)
                *pBundleEndNewLine = '\0';


            ExtOut(line);
        }

        if (!strncmp (ptr, "mov ", 4))
        {
            NextTerm (ptr);
            dest = FindReg(ptr);
            if (dest != NONE)
            {
                KillReg(reg, dest);

                NextTerm (ptr);

                RegIndex src = FindReg(ptr);
                if (src != NONE)
                {
                    if (FindSrc (ptr, reg, value, bDigit))
                    {
                        reg[dest].bValid = TRUE;
                        reg[dest].value = value;
                        reg[dest].sourceaddr = reg[src].sourceaddr;
                    }
                }
            }
        }
        // Ignore completers
        else if (!strncmp (ptr, "br.call", 7) || !strncmp (ptr, "brl.call", 8))
        {
            NextTerm (ptr);
            dest = FindReg(ptr);
            if (dest != NONE)
            {
                KillReg(reg, dest);

                NextTerm (ptr);

                RegIndex src = FindReg(ptr);

                if (FindSrc (ptr, reg, value, bDigit))
                {
                    if (bDigit)
                        value += cbIPOffset;
                
                    if (!HandleCall (value, reg))
                    {
                        if (NONE != src && reg[src].bValid && reg[src].sourceaddr)
                        {
                            value = reg[src].sourceaddr;

                            if (IsMethodDesc(value))
                            {        
                                NameForMD_s (value, g_mdName,mdNameLen);
                                ExtOut (" (MD: %S)", g_mdName);
                            }
                        }
                    }
                }
            }

            TrashVolatileRegs(reg);
        }
        else if (   !strncmp (ptr, "adds ", 5)
                 || !strncmp (ptr, "addl ", 5))
        {
            NextTerm (ptr);
            dest = FindReg(ptr);
            if (dest != NONE)
            {
                BOOL fSet = FALSE;

                NextTerm (ptr);
                if (FindSrc (ptr, reg, value, bDigit))
                {
                    NextTerm (ptr);
                    if (FindSrc (ptr, reg, value2, bDigit2))
                    {
                        reg[dest].bValid = TRUE;
                        reg[dest].value = value + value2;

                        fSet = TRUE;

                        HandleValue (value + value2);
                    }
                }

                if (!fSet)
                    KillReg(reg, dest);
            }
        }
        else if (!strncmp (ptr, "movl ", 5))
        {
            NextTerm (ptr);
            dest = FindReg(ptr);
            if (dest != NONE)
            {
                KillReg(reg, dest);

                NextTerm (ptr);
                if (FindSrc (ptr, reg, value, bDigit))
                {
                    reg[dest].bValid = TRUE;
                    reg[dest].value = value;

                    HandleValue (value);
                }
            }
        }
        // Ignore completers
        else if (!strncmp (ptr, "ld8", 3))
        {
            NextTerm (ptr);
            dest = FindReg(ptr);
            if (dest != NONE)
            {
                BOOL fSet = FALSE;

                NextTerm (ptr);

                if ('[' == ptr[0])
                {
                    RegIndex indreg = FindReg(ptr+1);
                    if (NONE != indreg)
                    {
                        if (FindSrc (ptr, reg, value, bDigit))
                        {
                            reg[dest].bValid = TRUE;
                            reg[dest].value = value;

                            fSet = TRUE;
                        }

                        if (strchr(ptr, ','))
                        {
                            NextTerm (ptr);
                            if (FindSrc (ptr, reg, value, bDigit) && bDigit)
                            {
                                reg[indreg].value += value;
                                if (indreg == R9)
                                    reg[dest].sourceaddr = reg[indreg].value;
                            }
                        }
                    }
                }

                if (!fSet)
                    KillReg(reg, dest);
            }
        }
        else
        {
            // assume this instruction will trash dest reg
            NextTerm (ptr);
            dest = FindReg(ptr);
            if (dest != NONE)
                KillReg(reg, dest);
        }
        
        ExtOut("\n");
        if (pBundleEndNewLine)
            ExtOut("\n");
    }
}


void DumpStackDummy (DumpStackFlag &DSFlag)
{
    dprintf("DumpStackDummy not yet implemented\n");
}

void DumpRegObjectHelper (const char *regName, size_t StackTop, size_t StackBottom, BOOL verifyFields)
{
    ULONG IREG;
    DEBUG_VALUE value;
    DWORD_PTR reg;
    
    g_ExtRegisters->GetIndexByName(regName, &IREG);
    g_ExtRegisters->GetValue(IREG, &value);
    reg = value.I64;

    if (reg>=StackTop && reg<=StackBottom)
        return;


    if (IsObject(reg, verifyFields)) {
        ExtOut ("%-" POINTERSIZE "s %p ", regName, (ULONG64)reg);
        if (DacpObjectData::GetObjectClassName(g_clrData,(CLRDATA_ADDRESS)reg,mdNameLen,g_mdName)==S_OK)
        {    
            ExtOut ("%S", g_mdName);
            DacpObjectData objectData;
            if (objectData.Request(g_clrData,(CLRDATA_ADDRESS)reg) != S_OK)
            {
                ExtOut ("    ");
                StringObjectContent(reg, FALSE, 40);
            }
            else if (IsObjectArray(&objectData))
            {
                // This is a methodtable
                if (DacpMethodTableData::GetMethodTableName(g_clrData,objectData.ElementTypeHandle,
                    mdNameLen,g_mdName)==S_OK)
                {
                    ExtOut ("    ");                        
                    ExtOut ("(%S[])", g_mdName);
                }                    
            }            
        }
        else
        {
            ExtOut("<unknown type>");
        }
        ExtOut ("\n");
    }
}

void DumpStackObjectsHelper (size_t StackTop, size_t StackBottom, BOOL verifyFields)
{
    ExtOut ("SP/REG           Object           Name\n");

    char regname[5];
#define DSO_REGISTERS(start, end)                               \
    for(int i=start; i<=end && i<=128; i++) {                   \
        sprintf(regname, "r%d\0", i);                           \
        DumpRegObjectHelper(regname, StackTop, StackBottom, verifyFields);    \
    } 

    DSO_REGISTERS(2,3);
    DSO_REGISTERS(8,11);
    DSO_REGISTERS(14,31);
    DSO_REGISTERS(32,128);

#undef DSO_REGISTERS

    DWORD_PTR ptr = StackTop & ~7;  // make certain quadword aligned
    for (;ptr < StackBottom; ptr += sizeof(DWORD_PTR))
    {       
        if (IsInterrupt())
            return;
        DWORD_PTR objAddr;
        move (objAddr, ptr);

        // rule out misidentification of a stack address as an object.
        if (objAddr>=StackTop && objAddr<=StackBottom)
            continue;


        if (!IsObject(objAddr, verifyFields))
        {
            continue;
        }
        
        DacpObjectData objectData;
        if (objectData.Request(g_clrData,(CLRDATA_ADDRESS)objAddr) != S_OK)
        {
            continue;
        }

        if (!IsMTForFreeObj((DWORD_PTR)objectData.MethodTable)) 
        {            
            ExtOut ("%p %p ", (ULONG64)ptr, (ULONG64)objAddr);
            if (DacpObjectData::GetObjectClassName(g_clrData,(CLRDATA_ADDRESS)objAddr,mdNameLen,g_mdName)==S_OK)
            {    
                ExtOut ("%S", g_mdName);
                if (objectData.ObjectType == OBJ_STRING)
                {
                    ExtOut ("    ");
                    StringObjectContent(objAddr, FALSE, 40);
                }
                else if (IsObjectArray(&objectData))
                {
                    // This is a methodtable
                    if (DacpMethodTableData::GetMethodTableName(g_clrData,objectData.ElementTypeHandle,
                        mdNameLen,g_mdName)==S_OK)
                    {
                        ExtOut ("    ");                        
                        ExtOut ("(%S[])", g_mdName);
                    }                    
                }
            }
            else
            {
                ExtOut("<unknown type>");
            }
            
            ExtOut ("\n");
        }        
    }
}

// Find the real callee site.  Handle JMP instruction.
// Return TRUE if we get the address, FALSE if not.
BOOL GetCalleeSite (DWORD_PTR IP, DWORD_PTR &IPCallee)
{
    dprintf("GetCalleeSite not yet implemented\n");
    return FALSE;
}
