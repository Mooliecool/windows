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
// MDFileFormat.cpp
//
// This file contains a set of helpers to verify and read the file format.
// This code does not handle the paging of the data, or different types of
// I/O.  See the StgTiggerStorage and StgIO code for this level of support.
//
//*****************************************************************************
#include "stdafx.h"                     // Standard header file.
#include "mdfileformat.h"               // The format helpers.
#include "posterror.h"                  // Error handling code.


//*****************************************************************************
// Verify the signature at the front of the file to see what type it is.
//*****************************************************************************
#define STORAGE_MAGIC_OLD_SIG   0x2B4D4F43  // BSJB
HRESULT MDFormat::VerifySignature(
    PSTORAGESIGNATURE pSig,             // The signature to check.
    ULONG             cbData)
{
    HRESULT     hr = S_OK;

    // If signature didn't match, you shouldn't be here.
    ULONG dwSignature = pSig->GetSignature();
    if (dwSignature == STORAGE_MAGIC_OLD_SIG)
        return (PostError(CLDB_E_FILE_OLDVER, 1, 0));
    if (dwSignature != STORAGE_MAGIC_SIG)
        return (PostError(CLDB_E_FILE_CORRUPT));

    // Check for overflow
    ULONG lVersionString = pSig->GetVersionStringLength();
    ULONG sum = sizeof(STORAGESIGNATURE) + lVersionString;
    if (sum < sizeof(STORAGESIGNATURE) || sum < lVersionString)
        return (PostError(CLDB_E_FILE_CORRUPT));

    // Check for invalid version string size
    if ((sizeof(STORAGESIGNATURE) + lVersionString) > cbData)
        return (PostError(CLDB_E_FILE_CORRUPT));

    // Check that the version string is null terminated. This string
    // is ANSI, so no double-null checks need to be made.
    {
        BYTE *pStart = &pSig->pVersion[0];
        BYTE *pEnd = pStart + lVersionString + 1; // Account for terminating NULL
        BYTE *pCur;

        for (pCur = pStart; pCur < pEnd; pCur++)
        {
            if (*pCur == NULL)
                break;
        }

        // If we got to the end without hitting a NULL, we have a bad version string
        if (pCur == pEnd)
            return (PostError(CLDB_E_FILE_CORRUPT));
    }

    // There is currently no code to migrate an old format of the 1.x.  This
    // would be added only under special circumstances.
    if (pSig->GetMajorVer() != FILE_VER_MAJOR || pSig->GetMinorVer() != FILE_VER_MINOR)
        hr = CLDB_E_FILE_OLDVER;

    if (FAILED(hr))
        hr = PostError(hr, (int) pSig->GetMajorVer(), (int) pSig->GetMinorVer());
    return (hr);
} // HRESULT MDFormat::VerifySignature()

//*****************************************************************************
// Skip over the header and find the actual stream data.
//*****************************************************************************
PSTORAGESTREAM MDFormat::GetFirstStream(// Return pointer to the first stream.
    PSTORAGEHEADER pHeader,             // Return copy of header struct.
    const void *pvMd)                   // Pointer to the full file.
{
    const BYTE  *pbMd;              // Working pointer.

    // Header data starts after signature.
    pbMd = (const BYTE *) pvMd;
    pbMd += sizeof(STORAGESIGNATURE);
    pbMd += ((STORAGESIGNATURE*)pvMd)->GetVersionStringLength();
    PSTORAGEHEADER pHdr = (PSTORAGEHEADER) pbMd;
    *pHeader = *pHdr;
    pbMd += sizeof(STORAGEHEADER);

    // ECMA specifies that the flags field is "reserved, must be 0".
	if (pHdr->GetFlags())
		return NULL;
	
    // The pointer is now at the first stream in the list.
    return ((PSTORAGESTREAM) pbMd);
} // PSTORAGESTREAM MDFormat::GetFirstStream()
