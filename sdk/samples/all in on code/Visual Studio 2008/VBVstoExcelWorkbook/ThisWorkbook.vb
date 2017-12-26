'/************************************* Module Header **************************************\
'* Module Name:  ThisWorkbook.vb
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

#Region "Using directives"
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


Public Class ThisWorkbook

    Private Sub ThisWorkbook_Startup(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Startup
        ' Adds the CourseQueryPane to the ActionsPane.
        ' To toggle the ActionsPane on/off, click the View tab in Excel Ribbon,
        ' then in the Show/Hide group, toggle the Document Actions button.
        Me.ActionsPane.Controls.Add(New CourseQueryPane())
    End Sub

    Private Sub ThisWorkbook_Shutdown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shutdown

    End Sub

End Class
