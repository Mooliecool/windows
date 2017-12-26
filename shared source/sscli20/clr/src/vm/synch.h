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

#ifndef __Synch_h__
#define __Synch_h__

enum WaitMode
{
    WaitMode_None =0x0,
    WaitMode_Alertable = 0x1,         // Can be waken by APC.  May pumping message.
    WaitMode_IgnoreSyncCtx = 0x2,     // Dispatch to synchronization context if existed.
    WaitMode_ADUnload = 0x4,          // The block is to wait for AD unload start.  If it is interrupted by AD Unload, we can start aborting.
    WaitMode_InDeadlock = 0x8,        // The wait can be terminated by host's deadlock detection
};


struct PendingSync;
class CRWLock;

class CLREvent
{
public:
    CLREvent()
    {
        LEAF_CONTRACT;
        m_handle = INVALID_HANDLE_VALUE;
        m_dwFlags = 0;
    }

#ifndef DACCESS_COMPILE
    ~CLREvent()
    {
        WRAPPER_CONTRACT;
        CloseEvent ();
    }
#endif

    // Create an Event that is host aware
    void CreateAutoEvent(BOOL bInitialState);
    void CreateManualEvent(BOOL bInitialState);

    void CreateMonitorEvent(SIZE_T Cookie); // robust against initialization races - for exclusive use by AwareLock

    void CreateRWLockReaderEvent(BOOL bInitialState, CRWLock* pRWLock);
    void CreateRWLockWriterEvent(BOOL bInitialState, CRWLock* pRWLock);

    // Create an Event that is not host aware
    void CreateOSAutoEvent (BOOL bInitialState);
    void CreateOSManualEvent (BOOL bInitialState);

    void CloseEvent();

    BOOL IsValid() const
    {
        LEAF_CONTRACT;
        return m_handle != INVALID_HANDLE_VALUE;
    }

    BOOL IsMonitorEventAllocated()
    {
        LEAF_CONTRACT;
        return m_dwFlags & CLREVENT_FLAGS_MONITOREVENT_ALLOCATED;
    }

#ifndef DACCESS_COMPILE
	HANDLE GetHandleUNHOSTED() {
        LEAF_CONTRACT;
        _ASSERTE (IsOSEvent() || !CLRSyncHosted());
		return m_handle;
	}
#endif // DACCESS_COMPILE

    BOOL Set();
    void SetMonitorEvent(); // robust against races - for exclusive use by AwareLock
    BOOL Reset();
    DWORD Wait(DWORD dwMilliseconds, BOOL bAlertable, PendingSync *syncState=NULL);
    DWORD WaitEx(DWORD dwMilliseconds, WaitMode mode, PendingSync *syncState=NULL);

private:
    void    RecycleHandle();
    HANDLE  GetRecycledHandle();        
    const static  unsigned CACHE_SIZE = 40;  
    static HANDLE m_cacheOSAutoEvent[CACHE_SIZE];
    static HANDLE m_cacheOSManualEvent[CACHE_SIZE];

    HANDLE m_handle;

    enum
    {
        CLREVENT_FLAGS_AUTO_EVENT = 0x0001,
        CLREVENT_FLAGS_OS_EVENT = 0x0002,
        CLREVENT_FLAGS_IN_DEADLOCK_DETECTION = 0x0004,

        CLREVENT_FLAGS_MONITOREVENT_ALLOCATED = 0x0008,
        CLREVENT_FLAGS_MONITOREVENT_SIGNALLED = 0x0010,

        // Several bits unused;
    };
    
    volatile DWORD m_dwFlags;

    BOOL IsAutoEvent() { LEAF_CONTRACT; return m_dwFlags & CLREVENT_FLAGS_AUTO_EVENT; }
    void SetAutoEvent ()
    {
        LEAF_CONTRACT;
        // cannot use `|=' operator on `volatile DWORD'
        m_dwFlags = m_dwFlags | CLREVENT_FLAGS_AUTO_EVENT;
    }
    BOOL IsOSEvent() { LEAF_CONTRACT; return m_dwFlags & CLREVENT_FLAGS_OS_EVENT; }
    void SetOSEvent ()
    {
        LEAF_CONTRACT;
        // cannot use `|=' operator on `volatile DWORD'
        m_dwFlags = m_dwFlags | CLREVENT_FLAGS_OS_EVENT;
    }
    BOOL IsInDeadlockDetection() { LEAF_CONTRACT; return m_dwFlags & CLREVENT_FLAGS_IN_DEADLOCK_DETECTION; }
    void SetInDeadlockDetection ()
    {
        LEAF_CONTRACT;
        // cannot use `|=' operator on `volatile DWORD'
        m_dwFlags = m_dwFlags | CLREVENT_FLAGS_IN_DEADLOCK_DETECTION;
    }

};

class CLRSemaphore {
public:
    CLRSemaphore()
    : m_handle(INVALID_HANDLE_VALUE)
    {
        LEAF_CONTRACT;
    }
    
    ~CLRSemaphore()
    {
        WRAPPER_CONTRACT;
        Close ();
    }

    void Create(DWORD dwInitial, DWORD dwMax);
    void Close();

    BOOL IsValid() const
    {
        LEAF_CONTRACT;
        return m_handle != INVALID_HANDLE_VALUE;
    }

    DWORD Wait(DWORD dwMilliseconds, BOOL bAlertable);
    BOOL Release(LONG lReleaseCount, LONG* lpPreviouseCount);

private:
    HANDLE m_handle;
};

class CLRMutex {
public:
    CLRMutex()
    : m_handle(INVALID_HANDLE_VALUE)
    {
        LEAF_CONTRACT;
    }
    
    ~CLRMutex()
    {
        WRAPPER_CONTRACT;
        Close ();
    }

    void Create(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCTSTR lpName);
    void Close();

    BOOL IsValid() const
    {
        LEAF_CONTRACT;
        return m_handle != INVALID_HANDLE_VALUE;
    }

    DWORD Wait(DWORD dwMilliseconds, BOOL bAlertable);
    BOOL Release();

private:
    HANDLE m_handle;
};

BOOL CLREventWaitWithTry(CLREvent *pEvent, DWORD timeout, BOOL fAlertable, DWORD *pStatus);
#endif
