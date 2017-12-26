/*=============================================================
**
** Source: gethostbyname.c
**
** Purpose: Positive test gethostbyname API against a verified 
**          host and dns server
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


int __cdecl main(int argc, char *argv[])
{
    WORD VersionRequested = MAKEWORD(2,2);
    WSADATA WsaData;
    int err;

    struct hostent *pMyHostent;
    char *pHostName = NULL;

    BOOL errorFlag = FALSE;
    int i = 0;
    int strlength = 0;
    char ch;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested, &WsaData);
    if ( err != 0 )
    {
        Fail("\nFailed to find a usable WinSock DLL!\n");
    }

    /*Confirm that the WinSock DLL supports 2.2.*/  
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

    pHostName = (char *)getRotorTestHost();
    if(NULL == pHostName)
    {
        Trace("\nFailed to read server name!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    if(0 == strlen(pHostName))
    {
        Trace("\nFailed to read server name!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*retrieve the host info by host name*/
    pMyHostent = gethostbyname(pHostName);

    if(NULL == pMyHostent)
    {
        Trace("\nFailed to call gethostbyname API!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*
    *further check the retrieved hostent struct contains host name  
    *with legal alphanumeric, dot, underscore and hyphan characters
    */ 
    strlength = (int)strlen(pMyHostent->h_name); 

    for(i = 0; i < strlength; i++)
    {
        ch = pMyHostent->h_name[i];
        if(!(ch == '.' || ch == '-' || (ch <= 'z' && ch >= 'a') ||
            (ch <= 'Z' && ch >= 'A') || (ch <='9' && ch >= '0') ||
            (ch == '_')))
        {
            errorFlag = TRUE;
        }
    }

    if(errorFlag)
    {
        Trace("\nFailed to call gethostbyname API to retrieve host info, "
                "the host name in hostent struct contains invalid characters "
                "the host name is = %s\n", pMyHostent->h_name);
               
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

    PAL_Terminate();
    return 0;
}

char * readServerName(const char *fileName)
{
    FILE *fileID;
    char *temp;

    temp = malloc(1024 * sizeof(char));
    if(NULL == temp)
    {
        Trace("\nFailed to allocate memory to store the server name!\n");
        return NULL;
    }

    fileID = fopen(fileName, "r");
    if(!fileID)
    {
        free(temp);
        Trace("\nFailed to open file for reading!\n");
        return "";
    }
    fgets(temp, 1024, fileID);
    fclose(fileID);
    strtok(temp,"\n");
    return temp;
}
