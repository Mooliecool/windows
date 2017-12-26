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
#ifndef __HOST_ASM_H__
#define __HOST_ASM_H__

#include "fusionp.h"
#include "actasm.h"
#include "genericstackprobe.h"

HRESULT CreateHostAssemblyFromStream(
        IStream        *pAsmStream,
        IStream        *pDebugStream, 
        UINT64          AssemblyId,
        UINT64          AssemblyContext,
        AssemblyBindInfo *pBindInfo,
        IHostAssembly **ppHostAsm);

class CHostAssembly : public IHostAssembly
{
public:
    // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    // Gets name def of assembly.
    STDMETHOD(GetAssemblyNameDef)( 
        /* out */ IAssemblyName **ppName);

    // Enumerates dep. assemblies.
    // Delegates to IAssemblyManifestImport
    STDMETHOD(GetNextAssemblyNameRef)( 
        /* in  */ DWORD nIndex,
        /* out */ IAssemblyName **ppName);

    // Enumerates modules.
    // Delegates to IAssemblyManifestImport
    STDMETHOD(GetNextAssemblyModule)( 
        /* in  */ DWORD nIndex,
        /* out */ IHostAssemblyModuleImport **ppModImport);

    // Get module by name
    // Delegates to IAssemblyManifestImport
    STDMETHOD(GetModuleByName)( 
        /* in  */ LPCOLESTR pszModuleName,
        /* out */ IHostAssemblyModuleImport **ppModImport);

    // Get assembly stream
    STDMETHOD(GetAssemblyStream)( 
        /* out */ IStream   **ppStreamAsm);

    STDMETHOD(GetAssemblyId)(
        /* out */  UINT64   *pAssemblyId);

    // Get assembly pdb stream
    STDMETHOD(GetAssemblyDebugStream)(
        /* out */ IStream **ppDebugStream);

    STDMETHODIMP_(LOADCTX_TYPE) GetFusionLoadContext();

    STDMETHOD(GetAssemblyContext)(
        /* out */  UINT64   *pAsmContext);
    
    CHostAssembly();
    virtual ~CHostAssembly();

    HRESULT Init(
        IStream          *pAsmStream,
        IStream          *pDebugStream,
        UINT64            AssemblyId,
        UINT64            AssemblyContext,
        AssemblyBindInfo *pBindInfo);

    // Activated assemblies

    HRESULT SetLoadContext(CLoadContext *pLoadContext);

    void SetApplicationContextID(LPVOID pAppCtx) {_ASSERTE((!_pAppCtx) ||(_pAppCtx == pAppCtx));_pAppCtx = pAppCtx;}
    LPVOID GetApplicationContextID() {return _pAppCtx;}

private:
    HRESULT CopyMetaData(IStream *pStream);
    
private:
    DWORD                       _dwSig;
    LONG                        _cRef;
    IMetaDataAssemblyImport     *_pImport;
    IAssemblyName               *_pName;
    CLoadContext                *_pLoadContext;
    IStream                     *_pDebugStream;  
    UINT64                      _AsmId;
    AssemblyBindInfo            _bindInfo;
    List<IHostAssemblyModuleImport *> _listModules;
    LPVOID                      _pAppCtx;
    UINT64                      _AsmContext;
    mdAssemblyRef               *_rAssemblyRefTokens;
    DWORD                       _cAssemblyRefTokens;
};

#endif // __HOST_ASM_H__
