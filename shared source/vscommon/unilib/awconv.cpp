// awconv.cpp - ANSI/wide conversions
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// June 1, 1998 [paulde]
//
//-----------------------------------------------------------------

#include "pch.h"
#include "awconv.h"
#include "vsmem.h"
#include "vsassert.h"

#if !defined(_M_CEE)
#pragma intrinsic(memcpy)
#endif

PSTR WINAPI CopyAFromWN(_Out_z_cap_(cchdest) PSTR dest, size_t cchdest, _In_opt_z_ PCWSTR src)
{
    if(dest == NULL)
    {
        VSASSERT(dest, "dest is null");
        return NULL;
    }

    if (cchdest <= 0)
    {
        VSASSERT(cchdest, "Can't copy a 0 length string");
        return NULL;
    }

    if (NULL == src || 0 == *src)
    {
        *dest = 0;
    }
    else
    {
        size_t cch = WideCharToMultiByte(CP_ACP, 0, src, -1, dest, (int)cchdest, NULL, NULL);
        dest[cchdest-1] = 0;
#ifdef _DEBUG
        if (!cch)
        {
            DWORD dw = GetLastError();
            VSASSERT(0, "WideCharToMultiByte failed");
        }
#endif
    }
    return dest;
}

PWSTR WINAPI CopyWFromAN(_Out_z_cap_(cchdest) PWSTR dest, size_t cchdest, _In_opt_z_ PCSTR src)
{
    if(dest == NULL)
    {
        VSASSERT(dest, "dest is null");
        return NULL;
    }

    if (cchdest <= 0)
    {
        VSASSERT(cchdest, "Can't copy a 0 length string");
        return NULL;
    }

    if (NULL == src || 0 == *src)
    {
        *dest = 0;
    }
    else
    {
        size_t cch = MultiByteToWideChar(CP_ACP, 0, src, -1, dest, (int)cchdest);
        dest[cchdest-1] = 0;
#ifdef _DEBUG
        if (!cch)
        {
            DWORD dw = GetLastError();
            VSASSERT(0, "MultiByteToWideChar failed");
        }
#endif
    }
    return dest;
}

HRESULT WINAPI StrDupW (_In_opt_z_ PCWSTR sz, _Deref_out_z_ PWSTR * ppszNew)
{
	if(ppszNew == NULL)
	{
		VSASSERT(false,"");
		return E_INVALIDARG;
	}
    *ppszNew = NULL;
    if (!sz)
        return S_OK;
    int  cb = (StrLen(sz) + 1)*sizeof(WCHAR);
    PWSTR pszNew = (PWSTR)VSAlloc(cb);
    if (NULL != pszNew)
    {
        memcpy(pszNew, sz, cb);
        *ppszNew = pszNew;
        return S_OK;
    }
    else
        return E_OUTOFMEMORY;
}

HRESULT WINAPI StrDupA (_In_opt_z_ PCSTR psz, _Deref_out_z_ PSTR * ppszNew)
{
    if(ppszNew == NULL)
    {
        VSASSERT(false,"");
        return E_INVALIDARG;
    }
    *ppszNew = NULL;
    if (!psz)
        return S_OK;
    int  cb = StrLenA(psz) + 1;
    PSTR pszNew = (PSTR)VSAlloc(cb);
    if (NULL != pszNew)
    {
        memcpy(pszNew, psz, cb);
        *ppszNew = pszNew;
        return S_OK;
    }
    else
        return E_OUTOFMEMORY;
}

HRESULT WINAPI StrDupAFromW (_In_opt_z_ PCWSTR psz, _Deref_out_z_ PSTR * ppszNew)
{
    return StrDupA(ANSISTR(psz), ppszNew);
}

HRESULT WINAPI StrDupWFromA (_In_opt_z_ PCSTR psz, _Deref_out_z_ PWSTR * ppszNew)
{
    return StrDupW(WIDESTR(psz), ppszNew);
}

BSTR WINAPI SysAllocStringFromA (_In_z_ PCSTR psz)
{
    return SysAllocString(WIDESTR(psz));
}

HRESULT WINAPI StrNDupW (_In_opt_ _Pre_count_(cchMax) PCWSTR pszSrc, unsigned int cchMax,
                         _Deref_out_z_ PWSTR * ppszCopy)
{
    EXPECTEDPTR(ppszCopy);
    // pszSrc may not be zero terminated, so we can't use StrLen and
    // we must scan no more than cchMax.
    unsigned int cch = 0;
    PWSTR p;
    if (pszSrc)
    {
        for (p = (PWSTR)pszSrc; *p; p++, cch++)
        {
            if (cch >= cchMax)
                break;
        }
    }

    p = VSALLOCSTR(cch+1);
    *ppszCopy = p;
    if (p)
    {
        memcpy(p, pszSrc, sizeof(WCHAR)*cch);
        p[cch] = 0;
    }
    else
        return E_OUTOFMEMORY;
    return S_OK;
}

BSTR WINAPI SysNDup (_In_opt_ _Pre_count_(cchMax) PCWSTR pszSrc, unsigned int cchMax)
{
    // pszSrc may not be zero terminated, so we can't use StrLen and
    // we must scan no more than cchMax.
    unsigned int cch = 0;
    if (pszSrc)
    {
        for (PCWSTR pch = pszSrc; *pch; pch++, cch++)
        {
            if (cch >= cchMax)
                break;
        }
    }

    BSTR bs = SysAllocStringLen(pszSrc, cch);
    return bs;
}

