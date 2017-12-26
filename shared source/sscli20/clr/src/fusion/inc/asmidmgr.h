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

#ifndef _ASSEMBLY_IDENTITY_MANAGER_H_
#define _ASSEMBLY_IDENTITY_MANAGER_H_

#include "fusionp.h"
#include "asmreflist.h"

class CAssemblyIdentityManager : public ICLRAssemblyIdentityManager
{
public:
    static ICLRAssemblyIdentityManager* GetSingletonIdMgr() 
    { 
        return &_IdentityManager;
    }

    // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    STDMETHOD(GetCLRAssemblyReferenceList)(
        LPCWSTR  *ppwzAssemblyReferences,
        DWORD    dwNumOfReferences,
        ICLRAssemblyReferenceList    **ppReferenceList
    );

    STDMETHOD(GetBindingIdentityFromFile)(
        LPCWSTR pwzFilePath,
        DWORD   dwFlags,        
        LPWSTR  pwzBuffer,
        DWORD   *pcchBufferSize
    );

    STDMETHOD(GetBindingIdentityFromStream)(
        IStream *pStream,
        DWORD   dwFlags,        
        LPWSTR  pwzBuffer,
        DWORD   *pcchBufferSize
    );

    STDMETHOD(GetReferencedAssembliesFromFile)(
        LPCWSTR                     pwzFilePath,
        DWORD                       dwFlags,        
        ICLRAssemblyReferenceList   *pExcludeAssembliesList,
        ICLRReferenceAssemblyEnum   **ppReferenceEnum
    );

    STDMETHOD(GetReferencedAssembliesFromStream)(
        IStream                     *pStream,
        DWORD                       dwFlags,        
        ICLRAssemblyReferenceList   *pExcludeAssembliesList,
        ICLRReferenceAssemblyEnum   **ppReferenceEnum
    );

    STDMETHOD(GetProbingAssembliesFromReference)(
        DWORD           dwMachineType,
        DWORD           dwFlags,        
        LPCWSTR         pwzReferenceIdentity,
        ICLRProbingAssemblyEnum     **ppProbingAssemblyEnum
    );
    
    STDMETHOD(IsStronglyNamed)(
        LPCWSTR pwzAssemblyIdentity,
        BOOL    *pbIsStronglyNamed
    );

private:
    // singleton
    // we don't have constructor/destructor
    static CAssemblyIdentityManager _IdentityManager;
};

class CReferenceAssemblyEnum : public ICLRReferenceAssemblyEnum
{
public:
    CReferenceAssemblyEnum();
    virtual ~CReferenceAssemblyEnum();

    static HRESULT Create(
            IMetaDataAssemblyImport *pImport, 
            DWORD                   dwFlags,
            ICLRAssemblyReferenceList *pExcludeAssembliesList,
            ICLRReferenceAssemblyEnum **ppRefEnum);

     // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    STDMETHOD(Get)(
        /* in */        DWORD   dwIndex,
        /* out, size_is(*pcchBufferSize) */    LPWSTR  pwzBuffer,
        /* in, out */   DWORD   *pcchBufferSize
    );

private:
    HRESULT Init(IMetaDataAssemblyImport *pImport, DWORD dwFlags, ICLRAssemblyReferenceList *pExcludeAssembliesList);
    
private:
    LONG    _cRef;
    DWORD   _dwRefCount;
    DWORD   _dwFlags;
    IAssemblyName **_ppAsmRefs;
};

class CProbingAssemblyEnum : public ICLRProbingAssemblyEnum
{
public:
    CProbingAssemblyEnum();
    virtual ~CProbingAssemblyEnum();

    static HRESULT Create(
            DWORD dwMachineType, 
            DWORD dwFlags,
            LPCWSTR pwzRefIdentity, 
            ICLRProbingAssemblyEnum **ppProbingEnum);

     // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    STDMETHOD(Get)(
        /* in */        DWORD   dwIndex,
        /* out, size_is(*pcchBufferSize) */    LPWSTR  pwzBuffer,
        /* in, out */   DWORD   *pcchBufferSize
    );

private:
    HRESULT Init(DWORD dwMachineType, 
                 DWORD dwFlags,
                 LPCWSTR pwzRefIdentity);
    
private:
    LONG    _cRef;
    DWORD   _dwFlags;
    PEKIND  _pe;
    IAssemblyName *_pName;
    BOOL    _bHasPE;
};

#endif
