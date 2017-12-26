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
// File: unknwn.h
// 
// ===========================================================================
// simplified unknwn.h for PAL

#include "rpc.h"
#include "rpcndr.h"

#ifndef __IUnknown_INTERFACE_DEFINED__
#define __IUnknown_INTERFACE_DEFINED__

typedef interface IUnknown IUnknown;

typedef /* [unique] */ IUnknown *LPUNKNOWN;

// 00000000-0000-0000-C000-000000000046
EXTERN_C const IID IID_IUnknown;

interface IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        REFIID riid,
        void **ppvObject) = 0;
        
    virtual ULONG STDMETHODCALLTYPE AddRef( void) = 0;
        
    virtual ULONG STDMETHODCALLTYPE Release( void) = 0;
};

#endif // __IUnknown_INTERFACE_DEFINED__

#ifndef __IClassFactory_INTERFACE_DEFINED__
#define __IClassFactory_INTERFACE_DEFINED__

// 00000001-0000-0000-C000-000000000046
EXTERN_C const IID IID_IClassFactory;
    
interface IClassFactory : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE CreateInstance( 
        IUnknown *pUnkOuter,
        REFIID riid,
        void **ppvObject) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE LockServer( 
        BOOL fLock) = 0;
};

#endif // __IClassFactory_INTERFACE_DEFINED__
