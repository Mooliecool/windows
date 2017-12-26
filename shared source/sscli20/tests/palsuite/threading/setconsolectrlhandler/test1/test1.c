/*=====================================================================
**
** Source: test1.c
**
** Purpose: Set the CtrlHandler to flip a flag 
** when CTRL_C_EVENT is signalled.  If the flag is flipped, the test 
** succeeds. Otherwise, it will fail. Finally, check to see that 
** removing the handler works alright.
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

int Flag = 1;

BOOL CtrlHandler(DWORD CtrlType) 
{ 
    if(CtrlType == CTRL_C_EVENT)
    {
        Flag = 0;
        return 1;
    }

    Trace("ERROR: The CtrlHandler was called, but the event was not a "
          "CTRL_C_EVENT.  This is considered failure.\n");

    return 0; 
} 

int __cdecl main(int argc, char **argv)
{

    if (0 != PAL_Initialize(argc,argv))
    {
        return FAIL;
    }
    
    /* Call the function to set the CtrlHandler */
    if( SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE) == 0 )
    {
        Fail("ERROR: SetConsoleCtrlHandler returned zero, indicating failure."
             "  GetLastError() returned %d.\n",GetLastError());
    }       
    
    /* Generate a CTRL_C event */
    if(GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0) == 0)
    {
        Fail("ERROR: GenerateConsoleCtrlEvent failed, returning zero.  "
             "GetLastError returned %d.\n",GetLastError());
    }
    
    
    /* Sleep for a couple seconds to ensure that the event has time to
       complete.
    */
    Sleep(2000);

    /* The event handling function should set Flag = 0 if it worked
       properly.  Otherwise this test fails.
    */
    if(Flag)
        {
        Fail("ERROR: When CTRL-C was generated it wasn't handled by the "
             "Ctrl Handler which was defined.\n");
        }
    

    /* Call the function to remove the CtrlHandler */

    if( SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, FALSE) == 0)
    {
        Fail("ERROR: SetConsoleCtrlHandler returned zero, indicating failure "
             "when attempting to remove a handler.  "
             "GetLastError() returned %d.\n",GetLastError());  
    }
    
    PAL_Terminate();
    return PASS;
}

