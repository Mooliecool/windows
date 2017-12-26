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
// File: CLASS.INL
//


#ifndef _CLASS_INL_
#define _CLASS_INL_

//*******************************************************************************
inline PTR_MethodDescChunk EEClass::GetChunks()
{
    LEAF_CONTRACT;
    return m_pChunks;
}

//*******************************************************************************
inline PTR_MethodDescChunk *EEClass::GetChunksPtr()
{
    LEAF_CONTRACT;
    return &m_pChunks;
}

//*******************************************************************************
// Our more optimised case if we are an interface - we know that the vtable won't be pointing to JITd code
// EXCEPT when it's a <clinit>
inline MethodDesc *EEClass::InterfaceFindMethod(LPCUTF8 pszName,
                                                PCCOR_SIGNATURE pSignature,
                                                DWORD cSignature,
                                                Module* pModule,
                                                FM_Flags flags,
                                                const Substitution *subst)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(IsInterface());
        PRECONDITION(!IsThunking());
        MODE_ANY;
    } CONTRACTL_END;

    return FindMethod(pszName, pSignature, cSignature, pModule, NULL, flags, subst);
}

//*******************************************************************************
inline size_t MethodTableBuilder::InstanceSliceOffsetForExplicit(
    BOOL containsPointers,
    MethodTable* pParentMethodTable)
{
    LEAF_CONTRACT;
    size_t dwInstanceSliceOffset =
        (pParentMethodTable != NULL) ? pParentMethodTable->GetClass()->m_dwNumInstanceFieldBytes : 0;

    // Since this class contains pointers, align it on a 4-byte
    // (32-bit) or 8-byte (64-bit) boundary if we aren't already
    if (containsPointers && !IS_ALIGNED(dwInstanceSliceOffset, sizeof(void*)))
    {
        dwInstanceSliceOffset = ALIGN_UP(dwInstanceSliceOffset, sizeof(void*));
    }

    return dwInstanceSliceOffset;
}

//*******************************************************************************
inline MethodTableBuilder::DeclaredMethodIterator::DeclaredMethodIterator(
            MethodTableBuilder &mtb) : m_mtb(mtb), m_idx(-1)
{
    LEAF_CONTRACT;
    CONSISTENCY_CHECK(FitsInI4(m_mtb.bmtMetaData->cMethAndGaps));
}

//*******************************************************************************
inline int MethodTableBuilder::DeclaredMethodIterator::CurrentIndex()
{
    LEAF_CONTRACT;
    CONSISTENCY_CHECK_MSG(0 <= m_idx && m_idx < (int)m_mtb.NumDeclaredMethods(),
                          "Invalid iterator state.");
    return m_idx;
}

//*******************************************************************************
inline BOOL MethodTableBuilder::DeclaredMethodIterator::Next()
{
    LEAF_CONTRACT;
    if (m_idx + 1 >= (int)m_mtb.NumDeclaredMethods())
        return FALSE;
    m_idx++;
    return TRUE;
}

//*******************************************************************************
inline mdMethodDef MethodTableBuilder::DeclaredMethodIterator::Token()
{
    LEAF_CONTRACT;
    CONSISTENCY_CHECK(TypeFromToken(m_mtb.bmtMetaData->pMethods[m_idx]) == mdtMethodDef);
    return m_mtb.bmtMetaData->pMethods[m_idx];
}

//*******************************************************************************
inline DWORD MethodTableBuilder::DeclaredMethodIterator::Attrs()
{
    LEAF_CONTRACT;
    return m_mtb.bmtMetaData->pMethodAttrs[m_idx];
}

//*******************************************************************************
inline DWORD MethodTableBuilder::DeclaredMethodIterator::RVA()
{
    LEAF_CONTRACT;
    return m_mtb.bmtMetaData->pMethodRVA[m_idx];
}

//*******************************************************************************
inline DWORD MethodTableBuilder::DeclaredMethodIterator::ImplFlags()
{
    LEAF_CONTRACT;
    return m_mtb.bmtMetaData->pMethodImplFlags[m_idx];
}

//*******************************************************************************
inline DWORD MethodTableBuilder::DeclaredMethodIterator::Classification()
{
    LEAF_CONTRACT;
    return m_mtb.bmtMetaData->pMethodClassifications[m_idx];
}

//*******************************************************************************
inline LPCSTR MethodTableBuilder::DeclaredMethodIterator::Name()
{
    LEAF_CONTRACT;
    return m_mtb.bmtMetaData->pstrMethodName[m_idx];
}

//*******************************************************************************
inline PCCOR_SIGNATURE MethodTableBuilder::DeclaredMethodIterator::GetSig(DWORD *pcbSig)
{
    LEAF_CONTRACT;
    return m_mtb.bmtInternal->pInternalImport->GetSigOfMethodDef(Token(), pcbSig);
}

//*******************************************************************************
inline BYTE  MethodTableBuilder::DeclaredMethodIterator::MethodImpl()
{
    LEAF_CONTRACT;
    return m_mtb.bmtMetaData->pMethodImpl[m_idx];
}

//*******************************************************************************
inline BOOL  MethodTableBuilder::DeclaredMethodIterator::IsMethodImpl()
{
    LEAF_CONTRACT;
    return m_mtb.bmtMetaData->pMethodImpl[m_idx] == METHOD_IMPL;
}

//*******************************************************************************
inline BYTE  MethodTableBuilder::DeclaredMethodIterator::MethodType()
{
    LEAF_CONTRACT;
    return m_mtb.bmtMetaData->pMethodType[m_idx];
}

//*******************************************************************************
inline MethodDesc *MethodTableBuilder::DeclaredMethodIterator::GetMethodDesc()
{
    LEAF_CONTRACT;
    return m_mtb.bmtMFDescs->ppMethodDescList[m_idx];
}

//*******************************************************************************
inline void MethodTableBuilder::DeclaredMethodIterator::SetMethodDesc(MethodDesc *pMD)
{
    LEAF_CONTRACT;
    m_mtb.bmtMFDescs->ppMethodDescList[m_idx] = pMD;
    m_mtb.bmtParent->ppParentMethodDescBuf[(m_idx * 2) + 1] = pMD;
}

//*******************************************************************************
inline class MethodDesc *MethodTableBuilder::DeclaredMethodIterator::GetParentMethodDesc()
{
    LEAF_CONTRACT;
    return m_mtb.bmtParent->ppParentMethodDescBuf[m_idx * 2];
}

//*******************************************************************************
inline void MethodTableBuilder::DeclaredMethodIterator::SetParentMethodDesc(MethodDesc *pMD)
{
    LEAF_CONTRACT;
    m_mtb.bmtParent->ppParentMethodDescBuf[m_idx * 2] = pMD;
}

//*******************************************************************************
inline MethodDesc *MethodTableBuilder::DeclaredMethodIterator::GetUnboxedMethodDesc()
{
    LEAF_CONTRACT;
    if (m_mtb.bmtMFDescs->ppUnboxMethodDescList != NULL)
    {
        return m_mtb.bmtMFDescs->ppUnboxMethodDescList[m_idx];
    }
    return NULL;
}

//*******************************************************************************
inline DWORD EEClass::SomeMethodsRequireInheritanceCheck()
{
    return (m_VMFlags & VMFLAG_METHODS_REQUIRE_INHERITANCE_CHECKS);
}

//*******************************************************************************
inline void EEClass::SetSomeMethodsRequireInheritanceCheck()
{
    m_VMFlags = m_VMFlags | VMFLAG_METHODS_REQUIRE_INHERITANCE_CHECKS;
}

//*******************************************************************************
inline Module *EEClass::GetLoaderModule()
{
    WRAPPER_CONTRACT;
    return GetMethodTable()->GetLoaderModule();
}

//*******************************************************************************
inline Module *EEClass::GetZapModule()
{
    WRAPPER_CONTRACT;
    return GetMethodTable()->GetZapModule();
}

//*******************************************************************************
inline BOOL EEClass::IsIntrospectionOnly()
{
    WRAPPER_CONTRACT;
    return GetAssembly()->IsIntrospectionOnly();
}

//*******************************************************************************
inline VOID EEClass::EnsureActive()
{
    WRAPPER_CONTRACT;
    GetModule()->EnsureActive();
}

#endif  // _CLASS_INL_

