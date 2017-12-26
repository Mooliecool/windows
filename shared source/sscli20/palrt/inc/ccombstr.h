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
// File: CComBSTR.h 
// 
// ===========================================================================

/*++

Abstract:

    Stripped down and modified version of CComBSTR

--*/

#ifndef __CCOMBSTR_H__
#define __CCOMBSTR_H__

#ifdef __cplusplus

#define AtlThrow(a) RaiseException(STATUS_NO_MEMORY,EXCEPTION_NONCONTINUABLE,0,NULL); 
#define ATLASSERT(a) _ASSERTE(a)
#define MAX_SATELLITESTRING 1024

class CComBSTR
{
public:
    BSTR m_str;
    CComBSTR()
    {
        m_str = NULL;
    }
    CComBSTR(int nSize)
    {
        if (nSize == 0)
            m_str = NULL;
        else
        {
            m_str = ::SysAllocStringLen(NULL, nSize);
            if (m_str == NULL)
                AtlThrow(E_OUTOFMEMORY);
        }
    }
    CComBSTR(int nSize, LPCOLESTR sz)
    {
        if (nSize == 0)
            m_str = NULL;
        else
        {
            m_str = ::SysAllocStringLen(sz, nSize);
            if (m_str == NULL)
                AtlThrow(E_OUTOFMEMORY);
        }
    }
    CComBSTR(LPCOLESTR pSrc)
    {
        if (pSrc == NULL)
            m_str = NULL;
        else
        {
            m_str = ::SysAllocString(pSrc);
            if (m_str == NULL)
                AtlThrow(E_OUTOFMEMORY);
        }
    }

    CComBSTR(const CComBSTR& src)
    {
        m_str = src.Copy();
        if (!!src && m_str == NULL)
            AtlThrow(E_OUTOFMEMORY);

    }

    CComBSTR& operator=(const CComBSTR& src)
    {
        if (m_str != src.m_str)
        {
            ::SysFreeString(m_str);
            m_str = src.Copy();
            if (!!src && m_str == NULL)
                AtlThrow(E_OUTOFMEMORY);
        }
        return *this;
    }

    CComBSTR& operator=(LPCOLESTR pSrc)
    {
        if (pSrc != m_str)
        {
            ::SysFreeString(m_str);
            if (pSrc != NULL)
            {
                m_str = ::SysAllocString(pSrc);
                if (m_str == NULL)
                    AtlThrow(E_OUTOFMEMORY);
            }
            else
                m_str = NULL;
        }
        return *this;
    }

    ~CComBSTR()
    {
        ::SysFreeString(m_str);
    }
    unsigned int ByteLength() const
    {
        return (m_str == NULL)? 0 : SysStringByteLen(m_str);
    }
    unsigned int Length() const
    {
        return (m_str == NULL)? 0 : SysStringLen(m_str);
    }
    operator BSTR() const
    {
        return m_str;
    }
    BSTR* operator&()
    {
        return &m_str;
    }
    BSTR Copy() const
    {
        if (m_str == NULL)
            return NULL;
        return ::SysAllocStringLen(m_str, SysStringLen(m_str));
    }
    HRESULT CopyTo(BSTR* pbstr)
    {
        ATLASSERT(pbstr != NULL);
        if (pbstr == NULL)
            return E_POINTER;
        *pbstr = Copy();
        if ((*pbstr == NULL) && (m_str != NULL))
            return E_OUTOFMEMORY;
        return S_OK;
    }
    // copy BSTR to VARIANT
    HRESULT CopyTo(VARIANT *pvarDest)
    {
        ATLASSERT(pvarDest != NULL);
        HRESULT hRes = E_POINTER;
        if (pvarDest != NULL)
        {
            V_VT (pvarDest) = VT_BSTR;
            V_BSTR (pvarDest) = Copy();
            if (V_BSTR (pvarDest) == NULL && m_str != NULL)
                hRes = E_OUTOFMEMORY;
            else
                hRes = S_OK;
        }
        return hRes;
    }

    void Attach(BSTR src)
    {
        if (m_str != src)
        {
            ::SysFreeString(m_str);
            m_str = src;
        }
    }
    BSTR Detach()
    {
        BSTR s = m_str;
        m_str = NULL;
        return s;
    }
    void Empty()
    {
        ::SysFreeString(m_str);
        m_str = NULL;
    }
    HRESULT Append(LPCOLESTR lpsz)
    {
        return Append(lpsz, UINT(lstrlenW(lpsz)));
    }

    HRESULT Append(LPCOLESTR lpsz, int nLen)
    {
        if (lpsz == NULL || (m_str != NULL && nLen == 0))
            return S_OK;
        int n1 = Length();
        BSTR b;
        b = ::SysAllocStringLen(NULL, n1+nLen);
        if (b == NULL)
            return E_OUTOFMEMORY;
        memcpy(b, m_str, n1*sizeof(OLECHAR));
        memcpy(b+n1, lpsz, nLen*sizeof(OLECHAR));
        b[n1+nLen] = NULL;
        SysFreeString(m_str);
        m_str = b;
        return S_OK;
    }

    HRESULT AssignBSTR(const BSTR bstrSrc)
    {
        HRESULT hr = S_OK;
        if (m_str != bstrSrc)
        {
            ::SysFreeString(m_str);
            if (bstrSrc != NULL)
            {
                m_str = SysAllocStringLen(bstrSrc, SysStringLen(bstrSrc));
                if (m_str == NULL)
                    hr = E_OUTOFMEMORY; 
            }
            else
                m_str = NULL;
        }
        
        return hr;
    }

    bool LoadString(HSATELLITE hInst, UINT nID)
    {
        ::SysFreeString(m_str);
        m_str = NULL;
        WCHAR SatelliteString[MAX_SATELLITESTRING];
        if (PAL_LoadSatelliteStringW(hInst, nID, SatelliteString, MAX_SATELLITESTRING))
        {
            m_str = SysAllocString(SatelliteString);
        }
        return m_str != NULL;
    }

    bool LoadString(PVOID hInst, UINT nID)
    {
        return LoadString ((HSATELLITE)hInst, nID);
    }

    CComBSTR& operator+=(LPCOLESTR pszSrc)
    {
        HRESULT hr;
        hr = Append(pszSrc);
        if (FAILED(hr))
            AtlThrow(hr);
        return *this;
    }

};
#endif // __cplusplus
#endif // __CCOMBSTR_H__
