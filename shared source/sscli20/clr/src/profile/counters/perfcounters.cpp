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
// ===========================================================================
// File: PerfCounters.CPP
// 
// ===========================================================================


// PerfCounters.cpp
#include "stdafx.h"

// Always enable perf counters
#define ENABLE_PERF_COUNTERS
#include "perfcounters.h"

#include "ipcmanagerinterface.h"

#ifdef ENABLE_PERF_COUNTERS
extern IPCWriterInterface*	g_pIPCManagerInterface;


//-----------------------------------------------------------------------------
// Instantiate static data
//-----------------------------------------------------------------------------

PerfCounterIPCControlBlock PerfCounters::m_garbage;

PerfCounterIPCControlBlock * PerfCounters::m_pPrivatePerf = &PerfCounters::m_garbage;

BOOL PerfCounters::m_fInit = false;


//-----------------------------------------------------------------------------
// Should never actually instantiate this class, so assert.
// ctor is also private, so we should never be here.
//-----------------------------------------------------------------------------
PerfCounters::PerfCounters()
{
	_ASSERTE(false);
}

//-----------------------------------------------------------------------------
// Create or Open memory mapped files for IPC for both shared & private
//-----------------------------------------------------------------------------
HRESULT PerfCounters::Init() // static
{
    CONTRACTL
    {
        THROWS;     // GetSecurityAttributes can throw
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

// Should only be called once
	_ASSERTE(!m_fInit);
	_ASSERTE(g_pIPCManagerInterface != NULL);

    m_pPrivatePerf= g_pIPCManagerInterface->GetPerfBlock();

    // Set attributes
    if (m_pPrivatePerf != NULL)
    {
        memset (m_pPrivatePerf, 0, sizeof (PerfCounterIPCControlBlock));
        m_pPrivatePerf->m_cBytes = sizeof(PerfCounterIPCControlBlock);
        m_pPrivatePerf->m_wAttrs = PERF_ATTR_ON;
    }

    m_fInit = true;

	return NOERROR;
}

//-----------------------------------------------------------------------------
// Reset certain counters to 0 at closure because we could still have
// dangling references to us
//-----------------------------------------------------------------------------
void ResetCounters()
{
    LEAF_CONTRACT;
    
// Signify this block is no longer being updated
	GetPrivatePerfCounters().m_wAttrs &= ~PERF_ATTR_ON;

    for(int iGen = 0; iGen < MAX_TRACKED_GENS; iGen ++)
	{
		GetPrivatePerfCounters().m_GC.cGenHeapSize[iGen] = 0;
	}

	GetPrivatePerfCounters().m_GC.cLrgObjSize = 0;
}

//-----------------------------------------------------------------------------
// Shutdown - close handles
//-----------------------------------------------------------------------------
void PerfCounters::Terminate() // static
{
    LEAF_CONTRACT;
    

// Should be created first
	_ASSERTE(m_fInit);

// Reset counters to zero for dangling references
	ResetCounters();

	if (m_pPrivatePerf != &PerfCounters::m_garbage)
	{
		m_pPrivatePerf = &PerfCounters::m_garbage;
	}

	m_fInit = false;

}

Perf_Contexts * STDMETHODCALLTYPE GetPrivateContextsPerfCounters()
{
    LEAF_CONTRACT;
    return (Perf_Contexts *)((unsigned char *)PerfCounters::GetPrivatePerfCounterPtr() + offsetof (PerfCounterIPCControlBlock, m_Context));
}

#endif // ENABLE_PERF_COUNTERS
