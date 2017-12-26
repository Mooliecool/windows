'****************************** Module Header ******************************\
' Module Name:    LinqToReflectionForm.vb
' Project:        CSLinqExtension
' Copyright (c) Microsoft Corporation.
'
' This LinqToReflectionForm uses the simple LINQ to Reflection library to 
' display the methods of the exported class in an assembly grouped by return 
' value type.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
#End Region

Public Class LinqToReflectionForm

    ' The file name of the CSV file
    Private filePath As String

    ' Show OpenFileDialog to select an .NET assembly
    Private Sub btnBrowse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnBrowse.Click
        If openFileDialog1.ShowDialog() = DialogResult.OK Then
            filePath = openFileDialog1.FileName
            txtBoxPath.Text = filePath
        End If
    End Sub

#Region "Group The Methods by Return Value Type"

    Private Sub btnGroup_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGroup.Click
        Try
            filePath = txtBoxPath.Text

            ' Check whether the file path is empty
            If String.IsNullOrEmpty(filePath) Then
                MessageBox.Show("Please select a .NET assembly file first!")
                Return
            End If

            ' Call the GroupMethodByReturnValue to get the grouped methods
            Dim methods = LinqToReflection.GroupMethodsByReturnValue(filePath)

            ' Display the data in the TreeView
            For Each group In methods
                Dim node As TreeNode = treeView1.Nodes.Add(group.Key)
                For Each mi In group
                    node.Nodes.Add(mi.Name)
                Next
            Next

            treeView1.ExpandAll()
        Catch ex As Exception
            MessageBox.Show(ex.ToString())
        End Try
    End Sub

#End Region


End Class