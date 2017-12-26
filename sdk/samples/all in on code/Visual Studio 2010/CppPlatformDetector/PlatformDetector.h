/****************************** Module Header ******************************\
Module Name:    PlatformDetector.h
Project:        CppPlatformDetector
Copyright (c) Microsoft Corporation.

Declares the helper functions for detecting the platform. 

GetOSName - Gets the name of the currently running operating system. For 
            example, "Microsoft Windows 7 Enterprise".
GetOSVersionString - Gets the concatenated string representation of the 
            platform identifier, version, and service pack that are currently 
            installed on the operating system. 
Is64BitOS - Determines whether the current operating system is a 64-bit 
            operating system.
Is64BitProcess - Determines whether the currently running process or an 
            arbitrary process running on the system is a 64-bit process.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once

#include <Windows.h>


//
//   FUNCTION: GetOSName(PWSTR, DWORD)
//
//   PURPOSE: Gets the name of the currently running operating system. For 
//   example, "Microsoft Windows 7 Enterprise".
//
//   PARAMETERS:
//   * pszName - The buffer for receiving the OS name. 
//   * cch - The size of buffer pointed by pszName, in characters.
//
//   RETURN VALUE: The function returns TRUE if it succeeds.
//
BOOL GetOSName(PWSTR pszName, DWORD cch);


//
//   FUNCTION: GetOSVersionString(PWSTR, DWORD)
//
//   PURPOSE: Gets the concatenated string representation of the platform 
//   identifier, version, and service pack that are currently installed on 
//   the operating system. For example, 
//   "Microsoft Windows NT 6.1.7600.0 Workstation"
//
//   PARAMETERS:
//   * pszVersionString - The buffer for receiving the OS version string. 
//   * cch - The size of buffer pointed by pszVersionString, in characters.
//
//   RETURN VALUE: The function returns TRUE if it succeeds.
//
BOOL GetOSVersionString(PWSTR pszVersionString, DWORD cch);


//
//   FUNCTION: Is64BitOS()
//
//   PURPOSE: Determines whether the current operating system is a 64-bit 
//   operating system.
//
//   RETURN VALUE: The function returns TRUE if the operating system is 
//   64-bit; otherwise, it returns FALSE.
//
BOOL Is64BitOS();


//
//   FUNCTION: Is64BitProcess(void)
//   
//   PURPOSE: Determines whether the currently running process is a 64-bit 
//   process.
//
//   RETURN VALUE: The function returns TRUE if the currently running process 
//   is 64-bit; otherwise, it returns FALSE.
//
BOOL Is64BitProcess(void);


//
//   FUNCTION: Is64BitProcess(HANDLE)
//   
//   PURPOSE: Determines whether the specified process is a 64-bit process.
//
//   PARAMETERS:
//   * hProcess - the process handle.
//
//   RETURN VALUE: The function returns TRUE if the given process is 64bit;
//   otherwise, it returns FALSE.
//
BOOL Is64BitProcess(HANDLE hProcess);