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

#ifndef _ASSEMBLY_REFERENCE_LSIT_H_
#define _ASSEMBLY_REFERENCE_LSIT_H_

#include "fusionp.h"

class CAssemblyReferenceList : public ICLRAssemblyReferenceList
{
public:
    CAssemblyReferenceList();
    virtual ~CAssemblyReferenceList();

    static HRESULT Create(
            LPCWSTR *ppwzAsmRefs, 
            DWORD dwNumOfRefs,
            ICLRAssemblyReferenceList **ppRefList);

    // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();
    
    STDMETHOD(IsStringAssemblyReferenceInList)(
        /* in */    LPCWSTR    pwzAssemblyName
    );    

    STDMETHOD(IsAssemblyReferenceInList)(
        IUnknown *pName
    );

private:
    HRESULT Init(LPCWSTR *ppwzAsmRefs, DWORD dwNumOfRefs);

private:
    LONG            _dwSig;
    LONG            _cRef;
    DWORD           _dwAsmCount;
    IAssemblyName   **_ppAsmList;
};

#endif
