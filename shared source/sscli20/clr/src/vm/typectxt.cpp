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
// typectxt.cpp
//
// Simple struct to record the data necessary to interpret ELEMENT_TYPE_VAR
// and ELEMENT_TYPE_MVAR within pieces of metadata, in particular within 
// signatures parsed by MetaSig and SigPointer.
//


#include "common.h"
#include "method.hpp"
#include "typehandle.h"
#include "field.h"



void SigTypeContext::InitTypeContext(MethodDesc *md, TypeHandle *exactClassInst, TypeHandle *exactMethodInst, SigTypeContext *pRes)
{
    WRAPPER_CONTRACT;
    STATIC_CONTRACT_SO_TOLERANT;
    MethodTable *pMT = md->GetMethodTable();

    if (pMT->IsArray())
    {
        pRes->m_classInstCount = 1;
        pRes->m_classInst = exactClassInst ? exactClassInst : pMT->GetClassOrArrayInstantiation();
    }
    else
    {
        pRes->m_classInstCount = pMT->GetNumGenericArgs();
        pRes->m_classInst = exactClassInst;
    }
    pRes->m_methInstCount = md->GetNumGenericMethodArgs();
    pRes->m_methInst = exactMethodInst;
}

void SigTypeContext::InitTypeContext(MethodDesc *md, SigTypeContext *pRes)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
 
        PRECONDITION(CheckPointer(md));
    } CONTRACTL_END;
 
    MethodTable *pMT = md->GetMethodTable();
    if (pMT->IsArray())
    {
        pRes->m_classInstCount = 1;
        pRes->m_classInst = pMT->GetClassOrArrayInstantiation();
    }
    else
    {
        pRes->m_classInstCount = pMT->GetNumGenericArgs();
        pRes->m_classInst = pMT->GetInstantiation();
    }
    pRes->m_methInstCount = md->GetNumGenericMethodArgs();
    pRes->m_methInst = md->GetMethodInstantiation();
}
 
void SigTypeContext::InitTypeContext(MethodDesc *md, TypeHandle declaringType, SigTypeContext *pRes)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
 
        PRECONDITION(CheckPointer(md));
    } CONTRACTL_END;
 
    if (declaringType.IsNull())
    {
        SigTypeContext::InitTypeContext(md, pRes);
    }
    else 
    {
        MethodTable *pMDMT = md->GetMethodTable();
        if (pMDMT->IsArray())
        {
            pRes->m_classInstCount = 1;
            pRes->m_classInst = declaringType.GetClassOrArrayInstantiation();
        }
        else 
        {
            pRes->m_classInstCount = pMDMT->GetNumGenericArgs();
            pRes->m_classInst = declaringType.GetInstantiationOfParentClass(pMDMT);
        }
        pRes->m_methInstCount = md->GetNumGenericMethodArgs();
        pRes->m_methInst = md->GetMethodInstantiation();
    }
}
 
void SigTypeContext::InitTypeContext(MethodDesc *md, TypeHandle declaringType, TypeHandle *exactMethodInst, SigTypeContext *pRes)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
 
        PRECONDITION(CheckPointer(md));
    } CONTRACTL_END;
 
    if (declaringType.IsNull())
    {
        SigTypeContext::InitTypeContext(md, pRes);
    }
    else 
    {
        MethodTable *pMDMT = md->GetMethodTable();
        if (pMDMT->IsArray())
        {
            pRes->m_classInstCount = 1;
            pRes->m_classInst = declaringType.GetClassOrArrayInstantiation();
        }
        else 
        {
            pRes->m_classInstCount = pMDMT->GetNumGenericArgs();
            pRes->m_classInst = declaringType.GetInstantiationOfParentClass(pMDMT);
        }
        pRes->m_methInstCount = md->GetNumGenericMethodArgs();
    }
    pRes->m_methInst = exactMethodInst ? exactMethodInst : md->GetMethodInstantiation();
}
 
void SigTypeContext::InitTypeContext(FieldDesc *pFD, TypeHandle declaringType, SigTypeContext *pRes)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
 
        PRECONDITION(CheckPointer(declaringType, NULL_OK));
        PRECONDITION(CheckPointer(pFD));
    } CONTRACTL_END;
    LEAF_CONTRACT;
    InitTypeContext(pFD->GetNumGenericClassArgs(),pFD->GetExactClassInstantiation(declaringType),0,NULL, pRes);
}

 
void SigTypeContext::InitTypeContext(TypeHandle th, SigTypeContext *pRes)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        SO_TOLERANT;
    } CONTRACTL_END;

    if (th.IsNull())
    {
        InitTypeContext(pRes);
    }
    else if (th.GetMethodTable()->IsArray())
    {
        InitTypeContext(1, th.GetMethodTable()->GetClassOrArrayInstantiation(), 0, NULL,pRes);
    }
    else
    {
        InitTypeContext(th.GetNumGenericArgs(), th.GetInstantiation(), 0, NULL,pRes);
    }
}


const SigTypeContext * SigTypeContext::GetOptionalTypeContext(MethodDesc *md, TypeHandle declaringType, SigTypeContext *pRes)
{ 
    WRAPPER_CONTRACT; 
    
    _ASSERTE(md);
    if (md->HasClassOrMethodInstantiation()  || md->GetMethodTable()->IsArray())
    {
        SigTypeContext::InitTypeContext(md, declaringType,pRes);
        return pRes;
    }
    else
    {
        _ASSERTE(pRes->m_classInstCount == 0);
        _ASSERTE(pRes->m_classInst == NULL);
        _ASSERTE(pRes->m_methInstCount == 0);
        _ASSERTE(pRes->m_methInst == NULL);
        return NULL;
    }
}

const SigTypeContext * SigTypeContext::GetOptionalTypeContext(TypeHandle th, SigTypeContext *pRes)
{ 
    WRAPPER_CONTRACT; 
    
    _ASSERTE (!th.IsNull());
    if (th.HasInstantiation() || th.GetMethodTable()->IsArray())
    { 
        SigTypeContext::InitTypeContext(th,pRes);
        return pRes;
    }
    else
    {
        // It should already have been null-initialized when allocated on the stack.
        _ASSERTE(pRes->m_classInstCount == 0);
        _ASSERTE(pRes->m_classInst == NULL);
        _ASSERTE(pRes->m_methInstCount == 0);
        _ASSERTE(pRes->m_methInst == NULL);
        return NULL;
    }
}


BOOL SigTypeContext::Equal(const SigTypeContext *pCtx1, const SigTypeContext *pCtx2)
{
    WRAPPER_CONTRACT;

    // Compare class inst counts
    if (pCtx1->m_classInstCount != pCtx2->m_classInstCount)
        return FALSE;

    // Compare method inst counts
    if (pCtx1->m_methInstCount != pCtx2->m_methInstCount)
        return FALSE;

    DWORD i;

    // Compare class inst values
    for (i = 0; i < pCtx1->m_classInstCount; i++) {
        if (pCtx1->m_classInst[i] != pCtx2->m_classInst[i])
            return FALSE;
    }

    // Compare method inst values
    for (i = 0; i < pCtx1->m_methInstCount; i++) {
        if (pCtx1->m_methInst[i] != pCtx2->m_methInst[i])
            return FALSE;
    }

    return TRUE;
}

