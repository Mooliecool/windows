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
// File: locks.cpp
//
// ===========================================================================

#include "pch.h"
#include "locks.h"

LONG    CTinyLock::m_iSpins = 0;
LONG    CTinyLock::m_iSleeps = 0;
LONG    CTinyLock::m_iSpinStart = -1;

DWORD   CLockBase::m_dwSlot = 0xffffffff;

////////////////////////////////////////////////////////////////////////////
// CLockBase::GetLockState

CLockState *CLockBase::GetLockState ()
{
    CLockState  *pState = (CLockState *)TlsGetValue (m_dwSlot);
    if (pState == NULL)
    {
        pState = new CLockState();
        if (!TlsSetValue (m_dwSlot, pState))
        {
            delete pState;
            pState = NULL;
        }
    }

    return pState;
}

////////////////////////////////////////////////////////////////////////////
// CLockBase::FreeLockState

void CLockBase::FreeLockState ()
{
    if (m_dwSlot != 0xffffffff)
    {
        CLockState  *pState = (CLockState *)TlsGetValue (m_dwSlot);
        if (pState != NULL)
        {
            delete pState;
            TlsSetValue (m_dwSlot, NULL);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// CLockBase::DllMain
//
// This is where the TLS slot is allocated, and per-thread lock state is freed.

void CLockBase::DllMain (DWORD dwReason)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            // Process creation -- allocate our TLS slot
            ASSERT (m_dwSlot == 0xffffffff);
            m_dwSlot = TlsAlloc();
            ASSERT (m_dwSlot != 0xffffffff);
            break;

        case DLL_PROCESS_DETACH:
            // We're done with our TLS slot.  Free this thread's lock state and release the slot
            FreeLockState ();
            ASSERT (m_dwSlot != 0xffffffff);
            TlsFree (m_dwSlot);
            m_dwSlot = 0xffffffff;
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            // Thread goes by-by...
            FreeLockState ();
            break;
    }
}

////////////////////////////////////////////////////////////////////////////
// CLockBase::AcquireLock -- acquire lock for read or write

LOCKKEY CLockBase::AcquireLock (bool fWrite)
{
    CLockState  *pState = GetLockState();

    if (pState == NULL)
    {
        VSFAIL ("LOCK STATE FAILURE!");
        return ACQUIRED_NOTHING;
    }

    bool        fIsLocked = pState->IsLocked (this);

    // Shortcut
    if (fIsLocked && !fWrite)
        return ACQUIRED_NOTHING;

    // Must acquire lock busy state
GetLockBusy:
    while (InterlockedExchange (&m_iLock, 1) != 0)
        Snooze ();

    if (fWrite)
    {
        if (fIsLocked)
        {
            if (m_fWrite)
            {
                // We're already writing...
                m_iLock = 0;
                return ACQUIRED_NOTHING;
            }

            if (m_iReadThreads == 1)
            {
                // We're reading alone, so we can be upgraded to writer...
                m_fWrite = true;
                m_iLock = 0;
                return ACQUIRED_WRITE;
            }

            ASSERT (m_iReadThreads > 1);

            // Readers exist.  Release lock busy flag, wait until only one reader (us), and try again
            m_iLock = 0;
            while (m_iReadThreads > 1)
                Snooze ();

            goto GetLockBusy;
        }

        if (!m_fWrite && (m_iReadThreads == 0))
        {
            // Lock is available for write.  A write lock is also a read lock.
            m_fWrite = true;
            m_iReadThreads = 1;
            pState->Lock (this);
            m_iLock = 0;
            return ACQUIRED_READWRITE;
        }

        // Lock is unavailable for write -- release lock busy flag and try again
        m_iLock = 0;
        Snooze ();
        goto GetLockBusy;
    }

    if (fIsLocked)
        return ACQUIRED_NOTHING;

    if (m_fWrite)
    {
        // Lock is unavailable for read -- release lock busy flag and try again
        m_iLock = 0;
        Snooze ();
        goto GetLockBusy;
    }

    // Lock is available for read.
    m_iReadThreads++;
    pState->Lock (this);
    m_iLock = 0;
    return ACQUIRED_READ;
}

////////////////////////////////////////////////////////////////////////////
// CLockBase::ReleaseLock

void CLockBase::ReleaseLock (LOCKKEY key)
{
    if (key == ACQUIRED_NOTHING)
        return;

    // Get lock busy flag
    while (InterlockedExchange (&m_iLock, 1) != 0)
        Snooze ();

    // We're no longer a reader in these cases...
    if (key == ACQUIRED_READ || key == ACQUIRED_READWRITE)
    {
        m_iReadThreads--;
        CLockState  *pState = GetLockState();
        if (pState != NULL)
            pState->Unlock (this);
    }

    // We're no longer a writer in these cases...
    if (key == ACQUIRED_WRITE || key == ACQUIRED_READWRITE)
        m_fWrite = false;

    // Give back the lock busy flag and we're done
    m_iLock = 0;
}


