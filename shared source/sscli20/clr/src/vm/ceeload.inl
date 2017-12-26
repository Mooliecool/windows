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
// ===========================================================================
// File: CEELOAD.INL
// 
// CEELOAD.INL has inline methods from CEELOAD.H.
// ===========================================================================

#ifndef CEELOAD_INL_
#define CEELOAD_INL_

inline
LookupMap::Iterator::Iterator(LookupMap* map)
{
    LEAF_CONTRACT;

    m_map = map;
    m_index = (DWORD) -1;
}
        
inline BOOL
LookupMap::Iterator::Next()
{
    LEAF_CONTRACT;

    if (!m_map || !m_map->pTable)
    {
        return FALSE;
    }

    m_index++;
    while (m_index == m_map->dwMaxIndex)
    {
        m_map = m_map->pNext;
        if (!m_map || !m_map->pTable)
        {
            return FALSE;
        }
    }

    return TRUE;
}

inline TADDR
LookupMap::Iterator::GetElement()
{
    LEAF_CONTRACT;

    TADDR result = m_map->pTable[m_index];
    if ((result & (TADDR)IS_MODULE_BACKPOINTER) != 0)
	return NULL;
    else
	return result;
}

inline DWORD
LookupMap::Iterator::GetIndex()
{
    LEAF_CONTRACT;
    return m_index;
}

inline LookupMap*
LookupMap::Iterator::GetMap()
{
    LEAF_CONTRACT;
    return m_map;
}

inline Assembly* Module::GetAssembly() const
{
    LEAF_CONTRACT;
    
    return m_pAssembly;
}

inline MethodDesc *Module::LookupMethodDef(mdMethodDef token)
{
    WRAPPER_CONTRACT;
    
    _ASSERTE(TypeFromToken(token) == mdtMethodDef);
    g_IBCLogger.LogRidMapAccess( MakePair( this, token ) );
    return PTR_MethodDesc(GetFromRidMap(
                                 &m_MethodDefToDescMap,
                                 RidFromToken(token)));
}

inline MethodDesc *Module::LookupMemberRefAsMethod(mdMemberRef token)
{
    WRAPPER_CONTRACT;
    
    _ASSERTE(TypeFromToken(token) == mdtMemberRef);
    g_IBCLogger.LogRidMapAccess( MakePair( this, token ) );
    MethodDesc *pMethodDesc =
        PTR_MethodDesc(GetFromRidMap(
                           &m_MemberRefToDescMap,
                           RidFromToken(token)));
    _ASSERTE(((size_t)pMethodDesc & IS_FIELD_MEMBER_REF) == 0);
    return pMethodDesc;
}

inline Assembly *Module::LookupAssemblyRef(mdAssemblyRef token)
{
    WRAPPER_CONTRACT;
    
    _ASSERTE(TypeFromToken(token) == mdtAssemblyRef);
    PTR_Module module= PTR_Module(GetFromRidMap(
                            &m_ManifestModuleReferencesMap,
                            RidFromToken(token)));
    return module?module->GetAssembly():NULL;
}

#ifndef DACCESS_COMPILE
inline void Module::ForceStoreAssemblyRef(mdAssemblyRef token, Assembly *value)
{
    WRAPPER_CONTRACT; // THROWS/GC_NOTRIGGER/INJECT_FAULT()/MODE_ANY
    _ASSERTE(value->GetManifestModule());
    _ASSERTE(TypeFromToken(token) == mdtAssemblyRef);

    if (!IncMapSize(&m_ManifestModuleReferencesMap, RidFromToken(token)))
    {
        COMPlusThrowOM();
    }
    SetInRidMap(&m_ManifestModuleReferencesMap,
                RidFromToken(token),
                PTR_HOST_TO_TADDR(value->GetManifestModule()));
}

inline void Module::StoreAssemblyRef(mdAssemblyRef token, Assembly *value)
{
    WRAPPER_CONTRACT;
    _ASSERTE(value->GetManifestModule());
    _ASSERTE(TypeFromToken(token) == mdtAssemblyRef);
    TryAddToRidMap(&m_ManifestModuleReferencesMap,
                       RidFromToken(token),
                       PTR_HOST_TO_TADDR(value->GetManifestModule()));
}

inline mdAssemblyRef Module::FindAssemblyRef(Assembly *value)
{
    WRAPPER_CONTRACT;

    return m_ManifestModuleReferencesMap.Find(PTR_HOST_TO_TADDR(value->GetManifestModule())) | mdtAssemblyRef;
}
#endif //DACCESS_COMPILE

inline BOOL Module::IsEditAndContinueCapable() 
{ 
    WRAPPER_CONTRACT; 
    return IsEditAndContinueCapable(m_file) && !GetAssembly()->IsDomainNeutral() && !this->IsReflection(); 
}

FORCEINLINE PTR_DomainLocalModule Module::GetDomainLocalModule(AppDomain *pDomain /*=NULL*/)
{
    WRAPPER_CONTRACT;
    if (!GetAssembly()->IsDomainNeutral())
    {
        CONSISTENCY_CHECK(!DomainLocalBlock::IsModuleID((SIZE_T)m_pDomainLocalModule));
        return m_pDomainLocalModule;
    }

    if (pDomain == NULL)
        pDomain = GetAppDomain();

    _ASSERTE(pDomain);

    return pDomain->GetDomainLocalBlock()->GetModuleSlot(GetModuleID());
}

FORCEINLINE ULONG Module::GetNumberOfActivations()
{
	_ASSERTE(m_Crst.OwnedByCurrentThread());
	return m_dwNumberOfActivations;
}

FORCEINLINE ULONG Module::IncrementNumberOfActivations()
{
	CrstPreempHolder lock(&m_Crst);
	return ++m_dwNumberOfActivations;
}

#endif  // CEELOAD_INL_
