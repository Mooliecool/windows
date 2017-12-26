/****************************** Module Header ******************************\
Module Name:    CppBasics.cpp
Project:        CppBasics
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <stdio.h>

extern void TypeCasting(void);
extern void FunctionPointer(void);
extern void Array(void);


int wmain(int argc, wchar_t *argv[])
{
	TypeCasting();

	FunctionPointer();

	Array();

	return 0;
}