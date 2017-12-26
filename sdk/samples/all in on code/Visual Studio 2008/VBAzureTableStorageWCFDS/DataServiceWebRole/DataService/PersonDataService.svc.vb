'****************************** Module Header ******************************\
' Module Name:	PersonDataService.vb
' Project:		VBAzureTableStorageWCFDS
' Copyright (c) Microsoft Corporation.
' 
' This is the WCF Data Service.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System.Data.Services
Imports System.Linq
Imports System.ServiceModel.Web

Namespace DataService
	Public Class PersonDataService
		Inherits DataService(Of [PersonDataServiceContext])

		' This method is called only once to initialize service-wide policies.
		Public Shared Sub InitializeService(ByVal config As IDataServiceConfiguration)
			config.SetEntitySetAccessRule("*", EntitySetRights.All)
			config.SetServiceOperationAccessRule("*", ServiceOperationRights.All)
		End Sub

	End Class
End Namespace