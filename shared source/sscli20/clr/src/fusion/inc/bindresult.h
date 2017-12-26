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

#ifndef __BIND_RESULT_H__
#define __BIND_RESULT_H__

#include "fusionp.h"
#include "list.h"

class CBindingInput
{
public:
    CBindingInput();
    ~CBindingInput();
    
    static HRESULT Create(IAssemblyName *pName, LPCWSTR pwzCodebase, LOADCTX_TYPE ctxType, LPCWSTR pwzProbingBase, CBindingInput **ppBindInput);

    ULONG AddRef();
    ULONG Release();

    DWORD GetHash();

    HRESULT IsEqual(CBindingInput *pBindInput);

private:
    HRESULT Init(IAssemblyName *pName, LPCWSTR pwzCodebase, LOADCTX_TYPE ctxType, LPCWSTR pwzProbingBase);
    
private:
    LONG            _cRef;
    LPWSTR          _pwzAsmName;
    LPWSTR          _pwzCodebase;
    LOADCTX_TYPE    _ctxType;
    LPWSTR          _pwzProbingBase;
};

class CBindingOutput
{
public:
    CBindingOutput();
    ~CBindingOutput();
    
    static HRESULT Create(IAssemblyName *pName, LOADCTX_TYPE ctxType, HRESULT hr, CBindingOutput **ppBindingOutput);

    ULONG AddRef();
    ULONG Release();
    
    HRESULT Init(IAssemblyName *pName, LOADCTX_TYPE ctxType, HRESULT hr);

public:
    LONG            _cRef;
    LPWSTR          _pwzAsmName;
    LOADCTX_TYPE    _ctxType;
    HRESULT         _hr;
};

class CBindingResult
{
public:
    CBindingResult(CBindingInput *pBindInput, CBindingOutput *pBindOutput);
    ~CBindingResult();

public:
    CBindingInput   *_pBindInput;
    CBindingOutput  *_pBindOutput;
};

#define BINDING_RESULT_CACHE_SIZE   107

class CBindingResultCache
{
public:
    CBindingResultCache();
    ~CBindingResultCache();

    static HRESULT Create(CBindingResultCache **ppBindCache);

    ULONG AddRef();
    ULONG Release();

    HRESULT CheckBindingResult(CBindingInput *pBindInput, CBindingOutput **pBindOutput);
    HRESULT AddBindingResult(CBindingInput *pBindInput, CBindingOutput *pBindOutput);

private:
    HRESULT Init();

private:
    LONG                    _cRef;
    CRITSEC_COOKIE          _cs;
    List<CBindingResult *>  _result[BINDING_RESULT_CACHE_SIZE];
};

#endif
