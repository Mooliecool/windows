/*=============================================================================
**
** Source: threadpriority.c
**
** Purpose: Test to ensure GetThreadPriority works properly.
** 
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               Fail
**               CreateThread
**               WaitForSingleObject
**               GetLastError
**               time()
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
**===========================================================================*/
#include <palsuite.h>


/* one thread for each priority level */
#define NUM_THREADS 7

/*
 * we keep track of the thread handles globally
 * to avoid calling GetCurrentThread() -- we want
 * to minimize dependencies on other functions
 */
static HANDLE hThread[ NUM_THREADS ] =
{
    NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

/*
 * each priority level is represented, from
 * lowest to highest
 */
static int nPriority[ NUM_THREADS ] =
{
    THREAD_PRIORITY_IDLE,
    THREAD_PRIORITY_LOWEST,
    THREAD_PRIORITY_BELOW_NORMAL,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_ABOVE_NORMAL,
    THREAD_PRIORITY_HIGHEST,
    THREAD_PRIORITY_TIME_CRITICAL
};

static long nElapsedTime[ NUM_THREADS ] =
{
    0, 0, 0, 0, 0, 0, 0
};


/**
 * CheckThreadPriority
 *
 * Helper function that checks the current thread priority
 * against an expected value.
 */
static VOID CheckThreadPriority( int index, int expectedPriority )
{
    int priority;
    DWORD dwError = 0;

    /* get the current thread priority */
    priority = GetThreadPriority( hThread[index] );
    if( priority == THREAD_PRIORITY_ERROR_RETURN )
    {
        /* GetThreadPriority call failed */
        dwError = GetLastError();
        Fail( "Unexpected GetThreadPriority() failure at index %d "
              "with error %d\n", index, dwError );
    }
    else if( priority != expectedPriority )
    {
        /* unexpected thread priority detected */
        Fail( "Unexpected initial thread priority value %d reported\n",
              priority );
    }
}


/**
 * ThreadFunc
 *
 * Thread function that sets its own priority to the priority
 * value in the global array nPriority[] at the index specified
 * as an argument to the thread function.
 */
DWORD PALAPI ThreadFunc( LPVOID param )
{
    int i, j, k;
    int index;
    int total = 0;
    time_t start;
    time_t finish;
    long duration;
    DWORD dwError = 0;

    /* cast function argument to a thread priority value */
    index = (int)param;
    if( index < 0 || index >= NUM_THREADS )
    {
        Fail( "Thread index argument was out of range, value was %d\n",
              index );
    }

    /*
     * check the old thread priority, it should be THREAD_PRIORITY_NORMAL,
     * if not it could potentially be a bug in GetThreadPriority() and not
     * in CreateThread() -- at the very least it provides us with a known
     * thread priority value to test against under a known condition even
     * if it's a trivial one.
     */
    CheckThreadPriority( index, THREAD_PRIORITY_NORMAL );


    /* set the new thread priority */
    if( ! SetThreadPriority( hThread[index], nPriority[index] ) )
    {
        dwError = GetLastError();
        Fail( "Unexpected SetThreadPriority() failure with error %d\n",
			  dwError );
    }

    /* time this thread's duration */
    time( &start );

    /* check the new thread priority */
    CheckThreadPriority( index, nPriority[index] );

    /*
     * Do a lengthy benign operation
     */
    for( i=0; i<1000; i++ )
    {
        for( j=0; j<1000; j++ )
        {
            total = j * i;
            for( k=0; k<1000; k++ )
            {
                total += k + i;
            }
        }
    }

    /* finish timing the thread and calculate the duration */
    time( &finish );
    duration = (long)(finish - start);

    /* update the elapsed time array */
    nElapsedTime[ index ] = duration;

    return 0;
}


/**
 * main
 *
 * executable entry point
 */
INT __cdecl main( INT argc, CHAR **argv )
{
    DWORD IDThread;

    int i;

    /* PAL initialization */
    if( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
    }

    /* set the thread priority of the main to the highest possible value
       this will give the chance to the main thread to create all the
       other threads */
    if(!SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
    {
        DWORD dwError;

        dwError = GetLastError();
        Fail( "Unexpected SetThreadPriority() failure with error %d\n",
			  dwError );
    }

    printf( "Launching multiple threads, please wait...\n" );

    /* Create multiple threads. */
    for( i = 0; i < NUM_THREADS; i++ )
    {
        hThread[i] = CreateThread( NULL,         /* no security attributes */
                                   0,            /* use default stack size */
          (LPTHREAD_START_ROUTINE) ThreadFunc,   /* thread function */
                          (LPVOID) i,            /* pass thread index as */
                                                 /* function argument */
                                   CREATE_SUSPENDED, /* create suspended */
                                   &IDThread );  /* returns thread identifier */

        /* Check the return value for success. */
        if( hThread[i] == NULL )
        {
	        /*ERROR */
            DWORD dwError = GetLastError();
            Fail( "Unexpected CreateThread error %d\n",
                  dwError );
        }

        ResumeThread(hThread[i]);
    }

    /* Wait for all threads to finish */
    for (i = 0; i < NUM_THREADS; i++)
    {
        DWORD dwRet;

        dwRet = WaitForSingleObject( hThread[i], INFINITE );

        if( dwRet == WAIT_FAILED )
        {
            /* ERROR */
            DWORD dwError = GetLastError();
            Fail( "Unexpected WaitForSingleObject error %d\n",
                  dwError );
        }
    }

    /* make sure the thread processing times match the priorities */
    for( i=NUM_THREADS-1; i > 0; i-- )
    {
        if( nElapsedTime[i] >= nElapsedTime[i-1] )
        {
            /*
             * a lower-priority thread finished faster than a lower-priority
             * thread -- this shouldn't happen if the dummy thread function
             * is non-trivial in length
             */
            Fail( "Failed: processing time doesn't match thread priority\n" );
        }
    }

    PAL_Terminate();
    return PASS;
}
