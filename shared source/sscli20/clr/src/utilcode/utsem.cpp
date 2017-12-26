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
/******************************************************************************
    FILE : UTSEM.CPP

    Purpose: Part of the utilities library for the VIPER project

    Abstract : Implements the UTSemReadWrite class.
-------------------------------------------------------------------------------
Revision History:

*******************************************************************************/
#include "stdafx.h"
#include "clrhost.h"
#include "ex.h"

#include <utsem.h>
#include "contract.h"

//TODO: this undef should not be here. We need to move this code or the callers of this
//      code to the new synchronization api
//      This function are redefined at the end of this file
#undef InitializeCriticalSection
#undef DeleteCriticalSection
#undef EnterCriticalSection
#undef LeaveCriticalSection
#undef CreateSemaphore
#undef ReleaseSemaphore

#define IncCantStopCount(x)
#define DecCantStopCount(x)



/******************************************************************************
Definitions of the bit fields in UTSemReadWrite::m_dwFlag:

Warning: The code assume that READER_MASK is in the low-order bits of the DWORD.
******************************************************************************/

const ULONG READERS_MASK      = 0x000003FF;    // field that counts number of readers
const ULONG READERS_INCR      = 0x00000001;    // amount to add to increment number of readers

// The following field is 2 bits long to make it easier to catch errors.
// (If the number of writers ever exceeds 1, we've got problems.)
const ULONG WRITERS_MASK      = 0x00000C00;    // field that counts number of writers
const ULONG WRITERS_INCR      = 0x00000400;    // amount to add to increment number of writers

const ULONG READWAITERS_MASK  = 0x003FF000;    // field that counts number of threads waiting to read
const ULONG READWAITERS_INCR  = 0x00001000;    // amount to add to increment number of read waiters

const ULONG WRITEWAITERS_MASK = 0xFFC00000;    // field that counts number of threads waiting to write
const ULONG WRITEWAITERS_INCR = 0x00400000;    // amoun to add to increment number of write waiters

/******************************************************************************
Function : UTSemReadWrite::UTSemReadWrite

Abstract: Constructor.
******************************************************************************/
UTSemReadWrite::UTSemReadWrite (DWORD ulcSpinCount,
        LPCSTR szSemaphoreName, LPCSTR szEventName)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    static BOOL fInitialized = FALSE;
    static DWORD maskMultiProcessor;    // 0 => uniprocessor, all 1 bits => multiprocessor

    if (!fInitialized)
    {
        SYSTEM_INFO SysInfo;

        GetSystemInfo (&SysInfo);
        if (SysInfo.dwNumberOfProcessors > 1)
            maskMultiProcessor = 0xFFFFFFFF;
        else
            maskMultiProcessor = 0;

        fInitialized = TRUE;
    }


    m_ulcSpinCount = ulcSpinCount & maskMultiProcessor;
    m_dwFlag = 0;
    m_hReadWaiterSemaphore = NULL;
    m_hWriteWaiterEvent = NULL;
    m_szSemaphoreName = szSemaphoreName;
    m_szEventName = szEventName;
}


/******************************************************************************
Function : UTSemReadWrite::~UTSemReadWrite

Abstract: Destructor
******************************************************************************/
UTSemReadWrite::~UTSemReadWrite ()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    _ASSERTE (m_dwFlag == 0 && "Destroying a UTSemReadWrite while in use");

    if (m_hReadWaiterSemaphore != NULL)
        delete m_hReadWaiterSemaphore;

    if (m_hWriteWaiterEvent != NULL)
        delete m_hWriteWaiterEvent;
}


/******************************************************************************
Function : UTSemReadWrite::LockRead

Abstract: Obtain a shared lock
******************************************************************************/
HRESULT UTSemReadWrite::LockRead ()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    ULONG dwFlag;
    ULONG ulcLoopCount = 0;

    // Inform CLR that the debugger shouldn't suspend this thread while 
    // holding this lock.
    IncCantStopCount(StateHolderParamValue);

    for (;;)
    {
        dwFlag = m_dwFlag;

        if (dwFlag < READERS_MASK)
        {
            if (dwFlag == (ULONG) VipInterlockedCompareExchange (&m_dwFlag, dwFlag + READERS_INCR, dwFlag))
                break;
        }

        else if ((dwFlag & READERS_MASK) == READERS_MASK)
            ClrSleepEx(1000,FALSE);

        else if ((dwFlag & READWAITERS_MASK) == READWAITERS_MASK)
            ClrSleepEx(1000,FALSE);

        else if (ulcLoopCount++ < m_ulcSpinCount)
            /* nothing */ ;

        else
        {
            if (dwFlag == (ULONG) VipInterlockedCompareExchange (&m_dwFlag, dwFlag + READWAITERS_INCR, dwFlag))
            {
                Semaphore* s = GetReadWaiterSemaphore();
                if (s == NULL)
                {
                    // Remove the flag we just set.
                    do
                    {
                        dwFlag = m_dwFlag;
                    }
                    while(dwFlag != (ULONG)VipInterlockedCompareExchange(&m_dwFlag, dwFlag - READWAITERS_INCR, dwFlag));
                    return E_OUTOFMEMORY;
                }
                s->Wait(INFINITE, FALSE);
                break;
            }
        }
    }

    _ASSERTE ((m_dwFlag & READERS_MASK) != 0 && "reader count is zero after acquiring read lock");
    _ASSERTE ((m_dwFlag & WRITERS_MASK) == 0 && "writer count is nonzero after acquiring write lock");
    return S_OK;
}



/******************************************************************************
Function : UTSemReadWrite::LockWrite

Abstract: Obtain an exclusive lock
******************************************************************************/
HRESULT UTSemReadWrite::LockWrite ()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    ULONG dwFlag;
    ULONG ulcLoopCount = 0;

    // Inform CLR that the debugger shouldn't suspend this thread while 
    // holding this lock.
    IncCantStopCount(StateHolderParamValue);

    for (;;)
    {
        dwFlag = m_dwFlag;

        if (dwFlag == 0)
        {
            if (dwFlag == (ULONG) VipInterlockedCompareExchange (&m_dwFlag, WRITERS_INCR, dwFlag))
                break;
        }

        else if ((dwFlag & WRITEWAITERS_MASK) == WRITEWAITERS_MASK)
            ClrSleepEx(1000,FALSE);

        else if (ulcLoopCount++ < m_ulcSpinCount)
            /*nothing*/ ;

        else
        {
            if (dwFlag == (ULONG) VipInterlockedCompareExchange (&m_dwFlag, dwFlag + WRITEWAITERS_INCR, dwFlag))
            {
                Event* e = GetWriteWaiterEvent();
                if (e == NULL)
                {
                    // Remove the flag we just set.
                    do
                    {
                        dwFlag = m_dwFlag;
                    }
                    while(dwFlag != (ULONG) VipInterlockedCompareExchange(&m_dwFlag, dwFlag - WRITEWAITERS_INCR, dwFlag));
                    
                    return E_OUTOFMEMORY;
                }
                e->Wait(INFINITE,FALSE);
                break;
            }
        }

    }

    _ASSERTE ((m_dwFlag & READERS_MASK) == 0 && "reader count is nonzero after acquiring write lock");
    _ASSERTE ((m_dwFlag & WRITERS_MASK) == WRITERS_INCR && "writer count is not 1 after acquiring write lock");
    return S_OK;
}



/******************************************************************************
Function : UTSemReadWrite::UnlockRead

Abstract: Release a shared lock
******************************************************************************/
void UTSemReadWrite::UnlockRead ()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    ULONG dwFlag;


    _ASSERTE ((m_dwFlag & READERS_MASK) != 0 && "reader count is zero before releasing read lock");
    _ASSERTE ((m_dwFlag & WRITERS_MASK) == 0 && "writer count is nonzero before releasing read lock");

    for (;;)
    {
        dwFlag = m_dwFlag;

        if (dwFlag == READERS_INCR)
        {        // we're the last reader, and nobody is waiting
            if (dwFlag == (ULONG) VipInterlockedCompareExchange (&m_dwFlag, 0, dwFlag))
                break;
        }

        else if ((dwFlag & READERS_MASK) > READERS_INCR)
        {        // we're not the last reader
            if (dwFlag == (ULONG) VipInterlockedCompareExchange (&m_dwFlag, dwFlag - READERS_INCR, dwFlag))
                break;
        }

        else
        {
            // here, there should be exactly 1 reader (us), and at least one waiting writer.
            _ASSERTE ((dwFlag & READERS_MASK) == READERS_INCR && "UnlockRead consistency error 1");
            _ASSERTE ((dwFlag & WRITEWAITERS_MASK) != 0 && "UnlockRead consistency error 2");

            // one or more writers is waiting, do one of them next
            // (remove a reader (us), remove a write waiter, add a writer
            if (dwFlag == (ULONG) VipInterlockedCompareExchange (&m_dwFlag,
                    dwFlag - READERS_INCR - WRITEWAITERS_INCR + WRITERS_INCR, dwFlag))
            {
                // By this time, a WriteWaiter event should have been created. If it wasn't, something
                // strange happened and if GetWriteWaiterEvent() fails, we're better to AV than to
                // face a deadlock.
                GetWriteWaiterEvent()->Set();
                break;
            }
        }
    }

    DecCantStopCount(StateHolderParamValue);
}


/******************************************************************************
Function : UTSemReadWrite::UnlockWrite

Abstract: Release an exclusive lock
******************************************************************************/
void UTSemReadWrite::UnlockWrite ()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    ULONG dwFlag;
    ULONG count;


    _ASSERTE ((m_dwFlag & READERS_MASK) == 0 && "reader count is nonzero before releasing write lock");
    _ASSERTE ((m_dwFlag & WRITERS_MASK) == WRITERS_INCR && "writer count is not 1 before releasing write lock");


    for (;;)
    {
        dwFlag = m_dwFlag;

        if (dwFlag == WRITERS_INCR)
        {        // nobody is waiting
            if (dwFlag == (ULONG) VipInterlockedCompareExchange (&m_dwFlag, 0, dwFlag))
                break;
        }

        else if ((dwFlag & READWAITERS_MASK) != 0)
        {        // one or more readers are waiting, do them all next
            count = (dwFlag & READWAITERS_MASK) / READWAITERS_INCR;
            // remove a writer (us), remove all read waiters, turn them into readers
            if (dwFlag == (ULONG) VipInterlockedCompareExchange (&m_dwFlag,
                    dwFlag - WRITERS_INCR - count * READWAITERS_INCR + count * READERS_INCR, dwFlag))
            {
                GetReadWaiterSemaphore()->ReleaseSemaphore(count, NULL);
                break;
            }
        }

        else
        {        // one or more writers is waiting, do one of them next
            _ASSERTE ((dwFlag & WRITEWAITERS_MASK) != 0 && "UnlockWrite consistency error");
                // (remove a writer (us), remove a write waiter, add a writer
            if (dwFlag == (ULONG) VipInterlockedCompareExchange (&m_dwFlag, dwFlag - WRITEWAITERS_INCR, dwFlag))
            {
                GetWriteWaiterEvent()->Set();
                break;
            }
        }
    }

    DecCantStopCount(StateHolderParamValue);
}

/******************************************************************************
Function : UTSemReadWrite::GetReadWaiterSemaphore

Abstract: Return the semaphore to use for read waiters
******************************************************************************/
Semaphore* UTSemReadWrite::GetReadWaiterSemaphore()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    if (m_hReadWaiterSemaphore == NULL)
    {
        FAULT_NOT_FATAL();


        NewHolder<Semaphore> pSemaphore(new (nothrow) Semaphore());
        
        if (pSemaphore == NULL)
            return NULL;

        HRESULT hr = S_OK;
        EX_TRY
        {
            CONTRACT_VIOLATION(ThrowsViolation);
            pSemaphore->CreateSemaphore(0, MAXLONG);
        }
        EX_CATCH
        {
            hr = E_OUTOFMEMORY;
        }
        EX_END_CATCH(SwallowAllExceptions)

        if (FAILED(hr))
            return NULL;
        
        if (NULL == VipInterlockedCompareExchange ((PVOID*)&m_hReadWaiterSemaphore, (PVOID*)(pSemaphore.GetValue()), NULL))
            pSemaphore.SuppressRelease();
    }

    return m_hReadWaiterSemaphore;
}


/******************************************************************************
Function : UTSemReadWrite::GetWriteWaiterEvent

Abstract: Return the semaphore to use for write waiters
******************************************************************************/
Event* UTSemReadWrite::GetWriteWaiterEvent()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    
    if (m_hWriteWaiterEvent == NULL)
    {
        FAULT_NOT_FATAL();

        NewHolder<Event> pEvent(new (nothrow) Event());

        if (pEvent == NULL)
            return NULL;

        EX_TRY
        {
            CONTRACT_VIOLATION(ThrowsViolation);
            pEvent->CreateAutoEvent(FALSE);
        }
        EX_CATCH
        {
            hr = E_OUTOFMEMORY;
        }
        EX_END_CATCH(SwallowAllExceptions)

        if (FAILED(hr))
            return NULL;


        if (NULL == VipInterlockedCompareExchange ((PVOID*)&m_hWriteWaiterEvent, (PVOID)pEvent, NULL))
            pEvent.SuppressRelease();
    }

    return m_hWriteWaiterEvent;
}

#define InitializeCriticalSection   Dont_Use_InitializeCriticalSection
#define DeleteCriticalSection       Dont_Use_DeleteCriticalSection
#define EnterCriticalSection        Dont_Use_EnterCriticalSection
#define LeaveCriticalSection        Dont_Use_LeaveCriticalSection
#define CreateSemaphore             Dont_Use_CreateSemaphore
#define ReleaseSemaphore            Dont_Use_ReleaseSemaphore

