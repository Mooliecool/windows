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

#include "common.h"
#include "securityattributes.h"
#include "security.h"
#include "eeconfig.h"
#include "corhost.h"

CorHostProtectionManager::CorHostProtectionManager()
{
    CONTRACTL 
    {
        MODE_ANY;
        GC_NOTRIGGER;
        NOTHROW;
    }CONTRACTL_END;

    m_eProtectedCategories = eNoChecks;
    m_fEagerSerializeGrantSet = false;
    m_fFrozen = false;
}

HRESULT CorHostProtectionManager::QueryInterface(REFIID id, void **pInterface)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    if (id == IID_ICLRHostProtectionManager)
    {
        *pInterface = GetHostProtectionManager();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG CorHostProtectionManager::AddRef()
{
    LEAF_CONTRACT;
    return 1;
}

ULONG CorHostProtectionManager::Release()
{
    LEAF_CONTRACT;
    return 1;
}

void CorHostProtectionManager::Freeze()
{
    LEAF_CONTRACT;
    m_fFrozen = true;
}

HRESULT CorHostProtectionManager::SetProtectedCategories(EApiCategories eProtectedCategories)
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    if(m_fFrozen)
        return E_FAIL;
    if((eProtectedCategories | eAll) != eAll)
        return E_FAIL;
    m_eProtectedCategories = eProtectedCategories;
    return S_OK;
}

EApiCategories CorHostProtectionManager::GetProtectedCategories()
{
    WRAPPER_CONTRACT;

    Freeze();
    return m_eProtectedCategories;
}

bool CorHostProtectionManager::GetEagerSerializeGrantSets() const
{
    LEAF_CONTRACT;

    // To provide more context about this flag in the hosting API, this is the case where, 
    // during the unload of an appdomain, we need to serialize a grant set for a shared assembly 
    // that has resolved policy in order to maintain the invariant that the same assembly loaded 
    // into another appdomain created in the future will be granted the same permissions
    // (since the current policy is potentially burned into the jitted code of the shared assembly already).

    return m_fEagerSerializeGrantSet;
}

HRESULT CorHostProtectionManager::SetEagerSerializeGrantSets()
{
    LEAF_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;

    m_fEagerSerializeGrantSet = true;
    return S_OK;
}
