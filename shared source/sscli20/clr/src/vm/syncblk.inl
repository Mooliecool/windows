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

#ifndef _SYNCBLK_INL_
#define _SYNCBLK_INL_

#ifndef DACCESS_COMPILE

// Helper encapsulating the core logic for releasing monitor. Returns what kind of 
// follow up action is necessary. This is FORCEINLINE to make it provide a very efficient implementation.
FORCEINLINE AwareLock::LeaveHelperAction AwareLock::LeaveHelper(Thread* pCurThread)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    if (m_HoldingThread != pCurThread)
        return AwareLock::LeaveHelperAction_Error;

    _ASSERTE((size_t)m_MonitorHeld & 1);
    _ASSERTE(m_Recursion >= 1);

#if defined(_DEBUG) && defined(TRACK_SYNC)
    // The best place to grab this is from the ECall frame
    Frame   *pFrame = pCurThread->GetFrame();
    int      caller = (pFrame && pFrame != FRAME_TOP ? (int) pFrame->GetReturnAddress() : -1);
    pCurThread->m_pTrackSync->LeaveSync(caller, this);
#endif

    if (--m_Recursion != 0)
    {
        return AwareLock::LeaveHelperAction_None;
    }

    m_HoldingThread->DecLockCount();
    m_HoldingThread = NULL;

    for (;;)
    {
        volatile LONG state = m_MonitorHeld;
        // Clear lock bit.
        if (FastInterlockCompareExchangeRelease((LONG*)&m_MonitorHeld, state - 1, state) == state)
        {
            // If wait count is non-zero on successful clear, we must signal the event.
            if (state & ~1)
            {
                return AwareLock::LeaveHelperAction_Signal;
            }
            break;
        }
    }

    return AwareLock::LeaveHelperAction_None;
}

// Helper encapsulating the core logic for releasing monitor. Returns what kind of 
// follow up action is necessary. This is FORCEINLINE to make it provide a very efficient implementation.
FORCEINLINE AwareLock::LeaveHelperAction ObjHeader::LeaveObjMonitorHelper(Thread* pCurThread)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    volatile DWORD syncBlockValue = m_SyncBlockValue;

    if (syncBlockValue & BIT_SBLK_SPIN_LOCK)
    {
        return AwareLock::LeaveHelperAction_Contention;
    }

    if (syncBlockValue & BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX)
    {
        if (syncBlockValue & BIT_SBLK_IS_HASHCODE)
        {
            // This thread does not own the lock.
            return AwareLock::LeaveHelperAction_Error;
        }

        SyncBlock *psb = g_pSyncTable [syncBlockValue & MASK_SYNCBLOCKINDEX].m_SyncBlock;
        _ASSERTE(psb != NULL);
        return psb->m_Monitor.LeaveHelper(pCurThread);
    }

    if ((syncBlockValue & SBLK_MASK_LOCK_THREADID) != pCurThread->GetThreadId())
    {
        // This thread does not own the lock.
        return AwareLock::LeaveHelperAction_Error;                
    }

    if (syncBlockValue & SBLK_MASK_LOCK_RECLEVEL)
    {
        // recursion and ThinLock
        DWORD newValue = syncBlockValue - SBLK_LOCK_RECLEVEL_INC;
        if (FastInterlockCompareExchangeRelease((LONG*)&m_SyncBlockValue, newValue, syncBlockValue) != (LONG)syncBlockValue)
        {
            return AwareLock::LeaveHelperAction_Yield;
        }
    }
    else
    {
        // We are leaving the lock
        DWORD newValue = (syncBlockValue & (~SBLK_MASK_LOCK_THREADID));
        if (FastInterlockCompareExchangeRelease((LONG*)&m_SyncBlockValue, newValue, syncBlockValue) != (LONG)syncBlockValue)
        {
            return AwareLock::LeaveHelperAction_Yield;
        }
        pCurThread->DecLockCount();
    }

    return AwareLock::LeaveHelperAction_None;
}

// Provide access to the object associated with this awarelock, so client can
// protect it.
inline OBJECTREF AwareLock::GetOwningObject()
{
    LEAF_CONTRACT;

    return (OBJECTREF) SyncTableEntry::GetSyncTableEntry()
                [(m_dwSyncIndex & ~SyncBlock::SyncBlockPrecious)].m_Object;
}

#endif // DACCESS_COMPILE

#endif  // _SYNCBLK_INL_
