/****************************** Module Header ******************************\
Module Name:  NativeDll.cpp
Project:      CSResourceLeaks.NativeDll
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <stdio.h>
#include <windows.h>
#include "NativeDll.h"


// 
NATIVEDLL_API void __cdecl GetDateTimeString(PWSTR pszTime, DWORD cchTime)
{
    SYSTEMTIME time;
    GetSystemTime(&time);

    const int MAX_LENGTH = 512;
    wchar_t *pszTmpTime = new wchar_t[MAX_LENGTH];
    swprintf_s(pszTmpTime, 
        MAX_LENGTH, 
        L"%d-%d-%d %d:%d:%d", 
        time.wYear, 
        time.wMonth, 
        time.wDay, 
        time.wHour, 
        time.wMinute, 
        time.wSecond);
    wcscpy_s(pszTime, cchTime, pszTmpTime);
}