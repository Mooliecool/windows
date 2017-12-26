/*============================================================
**
** Source: test2.c 
**
** Purpose: Test for CreateEvent.  Create the event with the
** initial state being not signaled.  Check to ensure that it
** times out when the event is triggered.
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

BOOL CreateEventTest()
{
    BOOL bRet = FALSE;
    DWORD dwRet = 0;

    LPSECURITY_ATTRIBUTES lpEventAttributes = 0;
    BOOL bManualReset = TRUE; 
    BOOL bInitialState = FALSE;
    LPCTSTR lpName = "Event #2";

    /* Create an event with the Initial State set to FALSE */

    HANDLE hEvent = CreateEvent( lpEventAttributes, 
                                 bManualReset, 
                                 bInitialState, 
                                 lpName); 
 
    if (hEvent != NULL)
    {
        /* This should ensure that the object is reset, or
           non-signaled.
        */
        
        dwRet = WaitForSingleObject(hEvent,0);

        if (dwRet != WAIT_TIMEOUT)
        {
            Trace("CloseEventTest:WaitForSingleObject %s "
                   "failed (%x)\n",lpName,GetLastError());
        }
        else
        {
            /* At this point, we've tested the function with success.
               So long as the HANDLE can be closed, this test should
               pass.
            */
            
            bRet = CloseHandle(hEvent);

            if (!bRet)
            {
                Trace("CloseEventTest:CloseHandle %s failed "
                       " (%x)\n",lpName,GetLastError());
            }
        }
    }
    else
    {
        Trace("CloseEventTest:CreateEvent %s "
               "failed (%x)\n",lpName,GetLastError());
    }
    
    return bRet;
}

int __cdecl main(int argc, char **argv)
{
    if(0 != (PAL_Initialize(argc, argv)))
    {
        return ( FAIL );
    }

    if(!CreateEventTest())
    {
        Fail ("Test failed\n");
    }

    PAL_Terminate();
    return ( PASS );

}
