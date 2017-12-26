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
// typekey.h
//
// ---------------------------------------------------------------------------
//
// Support for type lookups based on components of the type (as opposed to string)
// Used in
// * Table of constructed types (Module::m_pAvailableParamTypes)
// * Types currently being loaded (ClassLoader::m_pUnresolvedClassHash)
//
// Type handles are in one-to-one correspondence with TypeKeys
// In particular, note that tokens in the key are resolved TypeDefs
//
// ---------------------------------------------------------------------------

#ifndef _H_TYPEKEY
#define _H_TYPEKEY

class TypeKey
{
    // ELEMENT_TYPE_CLASS for ordinary classes and generic instantiations (including value types)
    // ELEMENT_TYPE_ARRAY and ELEMENT_TYPE_SZARRAY for array types
    // ELEMENT_TYPE_PTR and ELEMENT_TYPE_BYREF for pointer types
    // ELEMENT_TYPE_FNPTR for function pointer types
    CorElementType m_kind;

    union 
    {
        // m_kind = CLASS 
        struct 
        {
            Module *m_pModule;
            mdToken m_typeDef;
            DWORD   m_numGenericArgs; // 0 for non-generic types
            TypeHandle *m_pGenericArgs; // NULL for non-generic types
        } asClass;

        // m_kind = ARRAY, SZARRAY, PTR or BYREF
        struct 
        {
            void* m_paramType;   // The element type (actually a TypeHandle, but we don't want its constructor
                                 // run on a C++ union)
            DWORD m_rank;        // Non-zero for ARRAY, 1 for SZARRAY, 0 for PTR or BYREF
            BOOL m_isTemplateMethodTable; // TRUE if this key indexes the template method table for an array, rather than a type-desc
        } asParamType;

        // m_kind = FNPTR
        struct 
        {
            BYTE m_callConv;
            DWORD m_numArgs;
            TypeHandle* m_pRetAndArgTypes;
        } asFnPtr;
    } u; 

public:

    // Constructor for BYREF/PTR/ARRAY/SZARRAY types
    TypeKey(CorElementType etype, TypeHandle paramType, BOOL isTemplateMethodTable = FALSE, DWORD rank = 0)
    {
        WRAPPER_CONTRACT;
        PRECONDITION(rank > 0 && etype == ELEMENT_TYPE_ARRAY || 
                     rank == 1 && etype == ELEMENT_TYPE_SZARRAY ||
                     rank == 0 && (etype == ELEMENT_TYPE_PTR || etype == ELEMENT_TYPE_BYREF));
        PRECONDITION(CheckPointer(paramType));
        m_kind = etype;
        u.asParamType.m_paramType = paramType.AsPtr();
        u.asParamType.m_rank = rank;
        u.asParamType.m_isTemplateMethodTable = isTemplateMethodTable;
    }

    // Constructor for instantiated types
    TypeKey(Module *pModule, mdTypeDef token, DWORD numGenericArgs = 0, TypeHandle* pGenericArgs = NULL)
    {
        WRAPPER_CONTRACT;
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(TypeFromToken(token) == mdtTypeDef);
        PRECONDITION(!IsNilToken(token));
        PRECONDITION(CheckPointer(pGenericArgs, numGenericArgs == 0 ? NULL_OK : NULL_NOT_OK));
        m_kind = ELEMENT_TYPE_CLASS;
        u.asClass.m_pModule = pModule;
        u.asClass.m_typeDef = token;
        u.asClass.m_numGenericArgs = numGenericArgs;
        u.asClass.m_pGenericArgs = pGenericArgs;
    }
    
    // Constructor for function pointer type
    TypeKey(BYTE callConv, DWORD numArgs, TypeHandle* retAndArgTypes)
    {
        WRAPPER_CONTRACT;
        PRECONDITION(CheckPointer(retAndArgTypes));
        m_kind = ELEMENT_TYPE_FNPTR;
        u.asFnPtr.m_callConv = callConv;
        u.asFnPtr.m_numArgs = numArgs;
        u.asFnPtr.m_pRetAndArgTypes = retAndArgTypes;
    }

    CorElementType GetKind() const
    {
        LEAF_CONTRACT;
        return m_kind;
    }

    // Accessors on array/pointer types
    DWORD GetRank() const
    {
        LEAF_CONTRACT;
        PRECONDITION(CorTypeInfo::IsArray(m_kind));
        return u.asParamType.m_rank;
    }

    BOOL IsTemplateMethodTable() const
    {
        LEAF_CONTRACT;
        return u.asParamType.m_isTemplateMethodTable;
    }

    TypeHandle GetElementType() const
    {
        WRAPPER_CONTRACT;
        PRECONDITION(CorTypeInfo::IsModifier(m_kind));
        return TypeHandle::FromPtr(u.asParamType.m_paramType);
    }

    BOOL IsConstructed() const
    {
        LEAF_CONTRACT;
        return !(m_kind == ELEMENT_TYPE_CLASS && u.asClass.m_numGenericArgs == 0);
    }

    // Accessors on instantiated types
    Module* GetModule() const
    {
        WRAPPER_CONTRACT;
        if (m_kind == ELEMENT_TYPE_CLASS)
            return u.asClass.m_pModule;
        else if (CorTypeInfo::IsModifier(m_kind))
            return GetElementType().GetModule();
        else
            return NULL;
    }

    mdTypeDef GetTypeToken() const
    {
        LEAF_CONTRACT;
        PRECONDITION(m_kind == ELEMENT_TYPE_CLASS);
        return u.asClass.m_typeDef;
    }

    TypeHandle* GetInstantiation() const
    {
        LEAF_CONTRACT;
        PRECONDITION(m_kind == ELEMENT_TYPE_CLASS);
        return u.asClass.m_pGenericArgs;
    }

    DWORD GetNumGenericArgs()
    {
        LEAF_CONTRACT;
        PRECONDITION(m_kind == ELEMENT_TYPE_CLASS);
        return u.asClass.m_numGenericArgs;
    }

    BOOL HasInstantiation() 
    {
        LEAF_CONTRACT;
        return m_kind == ELEMENT_TYPE_CLASS && u.asClass.m_numGenericArgs != 0;
    }

    // Accessors on function pointer types
    DWORD GetNumArgs() const
    {
        LEAF_CONTRACT;
        PRECONDITION(m_kind == ELEMENT_TYPE_FNPTR);
        return u.asFnPtr.m_numArgs;
    }
        
    BYTE GetCallConv() const
    {
        LEAF_CONTRACT;
        PRECONDITION(m_kind == ELEMENT_TYPE_FNPTR);
        return u.asFnPtr.m_callConv;
    }
        
    TypeHandle* GetRetAndArgTypes() const
    {
        LEAF_CONTRACT;
        PRECONDITION(m_kind == ELEMENT_TYPE_FNPTR);
        return u.asFnPtr.m_pRetAndArgTypes;
    }       

    BOOL Equals(TypeKey *pKey)
    {
        WRAPPER_CONTRACT;
        return TypeKey::Equals(this, pKey);
    }

    // Comparison and hashing
    static BOOL Equals(TypeKey *pKey1, TypeKey *pKey2) 
    {
        WRAPPER_CONTRACT;
        if (pKey1->m_kind != pKey2->m_kind)
        {
            return FALSE;
        }
        if (pKey1->m_kind == ELEMENT_TYPE_CLASS)
        {
            if (pKey1->u.asClass.m_typeDef != pKey2->u.asClass.m_typeDef ||
                pKey1->u.asClass.m_pModule != pKey2->u.asClass.m_pModule ||
                pKey1->u.asClass.m_numGenericArgs != pKey2->u.asClass.m_numGenericArgs)
            {
                return FALSE;
            }
            for (DWORD i = 0; i < pKey1->u.asClass.m_numGenericArgs; i++)
            {
                if (pKey1->u.asClass.m_pGenericArgs[i] != pKey2->u.asClass.m_pGenericArgs[i])
                    return FALSE;
            }
            return TRUE;
        }
        else if (CorTypeInfo::IsModifier(pKey1->m_kind))
        {
            return pKey1->u.asParamType.m_paramType == pKey2->u.asParamType.m_paramType
                && pKey1->u.asParamType.m_rank == pKey2->u.asParamType.m_rank
                && pKey1->u.asParamType.m_isTemplateMethodTable == pKey2->u.asParamType.m_isTemplateMethodTable;
        }
        else
        {
            _ASSERTE(pKey1->m_kind == ELEMENT_TYPE_FNPTR);
            if (pKey1->u.asFnPtr.m_callConv != pKey2->u.asFnPtr.m_callConv ||
                pKey1->u.asFnPtr.m_numArgs != pKey2->u.asFnPtr.m_numArgs)
                return FALSE;
            
            // Includes return type
            for (DWORD i = 0; i <= pKey1->u.asFnPtr.m_numArgs; i++)
            {
                if (pKey1->u.asFnPtr.m_pRetAndArgTypes[i] != pKey2->u.asFnPtr.m_pRetAndArgTypes[i])
                    return FALSE;
            }
            return TRUE;
        }
    }

    DWORD ComputeHash()
    {
        if (m_kind == ELEMENT_TYPE_CLASS)
        {
            return ((DWORD_PTR)u.asClass.m_pModule ^ (DWORD_PTR)u.asFnPtr.m_numArgs ^ (DWORD_PTR)u.asClass.m_typeDef);
        }
        else if (CorTypeInfo::IsModifier(m_kind))
        {
            return ((DWORD_PTR)u.asParamType.m_paramType ^ (DWORD_PTR) u.asParamType.m_rank);
        }
        else return 0;
    }

    // Is this type part of an assembly loaded for introspection?
    BOOL IsIntrospectionOnly();


};


#endif /* _H_TYPEKEY */
