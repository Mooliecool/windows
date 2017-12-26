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
#ifndef ASM_H
#define ASM_H

#include "fusion.h"
#include "cache.h"
#include "list.h"
#include "asmimprt.h"

// 'MSAC'
#define ASSEMBLY_CLASS_SIGNATURE    (0x4d534143)

typedef struct tagAssemblyBindFailure
{
    IAssemblyName   *pName;
    HRESULT         hr;
}AssemblyBindFailure;

class CAssembly : public IAssembly
{
public:
    // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    // Gets name def of assembly.
    // Delegates to IAssemblyManifestImport.
    STDMETHOD(GetAssemblyNameDef)( 
        /* out */ LPASSEMBLYNAME *ppName);

    // Enumerates dep. assemblies.
    // Delegates to IAssemblyManifestImport
    STDMETHOD(GetNextAssemblyNameRef)( 
        /* in  */ DWORD nIndex,
        /* out */ LPASSEMBLYNAME *ppName);

    // Enumerates modules.
    // Delegates to IAssemblyManifestImport
    STDMETHOD(GetNextAssemblyModule)( 
        /* in  */ DWORD nIndex,
        /* out */ LPASSEMBLY_MODULE_IMPORT *ppImport);

    // Get module by name
    // Delegates to IAssemblyManifestImport
    STDMETHOD(GetModuleByName)( 
        /* in  */ LPCOLESTR pszModuleName,
        /* out */ LPASSEMBLY_MODULE_IMPORT *ppImport);

    // Get manifest module cache path.
    // Delegates to IAssemblyManifestImport
    STDMETHOD(GetManifestModulePath)( 
        /* out     */ LPOLESTR  pszModulePath,
        /* in, out */ LPDWORD   pccModulePath);
        
    STDMETHOD(GetAssemblyPath)(
        /* out     */ LPOLESTR pStr,
        /* in, out */ LPDWORD lpcwBuffer);

    STDMETHOD(GetAssemblyLocation)(
        /* out     */ DWORD *pdwAsmLocation);

    STDMETHODIMP_(LOADCTX_TYPE) GetFusionLoadContext();
    
    STDMETHOD(GetNextHardBoundDependency)(
        /* [in]   */ DWORD dwIndex, 
        /* [out]  */ IAssembly **ppILAsm, 
        /* [out]  */ IAssembly **ppNIAsm);

    CAssembly();
    virtual ~CAssembly();

    HRESULT Init(LPASSEMBLY_MANIFEST_IMPORT pImport,
        CTransCache *pTransCache, LPCOLESTR szCodebase, FILETIME *pftCodeBase);

    HRESULT SetAssemblyLocation(DWORD dwAsmLoc);

    // Activated assemblies

    HRESULT SetLoadContext(CLoadContext *pLoadContext);

    HRESULT GetProbingBase(
            __out_ecount_opt(*pccLength) LPWSTR pwzProbingBase, 
            DWORD *pccLength);

    HRESULT SetProbingBase(LPCWSTR pwzProbingBase);

    void SetFileHandle(HANDLE);

    void SetIsSystemAssembly(BOOL bIsSystem) { _isSystem = bIsSystem;}
    BOOL IsSystemAssembly() {return _isSystem;}

    void SetApplicationContextID(LPVOID pAppCtx) {_ASSERTE((!_pAppCtx) ||(_pAppCtx == pAppCtx));_pAppCtx = pAppCtx;}
    LPVOID GetApplicationContextID() { return _pAppCtx;}

    HRESULT GetInternalMDImport(IMetaDataAssemblyImport **ppMDImport) 
    {
        *ppMDImport = NULL;
        if (_pImport) {
            CAssemblyManifestImport *pAsmImport = static_cast<CAssemblyManifestImport *>(_pImport);
            return pAsmImport->GetInternalMDImport(ppMDImport);
        }
        else {
            return S_FALSE;
        }
    }
        
protected:    
    DWORD                      _dwSig;
    LONG                       _cRef;
    LPASSEMBLY_MANIFEST_IMPORT _pImport;
    IAssemblyName             *_pName;
    DWORD                      _dwAsmLoc;
    BOOL                       _isSystem; 
    LPWSTR                     _pwzProbingBase;
    CLoadContext              *_pLoadContext;
    HANDLE                     _hFile;

    // We keep a pointer of IApplicationContext as an ID of the app ctx. 
    // This will help to prevent GetAssemblyBindingClosure from giving a different app ctx.
    LPVOID                     _pAppCtx;  
};

// 'MSAN'
#define NATIVEIMAGEASSEMBLY_CLASS_SIGNATURE    (0x4d53414e)
class CNativeImageAssembly : public CAssembly
{
    friend class CAssemblyBindingClosure;

public: 
    CNativeImageAssembly();
    virtual ~CNativeImageAssembly();
    
    STDMETHODIMP_(ULONG)    Release();
    
    STDMETHOD(GetNextHardBoundDependency)(
        /* [in]   */ DWORD dwIndex, 
        /* [out]  */ IAssembly **ppILAsm, 
        /* [out]  */ IAssembly **ppNIAsm);

    void SetILAssembly(IAssembly *pAsm) 
    { 
        if (pAsm != _pILAsm) {
            SAFERELEASE(_pILAsm); 
            _pILAsm = pAsm; 
            _pILAsm->AddRef();
        }
    }

    HRESULT AddDependentAssembly(IAssembly *pAsm);
    HRESULT AddFailureDependentAssembly(IAssemblyName *pName, HRESULT hResult);

    DWORD GetNIStatus() {return _NIStatus;}
    void SetNIStatus(DWORD status) {_NIStatus = status; }

private:
    DWORD                      _NIStatus; 
    IAssembly                 *_pILAsm;
    List<IAssembly *>          _listDepAsms;
    List<AssemblyBindFailure>  _listFailureDepAsms; 
    DWORD                      _dwNumHardBoundDeps; 
};


class CDebugLog;
class CLoadContext;

STDAPI CreateAssemblyFromTransCacheEntry(
        CTransCache *pTransCache, 
        IAssemblyManifestImport *pManImport, 
        IAssembly **ppAsm);

STDAPI CreateAssemblyFromManifestFile(
        LPCOLESTR szFileName, 
        LPCOLESTR szCodebase, 
        FILETIME *pftCodebase, 
        IAssembly **ppAssembly);

STDAPI CreateAssemblyFromManifestImport(
        IAssemblyManifestImport *pImport,
        LPCOLESTR szCodebase, 
        FILETIME *pftCodebase,
        IAssembly **ppAssembly);

HRESULT CreateAssemblyFromCacheLookup(
        IApplicationContext *pAppCtx, 
        IAssemblyName *pNameRef,
        IAssembly **ppAsm, 
        CDebugLog *pdbglog);

#endif // ASM_H
