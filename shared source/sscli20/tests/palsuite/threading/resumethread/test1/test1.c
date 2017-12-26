/*============================================================
**
** Source: test1.c 
**
** Purpose: Test for ResumeThread.  Create a suspended Thread.
** First, ensure that it is indeed suspended.  Then call resumethread
** and check to ensure that the function has now run.
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
**=========================================================*/

#include <palsuite.h>

DWORD dwResumeThreadTestParameter = 0;

DWORD PALAPI ResumeThreadTestThread( LPVOID lpParameter)
{
    DWORD dwRet = 0;

    /* Save parameter so we can check and ensure this function ran
       properly.
    */
    
    dwResumeThreadTestParameter = (DWORD)lpParameter;

    return dwRet;
}

BOOL ResumeThreadTest()
{
    BOOL bRet = FALSE;
    DWORD dwRet = 0;

    LPSECURITY_ATTRIBUTES lpThreadAttributes = NULL;
    DWORD dwStackSize = 0; 
    LPTHREAD_START_ROUTINE lpStartAddress =  &ResumeThreadTestThread;
    LPVOID lpParameter = lpStartAddress;
    DWORD dwCreationFlags = CREATE_SUSPENDED;
    DWORD dwThreadId = 0;

    HANDLE hThread = 0;

    dwResumeThreadTestParameter = 0;

    /* Create a thread, with CREATE_SUSPENDED, so we can resume it! */

    hThread = CreateThread( lpThreadAttributes, 
                            dwStackSize, lpStartAddress, lpParameter, 
                            dwCreationFlags, &dwThreadId ); 
    
    if (hThread != INVALID_HANDLE_VALUE)
    {
        /* Wait for one second.  This should return WAIT_TIMEOUT */
        dwRet = WaitForSingleObject(hThread,1000);

        if (dwRet != WAIT_TIMEOUT)
        {
            Trace("ResumeThreadTest:WaitForSingleObject "
                   "failed (%x)\n",GetLastError());
        }
        else
        {
            /* Check to ensure the parameter hasn't changed.  The
               function shouldn't have occured yet.
            */
            if (dwResumeThreadTestParameter != 0)
            {
                Trace("ResumeThreadTest:parameter error\n");
            }
            else
            {
                /* Call ResumeThread and ensure the return value is
                   correct.
                */
                
                dwRet = ResumeThread(hThread);

                if (dwRet != 1)
                {
                    Trace("ResumeThreadTest:ResumeThread "
                           "failed (%x)\n",GetLastError());
                }
                else
                {
                    /* Wait again, now that the thread has been
                       resumed, and the return should be WAIT_OBJECT_0
                    */
                    dwRet = WaitForSingleObject(hThread,INFINITE);

                    if (dwRet != WAIT_OBJECT_0)
                    {
                        Trace("ResumeThreadTest:WaitForSingleObject "
                               "failed (%x)\n",GetLastError());
                    }
                    else
                    {
                        /* Check the param now and it should have been
                           set.
                        */
                        if (dwResumeThreadTestParameter != (DWORD)lpParameter)
                        {
                            Trace("ResumeThreadTest:parameter error\n");
                        }
                        else
                        {
                            bRet = TRUE;
                        }
                    }
                }
            }
        }
    }
    else
    {
        Trace("ResumeThreadTest:CreateThread failed (%x)\n",GetLastError());
    }

    return bRet; 
}

int __cdecl main(int argc, char **argv)
{

    if(0 != (PAL_Initialize(argc, argv)))
    {
	return ( FAIL );
    }

    if(!ResumeThreadTest())
    {
        Fail("Test Failed\n");
    }  

    PAL_Terminate();
    return (PASS);

}
