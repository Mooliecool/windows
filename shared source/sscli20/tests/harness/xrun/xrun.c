/*=============================================================
**
** Source: xrun.c
**
** Purpose: Start server and client for test.
**          This tool is to start server and client for test,
**          This tool will report the server or client test result
**          depending on the 3rd parameter(1 or 2)
**             1: report server test result
**             2: report client test result
**             usage: 
**             xrun server client flag(1 or 2)
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
**============================================================*/
#include <stdio.h>
#include <ctype.h>

#ifdef WIN32
#include <windows.h>
#else
/*Under FreeBSD*/
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#endif

#define SERVER_FLAG 1
#define CLIENT_FLAG 2
#define MAX_ARGS 30

#ifdef WIN32

/* no additional wrapper is required at this time. CreateProcess seems to be suffice */

#else

int execute_cmd(char* command)
{
   
    char* arglist[MAX_ARGS+1]; /* array to hold the command and argument list */
    char *token;               /* temporary variable to hold the parsed tokens from strtok */
    int length = 0;            /* length of the arglist array */
    int errcode = 0;           /* error code from this routine */
  
    /* get the first token from the command sent */
    token = strtok(command," "); 

    /* proceed as long as there is more work to do */
    while(token != NULL)
    {
        /* allocate the memory required to hold the command file or the argument */
        /* copy the token into the arglist in the location pointed by length */
        /* increment length */
        if(NULL !=  (arglist[length] = (char*)malloc(strlen(token)+1)))
        {
           strcpy(arglist[length],token);
           token = strtok(NULL," ");
           if (++length == MAX_ARGS) 
           {
               errcode = -1;			
               break;
           }

        /* if malloc fails log the appropriate message */
        /* set the error code as appropriate and break from the loop */
        } else {
       
           printf("Failed to allocate memory for execv");
           errcode = -1;
           break;   
          
        }
    }

    /* set the last element to NULL - as required by execv */
    arglist[length] = NULL;
    
    /* if error code is not already set launch the test case */
    if (-1 != errcode)
    {
       errcode = execv(arglist[0],arglist);
    }

    /* free the allocated memory */
    while(length >= 0)
    {
       free(arglist[length]);
       length--;
    } 
   
    return errcode;
}


#endif


int main(int argc, char *argv[])
{
    int nFlag;
    int ret;
    int retServer;
    int retClient;
    
#ifdef WIN32
    STARTUPINFO si;
    STARTUPINFO si_2;
    PROCESS_INFORMATION pi;
    PROCESS_INFORMATION pi_2;
    DWORD dwRetCode;
    HANDLE Processes[2];
#else
    /*Under FreeBSD*/
    pid_t pid;      /*for server process ID*/
    pid_t pid_2;    /*for client process ID*/
    int status;     /*for server status*/
    int status_2;   /*for client status*/

    

#ifndef _TIMESPEC_DECLARED
#ifdef __APPLE__
/* Mac OS X supports nanosleep, but doesn't have a header for it in 10.1. */

    struct timespec
    {
        time_t tv_sec;
        long tv_nsec;
    };
    
    int nanosleep(struct timespec *, struct timespec *);

#endif  /* __APPLE__ */
#endif  /* !_TIMESPEC_DECLARED */

    struct timespec rqtp;  /* time interval for nanosleep */ 
#endif

    if(argc == 1 || ((argc == 2) && !strcmp(argv[1],"/?")) 
       || !strcmp(argv[1],"/h") || !strcmp(argv[1],"/H"))
    {
        printf("Server and Client launching tool V1.0 2001\n");
        printf("Usage:\n");
        printf("xrun arg1(server) arg2(client) flag(1 or 2)\n");
        return -1;
    }

    if(argc < 4)
    {
        printf("\nInvalid argument number!\n");
        return -1;
    }

    nFlag = atoi(argv[3]);

    if(nFlag<1 || nFlag>2)
    {
        printf("\nInvalid flag number!\n");
        return -1;
    }

    retServer = 0;
    retClient = 0;


#ifdef WIN32

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    ZeroMemory( &si_2, sizeof(si_2) );
    si_2.cb = sizeof(si_2);
    ZeroMemory( &pi_2, sizeof(pi_2) );

    /*Start a child process for server*/
    if (0 == CreateProcess( NULL,    /* No module name (use command line). */
                            argv[1], /* Command line. */
                            NULL,    /* Process handle not inheritable. */
                            NULL,    /* Thread handle not inheritable. */
                            TRUE,    /* Set handle inheritance to FALSE. */
                            0,       /* No creation flags. */
                            NULL,    /* Use parent's environment block. */
                            NULL,    /* Use parent's starting directory. */
                            &si,     /* Pointer to STARTUPINFO structure. */
                            &pi ))   /* Pointer to PROCESS_INFORMATION 
                                        structure. */
    {
        printf("\nFailed to create child process for server routine!\n");
        return -1;
    }

    /* Give the server about 1.5 seconds to get set up */
    Sleep(1500); //was 0.5 seconds

    /*start a child process for client*/
    if (0 == CreateProcess( NULL,    /* No module name (use command line). */
                            argv[2], /* Command line. */
                            NULL,    /* Process handle not inheritable. */
                            NULL,    /* Thread handle not inheritable. */
                            TRUE,    /* Set handle inheritance to FALSE. */
                            0,       /* No creation flags. */
                            NULL,    /* Use parent's environment block. */
                            NULL,    /* Use parent's starting directory. */
                            &si_2,   /* Pointer to STARTUPINFO structure. */
                            &pi_2 )) /* Pointer to PROCESS_INFORMATION 
                                          structure. */
    {
        
        printf("\nFailed to create child process for client routine!\n");
        
        TerminateProcess(pi.hProcess, 1);
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
        
        return -1;
    }

    Processes[0] = pi.hProcess;
    Processes[1] = pi_2.hProcess;

    /*Wait until child process exits.*/
    if (WaitForMultipleObjects(2, Processes, TRUE, 60000) == WAIT_TIMEOUT)
    {
        retServer = -1;
        retClient = -1;
        TerminateProcess(pi.hProcess, 1);
        TerminateProcess(pi_2.hProcess, 1);
    }
    else
    {
        /*catch the server return code*/
        if (GetExitCodeProcess(pi.hProcess, &dwRetCode) == 0)
        {
            retServer = -1;
        }
        else
        {
            retServer =  dwRetCode;
        }

        /*catch the client return code*/
        if (GetExitCodeProcess(pi_2.hProcess, &dwRetCode) == 0)
        {
            retClient = -1;
        }
        else
        {
            retClient = dwRetCode;
        }
    }

    /*Close process and thread handles.*/
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    CloseHandle( pi_2.hProcess );
    CloseHandle( pi_2.hThread );

#else /*Under FreeBSD*/

    /*create a child process for server*/
    pid = fork();
    if(-1 == pid)
    {
        printf("\nFailed to create a process for server!\n");
        return -1;
    }
    if(0 == pid)
    {
        
        /*child process for starting a server routine*/
        if( -1 == execute_cmd(argv[1]))
        {
            printf("\nFailed to call execv to run a server routine\n");
            return -1;
        }
    }
    else
    {
        /*parent process*/
        /* Give the server about 1.5 seconds to get set up. */
        rqtp.tv_sec = 1; //was 0
        rqtp.tv_nsec = 500000000;
        nanosleep(&rqtp, 0);
        
        /*create another process for client routine*/
        pid_2 = fork();
        if(0 == pid_2)
        {
            /*child process for starting a client routine*/
            if(-1 == execute_cmd(argv[2]))
            {
                printf("\nFailed to call execv to run a client routine\n");
                return -1;
            }
        }

        /*wait for the termination of child precess(client)*/
        if (waitpid(pid_2, &status_2,0) != pid_2)
        {
            printf("\nClient process terminated unnormally!\n");
            retClient = -1;
        }
        else
        {
            if (WIFEXITED(status_2))
            {
                retClient = WEXITSTATUS(status_2);
            }
            else
            {
                printf("\nClient process timed out!\n");
                retClient = -1;
            }
        }

        /*wait for the termination of child precess(server)*/
        if (waitpid(pid, &status,0) != pid)
        {
            printf("\nServer process terminates unnormally!\n");
            retServer = -1;
        }
        else
        {
            if (WIFEXITED(status))
            {
                retServer = WEXITSTATUS(status);
            }
            else
            {
                printf("\nServer process timed out!\n");
                retServer = -1;
            }
        }
    }
#endif

    if (retServer == -1 || retClient == -1)
    {
        ret = -1;
    }
    if (SERVER_FLAG == nFlag)
    {
        ret = retServer;
    }
    else
    {
        ret = retClient;
    }
       
    return ret;
}

