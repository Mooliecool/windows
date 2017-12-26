/*============================================================
**
** Source: test1.c 
**
** Purpose: Test for CreateEvent.  Create an event, ensure the 
** HANDLE is valid.  Then check to ensure that the object is in the 
** signaled state.  Close the HANDLE and done.
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

/*
  Note:  From the rotor_pal documentation:

  lpEventAttributes will always be NULL, bManualReset can be either
  TRUE or FALSE, bInitialState can be either TRUE or FALSE, the lpName
  may be non-NULL

*/


#include <palsuite.h>

BOOL CreateEventTest()
{
    BOOL bRet = FALSE;
    DWORD dwRet = 0;

    LPSECURITY_ATTRIBUTES lpEventAttributes = NULL;
    BOOL bManualReset = TRUE; 
    BOOL bInitialState = TRUE;
    LPCTSTR lpName = "Event #1";

    /* Call CreateEvent, and check to ensure the returned HANDLE is a
       valid event HANDLE
    */
    
    HANDLE hEvent = CreateEvent( lpEventAttributes, 
                                 bManualReset, 
                                 bInitialState, 
                                 lpName); 
 
    if (hEvent != NULL)
    {
        /* Wait for the Object (for 0 time) and ensure that it returns
           the value indicating that the event is signaled.
        */
        dwRet = WaitForSingleObject(hEvent,0);

        if (dwRet != WAIT_OBJECT_0)
        {
            Trace("CreateEventTest:WaitForSingleObject %s "
                   "failed (%x)\n",lpName,GetLastError());
        }
        else
        {
            /* If we make it here, and CloseHandle succeeds, then the
               entire test has passed.  Otherwise bRet will still show
               failure
            */
            bRet = CloseHandle(hEvent);

            if (!bRet)
            {
                Trace("CreateEventTest:CloseHandle %s "
                       "failed (%x)\n",lpName,GetLastError());
            }           
        }
    }
    else
    {
        Trace("CreateEventTest:CreateEvent %s "
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
