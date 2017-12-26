/*=============================================================
**
** Source: select_server.c
**
** Purpose: Positively test the select API for readable socket(s) 
**          with some waiting time. This is a server routine 
**          which must be started before the related client is
**          started.
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
#include <palsuite.h>

/* Wait for client to finish in 10 seconds */
#define TIMEOUT 10000 

int __cdecl main(int argc, char *argv[])
{
    WORD VersionRequested = MAKEWORD(2, 2);
    WSADATA WsaData;
    int err;
    int socketID;
    struct sockaddr_in mySockaddr;
    int nBacklogNumber = 1;

    int socketFds;
    fd_set readFds;
    struct timeval waitTime;

    int returnCode = 0;
    HANDLE hProcess;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD clientExitCode = -1;
    char lpCommandLine[MAX_PATH] = "select_client";

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize the readable socket set*/
    FD_ZERO(&readFds);

    // Wait for 5 seconds for the client to connect.
    waitTime.tv_sec = 5L;
    waitTime.tv_usec = 0L;
    
    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested, &WsaData);
    if(err != 0)
    {
        Fail("\nFailed to find a usable WinSock DLL!\n");
    }

    /* Confirm that the WinSock DLL supports 2.2.*/
    if(LOBYTE( WsaData.wVersion ) != 2 ||
            HIBYTE( WsaData.wVersion ) != 2)
    {
        Trace("\nFailed to find a usable WinSock DLL!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*create sa stream socket in AF_INET domain*/
    socketID = socket(AF_INET, SOCK_STREAM, 0);

    if(INVALID_SOCKET == socketID)
    {
        Trace("\nFailed to call socket API to create a stream socket!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*prepare the sockaddr_in structure*/
    mySockaddr.sin_family = AF_INET;
    mySockaddr.sin_port = getRotorTestPort();
    mySockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    memset( &(mySockaddr.sin_zero), 0, 8);

    /*bind the local address to the created socket*/
    err = bind(socketID,(struct sockaddr *)&mySockaddr,
            sizeof(struct sockaddr));
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call bind API for positive test!\n");
        err = closesocket(socketID);
        if(SOCKET_ERROR == err)
        {    
            Trace("\nFailed to call closesocket API!\n");
        }
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*to setup the backlog number for a created socket*/
    err = listen(socketID, nBacklogNumber);
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call listen API to set the backlog number!\n");
        err = closesocket(socketID);
        if(SOCKET_ERROR == err)
        {    
            Trace("\nFailed to call closesocket API!\n");
        }
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

   /* Zero the data structure space */
    ZeroMemory ( &pi, sizeof(pi) );
    ZeroMemory ( &si, sizeof(si) );

    /* Set the process flags and standard io handles */
    si.cb = sizeof(si);
    
    //Create Process
    if(!CreateProcess( NULL, /* lpApplicationName*/
                        lpCommandLine, /* lpCommandLine */
                        NULL, /* lpProcessAttributes  */
                        NULL, /* lpThreadAttributes */
                        TRUE, /* bInheritHandles */
                        0, /* dwCreationFlags, */
                        NULL, /* lpEnvironment  */
                        NULL, /* pCurrentDirectory  */
                        &si, /* lpStartupInfo  */
                        &pi /* lpProcessInformation  */
                        ))
    {
        Trace("Error:%d: CreateProcess Failed\n", GetLastError());
       
        err = closesocket(socketID);
        if(SOCKET_ERROR == err)
        {    
            Trace("\nFailed to call closesocket API!\n");
        }
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }
    else
    {
        hProcess = pi.hProcess;
      //  Trace("Process created for [%d]\n", i);
    }

    /*add a socket to readabel socket set*/
    FD_SET(socketID,&readFds);

    /*monitor the readable socket set,blocking operation*/
    socketFds = select(0, &readFds, NULL, NULL, &waitTime);

    /*only check if find the the readable socket, */
    /*do not read the data from the socket*/
    if(SOCKET_ERROR == socketFds)
    {
        Trace("\nFailed to call select API to monitor the "
            "readable socket set!\n");
        err = closesocket(socketID);
        if(SOCKET_ERROR == err)
        {    
            Trace("\nFailed to call closesocket API!\n");
        }
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }
    /*waiting time is out*/
    if(0 == socketFds)
    {
        Trace("\nSelect API waiting time is out!\n");
        err = closesocket(socketID);
        if(SOCKET_ERROR == err)
        {    
            Trace("\nFailed to call closesocket API!\n");
        }
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }
 
    err = closesocket(socketID);
    if(SOCKET_ERROR == err)
    {
        Trace("\nFailed to call closesocket API!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API!\n");
    }

    returnCode = WaitForSingleObject( hProcess, TIMEOUT);  
    if( WAIT_OBJECT_0  != returnCode )
    {
        Fail("Error:%d: WFSO on Client Process failed\n", returnCode, GetLastError());
    }

    if(!GetExitCodeProcess( hProcess, &clientExitCode))
    {
        Fail("Error:%d: Failed to get exit code for the Client Process\n", GetLastError());
    }

    if(clientExitCode != PASS)
    {
        Fail("Error:%d: Client exit code is %d and expected PASS\n", clientExitCode);
    }

    if(!CloseHandle(pi.hThread))
    {
        Fail("Error:%d: CloseHandle on Process thread handle failed\n", GetLastError());
    }

    if(!CloseHandle(pi.hProcess))
    {
        Fail("Error:%d: CloseHandle on Process handle failed\n", GetLastError());
    }

    PAL_Terminate();
    return PASS;
}
