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
#ifndef _ASMENUM_
#define _ASMENUM_

#include <fusionp.h>
#include "cache.h"
#include "list.h"

// implementation of IAssemblyEnum
class CAssemblyEnum : public IAssemblyEnum
{
public:
    friend class CUnifiedStore;

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID iid, void **ppvObj);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // Main methods.
    STDMETHODIMP GetNextAssembly(LPVOID pvReserved,
        IAssemblyName **ppName, DWORD dwFlags);

    STDMETHODIMP Reset(void);
    STDMETHODIMP Clone(IAssemblyEnum** ppEnum);

    CAssemblyEnum();
    ~CAssemblyEnum();

    HRESULT Init(IApplicationContext *pAppCtx, 
        IAssemblyName *pName, DWORD dwFlags);
   
private:
    HRESULT PopulateZapAssemblyList();
    HRESULT PopulateAssemblyList();

    void ReleaseAssemblyList();

private:
    DWORD          _dwSig;
    LONG           _cRef;

    IApplicationContext *_pAppCtx;
    IAssemblyName *_pName;
    DWORD          _dwFlags;

    List<IAssemblyName *> _listAsms;
    LISTNODE       _pos;
};


#endif  // _ASMENUM_
