/****************************** Module Header ******************************\
Module Name:  ClassFactory.cpp
Project:      CppExeCOMServer
Copyright (c) Microsoft Corporation.

The file implements the class factory for the SimpleObject COM class. A class 
factory (aka a class object) is a component whose main purpose is to create 
other components. It provides many controls over the creation process of the 
component. When a client uses a CLSID to request the creation of an object 
instance, the first step is creation of a class factory, an intermediate 
object that contains an implementation of the methods of the IClassFactory 
interface. While COM provides several instance creation functions, the first 
step in the implementation of these functions is always the creation of a 
class factory. For example, CoCreateInstance provides a wrapper over a 
CoGetClassObject and CreateInstance method of IClassFactory interface. 
CoCreateInstance internally creates class factory for the specified CLSID, 
gets the IClassFactory interface pointer, and then creates the component by 
calling CreateInstance on IClassFactory interface pointer and then returns 
the requested interface pointer to the client by calling QueryInterface 
method in the CreateInstance method of IClassFactory.

The class factory implements the IClassFactory interface. The class factory 
object must implement these two methods of IClassFactory to support the 
object creation.

    IFACEMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv);
    IFACEMETHODIMP LockServer(BOOL fLock);

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "ClassFactory.h"
#include "SimpleObject.h"


extern long g_cServerLocks;


ClassFactory::ClassFactory() : m_cRef(1)
{
    InterlockedIncrement(&g_cServerLocks);
}

ClassFactory::~ClassFactory()
{
    InterlockedDecrement(&g_cServerLocks);
}


//
// IUnknown
//

IFACEMETHODIMP ClassFactory::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT hr = S_OK;

    if (IsEqualIID(IID_IUnknown, riid) || 
        IsEqualIID(IID_IDispatch, riid) ||  // For implementing IDispatch
        IsEqualIID(IID_IClassFactory, riid))
    {
        *ppv = static_cast<IUnknown *>(this);
        AddRef();
    }
    else
    {
        hr = E_NOINTERFACE;
        *ppv = NULL;
    }

    return hr;
}

IFACEMETHODIMP_(ULONG) ClassFactory::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

IFACEMETHODIMP_(ULONG) ClassFactory::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }
    return cRef;
}


// 
// IClassFactory
//

IFACEMETHODIMP ClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv)
{
    HRESULT hr = CLASS_E_NOAGGREGATION;

    // pUnkOuter is used for aggregation. We do not support it in the sample.
    if (pUnkOuter == NULL)
    {
        hr = E_OUTOFMEMORY;

        // Create the COM component.
        SimpleObject *pSimpleObj = new SimpleObject();
        if (pSimpleObj)
        {
            // Query the specified interface.
            hr = pSimpleObj->QueryInterface(riid, ppv);
            pSimpleObj->Release();
        }
    }

    return hr;
}

IFACEMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
    if (fLock)
    {
        InterlockedIncrement(&g_cServerLocks);
    }
    else
    {
        InterlockedDecrement(&g_cServerLocks);
    }
    return S_OK;
}