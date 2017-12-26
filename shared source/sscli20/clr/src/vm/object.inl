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
//
// OBJECT.INL
//
// Definitions inline functions of a Com+ Object
//

#ifndef _OBJECT_INL_
#define _OBJECT_INL_

#include "object.h"

inline void* Object::UnBox()       // if it is a value class, get the pointer to the first field
{
    LEAF_CONTRACT;
    _ASSERTE(GetMethodTable()->IsValueClass());
    _ASSERTE(!Nullable::IsNullableType(GetMethodTable()));
    return (void*)(PTR_HOST_TO_TADDR(this) + sizeof(*this));
}

inline ADIndex Object::GetAppDomainIndex()
{
    WRAPPER_CONTRACT;
#ifndef _DEBUG
    // ok to cast to AppDomain because we know it's a real AppDomain if it's not shared
    if (!GetGCSafeMethodTable()->IsDomainNeutral())
        return ((AppDomain*)GetGCSafeMethodTable()->GetDomain())->GetIndex();
#endif
        return GetHeader()->GetAppDomainIndex();
}

inline DWORD Object::GetNumComponents()
{
    LEAF_CONTRACT;
    // Yes, we may not even be an array, which means we are reading some of the object's memory - however,
    // ComponentSize will multiply out this value.  Therefore, m_NumComponents must be the first field in
    // ArrayBase.
    return (PTR_ArrayBase(PTR_HOST_TO_TADDR(this)))->m_NumComponents;
}

inline SIZE_T Object::GetSize()
{
    LEAF_CONTRACT;
    // mask the alignment bits because this methos is called during GC
    MethodTable *mT = GetGCSafeMethodTable();
    // strings have component size2, all other non-arrays should have 0
    _ASSERTE(( mT->GetComponentSize() <= 2) || mT->IsArray());
    return mT->GetBaseSize() + ((SIZE_T)GetNumComponents() * mT->GetComponentSize());
}

#ifdef DACCESS_COMPILE

inline void Object::EnumMemoryRegions(void)
{
    PTR_MethodTable methodTable =
        PTR_MethodTable(((TADDR)m_pMethTab) & ~3);

    TADDR ptr = PTR_HOST_TO_TADDR(this) - sizeof(ObjHeader);
    SIZE_T size = sizeof(ObjHeader) + sizeof(Object);

    // If it is unsafe to touch the MethodTable so just enumerate
    // the base object.
    if (methodTable.IsValid())
    {
        size = sizeof(ObjHeader) + GetSize();
    }

    while (size > 0) {
        // Use 0x10000000 instead of MAX_ULONG32 so that the chunks stays aligned
        SIZE_T chunk = min(size, 0x10000000);
        DacEnumMemoryRegion(ptr, chunk);
        ptr += chunk; size -= chunk;
    }

    // As an Object is very low-level don't propagate
    // the enumeration to the MethodTable.
}

#endif // #ifdef DACCESS_COMPILE
    


        // Get the CorElementType for the elements in the array.  Avoids creating a TypeHandle
inline CorElementType ArrayBase::GetArrayElementType() const 
{
    WRAPPER_CONTRACT;
    ArrayClass *pArrayClass = GetArrayClass();
    PREFIX_ASSUME(pArrayClass != NULL);
    return pArrayClass->GetArrayElementType();
}

inline unsigned ArrayBase::GetRank() const 
{
    WRAPPER_CONTRACT;
    ArrayClass *pArrayClass = GetArrayClass();
    PREFIX_ASSUME(pArrayClass != NULL);
    return pArrayClass->GetRank();
}

// Total element count for the array
inline DWORD ArrayBase::GetNumComponents() const 
{ 
    LEAF_CONTRACT;
    return m_NumComponents; 
}

inline /* static */ unsigned ArrayBase::GetDataPtrOffset(MethodTable* pMT)
{
    LEAF_CONTRACT;
#if !defined(DACCESS_COMPILE)
    _ASSERTE(pMT->IsArray());
#endif // DACCESS_COMPILE
    // The -sizeof(ObjHeader) is because of the sync block, which is before "this"
    return pMT->GetBaseSize() - sizeof(ObjHeader);
}

inline /* static */ unsigned ArrayBase::GetBoundsOffset(MethodTable* pMT) 
{
    WRAPPER_CONTRACT;
    _ASSERTE(pMT->IsArray());
    if (!pMT->IsMultiDimArray()) 
        return(offsetof(ArrayBase, m_NumComponents));
    _ASSERTE(pMT->GetInternalCorElementType() == ELEMENT_TYPE_ARRAY);
    unsigned ret = sizeof(ArrayBase);
    if (pMT->IsSharedByReferenceArrayTypes())
    {
        ret += sizeof(TADDR);
    }
    return(ret);
}

inline /* static */ unsigned ArrayBase::GetLowerBoundsOffset(MethodTable* pMT) 
{
    WRAPPER_CONTRACT;
    _ASSERTE(pMT->IsArray());
    // There is no good offset for this for a SZARRAY.  
    _ASSERTE(pMT->GetInternalCorElementType() == ELEMENT_TYPE_ARRAY);
    // Lower bounds info is after total bounds info
    // and total bounds info has rank elements
    return GetBoundsOffset(pMT) +
        (PTR_ArrayClass(PTR_HOST_TO_TADDR(pMT->GetClass())))->GetRank() *
        sizeof(INT32);
}

inline ArrayClass* ArrayBase::GetArrayClass() const 
{
    WRAPPER_CONTRACT;
    ArrayClass *pRet = PTR_ArrayClass(PTR_HOST_TO_TADDR(GetMethodTable()->GetClass()));
    PREFIX_ASSUME(pRet != NULL || GetMethodTable()->IsAsyncPinType());
    return pRet;
}

// Get the element type for the array, this works whether the the element
// type is stored in the array or not
inline TypeHandle ArrayBase::GetArrayElementTypeHandle() const 
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    
    TypeHandle ret;
    if (GetMethodTable()->IsSharedByReferenceArrayTypes()) 
    {
        // Then it is in the array instance. 
        ret = *(PTR_TypeHandle(PTR_HOST_TO_TADDR(this) + sizeof(*this)));

#if !defined(DACCESS_COMPILE)
        // DAC could query about random data so we cannot assert here.
        // Instead, DAC does the verification after it retrieves the TypeHandle of the elements
        // in ClrDataAccess::RequestObjectData().
        _ASSERTE(!ret.IsNull());
        _ASSERTE(ret.IsArray() || !ret.GetClass()->IsArrayClass());
#endif // !DACCESS_COMPILE
    }
    else 
    {
        // If not shared then the "approximate" handle in the ArrayClass is actually exact
        ArrayClass *pArrCls = GetArrayClass();
        PREFIX_ASSUME(pArrCls != NULL);
        g_IBCLogger.LogEEClassAndMethodTableAccess(pArrCls);
        ret = pArrCls->GetApproxArrayElementTypeHandle(); 
    }
    
    return ret;
}

//===============================================================================
// Returns true if this pMT is Nullable<T> for T == paramMT

__forceinline BOOL Nullable::IsNullableForType(TypeHandle type, MethodTable* paramMT) 
{
    if (type.IsTypeDesc())
        return FALSE;
    if (!type.AsMethodTable()->HasInstantiation())            // shortcut, if it is not generic it can't be Nullable<T>
        return FALSE;
	return Nullable::IsNullableForTypeHelper(type.AsMethodTable(), paramMT);
}

//===============================================================================
// Returns true if this type is Nullable<T> for some T.  

inline BOOL Nullable::IsNullableType(TypeHandle type) 
{
    WRAPPER_CONTRACT;
    
    if (type.IsTypeDesc())
        return FALSE;

    if (!type.AsMethodTable()->HasInstantiation())            // shortcut, if it is not generic it can't be Nullable<T>
        return FALSE;

    EEClass* cls = type.AsMethodTable()->GetClass();
    return (cls->HasSameTypeDefAs(g_pNullableEEClass));
}



#endif  // _OBJECT_INL_
