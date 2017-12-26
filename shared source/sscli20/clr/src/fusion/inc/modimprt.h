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
#ifndef MODIMPRT_H
#define MODIMPRT_H

#include <windows.h>
#include <winerror.h>
#include <objbase.h>
#include "asmitem.h"

class CDebugLog;
class CAssemblyManifestImport;

STDAPI
CreateAssemblyModuleImport(
    LPCWSTR            szModuleName,
    LPBYTE             pbHashValue,
    DWORD              cbHashValue,
    DWORD              dwFlags,
    LPASSEMBLYNAME     pNameDef,
    CAssemblyManifestImport *pManImport,
    LPASSEMBLY_MODULE_IMPORT *ppImport);

class CAssemblyModuleImport : public IAssemblyModuleImport
{
public:

    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (REFIID riid, LPVOID * ppvObj);
    STDMETHOD_(ULONG,AddRef) ();
    STDMETHOD_(ULONG,Release) ();

    // *** IStream methods implemented ***
    STDMETHOD(Read) (VOID *pv, ULONG cb, ULONG *pcbRead);

    // *** IStream methods not implemented ***    
    STDMETHOD(Write) (VOID const *pv, ULONG cb,
        ULONG *pcbWritten);
    STDMETHOD(Seek) (LARGE_INTEGER dlibMove, DWORD dwOrigin,
        ULARGE_INTEGER *plibNewPosition);
    STDMETHOD(SetSize) (ULARGE_INTEGER libNewSize);
    STDMETHOD(CopyTo) (IStream *pStm, ULARGE_INTEGER cb,
        ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
    STDMETHOD(Commit) (DWORD dwCommitFlags);
    STDMETHOD(Revert) ();
    STDMETHOD(LockRegion) (ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb, DWORD dwLockType);
    STDMETHOD(UnlockRegion) (ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb, DWORD dwLockType);
    STDMETHOD(Stat) (STATSTG *pStatStg, DWORD grfStatFlag);
    STDMETHOD(Clone) (IStream **ppStm);


    // ctor, dtor
    CAssemblyModuleImport ();
    ~CAssemblyModuleImport ();
    
    // Init
    HRESULT Init(
        LPCWSTR            szModuleName,
        LPBYTE             pbHashValue,
        DWORD              cbHashValue,
        LPASSEMBLYNAME     pNameDef,
        CAssemblyManifestImport *pManImport,
        DWORD              dwModuleFlags);

    // Read-only get functions
    STDMETHOD(GetModuleName)   (__out_ecount_opt(*pccModuleName) LPOLESTR szModuleName, LPDWORD pccModuleName);
    STDMETHOD(GetHashAlgId)    (LPDWORD pdwHashValueId);
    STDMETHOD(GetHashValue)    (LPBYTE pbHashValue, LPDWORD pcbHashValue);
    STDMETHOD(GetFlags)        (LPDWORD pdwFlags);
    STDMETHOD(GetModulePath)   (__out_ecount_opt(*pccModulePath) LPOLESTR szModulePath, LPDWORD pccModulePath);

    // Download API
    STDMETHODIMP_(BOOL) IsAvailable();
    STDMETHODIMP BindToObject(IAssemblyBindSink *pBindSink,
                              IApplicationContext *pAppCtx,
                              LONGLONG llFlags, LPVOID *ppv);

    // Not present in interface.
    HRESULT GetNameDef(LPASSEMBLYNAME *ppName);

private:
    HRESULT CreateLogObject(CDebugLog **ppdbglog, LPCWSTR pwszModuleName, IApplicationContext *pAppCtx);

private:
    DWORD _dwSig;
    LONG  _cRef;
    LPTSTR  _pszModuleName;
    
    LPBYTE _pbHashValue;
    DWORD  _cbHashValue;
    DWORD  _dwFlags;

    LPASSEMBLYNAME _pNameDef;
    CAssemblyManifestImport *_pManImport;
};

#endif // MODIMPRT_H
