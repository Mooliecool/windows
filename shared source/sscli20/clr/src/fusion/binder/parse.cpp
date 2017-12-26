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
#include <fusionp.h>
#include "parse.h"
#include "helpers.h"

//--------------------------------------------------------------------
// CParseUtils::TrimWhiteSpace
//   Inplace trim of leading and trailing whitespace.
//--------------------------------------------------------------------
VOID CParseUtils::TrimWhiteSpace(__deref_inout_ecount(*pcc)  LPWSTR *ppsz, __inout LPDWORD pcc)
{
    DWORD cc = *pcc;
    WCHAR* beg = *ppsz;
    WCHAR* end = beg + cc - 1;

    while ((cc != 0) && iswspace(*beg))
    {
        beg++;
        cc--;
    }

    while ((cc != 0) && iswspace(*end))
    {
        *end = L'\0';
        end--;
        cc--;
    }

    *ppsz = beg;
    *pcc = cc;
}

/* static */
VOID CParseUtils::TrimOuterQuotePairs(__deref_inout_ecount(*pcc) LPWSTR *ppsz, __inout LPDWORD pcc)
{
    WCHAR* beg = *ppsz;
    WCHAR* end = beg + *pcc - 1;

    if ((end > beg) && ((*beg == L'\"' || *beg == L'\''))) {
        if (*end == *beg) {
            beg++;
            end--;
        }
    }

    *ppsz = beg;
    *pcc = (DWORD)(end - beg) + 1;
}

//--------------------------------------------------------------------
// CParseUtils::GetDelimitedToken
// Inplace strtok based on one delimiter. Ignores delimiter scoped by quotes.
//--------------------------------------------------------------------
BOOL CParseUtils::GetDelimitedToken(
                                    __deref_inout_ecount(*pccBuf) LPWSTR* pszBuf,   __inout LPDWORD pccBuf,
                                    __deref_out_ecount((*pccTok+1)) LPWSTR* pszTok,   __out LPDWORD pccTok,
                                    WCHAR  cDelim,  BOOL bEscape,
                                    LPBOOL pbEscaped)
{
    WCHAR *pEnd;
    BOOL fQuote = FALSE,
         fRet   = FALSE;

    *pccTok = 0;
    *pszTok = *pszBuf;

    pEnd = *pszBuf + *pccBuf - 1;

    while (*pccBuf)
    {
        if ( ((**pszBuf == cDelim) && !fQuote)
            || (**pszBuf == L'\0'))
        {
            fRet = TRUE;
            break;
        }
        else if (bEscape && **pszBuf == DISPLAY_NAME_ESCAPE_CHAR) {
            if (pbEscaped) {
                *pbEscaped= TRUE;
            }
            (*pszBuf)++;
            (*pccBuf)--;
        }
        
        if (**pszBuf == L'"')
            fQuote = !fQuote;

        (*pszBuf)++;
        (*pccBuf)--;
    }

    if (fRet)
    {
        *pccBuf = (DWORD)(pEnd - *pszBuf);
        *pccTok = (DWORD)(*pszBuf - *pszTok);

        TrimWhiteSpace(pszTok, pccTok);
        TrimOuterQuotePairs(pszTok, pccTok);

        if (**pszBuf == cDelim)
            (*pszBuf)++;
    }

    return fRet;
}

//--------------------------------------------------------------------
// CParseUtils::GetKeyValuePair
// Inplace retrieval of key and value from a buffer of form key = <">value<">
//--------------------------------------------------------------------
BOOL CParseUtils::GetKeyValuePair(
            __in_ecount(ccB) LPWSTR  szB,    DWORD ccB,
            __deref_out_ecount(*pccK+1) LPWSTR* pszK,   __out LPDWORD pccK, 
            __deref_out_ecount(*pccV+1) LPWSTR* pszV,   __out LPDWORD pccV)
{
    if (GetDelimitedToken(&szB, &ccB, pszK, pccK, L'=', FALSE, NULL))
    {
        TrimWhiteSpace(pszK, pccK);
        TrimOuterQuotePairs(pszK, pccK);

        if (ccB)
        {
            *pszV = szB;
            *pccV = ccB;
            TrimWhiteSpace(pszV, pccV);
            TrimOuterQuotePairs(pszV, pccV);
        }
        else
        {
            *pszV = NULL;
            *pccV = 0;
        }
        return TRUE;
    }
    else
    {
        *pszK  = *pszV  = NULL;
        *pccK  = *pccV = 0;
    }
    return FALSE;
}

//--------------------------------------------------------------------
// CParseUtils::BinToUnicodeHex
//--------------------------------------------------------------------
VOID CParseUtils::BinToUnicodeHex(const BYTE *pSrc, UINT cSrc, __out_ecount(2*cSrc+1) LPWSTR pDst)
{
    UINT x;
    UINT y;

#define TOHEX(a) ((a)>=10 ? L'a'+(a)-10 : L'0'+(a))

    for ( x = 0, y = 0 ; x < cSrc ; ++x )
    {
        UINT v;
        v = pSrc[x]>>4;
        pDst[y++] = TOHEX( v );  
        v = pSrc[x] & 0x0f;                 
        pDst[y++] = TOHEX( v ); 
    }                                    
    pDst[y] = L'\0';
}

//--------------------------------------------------------------------
// CParseUtils::UnicodeHexToBin
//--------------------------------------------------------------------
VOID CParseUtils::UnicodeHexToBin(LPCWSTR pSrc, UINT cSrc, LPBYTE pDest)
{
    BYTE v;
    LPBYTE pd = pDest;
    LPCWSTR ps = pSrc;

    for (UINT i = 0; i < cSrc-1; i+=2)
    {
        v =  FROMHEX(TOLOWER(ps[i])) << 4;
        v |= FROMHEX(TOLOWER(ps[i+1]));
       *(pd++) = v;
    }
}

VOID CParseUtils::UnEscapeDelim(__inout_z LPWSTR pszBuf)
{
    WCHAR *pScan = pszBuf;
    WCHAR *pCopy = pScan;

    while ( *pScan ) {
        if (*pScan == DISPLAY_NAME_ESCAPE_CHAR ) {
            pScan++;
        }

        if (pScan != pCopy) {
            *pCopy = *pScan;
        }
        pCopy++;
        pScan++;
    }
    *pCopy = L'\0';
}

void CParseUtils::EscapeDelim(LPCWSTR pszIn, 
            __out_z LPWSTR pszOut, WCHAR cDelim)
{
    LPCWSTR psz = pszIn;
    LPWSTR pszBuf = pszOut;

    while (*psz) {
        if (*psz == cDelim || *psz == DISPLAY_NAME_ESCAPE_CHAR ) {
            *pszBuf++ = DISPLAY_NAME_ESCAPE_CHAR ;
        }
        *pszBuf = *psz;
        pszBuf++;
        psz++;
    }
    *pszBuf = L'\0';
}
