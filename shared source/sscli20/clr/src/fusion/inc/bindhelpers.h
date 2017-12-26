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

#ifndef __BIND_HELPER_H__
#define __BIND_HELPER_H__

#include "dbglog.h"
#include "actasm.h"
#include "bindresult.h"
#include "asm.h"


// This bind helper is only capable of binding to IL assembly only.
HRESULT BindHelper(IAssemblyName *pName, 
                   IApplicationContext *pAppCtx, 
                   LONGLONG llFlags,
                   IUnknown **ppUnk,
                   IFusionBindLog **ppdbglog);

HRESULT CompareAssemblies(IUnknown *pAsm1, IUnknown *pAsm2, BOOL *pbIsEqual);

// Caution: This API changes pNamePolicy. It will update pNamePolicy's architecture to match what host gives
HRESULT HostStoreAssemblyLookup(IAssemblyName *pNameSource,
                                IAssemblyName *pNamePolicy, 
                                DWORD dwPolicyApplied,
                                IApplicationContext *pAppCtx,
                                CDebugLog *pdbglog,
                                IHostAssembly **ppHostAsm);

HRESULT CacheBindResult(CBindingResultCache *pBindCache,
                        CBindingInput *pBindInput,
                        CBindingOutput *pBindOutput);

HRESULT CacheBindResult(CBindingResultCache *pBindCache,
                        IAssemblyName *pName, 
                        LPCWSTR pwzCodebase, 
                        LOADCTX_TYPE ctxType, 
                        LPCWSTR pwzProbingBase,
                        CBindingOutput *pBindOutput);

HRESULT CacheBindResult(CBindingResultCache *pBindCache,
                        CBindingInput *pBindInput,
                        IAssemblyName *pName, 
                        LOADCTX_TYPE ctxType, 
                        HRESULT hr);

HRESULT CacheBindResult(CBindingResultCache *pBindCache,
                        IAssemblyName *pNameRef, 
                        LPCWSTR pwzCodebase, 
                        LOADCTX_TYPE ctxTypeIn, 
                        LPCWSTR pwzProbingBase,
                        IAssemblyName *pNameDef, 
                        LOADCTX_TYPE ctxTypeOut, 
                        HRESULT hr);
#endif
