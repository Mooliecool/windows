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

#ifndef _TYPEHANDLE_INL_
#define _TYPEHANDLE_INL_

#include "typehandle.inl"

inline mdTypeDef TypeHandle::GetCl() const
{ 
    WRAPPER_CONTRACT; 
    PREFIX_ASSUME(GetMethodTable() != NULL);
    return GetMethodTable()->GetCl(); 
}

inline MethodTable* TypeHandle::GetMethodTable() const
{
    WRAPPER_CONTRACT;
    
    if (IsUnsharedMT()) 
        return AsMethodTable();
    else
        return(AsTypeDesc()->GetMethodTable());
}

inline void TypeHandle::SetIsFullyLoaded()
{
    WRAPPER_CONTRACT;
    
    if (IsUnsharedMT()) 
        AsMethodTable()->SetIsFullyLoaded();
    else
        AsTypeDesc()->SetIsFullyLoaded();
}

inline MethodTable* TypeHandle::GetMethodTableOfElementType() const
{
    WRAPPER_CONTRACT;
    
    if (IsUnsharedMT()) 
        return AsMethodTable();

    TypeHandle elementType = AsTypeDesc()->GetTypeParam();

    return elementType.GetMethodTableOfElementType();
}

inline MethodTable * TypeHandle::GetPossiblySharedArrayMethodTable() const
{
    WRAPPER_CONTRACT;
    
    _ASSERTE(IsArrayType());

    if (IsArray())
        return AsArray()->GetArrayClass()->GetMethodTable();
    else
        return AsMethodTable();
}

inline TypeHandle TypeHandle::GetElementType() const
{
    WRAPPER_CONTRACT;
    
    if (IsUnsharedMT() || IsGenericVariable()) 
        return *this;

	return AsTypeDesc()->GetTypeParam().GetElementType();
}

inline BOOL TypeHandle::IsZapped() const
{
    WRAPPER_CONTRACT;
    
    return (GetZapModule() != NULL);
}

inline ArrayTypeDesc* TypeHandle::AsArray() const
{ 
    WRAPPER_CONTRACT;
    
    _ASSERTE(IsArray());
    
    ArrayTypeDesc* result = PTR_ArrayTypeDesc(m_asTAddr & ~2);
    PREFIX_ASSUME(result != NULL);
    return result;
}

// Methods to allow you get get a the two possible representations
inline MethodTable* TypeHandle::AsMethodTable() const
{        
    WRAPPER_CONTRACT;

    _ASSERTE(IsUnsharedMT());
    
    //_ASSERTE(m_asMT == NULL || m_asMT->IsCanonicalMethodTable());</STRIP>
    return PTR_MethodTable(m_asTAddr);
}

inline TypeDesc* TypeHandle::AsTypeDesc() const 
{
    WRAPPER_CONTRACT;
    
    _ASSERTE(IsTypeDesc());
    
    TypeDesc* result = PTR_TypeDesc(m_asTAddr & ~2);
    PREFIX_ASSUME(result != NULL);
    return result;
}


inline BOOL TypeHandle::IsTypicalTypeDefinition() const
{
    WRAPPER_CONTRACT;
    return !HasInstantiation() || IsGenericTypeDefinition();
}

#endif  // _TYPEHANDLE_INL_

