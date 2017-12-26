'***************************** Module Header ******************************\
'* Module Name:  SampleServiceInsert.xaml.vb
'* Project:      VBADONETDataServiceSL3Client
'* Copyright (c) Microsoft Corporation.
'* 
'* SampleServiceInsert.xaml.vb demonstrates how to call ASP.NET Data Service
'* to select and insert records in Silverlight.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 8/19/2009 2:00 PM Allen Chen Created
'\**************************************************************************

Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Net
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Animation
Imports System.Windows.Shapes
Imports VBADONETDataServiceSL3Client.SampleService
Imports System.Data.Services.Client

Partial Public Class SampleServiceInsert
    Inherits UserControl
    ' The URL of ADO.NET Data Service
    Private Const _sampleUri As String = "http://localhost:8888/Samples.svc"
    ' returnedCategory => _context ={via async REST call}=> ADO.NET Data Service
    Private _context As SampleProjects
    Public Sub New()
        InitializeComponent()
        AddHandler Me.Loaded, AddressOf SampleServiceInsert_Loaded
    End Sub

    Private Sub SampleServiceInsert_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        LoadData()
    End Sub

    ''' <summary>
    ''' In this method we send a REST request to ADO.NET Data Service to request Category
    ''' records. 
    ''' </summary>
    Private Sub LoadData()
        _context = New SampleProjects(New Uri(_sampleUri))
        Dim query As DataServiceQuery(Of Category) = DirectCast((From c In _context.Categories _
            Select c), DataServiceQuery(Of Category))

        query.BeginExecute(AddressOf OnCategoryQueryComplete, query)
    End Sub

    ''' <summary>
    ''' Callback method of the query to get Category records.
    ''' </summary>
    ''' <param name="result"></param>
    Private Sub OnCategoryQueryComplete(ByVal result As IAsyncResult)
        Dispatcher.BeginInvoke(Function() CategoryQueryComplete(result))
    End Sub

    Private Function CategoryQueryComplete(ByVal result As IAsyncResult)

        Dim query As DataServiceQuery(Of Category) = TryCast(result.AsyncState, DataServiceQuery(Of Category))
        Try
            Dim returnedCategory = query.EndExecute(result)

            If returnedCategory IsNot Nothing Then

                Me.mainDataGrid.ItemsSource = returnedCategory.ToList()
            End If
        Catch ex As DataServiceQueryException
            Me.messageTextBlock.Text = String.Format("Error: {0} - {1}", ex.Response.StatusCode.ToString(), ex.Response.[Error].Message)
        End Try
        Return Nothing
    End Function

    ''' <summary>
    ''' In this event handler, we begin to send a REST request to ADO.NET
    ''' Data Service to insert a new Category.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub InsertButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim categorynameforinsert As String = Me.categoryTextBox.Text
        _context.AddToCategories(New Category With {.CategoryName = categorynameforinsert})
        _context.BeginSaveChanges(AddressOf OnChangesSaved, _context)
    End Sub

    ''' <summary>
    ''' Callback method of the insert operation.
    ''' </summary>
    ''' <param name="result"></param>
    Private Sub OnChangesSaved(ByVal result As IAsyncResult)
        Dispatcher.BeginInvoke(Function() ChangesSaved(result))
    End Sub

    Private Function ChangesSaved(ByVal result As IAsyncResult)
        Dim svcContext As SampleProjects = TryCast(result.AsyncState, SampleProjects)

        Try
            ' Complete the save changes operation and display the response.
            WriteOperationResponse(svcContext.EndSaveChanges(result))
        Catch ex As DataServiceRequestException
            ' Display the error from the response.
            WriteOperationResponse(ex.Response)
        Catch ex As InvalidOperationException
            messageTextBlock.Text = ex.Message
        Finally
            ' Reload the binding collection to refresh Grid to see if it's successfully updated. 
            ' You can also remove the following line. To see the update effect, just refresh the page or check out database directly.
            LoadData()
        End Try
        Return Nothing
    End Function
    ''' <summary>
    ''' This method shows details information of the response from ADO.NET Data Service.
    ''' </summary>
    ''' <param name="response"></param>
    Private Sub WriteOperationResponse(ByVal response As DataServiceResponse)
        messageTextBlock.Text = String.Empty
        Dim i As Integer = 1

        If response.IsBatchResponse Then
            messageTextBlock.Text = String.Format("Batch operation response code: {0}" & vbLf, response.BatchStatusCode)
        End If
        For Each change As ChangeOperationResponse In response
            messageTextBlock.Text += String.Format(vbTab & "Change {0} code: {1}" & vbLf, i.ToString(), change.StatusCode.ToString())
            If change.[Error] IsNot Nothing Then
                String.Format(vbTab & "Change {0} error: {1}" & vbLf, i.ToString(), change.[Error].Message)
            End If
            i += 1
        Next
    End Sub
End Class