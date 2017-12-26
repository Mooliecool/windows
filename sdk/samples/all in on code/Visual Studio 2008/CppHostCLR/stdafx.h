/****************************** Module Header ******************************\
Module Name:  stdafx.h
Project:      CppHostCLR
Copyright (c) Microsoft Corporation.

Include file for standard system include files, or project specific include 
files that are used frequently, but are changed infrequently. You can also 
declare helper functions that are frequently used in the project.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once

#include <stdio.h>
#include <windows.h>


//
//   FUNCTION: IsModuleLoaded(PCWSTR)
//
//   PURPOSE: Check whether or not the specified module is loaded in the 
//   current process.
//
//   PARAMETERS:
//   * pszModuleName - the module name
//
//   RETURN VALUE: The function returns TRUE if the specified module is 
//   loaded in the current process. If the module is not loaded, the function 
//   returns FALSE.
//
BOOL IsModuleLoaded(PCWSTR pszModuleName);