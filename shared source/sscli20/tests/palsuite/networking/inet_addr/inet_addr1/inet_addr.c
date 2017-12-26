/*=============================================================
**
** Source: inet_addr.c
**
** Purpose: Positively test inet_addr API to convert a valid dotted 
**          internet address to a unsigned long integer in network
**          byte order.
**          Test with Decimal, Octal and Hexadecimal format. 
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
    /*127.0.0.1 in network order*/
    unsigned long  networkAddress_4 = VAL32(0x0100007F);
    /*127.0.0 in network order*/
    unsigned long  networkAddress_3 = VAL32(0x00007F);
    /*127.0 in network order*/
    unsigned long  networkAddress_2 = VAL32(0x007F);
    /*127 in network order*/
    unsigned long  networkAddress_1 = VAL32(0x7F000000);
    unsigned long ulIpAddress;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /*initialize to use winsock2.dll*/
    err = WSAStartup(VersionRequested,&WsaData);
    if(err != 0)
    {
        Fail("\nFailed to find a usable WinSock DLL, error code=%d!\n",
                GetLastError());
    }

    /*Confirm that the WinSock DLL supports 2.2.*/  
    if(LOBYTE( WsaData.wVersion ) != 2 ||
            HIBYTE( WsaData.wVersion ) != 2)
    {
        /*we could not find a usable WinSock DLL.*/
        Trace("\nFailed to find a usable WinSock DLL!, error code=%d\n",
                GetLastError());
        err = WSACleanup();
        if(SOCKET_ERROR == err)
        {
            Trace("\nFailed to call WSACleanup API!, error code=%d\n",
                GetLastError());
        }
        Fail("");
    }

    /***
        test in decimal format
    ***/

    /*convert four parts decimal dotted IP address string 
     *to an unsigned long integer in network byte order
    */
    ulIpAddress = inet_addr("127.0.0.1");
    if(INADDR_NONE == ulIpAddress || networkAddress_4 != ulIpAddress  )
    {
        Fail("\nFailed to call inet_addr API to convert a four "
            "parts decimal dotted IP address to an unsigned long integer "
            "in network byte order!, error code=%d\n", GetLastError());
    }

    /*convert three parts decimal dotted IP address string 
     *to an unsigned long integer in network byte order
    */
    ulIpAddress = inet_addr("127.0.0");
    if(INADDR_NONE == ulIpAddress || networkAddress_3 != ulIpAddress  )
    {
        Fail("\nFailed to call inet_addr API to convert a three "
            "parts decimal dotted IP address to an unsigned long integer "
            "in network byte order!, error code=%d\n", GetLastError());
    }

    /*convert two parts decimal dotted IP address string 
     *to an unsigned long integer in network byte order
    */
    ulIpAddress = inet_addr("127.0");
    if(INADDR_NONE == ulIpAddress || networkAddress_2 != ulIpAddress  )
    {
        Fail("\nFailed to call inet_addr API to convert a two "
            "parts decimal dotted IP address to an unsigned long integer "
            "in network byte order!, error code=%d\n", GetLastError());
    }

    /*convert one part decimal dotted IP address string 
     *to an unsigned long integer in network byte order
    */
    ulIpAddress = inet_addr("127");
    if(INADDR_NONE == ulIpAddress || networkAddress_1 != ulIpAddress  )
    {
        Fail("\nFailed to call inet_addr API to convert a one "
            "part decimal dotted IP address to an unsigned long integer "
            "in network byte order!, error code=%d\n", GetLastError());
    }

    /***
        test in octal format
    ***/
    /*convert four parts octal dotted IP address string 
     *to an unsigned long integer in network byte order
    */
    ulIpAddress = inet_addr("0177.00.00.01");
    if(INADDR_NONE == ulIpAddress || networkAddress_4 != ulIpAddress  )
    {
        Fail("\nFailed to call inet_addr API to convert a four "
            "parts octal dotted IP address to an unsigned long integer "
            "in network byte order!, error code=%d\n", GetLastError());
    }

    /*convert three parts octal dotted IP address string 
     *to an unsigned long integer in network byte order
    */
    ulIpAddress = inet_addr("0177.00.00");
    if(INADDR_NONE == ulIpAddress || networkAddress_3 != ulIpAddress  )
    {
        Fail("\nFailed to call inet_addr API to convert a three "
            "parts octal dotted IP address to an unsigned long integer "
            "in network byte order!, error code=%d\n", GetLastError());
    }

    /*convert two parts octal dotted IP address string 
     *to an unsigned long integer in network byte order
    */
    ulIpAddress = inet_addr("0177.00");
    if(INADDR_NONE == ulIpAddress || networkAddress_2 != ulIpAddress  )
    {
        Fail("\nFailed to call inet_addr API to convert a two "
            "parts octal dotted IP address to an unsigned long integer "
            "in network byte order!, error code=%d\n", GetLastError());
    }

    /*convert one part octal dotted IP address string 
     *to an unsigned long integer in network byte order
    */
    ulIpAddress = inet_addr("0177");
    if(INADDR_NONE == ulIpAddress || networkAddress_1 != ulIpAddress  )
    {
        Fail("\nFailed to call inet_addr API to convert a one "
            "part octal dotted IP address to an unsigned long integer "
            "in network byte order!, error code=%d\n", GetLastError());
    }


    /***
        test in hexadecimal format
    ***/
    /*convert four parts hexadecimal dotted IP address string 
     *to an unsigned long integer in network byte order
    */
    ulIpAddress = inet_addr("0x7F.0x00.0x00.0x01");
    if(INADDR_NONE == ulIpAddress || networkAddress_4 != ulIpAddress  )
    {
        Fail("\nFailed to call inet_addr API to convert a four parts"
            "hexadecimal dotted IP address to an unsigned long integer "
            "in network byte order!, error code=%d\n", GetLastError());
    }

    /*convert three parts hexadecimal dotted IP address string 
     *to an unsigned long integer in network byte order
    */
    ulIpAddress = inet_addr("0x7F.0x00.0x00");
    if(INADDR_NONE == ulIpAddress || networkAddress_3 != ulIpAddress  )
    {
        Fail("\nFailed to call inet_addr API to convert a three parts"
            "hexadecimal dotted IP address to an unsigned long integer "
            "in network byte order!, error code=%d\n", GetLastError());
    }

    /*convert two parts hexadecimal dotted IP address string 
     *to an unsigned long integer in network byte order
    */
    ulIpAddress = inet_addr("0x7F.0x00");
    if(INADDR_NONE == ulIpAddress || networkAddress_2 != ulIpAddress  )
    {
        Fail("\nFailed to call inet_addr API to convert a two parts"
            "hexadecimal dotted IP address to an unsigned long integer "
            "in network byte order!, error code=%d\n", GetLastError());
    }

    /*convert one part hexadecimal dotted IP address string 
     *to an unsigned long integer in network byte order
    */
    ulIpAddress = inet_addr("0x7F");
    if(INADDR_NONE == ulIpAddress || networkAddress_1 != ulIpAddress  )
    {
        Fail("\nFailed to call inet_addr API to convert a one part"
            "hexadecimal dotted IP address to an unsigned long integer "
            "in network byte order!, error code=%d\n", GetLastError());
    }

    err = WSACleanup();
    if(SOCKET_ERROR == err)
    {
        Fail("\nFailed to call WSACleanup API!, Error code=%d\n",
                GetLastError());
    }

    PAL_Terminate();
    return PASS;
}

