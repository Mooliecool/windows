'***************************** Module Header ******************************\
'* Module Name:	ScoreCardForSchoolLinqToSQL.xaml.vb
'* Project:		VBADONETDataServiceSL3Client
'* Copyright (c) Microsoft Corporation.
'* 
'* ScoreCardForSchoolLinqToSQL.xaml.vb demonstrates how to create a class as a real
'* data source for UI. It's a bridge between data retrieved from auto generated ADO.NET 
'* Data Service client side code and the UI. Some tricks are also shown here such as 
'* Display and Editable attribute.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 8/18/2009 2:00 PM Allen Chen Created
'\**************************************************************************

Imports System
Imports System.Net
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Ink
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Animation
Imports System.Windows.Shapes
Imports System.ComponentModel.DataAnnotations
Imports VBADONETDataServiceSL3Client.SchoolLinqToSQLService


' The collection of ScoreCardForSchoolLinqToSQL objects is the data source
' of DataGrid control.
Public Class ScoreCardForSchoolLinqToSQL
    ' Maintain a reference to CourseGrade for delete purpose.
    Private _CourseGrade As CourseGrade
    <Display(AutoGenerateField:=False)> _
    Public Property CourseGrade() As CourseGrade
        Get
            Return _CourseGrade
        End Get
        Set(ByVal value As CourseGrade)
            _CourseGrade = value
        End Set
    End Property
    Private _PersonName As String
    <Editable(False)> _
    Public Property PersonName() As String
        Get
            Return _PersonName
        End Get
        Set(ByVal value As String)
            _PersonName = value
        End Set
    End Property
    Private _Course As String
    <Editable(False)> _
    Public Property Course() As String
        Get
            Return _Course
        End Get
        Set(ByVal value As String)
            _Course = value
        End Set
    End Property

    Private _Grade As System.Nullable(Of Decimal)
    <Editable(False)> _
    Public Property Grade() As System.Nullable(Of Decimal)
        Get
            Return _Grade
        End Get
        Set(ByVal value As System.Nullable(Of Decimal))
            _Grade = value
        End Set
    End Property
End Class
