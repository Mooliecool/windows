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
// File: loadstring.cpp
// 
// ===========================================================================

/*============================================================
**
**
** Purpose: Functions for loading strings from satellite files
**
** Date:  July 26, 2001
** 
**
===========================================================*/

#include "rotor_palrt.h"


#define MAX_SAT_STRING_LENGTH	511

typedef struct _HSATELLITE
{
    LPBYTE  rgb;
    UINT    cbMax;
} SATELLITE, *HSATELLITE, *PSATELLITE;

#include <pshpack1.h>
typedef struct SATTBL
{
    WORD    wID;
    ULONG   lOffset;
} SATTBL, *PSATTBL;
#include <poppack.h>

static HSATELLITE LoadSatelliteResourceFromHandle(HANDLE hfileSatellite)
{
    HANDLE      hFileMap = NULL;
    PSATELLITE  psat = NULL;
    BOOL        fSuccess = FALSE;

    if (hfileSatellite == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }

    psat = (PSATELLITE)malloc(sizeof(SATELLITE));
    if (psat == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Exit;
    }

    psat->cbMax = GetFileSize(hfileSatellite, NULL);

    // File must not be zero and big enough to have a valid signature
    if (psat->cbMax == INVALID_FILE_SIZE) 
    {
        goto Exit;
    }

    hFileMap = CreateFileMapping(hfileSatellite,
            NULL, //if null, gets default security
            PAGE_READONLY,
            0, //hmaxsize - 0 uses size of file
            0, //lmaxsize - 0 uses size of file
            NULL);  //Name
    if (hFileMap != NULL)
    {
        psat->rgb = (LPBYTE)MapViewOfFile(hFileMap,
                FILE_MAP_READ,
                0,
                0,
                0);

        if (psat->rgb) 
        {
            fSuccess = TRUE;
        }
    }

Exit:
    if (hFileMap != NULL)
    {
        CloseHandle(hFileMap);
    }

    CloseHandle(hfileSatellite);

    if (!fSuccess)
    {
        free(psat);
        psat = NULL;
    }

    return (HSATELLITE)psat;
}

HSATELLITE PALAPI PAL_LoadSatelliteResourceW(LPCWSTR lpwszSatelliteResourceFileName)
{
    return LoadSatelliteResourceFromHandle(CreateFileW(lpwszSatelliteResourceFileName,
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL));
}

HSATELLITE PALAPI PAL_LoadSatelliteResourceA(LPCSTR lpszSatelliteResourceFileName)
{
    return LoadSatelliteResourceFromHandle(CreateFileA(lpszSatelliteResourceFileName,
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL));
}

BOOL PALAPI PAL_FreeSatelliteResource(HSATELLITE hsatResource)
{
    PSATELLITE psat = (PSATELLITE)hsatResource;

    // Resource buffer better be valid
    if (psat == NULL || psat->rgb == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    BOOL fSuccess = UnmapViewOfFile((LPVOID)psat->rgb);
    psat->rgb = NULL;
    free(psat);

    return fSuccess;
}

/*       int DSatTblComp - function that compares two array
*               elements, returning neg when #1 < #2, pos when #1 > #2, and
*               0 when they are equal. Function is passed pointers to two
*               array elements.
*/
int __cdecl DSatTblComp(const void UNALIGNED * wID, PSATTBL UNALIGNED psattbl2)
{
    return (WORD)(UINT)wID - VAL16(psattbl2->wID);
}

UINT PALAPI PAL_LoadSatelliteStringW(
    HSATELLITE  hsatResource,
    UINT        uID,
    LPWSTR      lpBuffer,
    UINT        cwcBufferMax) 
{
    PSATELLITE  psat = (PSATELLITE)hsatResource;
    PSATTBL     psattbl;
    LPBYTE      pb = NULL;
    UINT        cwc = 0;
    PSATTBL     psattblLast = NULL;
    ULONG       iIndexMin;
    ULONG       dAlignmentAdjust;

    // Resource buffer better be valid
    if (psat == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }

    if (lpBuffer == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    // Validate internal data
    if (psat->rgb == NULL || psat->cbMax <= sizeof(ULONG))
    {
        SetLastError(ERROR_INVALID_DATA);
        return 0;
    }


    iIndexMin = VAL32(GET_UNALIGNED_32(psat->rgb + psat->cbMax - sizeof(ULONG)));

    // Force WORD aligned access for beginning of table
    dAlignmentAdjust = (iIndexMin & 1);
    psattbl = (PSATTBL)(psat->rgb + iIndexMin + dAlignmentAdjust);

    // next to last entry for bounds of binary search
    psattblLast = (PSATTBL)(psat->rgb + psat->cbMax - sizeof(SATTBL));

    // look for actual entry
    psattbl = (PSATTBL)bsearch(
        (const void *)uID, 
        (const void *)psattbl, 
        psattblLast - psattbl, 
        sizeof(SATTBL), 
        (int (__cdecl *)(const void*, const void*))DSatTblComp
        );

    if (psattbl == NULL)
    {
        SetLastError(ERROR_NOT_FOUND);
        return 0;
    }
    else
    {
        ULONG   lOffsetCur = VAL32(psattbl->lOffset);
        ULONG   lOffsetNext = VAL32(((psattbl + 1)->lOffset));
        UINT    cch = lOffsetNext - lOffsetCur;

        pb = psat->rgb + lOffsetCur;

        // Identify length of buffer needed
        cwc = MultiByteToWideChar(CP_UTF8, 
                0,
                (LPCSTR)pb, 
                cch, 
                NULL, 
                0); 

        if (cwc > 0)
        {
            LPWSTR  lpwsz = lpBuffer;
            LPWSTR  lpwszT = NULL;

            if (cwc >= cwcBufferMax)
            {
                // Need a bigger destination buffer.  Alloca will
                // throw on failure.  If it does everything is toast
                lpwszT = (LPWSTR)alloca((cwc + 1) * sizeof(WCHAR));
                lpwsz = lpwszT;
            }

            // Convert data
            cwc = MultiByteToWideChar(CP_UTF8,
                    0,
                    (LPCSTR)pb, 
                    cch, 
                    lpwsz, 
                    cwc);

            // How much of real string can be returned
            cwc = min(cwc, cwcBufferMax - 1);

            if (lpwsz == lpwszT)
            {
                // Copy partial string from temporary buffer
                wcsncpy(lpBuffer, lpwsz, cwc);
            }

            // Always zero terminate
            lpBuffer[cwc] = '\0';
        }
    }

    return cwc;
}

UINT PALAPI PAL_LoadSatelliteStringA(
    HSATELLITE  hsatResource,
    UINT        uID,
    LPSTR       lpBuffer,
    UINT        cbBufferMax)
{
    UINT    cchASCII;
    LPWSTR  lpwszGood = (LPWSTR)malloc(cbBufferMax * sizeof(WCHAR));

    if (lpwszGood == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }

    if(!PAL_LoadSatelliteStringW(hsatResource,
            uID,
            lpwszGood,
            cbBufferMax))
    {
        free(lpwszGood);
        return 0;
    }

    cchASCII = WideCharToMultiByte(CP_ACP, 
            0,
            lpwszGood, 
            wcslen(lpwszGood), 
            NULL, 
            0,
            NULL,
            NULL); 

    cchASCII = min(cchASCII, cbBufferMax);

    if (cchASCII == 0)
    {
        free(lpwszGood);
        return 0;
    }

    cchASCII = WideCharToMultiByte(CP_ACP, 
            0,
            lpwszGood, 
            wcslen(lpwszGood), 
            lpBuffer, 
            cchASCII,
            NULL,
            NULL);

    lpBuffer[cchASCII] = '\0';

    free(lpwszGood);

    return cchASCII;
}
