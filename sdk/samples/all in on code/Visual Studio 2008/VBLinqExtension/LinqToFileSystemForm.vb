'****************************** Module Header ******************************\
' Module Name:    LinqToFileSystemForm.vb
' Project:        VBLinqExtension
' Copyright (c) Microsoft Corporation.
'
' This LinqToFileSystemForm uses the simple LINQ to File System library to 
' group the files in one folder by their extension names and to show the 
' largest files with their sizes and names.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Using directives"
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
#End Region

Public Class LinqToFileSystemForm

    ' The target file folder path
    Private filePath As String

    ' Show FolderBrowserDialog to select a file folder
    Private Sub btnBrowse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnBrowse.Click
        Dim dialog As New FolderBrowserDialog()

        If dialog.ShowDialog() = DialogResult.OK Then
            filePath = dialog.SelectedPath
            txtBoxPath.Text = filePath
        End If
    End Sub

#Region "Group Files by Extension Names"

    Private Sub btnGroup_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGroup.Click
        Try
            filePath = txtBoxPath.Text

            ' Check whether the file path is empty
            If String.IsNullOrEmpty(filePath) Then
                MessageBox.Show("Please choose the file folder first!")
                Exit Sub
            End If

            ' Get the all the files under the folder by the GetFiles
            ' extension methods, then group the files by the extension
            ' names.
            Dim queryGroupByExt = From file In LinqToFileSystem.GetFiles(filePath) _
                Group file By Key = file.Extension.ToLower() Into fileGroup = Group _
                Order By Key _
                Select Key, fileGroup

            ' Display the files in TreeView
            For Each fg In queryGroupByExt
                Dim node As TreeNode = treeView1.Nodes.Add(fg.Key)
                For Each f In fg.fileGroup
                    node.Nodes.Add(f.Name)
                Next
            Next

            treeView1.ExpandAll()
        Catch ex As Exception
            MessageBox.Show(ex.ToString())
        End Try
    End Sub

#End Region


#Region "Show The Largest Files"

    Private Sub btnMax_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMax.Click
        Try
            filePath = txtBoxPath.Text

            ' Check whether the file path is empty
            If String.IsNullOrEmpty(filePath) Then
                MessageBox.Show("Please choose the file folder first!")
                Exit Sub
            End If

            ' Get the largest files by the LargestFiles extension method
            Dim largestFileLists = LinqToFileSystem.GetFiles(filePath).LargestFiles()

            ' Display the data
            If largestFileLists.Count() > 0 Then
                Dim text As String = String.Format(largestFileLists.Count() & " file(s) with max length " & _
                                                     largestFileLists.First().Length & " bytes:")

                For Each file In largestFileLists
                    text += vbCr & vbLf & file.Name
                Next

                MessageBox.Show(text)
            End If
        Catch ex As Exception
            MessageBox.Show(ex.ToString())
        End Try
    End Sub

#End Region

   
End Class