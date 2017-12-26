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
// File: primitives.cpp
//
// Platform-specific debugger primitives
//
//*****************************************************************************

#include "primitives.h"


//
// CopyThreadContext() does an intelligent copy from pSrc to pDst,
// respecting the ContextFlags of both contexts.
//
void CORDbgCopyThreadContext(CONTEXT* pDst, CONTEXT* pSrc)
{
    DWORD dstFlags = pDst->ContextFlags;
    DWORD srcFlags = pSrc->ContextFlags;
    LOG((LF_CORDB, LL_INFO1000000,
         "CP::CTC: pDst=0x%08x dstFlags=0x%x, pSrc=0x%08x srcFlags=0x%x\n",
         pDst, dstFlags, pSrc, srcFlags));

    if ((dstFlags & srcFlags & CONTEXT_CONTROL) == CONTEXT_CONTROL)
        CopyContextChunk(&(pDst->Ebp), &(pSrc->Ebp), pDst->ExtendedRegisters,
                         CONTEXT_CONTROL);
    
    if ((dstFlags & srcFlags & CONTEXT_INTEGER) == CONTEXT_INTEGER)
        CopyContextChunk(&(pDst->Edi), &(pSrc->Edi), &(pDst->Ebp),
                         CONTEXT_INTEGER);

    
    if ((dstFlags & srcFlags & CONTEXT_FLOATING_POINT) == CONTEXT_FLOATING_POINT)
        CopyContextChunk(&(pDst->FloatSave), &(pSrc->FloatSave),
                         (&pDst->FloatSave)+1,
                         CONTEXT_FLOATING_POINT);
    
}


// Update the regdisplay from a given context. 
void CORDbgSetDebuggerREGDISPLAYFromContext(DebuggerREGDISPLAY *pDRD, 
                                            CONTEXT* pContext)
{
    // We must pay attention to the context flags so that we only use valid portions
    // of the context.
    DWORD flags = pContext->ContextFlags;
    if ((flags & CONTEXT_CONTROL) == CONTEXT_CONTROL)
    {    
        pDRD->PC = (SIZE_T)CORDbgGetIP(pContext);
        pDRD->SP = (SIZE_T)CORDbgGetSP(pContext);
        pDRD->FP = (SIZE_T)CORDbgGetFP(pContext);
    }

    if ((flags & CONTEXT_INTEGER) == CONTEXT_INTEGER)
    {
        pDRD->Eax = pContext->Eax;
        pDRD->Ebx = pContext->Ebx;
        pDRD->Ecx = pContext->Ecx;
        pDRD->Edx = pContext->Edx;
        pDRD->Esi = pContext->Esi;
        pDRD->Edi = pContext->Edi;
    }
}
