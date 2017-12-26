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
// ===========================================================================
// File: sarray.cpp
// 
// ===========================================================================
/*++

Abstract:

    PALRT SAFEARRAY support

Revision History:

--*/

#include "rotor_palrt.h"
#include "oautil.h"

STDAPI_(SAFEARRAY *) SafeArrayCreateVector(VARTYPE vt, LONG lLbound, ULONG cElements)
{
    SAFEARRAYBOUND sabound = {cElements, lLbound};

    SAFEARRAY FAR* psa;
    unsigned long  cbSize;

    _ASSERTE(vt == VT_VARIANT);
    _ASSERTE(lLbound == 0);
    _ASSERTE(cElements < 10);

    cbSize = cElements * sizeof(VARIANT) + sizeof(SAFEARRAY); // add the descriptor

    if( NULL == (psa = (SAFEARRAY FAR*)malloc(cbSize)))
      return NULL;

    // zero out the allocated memory
    memset(psa, 0, cbSize);
    psa = (SAFEARRAY FAR*)((BYTE *)psa);

    psa->pvData = (LPVOID)((BYTE *)psa + sizeof(SAFEARRAY));
    psa->cDims      = 1;
    psa->cbElements = sizeof(VARIANT);
    psa->fFeatures  = FADF_VARIANT;
    psa->rgsabound[0] = sabound;

    return psa;
}

STDAPI_(UINT) SafeArrayGetDim(SAFEARRAY * psa)
{
    return psa->cDims;
}

STDAPI SafeArrayGetElement(SAFEARRAY * psa, LONG * rgIndices, void * pv)
{
    ULONG ofs;
    
    _ASSERTE(psa->cDims == 1);
    ofs = rgIndices[0] * psa->cbElements;
    memcpy(pv, ((unsigned char*)psa->pvData) + ofs, sizeof(VARIANT));
    
    return NOERROR;
}

STDAPI SafeArrayGetLBound(SAFEARRAY * psa, UINT nDim, LONG * plLbound)
{
    _ASSERTE(psa->cDims == 1);
    *plLbound = psa->rgsabound[0].lLbound;
    
    return NOERROR;
}

STDAPI SafeArrayGetUBound(SAFEARRAY * psa, UINT nDim, LONG * plUbound)
{
    _ASSERTE(psa->cDims == 1);
    _ASSERTE(psa->rgsabound[0].cElements != 0);
    *plUbound = psa->rgsabound[0].lLbound + psa->rgsabound[0].cElements - 1;

    return NOERROR;
}

STDAPI SafeArrayGetVartype(SAFEARRAY * psa, VARTYPE * pvt)
{
    *pvt = VT_VARIANT;
    
    return NOERROR;
}

STDAPI SafeArrayPutElement(SAFEARRAY * psa, LONG * rgIndices, void * pv)
{
    void * pvData;
    ULONG ofs;

    _ASSERTE(psa->cDims == 1);
    ofs = rgIndices[0] * psa->cbElements;
    pvData = ((unsigned char*)psa->pvData) + ofs;

    _ASSERTE(psa->fFeatures & FADF_VARIANT);
    _ASSERTE(V_VT((VARIANT*)pv) != VT_DISPATCH);
    _ASSERTE(V_VT((VARIANT*)pv) != VT_UNKNOWN);
    _ASSERTE(V_VT((VARIANT*)pv) != VT_BSTR);
    memcpy(pvData, pv, sizeof(VARIANT));

    return NOERROR;
}

STDAPI SafeArrayDestroy(SAFEARRAY * psa)
{
    if(psa == NULL)
      return NOERROR;

    free(psa);

    return NOERROR;
}










