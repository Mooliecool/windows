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
////////////////////////////////////////////////////////////////////////////////
// CHECKSUM.CPP
//
// Contains code for opening an file and getting it's checksum at the same time
// this file is 'shared' with ALink and C# so make sure to integrate
// any changes into both files

#include "pch.h"
#include "file_can.h"

#ifdef PLATFORM_UNIX
#define ENVIRONMENT_SEPARATOR L':'
#else   // PLATFORM_UNIX
#define ENVIRONMENT_SEPARATOR L';'
#endif  // PLATFORM_UNIX

// SOme stuff from file.cpp
HANDLE OpenFileEx( LPCWSTR filename, DWORD *fileLen, LPCWSTR relPath, bool bWriteAlso);
HRESULT OpenAndReadFile (LPCWSTR pszFileName, CAllocBuffer & outBuffer);
HRESULT ConvertToUnicode (CBuffer & inBuffer, UINT uiCodePage, WCAllocBuffer & outBuffer, FileType *fileType);

HRESULT ReadTextFile (PCWSTR pszFileName, UINT uiCodePage, WCAllocBuffer & outBuffer, FileType *fileType, Checksum * checksum)
{
    if (checksum == NULL)
        return ReadTextFile( pszFileName, uiCodePage, outBuffer, fileType);
    *fileType = ftUnknown;
    memset(checksum, 0, sizeof(*checksum));


    CAllocBuffer cTextBuffer;
    HRESULT     hr = S_OK;
    
    // Open the file
    if (FAILED(hr = OpenAndReadFile( pszFileName, cTextBuffer)))
        goto LExit;



    hr = ConvertToUnicode( cTextBuffer, uiCodePage, outBuffer, fileType);

LExit:
    if (FAILED(hr)) {
        if (checksum->pvData != NULL)
            VSFree(checksum->pvData);
        memset(checksum, 0, sizeof(*checksum));
    }

    return hr;
}

