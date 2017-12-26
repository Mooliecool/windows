'************************************ Module Header **************************************\
'* Module Name:	MainForm.vb
'* Project:		VBWinFormTreeViewLoad
'* Copyright (c) Microsoft Corporation.
'* 
'* This example demonstrates how to create a TreeView from a DataTable.
'* 
'* For more information about the TreeView control, see:
'* 
'*  Windows Forms TreeView control
'*  http://msdn.microsoft.com/en-us/library/ch6etkw4.aspx
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\*****************************************************************************************

Imports System
Imports System.Data
Imports System.Windows.Forms

Public Class MainForm
    ' This DataTable will fill the TreeView
    Private _dtEmployees As DataTable

    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Me._dtEmployees = Me.CreateDataTable()

        ' Call the BuildTree method to build the TreeView.
        CreateTreeViewFromDataTable.BuildTree(Me._dtEmployees, Me.TreeView1, True, "LastName", "EmployeeID", "ReportsTo")
    End Sub

    Public Function CreateDataTable() As DataTable
        Dim dataTable As New DataTable()

        ' The value in this column will identify the TreeNode
        dataTable.Columns.Add("EmployeeID")

        ' The value in this column will display on the TreeNode
        dataTable.Columns.Add("LastName")

        ' The value in this column will identify its parentId 
        dataTable.Columns.Add("ReportsTo")

        ' Fill the DataTable
        dataTable.Rows.Add(1, "Davolio", 2)
        dataTable.Rows.Add(2, "Fuller", DBNull.Value)
        dataTable.Rows.Add(3, "Leverling", 2)
        dataTable.Rows.Add(4, "Peacock", 2)
        dataTable.Rows.Add(5, "Buchanan", 2)
        dataTable.Rows.Add(6, "Suyama", 5)
        dataTable.Rows.Add(7, "King", 5)
        dataTable.Rows.Add(8, "Callahan", 2)
        dataTable.Rows.Add(9, "Dodsworth", 5)
        Return dataTable
    End Function
End Class
