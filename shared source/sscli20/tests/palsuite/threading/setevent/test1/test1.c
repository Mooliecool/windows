/*============================================================
**
** Source: test1.c 
**
** Purpose: Test for SetEvent.  Create an Event and then set
** this event, checking the return value.  Ensure that it returns
** positive.
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

BOOL SetEventTest()
{
    int bRet = 0;
    DWORD dwRet = 0;

    LPSECURITY_ATTRIBUTES lpEventAttributes = 0;
    BOOL bManualReset = TRUE; 
    BOOL bInitialState = FALSE;
    LPCTSTR lpName = "Event #3";

    /* Create an event which we can use with SetEvent */
    HANDLE hEvent = CreateEvent( lpEventAttributes, 
                                 bManualReset, bInitialState, lpName); 
 
    if (hEvent != INVALID_HANDLE_VALUE)
    {
        dwRet = WaitForSingleObject(hEvent,0);

        if (dwRet != WAIT_TIMEOUT)
        {
            Trace("SetEventTest:WaitForSingleObject %s "
                   "failed (%x)\n",lpName,GetLastError());
        }
        else
        {
            /* Set the event to the previously created event and check
               the return value.
            */
            bRet = SetEvent(hEvent);
            
            if (!bRet)
            {
                Trace("SetEventTest:SetEvent %s "
                       "failed (%x)\n",lpName,GetLastError());
            }
            else
            {
                dwRet = WaitForSingleObject(hEvent,0);

                if (dwRet != WAIT_OBJECT_0)
                {
                    Trace("SetEventTest:WaitForSingleObject %s "
                           "failed (%x)\n",lpName,GetLastError());
                }
                else
                {
                    dwRet = CloseHandle(hEvent);

                    if (!dwRet)
                    {
                        Trace("SetEventTest:CloseHandle %s "
                               "failed (%x)\n",lpName,GetLastError());
                    }
                }
            }
        }
    }
    else
    {
        Trace("SetEventTest:CreateEvent %s "
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
   
    if(SetEventTest() == 0)
    {
        Fail ("Test failed\n");
    }

    PAL_Terminate();
    return ( PASS );

}
