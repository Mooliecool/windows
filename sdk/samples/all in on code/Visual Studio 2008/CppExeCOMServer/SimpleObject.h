/****************************** Module Header ******************************\
Module Name:  SimpleObject.h
Project:      CppExeCOMServer
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once

#include <windows.h>
#include "CppExeCOMServer_h.h"  // For ISimpleObject


class SimpleObject : public ISimpleObject
{
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IDispatch
    IFACEMETHODIMP GetTypeInfoCount(UINT *pctinfo);
    IFACEMETHODIMP GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo **pptinfo);
    IFACEMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid);
    IFACEMETHODIMP Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

    // ISimpleObject
    IFACEMETHODIMP get_FloatProperty(FLOAT *pVal);
	IFACEMETHODIMP put_FloatProperty(FLOAT newVal);
	IFACEMETHODIMP HelloWorld(BSTR *pRet);
	IFACEMETHODIMP GetProcessThreadID(LONG *pdwProcessId, LONG *pdwThreadId);

    SimpleObject();

protected:
    ~SimpleObject();

private:
    // Reference count of component.
    long m_cRef;

    // The value of FloatProperty.
    float m_fField;

    // Pointer to type-library (for implementing IDispatch).
    LPTYPEINFO m_ptinfo;

    // Helper function to load the type info (for implementing IDispatch).
    HRESULT LoadTypeInfo(ITypeInfo **pptinfo, const CLSID& libid, const CLSID& iid, LCID lcid);
};