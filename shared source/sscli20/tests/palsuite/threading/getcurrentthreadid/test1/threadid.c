/*============================================================
**
** Source: getcurrentthreadid/test1/threadid.c
**
** Purpose: Test to ensure GetCurrentThreadId returns the threadId of the
** current thread. 
** 
** Dependencies: CloseHandle
**               WaitForSingleObject
**               CreateThread
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

DWORD dwThreadIdTF;

DWORD PALAPI ThreadFunction ( LPVOID lpParam )
{
    Trace ("thread code executed\n");
    dwThreadIdTF = GetCurrentThreadId();
    return 0;
}

int __cdecl main( int argc, char **argv ) 
{
    extern DWORD dwThreadIdTF;
    DWORD dwThreadIdCT;
    HANDLE hThread; 
    DWORD dwThreadParam = 1;
    DWORD dwThreadWait;
    
    if(0 != (PAL_Initialize(argc, argv)))
    {
        return ( FAIL );
    }
    
    hThread = CreateThread(
        NULL,            
        0,               
        ThreadFunction,  
        &dwThreadParam,  
        0,               
        &dwThreadIdCT);  
    
    if ( NULL == hThread ) 
    {
        Fail ( "CreateThread() call failed - returned NULL");
    }
    else 
    {
	dwThreadWait = WaitForSingleObject( hThread, INFINITE );   
    
        Trace ("dwThreadWait returned %d\n", dwThreadWait );
    
	if ( dwThreadIdCT == dwThreadIdTF )
	{
            Trace ( "ThreadId numbers match - GetCurrentThreadId"
		     " works.  dwThreadIdCT == dwThreadIdTF == %d\n",
		     dwThreadIdTF );
	    PAL_Terminate();
            return ( PASS );
	}
	else 
	{
            Fail ( "ThreadId numbers don't match - "
		     "GetCurrentThreadId fails dwThreadIdCT = %d "
		     "and dwThreadIdTF = %d\n", dwThreadIdCT, dwThreadIdTF);
	}
    }

    PAL_Terminate();
    return (FAIL);

}
