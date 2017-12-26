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

#ifndef __eexcp_h__
#define __eexcp_h__

#include "corhlpr.h"
#include "daccess.h"

struct EE_ILEXCEPTION_CLAUSE;
typedef DPTR(EE_ILEXCEPTION_CLAUSE) PTR_EE_ILEXCEPTION_CLAUSE;

struct EE_ILEXCEPTION_CLAUSE  {
    CorExceptionFlag    Flags;  
    DWORD               TryStartPC;    
    DWORD               TryEndPC;
    DWORD               HandlerStartPC;  
    DWORD               HandlerEndPC;  
    union {
        void*           TypeHandle; 
        mdToken         ClassToken;
        DWORD           FilterOffset;
    };  
};

struct EE_ILEXCEPTION;
typedef DPTR(EE_ILEXCEPTION) PTR_EE_ILEXCEPTION;

struct EE_ILEXCEPTION : public COR_ILMETHOD_SECT_FAT 
{
    EE_ILEXCEPTION_CLAUSE Clauses[1];     // actually variable size

    void Init(unsigned ehCount) 
    {
        LEAF_CONTRACT;

        SetKind(CorILMethod_Sect_FatFormat);
        SetDataSize((unsigned)sizeof(EE_ILEXCEPTION_CLAUSE) * ehCount); 
    }

    unsigned EHCount() const 
    {
        LEAF_CONTRACT;

        return GetDataSize() / (DWORD) sizeof(EE_ILEXCEPTION_CLAUSE);
    }

    static unsigned Size(unsigned ehCount)
    {
        LEAF_CONTRACT;

        _ASSERTE(ehCount > 0);

        // EE_ILEXCEPTION includes 1 EE_ILEXCEPTION_CLAUSE, hence the ehCount - 1.
        return (sizeof(EE_ILEXCEPTION) + sizeof(EE_ILEXCEPTION_CLAUSE) * (ehCount - 1));
    }
    
    EE_ILEXCEPTION_CLAUSE *EHClause(unsigned i) 
    {
        return &(PTR_EE_ILEXCEPTION_CLAUSE(PTR_HOST_MEMBER_TADDR(EE_ILEXCEPTION,this,Clauses))[i]);
    }
};

#define COR_ILEXCEPTION_CLAUSE_CACHED_CLASS     0x10000000
#define COR_ILEXCEPTION_CLAUSE_MUST_CACHE_CLASS 0x20000000

inline BOOL HasCachedTypeHandle(EE_ILEXCEPTION_CLAUSE *EHClause)
{
    _ASSERTE(sizeof(EHClause->Flags) == sizeof(DWORD));
    return (EHClause->Flags & COR_ILEXCEPTION_CLAUSE_CACHED_CLASS);
}

inline void SetHasCachedTypeHandle(EE_ILEXCEPTION_CLAUSE *EHClause)
{
    _ASSERTE(! HasCachedTypeHandle(EHClause));
    EHClause->Flags = (CorExceptionFlag)(EHClause->Flags | COR_ILEXCEPTION_CLAUSE_CACHED_CLASS);
}

inline BOOL IsFinally(EE_ILEXCEPTION_CLAUSE *EHClause)
{
    LEAF_CONTRACT;

    return (EHClause->Flags & COR_ILEXCEPTION_CLAUSE_FINALLY);
}

inline BOOL IsFault(EE_ILEXCEPTION_CLAUSE *EHClause)
{
    LEAF_CONTRACT;

    return (EHClause->Flags & COR_ILEXCEPTION_CLAUSE_FAULT);
}

inline BOOL IsFaultOrFinally(EE_ILEXCEPTION_CLAUSE *EHClause)
{
    return IsFault(EHClause) || IsFinally(EHClause);
}

inline BOOL IsFilterHandler(EE_ILEXCEPTION_CLAUSE *EHClause)
{
    LEAF_CONTRACT;

    return EHClause->Flags & COR_ILEXCEPTION_CLAUSE_FILTER;
}

inline BOOL IsTypedHandler(EE_ILEXCEPTION_CLAUSE *EHClause)
{
    return ! (IsFilterHandler(EHClause) || IsFaultOrFinally(EHClause));
}

inline BOOL IsDuplicateClause(EE_ILEXCEPTION_CLAUSE* pEHClause)
{
    return pEHClause->Flags & COR_ILEXCEPTION_CLAUSE_DUPLICATED;
}


#endif // __eexcp_h__

