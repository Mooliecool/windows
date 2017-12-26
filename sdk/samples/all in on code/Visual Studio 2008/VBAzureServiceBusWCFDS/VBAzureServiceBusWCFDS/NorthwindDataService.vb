'***************************** Module Header ******************************\
'* Module Name:	NorthwindDataService.vb
'* Project:		VBAzureServiceBusWCFDS
'* Copyright (c) Microsoft Corporation.
'* 
'* This module is WCF Data Service that exposes Northwind DB
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************
Imports System.Data.Services

Public Class NorthwindDataService
    Inherits DataService(Of NorthwindEntities)
    ' This method is called only once to initialize service-wide policies.
    Public Shared Sub InitializeService(ByVal config As IDataServiceConfiguration)
        ' TODO: set rules to indicate which entity sets and service operations are visible, updatable, etc.
        ' Examples:
        config.UseVerboseErrors = True
        config.SetEntitySetAccessRule("*", EntitySetRights.All)
        ' config.SetServiceOperationAccessRule("MyServiceOperation", ServiceOperationRights.All);
    End Sub
End Class
