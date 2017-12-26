'***************************** Module Header ******************************\
'* Module Name:	SchoolLinqToSQLServiceDelete.xaml.cs
'* Project:		CSADONETDataServiceSL3Client
'* Copyright (c) Microsoft Corporation.
'* 
'* SchoolLinqToSQLServiceDelete.cs demonstrates how to call ASP.NET Data Service
'* to select and delete records in Silverlight.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************

Imports VBADONETDataServiceSL3Client.SchoolLinqToSQLService
Imports System.Data.Services.Client

Partial Public Class SchoolLinqToSQLServiceDelete
    Inherits UserControl
    ' The data source of DataGrid control
    Private _collection As New List(Of ScoreCardForSchoolLinqToSQL)()
    ' The URL of ADO.NET Data Service
    Private Const _schoolLinqToSQLUri As String = "http://localhost:8888/SchoolLinqToSQL.svc"
    ' collection => returnedCourseGrade => _context ={via async REST call}=> ADO.NET Data Service
    Private _context As SchoolLinqToSQLDataContext

    Public Sub New()
        InitializeComponent()
        AddHandler Me.Loaded, AddressOf SchoolLinqToSQLServiceDelete_Loaded
    End Sub

    Private Sub SchoolLinqToSQLServiceDelete_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        LoadData()
    End Sub

    ''' <summary>
    ''' In this method we send a REST request to ADO.NET Data Service to request CourseGrade
    ''' records. We expand Person and Course so the foreign records will be returned as well
    ''' </summary>
    Private Sub LoadData()
        _context = New SchoolLinqToSQLDataContext(New Uri(_schoolLinqToSQLUri))
        Dim query As DataServiceQuery(Of CourseGrade) = DirectCast((From c In _context.CourseGrades.Expand("Person").Expand("Course") _
            Select c), DataServiceQuery(Of CourseGrade))

        query.BeginExecute(AddressOf OnCourseGradeQueryComplete, query)
    End Sub

    ''' <summary>
    ''' Callback method of the query to get CourseGrade records.
    ''' </summary>
    ''' <param name="result"></param>
    Private Sub OnCourseGradeQueryComplete(ByVal result As IAsyncResult)
        Dispatcher.BeginInvoke(Function() CourseGradeQueryComplete(result))
    End Sub

    Private Function CourseGradeQueryComplete(ByVal result As IAsyncResult)
        Dim query As DataServiceQuery(Of CourseGrade) = TryCast(result.AsyncState, DataServiceQuery(Of CourseGrade))
        Try
            Dim returnedCourseGrade = query.EndExecute(result)

            If returnedCourseGrade IsNot Nothing Then
                _collection = (From c In returnedCourseGrade.ToList() _
                    Select New ScoreCardForSchoolLinqToSQL With {.CourseGrade = c, _
                                                                .Course = c.Course.Title, _
                                                                .Grade = c.Grade, _
                                                                .PersonName = String.Format("{0} {1}", _
                                                                                            c.Person.FirstName, c.Person.LastName)}).ToList()

                Me.mainDataGrid.ItemsSource = _collection
            End If
        Catch ex As DataServiceQueryException
            Me.messageTextBlock.Text = String.Format("Error: {0} - {1}", ex.Response.StatusCode.ToString(), ex.Response.[Error].Message)

        End Try
        Return Nothing
    End Function

    ''' <summary>
    ''' This event handler handles the Click event of the Delete Button control
    ''' It deletes the record that is on the same row of the Delete Button,
    ''' via an async REST call to ADO.NET Data Service.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub DeleteButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim b As Button = DirectCast(sender, Button)
        Dim s As ScoreCardForSchoolLinqToSQL = TryCast(b.DataContext, ScoreCardForSchoolLinqToSQL)
        If s IsNot Nothing Then
            Dim recordfordelete As CourseGrade = s.CourseGrade
            _context.DeleteObject(recordfordelete)
            _context.BeginSaveChanges(SaveChangesOptions.None, AddressOf OnChangesSaved, _context)
        End If
    End Sub

    ''' <summary>
    ''' Callback method of the delete operation.
    ''' </summary>
    ''' <param name="result"></param>
    Private Sub OnChangesSaved(ByVal result As IAsyncResult)
        Dispatcher.BeginInvoke(Function() ChangesSaved(result))
    End Sub

    Private Function ChangesSaved(ByVal result As IAsyncResult)
        Dim svcContext As SchoolLinqToSQLDataContext = TryCast(result.AsyncState, SchoolLinqToSQLDataContext)

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
