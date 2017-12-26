/****************************** Module Header ******************************\
Module Name:  Deadlock.cpp
Project:      CppApplicationHang
Copyright (c) Microsoft Corporation.

Deadlock refers to a specific condition when two or more processes are each 
waiting for each other to release a resource, or more than two processes 
are waiting for resources in a circular chain. Because none of threads are 
willing to release their protected resources, what ultimately happens is 
that none of the threads will ever make any progress. They simply sit there 
and wait for the others to make a move, and a deadlock ensues.

In this sample, two threads are running TriggerDeadlock and 
DeadlockThreadProc respectively. The thread that runs TriggerDeadlock 
acquires resource2 first, and waits for resource1. The thread running 
DeadlockThreadProc acquires resource1, and waits for the leave of resource2 
in the first thread. Neither thread is willing to release its protected 
resource first, so a deadlock occurs.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "StdAfx.h"
#include "Deadlock.h"


CRITICAL_SECTION g_cs1;  // Guard resrouce1
CRITICAL_SECTION g_cs2;  // Guard resource2


DWORD WINAPI DeadlockThreadProc(LPVOID lpParam)
{
	// Acquire resource1 
	EnterCriticalSection(&g_cs1);

	// Do work with resource1
	// ...
	Sleep(3000);

	// Acquire resource2
	EnterCriticalSection(&g_cs2);

	// Do work with resource2
	// ...

	LeaveCriticalSection(&g_cs2);
	LeaveCriticalSection(&g_cs1);

	return 1;
}

void TriggerDeadlock()
{
	// Initialize the two critical sections
	InitializeCriticalSection(&g_cs1);
	InitializeCriticalSection(&g_cs2);

	// Create the second thread
	DWORD dwId = 0;
	HANDLE hThread = CreateThread(NULL, 0, DeadlockThreadProc, NULL, 0, &dwId);
	if (hThread)
	{
		// Acquire resource2
		EnterCriticalSection(&g_cs2);

		// Do work with resource2
		// ...
		Sleep(2000);

		// Acquire resource1
		EnterCriticalSection(&g_cs1);

		// Do work with resource1
		// ...

		LeaveCriticalSection(&g_cs1);
		LeaveCriticalSection(&g_cs2);

		// Wait for the second thread to finish
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}

	// Delete the two critical sections
	DeleteCriticalSection(&g_cs1);
	DeleteCriticalSection(&g_cs2);
}