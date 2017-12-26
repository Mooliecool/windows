'****************************** Module Header ******************************\
' Module Name:	Person.vb
' Project:		VBAzureTableStorageWCFDataServices
' Copyright (c) Microsoft Corporation.
' 
' This is the table storage entity.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports Microsoft.WindowsAzure.StorageClient

Namespace Entities
	Public Class Person
		Inherits TableServiceEntity
		'By default, when creating a new entity, the PartitionKey is set to the current year, and the RowKey is a GUID. Insert the ticks in the beginning of RowKey because the result returned by a query is ordered by PartitionKey and then RowKey.
		Public Sub New()
			MyBase.New(DateTime.UtcNow.ToString("yyyy"), String.Format("{0:10}_{1}", (DateTime.MaxValue.Ticks - DateTime.Now.Ticks), Guid.NewGuid))
		End Sub

		Public Sub New(ByVal partitionKey As String, ByVal rowKey As String)
			MyBase.New(partitionKey, rowKey)
		End Sub

		Private _age As Integer
		Private _name As String

		Public Property Age() As Integer
			Get
				Return Me._age
			End Get
			Set(ByVal value As Integer)
				Me._age = value
			End Set
		End Property

		Public Property Name() As String
			Get
				Return Me._name
			End Get
			Set(ByVal value As String)
				Me._name = value
			End Set
		End Property
	End Class
End Namespace