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

#ifdef UNICODE
#undef UNICODE
#endif


#include <windows.h>
#include <winerror.h>
#include "fusionp.h"
#include "disk.h"
#include "helpers.h"


HRESULT GetFileSizeRoundedToCluster(HANDLE hFile, PDWORD pdwSizeLow, PDWORD pdwSizeHigh)
{
    static BOOL bFirstTime=TRUE;
    static DWORD    dwClusterSizeMinusOne, dwClusterSizeMask;

    HRESULT hr=S_OK;
    DWORD dwFileSizeLow, dwFileSizeHigh, dwError;

    // ASSERT(pdwSizeLow);
    // ASSERT(pdwSizeHigh);

    if(hFile == INVALID_HANDLE_VALUE)
    {
        dwFileSizeLow  = *pdwSizeLow;
        dwFileSizeHigh = *pdwSizeHigh;

    }
    else
    {
        dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);

        if ( (dwFileSizeLow == 0xFFFFFFFF)     && 
                ((dwError = GetLastError()) != NO_ERROR) )
        { 
            hr = HRESULT_FROM_WIN32(dwError);
            return hr;
        }
    }

    if (bFirstTime)
    {
        // approximate the cluster size by 1k
        dwClusterSizeMinusOne = 1023;
        bFirstTime = FALSE;
    }


    *pdwSizeLow = (dwFileSizeLow + dwClusterSizeMinusOne) & dwClusterSizeMask;

    if(*pdwSizeLow < dwFileSizeLow)
        dwFileSizeHigh++; // Add overflow from low.

    *pdwSizeHigh = dwFileSizeHigh;

    return S_OK;
}



