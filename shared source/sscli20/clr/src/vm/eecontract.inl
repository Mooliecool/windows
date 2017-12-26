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
// ---------------------------------------------------------------------------
// EEContract.cpp
// ---------------------------------------------------------------------------

#include "dbginterface.h"

void EEContract::DoChecks()
{
    PRESERVE_PROBE_ACROSS_DEBUG_CODE;
    
    // this is debug scenario, but it uses strings, so shut up SO checking
    CONTRACT_VIOLATION(SOToleranceViolation);
    
    // This tells the static contract checker to ignore the TRIGGERSGC in this function
     SCAN_IGNORE_TRIGGER;

    Contract::DoChecks();

    // GC mode check

    if (!m_gcModeDisabled)
    {
        switch (m_gcMode)
        {
        case GCUnspecified:
            // allow this to mean any for now
        case GCAny:
            break;

        case GCCooperative:
            
            //
            // Check if this is the debugger helper thread and has the runtime
            // stoppped.  If both of these things are true, then we do not care
            // whether we are in COOP mode or not.
            //
            if ((g_pDebugInterface != NULL) && 
                g_pDebugInterface->ThisIsHelperThread() &&
                g_pDebugInterface->IsStopped())
            {
                break;
            }

            _ASSERTE(m_Thread->PreemptiveGCDisabled());
            break;

        case GCPreemptive:
            _ASSERTE(!m_Thread->PreemptiveGCDisabled());
            break;
        }
    }

    // GC Trigger check

    if (!m_gcTriggerDisabled)
    {
        switch (m_gcTrigger)
        {
        case GCTriggerUnspecified:
            // Defaults to Triggers

        case GCTriggers:
            TRIGGERSGC();
            break;

        case GCForbid:
            BEGINFORBIDGC();
            break;
        }
    }
}

void ~EEContract::Contract
{
}

