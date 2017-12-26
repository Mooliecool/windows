/*=============================================================================
**
** Source: test6.c
**
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               GenerateConsoleCtrlEvent
**
** Purpose:
**
** Test to ensure proper operation of the SetConsoleCtrlHandler()
** API by trying to remove a non-existent handler.
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


static BOOL g_bFlag1 = FALSE;
static BOOL g_bFlag2 = FALSE;


/* first handler function */
static BOOL PALAPI CtrlHandler1( DWORD CtrlType ) 
{ 
    if( CtrlType == CTRL_C_EVENT )
    {
        g_bFlag1 = TRUE;
        return TRUE;
    }

    return FALSE;
}


/* second handler function */
static BOOL PALAPI CtrlHandler2( DWORD CtrlType ) 
{ 
    if( CtrlType == CTRL_C_EVENT )
    {
        g_bFlag2 = TRUE;
        return TRUE;
    }

    return FALSE;
}





/* main entry point function */
int __cdecl main( int argc, char **argv ) 

{
    /* local variables */
    BOOL    ret = PASS;


    /* PAL initialization */
    if( (PAL_Initialize(argc, argv)) != 0 )
    {
        return( FAIL );
    }


    /* set the console control handler function */
    if( ! SetConsoleCtrlHandler( CtrlHandler1, TRUE ) )
    {
        ret = FAIL;
        Trace( "ERROR:%lu:SetConsoleCtrlHandler() failed to add "
                "CtrlHandler1\n",
                GetLastError() );
        Fail( "Test failed\n" );
    }
    
    /* test that the right control handler functions are set */
    if( ! GenerateConsoleCtrlEvent( CTRL_C_EVENT, 0 ) )
    {
        Trace( "ERROR:%lu:GenerateConsoleCtrlEvent() failed\n",
                GetLastError() );
        ret = FAIL;
        goto done;
    }

    /* give the handlers a chance to execute */    
    Sleep( 2000 );
    
    /* check the results */
    if( g_bFlag2 )
    {
        Trace( "ERROR:CtrlHandler2() was inexplicably called\n" );
        ret = FAIL;
        goto done;
    }
    
    if( ! g_bFlag1 )
    {
        Trace( "ERROR:CtrlHandler1() was not called but should have been\n" );
        ret = FAIL;
        goto done;
    }
    
    /* reset our flags */
    g_bFlag1 = FALSE;

    
    /* try to unset CtrlHandler2, which isn't set in the first place */
    if( SetConsoleCtrlHandler( CtrlHandler2, FALSE ) )
    {
        ret = FAIL;
        Trace( "ERROR:SetConsoleCtrlHandler() succeeded trying to "
                "remove CtrlHandler2, which isn't set\n" );
        goto done;
    }


    /* make sure that the existing control handler functions are still set */
    if( ! GenerateConsoleCtrlEvent( CTRL_C_EVENT, 0 ) )
    {
        Trace( "ERROR:%lu:GenerateConsoleCtrlEvent() failed\n",
                GetLastError() );
        ret = FAIL;
        goto done;
    }

    /* give the handlers a chance to execute */    
    Sleep( 2000 );
    
    /* check the results */
    if( g_bFlag2 )
    {
        Trace( "ERROR:CtrlHandler2() was inexplicably called\n" );
        ret = FAIL;
        goto done;
    }
    
    if( ! g_bFlag1 )
    {
        Trace( "ERROR:CtrlHandler1() was not called but should have been\n" );
        ret = FAIL;
        goto done;
    }
    
    
    
done:
    /* unset any handlers that were set */
    if( ! SetConsoleCtrlHandler( CtrlHandler1, FALSE ) )
    {
        ret = FAIL;
        Trace( "ERROR:%lu:SetConsoleCtrlHandler() failed to "
                "remove CtrlHandler1\n",
                GetLastError() );
        Fail( "Test failed\n" );
    }
    
    
    /* PAL termination */
    PAL_Terminate();

    
    /* return our result */
    return ret;
}

