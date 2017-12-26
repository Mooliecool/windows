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
#ifndef __PCYCACHE_H_INCLUDED__
#define __PCYCACHE_H_INCLUDED__

#include "list.h"
#include "helpers.h"

#define POLICY_CACHE_SIZE                  53

class CPolicyMapping {
    public:
        CPolicyMapping();

        virtual ~CPolicyMapping();

        static HRESULT Create(IAssemblyName *pNameRefSource,
                              IAssemblyName *pNameRefPolicy,
                              AsmBindHistoryInfo *pBindHistory,
                              DWORD dwPoliciesApplied,
                              CPolicyMapping **ppMapping);

        HRESULT IsMatchingSource(IAssemblyName *pName);

    private:
        HRESULT Init(IAssemblyName *pNameSource, 
                     IAssemblyName *pNamePolicy,
                     AsmBindHistoryInfo *pBindHistory,
                     DWORD dwPoliciesApplied);

    public:
        LPWSTR                  _pwzNameSource;
        WORD                    _wVersSource[4];
        LPWSTR                  _pwzCulture;
        BYTE                    _pbPktSource[PUBLIC_KEY_TOKEN_LEN];
        PEKIND                  _pe;
        BOOL                    _bRetarget;
        LPWSTR                  _pwzNamePolicy;
        WORD                    _wVersPolicy[4];
        DWORD                   _cbPktPolicy;
        LPBYTE                  _pbPktPolicy;
        PEKIND                  _pePolicy;
        AsmBindHistoryInfo     *_pBindHistory;
        DWORD                   _dwPoliciesApplied;
};

class CPolicyCache : public IUnknown {
    public:
        CPolicyCache();
        virtual ~CPolicyCache();
        
        static HRESULT Create(CPolicyCache **ppPolicyCache);

        // IUnknown methods

        STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();

        // Helpers

        HRESULT InsertPolicy(IAssemblyName *pNameRefSource,
                             IAssemblyName *pNameRefPolicy,
                             AsmBindHistoryInfo *pBindHistory,
                             DWORD dwPolicyApplied);

        HRESULT LookupPolicy(IAssemblyName *pNameRefSource,
                             IAssemblyName **ppNameRefPolicy,
                             AsmBindHistoryInfo **ppBindHistory,
                             DWORD *pdwPolicyApplied);

    private:
        HRESULT Init();

    private:
        LONG                                  _cRef;
        CRITSEC_COOKIE                        _cs;
        List<CPolicyMapping *>                _listMappings[POLICY_CACHE_SIZE];

};


HRESULT PreparePolicyCache(IApplicationContext *pAppCtx, CPolicyCache **ppPolicyCache);

#endif
