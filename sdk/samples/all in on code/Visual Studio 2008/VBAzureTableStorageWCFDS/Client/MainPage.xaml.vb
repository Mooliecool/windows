'****************************** Module Header ******************************\
' Module Name:	MainPage.xaml.vb
' Project:		VBAzureTableStorageWCFDS
' Copyright (c) Microsoft Corporation.
' 
' This is a test Silverlight client that verifies our WCF Data Service is working.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports Client.DataServiceReference
Imports System.Collections.ObjectModel

Partial Public Class MainPage
	Inherits UserControl
	Private _ctx As PersonDataServiceContext
	Private _dataSource As ObservableCollection(Of Person) = New ObservableCollection(Of Person)

	Public Sub New()
		InitializeComponent()
	End Sub

	Private Sub UserControl_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
		' Use the relative URI to find the WCF Data Services.
		Dim uri As New Uri(Application.Current.Host.Source, "../../DataService/PersonDataService.svc")
		' Load all entities. 
		Me._ctx = New PersonDataServiceContext(uri)
		Me._ctx.Person.BeginExecute(AddressOf QueryCallback, Nothing)
	End Sub

	''' <summary>
	''' In VB 9, anonymous lambda expressions are not supported.
	''' So we have to explictly write a callback method.
	''' If you upgrade to Silverlight 4, you can take advantage of anonymous lambda expressions.
	''' Silverlight 4 supports VB 10.
	''' </summary>
	Private Sub QueryCallback(ByVal result As IAsyncResult)
		Dim people As IEnumerable(Of Person) = Me._ctx.Person.EndExecute(result)
		Me._dataSource.Clear()
		Dim person As Person
		For Each person In people
			Me._dataSource.Add(person)
		Next
		Me.MainDataGrid.ItemsSource = Me._dataSource
		Me.NewButton.IsEnabled = True
		Me.DeleteButton.IsEnabled = True
		Me.UpdateButton.IsEnabled = True
	End Sub

	Private Sub NewButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
		Dim age As Integer
		If Me.ValidateInput(age) Then
			Dim person As New Person
			person.Name = Me.NameTextBox.Text
			person.Age = age
			Me.InsertNewEntity(person)
			Me.NameTextBox.Text = ""
			Me.AgeTextBox.Text = ""
		End If
	End Sub

	''' <summary>
	''' Invoke the WCF Data Services to insert a new entity.
	''' </summary>
	Private Sub InsertNewEntity(ByVal person As Person)
		Me._ctx.AddObject("Person", person)
		Me._ctx.BeginSaveChanges(AddressOf InsertCallback, person)
	End Sub

	Private Sub InsertCallback(ByVal result As IAsyncResult)
		Me._ctx.EndSaveChanges(result)
		Dim person As Person
		person = result.AsyncState
		Me._dataSource.Insert(0, person)
		Me.MainDataGrid.SelectedItem = person
		MessageBox.Show("Entity successfully inserted!")
	End Sub

	Private Sub DeleteButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
		Dim person As Person = TryCast(Me.MainDataGrid.SelectedItem, Person)
		If (Not person Is Nothing) Then
			Me.DeleteEntity(person)
		Else
			MessageBox.Show("Please select an item.")
		End If
	End Sub

	''' <summary>
	''' Invoke the WCF Data Services to delete the selected entity.
	''' </summary>
	Private Sub DeleteEntity(ByVal person As Person)
		Me._ctx.DeleteObject(person)
		Me._ctx.BeginSaveChanges(AddressOf DeleteCallback, person)
	End Sub

	Private Sub DeleteCallback(ByVal result As IAsyncResult)
		Me._ctx.EndSaveChanges(result)
		Dim person As Person
		person = result.AsyncState
		Me._dataSource.Remove(person)
		Me.MainDataGrid.SelectedItem = Nothing
		MessageBox.Show("Entity successfully deleted!")
	End Sub

	Private Sub UpdateButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
		Dim age As Integer
		If Me.ValidateInput(age) Then
			Dim personOld As Person = TryCast(Me.MainDataGrid.SelectedItem, Person)
			If (Not personOld Is Nothing) Then
				Dim personNew As New Person(personOld.PartitionKey, personOld.RowKey)
				personNew.Name = Me.NameTextBox.Text
                personNew.Age = age
                personNew.Timestamp = personOld.Timestamp
				Me.UpdateEntity(personOld, personNew)
				Me.NameTextBox.Text = ""
				Me.AgeTextBox.Text = ""
			Else
				MessageBox.Show("Please select an item.")
			End If
		End If
	End Sub

	''' <summary>
	''' Invoke the WCF Data Services to update the selected entity.
	''' </summary>
	Private Sub UpdateEntity(ByVal personOld As Person, ByVal personNew As Person)
		Me._ctx.Detach(personOld)
		Me._ctx.AttachTo("Person", personNew)
		Me._ctx.UpdateObject(personNew)
		Me.personOld = personOld
		Me.personNew = personNew
		Me._ctx.BeginSaveChanges(AddressOf UpdateCallback, Nothing)
	End Sub

	Private personOld As Person
	Private personNew As Person

	Private Sub UpdateCallback(ByVal result As IAsyncResult)
		Me._ctx.EndSaveChanges(result)
		Dim index As Integer = Me._dataSource.IndexOf(personOld)
		Me._dataSource.Remove(personOld)
		Me._dataSource.Insert(index, personNew)
		Me.MainDataGrid.SelectedItem = personNew
		MessageBox.Show("Entity successfully updated!")
	End Sub

	Private Function ValidateInput(ByRef age As Integer) As Boolean
		age = 0
		If String.IsNullOrEmpty(Me.NameTextBox.Text) Then
			MessageBox.Show("Please type a name.")
			Return False
		End If
		Try
			age = Integer.Parse(Me.AgeTextBox.Text)
		Catch
			MessageBox.Show("Invalid age.")
			Return False
		End Try
		Return True
	End Function
End Class