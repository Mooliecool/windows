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
// File: shstr.cpp
//
// ShStr class ported from shlwapi for urlpars.cpp (especially for Fusion)
// ===========================================================================

#include "shlwapip.h"
#include "shstr.h"

//
//  ShStr Public Methods
//

//
//  Constructors
//

ShStr :: ShStr () 
{
    _szDefaultBuffer[0] = '\0';
    _pszStr = _szDefaultBuffer;
    _cchSize = ARRAYSIZE(_szDefaultBuffer);
}


/**************
    StrStr SetStr() methods

  Return:   
    Success - a pointer to the object
    Failure - NULL
**************/

HRESULT 
ShStr :: SetStr (LPCSTR pszStr)
{
    Reset();
    
    return _SetStr(pszStr);

}

HRESULT 
ShStr :: SetStr (LPCSTR pszStr, DWORD cchStr)
{
    Reset();
    
    return _SetStr(pszStr, cchStr);

}

HRESULT 
ShStr :: SetStr (LPCWSTR pwszStr, DWORD cchStr)
{
    Reset();
    
    return _SetStr(pwszStr, cchStr);

}


HRESULT
ShStr :: Append(LPCTSTR pszStr, DWORD cchStr)
{
    HRESULT hr = S_OK;

    if(pszStr)
    {
        DWORD cchLen = GetLen();

        if(cchStr == (DWORD) -1)
            cchStr = lstrlen(pszStr);

        //
        //  StrCpyN automagically appends the null term, 
        //  so we need to give room for it
        //
        cchStr++;

        if(SUCCEEDED(SetSize(cchStr + cchLen)))
            StrCpyN(_pszStr + cchLen, pszStr, cchStr);
        else
            hr = E_OUTOFMEMORY;
    }
    else
        hr = E_INVALIDARG;

    return hr;
}


/**************
    ShStr Utility methods

**************/


/**************
    ShStr SetSize method

    Sets the size of the internal buffer if larger than default

  Return:   
    Success - a pointer to the object
    Failure - NULL
**************/
HRESULT
ShStr :: SetSize(DWORD cchSize)
{
    HRESULT hr = S_OK;
    DWORD cchNewSize = _cchSize;

    ASSERT(!(_cchSize % DEFAULT_SHSTR_LENGTH));

    // so that we always allocate in increments
    while (cchSize > cchNewSize)
        cchNewSize <<= 2;
    
    if(cchNewSize != _cchSize)
    {
        if(cchNewSize > DEFAULT_SHSTR_LENGTH)
        {
            LPTSTR psz;

            psz = (LPTSTR) LocalAlloc(0, CbFromCch(cchNewSize));
    
            if(psz)
            {
                StrCpyN(psz, _pszStr, cchSize);
                Reset();
                _cchSize = cchNewSize;
                _pszStr = psz;
            }
            else 
                hr = E_OUTOFMEMORY;
        }
        else
        {
            if (_pszStr && _cchSize) 
                StrCpyN(_szDefaultBuffer, _pszStr, ARRAYSIZE(_szDefaultBuffer));

            Reset();

            _pszStr = _szDefaultBuffer;
        }
    }

    return hr;
}

#ifdef DEBUG
BOOL
ShStr :: IsValid()
{
    BOOL fRet = TRUE;

    if(!_pszStr)
        fRet = FALSE;

    ASSERT( ((_cchSize != ARRAYSIZE(_szDefaultBuffer)) && (_pszStr != _szDefaultBuffer)) ||
            ((_cchSize == ARRAYSIZE(_szDefaultBuffer)) && (_pszStr == _szDefaultBuffer)) );

    ASSERT(!(_cchSize % DEFAULT_SHSTR_LENGTH));

    return fRet;
}
#endif //DEBUG

VOID 
ShStr :: Reset()
{
    if (_pszStr && (_cchSize != ARRAYSIZE(_szDefaultBuffer))) 
        LocalFree(_pszStr);

    _szDefaultBuffer[0] = TEXT('\0');
    _pszStr = _szDefaultBuffer;
    _cchSize = ARRAYSIZE(_szDefaultBuffer);
}


//
//  ShStr Private Methods
//


/**************
    StrStr Set* methods

  Return:   
    Success - a pointer to the object
    Failure - NULL
**************/
HRESULT 
ShStr :: _SetStr(LPCSTR pszStr)
{
    HRESULT hr = S_FALSE;

    if(pszStr)
    {
        DWORD cchStr;

        cchStr = lstrlenA(pszStr);
    
        if(cchStr)
        {
            hr = SetSize(cchStr +1);

            if (SUCCEEDED(hr))
#ifdef UNICODE
                MultiByteToWideChar(CP_ACP, 0,
                    pszStr, -1,
                    _pszStr, _cchSize);
#else //!UNICODE
                lstrcpyA(_pszStr, pszStr);
#endif //UNICODE
        }
    }

    return hr;
}

HRESULT 
ShStr :: _SetStr(LPCSTR pszStr, DWORD cchStr)
{
    HRESULT hr = S_FALSE;

    if(pszStr && cchStr)
    {
        if (cchStr == (DWORD) -1)
            cchStr = lstrlenA(pszStr);

        hr = SetSize(cchStr +1);

        if(SUCCEEDED(hr))
        {
#ifdef UNICODE
            MultiByteToWideChar(CP_ACP, 0,
                pszStr, cchStr,
                _pszStr, _cchSize);
            _pszStr[cchStr] = TEXT('\0');

#else //!UNICODE
            StrCpyN(_pszStr, pszStr, (++cchStr < _cchSize ? cchStr : _cchSize) );
#endif //UNICODE
        }
    }

    return hr;
}

HRESULT 
ShStr :: _SetStr (LPCWSTR pwszStr, DWORD cchStrIn)
{
    DWORD cchStr = cchStrIn;
    HRESULT hr = S_FALSE;

    if(pwszStr && cchStr)
    {
        if (cchStr == (DWORD) -1)
#ifdef UNICODE
            cchStr = lstrlen(pwszStr);
#else //!UNICODE
        cchStr = WideCharToMultiByte(CP_ACP, 0,
            pwszStr, cchStrIn,
            NULL, 0,
            NULL, NULL);
#endif //UNICODE

        if(cchStr)
        {
            hr = SetSize(cchStr +1);

            if(SUCCEEDED(hr))
            {
#ifdef UNICODE 
                StrCpyN(_pszStr, pwszStr, (cchStr + 1< _cchSize ? cchStr + 1: _cchSize));
#else //!UNICODE
                cchStr = WideCharToMultiByte(CP_ACP, 0,
                    pwszStr, cchStrIn,
                    _pszStr, _cchSize,
                    NULL, NULL);
                _pszStr[cchStr < _cchSize ? cchStr : _cchSize] = TEXT('\0');
                ASSERT (cchStr);
#endif //UNICODE
            }
        }
#ifdef DEBUG
        else
        {
            DWORD dw;
            dw = GetLastError();
        }
#endif //DEBUG

    }
#ifdef DEBUG
    else
    {
        DWORD dw;
        dw = GetLastError();
    }
#endif //DEBUG

    return hr;
}
