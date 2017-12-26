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
#ifndef __SafeGetFileSize_h__
#define __SafeGetFileSize_h__

#include "contract.h"

//*****************************************************************************
// This provides a wrapper around GetFileSize() that forces it to fail
// if the file is >4g and pdwHigh is NULL. Other than that, it acts like
// the genuine GetFileSize().
//
//*****************************************************************************
DWORD inline SafeGetFileSize(HANDLE hFile, DWORD *pdwHigh)
{
    WRAPPER_CONTRACT;

    if (pdwHigh != NULL)
    {
        return ::GetFileSize(hFile, pdwHigh);
    }
    else
    {
        DWORD hi;
        DWORD lo = ::GetFileSize(hFile, &hi);
        if (lo == 0xffffffff && GetLastError() != NO_ERROR)
        {
            return lo;
        }
        // api succeeded. is the file too large?
        if (hi != 0)
        {
            // there isn't really a good error to set here...
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return 0xffffffff;
        }

        if (lo == 0xffffffff)
        {
            // note that a success return of (hi=0,lo=0xffffffff) will be
            // treated as an error by the caller. Again, that's part of the
            // price of being a slacker and not handling the high dword.
            // We'll set a lasterror for him to pick up.                    
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        }

        return lo;
    }

}

#endif //__SafeGetFileSize_h__
