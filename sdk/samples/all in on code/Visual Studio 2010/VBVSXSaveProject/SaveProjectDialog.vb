'********************************* Module Header *********************************\
' Module Name:        SaveProjectDialog.vb
' Project :           VBVSXSaveProject
' Copyright (c)       Microsoft Corporation
'
' This dialog is used to display the files included in the project, or under the project
' folder. Users can select the files that need to be copied.
' 
' The source is subject to the Microsoft Public License.
' See http://www.microsoft.com/en-us/openness/default.aspx
' All other rights reserved
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***********************************************************************************

Imports System.IO
Imports System.Windows.Forms
Imports EnvDTE
Imports Microsoft.VBVSXSaveProject.Files

Partial Public Class SaveProjectDialog
    Inherits Form
#Region "Declare variables to store files information."
    ''' <summary>
    ''' The path of project folder.
    ''' </summary>
    Public Property OriginalFolderPath() As String

    ''' <summary>
    ''' The path is that you select folder in Folder Browser Dialog.
    ''' </summary>
    Private _newFolderPath As String
    Public Property NewFolderPath() As String
        Get
            Return _newFolderPath
        End Get
        Private Set(ByVal value As String)
            _newFolderPath = value
        End Set
    End Property

    ''' <summary>
    ''' The files included in the project, or under the project folder. 
    ''' </summary>
    Public Property FilesItems() As List(Of Files.ProjectFileItem)

    ''' <summary>
    ''' Specify whether the new project should be opened.
    ''' </summary>
    Public ReadOnly Property OpenNewProject() As Boolean
        Get
            Return chkOpenProject.Checked
        End Get
    End Property

#End Region

    ''' <summary>
    ''' Construct the SaveProject Dialog.
    ''' </summary>
    Public Sub New()
        InitializeComponent()

        ' Auto generate column is set false.
        Me.dgvFiles.AutoGenerateColumns = False
    End Sub

    ''' <summary>
    ''' Save project as other directory when you click this button.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub btnSaveAs_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnSaveAs.Click
        ' Get the path of folder that you want to save by selecting in 
        ' FolderBrowserDialog.
        Using dialog As New FolderBrowserDialog()
            ' The new folder button is enable.
            dialog.ShowNewFolderButton = True

            ' Get the result that user operated on FolderBrowserDialog.
            Dim result = dialog.ShowDialog()

            If result = System.Windows.Forms.DialogResult.OK Then
                ' Get the path of folder.
                Me.NewFolderPath = dialog.SelectedPath

                ' Copy the files that user selected.
                CopySelectedItems()

                ' Click on OK, and then close this window.
                Me.DialogResult = System.Windows.Forms.DialogResult.OK
                Me.Close()
            Else
                Return
            End If
        End Using
    End Sub

    ''' <summary>
    ''' The save project dialog is load.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub SaveProjectDialog_Load(ByVal sender As Object, ByVal e As EventArgs) Handles MyBase.Load
        ' Associate with the DataSource with FilesItems.
        dgvFiles.DataSource = FilesItems

        For Each row As DataGridViewRow In dgvFiles.Rows
            Dim item As ProjectFileItem = TryCast(row.DataBoundItem, ProjectFileItem)

            row.Cells("colCopy").ReadOnly = Not item.IsUnderProjectFolder
        Next row
    End Sub

#Region "CreateAndCopyFiles  "
    ''' <summary>
    ''' Copy the files that related to the item in DataGridView that you selected.
    ''' </summary>
    Private Sub CopySelectedItems()
        ' Get the files information from Solution Explorer.
        Dim fileItems As List(Of Files.ProjectFileItem) =
            TryCast(dgvFiles.DataSource, List(Of Files.ProjectFileItem))

        ' Copy the files from original directory to newFolderPath.
        For Each fileItem In fileItems
            If fileItem.IsUnderProjectFolder AndAlso fileItem.NeedCopy Then
                ' Get the absolute path of project file that you save as.
                Dim newFile As New FileInfo(
                    String.Format("{0}\{1}",
                                  NewFolderPath,
                                  fileItem.FullName.Substring(OriginalFolderPath.Length)))

                ' Create the directory by the file full name.
                If Not newFile.Directory.Exists Then
                    Directory.CreateDirectory(newFile.Directory.FullName)
                End If

                ' Copy files.
                fileItem.Fileinfo.CopyTo(newFile.FullName)
            End If
        Next fileItem
    End Sub

#End Region

    ''' <summary>
    ''' Cancel save project to another directory.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub btnCancel_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnCancel.Click
        ' Cancel and close this windows.
        Me.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.Close()
    End Sub
End Class