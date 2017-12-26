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

#ifndef _HOST_BINDING_POLICY_MANAGER_H_
#define _HOST_BINDING_POLICY_MANAGER_H_

#include "fusionp.h"

class CHostBindingPolicyManager : public ICLRHostBindingPolicyManager
{
public:
    static ICLRHostBindingPolicyManager* GetSingletonPolicyMgr()
    {
        return &_HostPolicyManager;
    }

    // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    STDMETHOD(ModifyApplicationPolicy)(
        /* in */    LPCWSTR         pwzSourceAssemblyIdentity,
        /* in */    LPCWSTR         pwzTargetAssemblyIdentity,
        /* in */    BYTE            *pbApplicationPolicy,
        /* in */    DWORD           cbAppPolicySize,
        /* in */    DWORD           dwPolicyModifyFlags,
        /* out, size_is(*pcbNewAppPolicySize) */    BYTE        *pbNewApplicationPolicy,
        /* in, out */    DWORD      *pcbNewAppPolicySize
    );

    STDMETHOD(EvaluatePolicy)(
        /* in */    LPCWSTR             pwzReferenceIdentity,
        /* in */    BYTE                *pbApplicationPolicy,
        /* in */    DWORD               cbAppPolicySize,
        /* out, size_is(*pcchPostPolicyReferenceIdentity) */    LPWSTR              pwzPostPolicyReferenceIdentity,
        /* in, out */ DWORD             *pcchPostPolicyReferenceIdentity,
        /* out */    DWORD                *pdwPoliciesApplied
    );

private:
    // singleton
    static CHostBindingPolicyManager _HostPolicyManager;
};

#endif
