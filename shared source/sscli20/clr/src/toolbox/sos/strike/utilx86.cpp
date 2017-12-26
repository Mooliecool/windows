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


/**********************************************************************\
* Routine Description:                                                 *
*                                                                      *
*    This function is called to determine if a DWORD on the stack is   *  
*    a return address.
*    It does this by checking several bytes before the DWORD to see if *
*    there is a call instruction.                                      *
*                                                                      *
\**********************************************************************/
void isRetAddr(DWORD_PTR retAddr, DWORD_PTR* whereCalled)
{
    *whereCalled = 0;
    // don't waste time values clearly out of range
    if (retAddr < 0x1000 || retAddr > 0xC0000000)   
        return;

    unsigned char spotend[6];
    move (spotend, retAddr-6);
    unsigned char *spot = spotend+6;
    DWORD_PTR addr;
    
    // Note this is possible to be spoofed, but pretty unlikely
    // call XXXXXXXX
    if (spot[-5] == 0xE8) {
        move (*whereCalled, retAddr-4);
        *whereCalled += retAddr;
        //*whereCalled = *((int*) (retAddr-4)) + retAddr;
        if (*whereCalled < 0xC0000000 && *whereCalled > 0x1000
            && g_ExtData->ReadVirtual(*whereCalled,&addr,sizeof(addr),NULL) == S_OK)
        {
            DWORD_PTR callee;
            if (GetCalleeSite(*whereCalled,callee)) {
                *whereCalled = callee;
            }
            return;
        }
        else
            *whereCalled = 0;
    }

    // call [XXXXXXXX]
    if (spot[-6] == 0xFF && (spot[-5] == 025))  {
        move (addr, retAddr-4);
        if (g_ExtData->ReadVirtual(addr,whereCalled,sizeof(*whereCalled),NULL) == S_OK) {
            move (*whereCalled, addr);
            //*whereCalled = **((unsigned**) (retAddr-4));
            if (*whereCalled < 0xC0000000 && *whereCalled > 0x1000
                && g_ExtData->ReadVirtual(*whereCalled,&addr,sizeof(addr),NULL) == S_OK) 
            {
                DWORD_PTR callee;
                if (GetCalleeSite(*whereCalled,callee)) {
                    *whereCalled = callee;
                }
                return;
            }
            else
                *whereCalled = 0;
        }
        else
            *whereCalled = 0;
    }

    // call [REG+XX]
    if (spot[-3] == 0xFF && (spot[-2] & ~7) == 0120 && (spot[-2] & 7) != 4)
    {
        *whereCalled = 0xFFFFFFFF;
        return;
    }
    if (spot[-4] == 0xFF && spot[-3] == 0124)
    {
        *whereCalled = 0xFFFFFFFF;
        return;
    }

    // call [REG+XXXX]
    if (spot[-6] == 0xFF && (spot[-5] & ~7) == 0220 && (spot[-5] & 7) != 4)
    {
        *whereCalled = 0xFFFFFFFF;
        return;
    }
    if (spot[-7] == 0xFF && spot[-6] == 0224)
    {
        *whereCalled = 0xFFFFFFFF;
        return;
    }
    
    // call [REG]
    if (spot[-2] == 0xFF && (spot[-1] & ~7) == 0020 && (spot[-1] & 7) != 4 && (spot[-1] & 7) != 5)
    {
        *whereCalled = 0xFFFFFFFF;
        return;
    }
    
    // call REG
    if (spot[-2] == 0xFF && (spot[-1] & ~7) == 0320 && (spot[-1] & 7) != 4)
    {
        *whereCalled = 0xFFFFFFFF;
        return;
    }
    
    // There are other cases, but I don't believe they are used.
    return;
}

