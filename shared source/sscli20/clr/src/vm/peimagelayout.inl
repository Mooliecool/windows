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

#ifndef PEIMAGEVIEW_INL_
#define PEIMAGEVIEW_INL_

#include "util.hpp"
#include "peimage.h"



inline BOOL PEImageLayout::IsLoadLibraryHandle()
{
    LEAF_CONTRACT;
    return FALSE;
}


inline const SString &PEImageLayout::GetPath()
{
    return m_pOwner?m_pOwner->GetPath():SString::Empty();
}

inline void PEImageLayout::AddRef()
{
    CONTRACT_VOID
    {
        PRECONDITION(m_refCount>0 && m_refCount < COUNT_T_MAX);
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACT_END;

    FastInterlockIncrement(&m_refCount);

    RETURN;
}

inline ULONG PEImageLayout::Release()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;
    ULONG result=FastInterlockDecrement(&m_refCount);
    if (result == 0 )
    {
        delete this;
    }
    return result;
}


inline PEImageLayout::~PEImageLayout()
{
    CONTRACTL
    {
        DESTRUCTOR_CHECK;
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

#ifndef DACCESS_COMPILE
#endif
}

inline PEImageLayout::PEImageLayout()
    : m_refCount(1)
    , m_pOwner(NULL)
    , m_bPersistent(FALSE)
{
    LEAF_CONTRACT;
}

inline void PEImageLayout::Startup()
{
    CONTRACT_VOID
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        POSTCONDITION(CheckStartup());
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACT_END;

    if (CheckStartup())
        RETURN;

    RETURN;
}

inline CHECK PEImageLayout::CheckStartup()
{
    WRAPPER_CONTRACT;
    CHECK_OK;
}

inline BOOL PEImageLayout::CompareBase(UPTR base, UPTR mapping)
{
    CONTRACTL
    {
        PRECONDITION(CheckPointer((PEImageLayout *)mapping));
        PRECONDITION(CheckPointer((PEImageLayout *)(base<<1),NULL_OK));
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    if (base==NULL) //we were searching for 'Any'
        return TRUE;
    return ((PEImageLayout*)mapping)->GetBase()==((PEImageLayout*)(base<<1))->GetBase();
  
}

inline void PEImageLayout::MakePersistent()
{
    LEAF_CONTRACT;
    m_bPersistent=TRUE;
}

inline BOOL PEImageLayout::IsPersistent()
{
    LEAF_CONTRACT;
    return m_bPersistent;
}


#endif //PEIMAGEVIEW_INL_
