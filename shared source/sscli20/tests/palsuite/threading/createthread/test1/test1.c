/*============================================================
**
** Source: test1.c 
**
** Purpose: Test for CreateThread.  Call CreateThread and ensure
** that it succeeds.  Also check to ensure the paramater is passed
** properly.
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

DWORD dwCreateThreadTestParameter = 0;

DWORD PALAPI CreateThreadTestThread( LPVOID lpParameter)
{
    DWORD dwRet = 0;
    
    /* save parameter for test */
    dwCreateThreadTestParameter = (DWORD)lpParameter;
    
    return dwRet;
}

BOOL CreateThreadTest()
{
    BOOL bRet = FALSE;
    DWORD dwRet = 0;

    LPSECURITY_ATTRIBUTES lpThreadAttributes = NULL;
    DWORD dwStackSize = 0; 
    LPTHREAD_START_ROUTINE lpStartAddress =  &CreateThreadTestThread;
    LPVOID lpParameter = lpStartAddress;
    DWORD dwCreationFlags = 0;  /* run immediately */
    DWORD dwThreadId = 0;

    HANDLE hThread = 0;

    dwCreateThreadTestParameter = 0;

    /* Create a thread, passing the appropriate paramaters as declared
       above.
    */
    
    hThread = CreateThread( lpThreadAttributes,   
                            dwStackSize,          
                            lpStartAddress,       
                            lpParameter,          
                            dwCreationFlags,      
                            &dwThreadId );       
    
    /* Ensure that the HANDLE is not invalid! */
    if (hThread != INVALID_HANDLE_VALUE)
    {
        dwRet = WaitForSingleObject(hThread,INFINITE);

        if (dwRet != WAIT_OBJECT_0)
        {
            Trace("CreateThreadTest:WaitForSingleObject "
                   "failed (%x)\n",GetLastError());
        }
        else
        {
            /* Check to ensure that the parameter passed to the thread
               function is the same in the function as what we passed.
            */
            
            if (dwCreateThreadTestParameter != (DWORD)lpParameter)
            {
                Trace("CreateThreadTest:parameter error.  The "
                       "parameter passed should have been %d but when "
                       "passed to the Thread function it was %d.\n",
                       dwCreateThreadTestParameter,lpParameter);
            }
            else
            {
                bRet = TRUE;
            }
            CloseHandle(hThread);
        }
    }
    else
    {
        Trace("CreateThreadTest:CreateThread failed (%x)\n",GetLastError());
    }

    return bRet; 
}


int __cdecl main(int argc, char **argv)
{
    if(0 != (PAL_Initialize(argc, argv)))
    {
        return ( FAIL );
    }
    
    if(!CreateThreadTest()) 
    {
        Fail ("Test failed\n");
    }
      
    PAL_Terminate();
    return ( PASS );

}
