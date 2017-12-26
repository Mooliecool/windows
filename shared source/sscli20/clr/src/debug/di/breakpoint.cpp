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
// File: breakpoint.cpp
//
//*****************************************************************************
#include "stdafx.h"

/* ------------------------------------------------------------------------- *
 * Breakpoint class
 * ------------------------------------------------------------------------- */

CordbBreakpoint::CordbBreakpoint(CordbProcess * pProcess, CordbBreakpointType bpType)
  : CordbBase(pProcess, 0, enumCordbBreakpoint), 
  m_active(false), m_type(bpType)
{
}

// Neutered by CordbAppDomain
void CordbBreakpoint::Neuter(NeuterTicket ticket)
{
    AddRef();
    {
        m_pAppDomain = NULL; // clear ref
        CordbBase::Neuter(ticket);
    }
    Release();
}

HRESULT CordbBreakpoint::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugBreakpoint)
        *pInterface = static_cast<ICorDebugBreakpoint*>(this);
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown *>(static_cast<ICorDebugBreakpoint*>(this));
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

HRESULT CordbBreakpoint::BaseIsActive(BOOL *pbActive)
{
    *pbActive = m_active ? TRUE : FALSE;

    return S_OK;
}

/* ------------------------------------------------------------------------- *
 * Function Breakpoint class
 * ------------------------------------------------------------------------- */

CordbFunctionBreakpoint::CordbFunctionBreakpoint(CordbCode *code,
                                                 SIZE_T offset)
  : CordbBreakpoint(code->GetProcess(), CBT_FUNCTION), 
  m_code(code), m_offset(offset)
{
    // Remember the app domain we came from so that breakpoints can be
    // deactivated from within the ExitAppdomain callback.
    m_pAppDomain = m_code->GetAppDomain();
    _ASSERTE(m_pAppDomain != NULL);
}

CordbFunctionBreakpoint::~CordbFunctionBreakpoint()
{
}

void CordbFunctionBreakpoint::Neuter(NeuterTicket ticket)
{
    Disconnect();
    CordbBreakpoint::Neuter(ticket);
}

HRESULT CordbFunctionBreakpoint::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugFunctionBreakpoint)
    {
        *pInterface = static_cast<ICorDebugFunctionBreakpoint*>(this);
    }
    else
    {
        // Not looking for a function breakpoint? See if the base class handles
        // this interface. (bug 143976)
        return CordbBreakpoint::QueryInterface(id, pInterface);
    }

    AddRef();
    return S_OK;
}

HRESULT CordbFunctionBreakpoint::GetFunction(ICorDebugFunction **ppFunction)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(ppFunction, ICorDebugFunction **);

    if (m_code == NULL)
    {
        return CORDBG_E_PROCESS_TERMINATED;
    }        

    *ppFunction = static_cast<ICorDebugFunction*> (m_code->m_function);
    (*ppFunction)->AddRef();

    return S_OK;
}

// m_id is actually a LSPTR_BREAKPOINT. Get it as a type-safe member.
LSPTR_BREAKPOINT CordbFunctionBreakpoint::GetLsPtrBP()
{
    LSPTR_BREAKPOINT p;
    p.Set((void*) m_id);
    return p;
}

HRESULT CordbFunctionBreakpoint::GetOffset(ULONG32 *pnOffset)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pnOffset, SIZE_T *);
    
    *pnOffset = (ULONG32)m_offset;

    return S_OK;
}

HRESULT CordbFunctionBreakpoint::Activate(BOOL bActive)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    OK_IF_NEUTERED(this); // we'll check again later
    if (bActive == (m_active == true) )
        return S_OK;

    if (m_code == NULL || this->IsNeutered())
        return CORDBG_E_PROCESS_TERMINATED;

    HRESULT hr;
    ATT_ALLOW_LIVE_DO_STOPGO(GetProcess());

    // For legacy, check this error condition. We must do this under the stop-go lock to ensure
    // that the m_code object was not deleted out from underneath us.
    if (m_code->IsNeutered())
    {
        return CORDBG_E_CODE_NOT_AVAILABLE;
    }

        
    //
    //
    CordbProcess *process = GetProcess();

    process->Lock();
    process->ClearPatchTable(); // if we add something, then the right side 
                                // view of the patch table is no longer valid
    process->Unlock();

    DebuggerIPCEvent *event = (DebuggerIPCEvent *) _alloca(CorDBIPC_BUFFER_SIZE);

    CordbAppDomain *pAppDomain = GetAppDomain();
    _ASSERTE (pAppDomain != NULL);
    if (bActive)
    {
        process->InitIPCEvent(event, 
                              DB_IPCE_BREAKPOINT_ADD, 
                              true,
                              pAppDomain->GetADToken());
        event->BreakpointData.funcMetadataToken = m_code->m_function->m_token;
        event->BreakpointData.funcDebuggerModuleToken = m_code->m_function->m_module->m_debuggerModuleToken;
        event->BreakpointData.encVersion = m_code->m_nVersion;

        BOOL isIL = m_code->IsIL();

        event->BreakpointData.isIL = isIL ? true : false;
        event->BreakpointData.offset = m_offset;
        event->BreakpointData.nativeCodeMethodDescToken = m_code->m_nativeCodeMethodDescToken;
        event->BreakpointData.nativeCodeJITInfoToken = m_code->m_nativeCodeJITInfoToken;

        // Note: we're sending a two-way event, so it blocks here
        // until the breakpoint is really added and the reply event is
        // copied over the event we sent.
        hr = process->SendIPCEvent(event, CorDBIPC_BUFFER_SIZE);

        hr = WORST_HR(hr, event->hr);
        if (FAILED(hr))
            return hr;

            
        m_id = LsPtrToCookie(event->BreakpointData.breakpointToken);

        // If we weren't able to allocate the BP, we should have set the
        // hr on the left side.
        _ASSERTE(m_id != 0);

        process->Lock();

        pAppDomain->m_breakpoints.AddBase(this);
        m_active = true;

        process->Unlock();

        // Continue called automatically by StopContinueHolder
    }
    else
    {
        _ASSERTE (pAppDomain != NULL);

        if (process->IsSafeToSendEvents() && (pAppDomain->m_fAttached == TRUE))
        {            
            process->InitIPCEvent(event, 
                                  DB_IPCE_BREAKPOINT_REMOVE, 
                                  false,
                                  pAppDomain->GetADToken());
            event->BreakpointData.breakpointToken = GetLsPtrBP(); 

            hr = process->SendIPCEvent(event, CorDBIPC_BUFFER_SIZE);            
            hr = WORST_HR(hr, event->hr);
        }
        else
        {
            hr = CORDBHRFromProcessState(process, pAppDomain);
        }            
        
        process->Lock();


        pAppDomain->m_breakpoints.RemoveBase(LsPtrToCookie(GetLsPtrBP()));
        m_active = false;


        process->Unlock();
    
    }

    return hr;
}

void CordbFunctionBreakpoint::Disconnect()
{
    m_code.Clear();
}

/* ------------------------------------------------------------------------- *
 * Stepper class
 * ------------------------------------------------------------------------- */

CordbStepper::CordbStepper(CordbThread *thread, CordbFrame *frame)
  : CordbBase(thread->GetProcess(), 0, enumCordbStepper), 
    m_thread(thread), m_frame(frame),
    m_stepperToken(0), m_active(false),
    m_rangeIL(TRUE),
    m_fIsJMCStepper(false),
    m_rgfMappingStop(STOP_OTHER_UNMAPPED),
    m_rgfInterceptStop(INTERCEPT_NONE)
{
}

HRESULT CordbStepper::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugStepper)
        *pInterface = static_cast<ICorDebugStepper *>(this);
    else if (id == IID_ICorDebugStepper2)
        *pInterface = static_cast<ICorDebugStepper2 *>(this);
    else if (id == IID_IUnknown)
        *pInterface = static_cast<IUnknown *>(static_cast<ICorDebugStepper *>(this));
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

HRESULT CordbStepper::SetRangeIL(BOOL bIL)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    m_rangeIL = (bIL != FALSE);

    return S_OK;
}

HRESULT CordbStepper::SetJMC(BOOL fIsJMCStepper)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    // Can't have JMC and stopping with anything else.
    if (m_rgfMappingStop & STOP_ALL)
        return E_INVALIDARG;
            
    m_fIsJMCStepper = (fIsJMCStepper != FALSE);
    return S_OK;
}

HRESULT CordbStepper::IsActive(BOOL *pbActive)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pbActive, BOOL *);
    
    *pbActive = m_active;

    return S_OK;
}

// M_id is a ptr to the stepper in the LS process.
LSPTR_STEPPER CordbStepper::GetLsPtrStepper()
{
    LSPTR_STEPPER p;
    p.Set((void*) m_id);
    return p;
}

HRESULT CordbStepper::Deactivate()
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    if (!m_active)
        return S_OK;
        
    FAIL_IF_NEUTERED(this);

    if (m_thread == NULL)
        return CORDBG_E_PROCESS_TERMINATED;

    HRESULT hr;
    CordbProcess *process = GetProcess();
    ATT_ALLOW_LIVE_DO_STOPGO(process);
    
    process->Lock();

    if (!m_active) // another thread may be deactivating (e.g. step complete event)
    {
        process->Unlock();
        return S_OK;
    }

    CordbAppDomain *pAppDomain = GetAppDomain();
    _ASSERTE (pAppDomain != NULL);

    DebuggerIPCEvent event;
    process->InitIPCEvent(&event, 
                          DB_IPCE_STEP_CANCEL, 
                          false,
                          pAppDomain->GetADToken());

    event.StepData.stepperToken = GetLsPtrStepper(); 

    process->Unlock();
    hr = process->SendIPCEvent(&event, sizeof(DebuggerIPCEvent));
    hr = WORST_HR(hr, event.hr);
    process->Lock();


    process->m_steppers.RemoveBase((ULONG_PTR)m_id);
    m_active = false;

    process->Unlock();

    return hr;
}

HRESULT CordbStepper::SetInterceptMask(CorDebugIntercept mask)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    m_rgfInterceptStop = mask;
    return S_OK;
}

HRESULT CordbStepper::SetUnmappedStopMask(CorDebugUnmappedStop mask)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    
    // You must be Win32 attached to stop in unmanaged code.
    if ((mask & STOP_UNMANAGED) &&
        !(GetProcess()->m_state & CordbProcess::PS_WIN32_ATTACHED))
        return E_INVALIDARG;

    // Limitations on JMC Stepping - if JMC stepping is active,
    // all other stop masks must be disabled.
    // The jit can't place JMC probes before the prolog, so if we're 
    // we're JMC stepping, we'll stop after the prolog. 
    // The implementation for JMC stepping also doesn't let us stop in
    // unmanaged code. (because there are no probes there).
    // So enforce those implementation limitations here.
    if (m_fIsJMCStepper)
    {
        if (mask & STOP_ALL)
            return E_INVALIDARG;
    }

    
    
    m_rgfMappingStop = mask;
    return S_OK;
}

HRESULT CordbStepper::Step(BOOL bStepIn)
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
    
    if (m_thread == NULL)
        return CORDBG_E_PROCESS_TERMINATED;

    return StepRange(bStepIn, NULL, 0);
}

HRESULT CordbStepper::StepRange(BOOL bStepIn, 
                                COR_DEBUG_STEP_RANGE ranges[], 
                                ULONG32 cRangeCount)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY_OR_NULL(ranges,COR_DEBUG_STEP_RANGE, 
                                   cRangeCount, true, true);

    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    if (m_thread == NULL)
        return CORDBG_E_PROCESS_TERMINATED;

    if (m_active)
    {
        //
        // Deactivate the current stepping. 
        // or return an error???
        //

        HRESULT hr = Deactivate();

        if (FAILED(hr))
            return hr;
    }

    // Validate step-ranges. Ranges are exclusive, so end offset
    // should always be greater than start offset.
    // Ranges don't have to be sorted.
    // Zero ranges is ok; though they ought to just call Step() in that case.
    for(ULONG32 i = 0; i < cRangeCount; i++)
    {
        if (ranges[i].startOffset >= ranges[i].endOffset)
        {
            STRESS_LOG2(LF_CORDB, LL_INFO10, "Illegal step range. 0x%x-0x%x\n", ranges[i].startOffset, ranges[i].endOffset);
            return ErrWrapper(E_INVALIDARG);
        }
    }
    
    CordbProcess *process = GetProcess();
    
    //
    // Build step event
    //

    DebuggerIPCEvent *event = 
      (DebuggerIPCEvent *) _alloca(CorDBIPC_BUFFER_SIZE);

    process->InitIPCEvent(event, 
                          DB_IPCE_STEP, 
                          true,
                          GetAppDomain()->GetADToken());

    event->StepData.threadToken = m_thread->m_debuggerThreadToken;
    event->StepData.rgfMappingStop = m_rgfMappingStop;
    event->StepData.rgfInterceptStop = m_rgfInterceptStop;
    event->StepData.IsJMCStop = !!m_fIsJMCStepper;

        
    if (m_frame == NULL)
        event->StepData.frameToken = LEAF_MOST_FRAME;
    else
        event->StepData.frameToken = m_frame->GetFramePointer();

    event->StepData.stepIn = bStepIn != 0;

    event->StepData.totalRangeCount = cRangeCount;
    event->StepData.rangeIL = m_rangeIL;

    //
    // Send ranges.  We may have to send > 1 message.
    //

    COR_DEBUG_STEP_RANGE *rStart = &event->StepData.range;
    COR_DEBUG_STEP_RANGE *rEnd = ((COR_DEBUG_STEP_RANGE *) 
                                  (((BYTE *)event) + 
                                   CorDBIPC_BUFFER_SIZE)) - 1;
    int n = cRangeCount;
    if (n > 0)
    {
        while (n > 0)
        {
            COR_DEBUG_STEP_RANGE *r = rStart;

            if (n < rEnd - r)
                rEnd = r + n;

            while (r < rEnd)
                *r++ = *ranges++;

            n -= event->StepData.rangeCount = (unsigned int)(r - rStart);

            //
            // Send step event (two-way event here...)
            //

            HRESULT hr = process->SendIPCEvent(event,
                                               CorDBIPC_BUFFER_SIZE);
            hr = WORST_HR(hr, event->hr);
            if (FAILED(hr))
                return hr;
        }
    }
    else
    {
        //
        // Send step event without any ranges (two-way event here...)
        //

        HRESULT hr = process->SendIPCEvent(event,
                                           CorDBIPC_BUFFER_SIZE);
        hr = WORST_HR(hr, event->hr);
        if (FAILED(hr))
            return hr;
    }

    m_id = LsPtrToCookie(event->StepData.stepperToken);

    LOG((LF_CORDB,LL_INFO10000, "CS::SR: m_id:0x%x | 0x%x \n", m_id, 
        LsPtrToCookie(event->StepData.stepperToken)));

#ifdef _DEBUG
    CordbAppDomain *pAppDomain = GetAppDomain();
#endif
    _ASSERTE (pAppDomain != NULL);

    process->Lock();

    process->m_steppers.AddBase(this);
    m_active = true;

    process->Unlock();

    return S_OK;
}

HRESULT CordbStepper::StepOut()
{
    PUBLIC_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());
        
    if (m_thread == NULL)
        return CORDBG_E_PROCESS_TERMINATED;

    if (m_active)
    {
        //
        // Deactivate the current stepping. 
        // or return an error???
        //

        HRESULT hr = Deactivate();

        if (FAILED(hr))
            return hr;
    }

    CordbProcess *process = GetProcess();

    // We don't do native step-out.
    if (process->SupportsVersion(ver_ICorDebugProcess2))
    {
        if ((m_rgfMappingStop & STOP_UNMANAGED) != 0)
        {
            return ErrWrapper(CORDBG_E_CANT_INTEROP_STEP_OUT);
        }
    }
    
    //
    // Build step event
    //

    DebuggerIPCEvent *event = 
      (DebuggerIPCEvent *) _alloca(CorDBIPC_BUFFER_SIZE);

    process->InitIPCEvent(event, 
                          DB_IPCE_STEP_OUT, 
                          true,
                          GetAppDomain()->GetADToken());

    event->StepData.threadToken = m_thread->m_debuggerThreadToken;
    event->StepData.rgfMappingStop = m_rgfMappingStop;
    event->StepData.rgfInterceptStop = m_rgfInterceptStop;
    event->StepData.IsJMCStop = !!m_fIsJMCStepper;

    if (m_frame == NULL)
        event->StepData.frameToken = LEAF_MOST_FRAME;
    else
        event->StepData.frameToken = m_frame->GetFramePointer();

    event->StepData.totalRangeCount = 0;

    // Note: two-way event here...
    HRESULT hr = process->SendIPCEvent(event, CorDBIPC_BUFFER_SIZE);
    hr = WORST_HR(hr, event->hr);
    if (FAILED(hr))
        return hr;

    m_id = LsPtrToCookie(event->StepData.stepperToken);

#ifdef _DEBUG
    CordbAppDomain *pAppDomain = GetAppDomain();
#endif
    _ASSERTE (pAppDomain != NULL);

    process->Lock();

    process->m_steppers.AddBase(this);
    m_active = true;

    process->Unlock();
    
    return S_OK;
}

void CordbStepper::Disconnect()
{
    m_thread = NULL;
}


