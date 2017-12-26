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
// File: servprov.h
// 
// =========================================================================== 
// simplified servprov.h for PAL

#include "rpc.h"
#include "rpcndr.h"

#include "unknwn.h"

#ifndef __IServiceProvider_INTERFACE_DEFINED__
#define __IServiceProvider_INTERFACE_DEFINED__

// 6d5140c1-7436-11ce-8034-00aa006009fa
EXTERN_C const IID IID_IServiceProvider;

interface IServiceProvider : public IUnknown
{
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE QueryService( 
        /* [in] */ REFGUID guidService,
        /* [in] */ REFIID riid,
        /* [out] */ void **ppvObject) = 0;    
};

#endif // __IServiceProvider_INTERFACE_DEFINED__
