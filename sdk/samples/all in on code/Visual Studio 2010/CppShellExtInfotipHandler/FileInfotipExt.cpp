/****************************** Module Header ******************************\
Module Name:  FileInfotipExt.cpp
Project:      CppShellExtInfotipHandler
Copyright (c) Microsoft Corporation.

The code sample demonstrates creating a Shell infotip handler with C++. An 
infotip handler is a shell extension handler that provides pop-up text when 
the user hovers the mouse pointer over the object. It is the most flexible 
way to customize infotips. The alternative way is to specify either a fixed 
string or a list of certain file properties to be displayed (See the Infotip 
Customization section in http://msdn.microsoft.com/en-us/library/cc144105.aspx)

The example infotip handler customizes the infotips of .cpp file objects. 
When you hover your mouse pointer over a .cpp file object in the Windows 
Explorer, you will see an infotip with the text:

    File: <File path, e.g. D:\Test.cpp>
    Lines: <Line number, e.g. 123 or N/A>
    - Infotip displayed by CppShellExtInfotipHandler

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "FileInfotipExt.h"
#include <fstream>
#include <string>
#include <strsafe.h>
using namespace std;


FileInfotipExt::FileInfotipExt() : m_cRef(1)
{
}

FileInfotipExt::~FileInfotipExt()
{
}


#pragma region IUnknown

// Query to the interface the component supported.
IFACEMETHODIMP FileInfotipExt::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT hr = S_OK;

    if (IsEqualIID(IID_IUnknown, riid) || 
        IsEqualIID(IID_IQueryInfo, riid))
    {
        *ppv = static_cast<IQueryInfo *>(this);
    }
    else if (IsEqualIID(IID_IPersistFile, riid))
    {
        *ppv = static_cast<IPersistFile *>(this);
    }
    else
    {
        hr = E_NOINTERFACE;
        *ppv = NULL;
    }

    if (*ppv)
    {
        AddRef();
    }

    return hr;
}

// Increase the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) FileInfotipExt::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// Decrease the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) FileInfotipExt::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }

    return cRef;
}

#pragma endregion


#pragma region IPersistFile

IFACEMETHODIMP FileInfotipExt::GetClassID(CLSID *pClassID)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP FileInfotipExt::IsDirty(void)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP FileInfotipExt::Load(LPCOLESTR pszFileName, DWORD dwMode)
{
    // pszFileName contains the absolute path of the file to be opened.
    return StringCchCopy(
        m_szSelectedFile, 
        ARRAYSIZE(m_szSelectedFile), 
        pszFileName);
}

IFACEMETHODIMP FileInfotipExt::Save(LPCOLESTR pszFileName, BOOL fRemember)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP FileInfotipExt::SaveCompleted(LPCOLESTR pszFileName)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP FileInfotipExt::GetCurFile(LPOLESTR *ppszFileName)
{
    return E_NOTIMPL;
}

#pragma endregion


#pragma region IQueryInfo

IFACEMETHODIMP FileInfotipExt::GetInfoTip(DWORD dwFlags, LPWSTR *ppwszTip)
{
    // ppwszTip is the address of a Unicode string pointer that receives the 
	// tip string pointer. Extensions that implement this method must 
	// allocate memory for ppwszTip by calling CoTaskMemAlloc. Shell knows to 
	// free the memory when the info tip is no longer needed.
    const int cch = MAX_PATH + 512;
    *ppwszTip = static_cast<LPWSTR>(CoTaskMemAlloc(cch * sizeof(wchar_t)));
    if (*ppwszTip == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // Prepare the text of the infotip. The example infotip is composed of 
    // the file path and the count of code lines.
    wchar_t szLineNum[50] = L"N/A";
    {
        wifstream infile(m_szSelectedFile);
        if (infile.good())
        {
            __int64 lineNum = 0;
            wstring line;
            while (getline(infile, line))
            {
                lineNum++;
            }
            // Ignore the return value because this call will not fail.
            StringCchPrintf(szLineNum, ARRAYSIZE(szLineNum), L"%I64i", lineNum);
        }
    }

    HRESULT hr = StringCchPrintf(*ppwszTip, cch, 
        L"File: %s\nLines: %s\n- Infotip displayed by CppShellExtInfotipHandler", 
        m_szSelectedFile, szLineNum);
    if (FAILED(hr))
    {
        CoTaskMemFree(*ppwszTip);
    }

    return hr;
}

IFACEMETHODIMP FileInfotipExt::GetInfoFlags(DWORD *pdwFlags)
{
    return E_NOTIMPL;
}

#pragma endregion