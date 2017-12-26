/*============================================================
**
** Source: exectest_unix.c
**
**
** Purpose: The code to execute a given test under Unix.
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
**
**=========================================================*/

#include <stdio.h>
#include <sys/signal.h>
#include <time.h>
#include "testharness.h"
#include "error.h"
#include "exectest.h" 


char szOutputFile[MAX_PATH];

char *LogSignalError(int TheSignal);

/*
 * Create a temporary file name based on the current time
 * and return it in the given buffer.
 */
int MakeTempFileName(char *szBuf)
{
    time_t t;
    struct tm *pCurTime;
    int fd;
    
    time(&t);
    pCurTime = localtime(&t);

    sprintf(szBuf, "/tmp/%d_%d_XXXXXX", pCurTime->tm_min, pCurTime->tm_sec);
    fd = mkstemp(szBuf);
    if (fd == -1)
    {
        return 0;
    }

    close(fd);
    return 1;
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
    char szOutputFile[L_tmpnam];
    FILE *FOut;  /* Output File */ 
    FILE *FErr;  /* Error File */ 
    int ret = 0;
    int iOutLen;
    int status;
    pid_t pid;
    char *szTok;
    int argc;
    char *argv[32];
    char *szCmd;
  
    /* Create temporary output file name */
    if (!MakeTempFileName(szOutputFile))
    {
        return 1;
    }

    /*
     * Flush all file buffers now so they won't be flushed later by both 
     * parent and child.
     */
    fflush(NULL);
  
    pid = fork();
  
    if (pid == -1)
    {
        return 1;
    }
    /* Child Code */
    else if (0 == pid)
    {
        /* This is leaked, but it only affects the child process. */
        szCmd = malloc(strlen(strCmdLine) + 1);
        if (szCmd == NULL)
        {
            /* Not much we can do about this */
            exit(1);
        }
        
        FOut = freopen(szOutputFile, "a", stdout);
        FErr = freopen(szOutputFile, "a", stderr);

        strcpy(szCmd, strCmdLine);
        argc = 0;
                
        szTok = strtok(szCmd, " ");
      
        while (szTok != 0)
        {           
            argc++;
            argv[argc-1] = szTok;
          
            szTok = strtok(0, " ");
        }
      
        argv[argc] = 0;
      
        alarm(60); /* Time out after 60 seconds if test hangs */
        execvp(argv[0],argv);
        
        /* If it actually gets here, it means that exec has failed. */

        fflush (FOut);
        fclose (FOut);

        fflush (FErr);
        fclose (FErr);

        exit(1);
    }
    /* Parent Code */
    else
    {  
        
        /* Wait until child process exits.*/
        
        if (waitpid (pid, &status, 0) != pid)
        {
            ret = 1;
        }
        else
        {  
            if (WIFEXITED (status))
            {
                *pRetCode = (unsigned int)WEXITSTATUS(status);
            }
            /* If it signals, generate a message to put in the log */
            else if(WIFSIGNALED(status)) 
            {
                FOut = fopen(szOutputFile,"a");
                fprintf(FOut,LogSignalError(WTERMSIG(status)));
                *pRetCode = (unsigned int)WTERMSIG(status);
                fclose(FOut);
            }
            else
            {
                ret = 1;
            }
          
        }
    }

    FOut = fopen(szOutputFile, "r");
    if (FOut != 0)
    {
        iOutLen = fread(szOutputBuf, 1, iMaxOutput-1, FOut);
        szOutputBuf[iOutLen] = 0;
        fclose(FOut);     
    }
    else
    {
        strcpy(szOutputBuf, "-----");
    }
  
    unlink(szOutputFile);
  
    return ret;
}


/*
 * Converts a signal to an appropriate string.
 */
char *LogSignalError(int TheSignal) 
{
    char * ret = "";

    switch (TheSignal) 
    {
    case SIGHUP:
        ret = "The program signaled SIGHUP.";
        break;
    case SIGINT:
        ret = "The program signaled SIGINT.";
        break;
    case SIGQUIT:
        ret = "The program signaled SIGQUIT.";
        break;
    case SIGILL:
        ret = "The program signaled SIGILL, illegal instruction.";
        break;
    case SIGTRAP:
        ret = "The program signaled SIGTRAP.";
        break;
    case SIGABRT:
        ret = "The program signaled SIGABRT, abort().";
        break;
#ifdef SIGEMT
    case SIGEMT:
        ret = "The program signaled SIGEMT, EMT Instruction.";
        break;
#endif  // SIGEMT
    case SIGFPE:
        ret = "The program signaled SIGFPE, Floating Point Exception.";
        break;
    case SIGKILL:
        ret = "The program signaled SIGKILL.";
        break;
    case SIGBUS:
        ret = "The program signaled SIGBUS, Bus Error.";
        break;
    case SIGSEGV:
        ret = "The program signaled SIGEGV, segmentation violation.";
        break;
    case SIGSYS:
        ret = "The program signaled SIGSYS, Non-existent system call invoked.";
        break;
    case SIGPIPE:
        ret = "The program signaled SIGPIPE, write on a pipe with no one "
            "to read it.";
        break;
    case SIGALRM:
        ret = "The program signaled SIGALRM, the alarm went off.  "
            "The test took too long to complete (it was hanging)";
        break;
    default:
        ret = "The program signaled an unidentified signal.";
        break;
    }

    return ret;  
}

