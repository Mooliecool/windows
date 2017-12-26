'/************************************* Module Header **************************************\
'* Module Name:	GetWrapperForm.vb
'* Project:		VBVstoGetWrapperObject
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBVstoGetWrapperObject project demonstrates how to get a VSTO wrapper
'* object from an existing Office COM object.
'*
'* This feature requires Visual Studio Tools for Office 3.0 SP1 (included in 
'* Visual Studio 2008 SP1) for both design-time and runtime support.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 10/30/2009 3:00 PM Tim Li Created
'\******************************************************************************************/

#Region "Import directives"
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
Imports Excel = Microsoft.Office.Interop.Excel
Imports Microsoft.Office.Tools.Excel
Imports Microsoft.Office.Tools.Excel.Extensions
Imports System.Runtime.InteropServices
Imports Microsoft.VisualStudio.Tools.Applications.Runtime
#End Region

Public Class GetWrapperForm

    Private Sub GetWrapperForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        btnRefreshWb.PerformClick()
    End Sub

    ''' <summary>
    ''' Refresh the Workbook list.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub btnRefreshWb_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnRefreshWb.Click
        cboWorkbooks.Items.Clear()

        For Each wb As Excel.Workbook In Globals.ThisAddIn.Application.Workbooks
            cboWorkbooks.Items.Add(wb)
        Next

        Dim hasWorkbook As Boolean = cboWorkbooks.Items.Count > 0

        cboWorksheets.Enabled = hasWorkbook
        btnRefreshWs.Enabled = hasWorkbook
        btnAddListObject.Enabled = hasWorkbook

        If (hasWorkbook) Then
            cboWorkbooks.SelectedIndex = 0
        End If
    End Sub

    ''' <summary>
    ''' Workbook selection changed, refresh the Workseet list.
    ''' </summary>
    Private Sub cboWorkbooks_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles cboWorkbooks.SelectedIndexChanged
        If Not cboWorkbooks.SelectedItem Is Nothing Then
            btnRefreshWs.PerformClick()
        End If
    End Sub

    ''' <summary>
    ''' Refresh the Worksheet list.
    ''' </summary>
    Private Sub btnRefreshWs_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnRefreshWs.Click
        Dim wb As Excel.Workbook = CType(cboWorkbooks.SelectedItem, Excel.Workbook)
        cboWorksheets.Items.Clear()

        For Each ws As Excel.Worksheet In wb.Worksheets
            cboWorksheets.Items.Add(ws)
        Next

        cboWorksheets.SelectedIndex = 0
    End Sub

    ''' <summary>
    ''' Adds
    ''' </summary>
    Private Sub btnAddListObject_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnAddListObject.Click
        '' This is Microsoft.Office.Interop.Excel.Worksheet (COM)
        Dim ws As Excel.Worksheet = CType(cboWorksheets.SelectedItem, Excel.Worksheet)
        ws.Activate()

        '' This is Microsoft.Office.Tools.Excel.Worksheet (VSTO wrapper)
        Dim vstoWs As Worksheet = Worksheet.GetVstoObject(ws)

        Try

            '' Now we have the VSTO wrapper, add some VSTO objects to it...
            '' First a ListObject
            Dim lo As ListObject = vstoWs.Controls.AddListObject(vstoWs.Range("A3"), "myTable")
            '' Try bind some data to the ListObject
            lo.DataSource = GetDemoData()
            lo.DataMember = "DemoTable"

            '' Now add a button.
            Dim btnVsto As Button = vstoWs.Controls.AddButton(vstoWs.Range("A1"), "btnVSTO")
            btnVsto.Text = "VSTO Button"
            btnVsto.Width = 100
            btnVsto.Height = 23
            '' Setup the button Click event handler.
            AddHandler btnVsto.Click, AddressOf btnVsto_Click


        Catch rtEx As RuntimeException
            MessageBox.Show(rtEx.ToString(), "GetVstoObject demo", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub

    Private Sub btnVsto_Click(ByVal sender As Object, ByVal e As EventArgs)
        MessageBox.Show("VSTO button clicked.", "GetVstoObject demo", MessageBoxButtons.OK, MessageBoxIcon.Information)
    End Sub

    ''' <summary>
    ''' Generates some data for ListObject databinding.
    ''' </summary>
    Private Function GetDemoData() As DemoData
        Dim data As DemoData = New DemoData()
        data.DemoTable.Rows.Add(New Object() {Nothing, "John", New DateTime(1978, 2, 20)})
        data.DemoTable.Rows.Add(New Object() {Nothing, "Eric", New DateTime(1987, 6, 12)})
        data.DemoTable.Rows.Add(New Object() {Nothing, "Mary", New DateTime(1980, 8, 10)})
        data.DemoTable.Rows.Add(New Object() {Nothing, "Mike", New DateTime(1991, 1, 9)})
        data.DemoTable.Rows.Add(New Object() {Nothing, "Joe", New DateTime(1983, 3, 31)})
        data.DemoTable.Rows.Add(New Object() {Nothing, "Lance", New DateTime(1988, 5, 11)})
        data.DemoTable.Rows.Add(New Object() {Nothing, "Tom", New DateTime(1970, 9, 30)})

        Return data
    End Function
End Class