/*============================================================
**
** Source: GetCurrentThread/test1/thread.c
**
** Purpose: Test to ensure GetCurrentThread returns a handle to 
** the current thread.
**
** Dependencies: GetThreadPriority
**               SetThreadPriority
**               Fail   
**               Trace
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

int __cdecl main( int argc, char **argv ) 
{

    HANDLE hThread; 
    int nPriority;
 
    if(0 != (PAL_Initialize(argc, argv)))
    {
        return ( FAIL );
    }
   
    hThread = GetCurrentThread();
    
    nPriority = GetThreadPriority(hThread);

    if ( THREAD_PRIORITY_NORMAL != nPriority )
    {
	if ( THREAD_PRIORITY_ERROR_RETURN == nPriority ) 
	{
	    Fail ("GetThreadPriority function call failed for %s\n"
		    "GetLastError returned %d\n", argv[0], GetLastError());
	}
	else 
	{
	    Fail ("GetThreadPriority function call failed for %s\n"
		    "The priority returned was %d\n", argv[0], nPriority);
	}
    }
    else
    {
	nPriority = 0;
	
	if (0 == SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST)) 
	{
	    Fail ("Unable to set thread priority.  Either handle doesn't"
		    " point to current thread \nor SetThreadPriority "
		    "function failed.  Failing test.\n");
	}
	
	nPriority = GetThreadPriority(hThread);
	
	if ( THREAD_PRIORITY_ERROR_RETURN == nPriority ) 
	{
	    Fail ("GetThreadPriority function call failed for %s\n"
		    "GetLastError returned %d\n", argv[0], GetLastError());
	}
	else if ( THREAD_PRIORITY_HIGHEST == nPriority ) 
	{
	    Trace ("GetCurrentThread returns handle to the current "
		    "thread.\n");
	    exit ( PASS );
	} 
	else 
	{
	    Fail ("Unable to set thread priority.  Either handle doesn't"
		    " point to current thread \nor SetThreadPriority "
		    "function failed.  Failing test.\n");
	}
    }

    PAL_Terminate();
    return ( PASS );    
    
}
