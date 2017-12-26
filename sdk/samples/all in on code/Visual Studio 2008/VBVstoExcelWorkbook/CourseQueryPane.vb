'/************************************* Module Header **************************************\
'* Module Name:  CourseQueryPane.vb
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
'* * 11/4/2009 6:00 PM Tim Li Created
'\******************************************************************************************/

#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Drawing
Imports System.Data
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
#End Region


Public Class CourseQueryPane

    Private Sub CourseQueryPane_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' Fill the student names list.
        studentListTableAdapter.FillStudentList(SchoolDataSet.StudentList)
    End Sub

    Private Sub cmdQuery_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmdQuery.Click
        'Update course list for selected student.
        Globals.Sheet1.UpdateCourseList(cboName.Text.Trim())
    End Sub
End Class
