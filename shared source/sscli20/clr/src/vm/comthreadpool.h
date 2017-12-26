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
/*============================================================
**
** Header: COMThreadPool.h
**       
**
** Purpose: Native methods on System.ThreadPool
**          and its inner classes
**
** Date:  August, 1999
**
===========================================================*/

#ifndef _COMTHREADPOOL_H
#define _COMTHREADPOOL_H

#include "delegateinfo.h"

class ThreadPoolNative
{

public:

    static void Init();

    static FCDECL2(FC_BOOL_RET, CorSetMaxThreads, DWORD workerThreads, DWORD completionPortThreads);
    static FCDECL2(VOID, CorGetMaxThreads, DWORD* workerThreads, DWORD* completionPortThreads);
    static FCDECL2(FC_BOOL_RET, CorSetMinThreads, DWORD workerThreads, DWORD completionPortThreads);
    static FCDECL2(VOID, CorGetMinThreads, DWORD* workerThreads, DWORD* completionPortThreads);
    static FCDECL2(VOID, CorGetAvailableThreads, DWORD* workerThreads, DWORD* completionPortThreads);

    static FCDECL7(LPVOID, CorRegisterWaitForSingleObject,
                                Object* waitObjectUNSAFE,
                                Object* stateUNSAFE,
                                UINT32 timeout,
                                CLR_BOOL executeOnlyOnce,
                                Object* registeredWaitObjectUNSAFE,
                                StackCrawlMark* stackMark,
                                CLR_BOOL compressStack);

    static FCDECL3(FC_BOOL_RET, CorQueueUserWorkItem, Object* stateUNSAFE, StackCrawlMark* stackMark, CLR_BOOL compressStack);
    static FCDECL1(FC_BOOL_RET, CorPostQueuedCompletionStatus, LPOVERLAPPED lpOverlapped);    
    static FCDECL2(FC_BOOL_RET, CorUnregisterWait, LPVOID WaitHandle, Object * objectToNotify);
    static FCDECL1(void, CorWaitHandleCleanupNative, LPVOID WaitHandle);
    static FCDECL1(FC_BOOL_RET, CorBindIoCompletionCallback, HANDLE fileHandle);
};

class TimerBaseNative;
typedef TimerBaseNative* TIMERREF;

class TimerBaseNative : Object
{
    friend class TimerNative;
    friend class ThreadPoolNative;

    LPVOID          timerHandle;
    DelegateInfo*   delegateInfo;
    LONG            timerDeleted;

    __inline LPVOID         GetTimerHandle() { LEAF_CONTRACT; return timerHandle;}
    __inline void           SetTimerHandle(LPVOID handle) { LEAF_CONTRACT; timerHandle = handle;}
    __inline PLONG          GetAddressTimerDeleted() { LEAF_CONTRACT; return &timerDeleted;}
    __inline BOOL           IsTimerDeleted() { LEAF_CONTRACT; return timerDeleted;}

public:
    __inline DelegateInfo*  GetDelegateInfo() { LEAF_CONTRACT; return delegateInfo;}
    __inline void           SetDelegateInfo(DelegateInfo* delegate) { LEAF_CONTRACT; delegateInfo = delegate;}
};
class TimerNative
{
public:
    static FCDECL5(VOID, CorCreateTimer, TimerBaseNative* pThisUNSAFE, Object* stateUNSAFE, INT32 dueTime, INT32 period, StackCrawlMark* stackMark);
    static FCDECL3(FC_BOOL_RET, CorChangeTimer, TimerBaseNative* pThisUNSAFE, INT32 dueTime, INT32 period);
    static FCDECL2(FC_BOOL_RET, CorDeleteTimer, TimerBaseNative* pThisUNSAFE, Object* notifyObjectHandle);
};



#endif
