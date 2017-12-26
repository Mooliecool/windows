/*============================================================================
**
** Source:  test2.c
**
** Purpose: Test that errno is 'per-thread' as noted in the documentation. 
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
**==========================================================================*/

#include <palsuite.h>

/* 
   This thread function just checks that errno is initially 0 and then sets
   it to a new value before returning.
*/
DWORD PALAPI ThreadFunc( LPVOID lpParam ) 
{ 
       
    if(errno != 0) 
    {
        *((DWORD*)lpParam) = 1;
    }

    errno = 20;

    return 0; 
} 


int __cdecl main(int argc, char *argv[])
{
    DWORD dwThreadId, dwThrdParam = 0; 
    HANDLE hThread; 
    
    
    if (PAL_Initialize(argc, argv))
    {
        return FAIL;
    }
    
    /* Set errno to a value within this thread */

    errno = 50;
    
    hThread = CreateThread(NULL, 0, ThreadFunc, &dwThrdParam, 0, &dwThreadId);
    
    if (hThread == NULL) 
    {
        Fail("ERROR: CreateThread failed to create a thread.  "
             "GetLastError() returned %d.\n",GetLastError());
    }
    
    WaitForSingleObject(hThread, INFINITE);
 
    /* This checks the result of calling the thread */
    if(dwThrdParam)
    {
        Fail("ERROR: errno was not set to 0 in the new thread.  Each "
             "thread should have its own value for errno.\n");
    }
    
    /* Check to make sure errno is still set to 50 */
    if(errno != 50)
    {
        Fail("ERROR: errno should be 50 in the main thread, even though "
             "it was set to 20 in another thread.  Currently it is %d.\n",
             errno);
    }
    
    PAL_Terminate();
    return PASS;
}
