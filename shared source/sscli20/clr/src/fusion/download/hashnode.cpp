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
#include "fusionp.h"
#include "util.h"
#include "hashnode.h"
#include "helpers.h"

HRESULT CHashNode::Create(LPCWSTR pwzSource, CHashNode **ppHashNode)
{
    HRESULT                               hr = S_OK;
    CHashNode                            *pHashNode = NULL;

    if (!pwzSource || !ppHashNode) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppHashNode = NULL;

    pHashNode = NEW (CHashNode());
    if (!pHashNode) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pHashNode->Init(pwzSource);
    if (FAILED(hr)) {
        SAFEDELETE(pHashNode);
        goto Exit;
    }

    *ppHashNode = pHashNode;

Exit:
    return hr;
}

CHashNode::CHashNode()
: _pwzSource(NULL)
{
}

CHashNode::~CHashNode()
{
    SAFEDELETEARRAY(_pwzSource);
}

HRESULT CHashNode::Init(LPCWSTR pwzSource)
{
    HRESULT                           hr = S_OK;
    
    _pwzSource = WSTRDupDynamic(pwzSource);
    if (!_pwzSource) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

Exit:
    return hr;
}

BOOL CHashNode::IsDuplicate(LPCWSTR pwzStr) const
{
   BOOL                                 bRet = FALSE;

   if (!pwzStr) {
       goto Exit;
   }


   if (!FusionCompareStringI(_pwzSource, pwzStr)) {
       bRet = TRUE;
   }

Exit:
   return bRet;
}
    
