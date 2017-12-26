'****************************** Module Header ******************************\
' Module Name:	PersonDataServiceContext.vb
' Project:		VBAzureTableStorageWCFDataServices
' Copyright (c) Microsoft Corporation.
' 
' This is the WCF Data Service object context.
' Do not confuse it with PersonTableStorageContext (the table storage context).
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
Imports WebRole.Entities


Namespace DataService
	''' <summary>
	''' This is the WCF Data Service object context.
	''' Do not confuse it with PersonTableStorageContext (the table storage context).
	''' Table storage does not use EDM or LINQ to SQL.
	''' So we must create a custom object context that implements IUpdatable.
	''' This sample only demonstrates simple CRUD operations. So not all methods are implemented.
	''' </summary>
	Public Class PersonDataServiceContext
		Implements IUpdatable
		Public Sub AddReferenceToCollection(ByVal targetResource As Object, ByVal propertyName As String, ByVal resourceToBeAdded As Object) Implements IUpdatable.AddReferenceToCollection
			Throw New NotImplementedException
		End Sub

		Public Sub ClearChanges() Implements IUpdatable.ClearChanges
			Throw New NotImplementedException
		End Sub

		Public Function CreateResource(ByVal containerName As String, ByVal fullTypeName As String) As Object Implements IUpdatable.CreateResource
			Dim entity As New Person
			' Add the entity to table context.
			Me._ctx.AddObject(PersonTableStorageContext.TableName, entity)
			Return entity
		End Function

		''' <summary>
		''' Delete the person entity from table storage.
		''' </summary>
		Public Sub DeleteResource(ByVal targetResource As Object) Implements IUpdatable.DeleteResource
			Dim person As Person = TryCast(targetResource, Person)
			If (person Is Nothing) Then
				Throw New DataServiceException(400, "Invalid object. Object must be a Person")
			End If
			Me._ctx.DeleteObject(person)
			Me._ctx.SaveChanges()
		End Sub

		''' <summary>
		''' GetResource is invoked before updating and deletion.
		''' </summary>
		Public Function GetResource(ByVal query As IQueryable, ByVal fullTypeName As String) As Object Implements IUpdatable.GetResource
			Dim tableQuery As IQueryable(Of Person) = TryCast(query, IQueryable(Of Person))
			If (tableQuery Is Nothing) Then
				Throw New DataServiceException(400, "Invalid query.")
			End If
			Return tableQuery.First()
		End Function

		Public Function GetValue(ByVal targetResource As Object, ByVal propertyName As String) As Object Implements IUpdatable.GetValue
			Throw New NotImplementedException
		End Function

		Public Sub RemoveReferenceFromCollection(ByVal targetResource As Object, ByVal propertyName As String, ByVal resourceToBeRemoved As Object) Implements IUpdatable.RemoveReferenceFromCollection
			Throw New NotImplementedException
		End Sub

		Public Function ResetResource(ByVal resource As Object) As Object Implements IUpdatable.ResetResource
			Throw New NotImplementedException
		End Function

		Public Function ResolveResource(ByVal resource As Object) As Object Implements IUpdatable.ResolveResource
			Return resource
		End Function

		Public Sub SaveChanges() Implements IUpdatable.SaveChanges
			Me._ctx.SaveChanges()
		End Sub

		Public Sub SetReference(ByVal targetResource As Object, ByVal propertyName As String, ByVal propertyValue As Object) Implements IUpdatable.SetReference
			Throw New NotImplementedException
		End Sub

		''' <summary>
		''' SetValue is invoked before updating.
		''' </summary>
		Public Sub SetValue(ByVal targetResource As Object, ByVal propertyName As String, ByVal propertyValue As Object) Implements IUpdatable.SetValue
			' The Partition/RowKey should not be modified.
			If (((Not propertyValue Is Nothing) AndAlso (propertyName <> "PartitionKey")) AndAlso (propertyName <> "RowKey")) Then
				Dim person As Person = DirectCast(targetResource, Person)
				GetType(Person).GetProperty(propertyName).SetValue(person, propertyValue, Nothing)
				Me._ctx.UpdateObject(person)
			End If
		End Sub


		Public ReadOnly Property Person() As IQueryable(Of Person)
			Get
				Return Me._ctx.Person
			End Get
		End Property


		Private _ctx As PersonTableStorageContext = New PersonTableStorageContext
	End Class
End Namespace