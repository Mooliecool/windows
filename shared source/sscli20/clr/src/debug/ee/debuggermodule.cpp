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
// File: DebuggerModule.cpp
//
// Stuff for tracking DebuggerModules.
//
//*****************************************************************************

#include "stdafx.h"
#include "../inc/common.h"
#include "perflog.h"
#include "eeconfig.h" // This is here even for retail & free builds...
#include "vars.hpp"
#include <limits.h>
#include "ilformatter.h"
#include "memoryreport.h"
#include "debuginfostore.h"


/* ------------------------------------------------------------------------ *
 * Debugger Module routines
 * ------------------------------------------------------------------------ */

// This sets the JMC status for the entire module.
// fStatus - default status for whole module
// cTokens - size of array in pTokens
// pTokens - array of tokens (methods & classes) that have JMC status !fStatus.
void DebuggerModule::SetJMCStatus(bool fStatus, ULONG32 cTokens, mdToken * pTokens)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(ThisIsHelperThreadWorker());
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO100000, "DM::SetJMCStatus, status=%d, this=%p\n", fStatus, this));

    // Should only be doing this on primary modules.
    _ASSERTE(this->GetPrimaryModule() == this);

    // Token list is not yet implemented
    _ASSERTE(cTokens == 0);
    _ASSERTE(pTokens == NULL);

    // Ensure that all active DMIs have our status.
    // All new DMIs can lookup their status from us.
    // This should also update the module count of active JMC DMI's.
    DebuggerMethodInfoTable * pTable = g_pDebugger->GetMethodInfoTable();

    if (pTable != NULL)
    {
        Debugger::DebuggerDataLockHolder debuggerDataLockHolder(g_pDebugger);
        HASHFIND info;

        for (DebuggerMethodInfo *dmi = pTable->GetFirstMethodInfo(&info);
            dmi != NULL;
            dmi = pTable->GetNextMethodInfo(&info))
        {
            if (dmi->GetRuntimeModule() == m_pRuntimeModule)
            {
                // This DMI is in this module, so update its status
                dmi->SetJMCStatus(fStatus);
            }
        }
    }

    m_fDefaultJMCStatus = fStatus;

#ifdef _DEBUG
    // If we're disabling JMC in this module, then we shouldn't
    // have any active JMC functions.
    if (!fStatus && (cTokens == 0))
    {
        _ASSERTE(m_cTotalJMCFuncs == 0);
    }
#endif
}


// Select the primary module.
// Primary Modules are selected DebuggerModules that map 1:1 w/ Module*.
// If the runtime module is not shared, then we're our own Primary Module.
// If the Runtime module is shared, the primary module is some specific instance.
// Note that a domain-neutral module can be loaded into multiple domains without
// being loaded into the default domain, and so there is no "primary module" as far
// as the CLR is concerned - we just pick any one and call it primary.
void DebuggerModule::PickPrimaryModule()
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    Debugger::DebuggerDataLockHolder ch(g_pDebugger);

    LOG((LF_CORDB, LL_INFO100000, "DM::PickPrimaryModule, this=0x%p\n", this));

    // We're our own primary module, unless something else proves otherwise.
    // Note that we should be able to skip all of this if this module is not domain neutral
    m_pPrimaryModule = this;

    // This should be thread safe because our creation for the DebuggerModules
    // are serialized.

    // Lookup our Runtime Module. If it's already in there,
    // then
    DebuggerModuleTable * pTable = g_pDebugger->GetModuleTable();

    // If the table doesn't exist yet, then we must be a primary module.
    if (pTable == NULL)
    {
        LOG((LF_CORDB, LL_INFO100000, "DM::PickPrimaryModule, this=0x%p, table not created yet\n", this));
        return;
    }

    // Look through existing module list to find a common primary DebuggerModule
    // for the given EE Module. We don't know what order we'll traverse in.

    HASHFIND f;
    for (DebuggerModule * m = pTable->GetFirstModule(&f);
         m != NULL;
         m = pTable->GetNextModule(&f))
    {

        if (m->GetRuntimeModule() == this->GetRuntimeModule())
        {
            // Make sure we're picking another primary module.
            if (m->GetPrimaryModule() == m)
            {
                // If we find another one, it must be domain neutral
                _ASSERTE( m_pRuntimeModule->GetAssembly()->IsDomainNeutral() );
                
                m_pPrimaryModule = m;
                LOG((LF_CORDB, LL_INFO100000, "DM::PickPrimaryModule, this=0x%p, primary=0x%p\n", this, m));
                return;
            }
        }
    } // end for

    // If we got here, then this instance is a Primary Module.
    LOG((LF_CORDB, LL_INFO100000, "DM::PickPrimaryModule, this=%p is first, primary.\n", this));
}


// Return true if this module has any live (jitted) JMC functions.
// If a module has no jitted JMC functions, then it's as if it's a
// non-user module.
bool DebuggerModule::HasAnyJMCFunctions()
{
    LEAF_CONTRACT;

    // Only update counts on the primary module.
    _ASSERTE(this->GetPrimaryModule() == this);

    // If we have any live JMC funcs in us, then we're a JMC module.
    // We count JMC functions when we either explicitly toggle their status
    // or when we get the DMI for them (which happens in a jit-complete).
    // Since we don't get the jit-completes for ngen modules, we also check the module's
    // "default" status. This means we may err on the side of believing we have
    // JMC methods.
    return (m_cTotalJMCFuncs > 0) || m_fDefaultJMCStatus;
}

// Alter our module's count of JMC functions.
// Since these may be called on multiple threads (say 2 threads are jitting
// methods within a module), make it thread safe.
void DebuggerModule::IncJMCFuncCount()
{
    LEAF_CONTRACT;

    // Only update counts on the primary module.
    _ASSERTE(this->GetPrimaryModule() == this);

    InterlockedIncrement(&m_cTotalJMCFuncs);
}

void DebuggerModule::DecJMCFuncCount()
{
    LEAF_CONTRACT;

    // Only update counts on the primary module.
    _ASSERTE(this->GetPrimaryModule() == this);

    InterlockedDecrement(&m_cTotalJMCFuncs);
}

// DMIs are lazily created. Let them lookup what the default is.
// Status is only updated on the primary module, so we get that status.
bool DebuggerModule::LookupMethodDefaultJMCStatus(DebuggerMethodInfo * dmi)
{
    LEAF_CONTRACT;


    _ASSERTE(dmi != NULL);
    _ASSERTE(dmi->GetRuntimeModule() == m_pRuntimeModule);
    return this->GetPrimaryModule()->m_fDefaultJMCStatus;
}

#ifndef DACCESS_COMPILE


DebuggerModuleTable::DebuggerModuleTable() : CHashTableAndData<CNewZeroData>(101)
{
    WRAPPER_CONTRACT;

    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
    NewInit(101, sizeof(DebuggerModuleEntry), 101);
}

DebuggerModuleTable::~DebuggerModuleTable()
{
    WRAPPER_CONTRACT;

    _ASSERTE(ThreadHoldsLock());
    Clear();
}


#ifdef _DEBUG
bool DebuggerModuleTable::ThreadHoldsLock()
{
    // In shutdown (g_fProcessDetach), the shutdown thread implicitly holds all locks.
    return g_fProcessDetach || g_pDebugger->HasDebuggerDataLock();
}
#endif

//
// RemoveModules removes any module loaded into the given appdomain from the hash.  This is used when we send an
// ExitAppdomain event to ensure that there are no leftover modules in the hash. This can happen when we have shared
// modules that aren't properly accounted for in the CLR. We miss sending UnloadModule events for those modules, so
// we clean them up with this method.
//
void DebuggerModuleTable::RemoveModules(AppDomain *pAppDomain)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    LOG((LF_CORDB, LL_INFO1000, "DMT::RM removing all modules from AD 0x%08x\n", pAppDomain));

    _ASSERTE(ThreadHoldsLock());

    HASHFIND hf;
    DebuggerModuleEntry *pDME = (DebuggerModuleEntry *) FindFirstEntry(&hf);

    while (pDME != NULL)
    {
        DebuggerModule *pDM = pDME->module;

        if (pDM->GetAppDomain() == pAppDomain)
        {
            LOG((LF_CORDB, LL_INFO1000, "DMT::RM removing DebuggerModule 0x%08x\n", pDM));

            // Defer to the normal logic in RemoveModule for the actual removal. This accuratley simulates what
            // happens when we process an UnloadModule event.
            RemoveModule(pDM->GetRuntimeModule(), pAppDomain);

            // Start back at the first entry since we just modified the hash.
            pDME = (DebuggerModuleEntry *) FindFirstEntry(&hf);
        }
        else
        {
            pDME = (DebuggerModuleEntry *) FindNextEntry(&hf);
        }
    }

    LOG((LF_CORDB, LL_INFO1000, "DMT::RM done removing all modules from AD 0x%08x\n", pAppDomain));
}

void DebuggerModuleTable::Clear()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(ThreadHoldsLock());

    HASHFIND hf;
    DebuggerModuleEntry *pDME;

    pDME = (DebuggerModuleEntry *) FindFirstEntry(&hf);

    while (pDME)
    {
        DebuggerModule *pDM = pDME->module;
        Module         *pEEM = pDM->GetRuntimeModule();

        TRACE_FREE(pDME->module);
        DeleteInteropSafe(pDM);
        Delete(HASH(pEEM), (HASHENTRY *) pDME);

        pDME = (DebuggerModuleEntry *) FindFirstEntry(&hf);
    }

    CHashTableAndData<CNewZeroData>::Clear();
}

void DebuggerModuleTable::AddModule(DebuggerModule *module)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(ThreadHoldsLock());

    _ASSERTE(module != NULL);

    LOG((LF_CORDB, LL_EVERYTHING, "DMT::AM: DebuggerMod:0x%x Module:0x%x AD:0x%x\n",
        module, module->GetRuntimeModule(), module->GetAppDomain()));

    ((DebuggerModuleEntry*)Add(HASH(module->GetRuntimeModule())))->module =
      module;

    // Don't need to update the primary module since it was set when we created
    // the module.
    _ASSERTE(module->GetPrimaryModule() != NULL);
}

//-----------------------------------------------------------------------------
// Remove a DebuggerModule  from the module table.
// This occurs in response to AppDomain unload.
// Note that this doesn't necessarily mean the EE Module is being unloaded (it may be shared)
//-----------------------------------------------------------------------------
void DebuggerModuleTable::RemoveModule(Module* module, AppDomain *pAppDomain)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(module != NULL);
    _ASSERTE(ThreadHoldsLock());
    
    DebuggerModule * pDeletedModule = NULL;

    LOG((LF_CORDB, LL_EVERYTHING, "DMT::RM: mod:0x%x AD:0x%x neutral:0x%x\n",
        module, pAppDomain, module->GetAssembly()->IsDomainNeutral() ));

    // If this is a domain neutral module, then scan the complete list of DebuggerModules looking
    // for the one with a matching appdomain id.
    // Note: we have to make sure to lookup the module with the app domain parameter if the module lives in a shared
    // assembly or the system assembly.                                     
    _ASSERTE( SystemDomain::SystemAssembly()->IsDomainNeutral() );
    if (module->GetAssembly()->IsDomainNeutral())
    {
        // This module is being unloaded from a specific AppDomain, but may still exist in other AppDomains

        HASHFIND findmodule;
        DebuggerModuleEntry *moduleentry;

        for (moduleentry =  (DebuggerModuleEntry*) FindFirstEntry(&findmodule);
             moduleentry != NULL;
             moduleentry =  (DebuggerModuleEntry*) FindNextEntry(&findmodule))
        {
            DebuggerModule *pModule = moduleentry->module;

            if ((pModule->GetRuntimeModule() == module) &&
                (pModule->GetAppDomain() == pAppDomain))
            {
                LOG((LF_CORDB, LL_EVERYTHING, "DMT::RM: found 0x%x (DM:0x%x)\n",
                    moduleentry, moduleentry->module));

                pDeletedModule = pModule;

                // Remove from table
                Delete(HASH(module), (HASHENTRY *)moduleentry);

                break;
            }
        }
        // we should always find the module!!
        _ASSERTE (moduleentry != NULL);
    }
    else
    {
        // This module is not shared among multiple AppDomains
        
        DebuggerModuleEntry *entry
          = (DebuggerModuleEntry *) Find(HASH(module), KEY(module));

        _ASSERTE(entry != NULL); // it had better be in there!
        
        if (entry != NULL) // if its not, we fail gracefully in a free build
        {
            LOG((LF_CORDB, LL_EVERYTHING, "DMT::RM: found 0x%x (DM:0x%x)\n",
                entry, entry->module));

            pDeletedModule = entry->module;

            // Remove from table
            Delete(HASH(module), (HASHENTRY *)entry);

            // There should not be any other entry in the table for the same module
            _ASSERTE( Find(HASH(module), KEY(module)) == NULL );
        }
    }

    _ASSERTE(pDeletedModule != NULL);

    // Update the primary module pointers. If any other module had this as a
    // primary module, then we have to update that pointer (since we can't
    // have our primary module be deleted!)
    {
        HASHFIND findmodule;
        DebuggerModuleEntry *moduleentry;

        DebuggerModule * pNewPrimary = NULL;

        for (moduleentry =  (DebuggerModuleEntry*) FindFirstEntry(&findmodule);
             moduleentry != NULL;
             moduleentry =  (DebuggerModuleEntry*) FindNextEntry(&findmodule))
        {
            DebuggerModule *pOther = moduleentry->module;
            _ASSERTE(pOther != NULL);
            _ASSERTE(pOther != pDeletedModule);

            // If pOther's primary was just deleted, then update it.
            if (pOther->GetPrimaryModule() == pDeletedModule)
            {
                if (pNewPrimary == NULL)
                {
                    pNewPrimary = pOther;
                    LOG((LF_CORDB, LL_INFO1000, "DMT::RM changed primary module from 0x%p to 0x%p\n", pDeletedModule, pNewPrimary));
                }
                pOther->SetPrimaryModule(pNewPrimary);
            }
        } // end for
    }

    DeleteInteropSafe(pDeletedModule);
}


#endif // DACCESS_COMPILE

DebuggerModule *DebuggerModuleTable::GetModule(Module* module)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(module != NULL);
    _ASSERTE(ThreadHoldsLock());
    
    DebuggerModuleEntry *entry
      = (DebuggerModuleEntry *) Find(HASH(module), KEY(module));
    if (entry == NULL)
        return NULL;
    else
        return entry->module;
}

// We should never look for a NULL Module *
DebuggerModule *DebuggerModuleTable::GetModule(Module* module, AppDomain* pAppDomain)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(module != NULL);
    _ASSERTE(ThreadHoldsLock());


    HASHFIND findmodule;
    DebuggerModuleEntry *moduleentry;

    for (moduleentry =  (DebuggerModuleEntry*) FindFirstEntry(&findmodule);
         moduleentry != NULL;
         moduleentry =  (DebuggerModuleEntry*) FindNextEntry(&findmodule))
    {
        DebuggerModule *pModule = moduleentry->module;

        if ((pModule->GetRuntimeModule() == module) &&
            (pModule->GetAppDomain() == pAppDomain))
            return pModule;
    }

    // didn't find any match! So return a matching module for any app domain
    return NULL;
}

DebuggerModule *DebuggerModuleTable::GetFirstModule(HASHFIND *info)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(ThreadHoldsLock());

    DebuggerModuleEntry *entry = (DebuggerModuleEntry *) FindFirstEntry(info);
    if (entry == NULL)
        return NULL;
    else
        return entry->module;
}

DebuggerModule *DebuggerModuleTable::GetNextModule(HASHFIND *info)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(ThreadHoldsLock());

    DebuggerModuleEntry *entry = (DebuggerModuleEntry *) FindNextEntry(info);
    if (entry == NULL)
        return NULL;
    else
        return entry->module;
}


