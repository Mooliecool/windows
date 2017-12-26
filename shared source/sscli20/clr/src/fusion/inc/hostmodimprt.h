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
#ifndef __HOST_MODIMPRT_H__
#define __HOST_MODIMPRT_H__

#include "fusionp.h"

class CDebugLog;

STDAPI
CreateHostAssemblyModuleImport(
    ModuleBindInfo    *pBindInfo,
    IStream           *pStream,
    IStream           *pDebugStream,
    DWORD              dwModuleId,
    IHostAssemblyModuleImport **ppImport);

class CHostAssemblyModuleImport : public IHostAssemblyModuleImport
{
public:

    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (REFIID riid, LPVOID * ppvObj);
    STDMETHOD_(ULONG,AddRef) ();
    STDMETHOD_(ULONG,Release) ();

    // ctor, dtor
    CHostAssemblyModuleImport ();
    ~CHostAssemblyModuleImport ();
    
    // Init
    HRESULT Init(
                ModuleBindInfo    *pBindInfo,
                IStream           *pStream,
                IStream           *pDebugStream,
                DWORD              dwModuleId);

    STDMETHOD(GetModuleName)   (
                LPOLESTR szModuleName, 
                LPDWORD pccModuleName);
    
    STDMETHOD(GetModuleStream) (
                IStream **ppStreamModule);
   
    STDMETHOD(GetModuleId) (
                DWORD *pdwModuleId);
        
    STDMETHOD(GetModuleDebugStream)(
                IStream **ppDebugStream);

private:
    DWORD       _dwSig;
    LONG        _cRef;
    LPASSEMBLYNAME _pNameDef;
    IStream    *_pStream; 
    IStream    *_pDebugStream;
    DWORD       _dwModuleId;
    ModuleBindInfo _bindInfo;
    BOOL        _bAskedHost;
};

#endif // MODIMPRT_H
