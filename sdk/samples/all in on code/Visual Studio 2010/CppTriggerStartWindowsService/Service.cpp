/****************************** Module Header ******************************\
* Module Name:  Service.cpp
* Project:      CppTriggerStartWindowsService
* Copyright (c) Microsoft Corporation.
* 
* The file implements the skeleton of the Windows service. These functions 
* can be reused by all Windows services:
* 
*   SvcMain            Entry point for the service
*   SvcCtrlHandler     Service control handler
*   SvcReportStatus    Report service status to SCM
*   SvcReportEvent     Report an event to Windows event log
* 
* You can customize the service body by rewriting the SvcInit and SvcStop 
* functions. They are called when the service is being started and when the 
* service is to be stopped respectively. For example, you can create a named 
* pipe and monitor the named pipe in SvcInit, and close the named pipe in 
* SvcStop. In this sample, we just report the function call information to 
* the Application log in the two functions to keep the sample simple.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region "Includes"
#include <stdio.h>
#include <windows.h>
#include "Service.h"
#pragma endregion


SERVICE_STATUS          g_ssSvcStatus;         // Current service status
SERVICE_STATUS_HANDLE   g_sshSvcStatusHandle;  // Current service status handle
HANDLE                  g_hSvcStopEvent;


void WINAPI SvcMain(DWORD dwArgc, LPWSTR* lpszArgv);
void WINAPI SvcCtrlHandler(DWORD dwCtrl);
void SvcInit(DWORD dwArgc, LPWSTR* lpszArgv);
void SvcStop();
void SvcReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
void SvcReportEvent(LPWSTR lpszFunction, DWORD dwErr = 0);


//
//  FUNCTION: RunService
//
//  PURPOSE: Run the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    none
//
void RunService()
{
	// You can add any additional services for the process to this table.
	SERVICE_TABLE_ENTRY dispatchTable[] = 
	{
		{ SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)SvcMain }, 
		{ NULL, NULL }
	};

	// This call returns when the service has stopped.
	// The process should simply terminate when the call returns.
	if (!StartServiceCtrlDispatcher(dispatchTable))
	{
		SvcReportEvent(L"StartServiceCtrlDispatcher", GetLastError());
	}
}


//
//  FUNCTION: SvcMain
//
//  PURPOSE: Entry point for the service
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    This routine performs the service initialization and then calls the 
//    user defined SvcInit() routine to perform majority of the work.
//
void WINAPI SvcMain(DWORD dwArgc, LPWSTR* lpszArgv)
{
	SvcReportEvent(L"Enter SvcMain");

	// Register the handler function for the service
	g_sshSvcStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, 
		SvcCtrlHandler);
	if (!g_sshSvcStatusHandle)
	{
		SvcReportEvent(L"RegisterServiceCtrlHandler", GetLastError());
		return; 
	}

	// These SERVICE_STATUS members remain as set here
	g_ssSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; 
	g_ssSvcStatus.dwServiceSpecificExitCode = 0;

	// Report initial status to the SCM
	SvcReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	// Perform service-specific initialization and work.
	SvcInit(dwArgc, lpszArgv);
}


//
//  FUNCTION: SvcCtrlHandler
//
//  PURPOSE: Called by SCM whenever a control code is sent to the service 
//           using the ControlService function.
//
//  PARAMETERS:
//    dwCtrlCode - type of control requested
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    none
//
void WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
	// Handle the requested control code.
	switch(dwCtrl) 
	{  
	case SERVICE_CONTROL_STOP: 
		// Stop the service

		// SERVICE_STOP_PENDING should be reported before setting the Stop 
		// Event - g_hSvcStopEvent - in SvcStop(). This avoids a race 
		// condition which may result in a 1053 - The Service did not 
		// respond... error.
		SvcReportStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

		SvcStop();

		SvcReportStatus(g_ssSvcStatus.dwCurrentState, NO_ERROR, 0);

		return;

	case SERVICE_CONTROL_INTERROGATE: 
		break; 

	default:
		break;
	} 

}

//
//  FUNCTION: SvcInit
//
//  PURPOSE: Actual code of the service that does the work.
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    none
//
void SvcInit(DWORD dwArgc, LPWSTR* lpszArgv)
{
	SvcReportEvent(L"Enter SvcInit");

	// Service initialization.

	// Declare and set any required variables. Be sure to periodically call 
	// ReportSvcStatus() with SERVICE_START_PENDING. If initialization fails, 
	// call ReportSvcStatus with SERVICE_STOPPED.

	// Create a manual-reset event that is not signaled at first. The control 
	// handler function, SvcCtrlHandler, signals this event when it receives 
	// the stop control code.
	g_hSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hSvcStopEvent == NULL)
	{
		SvcReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}

	// Report running status when initialization is complete.
	SvcReportStatus(SERVICE_RUNNING, NO_ERROR, 0);

	// Perform work until service stops.
	while(TRUE)
	{
		// Perform work ...

		// Check whether to stop the service.
		WaitForSingleObject(g_hSvcStopEvent, INFINITE);

		SvcReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}
}


//
//  FUNCTION: SvcStop
//
//  PURPOSE: Stops the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    If a SvcStop procedure is going to take longer than 3 seconds to 
//    execute, it should spawn a thread to execute the stop code, and return. 
//    Otherwise, the ServiceControlManager will believe that the service has
//    stopped responding.
//
void SvcStop()
{
	SvcReportEvent(L"Enter SvcStop");

	// Signal the service to stop.
	if (g_hSvcStopEvent)
	{
		SetEvent(g_hSvcStopEvent);
	}
}


//
//  FUNCTION: SvcReportStatus
//
//  PURPOSE: Sets the current service status and reports it to the SCM.
//
//  PARAMETERS:
//    dwCurrentState - the state of the service (see SERVICE_STATUS)
//    dwWin32ExitCode - error code to report
//    dwWaitHint - Estimated time for pending operation, in milliseconds
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    none
//
void SvcReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, 
					 DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure.

	g_ssSvcStatus.dwCurrentState = dwCurrentState;
	g_ssSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	g_ssSvcStatus.dwWaitHint = dwWaitHint;

	g_ssSvcStatus.dwControlsAccepted = 
		(dwCurrentState == SERVICE_START_PENDING) ? 
		0 : SERVICE_ACCEPT_STOP;

	g_ssSvcStatus.dwCheckPoint = 
		((dwCurrentState == SERVICE_RUNNING) || 
		(dwCurrentState == SERVICE_STOPPED)) ? 
		0 : dwCheckPoint++;

	// Report the status of the service to the SCM.
	SetServiceStatus(g_sshSvcStatusHandle, &g_ssSvcStatus);
}


//
//  FUNCTION: SvcReportEvent
//
//  PURPOSE: Allows any thread to log an error message
//
//  PARAMETERS:
//    lpszFunction - name of function that failed
//    dwErr - error code returned from the function
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
void SvcReportEvent(LPWSTR lpszFunction, DWORD dwErr) 
{
	HANDLE hEventSource;
	LPCWSTR lpszStrings[2];
	wchar_t szBuffer[80];

	hEventSource = RegisterEventSource(NULL, SERVICE_NAME);
	if (NULL != hEventSource)
	{
		WORD wType;
		if (dwErr == 0)
		{
			swprintf_s(szBuffer, ARRAYSIZE(szBuffer), lpszFunction);
			wType = EVENTLOG_INFORMATION_TYPE;
		}
		else
		{
			swprintf_s(szBuffer, ARRAYSIZE(szBuffer), L"%s failed w/err 0x%08lx", 
				lpszFunction, dwErr);
			wType = EVENTLOG_ERROR_TYPE;
		}

		lpszStrings[0] = SERVICE_NAME;
		lpszStrings[1] = szBuffer;

		ReportEvent(hEventSource,  // Event log handle
			wType,                 // Event type
			0,                     // Event category
			0,                     // Event identifier
			NULL,                  // No security identifier
			2,                     // Size of lpszStrings array
			0,                     // No binary data
			lpszStrings,           // Array of strings
			NULL);                 // No binary data

		DeregisterEventSource(hEventSource);
	}
}