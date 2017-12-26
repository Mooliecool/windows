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

#ifndef _FPTRSTUBS_H
#define _FPTRSTUBS_H

#include "common.h"

// FuncPtrStubs contains stubs that is used by GetMultiCallableAddrOfCode() if
// the function has not been jitted. Using a stub decouples ldftn from
// the prestub, so prestub does not need to be backpatched.
//
// This stub is also used in other places which need a function pointer

class FuncPtrStubs
{
public :
    FuncPtrStubs();

    Precode*            Lookup(MethodDesc * pMD, PrecodeType type);
    PBYTE               GetFuncPtrStub(MethodDesc * pMD, PrecodeType type);

    Precode*            Lookup(MethodDesc * pMD)
    {
        return Lookup(pMD, GetDefaultType(pMD));
    }

    PBYTE               GetFuncPtrStub(MethodDesc * pMD)
    {
        return GetFuncPtrStub(pMD, GetDefaultType(pMD));
    }

    static PrecodeType GetDefaultType(MethodDesc* pMD);

private:
    Crst                m_hashTableCrst;

    struct PrecodeKey
    {
        PrecodeKey(MethodDesc* pMD, PrecodeType type)
            : m_pMD(pMD), m_type(type)
        {
        }

        MethodDesc*     m_pMD;
        PrecodeType     m_type;
    };

    class PrecodeTraits : public NoRemoveSHashTraits< DefaultSHashTraits<Precode*> >
    {
    public:
        typedef PrecodeKey key_t;

        static key_t GetKey(element_t e)
        {
            LEAF_CONTRACT;
            return PrecodeKey(e->GetMethodDesc(), e->GetType());
        }
        static BOOL Equals(key_t k1, key_t k2)
        {
            LEAF_CONTRACT;
            return (k1.m_pMD == k2.m_pMD) && (k1.m_type == k2.m_type);
        }
        static count_t Hash(key_t k)
        {
            LEAF_CONTRACT;
            return (count_t)(size_t)k.m_pMD ^ k.m_type;
        }
    };

    SHash<PrecodeTraits>    m_hashTable;    // To find a existing stub for a method
};

#endif // _FPTRSTUBS_H
