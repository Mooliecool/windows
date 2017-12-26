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
#ifndef UTIL_H
#define UTIL_H

#include "fusionp.h"
#include "fusionheap.h"
#include "shlwapi.h"

inline
WCHAR*
WSTRDupDynamic(LPCWSTR pwszSrc)
{
    LPWSTR pwszDest = NULL;
    if (pwszSrc != NULL)
    {
        const DWORD dwLen = lstrlenW(pwszSrc) + 1;
        pwszDest = FUSION_NEW_ARRAY(WCHAR, dwLen);

        if( pwszDest )
            memcpy(pwszDest, pwszSrc, dwLen * sizeof(WCHAR));
    }

    return pwszDest;
}
#endif  // UTIL_H
