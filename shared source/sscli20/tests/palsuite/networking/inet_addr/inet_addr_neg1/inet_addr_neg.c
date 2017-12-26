/*=============================================================
**
** Source: inet_addr_neg.c
**
** Purpose: Negative test inet_addr API to convert an invalid dotted 
**          internet address to a unsigned long integer 
**          test with decimal, octal and hexadecimal format
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
    int err;
    const char *InvalidHostAddress_decimal = "127.0.0.265";
    const char *InvalidHostAddress_octal = "0177.00.00.0411";
    const char *InvalidHostAddress_hexadecimal = "0x7F.0x0.0x0.0x109";
    unsigned long ulIpAddress;

    /*Initialize the PAL environment*/
    err = PAL_Initialize(argc, argv);
    if(0 != err)
    {
        return FAIL;
    }

    /***
        negative test the inet_addr API, 
        passing an invalid decimal format IP address
    ***/
    err = inet_addr(InvalidHostAddress_decimal);
    if(INADDR_NONE != err)
    { 
        Fail("\nFailed to call inet_addr API for negative test "
            "by passing invalid decimal format IP address, "
            "an error is expected, but no error or no expected erorr "
            "is detected, error code=%s\n", GetLastError());
    }

    /***
        negative test the inet_addr API, 
        passing an invalid octal format IP address
    ***/
    err = inet_addr(InvalidHostAddress_octal);
    if(INADDR_NONE != err)
    { 
        Fail("\nFailed to call inet_addr API for negative test "
            "by passing invalid octal format IP address, "
            "an error is expected, but no error or no expected erorr "
            "is detected, error code=%s\n", GetLastError());
    }

    /***
        negative test the inet_addr API, 
        passing an invalid hexadecimal format IP address
    ***/
    err = inet_addr(InvalidHostAddress_hexadecimal);
    if(INADDR_NONE != err)
    { 
        Fail("\nFailed to call inet_addr API for negative test "
            "by passing invalid hexadecimal format IP address, "
            "an error is expected, but no error or no expected erorr "
            "is detected, error code=%s\n", GetLastError());
    }

    
    /*test inet_addr by passing a space as a dotted IP address*/
    ulIpAddress = inet_addr(" ");
    if(0 != ulIpAddress)
    {
        Fail("\nFailed to call inet_addr API by passing a space " 
            "as a dotted IP address, a zero value is expected, "
            "but no zero value returns, error code=%d\n", GetLastError());
    }

    PAL_Terminate();
    return PASS;
}
