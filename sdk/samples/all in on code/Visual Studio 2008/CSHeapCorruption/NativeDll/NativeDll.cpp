/****************************** Module Header ******************************\
Module Name:  NativeDll.cpp
Project:      CSHeapCorruption.NativeDll
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


NATIVEDLL_API void __stdcall InitializeBuffer(int buffer[], DWORD dwSize)
{
    for (DWORD i = 0; i <= 2 * dwSize; i++)
    {
        buffer[i] = i;
    }
}


NATIVEDLL_API PVOID __stdcall AllocateMemory(DWORD dwBytes)
{
    // Allocate a block of memory on the CRT heap.
    return new BYTE[dwBytes];
}