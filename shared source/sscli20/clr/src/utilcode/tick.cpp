// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
//*****************************************************************************
//  tick.cpp
//
//  This contains a bunch of C++ utility classes.
//
//*****************************************************************************

#include "stdafx.h"

#include "ostype.h"

#define MAXULONGLONG                     UI64(0xffffffffffffffff)
#define MAXULONG    0xffffffff  // winnt

static ULONGLONG TickCountOffset = MAXULONGLONG;


ULONGLONG CLRGetTickCount64()
{


    DWORD tick1, tick2;
    ULONGLONG fileTime1, fileTime2;
    do
    {
        FILETIME fileTime;
        GetSystemTimeAsFileTime(&fileTime);
        fileTime1 = (((INT64) fileTime.dwHighDateTime << 32) + fileTime.dwLowDateTime)/10000;  // convert to millisecond
        tick1 = GetTickCount();
        GetSystemTimeAsFileTime(&fileTime);
        fileTime2 = (((INT64) fileTime.dwHighDateTime << 32) + fileTime.dwLowDateTime)/10000;  // convert to millisecond
        tick2 = GetTickCount();
    } while ( tick1 > tick2 || (fileTime1/MAXULONG) != (fileTime2/MAXULONG) );  // To handle wrapping in either counter

    if (TickCountOffset == MAXULONGLONG)
    {
        TickCountOffset = (ULONGLONG)(int)(DWORD)((fileTime1%MAXULONG) - tick1);
    }
    
    return (ULONGLONG)tick1 + TickCountOffset + (fileTime1/MAXULONG)*MAXULONG;
}


