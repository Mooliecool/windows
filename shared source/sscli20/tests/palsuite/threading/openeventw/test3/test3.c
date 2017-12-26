/*=============================================================================
**
** Source: test3.c
**
** Purpose: Test to ensure that OpenEventW() works when
** opening an event created by another process. This test
** program launches a child process which creates a
** named, initially-unset event. The child waits up to
** 10 seconds for the parent process to open that event
** and set it, and returns PASS if the event was set or FAIL
** otherwise. The parent process checks the return value
** from the child to verify that the opened event was
** properly used across processes.
**
** Dependencies: PAL_Initialize
**               PAL_Terminate
**               Fail
**               ZeroMemory
**               GetCurrentDirectoryW
**               CreateProcessW
**               WaitForSingleObject
**               GetExitCodeProcess
**               GetLastError
**               strlen
**               strncpy
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


static const char* rgchPathDelim = "\\";


int
mkAbsoluteFilename( LPSTR dirName,
                    DWORD dwDirLength,
                    LPCSTR fileName,
                    DWORD dwFileLength,
                    LPSTR absPathName )
{
    DWORD sizeDN, sizeFN, sizeAPN;

    sizeDN = strlen( dirName );
    sizeFN = strlen( fileName );
    sizeAPN = (sizeDN + 1 + sizeFN + 1);

    /* ensure ((dirName + DELIM + fileName + \0) =< _MAX_PATH ) */
    if( sizeAPN > _MAX_PATH )
    {
        return ( 0 );
    }

    strncpy( absPathName, dirName, dwDirLength +1 );
    strncpy( absPathName, rgchPathDelim, 2 );
    strncpy( absPathName, fileName, dwFileLength +1 );

    return (sizeAPN);

}


int __cdecl main( int argc, char **argv )

{
    BOOL ret = FAIL;
    const char* rgchChildFile = "childprocess";

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    DWORD dwExitCode;
    DWORD dwFileLength;
    DWORD dwDirLength;
    DWORD dwSize;

    char  rgchDirName[_MAX_DIR];
    char  absPathBuf[_MAX_PATH];
    char* rgchAbsPathName;

    DWORD  dwRet = 0;
    HANDLE hEvent = NULL;
    WCHAR  wcName[] = {'P','A','L','R','o','c','k','s','\0'};
    LPWSTR lpName = wcName;


    /* initialize the PAL */
    if( PAL_Initialize(argc, argv) != 0 )
    {
	    return( FAIL );
    }

    /* zero our process and startup info structures */
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof( si );
    ZeroMemory( &pi, sizeof(pi) );

    /* build the absolute path to the child process */
    rgchAbsPathName = &absPathBuf[0];
    dwFileLength = strlen( rgchChildFile );

    dwDirLength = GetCurrentDirectory( _MAX_PATH, rgchDirName );
    if( dwDirLength == 0 )
    {
        Fail( "ERROR:%lu:GetCurrentDirectory call failed\n",
              GetLastError() );
    }

    dwSize = mkAbsoluteFilename(   rgchDirName,
                                   dwDirLength,
                                   rgchChildFile,
                                   dwFileLength,
                                   rgchAbsPathName );
    if( dwSize == 0 )
    {
        Fail( "Palsuite Code: mkAbsoluteFilename() call failed.  Could ",
              "not build absolute path name to file\n.  Exiting.\n" );
    }

    /* launch the child process */
    if( !CreateProcess(     NULL,               /* module name to execute */
                            rgchAbsPathName,    /* command line */
                            NULL,               /* process handle not */
                                                /* inheritable */
                            NULL,               /* thread handle not */
                                                /* inheritable */
                            FALSE,              /* handle inheritance */
                            CREATE_NEW_CONSOLE, /* dwCreationFlags */
                            NULL,               /* use parent's environment */
                            NULL,               /* use parent's starting */
                                                /* directory */
                            &si,                /* startup info struct */
                            &pi )               /* process info struct */
        )
    {
        Fail( "ERROR:%lu:CreateProcess call failed\n",
              GetLastError() );
    }

    /* give the child process a couple of seconds to run */
    Sleep( 2000 );

    /* open a handle to the event created in the child process */
    hEvent = OpenEventW( EVENT_ALL_ACCESS,  /* we want all rights */
                         FALSE,             /* no inherit         */
                         lpName );

    if( hEvent == NULL )
    {
        /* ERROR */
        Trace( "ERROR:%lu:OpenEventW() call failed\n", GetLastError() );
        goto childwait;
    }

    /* verify that the event isn't signalled yet */
    dwRet = WaitForSingleObject( hEvent, 0 );
    if( dwRet != WAIT_TIMEOUT )
    {
        /* ERROR */
        Trace( "ERROR:WaitForSingleObject() call returned %lu, "
                "expected WAIT_TIMEOUT\n",
                dwRet );
        goto childwait;
    }

    /* set the event -- should take effect in the child process */
    if( ! SetEvent( hEvent ) )
    {
        /* ERROR */
        Trace( "ERROR:%lu:SetEvent() call failed\n", GetLastError() );
    }


    /* wait for the child process to complete */
    dwRet = WaitForSingleObject ( pi.hProcess, 10000 );
    if( dwRet != WAIT_OBJECT_0 )
    {
        Trace( "ERROR:WaitForSingleObject() returned %lu, "
                "expected %lu\n",
                dwRet,
                WAIT_OBJECT_0 );
        goto cleanup;
    }


childwait:

    /* check the exit code from the process */
    if( ! GetExitCodeProcess( pi.hProcess, &dwExitCode ) )
    {
        Trace( "ERROR:%lu:GetExitCodeProcess call failed\n",
              GetLastError() );
        goto cleanup;
    }

    /* check for success */
    ret = (dwExitCode == PASS) ? PASS : FAIL;



cleanup:

    if( hEvent != NULL )
    {
        if( ! CloseHandle ( hEvent ) )
        {
            Trace( "ERROR:%lu:CloseHandle call failed on event handle\n",
                  GetLastError() );
            ret = FAIL;
        }
    }


    /* close process and thread handle */
    if( ! CloseHandle ( pi.hProcess ) )
    {
        Trace( "ERROR:%lu:CloseHandle call failed on process handle\n",
              GetLastError() );
        ret = FAIL;
    }

    if( ! CloseHandle ( pi.hThread ) )
    {
        Trace( "ERROR:%lu:CloseHandle call failed on thread handle\n",
              GetLastError() );
        ret = FAIL;
    }

    /* output a convenient error message and exit if we failed */
    if( ret == FAIL )
    {
        Fail( "test failed\n" );
    }


    /* terminate the PAL */
    PAL_Terminate();

    /* return success */
    return ret;
}
