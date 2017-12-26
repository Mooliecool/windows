/****************************** Module Header ******************************\
Module Name:  stdafx.cpp
Project:      CppHostCLR
Copyright (c) Microsoft Corporation.

Implements the helper functions that are frequently used in the project.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "stdafx.h"


BOOL IsModuleLoaded(PCWSTR pszModuleName)
{
	// Get the module in the process according to the module name.
	HMODULE hMod = GetModuleHandle(pszModuleName);
    return (hMod != NULL);
}