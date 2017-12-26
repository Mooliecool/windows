/****************************** Module Header ******************************\
* Module Name:  CppLibFuncs.cpp
* Project:      CppStaticLibrary
* Copyright (c) Microsoft Corporation.
* 
* The implementation of the functions in the static library.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "CppLibFuncs.h"


int /*__cdecl*/ GetStringLength1(PCWSTR pszString)
{
	return wcslen(pszString);
}


int __stdcall GetStringLength2(PCWSTR pszString)
{
	return wcslen(pszString);
}