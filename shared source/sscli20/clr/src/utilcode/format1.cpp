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
/***************************************************************************/
/* routines for parsing file format stuff ... */
/* this is split off from format.cpp because this uses meta-data APIs that
   are not present in many builds.  Thus if someone needs things in the format.cpp
   file but does not have the meta-data APIs, I want it to link */

#include "stdafx.h"
#include "cor.h"
#include "corpriv.h"
#include "../inc/corhlpr.cpp"


BOOL (__stdcall  *g_pfnCOMPlusIsMonitorException)(struct _EXCEPTION_POINTERS *pExceptionInfo);

static LONG FilterAllExceptions(PEXCEPTION_POINTERS pExceptionPointers, LPVOID lpvParam)
{
    if ((g_pfnCOMPlusIsMonitorException != NULL)
        && (*g_pfnCOMPlusIsMonitorException)(pExceptionPointers))
        return EXCEPTION_CONTINUE_EXECUTION;

    if ((pExceptionPointers->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) ||  
        (pExceptionPointers->ExceptionRecord->ExceptionCode == EXCEPTION_ARRAY_BOUNDS_EXCEEDED) ||
        (pExceptionPointers->ExceptionRecord->ExceptionCode == EXCEPTION_IN_PAGE_ERROR))
        return EXCEPTION_EXECUTE_HANDLER;

    return EXCEPTION_CONTINUE_SEARCH;
}

/***************************************************************************/
COR_ILMETHOD_DECODER::COR_ILMETHOD_DECODER(COR_ILMETHOD* header, void *pInternalImport, DecoderStatus* wbStatus)
{

    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_FORBID_FAULT;

    // Can't put contract because of SEH
    // CONTRACTL
    // {
    //    NOTHROW;
    //    GC_NOTRIGGER;
    //    FORBID_FAULT;
    // }
    // CONTRACTL_END

    bool errorInInit = false;

    PAL_TRY
    {
        // Decode the COR header into a more convenient form
        DecoderInit(this, (COR_ILMETHOD *) header);
    }
    PAL_EXCEPT_FILTER(FilterAllExceptions, NULL)
    {
        errorInInit = true;
        Code = 0;
        SetLocalVarSigTok(0);
        if (wbStatus != NULL)
        {
            *wbStatus = FORMAT_ERROR;
        }
    }
    PAL_ENDTRY

    if (errorInInit)
    {
        return;
    }

    // If there is a local variable sig, fetch it into 'LocalVarSig'
    if (GetLocalVarSigTok() && pInternalImport)
    {
        IMDInternalImport* pMDI = reinterpret_cast<IMDInternalImport*>(pInternalImport);

        if (wbStatus != NULL)
        {
            if ((!pMDI->IsValidToken(GetLocalVarSigTok())) || (TypeFromToken(GetLocalVarSigTok()) != mdtSignature)
                || (RidFromToken(GetLocalVarSigTok())==0))
            {
                *wbStatus = FORMAT_ERROR;         // failure bad local variable signature token
                return;
            }
        }

        DWORD cSig = 0;
        LocalVarSig = pMDI->GetSigFromToken(GetLocalVarSigTok(), &cSig);
        
        if (wbStatus != NULL)
        {
            if (!SUCCEEDED(validateTokenSig(GetLocalVarSigTok(), LocalVarSig, cSig, 0, pMDI)) ||
                *LocalVarSig != IMAGE_CEE_CS_CALLCONV_LOCAL_SIG)
            {
                *wbStatus = VERIFICATION_ERROR;   // failure validating local variable signature
                return;
            }
        }
    }

    if (wbStatus != NULL)
    {
        *wbStatus = SUCCESS;
    }
}

