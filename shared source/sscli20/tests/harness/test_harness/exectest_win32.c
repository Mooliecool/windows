/*============================================================
**
** Source: exectest_win32.c
**
**
** Purpose: The code to execute a given test under Win32.
**
** 
**  
**   Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
**  
**   The use and distribution terms for this software are contained in the file
**   named license.txt, which can be found in the root of this distribution.
**   By using this software in any fashion, you are agreeing to be bound by the
**   terms of this license.
**  
**   You must not remove this notice, or any other, from this software.
**  
** 
**
**=========================================================*/

#include <stdio.h>
#include <io.h>
#include "testharness.h"
#include "error.h"
#include "exectest.h" 

char szOutputFile[MAX_PATH];


/*
 * Create a temporary file name based on the tick count
 * and return it in the given buffer.
 */
void MakeTempFileName(char *szBuf)
{
    char szTmpBuf[MAX_PATH];

    GetTempPath(MAX_PATH, (LPSTR)szTmpBuf);

    sprintf(szBuf, "%s%lu_XXXXXX", szTmpBuf, GetTickCount());
    mktemp(szBuf);
}

/*
 * Executes the given command, placing the return code in pRetCode.  Standard
 * output is copied to the output buffer (up to the max).
 * If the function fails to execute the command for some reason, 1 is returned, 
 * otherwise 0.
 */
int ExecuteTest(char *strCmdLine, unsigned int *pRetCode, 
                     char *szOutputBuf, int iMaxOutput)
{
    FILE *FOut;  /* Output File */ 
    int ret = 0;
    int iOutLen;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DEBUG_EVENT debugEvent;
    DWORD dwRetCode;
    HANDLE hOutput;
    SECURITY_ATTRIBUTES sa;

    /* Create temporary output file name */
    MakeTempFileName(szOutputFile);


    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = 0;
    sa.bInheritHandle = TRUE;

    /* Create a file for the child's stdout */
    hOutput = CreateFile(szOutputFile, GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE, &sa,
                        CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, 0);

    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = hOutput;
    si.hStdError = hOutput;

    /* Start the child process. */
    if (0 == CreateProcess(NULL,   /* No module name (use command line). */
                           strCmdLine, /* Command line. */
                           NULL,   /* Process handle not inheritable. */
                           NULL,   /* Thread handle not inheritable. */
                           TRUE,   /* Set handle inheritance to TRUE. */
                           /* Indicate that we want to debug the process. */
                           DEBUG_ONLY_THIS_PROCESS, 
                           NULL,   /* Use parent's environment block. */
                           NULL,   /* Use parent's starting directory. */
                           &si,    /* Pointer to STARTUPINFO structure. */
                           &pi ))/* Pointer to PROCESS_INFORMATION structure. */ 
    {
        CloseHandle(hOutput);
        DeleteFile(szOutputFile);
        return 1;
    }


    /*
     * Debug message handling loop.  Won't stop until the child exits.
     * Note: This code is MEANT to disable the ability to debug.  The test
     * harness should run without interuption until complete.  Any debugging 
     * should be done on individual test cases.  Perhaps we'll create a debug
     * mode at some point to switch it on and off?
    */  
    for (;;)
    {
        /* Wait for at most 60 seconds */
        if (!WaitForDebugEvent(&debugEvent, 65000))
        {
            TerminateProcess(pi.hProcess, -1);
            ret = 1;
            break;
        }    
        if (debugEvent.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
        {        
            /* After Continuing from here, the debugged application will exit. */
            ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, 
                               DBG_CONTINUE);
      
            /* 
             * For whatever reason, we get debugging messages from the child 
             * processes (seems to me the DEBUG_ONLY_THIS_PROCESS flag would
             * prevent this).  Don't stop handling messages if this is a child.
             */
            if (pi.dwProcessId == debugEvent.dwProcessId)
            {
                break;
            }
        }
        else if (debugEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
        {
            /*
             * If the exception is a debug breakpoint, we squash it.
             * Otherwise we just throw it back at the application.
             */
            if (debugEvent.u.Exception.ExceptionRecord.ExceptionCode ==
                 EXCEPTION_BREAKPOINT)           
            {
                ContinueDebugEvent(debugEvent.dwProcessId, 
                                   debugEvent.dwThreadId, DBG_CONTINUE);
            }
            else
            {
                ContinueDebugEvent(debugEvent.dwProcessId, 
                                   debugEvent.dwThreadId, 
                                   DBG_EXCEPTION_NOT_HANDLED);
            }            

        }
        else if (debugEvent.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT)
        {
            /*
             * Close the handles so that when the child process exits,
             * the child's EXE and DLL files don't remain locked on disk.
             */
            if (debugEvent.u.CreateProcessInfo.hFile) 
            {
                CloseHandle(debugEvent.u.CreateProcessInfo.hFile);
            }
            if (debugEvent.u.CreateProcessInfo.hProcess) 
            {
                CloseHandle(debugEvent.u.CreateProcessInfo.hProcess);
            }
            if (debugEvent.u.CreateProcessInfo.hThread) 
            {
                CloseHandle(debugEvent.u.CreateProcessInfo.hThread);
            }
            ContinueDebugEvent(debugEvent.dwProcessId, 
                               debugEvent.dwThreadId, DBG_CONTINUE);
        }
        else if (debugEvent.dwDebugEventCode == CREATE_THREAD_DEBUG_EVENT)
        {
            /*
             * Close the handles so that when the child process exits,
             * the child's EXE and DLL files don't remain locked on disk.
             */
            if (debugEvent.u.CreateThread.hThread) 
            {
                CloseHandle(debugEvent.u.CreateThread.hThread);
            }
            ContinueDebugEvent(debugEvent.dwProcessId, 
                               debugEvent.dwThreadId, DBG_CONTINUE);
        }
        else if (debugEvent.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT)
        {
            /*
             * Close the handles so that when the child process exits,
             * the child's EXE and DLL files don't remain locked on disk.
             */
            if (debugEvent.u.LoadDll.hFile) 
            {
                CloseHandle(debugEvent.u.LoadDll.hFile);
            }
            ContinueDebugEvent(debugEvent.dwProcessId, 
                               debugEvent.dwThreadId, DBG_CONTINUE);
        }
        else
        {
            ContinueDebugEvent(debugEvent.dwProcessId, 
                               debugEvent.dwThreadId, DBG_CONTINUE);
        }
    }

    /* Wait for the child process to complete (Max 10 Seconds) */
    WaitForSingleObject(pi.hProcess, 10000);

    /* Retrieve the value returned by the child */
    if (GetExitCodeProcess(pi.hProcess, &dwRetCode) == 0)
    {
        ret = 1;
    }
    else
    {     
        *pRetCode = (unsigned int)dwRetCode;
    }

    /*
     * Close process and thread handles. 
     */
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hOutput);

  
    /* Retrieve the output written to the output file by the child */
    FOut = fopen(szOutputFile, "r");
    if (FOut != 0)
    {
        iOutLen = fread(szOutputBuf, 1, iMaxOutput-1, FOut);
        szOutputBuf[iOutLen] = 0;
        fclose(FOut);     
    }
    else
    {
        szOutputBuf[0] = 0;
    }
  
    /* Delete the output file */
    if (DeleteFile(szOutputFile) == 0)
    {
        /* If we can't delete it display an error message */
        HarnessMessage("\n\nFailed to Deleted Temp File %s. Error: %u\n\n",
                       szOutputFile, GetLastError());
    }
  
    return ret;
}
