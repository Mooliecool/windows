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
/* ------------------------------------------------------------------------- *
 * DebuggerRegDisplayHelper.cpp -- implementation of the platform-dependent 
 *                                 methods for transferring information between
 *                                 REGDISPLAY and DebuggerREGDISPLAY
 * ------------------------------------------------------------------------- */

#include "stdafx.h"


void CopyREGDISPLAY(REGDISPLAY* pDst, REGDISPLAY* pSrc)
{
    *pDst = *pSrc;
}

void SetDebuggerREGDISPLAYFromREGDISPLAY(DebuggerREGDISPLAY* pDRD, REGDISPLAY* pRD)
{
    // Frame pointer        
    LPVOID FPAddress = GetRegdisplayFPAddress(pRD);
    pDRD->FP  = (FPAddress == NULL ? 0 : *((SIZE_T *)FPAddress));
    pDRD->pFP = PushedRegAddr(pRD, FPAddress);

    // All that's needed for now
    pDRD->SP = pRD->SP;
    pDRD->PC = (SIZE_T)*(pRD->pPC);
}
