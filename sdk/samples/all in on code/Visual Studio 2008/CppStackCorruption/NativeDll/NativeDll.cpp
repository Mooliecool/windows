/****************************** Module Header ******************************\
Module Name:  NativeDll.cpp
Project:      CppStackCorruption.NativeDll
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


NATIVEDLL_API int __stdcall GetStringLength(PCWSTR pszString)
{
    return wcslen(pszString);
}