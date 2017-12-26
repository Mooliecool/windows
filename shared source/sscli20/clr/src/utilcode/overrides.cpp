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

#include "stdafx.h"
#include <stdlib.h>
#include "utilcode.h"
#include "corperm.h"

HRESULT STDMETHODCALLTYPE GetSecurityPolicyRegKey (__out WCHAR** ppszSecurityPolicy)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return E_OUTOFMEMORY);
    }
    CONTRACTL_END;

    DWORD dwLen=0;
    HRESULT hr=GetCORVersion(NULL, 0, &dwLen);
    if (hr!=HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
        return hr;

    size_t bufSize = _countof(FRAMEWORK_REGISTRY_KEY_W)+1+dwLen+_countof(KEY_COM_SECURITY_POLICY);
    NewHolder<WCHAR> key(new(nothrow) WCHAR[bufSize]);
    if (key==NULL)
        return E_OUTOFMEMORY;
    wcscpy_s(key, bufSize, FRAMEWORK_REGISTRY_KEY_W L"\\");
    hr=GetCORVersion(key+NumItems(FRAMEWORK_REGISTRY_KEY_W),dwLen,&dwLen);
    if (FAILED(hr))
        return hr;

    size_t offset = _countof(FRAMEWORK_REGISTRY_KEY_W)+dwLen-1;
    wcscpy_s(key+offset,bufSize - offset, KEY_COM_SECURITY_POLICY);
    key.SuppressRelease();
    *ppszSecurityPolicy = key;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ApplyCustomZoneOverride(DWORD* pdwZone)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(*pdwZone>=NumZones); 
        INJECT_FAULT(return E_OUTOFMEMORY);
    }
    CONTRACTL_END;

    NewHolder<WCHAR> key(NULL);
    HRESULT hr = GetSecurityPolicyRegKey(&key);
    if (FAILED(hr))
        return hr;
    if (REGUTIL::GetLong(KEY_COM_SECURITY_ZONEOVERRIDE, 0, key, HKEY_POLICY_ROOT)==1)
        *pdwZone=Internet;
    return S_OK;
};
