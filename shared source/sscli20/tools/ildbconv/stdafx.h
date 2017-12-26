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
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>

#include "windef.h"
#include "crtdbg.h"
#include "malloc.h"

#include "unknwn.h"
#include "corhdr.h"
#include "corsym.h"
#include "cor.h"

// Debug Info
struct RSDSI                       // RSDS debug info
{
    DWORD   dwSig;                 // RSDS
    GUID    guidSig;
    DWORD   age;
    char    szPdb[0];  // followed by a zero-terminated UTF8 file name
};


class MapFile
{
public:
    PIMAGE_SECTION_HEADER ImageRvaToSection(ULONG Rva);
    PVOID ImageRvaToVa(ULONG Rva);
    HRESULT FindHeaders();
    IMAGE_COR20_HEADER *FindCorHeader();
    IMAGE_DEBUG_DIRECTORY *FindDebugEntry();
    RSDSI *FindDebugInfo();

    HRESULT FileMapping(
            HANDLE hFileMappingObject,   // handle to file-mapping object
            DWORD dwDesiredAccess,       // access mode
            SIZE_T dwNumberOfBytesToMap  // number of bytes to map
            );

    MapFile()
    {
        // Just clear everything
        memset(this, 0, sizeof(MapFile));
    }

    ~MapFile()
    {
        if (m_pbMapAddress)
        {
            UnmapViewOfFile(m_pbMapAddress);
            m_pbMapAddress = NULL; 
        }
    }

private:
    bool  m_bIs64;    // Is this a 64 bit image
    IMAGE_NT_HEADERS32     *m_pNT32;  // Pointer if 32 bit image
    IMAGE_NT_HEADERS64     *m_pNT64;  // Pointer if 64 bit image
    BYTE *m_pbMapAddress;  // Map address of the exe we're working with

};

// Useful struct
struct DocumentEntry
{
    ISymUnmanagedDocumentWriter *pDocumentWriter;
    WCHAR *wcsUrl;
};


//-----------------------------------------------------------
// UnicodeToUTF8
// Translate the Unicode string to a UTF8 string
// Return the length in UTF8 of the Unicode string
//    Including NULL terminator
//-----------------------------------------------------------
inline int WINAPI UnicodeToUTF8(
    PCWSTR pUni,  // Unicode string
    PSTR pUTF8,   // [optional, out] Buffer for UTF8 string
    int cbUTF     // length of UTF8 buffer
)
{
    // Pass in the length including the NULL terminator
    int cchSrc = (int)wcslen(pUni)+1;
    return WideCharToMultiByte(CP_UTF8, 0, pUni, cchSrc, pUTF8, cbUTF, NULL, false);
}

inline HRESULT HrFromWin32(DWORD dwWin32Error)
{
    return HRESULT_FROM_WIN32(dwWin32Error);
}

//---------------------------------------------------------------------------
// Cover for GetLastError that return an HRESULT.  If last error was not set,
// then we are forced to E_FAIL.
//---------------------------------------------------------------------------
HRESULT HrLastError()
{
  long lErr = GetLastError();
  return lErr ? HRESULT_FROM_WIN32(lErr) : E_FAIL;
}

// Define lengthof macro - length of an array.
#define lengthof(a) (sizeof(a) / sizeof((a)[0]))

#ifndef IfFailGoto
#define IfFailGoto(EXPR, LABEL) \
do { hr = (EXPR); if(FAILED(hr)) { goto LABEL; } } while (0)
#endif

#ifndef IfFailRet
#define IfFailRet(EXPR) \
do { hr = (EXPR); if(FAILED(hr)) { return (hr); } } while (0)
#endif

#ifndef IfFailGo
#define IfFailGo(EXPR) IfFailGoto(EXPR, Error)
#endif

#ifndef IfNullGo
#define IfNullGo(EXPR) if (EXPR == NULL) { hr = E_OUTOFMEMORY; goto Error; }
#endif

#ifndef IfFalseGo
#define IfFalseGo(EXPR, ERRORCODE) if (!EXPR) { hr = ERRORCODE; goto Error; }
#endif

#ifndef RELEASE
#define RELEASE(lpUnk) if ((lpUnk) != NULL) { (lpUnk)->Release(); (lpUnk) = NULL; } 
#endif
