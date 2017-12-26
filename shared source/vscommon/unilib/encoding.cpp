// encoding.cpp
#include "pch.h"
//#include <objbase.h>
#include "encoding.h"
#include "vsassert.h"
#include "vsmem.h"
#include "dbgout.h"

#if !defined(_M_CEE)
#pragma intrinsic (strcmp, memcmp)
#endif

//================================================================
void    WINAPI FreeCharsetResources (void);
HRESULT WINAPI GetIMultiLanguage    (IMultiLanguage ** ppi);
HRESULT WINAPI GetIMultiLanguage2   (IMultiLanguage2 ** ppi);
HRESULT WINAPI CodepageFromCharsetNameA (PCSTR  pszCharset, int cchName, UINT * pCodepage);
HRESULT WINAPI CodepageFromCharsetNameW (PCWSTR pszCharset, int cchName, UINT * pCodepage);

__declspec(thread) static IMultiLanguage* s_pMultiLanguage = nullptr;

void WINAPI FreeCharsetResources (void)
{
    IMultiLanguage * pML = s_pMultiLanguage;
    if (pML != nullptr)
    {
        pML->Release();
        s_pMultiLanguage = nullptr;
    }
}

HRESULT WINAPI GetIMultiLanguage (IMultiLanguage ** ppi)
{
    if (!ppi) return E_POINTER;
    HRESULT hr = S_OK;
    IMultiLanguage * pML = s_pMultiLanguage;
    if (!pML)
    {
        // We have a thread local cache, but it has not been initialized yet.
        hr = CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, 
            IID_IMultiLanguage, (void**)&pML);
        if (SUCCEEDED(hr) && pML)
        {
            s_pMultiLanguage = pML;
        }
    }
    if (SUCCEEDED(hr) && pML)
    {
        pML->AddRef(); // addref return val
    }

    *ppi = pML;
    return hr;
}

HRESULT WINAPI GetIMultiLanguage2 (IMultiLanguage2 ** ppi)
{
    if (!ppi) return E_POINTER;
    *ppi = 0;
    IMultiLanguage * pML = 0;
    HRESULT hr = GetIMultiLanguage(&pML);
    if (SUCCEEDED(hr))
        hr = pML->QueryInterface(IID_IMultiLanguage2, (void**)ppi);
    if (pML) pML->Release();
    return hr;
}

HRESULT WINAPI MLConvertStringToUnicode (
    DWORD                           dwEncoding,
    _In_count_(*pcSrcSize) CHAR *  pSrcStr,
    _Inout_opt_ UINT *                          pcSrcSize,
    _Out_bytecap_post_bytecount_(*pcDstSize, *pcDstSize) PWSTR  pDstStr,
    _Inout_ UINT *                          pcDstSize
    )
{
    HRESULT hr = E_FAIL;
    DWORD dwMode = 0;
    IMultiLanguage2 * pML2 = 0;
    if (SUCCEEDED(GetIMultiLanguage2(&pML2)))
    {
        hr = pML2->ConvertStringToUnicodeEx(&dwMode, dwEncoding, pSrcStr, pcSrcSize, pDstStr, pcDstSize, 0, NULL);
        RELEASE(pML2);
    }
    else
    {
        IMultiLanguage * pML  = 0;
        if (FAILED(GetIMultiLanguage(&pML)))
            return E_NOINTERFACE;
        hr = pML->ConvertStringToUnicode(&dwMode, dwEncoding, pSrcStr, pcSrcSize, pDstStr, pcDstSize);
        RELEASE(pML);
    }
    return hr;
}

HRESULT WINAPI MLConvertStringFromUnicode(
    DWORD       dwEncoding,
    _In_count_(*pcSrcSize) WCHAR *     pSrcStr,
    _Inout_ UINT *                              pcSrcSize,
    _Out_cap_post_count_(*pcDstSize, *pcDstSize) CHAR *     pDstStr,
    _Inout_ UINT *                              pcDstSize,
    DWORD                               dwFlag,
    _In_opt_z_ WCHAR *             lpFallBack
    )
{
    HRESULT hr = E_FAIL;
    DWORD dwMode = 0;
    IMultiLanguage2 * pML2 = 0;
    WCHAR* szDef = L"?";
    if (!lpFallBack) lpFallBack = szDef;
    if (SUCCEEDED(GetIMultiLanguage2(&pML2)))
    {
        hr = pML2->ConvertStringFromUnicodeEx(&dwMode, dwEncoding, pSrcStr, pcSrcSize, pDstStr, pcDstSize, dwFlag, lpFallBack);
        RELEASE(pML2);
    }
    else
    {
        IMultiLanguage *  pML  = 0;
        if (FAILED(GetIMultiLanguage(&pML)))
            return E_NOINTERFACE;
        hr = pML->ConvertStringFromUnicode(&dwMode, dwEncoding, pSrcStr, pcSrcSize, pDstStr, pcDstSize);
        RELEASE(pML);
    }
    return hr;
}

//================================================================

struct _tagCharset
{
    const WCHAR *   szName;
    UINT            cp;
};

// this can contain only items that map to Windows codepages or UTF-8
static const _tagCharset s_CharsetLookupTable[] = 
{
    {  L"utf-8",               65001  },
    {  L"shift_jis",             932  },
    {  L"ks_c_5601",             949  },
    {  L"ks_c_5601-1987",        949  },
    {  L"gb2312",                936  },
    {  L"big5",                  950  },
    {  L"unicode-1-1-utf-8",   65001  },
    {  L"unicode-2-0-utf-8",   65001  },
    {  L"macintosh",           10000  },
    {  0,                          0  }
};

static HRESULT CPFromCharSet (PCWSTR pszCharSet, UINT * pCp);

static UINT _LookupCodepage (PCWSTR pszCharset, const _tagCharset * pcs = s_CharsetLookupTable)
{
    PCWSTR psz;
    while (psz = pcs->szName)
    {
        if (0 == CompareCase(psz, pszCharset)) // assumes charset is normalized to lowercase
            break;
        pcs++;
    }
    return pcs->cp;
}

static bool _fAnsiToCharsetName (PCSTR src, int cchSrc, _Out_opt_ PWSTR dst)
{
    PCSTR p   = src;
    WCHAR ch;
    if(dst == NULL)
    {
        VSASSERT(false,"");
        return false;
    }

    if (-1 == cchSrc) cchSrc = (int) strlen(src);
    while (cchSrc--)
    {
        ch = *p++;
        if (ch > 'z' || ch < '-') 
            return false;
        else if (ch >= 'a')
        {
            // ok
        }
        else if (ch > 'Z')
        {
            if (ch != '_')
                return false;
        }
        else if (ch >= 'A')
            ch += ' ';
        else if (ch >= '0' && ch <= ':')
        {
            // ok
        }
        else if (ch == '-' || ch == '.')
        {
            // ok
        }
        else
            return false;
        *dst++ = (WCHAR)ch;
    }
    *dst = 0;
    return true;
}

static bool _fUnicodeToCharsetName (_In_z_ PCWSTR src, int cchSrc, _Out_cap_(cchDst) PWSTR dst, unsigned int cchDst)
{
    if(dst == NULL)
    {
        VSASSERT(false,"");
        return false;
    }

    PWSTR   dstEnd = dst + cchDst - 1;  //Set ----ide a character for the null termination
    if (dstEnd < dst)
    {
        VSASSERT(false,"");
        return false;
    }

    PCWSTR p   = src;
    WCHAR ch;
    if (-1 == cchSrc) cchSrc = StrLen(src);
    while (cchSrc--)
    {
        ch = *p++;
        if (ch > L'z' || ch < L'-') 
            return false;
        else if (ch >= L'a')
        {
            // ok
        }
        else if (ch > L'Z')
        {
            if (ch != L'_')
                return false;
        }
        else if (ch >= L'A')
            ch += L' ';
        else if (ch >= L'0' && ch <= L':')
        {
            // ok
        }
        else if (ch == L'-' || ch == L'.')
        {
            // ok
        }
        else
            return false;

        if (dst >= dstEnd)
        {
            return false;
        }
        *dst++ = ch;
    }
    *dst = 0;
    return true;
}

static HRESULT _CPFromCS (_In_z_ PWSTR pszCS, UINT * pCP)
{
    if(pCP == NULL || pszCS == NULL)
    {
        VSASSERT(false,"");
        return E_INVALIDARG;
    }
    HRESULT hr = S_OK;
    UINT    cp = 0;
    *pCP = 0;
    if (0 == memcmp(L"windows-", pszCS, 16))
    {
        pszCS += 8;
        PWSTR pchStop = pszCS;
        cp = wcstoul(pszCS, &pchStop, 10);
        if (pchStop == pszCS)
            return E_FAIL;
        else
            *pCP = cp;
    }
    else
    {
        cp = _LookupCodepage(pszCS);
        hr = cp ? S_OK : E_FAIL;
        if (FAILED(hr))
        {
            BSTR bsCS = SysAllocString(pszCS);
            if (bsCS)
            {
                MIMECSETINFO csi;
                memset(&csi, 0, sizeof(csi));

                IMultiLanguage2 * pML = 0;
                hr = GetIMultiLanguage2(&pML);
                if (pML)
                {
                    hr = pML->GetCharsetInfo(bsCS, &csi);
                    if (SUCCEEDED(hr))
                        cp = csi.uiInternetEncoding;
                    pML->Release();
                }
                else
                {
                    // fall back to IMultiLanguage
                    IMultiLanguage * pML = 0;
                    hr = GetIMultiLanguage(&pML);
                    if (pML)
                    {
                        hr = pML->GetCharsetInfo(bsCS, &csi);
                        if (SUCCEEDED(hr))
                            cp = csi.uiInternetEncoding;
                        pML->Release();
                    }
                }
                if(FAILED(hr))
                {
                    if(!wcscmp(bsCS,L"utf-8"))
                    {
                        cp = CP_UTF8;
                        hr = S_OK;
                    }
                }
                SysFreeString(bsCS);
            }
            else
                hr = E_OUTOFMEMORY;
        }
    }
    *pCP = cp;
    return hr;
}

HRESULT WINAPI CodepageFromCharsetNameW (PCWSTR pszCharset, int cch, UINT * pCodepage)
{
    HRESULT hr = S_OK;
    UINT    cp = 0;
    WCHAR   sz[50];

    *pCodepage = 0;

    if (-1 == cch) cch = StrLen(pszCharset);
    if (cch >= 50)
        return E_FAIL; // invalid

    if (_fUnicodeToCharsetName(pszCharset, cch, sz, _countof(sz)))
        hr = _CPFromCS(sz, pCodepage);
    else
        hr = E_FAIL;

    return hr;
}

HRESULT WINAPI CodepageFromCharsetNameA (PCSTR pszCharset, int cch, UINT * pCodepage)
{
    HRESULT hr = S_OK;
    UINT    cp = 0;
    WCHAR   sz[50];

    *pCodepage = 0;

    if (-1 == cch) cch = (int) strlen(pszCharset);
    if (cch >= 50)
        return E_FAIL; // invalid

    if (_fAnsiToCharsetName(pszCharset, cch, sz))
        hr = _CPFromCS(sz, pCodepage);
    else
        hr = E_FAIL;

    return hr;
}

//================================================================

#define _WIDE_ 0
#include "cset.cpp"
#undef _WIDE_

#define _WIDE_ 1
#include "cset.cpp"
#undef _WIDE_

HRESULT GetNextNameValue (
    PCWSTR      pch, 
    PCWSTR      pchEnd, 
    int *       pcchName, 
    PCWSTR *    ppchName,
    int *       pcchValue,
    PCWSTR *    ppchValue, 
    PCWSTR *    ppchNext
    )
{
    *ppchNext = *ppchName = *ppchValue = pch;
    *pcchName = *pcchValue = 0;
    HRESULT hr = S_OK;

    // scan name
    IfFailGo(ScanName(pch, pchEnd, ppchName, pcchName));
    pch = *ppchName + *pcchName;

    // skip whitespace
    while ((pch < pchEnd) && (*pch <= 32)) pch++; 
    if (pch >= pchEnd ) { hr = E_FAIL; goto Error; }

    // scan '='
    if (L'=' != *pch  ) { hr = E_FAIL; goto Error; }
    pch++;

    if (pch  >= pchEnd) { hr = E_FAIL; goto Error; }
    IfFailGo(ScanValue(pch, pchEnd, ppchValue, pcchValue));
    pch = *ppchValue + *pcchValue;
Error:
    *ppchNext = pch;
    return hr;
}

HRESULT WINAPI ScanXMLEncodingW (
    PCWSTR  pch, 
    int     cch, 
    UINT *  pCp
    )
{
    HRESULT hr = E_FAIL;
    if (!pCp) return E_POINTER;
    if (!pch || !cch) return E_INVALIDARG;
    *pCp = CP_INVALID;

    PCWSTR pchEnd = pch + cch;
    if (UCH_BOM == *pch)
    {
        *pCp = CP_UNICODE;
        pch++;
    }

    // skip whitespace
    while ((pch < pchEnd) && (*pch <= 32)) pch++; 
    // limit declaration length
    pchEnd = min(pchEnd, pch + 250);
    // must start with '<?xml ' (case-insensitive)
    if (pchEnd-pch < 6) return E_FAIL;

    if (!(  (L'<' == pch[0]) &&
            (L'?' == pch[1]) &&
            (L'x' == pch[2] || L'X' == pch[2]) &&
            (L'm' == pch[3] || L'M' == pch[3]) &&
            (L'l' == pch[4] || L'L' == pch[4]) &&
            (L' ' == pch[5])   ))
            return E_FAIL;

    pch += 6;
    hr = S_FALSE;

    PCWSTR pchName;
    PCWSTR pchValue;
    int cchName, cchValue;

#if 0
    // 'version = "x.x"' seems to be optional or can be in any oder within the tag

    // first name/attribute pair must be 'version="1.0"'
    IfFailRet(GetNextNameValue(pch, pchEnd, &cchName, &pchName, &cchValue, &pchValue, &pch));
    if ((cchName != 7) || memcmp(L"version", pchName, 14)) return E_FAIL;
#endif

    // look for 'encoding'
    for (;;)
    {
        hr = GetNextNameValue(pch, pchEnd, &cchName, &pchName, &cchValue, &pchValue, &pch);
        if (SUCCEEDED(hr))
        {
            if ((8 == cchName) && (0 == memcmp(L"encoding", pchName, 16)))
            {
                // adjust for quotes
                if (L'"' == *pchValue || L'\'' == *pchValue)
                {
                    pchValue++;
                    cchValue--;
                    if (L'"' == pchValue[cchValue-1] || L'\'' == pchValue[cchValue-1])
                      cchValue--;
                }
                // look up charset
                return CodepageFromCharsetNameW (pchValue, cchValue, pCp);
            }
            continue;
        }
        hr = S_FALSE;
        while ((pch < pchEnd) && (*pch <= 32)) pch++; 
        if (pch >= pchEnd)
            goto _Exit;
        if (L'?' == *pch)
        {
            ++pch;
            if (pch >= pchEnd || L'>' == *pch)
                goto _Exit;
        }
        // if here, invalid xml
        hr = E_FAIL;
        break;
    }
_Exit:
    if (S_FALSE == hr)  // parsed xml declaration ok, but no encoding specified
        *pCp = CP_UTF8; // pass back the XML default encoding UTF-8
    return hr;
}

HRESULT WINAPI ScanXMLEncodingA (
    PCSTR   pch, 
    int     cb, 
    UINT *  pCp
    )
{
    PCSTR   pEnd;
    PCSTR   pDeclaration;
    WCHAR   sz[256];
    int     cch;

    if (!pCp || !pch || !cb) return E_INVALIDARG;
    *pCp = CP_INVALID;
    pEnd = pch + cb;

    while ((pch < pEnd) && ((unsigned char)(*pch) <= 32)) pch++;

    // limit declaration length
    pEnd = min(pEnd, pch + 250);

    // must start with '<?xml ' (case-insensitive)
    if (pEnd-pch < 6) return E_FAIL;
    if (!(  ('<' == pch[0]) &&
            ('?' == pch[1]) &&
            ('x' == pch[2] || 'X' == pch[2]) &&
            ('m' == pch[3] || 'M' == pch[3]) &&
            ('l' == pch[4] || 'L' == pch[4]) &&
            (' ' == pch[5])   ))
            return E_FAIL;

    pDeclaration = pch;
    pch += 6;

    while ((pch < pEnd) && (*pch != '>')) pch++; 
    if (pch < pEnd) pch++;

    if ((int)(pch-pDeclaration) <= 0)
        return E_FAIL;
    cch = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pDeclaration, (int)(pch-pDeclaration), sz, _countof(sz));
    if(cch)
        sz[cch] = 0;
    else
        sz[_countof(sz)-1] = 0;
    return ScanXMLEncodingW (sz, cch, pCp);
}

