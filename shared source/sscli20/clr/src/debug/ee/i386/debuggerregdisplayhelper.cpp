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

    pDRD->pEdi = PushedRegAddr(pRD, pRD->pEdi);
    pDRD->Edi  = (pRD->pEdi == NULL ? 0 : *(pRD->pEdi));
    pDRD->pEsi = PushedRegAddr(pRD, pRD->pEsi);
    pDRD->Esi  = (pRD->pEsi == NULL ? 0 : *(pRD->pEsi));
    pDRD->pEbx = PushedRegAddr(pRD, pRD->pEbx);
    pDRD->Ebx  = (pRD->pEbx == NULL ? 0 : *(pRD->pEbx));
    pDRD->pEdx = PushedRegAddr(pRD, pRD->pEdx);
    pDRD->Edx  = (pRD->pEdx == NULL ? 0 : *(pRD->pEdx));
    pDRD->pEcx = PushedRegAddr(pRD, pRD->pEcx);
    pDRD->Ecx  = (pRD->pEcx == NULL ? 0 : *(pRD->pEcx));
    pDRD->pEax = PushedRegAddr(pRD, pRD->pEax);
    pDRD->Eax  = (pRD->pEax == NULL ? 0 : *(pRD->pEax));
    pDRD->SP   = pRD->Esp;
    pDRD->PC   = (SIZE_T)*(pRD->pPC);

    // Please leave EBP, ESP, EIP at the front so I don't have to scroll
    // left to see the most important registers.  Thanks!
    LOG( (LF_CORDB, LL_INFO1000, "DT::TASSC:Registers:"
          "Ebp = %x   Esp = %x   Eip = %x   Edi:%d   "
          "Esi = %x   Ebx = %x   Edx = %x   Ecx = %x   Eax = %x\n",
          pDRD->FP, pDRD->SP, pDRD->PC, pDRD->Edi,
          pDRD->Esi, pDRD->Ebx, pDRD->Edx, pDRD->Ecx, pDRD->Eax ) );
}
