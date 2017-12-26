/****************************** Module Header ******************************\
Module Name:  OrphanedCS.cpp
Project:      CppApplicationHang
Copyright (c) Microsoft Corporation.

Examination of the critical section in question leads to a thread ID that 
does not exist. This is referred to as an orphaned critical section. 

Here, we demonstrate two typical scenarios of orphaned critical section:

Orphaned Critical Section Scenario 1:
An exception filter catches an exception during execution of code 
(typically it is some third party code written for RPC or COM server) that 
is holding a critical section.

Orphaned Critical Section Scenraio 2:
The main thread calls TerminateThread to terminate a thread that owns a 
critical section.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "StdAfx.h"
#include "OrphanedCS.h"


CRITICAL_SECTION g_cs;


/////////////////////////////////////////////////////////////////////////////
// Orphaned Critical Section Scenario 1:
// An exception filter catches an exception during execution of code 
// (typically it is some third party code written for RPC or COM server) that 
// is holding a critical section.
// 

//
//  FUNCTION: OrphanedCSThreadProc1(LPVOID)
//
//  PURPOSE: The worker thread calls into some third-party code under the 
//           protection of the global critical section. The code also makes 
//           an attempt at being exception safe by wrapping the call with a 
//           try/catch statement, attempting to catch all exceptions thrown, 
//           and then it returns a failure code if an exception is thrown.
//
DWORD WINAPI OrphanedCSThreadProc1(LPVOID lpParam)
{
	try
	{
		EnterCriticalSection(&g_cs);
		
		// Do work with the protected resource
		// ...
		// Call some third-party code that may throw an exception
		// ...
		throw "InvalidParameter";

		LeaveCriticalSection(&g_cs);
	}
	catch (...)
	{
		// Error occured
		return 0;
	}
	return 1;
}


//
//  FUNCTION: TriggerOrphanedCS1()
//
//  PURPOSE: The main thread starts by initializing a global critical section 
//           followed by the creation of a new thread. After the thread has 
//           been successfully created, it then tries to enter the global 
//           critical section to perform some work. When done, it leaves the 
//           critical section and waits for the worker thread to finish.
// 
void TriggerOrphanedCS1()
{
	InitializeCriticalSection(&g_cs);

	DWORD dwId = 0;
	HANDLE hThread = CreateThread(NULL, 0, OrphanedCSThreadProc1, NULL, 0, &dwId);
	if (hThread)
	{
		// Do some work
		// ...
		Sleep(1000);

		EnterCriticalSection(&g_cs);

		// Do work with the protected resource
		// ...

		LeaveCriticalSection(&g_cs);

		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}

	DeleteCriticalSection(&g_cs);
}


/////////////////////////////////////////////////////////////////////////////
// Orphaned Critical Section Scenario 2: 
// The main thread calls TerminateThread to terminate a thread that owns a 
// critical section. 
// 
// TerminateThread is a dangerous function that should only be used in the 
// most extreme cases. You should call TerminateThread only if you know 
// exactly what the target thread is doing, and you control all of the code 
// that the target thread could possibly be running at the time of the 
// termination. For example, TerminateThread can result in the following 
// problems:
// 
// 1) If the target thread owns a critical section, the critical section will 
// not be released. 
// 2) If the target thread is allocating memory from the heap, the heap lock 
// will not be released. 
// 3) If the target thread is executing certain kernel32 calls when it is 
// terminated, the kernel32 state for the thread's process could be 
// inconsistent. 
// 4) If the target thread is manipulating the global state of a shared DLL, 
// the state of the DLL could be destroyed, affecting other users of the DLL. 
// 

//
//  FUNCTION: OrphanedCSThreadProc2(LPVOID)
//
//  PURPOSE: The worker thread acquires the critical section, do some work 
//           with the protected resource, and leave the critical section. 
// 
DWORD WINAPI OrphanedCSThreadProc2(LPVOID lpParam)
{
	EnterCriticalSection(&g_cs);
		
	// Do work with the protected resource
	// ...
	Sleep(3000);

	LeaveCriticalSection(&g_cs);

	return 1;
}


//
//  FUNCTION: TriggerOrphanedCS2()
//
//  PURPOSE: The main thread starts by initializing a global critical section 
//           followed by the creation of a new thread. Next, the main thread 
//           performs some work and terminates the second thread. It then 
//           tries to enter the global critical section. When done, it leaves 
//           the critical section and waits for the worker thread to finish.
// 
void TriggerOrphanedCS2()
{
	InitializeCriticalSection(&g_cs);

	DWORD dwId = 0;
	HANDLE hThread = CreateThread(NULL, 0, OrphanedCSThreadProc2, NULL, 0, &dwId);
	if (hThread)
	{
		// Do some work
		// ...
		Sleep(1000);

		// Terminate the second thread
		TerminateThread(hThread, 0);

		EnterCriticalSection(&g_cs);

		// Do work with the protected resource
		// ...

		LeaveCriticalSection(&g_cs);

		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}

	DeleteCriticalSection(&g_cs);
}