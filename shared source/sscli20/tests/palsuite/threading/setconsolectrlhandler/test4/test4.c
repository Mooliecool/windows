/*=====================================================================
**
** Source: test1.c
**
** Purpose: This is a MANUAL test.  This is also a NEGATIVE test.  We want
** this function to return 1 (FAIL). This test checks that we can set a 
** handler for CTRL_C and then remove that handler.
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

/* Note: If an error occurs in this test, we have to return 0.  
   The only time this test passes, is when CTRL-C is signaled, and the
   program exits with a code of 1.
*/

#include <palsuite.h>

int Flag = 1;

BOOL CtrlHandler(DWORD CtrlType) 
{ 
    if(CtrlType == CTRL_C_EVENT)
    {
        Flag = 0;
        return 1;
    }

    Trace("ERROR: The CtrlHandler was called, but the event was not a "
          "CTRL_C_EVENT.  This is considered failure.");

    return 0; 
} 

int __cdecl main(int argc, char **argv)
{
    int counter = 0;

    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }
    
    /* Call the function to set the CtrlHandler */
    if( SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE) == 0 )
    {
        Trace("ERROR: SetConsoleCtrlHandler returned zero, indicating failure."
             "  GetLastError() returned %d.\n",GetLastError());
        return 0;
    }       
    
    /* Call the function to remove the CtrlHandler */

    if( SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, FALSE) == 0)
    {
        Trace("ERROR: SetConsoleCtrlHandler returned zero, indicating failure "
             "when attempting to remove a handler.  "
             "GetLastError() returned %d.\n",GetLastError());  
        return 0;
    }
        
    /* Prompt for the tester to press CTRL-C.  They have a limited amount
       of time to type this.  (Incase the CTRL-C handler isn't working 
       properly)
    */
    printf("Please press CTRL-C now.  This is timed.  If CTRL-C is not "
           "pressed by the time I count to 1000000000 then the test "
           "will automatically fail.\n");
    
    while(Flag)
    {
        counter++;
        if(counter == 1000000000)
        {
            Trace("ERROR: The time ran out.  CTRL-C was never pressed.");
            return 0;
        }
    }
    
    Trace("MANUAL: The test has failed.  Now calling PAL_Terminate().\n");
    PAL_Terminate();
    return 0;
}

