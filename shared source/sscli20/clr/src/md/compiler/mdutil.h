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
//*****************************************************************************
// MDUtil.h
//
// Contains utility code for MD directory
//
//*****************************************************************************
#ifndef __MDUtil__h__
#define __MDUtil__h__

#include "metadata.h"


HRESULT _GetFixedSigOfVarArg(           // S_OK or error.
    PCCOR_SIGNATURE pvSigBlob,          // [IN] point to a blob of COM+ method signature
    ULONG   cbSigBlob,                  // [IN] size of signature
    CQuickBytes *pqbSig,                // [OUT] output buffer for fixed part of VarArg Signature
    ULONG   *pcbSigBlob);               // [OUT] number of bytes written to the above output buffer

ULONG _GetSizeOfConstantBlob(
    DWORD       dwCPlusTypeFlag,            // ELEMENT_TYPE_*
    void        *pValue,                    // BLOB value
    ULONG       cchString);                 // Size of string in wide chars, or -1 for auto.




class RegMeta;

//*********************************************************************
//
// Structure to record the all loaded modules and helpers.
// RegMeta instance is added to the global variable that is tracking 
// the opened scoped. This happens in RegMeta's constructor. 
// In RegMeta's destructor, the RegMeta pointer will be removed from
// this list.
//
//*********************************************************************
class UTSemReadWrite;
#define LOADEDMODULES_HASH_SIZE 47
class LOADEDMODULES : public CDynArray<RegMeta *> 
{
public:
    static UTSemReadWrite *m_pSemReadWrite; // Lock for multithreading
    static RegMeta *(m_HashedModules[LOADEDMODULES_HASH_SIZE]);
    
    static ULONG HashFileName(LPCWSTR szName);

    static HRESULT AddModuleToLoadedList(RegMeta *pRegMeta);
    static BOOL RemoveModuleFromLoadedList(RegMeta *pRegMeta);  // true if found and removed.
    
    static HRESULT FindCachedReadOnlyEntry(LPCWSTR szName, DWORD dwOpenFlags, RegMeta **ppMeta);
    
    static HRESULT ResolveTypeRefWithLoadedModules(
        mdTypeRef   tr,                     // [IN] TypeRef to be resolved.
        IMetaModelCommon *pCommon,          // [IN] scope in which the typeref is defined.
        REFIID      riid,                   // [IN] iid for the return interface
        IUnknown    **ppIScope,             // [OUT] return interface
        mdTypeDef   *ptd);                  // [OUT] typedef corresponding the typeref

#if defined(_DEBUG)
    static BOOL IsEntryInList(RegMeta *pRegMeta);
#endif
};


#endif // __MDUtil__h__
