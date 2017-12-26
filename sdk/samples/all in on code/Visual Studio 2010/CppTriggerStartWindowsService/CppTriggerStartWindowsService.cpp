/****************************** Module Header ******************************\
* Module Name:  CppTriggerStartWindowsService.cpp
* Project:      CppTriggerStartWindowsService
* Copyright (c) Microsoft Corporation.
* 
* The file defines the entry point of the application. According to the 
* arguments in the command line, the function installs or uninstalls or 
* starts or queries the service by calling into different routines.
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
#include "ServiceInstaller.h"
#pragma endregion


//
//  FUNCTION: wmain
//
//  PURPOSE: entrypoint for the application
// 
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    wmain() either performs the command line task, or run the service
//
int wmain(int argc, wchar_t* argv[])
{
	if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
	{
		if (_wcsicmp(L"install", argv[1] + 1) == 0)
		{
			SvcInstall();
		}
		else if (_wcsicmp(L"remove", argv[1] + 1) == 0)
		{
			SvcUninstall();
		}
		else if (_wcsicmp(L"query", argv[1] + 1) == 0)
		{
			SvcQueryConfig();
		}
	}
	else
	{
		_putws(L"Parameters:");
		_putws(L" -install    to install the service (require admin permission)");
		_putws(L" -remove     to remove the service (require admin permission)");
		_putws(L" -query      to query the configuration of the service");

		RunService();
	}

	return 0;
}