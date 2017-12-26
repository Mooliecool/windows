'/************************************* Module Header **************************************\
'* Module Name:  Sheet1.vs
'* Project:      VBVstoExcelWorkbook
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBVstoExcelWorkbook provides the examples on how to customize Excel 
'* Workbooks by using the ListObject and the document Actions Pane.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* *  11/4/2009 6:00 PM Tim Li Created
'\******************************************************************************************/

#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.Data
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
Imports System.Xml.Linq
Imports Microsoft.VisualStudio.Tools.Applications.Runtime
Imports Excel = Microsoft.Office.Interop.Excel
Imports Office = Microsoft.Office.Core
#End Region


Public Class Sheet1

    Private Sub Sheet1_Startup(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Startup

    End Sub

    Private Sub Sheet1_Shutdown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shutdown

    End Sub

    Friend Sub UpdateCourseList(ByVal studentName As String)
        ' Update the title.
        Me.Range("A1", "A1").Value2 = "Course List for " + studentName

        'Update the DataTable.
        CourseListTableAdapter.FillCourseList(SchoolDataSet.CourseList, studentName)
    End Sub
End Class
