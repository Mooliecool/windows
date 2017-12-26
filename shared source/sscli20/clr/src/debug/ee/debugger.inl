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
// File: debugger.inl
//
// Inline definitions for the Left-Side of the CLR debugging services
// This is logically part of the header file. 
//
//*****************************************************************************

#ifndef DEBUGGER_INL_
#define DEBUGGER_INL_

//=============================================================================
// Inlined methods for Debugger.
//=============================================================================
inline bool Debugger::HasLazyData()
{
    LEAF_CONTRACT;
    return (m_pLazyData != NULL);
}
inline RCThreadLazyInit *Debugger::GetRCThreadLazyData()
{
    LEAF_CONTRACT;
    return &(GetLazyData()->m_RCThread);
}

inline DebuggerLazyInit *Debugger::GetLazyData() 
{ 
    LEAF_CONTRACT;
    _ASSERTE(m_pLazyData != NULL); 
    return m_pLazyData; 
}

inline DebuggerModuleTable * Debugger::GetModuleTable() 
{ 
    LEAF_CONTRACT;

    return m_pModules; 
}


//=============================================================================
// Inlined methods for DebuggerModule.
//=============================================================================


//-----------------------------------------------------------------------------
// Constructor for a Debugger-Module.
//-----------------------------------------------------------------------------
inline DebuggerModule::DebuggerModule(Module* pRuntimeModule, AppDomain *pAppDomain) :
        m_enableClassLoadCallbacks(FALSE),
        m_fDefaultJMCStatus(false),
        m_cTotalJMCFuncs(0),
        m_pPrimaryModule(NULL),
        m_pRuntimeModule(pRuntimeModule),
        m_pAppDomain(pAppDomain)
{
    LOG((LF_CORDB,LL_INFO10000, "DM::DM this:0x%x Module:0x%x AD:0x%x\n",
        this, pRuntimeModule, pAppDomain));

    // Pick a primary module.
    // Arguably, this could be in DebuggerModuleTable::AddModule
    PickPrimaryModule();


    // Do we have any optimized code?

    if (CORLaunchedByDebugger())
    {
        // We don't know if we have any optimized code or not. This will be decided by the 
        // end of the load module callback.        
        m_fHasOptimizedCode = false;
    }
    else
    {
        DWORD dwDebugBits = pRuntimeModule->GetDebuggerInfoBits();
        m_fHasOptimizedCode = CORDebuggerAllowJITOpts(dwDebugBits);
    }
}
    
//-----------------------------------------------------------------------------
// Returns true if we have any optimized code in the module.
// JMC-probes aren't emitted in optimized code. 
//-----------------------------------------------------------------------------
inline bool DebuggerModule::HasAnyOptimizedCode() 
{ 
    LEAF_CONTRACT;

    return m_fHasOptimizedCode; 
}

//-----------------------------------------------------------------------------
// If the debugger updates things to allow optimized code, 
// then we have to track that.
//-----------------------------------------------------------------------------
inline void DebuggerModule::MarkAllowedOptimizedCode()
{
    LEAF_CONTRACT;

    m_fHasOptimizedCode = true;
}

//-----------------------------------------------------------------------------
// If the debugger updates the JIT compiler flags to disallow JIT optimization,
// then we have to track that.
//-----------------------------------------------------------------------------
inline void DebuggerModule::UnmarkAllowedOptimizedCode()
{
    LEAF_CONTRACT;

    m_fHasOptimizedCode = false;
}

//-----------------------------------------------------------------------------
// Return true if we've enabled class-load callbacks.
//-----------------------------------------------------------------------------
inline BOOL DebuggerModule::ClassLoadCallbacksEnabled(void) 
{ 
    return m_enableClassLoadCallbacks; 
}

//-----------------------------------------------------------------------------
// Set whether we should enable class-load callbacks for this module.
//-----------------------------------------------------------------------------
inline void DebuggerModule::EnableClassLoadCallbacks(BOOL f) 
{ 
    if (m_enableClassLoadCallbacks != f)
    {
        if (f)
        {
            _ASSERTE(g_pDebugger != NULL);
            g_pDebugger->IncrementClassLoadCallbackCount();
        }
        else
        {
            _ASSERTE(g_pDebugger != NULL);
            g_pDebugger->DecrementClassLoadCallbackCount();
        }

        m_enableClassLoadCallbacks = f;
    }    
}

//-----------------------------------------------------------------------------
// Return the appdomain that this module exists in.
//-----------------------------------------------------------------------------
inline AppDomain* DebuggerModule::GetAppDomain() 
{
    return m_pAppDomain;
}

//-----------------------------------------------------------------------------
// Return the EE module that this module corresponds to.
//-----------------------------------------------------------------------------
inline Module * DebuggerModule::GetRuntimeModule()
{
    return m_pRuntimeModule;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
inline DebuggerModule * DebuggerModule::GetPrimaryModule() 
{
    _ASSERTE(m_pPrimaryModule != NULL);
    return m_pPrimaryModule; 
}

//-----------------------------------------------------------------------------
// This is called by DebuggerModuleTable to set our primary module.
//-----------------------------------------------------------------------------
inline void DebuggerModule::SetPrimaryModule(DebuggerModule * pPrimary)
{
    _ASSERTE(pPrimary != NULL);
    // Our primary module must by definition refer to the same runtime module as us 
    _ASSERTE(pPrimary->GetRuntimeModule() == this->GetRuntimeModule());

    LOG((LF_CORDB, LL_EVERYTHING, "DM::SetPrimaryModule - this=%p, pPrimary=%p\n", this, pPrimary));
    m_pPrimaryModule = pPrimary;        
}

#endif  // DEBUGGER_INL_
