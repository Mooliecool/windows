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
#ifndef PARSE_H
#define PARSE_H

#define CTSTRLEN(s) (sizeof(s)/sizeof(TCHAR) - 1)
#define RFC1766_KEY_SZ L"MIME\\Database\\Rfc1766"

#define NAME_BUF_SIZE MAX_PATH
#define VALUE_BUF_SIZE MAX_PATH

// Used for generating display name.
#define FLAG_QUOTE          0x1
#define FLAG_DELIMIT        0x2

#define PARSE_FLAGS_LCID_TO_SZ 0x1
#define PARSE_FLAGS_SZ_TO_LCID 0x2

#define DISPLAY_NAME_ESCAPE_CHAR L'\\'

typedef enum tagFusionNameParsedFlags{
    FUSION_NAME_PARSED_VERSION = 0x1,
    FUSION_NAME_PARSED_FILE_VERSION = 0x2,
    FUSION_NAME_PARSED_PUBLIC_KEY = 0x4,
    FUSION_NAME_PARSED_PUBLIC_KEY_TOKEN = 0x8,
    FUSION_NAME_PARSED_CULTURE = 0x10,
    FUSION_NAME_PARSED_CUSTOM = 0x20,
    FUSION_NAME_PARSED_RETARGET = 0x40,
    FUSION_NAME_PARSED_CONFIG_MASK = 0x80,
    FUSION_NAME_PARSED_PROCESSOR_ARCHITECTURE = 0x100,
    FUSION_NAME_PARSED_MVID = 0x200
}FusionNameParsedFlags;
    

// ---------------------------------------------------------------------------
// CParseUtils
// Generic parsing utils.
// ---------------------------------------------------------------------------
class CParseUtils
{

public:

    // Inline strip leading and trailing whitespace.
    static VOID TrimWhiteSpace(__deref_inout_ecount(*pcc) LPWSTR *ppsz, __inout LPDWORD pcc);

    // inline strip outer quote pairs
    static VOID TrimOuterQuotePairs(__deref_inout_ecount(*pcc) LPWSTR *ppsz, __inout LPDWORD pcc);

    // Inline parse of delimited token.
    static BOOL GetDelimitedToken(
                __deref_inout_ecount(*pccBuf) LPWSTR* pszBuf,   __inout LPDWORD pccBuf,
                __deref_out_ecount(*pccTok+1) LPWSTR* pszTok,   __out LPDWORD pccTok,
                WCHAR cDelim, BOOL bEscapeDelim, LPBOOL pbContainDelim);
        
    // Inline parse of key=value token.
    static BOOL GetKeyValuePair(
            __in_ecount(ccB) LPWSTR  szB,    DWORD ccB,
            __deref_out_ecount(*pccK+1) LPWSTR* pszK,   __out LPDWORD pccK,
            __deref_out_ecount(*pccV+1) LPWSTR* pszV,   __out LPDWORD pccV);

    // Converts binary to hex encoded unicode string.
    static VOID BinToUnicodeHex(const BYTE * pSrc, UINT cSrc, __out_ecount(2*cSrc+1) LPWSTR pDst);

    // Converts hex encoded unicode string to binary.
    static VOID UnicodeHexToBin(LPCWSTR pSrc, UINT cSrc, LPBYTE pDest);

    // Inline trim cDelim pairs
    static VOID UnEscapeDelim(__inout_z LPWSTR pwzBuf);

    // Escape delimiter, assume the output has enough buffer
    // This technically does not belong here. But just to couple it 
    // with all the escaping/parsing code.
    static void EscapeDelim(LPCWSTR pszIn, __out_z LPWSTR pszOut, WCHAR cDelim);
};

#endif // PARSE_H
