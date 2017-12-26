/*============================================================
**
** Source: test1.c 
**
** Purpose: Test for ResetEvent.  Create an event with an intial
** state signaled.  Then reset that signal, and check to see that 
** the event is now not signaled.
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

BOOL ResetEventTest()
{
    BOOL bRet = FALSE;
    DWORD dwRet = 0;

    LPSECURITY_ATTRIBUTES lpEventAttributes = 0;
    BOOL bManualReset = TRUE; 
    BOOL bInitialState = TRUE;
    LPCTSTR lpName = "Event #4";

    /* Create an Event, ensure it is valid */
    HANDLE hEvent = CreateEvent( lpEventAttributes, 
                                 bManualReset, bInitialState, lpName); 
    
    if (hEvent != INVALID_HANDLE_VALUE)
    {
        /* Check that WaitFor returns WAIT_OBJECT_0, indicating that
           the event is signaled.
        */
        
        dwRet = WaitForSingleObject(hEvent,0);

        if (dwRet != WAIT_OBJECT_0)
        {
            Fail("ResetEventTest:WaitForSingleObject %s "
                   "failed (%x)\n",lpName,GetLastError());
        }
        else
        {
            /* Call ResetEvent, which will reset the signal */
            bRet = ResetEvent(hEvent);

            if (!bRet)
            {
                Fail("ResetEventTest:ResetEvent %s failed "
                       "(%x)\n",lpName,GetLastError());
            }
            else
            {
                /* Call WaitFor again, and since it has been reset, 
                   the return value should now be WAIT_TIMEOUT 
                */
                dwRet = WaitForSingleObject(hEvent,0);

                if (dwRet != WAIT_TIMEOUT)
                {
                    Fail("ResetEventTest:WaitForSingleObject "
                           "%s failed (%x)\n",lpName,GetLastError());
                }
                else
                {
                    bRet = CloseHandle(hEvent);

                    if (!bRet)
                    {
                        Fail("ResetEventTest:CloseHandle %s failed"
                               "(%x)\n",lpName,GetLastError());
                    }
                }
            }
        }
    }
    else
    {
        Fail("ResetEventTest:CreateEvent %s failed "
               "(%x)\n",lpName,GetLastError());
    }
    
    return bRet;
}

int __cdecl main(int argc, char **argv)
{

    if(0 != (PAL_Initialize(argc, argv)))
    {
        return ( FAIL );
    }
    
    if(!ResetEventTest())
    {
        Fail ("Test failed\n");
    }
    
    PAL_Terminate();
    return ( PASS );

}
