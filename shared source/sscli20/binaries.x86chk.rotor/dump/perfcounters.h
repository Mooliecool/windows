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
//-----------------------------------------------------------------------------
// PerfCounters.h
//
// Internal Interface for CLR to use Performance counters
//-----------------------------------------------------------------------------

#ifndef _PerfCounters_h_
#define _PerfCounters_h_

#include "perfcounterdefs.h"

#pragma pack()

#ifdef ENABLE_PERF_COUNTERS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// This code section active iff we're using Perf Counters
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PerfCounter class serves as namespace with data protection. 
// Enforce this by making constructor private
//-----------------------------------------------------------------------------
class PerfCounters
{
private:
	PerfCounters();

public:
	static HRESULT Init();
	static void Terminate();

    static PerfCounterIPCControlBlock * GetPrivatePerfCounterPtr();

private:
	static HANDLE m_hPrivateMapPerf;

	static PerfCounterIPCControlBlock * m_pPrivatePerf;

	static BOOL m_fInit;
	
// Set pointers to garbage so they're never null.
	static PerfCounterIPCControlBlock m_garbage;

    friend PerfCounterIPCControlBlock & GetPrivatePerfCounters();
};

//-----------------------------------------------------------------------------
// Utility functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Get the perf counters specific to our process
//-----------------------------------------------------------------------------
inline PerfCounterIPCControlBlock & GetPrivatePerfCounters()
{
    LEAF_CONTRACT;
    
    return *PerfCounters::m_pPrivatePerf;
}

inline PerfCounterIPCControlBlock *PerfCounters::GetPrivatePerfCounterPtr()
{
    LEAF_CONTRACT;

    return m_pPrivatePerf;
};

extern "C" Perf_Contexts * STDMETHODCALLTYPE GetPrivateContextsPerfCounters();

#define COUNTER_ONLY(x) x

#define PERF_COUNTER_NUM_OF_ITERATIONS 10

inline UINT64 GetCycleCount_UINT64()
{
    LEAF_CONTRACT;
    
    LARGE_INTEGER qwTmp;
    QueryPerformanceCounter(&qwTmp);
    return qwTmp.QuadPart;
}

#define PERF_COUNTER_TIMER_PRECISION UINT64
#define GET_CYCLE_COUNT GetCycleCount_UINT64

#define PERF_COUNTER_TIMER_START() \
PERF_COUNTER_TIMER_PRECISION _startPerfCounterTimer = GET_CYCLE_COUNT();

#define PERF_COUNTER_TIMER_STOP(global) \
global = (GET_CYCLE_COUNT() - _startPerfCounterTimer);




#else // ENABLE_PERF_COUNTERS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// This code section active iff we're NOT using Perf Counters
// Note, not even a class definition, so all usages of PerfCounters in client
// should be in #ifdef or COUNTER_ONLY(). 
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define COUNTER_ONLY(x)


#endif // ENABLE_PERF_COUNTERS


#endif // _PerfCounters_h_
