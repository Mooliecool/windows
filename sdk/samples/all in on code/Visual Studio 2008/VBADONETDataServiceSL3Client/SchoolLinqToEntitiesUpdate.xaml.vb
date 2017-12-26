' ***************************** Module Header ******************************\
'* Module Name:	SchoolLinqToEntitiesUpdate.xaml.vb
'* Project:		VBADONETDataServiceSL3Client
'* Copyright (c) Microsoft Corporation.
'* 
'* SchoolLinqToEntitiesUpdate.xaml.vb demonstrates how to call ASP.NET Data Service
'* to select and update records in Silverlight.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
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
Imports System.Data.Services.Client
Imports VBADONETDataServiceSL3Client.SchoolLinqToEntitiesService
Imports System.Windows.Browser
Imports System.Globalization
Imports System.IO
Imports System.Net.Browser

Partial Public Class SchoolLinqToEntitiesUpdate
    Inherits UserControl
    ' The data source of DataGrid control
    Private _collection As New List(Of ScoreCardForSchoolLinqToEntities)()
    ' The URL of ADO.NET Data Service
    Private Const _schoolLinqToEntitiesUri As String = "http://localhost:8888/SchoolLinqToEntities.svc"
    ' collection => returnedCourseGrade => _entities ={via async REST call}=> ADO.NET Data Service
    Private _entities As SQLServer2005DBEntities

    Public Sub New()
        InitializeComponent()
        AddHandler Me.Loaded, AddressOf MainPage_Loaded
    End Sub

    Private Sub MainPage_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)

        LoadData()
    End Sub

    ''' <summary>
    ''' In this method we send a REST request to ADO.NET Data Service to request CourseGrade
    ''' records. We expand Person and Course so the foreign records will be returned as well
    ''' </summary>
    Private Sub LoadData()
        _entities = New SQLServer2005DBEntities(New Uri(_schoolLinqToEntitiesUri))
        Dim query As DataServiceQuery(Of CourseGrade) = DirectCast((From c In _entities.CourseGrade.Expand("Person").Expand("Course") _
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
                ' Due to the following QueryInterceptor at server side only Course ID>4000 can be returned:
                ' [QueryInterceptor("Course")]
                ' public Expression<Func<Course, bool>> QueryCourse()
                ' {
                ' // LINQ lambda expression to filter the course objects
                ' return c => c.CourseID > 4000;
                ' }

                _collection = (From c In returnedCourseGrade.ToList() _
                    Select New ScoreCardForSchoolLinqToEntities With {.CourseGrade = c, _
                                                                      .Course = If(c.Course Is Nothing, "Only Course ID>4000 can be shown here", c.Course.Title), _
                                                                      .Grade = c.Grade, _
                                                                      .PersonName = String.Format("{0} {1}", c.Person.FirstName, c.Person.LastName) _
                                                                      }).ToList()

                Me.mainDataGrid.ItemsSource = _collection
            End If
        Catch ex As DataServiceQueryException
            Me.messageTextBlock.Text = String.Format("Error: {0} - {1}", ex.Response.StatusCode.ToString(), ex.Response.[Error].Message)
        End Try
        Return Nothing
    End Function

    ''' <summary>
    ''' This event handler handles the RowEdited event of the DataGrid control
    ''' It updates the edited Grade via an async REST call to ADO.NET Data Service.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub mainDataGrid_RowEditEnded(ByVal sender As Object, ByVal e As DataGridRowEditEndedEventArgs)
        Dim s As ScoreCardForSchoolLinqToEntities = TryCast(e.Row.DataContext, ScoreCardForSchoolLinqToEntities)
        If s IsNot Nothing Then
            Dim recordforupdate As CourseGrade = s.CourseGrade
            _entities.UpdateObject(recordforupdate)

            _entities.BeginSaveChanges(SaveChangesOptions.ReplaceOnUpdate, AddressOf OnChangesSaved, _entities)

        End If
    End Sub

    ''' <summary>
    ''' Callback method of the update operation.
    ''' </summary>
    ''' <param name="result"></param>
    Private Sub OnChangesSaved(ByVal result As IAsyncResult)
        Dispatcher.BeginInvoke(Function() ChangesSaved(result))
    End Sub

    Private Function ChangesSaved(ByVal result As IAsyncResult)

        Dim svcContext As SQLServer2005DBEntities = TryCast(result.AsyncState, SQLServer2005DBEntities)

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