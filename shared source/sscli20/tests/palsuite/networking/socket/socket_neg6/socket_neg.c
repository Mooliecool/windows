/*=============================================================
**
** Source: socket_neg.c
**
** Purpose: Negatively test socket API by passing invalid 
**          combinations of address family, socket type and 
**          protocol.
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
    WORD VersionRequested = MAKEWORD(2, 2);
    WSADATA WsaData;
    int SocketID;
    int err;

    int i;
    int j;
    int k;
    char errBuffer[30000];
    char errMsg[100];

    u_long error;
    BOOL FailFlag = FALSE;
    int socketType[5] ={SOCK_STREAM, 
                        SOCK_DGRAM, 
                        SOCK_RAW,
                        SOCK_RDM, 
                        SOCK_SEQPACKET};

    char *pSocketType[5]={"SOCK_STREAM", 
                       "SOCK_DGRAM",    
                       "SOCK_RAW", 
                       "SOCK_RDM", 
                       "SOCK_SEQPACKET"};

    int AF[29]= {AF_UNIX,
                    AF_INET,
                    AF_IMPLINK,
                    AF_PUP,
                    AF_CHAOS,
                    AF_NS,
                    AF_IPX,
                    AF_ISO,
                    AF_OSI,
                    AF_ECMA,
                    AF_DATAKIT,
                    AF_CCITT,
                    AF_SNA,
                    AF_DECnet,
                    AF_DLI,
                    AF_LAT,
                    AF_HYLINK,
                    AF_APPLETALK,
                    AF_NETBIOS,
                    AF_VOICEVIEW,
                    AF_FIREFOX,
                    AF_UNKNOWN1,
                    AF_BAN,
                    AF_ATM,
                    AF_INET6,
                    AF_CLUSTER,
                    AF_12844,
                    AF_IRDA,
                    AF_NETDES};

    char *pAF[29]= {"AF_UNIX",
                    "AF_INET",
                    "AF_IMPLINK",
                    "AF_PUP",
                    "AF_CHAOS",
                    "AF_NS",
                    "AF_IPX",
                    "AF_ISO",
                    "AF_OSI",
                    "AF_ECMA",
                    "AF_DATAKIT",
                    "AF_CCITT",
                    "AF_SNA",
                    "AF_DECnet",
                    "AF_DLI",
                    "AF_LAT",
                    "AF_HYLINK",
                    "AF_APPLETALK",
                    "AF_NETBIOS",
                    "AF_VOICEVIEW",
                    "AF_FIREFOX",
                    "AF_UNKNOWN1",
                    "AF_BAN",
                    "AF_ATM",
                    "AF_INET6",
                    "AF_CLUSTER",
                    "AF_12844",
                    "AF_IRDA",
                    "AF_NETDES"};

    int prot[9]={IPPROTO_ICMP,
                    IPPROTO_IGMP,
                    IPPROTO_GGP,
                    IPPROTO_TCP,
                    IPPROTO_PUP,
                    IPPROTO_UDP,
                    IPPROTO_IDP,
                    IPPROTO_ND,
                    IPPROTO_RAW};
                    
     char *pProt[9]={"IPPROTO_ICMP",
                    "IPPROTO_IGMP",
                    "IPPROTO_GGP",
                    "IPPROTO_TCP",
                    "IPPROTO_PUP",
                    "IPPROTO_UDP",
                    "IPPROTO_IDP",
                    "IPPROTO_ND",
                    "IPPROTO_RAW"};

    /*Initialize the PAL environment*/
    err  = PAL_Initialize(argc,argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested, &WsaData);
    if (0 != err)
    {
        Fail("\nFailed to find a usable WinSock DLL!\n");
    }

    /*Confirm that the WinSock DLL supports 2.2.*/
    if ( LOBYTE( WsaData.wVersion ) != 2 ||
            HIBYTE( WsaData.wVersion ) != 2 )
    {
        Trace("\nFailed to find a usable WinSock DLL!\n");
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!\n");
        }
        Fail("");
    }

    /*initialize the error message buffer*/
    memset(errBuffer, 0, 20000);

    for(i=0; i<29; i++)
    {
        for(j=0; j<5; j++)
        {
            for(k = 0; k<9; k++)
            {
                SocketID = socket(AF[i], socketType[j], prot[k]);
                error = GetLastError();
                if(!(WSAEAFNOSUPPORT == error || WSAEPROTONOSUPPORT == error)
                    && INVALID_SOCKET != SocketID)
                {
                    /*
                    *some valid combinations are not 
                    *written to the error buffer
                    */
                    if(!((AF[i] == AF_INET && 
                        socketType[j] == SOCK_STREAM &&
                        prot[k] == IPPROTO_TCP) ||
                        (AF[i] == AF_INET && 
                        socketType[j] == SOCK_DGRAM &&
                        prot[k] == IPPROTO_UDP) ||
                        (AF[i] == AF_INET && socketType[j] == SOCK_RAW))
                       )
                    {
                        strcat(errBuffer, pAF[i]);
                        strcat(errBuffer, ", ");
                        strcat(errBuffer, pSocketType[j]);
                        strcat(errBuffer, ", ");
                        strcat(errBuffer, pProt[k]);
                        
                        memset(errMsg, 0, 100);
                        /*add error code to error buffer*/
                        sprintf(errMsg, "%s%u", "error code=", 
                                error); 
                        strcat(errBuffer, ", ");
                        strcat(errBuffer, errMsg);
                        strcat(errBuffer, "; ");
                        FailFlag = TRUE;

                        if(INVALID_SOCKET != SocketID)
                        {
                            err = closesocket(SocketID);
                            if(SOCKET_ERROR == err)
                            {
                                Fail("\nFailed to call closesocket API!\n");
                            }        
                        }
                    }
                }
            }
        }
    }

    /*terminate use of WinSock DLL*/
    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API!\n");
    }
    
    if(FailFlag)
    {
        Fail("\nFailed to call socket API for a negative test, "
            "the following combination of Address family, socket type "
            "and protocol are expected to generate errors, but no "
            "error or no expected error is gengrated, the combination are: "
            " %s \n", errBuffer);
    }


    PAL_Terminate();
    return PASS;
}
