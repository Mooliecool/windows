/****************************** Module Header ******************************\
Module Name:  dllmain.cpp
Project:      CppDynamicLinkLibrary
Copyright (c) Microsoft Corporation.

The file defines the DllMain callback function, which is an optional entry 
point into a dynamic-link library (DLL). When the system starts or 
terminates a process or thread, it calls the entry-point function for each 
loaded DLL using the first thread of the process. The system also calls the 
entry-point function for a DLL when it is loaded or unloaded using the 
LoadLibrary and FreeLibrary functions.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include <windows.h>


BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

