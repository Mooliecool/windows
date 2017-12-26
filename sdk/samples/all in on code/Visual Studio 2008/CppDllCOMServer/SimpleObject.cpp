/****************************** Module Header ******************************\
Module Name:  SimpleObject.cpp
Project:      CppDllCOMServer
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "SimpleObject.h"
#include "CppDllCOMServer_i.c"  // For component GUIDs


SimpleObject::SimpleObject() : m_cRef(1), m_fField(0.0F), m_ptinfo(NULL)
{
}

SimpleObject::~SimpleObject()
{
}


//
// IUnknown
//

// Query to the interface the component supported.
IFACEMETHODIMP SimpleObject::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT hr = S_OK;

    if (IsEqualIID(IID_IUnknown, riid))
    {
        *ppv = static_cast<IUnknown *>(this);
    }
    else if (IsEqualIID(IID_IDispatch, riid)) // For implementing IDispatch
    {
        *ppv = static_cast<IDispatch *>(this);
    }
    else if (IsEqualIID(IID_ISimpleObject, riid))
    {
        *ppv = static_cast<ISimpleObject *>(this);
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
IFACEMETHODIMP_(ULONG) SimpleObject::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// Decrease the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) SimpleObject::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }

    return cRef;
}


//
// IDispatch
//

// This method retrieves the number of type information interfaces that an 
// object provides, either 0 or 1. If the object provides type information, 
// this *pctinfo is 1; otherwise the *pctinfo is 0.
IFACEMETHODIMP SimpleObject::GetTypeInfoCount(UINT *pctinfo)
{
    if (pctinfo == NULL) 
    {
        return E_POINTER; 
    }
    *pctinfo = 1;
    return S_OK;
}

// This method retrieves the type information for an object.
IFACEMETHODIMP SimpleObject::GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo **pptinfo)
{
    HRESULT hr = S_OK;
    *pptinfo = NULL;

    if (itinfo != 0)
    {
        return ResultFromScode(DISP_E_BADINDEX);
    }

    if (m_ptinfo == NULL)
    {
        // Load the type info.
        hr = LoadTypeInfo(&m_ptinfo, LIBID_CppDllCOMServerLib, IID_ISimpleObject, 0);
    }

    if (SUCCEEDED(hr))
    {
        // AddRef and return pointer to cached typeinfo.
        m_ptinfo->AddRef();
        *pptinfo = m_ptinfo;
    }

    return hr;
}

// Helper function to load the type info.
HRESULT SimpleObject::LoadTypeInfo(ITypeInfo **pptinfo, 
                                   const CLSID& libid, 
                                   const CLSID& iid, 
                                   LCID lcid)
{
    HRESULT hr;
    LPTYPELIB ptlib = NULL;
    LPTYPEINFO ptinfo = NULL;

    *pptinfo = NULL;

    // Load the type library.
    hr = LoadRegTypeLib(libid, 1, 0, lcid, &ptlib);
    if (SUCCEEDED(hr))
    {
        // Get type information for interface of the object.
        hr = ptlib->GetTypeInfoOfGuid(iid, &ptinfo);
        if (SUCCEEDED(hr))
        {
            *pptinfo = ptinfo;
        }

        ptlib->Release();
    }

    return hr;
}

// Maps a single member and an optional set of argument names to a 
// corresponding set of integer DISPIDs, which can be used on subsequent 
// calls to IDispatch::Invoke. The dispatch function DispGetIDsOfNames 
// provides a standard implementation of GetIDsOfNames.
IFACEMETHODIMP SimpleObject::GetIDsOfNames(REFIID riid, 
                                           LPOLESTR *rgszNames, 
                                           UINT cNames, 
                                           LCID lcid, 
                                           DISPID* rgdispid)
{
    HRESULT hr = S_OK;

    if (m_ptinfo == NULL)
    {
        // Load the type info.
        hr = LoadTypeInfo(&m_ptinfo, LIBID_CppDllCOMServerLib, IID_ISimpleObject, 0);
    }

    if (SUCCEEDED(hr))
    {
        hr = DispGetIDsOfNames(m_ptinfo, rgszNames, cNames, rgdispid);
    }

    return hr;
}

// Provides access to properties and methods exposed by an object. The 
// dispatch function DispInvoke Function provides a standard implementation 
// of IDispatch::Invoke.
IFACEMETHODIMP SimpleObject::Invoke(DISPID dispidMember, 
                                    REFIID riid,
                                    LCID lcid,
                                    WORD wFlags,
                                    DISPPARAMS *pdispParams,
                                    VARIANT *pvarResult,
                                    EXCEPINFO *pExcepInfo, 
                                    UINT *puArgErr)
{
    HRESULT hr = S_OK;

    if (m_ptinfo == NULL)
    {
        // Load the type info.
        hr = LoadTypeInfo(&m_ptinfo, LIBID_CppDllCOMServerLib, IID_ISimpleObject, 0);
    }

    if (SUCCEEDED(hr))
    {
        hr = DispInvoke(this, m_ptinfo, dispidMember, wFlags, pdispParams, 
            pvarResult, pExcepInfo, puArgErr); 
    }

    return hr;
}


//
// ISimpleObject
//

IFACEMETHODIMP SimpleObject::get_FloatProperty(FLOAT *pVal)
{
    *pVal = m_fField;
    return S_OK;
}

IFACEMETHODIMP SimpleObject::put_FloatProperty(FLOAT newVal)
{
    m_fField = newVal;
    return S_OK;
}

IFACEMETHODIMP SimpleObject::HelloWorld(BSTR *pRet)
{
    // Allocate memory for the string.
    *pRet = ::SysAllocString(L"HelloWorld");
    if (pRet == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // The client is now responsible for freeing the BSTR.
    return S_OK;
}

IFACEMETHODIMP SimpleObject::GetProcessThreadID(LONG *pdwProcessId, LONG *pdwThreadId)
{
    *pdwProcessId = GetCurrentProcessId();
    *pdwThreadId = GetCurrentThreadId();

    return S_OK;
}