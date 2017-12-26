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
// File: MethodIter.cpp
// Iterate through jitted instances of a method.
//*****************************************************************************

#include "common.h"
#include "methoditer.h"


BOOL LoadedMethodDescIterator::Next()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END
    if (m_fFirstTime == TRUE)
    {
        m_fFirstTime = FALSE;
        
        // This is the 1st time we've called Next(). must Initialize iterator
        if (m_mainMD == NULL)
        {
			m_mainMD = m_module->LookupMethodDef(m_md);
		}
		
        // note m_mainMD should be sufficiently restored to allow us to get
        // at the method table, flags and token etc.
        if (!m_mainMD)
        {
            return FALSE;
        }        

        // Needs to work w/ non-generic methods too.
        if (!m_mainMD->HasClassOrMethodInstantiation())
        {
            return TRUE;
        }
 
        m_assemIterator = m_pAppDomain->IterateAssembliesEx((AssemblyIterationFlags) (kIncludeLoaded|kIncludeExecution));
    }
    else
    {
        // This is the 2nd or more time we called Next().
        
        // If the method + type is not generic, then nothing more to iterate.
        if (!m_mainMD->HasClassOrMethodInstantiation())
        {
            return FALSE;
        }
        goto ADVANCE_METHOD;
    }
    
ADVANCE_ASSEMBLY:
    if  (!m_assemIterator.Next())
        return FALSE;
    
    m_moduleIterator = m_assemIterator.GetDomainAssembly()->IterateModules(FALSE);
    
ADVANCE_MODULE:
    if  (!m_moduleIterator.Next())
        goto ADVANCE_ASSEMBLY;
    if  (!m_moduleIterator.GetDomainFile()->IsLoaded()||m_moduleIterator.GetLoadedModule()->IsResource())
        goto ADVANCE_MODULE;
    
    if (m_mainMD->HasClassInstantiation())
        m_typeIterator.Reset();
    else
        m_startedNonGenericType = FALSE;
    
ADVANCE_TYPE:
    if (m_mainMD->HasClassInstantiation())
    {
        if (!m_moduleIterator.GetLoadedModule()->GetAvailableParamTypes()->FindNext(&m_typeIterator, &m_typeIteratorEntry))
            goto ADVANCE_MODULE;
        if (CORCOMPILE_IS_TOKEN_TAGGED(m_typeIteratorEntry->data.AsTAddr()))
            goto ADVANCE_TYPE;

        
        TypeHandle th = m_typeIteratorEntry->data;
        
        if (th.IsEncodedFixup())
            goto ADVANCE_TYPE;
        
        if (!th.IsUnsharedMT())
            goto ADVANCE_TYPE;
        
        if (!th.IsRestored())
            goto ADVANCE_TYPE;
        
        MethodTable *pMT = th.GetMethodTable();
        
        // Make sure we can get the class token it is in the EEClass
        if (CORCOMPILE_IS_TOKEN_TAGGED(pMT->GetClass()))
            goto ADVANCE_TYPE;
        EEClass *pClass = pMT->GetClass();
        
        // Check the class token 
        if (pClass->GetCl() !=  m_mainMD->GetMethodTable()->GetCl())
            goto ADVANCE_TYPE;
        
        // Check the module pointer is available 
        if (CORCOMPILE_IS_TOKEN_TAGGED(pClass->GetModule()))
            goto ADVANCE_TYPE;
        
        // Check the module is correct
        if (pClass->GetModule() !=  m_module)
            goto ADVANCE_TYPE;
    }
    else if (m_startedNonGenericType)
        goto ADVANCE_MODULE;
    else
        m_startedNonGenericType = TRUE;
    
    if (m_mainMD->HasMethodInstantiation())
        m_methodIterator.Reset();
    else
        m_startedNonGenericMethod = FALSE;
    
ADVANCE_METHOD:
    if (m_mainMD->HasMethodInstantiation())
    {
        if (!m_moduleIterator.GetLoadedModule()->GetInstMethodHashTable()->FindNext(&m_methodIterator, &m_methodIteratorEntry))
            goto ADVANCE_TYPE;
        if (CORCOMPILE_IS_TOKEN_TAGGED(PTR_TO_TADDR(m_methodIteratorEntry->data)))
            goto ADVANCE_METHOD;
        if (!m_methodIteratorEntry->data->IsRestored())
            goto ADVANCE_METHOD;
        if (m_methodIteratorEntry->data->GetModule() != m_module)
            goto ADVANCE_METHOD;
        if (m_methodIteratorEntry->data->GetMemberDef() != m_md)
            goto ADVANCE_METHOD;
    }
    else if (m_startedNonGenericMethod)
        goto ADVANCE_TYPE;
    else
        m_startedNonGenericMethod = TRUE;
    return TRUE;
}
 
MethodDesc *LoadedMethodDescIterator::Current()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(m_mainMD));
    }
    CONTRACTL_END
    
    
    if (m_mainMD->HasMethodInstantiation())
    {
        _ASSERTE(m_methodIteratorEntry);
        return m_methodIteratorEntry->data;
    }
    
    if (!m_mainMD->HasClassInstantiation())
    {   
        // No Method or Class instantiation,then it's not generic.
        return m_mainMD;
    }
    
    MethodTable *pMT = m_typeIteratorEntry->data.GetMethodTable();
    PREFIX_ASSUME(pMT != NULL);
    _ASSERTE(pMT);
    
    return pMT->GetMethodDescForSlot(m_mainMD->GetSlot());
}

// Initialize the iterator. It will cover generics + prejitted;
// but it is not EnC aware.
void
LoadedMethodDescIterator::Start(
    AppDomain * pAppDomain, 
    Module *pModule,
    mdMethodDef md)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(pAppDomain));
    }
    CONTRACTL_END
    m_mainMD = NULL;
    m_module = pModule;
    m_md = md;
    m_pAppDomain = pAppDomain;
    m_fFirstTime = TRUE;

    _ASSERTE(TypeFromToken(m_md) == mdtMethodDef);
    
}


void
LoadedMethodDescIterator::Start(
    AppDomain     *pAppDomain, 
    Module          *pModule,
    mdMethodDef     md,
    MethodDesc      *pMethodDesc)
{
    Start(pAppDomain, pModule, md);
    m_mainMD = pMethodDesc;
}

LoadedMethodDescIterator::LoadedMethodDescIterator(void)
{
    LEAF_CONTRACT;
    m_mainMD = NULL;
    m_module = NULL;
    m_md = mdTokenNil;
    m_pAppDomain = NULL;
}
