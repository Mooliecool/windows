/****************************** Module Header ******************************\
* Module Name:  ServiceInstaller.h
* Project:      CppWin7TriggerStartService
* Copyright (c) Microsoft Corporation.
* 
* The file declares functions to install/uninstall the service, and to query 
* the service trigger-start configuration.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once


//
//  FUNCTION: SvcInstall
//
//  PURPOSE: Installs the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
void SvcInstall();


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
//
void SvcUninstall();


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
void SvcQueryConfig();