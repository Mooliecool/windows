// encoding.h

#pragma once
#include "mlang.h"      // IMultiLanguage, IMultiLanguage2

//------------------------------------------------------------------------------
// ScanHTMLCharSet
//
// S_OK    : found a properly formed META charset tag
// S_FALSE : lexically valid HTML, but no META tag found
// error   : not valid HTML
// 
HRESULT ScanHTMLCharSetA (PCSTR  pData, int cch, UINT * pCp);
HRESULT ScanHTMLCharSetW (PCWSTR pData, int cch, UINT * pCp);
HRESULT ScanXMLEncodingA (PCSTR  pData, int cch, UINT * pCp);
HRESULT ScanXMLEncodingW (PCWSTR pData, int cch, UINT * pCp);

// These parse the charset name from the data, returning a pointer to the start 
// of the name in the buffer (if specified) and the length fo the name.
//
HRESULT ScanHTMLCharSetNameA (PCSTR  pData, int cb,  PCSTR  * ppName,   int * pcchName );
HRESULT ScanHTMLCharSetNameW (PCWSTR pData, int cch, PCWSTR * ppchName, int * pcchName);
HRESULT ScanXMLEncodingNameA (PCSTR  pData, int cb,  PCSTR  * ppName,   int * pcchName );
HRESULT ScanXMLEncodingNameW (PCWSTR pData, int cch, PCWSTR * ppchName, int * pcchName);

// Get a Windows codepage from a W3C charset/encoding name.
// If cchName is -1, pszCharset is assumed to be 0-terminated.
// The returned codepage  may be a special codepage that can be  converted only by the 
// MLConvert* functions. If the Windows API IsValidCodepage fails, it may still be valid 
// for MLConvert*.
HRESULT WINAPI CodepageFromCharsetNameA (PCSTR  pszCharset, int cchName, UINT * pCodepage);
HRESULT WINAPI CodepageFromCharsetNameW (PCWSTR pszCharset, int cchName, UINT * pCodepage);

HRESULT WINAPI GetIMultiLanguage    (IMultiLanguage ** ppi);
HRESULT WINAPI GetIMultiLanguage2   (IMultiLanguage2 ** ppi);

// You get a UINT from codepage detection, and you pass a DWORD to MLConvert, but
// that's the way MLANG defined it. go figure.
//
// For more information on the params to the MLConvert* functions, see the latest 
// MLANG documentation on MSDN. You may need to use MSDN online at 
// http://msdn.microsoft.com/workshop/
//

HRESULT WINAPI MLConvertStringToUnicode (
    DWORD                           dwEncoding, // codepage returned from CodepageFromCharSetName
    _In_count_(*pcSrcSize) CHAR *  pSrcStr,
    _Inout_opt_ UINT *                          pcSrcSize,  // in/out, may be -1
    _Out_bytecap_post_bytecount_(*pcDstSize, *pcDstSize) PWSTR  pDstStr,    // in/out
    _Inout_ UINT *                          pcDstSize
    );

HRESULT WINAPI MLConvertStringFromUnicode (
    DWORD                               dwEncoding, // codepage returned from CodepageFromCharSetName
    _In_count_(*pcSrcSize)  WCHAR *     pSrcStr,
    _Inout_ UINT *                              pcSrcSize,  // in/out, may be -1
    _Out_cap_post_count_(*pcDstSize, *pcDstSize) CHAR *     pDstStr,
    _Inout_ UINT *                              pcDstSize,  // in/out
    DWORD                               dwFlag,     // MLCONVCHAR enum
    _In_opt_z_ WCHAR *             lpFallBack
    );

//------------------------------------------------------------------------------
// FreeCharsetResources - Free resources acquired by APIs in this file.
//
// You must call FreeCharsetResources once at app termination if you use any
// of CodepageFromCharsetNameA|W, ScanHTML|XML*, GetImageFormat, LoadTextImage*, MLConvert*
// otherwise you may fail to release cached resources.
//
void    WINAPI FreeCharsetResources (void);

