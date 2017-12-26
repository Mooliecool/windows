/*=====================================================================
**
** Source:  test2.c
**
** Purpose: Tests that a child thread in the middle of a 
**          WaitForMultipleObjectsEx call will be interrupted by QueueUserAPC
**          if the alert flag was set.
**
** 
**  Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
** 
**  The use and distribution terms for this software are contained in the file
**  named license.txt, which can be found in the root of this distribution.
**  By using this software in any fashion, you are agreeing to be bound by the
**  terms of this license.
** 
**  You must not remove this notice, or any other, from this software.
** 
**
**===================================================================*/

#include <palsuite.h>

/* Based on SleepEx/test2 */

const int ChildThreadWaitTime = 4000;
const int InterruptTime = 2000; 
const int AcceptableDelta = 100;

void RunTest(BOOL AlertThread);
VOID PALAPI APCFunc(ULONG_PTR dwParam);
DWORD PALAPI WaiterProc(LPVOID lpParameter);

DWORD ThreadWaitDelta;

int __cdecl main( int argc, char **argv ) 
{
    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

  
    /* 
     * Check that Queueing an APC in the middle of a wait does interrupt
     * it, if it's in an alertable state.
     */

    RunTest(TRUE);
    if (abs(ThreadWaitDelta - InterruptTime) > AcceptableDelta)
    {
        Fail("Expected thread to wait for %d ms (and get interrupted).\n"
            "Thread waited for %d ms! (Acceptable delta: %d)\n", 
            InterruptTime, ThreadWaitDelta, AcceptableDelta);
    }

    /* 
     * Check that Queueing an APC in the middle of a wait does NOT interrupt 
     * it, if it is not in an alertable state.
     */
    RunTest(FALSE);
    if (abs(ThreadWaitDelta - ChildThreadWaitTime) > AcceptableDelta)
    {
        Fail("Expected thread to wait for %d ms (and not be interrupted).\n"
            "Thread waited for %d ms! (Acceptable delta: %d)\n", 
            ChildThreadWaitTime, ThreadWaitDelta, AcceptableDelta);
    }


    PAL_Terminate();
    return PASS;
}

void RunTest(BOOL AlertThread)
{
    HANDLE hThread = 0;
    DWORD dwThreadId = 0;
    int ret;

    hThread = CreateThread( NULL, 
                            0, 
                            (LPTHREAD_START_ROUTINE)WaiterProc,
                            (LPVOID) AlertThread,
                            0,
                            &dwThreadId);

    if (hThread == NULL)
    {
        Fail("ERROR: Was not able to create the thread to test!\n"
            "GetLastError returned %d\n", GetLastError());
    }

    Sleep(InterruptTime);

    ret = QueueUserAPC(APCFunc, hThread, 0);
    if (ret == 0)
    {
        Fail("QueueUserAPC failed! GetLastError returned %d\n", 
            GetLastError());
    }

    ret = WaitForSingleObject(hThread, INFINITE);
    if (ret == WAIT_FAILED)
    {
        Fail("Unable to wait on child thread!\nGetLastError returned %d.\n", 
            GetLastError());
    }
}

/* Function doesn't do anything, just needed to interrupt the wait*/
VOID PALAPI APCFunc(ULONG_PTR dwParam)
{    
}

/* Entry Point for child thread. */
DWORD PALAPI WaiterProc(LPVOID lpParameter)
{
    HANDLE Semaphore;
    DWORD OldTickCount;
    DWORD NewTickCount;
    BOOL Alertable;
    DWORD ret;

    /* Create a semaphore that is not in the signalled state */
    Semaphore = CreateSemaphoreW(NULL, 0, 1, NULL);

    if (Semaphore == NULL)
    {
        Fail("Failed to create semaphore!  GetLastError returned %d.\n",
            GetLastError());
    }

    Alertable = (BOOL) lpParameter;

    OldTickCount = GetTickCount();

    ret = WaitForMultipleObjectsEx(1, &Semaphore, FALSE, ChildThreadWaitTime, 
        Alertable);
    
    NewTickCount = GetTickCount();


    if (Alertable && ret != WAIT_IO_COMPLETION)
    {
        Fail("Expected the interrupted wait to return WAIT_IO_COMPLETION.\n"
            "Got %d\n", ret);
    }
    else if (!Alertable && ret != WAIT_TIMEOUT)
    {
        Fail("WaitForMultipleObjectsEx did not timeout.\n"
            "Expected return of WAIT_TIMEOUT, got %d.\n", ret);
    }


    /* 
    * Check for DWORD wraparound
    */
    if (OldTickCount>NewTickCount)
    {
        OldTickCount -= NewTickCount+1;
        NewTickCount  = 0xFFFFFFFF;
    }

    ThreadWaitDelta = NewTickCount - OldTickCount;

    ret = CloseHandle(Semaphore);
    if (!ret)
    {
        Fail("Unable to close handle to semaphore!\n"
            "GetLastError returned %d\n", GetLastError());
    }

    return 0;
}


