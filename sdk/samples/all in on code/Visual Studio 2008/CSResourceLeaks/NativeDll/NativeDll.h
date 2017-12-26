/****************************** Module Header ******************************\
Module Name:  NativeDll.h
Project:      CSResourceLeaks.NativeDll
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

// The following ifdef block is the standard way of creating macros which 
// make exporting from a DLL simpler. All files within this DLL are compiled 
// with the NATIVEDLL_EXPORTS symbol defined on the command line. This symbol 
// should not be defined on any project that uses this DLL. This way any 
// other project whose source files include this file see NATIVEDLL_API 
// functions as being imported from a DLL, whereas this DLL sees symbols 
// defined with this macro as being exported.

#ifdef NATIVEDLL_EXPORTS
#define NATIVEDLL_API __declspec(dllexport)
#else
#define NATIVEDLL_API __declspec(dllimport)
#endif


EXTERN_C NATIVEDLL_API void __cdecl GetDateTimeString(PWSTR, DWORD);