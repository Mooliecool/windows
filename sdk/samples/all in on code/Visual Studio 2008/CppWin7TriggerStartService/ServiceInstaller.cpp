/************************************** Module Header **************************************\
* Module Name:  ServiceInstaller.cpp
* Project:      CppWin7TriggerStartService
* Copyright (c) Microsoft Corporation.
* 
* The file implements functions to install/uninstall the service, and to query the service 
* trigger-start configuration.
* 
* In this code example, we demonstrate how to configure a service to start when a generic USB 
* disk becomes available. It also shows how to trigger start service when the first IP 
* address becomes available, and trigger stop the service when the last IP address becomes 
* unavailable. Additionally, you will learn how to query the curret trigger start 
* configuration of a service.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*******************************************************************************************/

#pragma region "Includes"
#include <stdio.h>
#include <windows.h>
#include "ServiceInstaller.h"
#include "Service.h"
#include "ServiceTriggerStart.h"
#pragma endregion


//
//  FUNCTION: SvcInstall
//
//  PURPOSE: Installs the service, and configure the service to trigger-start if applicable
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    This function requires administrative priviledge
//
void SvcInstall()
{
	wchar_t szPath[MAX_PATH];
	if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)) == 0)
	{
		wprintf(L"GetModuleFileName failed w/err 0x%08lx\n", GetLastError());
		return;
	}

	// Open the local default service control manager database
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, 
		SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
	if (!schSCManager)
	{
		wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
		return;
	}

	// Install the service into SCM by calling CreateService
	SC_HANDLE schService = CreateService(
		schSCManager,                   // SCManager database
		SERVICE_NAME,                   // Name of service
		SERVICE_DISPLAY_NAME,           // Name to display
		SERVICE_CHANGE_CONFIG,          // Desired access
		SERVICE_WIN32_OWN_PROCESS,      // Service type
		SERVICE_DEMAND_START,           // Start type
		SERVICE_ERROR_NORMAL,           // Error control type
		szPath,                         // Service's binary
		NULL,                           // No load ordering group
		NULL,                           // No tag identifier
		SERVICE_DEPENDENCIES,           // Dependencies
		SERVICE_ACCOUNT,                // Service running account
		SERVICE_PASSWORD);              // Password of the account

	if (NULL != schService)
	{
		wprintf(L"%s installed.\n", SERVICE_DISPLAY_NAME);

		// Check whether the current system supports service trigger start
		if (SupportServiceTriggerStart())
		{
			// Configure service trigger start
			// Please note that the service handle (schService) must be opened or created 
			// with the SERVICE_CHANGE_CONFIG access right (see the above call to 
			// CreateService), otherwise, the function will fail with ERROR_ACCESS_DENIED. 

			_putws(L"Configuring service trigger start...");

#if defined(SERVICE_TRIGGER_START_ON_USB)

			// Set the service to trigger-start when a generic USB disk becomes available
			if (!SetServiceTriggerStartOnUSBArrival(schService))
			{
				wprintf(L"SetServiceTriggerStartOnUSBArrival failed w/err 0x%08lx\n", 
					GetLastError());
			}

#elif defined(SERVICE_TRIGGER_START_ON_IP_ADDRESS)

			// Set the service to trigger-start when the first IP address becomes available, 
			// and trigger-stop when the last IP address becomes unavailable.
			if (!SetServiceTriggerStartOnIPAddressArrival(schService))
			{
				wprintf(L"SetServiceTriggerStartOnIPAddressArrival failed w/err 0x%08lx\n", 
					GetLastError());
			}

#elif defined(SERVICE_TRIGGER_START_ON_FIREWALL_PORT)

			// Set the service to trigger-start when a firewall port (UDP 5001 in this 
			// example) is opened, and trigger-stop when the a firewall port (UDP 5001 in 
			// this example) is closed. 
			if (!SetServiceTriggerStartOnFirewallPortEvent(schService))
			{
				wprintf(L"SetServiceTriggerStartOnFirewallPortEvent failed w/err 0x%08lx\n", 
					GetLastError());
			}

#endif
		}
		else
		{
			_putws(L"The current system does not support trigger-start service.");
		}

		CloseServiceHandle(schService);
	}
	else
	{
		wprintf(L"CreateService failed w/err 0x%08lx\n", GetLastError());
	}

	CloseServiceHandle(schSCManager);
}


//
//  FUNCTION: SvcUninstall
//
//  PURPOSE: Stops and removes the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    This function requires administrative priviledge
//
void SvcUninstall()
{
	// Open the local default service control manager database
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (!schSCManager)
	{
		wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
		return;
	}

	// Open the service with delete, stop and query status permissions
	SC_HANDLE schService = OpenService(schSCManager, SERVICE_NAME, 
		DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS);

	if (NULL != schService)
	{
		// Try to stop the service
		SERVICE_STATUS ssSvcStatus;
		if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus))
		{
			wprintf(L"Stopping %s.", SERVICE_DISPLAY_NAME);
			Sleep(1000);

			while (QueryServiceStatus(schService, &ssSvcStatus))
			{
				if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
				{
					wprintf(L".");
					Sleep(1000);
				}
				else break;
			}

			if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
			{
				wprintf(L"\n%s stopped.\n", SERVICE_DISPLAY_NAME);
			}
			else
			{
				wprintf(L"\n%s failed to stop.\n", SERVICE_DISPLAY_NAME);
			}
		}

		// Now remove the service by calling DeleteService
		if (DeleteService(schService))
		{
			wprintf(L"%s removed.\n", SERVICE_DISPLAY_NAME);
		}
		else
		{
			wprintf(L"DeleteService failed w/err 0x%08lx\n", GetLastError());
		}

		CloseServiceHandle(schService);
	}
	else
	{
		wprintf(L"OpenService failed w/err 0x%08lx\n", GetLastError());
	}

	CloseServiceHandle(schSCManager);
}


//
//  FUNCTION: SvcQueryConfig
//
//  PURPOSE: Query the service status and trigger-start configuration
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    This function does not require administrative priviledge
//
void SvcQueryConfig()
{
	// Open the local default service control manager database
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (!schSCManager)
	{
		wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
		return;
	}

	// Try to open the service to query its status and config
	SC_HANDLE schService = OpenService(schSCManager, SERVICE_NAME, 
		SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG);

	if (NULL != schService)
	{
		wprintf(L"%s was installed.\n", SERVICE_DISPLAY_NAME);

		DWORD cbBytesNeeded;

		// 
		// Query the status of the service
		// 

		SERVICE_STATUS_PROCESS ssp;
		if (QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, 
			sizeof(ssp), &cbBytesNeeded))
		{
			wprintf(L"Service status: ");
			switch (ssp.dwCurrentState)
			{
			case SERVICE_STOPPED: _putws(L"Stopped"); break;
			case SERVICE_RUNNING: _putws(L"Running"); break;
			case SERVICE_PAUSED: _putws(L"Paused"); break;
			case SERVICE_START_PENDING:
			case SERVICE_STOP_PENDING:
			case SERVICE_CONTINUE_PENDING:
			case SERVICE_PAUSE_PENDING: _putws(L"Pending"); break;
			}
		}
		else
		{
			wprintf(L"QueryServiceStatusEx failed w/err 0x%08lx\n", GetLastError());
		}

		// 
		// Query the trigger-start configuration of the service
		// 

		BOOL fIsTriggerStart;
		if (GetServiceTriggerInfo(schService, &fIsTriggerStart))
		{
			wprintf(L"Is trigger-start: %s", fIsTriggerStart ? L"Yes" : L"No");
		}
		else
		{
			wprintf(L"GetServiceTriggerInfo failed w/err 0x%08lx\n", GetLastError());
		}
		
		CloseServiceHandle(schService);
	}
	else
	{
		DWORD dwErr = GetLastError();
		if (dwErr == ERROR_SERVICE_DOES_NOT_EXIST)
		{
			wprintf(L"%s was not installed.\n", SERVICE_DISPLAY_NAME);
		}
		else
		{
			wprintf(L"OpenService failed w/err 0x%08lx\n", dwErr);
		}
	}

	CloseServiceHandle(schSCManager);
}