'****************************** Module Header ******************************\
' Module Name:	PersonTableStorageContext.vb
' Project:		VBAzureTableStorageWCFDataServices
' Copyright (c) Microsoft Corporation.
' 
' This is the table storage context.
' Do not confuse it with PersonDataServiceContext (the WCF Data Service object context).
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports Microsoft.WindowsAzure
Imports Microsoft.WindowsAzure.StorageClient

Namespace Entities
	''' <summary>
	''' This is the table storage context.
	''' Do not confuse it with PersonDataServiceContext (the WCF Data Service object context).
	''' Table storage contexts should derive from TableServiceContext.
	''' </summary>
	Public Class PersonTableStorageContext
		Inherits TableServiceContext
		Shared Sub New()
			account = CloudStorageAccount.FromConfigurationSetting("DataConnectionString")
			Dim tableClient = New CloudTableClient(PersonTableStorageContext.account.TableEndpoint.AbsoluteUri, PersonTableStorageContext.account.Credentials).CreateTableIfNotExist(PersonTableStorageContext.TableName)
		End Sub

		Public Sub New()
			MyBase.New(PersonTableStorageContext.account.TableEndpoint.AbsoluteUri, PersonTableStorageContext.account.Credentials)
		End Sub

		Public ReadOnly Property Person() As IQueryable(Of Person)
			Get
				Return MyBase.CreateQuery(Of Person)(PersonTableStorageContext.TableName)
			End Get
		End Property

		Private Shared account As CloudStorageAccount
		Public Shared TableName As String = "Person"
	End Class
End Namespace