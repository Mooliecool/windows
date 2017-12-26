/****************************** Module Header ******************************\
Module Name:  ClassFactory.h
Project:      CppExeCOMServer
Copyright (c) Microsoft Corporation.

The file declares the class factory for the SimpleObject COM class. A class 
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

#pragma once

#include <unknwn.h>     // For IClassFactory
#include <windows.h>


class ClassFactory : public IClassFactory
{
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IClassFactory
    IFACEMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv);
    IFACEMETHODIMP LockServer(BOOL fLock);

    ClassFactory();

protected:
    ~ClassFactory();

private:
    long m_cRef;
};