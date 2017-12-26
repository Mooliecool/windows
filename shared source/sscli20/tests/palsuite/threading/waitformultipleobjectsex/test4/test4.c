/*=====================================================================
**
** Source:  test4.c
**
** Purpose: Tests that waiting on an abandonded mutex will a return 
**          WAIT_ABANDONED_0.  Does this by creating a child thread that 
**          acquires the mutex and exits.
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


const int ChildThreadWaitTime = 1000;
const int ParentDelayTime = 2000; 

DWORD PALAPI AbandoningProc(LPVOID lpParameter);

int __cdecl main( int argc, char **argv ) 
{
    HANDLE Mutex;
    HANDLE hThread = 0;
    DWORD dwThreadId = 0;
    int ret;

    if (0 != (PAL_Initialize(argc, argv)))
    {
        return FAIL;
    }

    Mutex = CreateMutexW(NULL, FALSE, NULL);
    if (Mutex == NULL)
    {
        Fail("Unable to create the mutex.  GetLastError returned %d\n", 
            GetLastError());
    }

    hThread = CreateThread( NULL, 
                            0, 
                            (LPTHREAD_START_ROUTINE)AbandoningProc,
                            (LPVOID) Mutex,
                            0,
                            &dwThreadId);

    if (hThread == NULL)
    {
        Fail("ERROR: Was not able to create the thread to test!\n"
            "GetLastError returned %d\n", GetLastError());
    }

    Sleep(ParentDelayTime);

    ret = WaitForMultipleObjectsEx(1, &Mutex, FALSE, INFINITE, FALSE);
    if (ret != WAIT_ABANDONED_0)
    {
        Fail("Expected WaitForMultipleObjectsEx to return WAIT_ABANDONED_0\n"
            "Got %d\n", ret);
    }

    ReleaseMutex(Mutex);
    if (!CloseHandle(Mutex))
    {
        Fail("CloseHandle on the mutex failed!\n");
    }

    if (!CloseHandle(hThread))
    {
        Fail("CloseHandle on the thread failed!\n");
    }

    PAL_Terminate();
    return PASS;
}

/* 
 * Entry Point for child thread. Acquries a mutex and exit's without 
 * releasing it.
 */
DWORD PALAPI AbandoningProc(LPVOID lpParameter)
{
    HANDLE Mutex;
    DWORD ret;

    Mutex = (HANDLE) lpParameter;
    
    Sleep(ChildThreadWaitTime);

    ret = WaitForSingleObject(Mutex, 0);
    if (ret != WAIT_OBJECT_0)
    {
        Fail("Expected the WaitForSingleObject call on the mutex to succeed\n"
            "Expected return of WAIT_OBJECT_0, got %d\n", ret);
    }

    return 0;
}
