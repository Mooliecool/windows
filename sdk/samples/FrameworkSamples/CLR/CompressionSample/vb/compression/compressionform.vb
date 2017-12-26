 '-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'-----------------------------------------------------------------------
Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports System.Data
Imports System.Collections.Generic
Imports System.Threading


'/ <summary>
'/ Main form for the application
'/ </summary>

Partial Public Class CompressionForm
    Inherits System.Windows.Forms.Form
    '/ <summary>
    '/ Required designer variable.
    '/ </summary>
    Private components As System.ComponentModel.IContainer = Nothing
    Private cFileDialog As CustomFileDialog ' Dialog item for menu clicks
    Private archive As ZipFile 'Archive for zipping operations
    Public Shared statusMessage As String
    'For the status message to fixed by 
    'other components
    Public Sub New()
        statusMessage = Nothing

        InitializeComponent()
        'Initialize the list view
        fileListView.View = View.Details
        fileListView.GridLines = True
        fileListView.Columns.Add("Name", 150, HorizontalAlignment.Left)
        fileListView.Columns.Add("Modified", 130, HorizontalAlignment.Left)
        fileListView.Columns.Add("Size", 75, HorizontalAlignment.Right)
        fileListView.Columns.Add("Ratio %", 50, HorizontalAlignment.Right)
        fileListView.Columns.Add("Compressed size", 95, HorizontalAlignment.Right)
        fileListView.Columns.Add("Path", 250, HorizontalAlignment.Left)

        EnableControls(False)
        EnableExtractRemoveButtons(False)

        cFileDialog = New CustomFileDialog()

    End Sub 'New


    Private Sub EnableControls(ByVal value As Boolean)
        addMenuStripButton.Enabled = value
        addToolStripButton.Enabled = value
        extractAllMenuStripButton.Enabled = value

    End Sub 'EnableControls


    Private Sub newGzipToolStripMenuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles newGzipToolStripButton.Click, newGzipMenuToolStripButton.Click

        NewArchive(ZipConstants.GZIP)

    End Sub 'newGzipToolStripMenuItem_Click

    Private Sub newDeflateToolStripMenuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles newDeflateToolStripButton.Click, newDeflateMenuToolStripButton.Click

        NewArchive(ZipConstants.DEFLATE)

    End Sub 'newDeflateToolStripMenuItem_Click


    Private Sub NewArchive(ByVal method As Byte)
        Dim name As String = cFileDialog.NewMode()
        If name Is Nothing Then
            fileListView.Focus()
            Return
        End If

        Dim mode As System.IO.FileMode = System.IO.FileMode.CreateNew
        If System.IO.File.Exists(name) Then
            If ShowOverwriteDialog() = MsgBoxResult.Yes Then
                mode = System.IO.FileMode.Truncate
            Else
                fileListView.Focus()
                Return
            End If
        End If

        If Not (archive Is Nothing) Then
            archive.Close()
        End If
        archive = New ZipFile(name, method, mode)
        Clear()

        If statusMessage.Length <> 0 Then
            DisplayStatusMessage()
            ChangeTitle(name, archive.CompressionMethod())
            EnableControls(True)
        Else
            Clear()
        End If

    End Sub 'NewArchive


    Private Function ShowOverwriteDialog() As DialogResult
        Dim opt As MessageBoxOptions
        If System.Threading.Thread.CurrentThread.CurrentUICulture.TextInfo.IsRightToLeft = True Then
            opt = MessageBoxOptions.RightAlign Or MessageBoxOptions.RtlReading
        Else
            opt = MessageBoxOptions.DefaultDesktopOnly
        End If
        Return MessageBox.Show(ZipConstants.FileReplace, ZipConstants.Replace, MessageBoxButtons.YesNo, MessageBoxIcon.None, MessageBoxDefaultButton.Button1, opt)

    End Function 'ShowOverwriteDialog


    Private Sub openToolStripMenuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles openToolStripButton.Click, openMenuStripButton.Click

        Dim name As String = cFileDialog.OpenMode()
        If name Is Nothing Then
            fileListView.Focus()
            Return
        End If
        If Not (archive Is Nothing) Then
            archive.Close()
        End If
        archive = New ZipFile(name)
        Clear()

        If statusMessage.Length <> 0 Then
            DisplayStatusMessage()
            ChangeTitle(name, archive.CompressionMethod())
            EnableControls(True)
        Else
            Clear()
        End If

    End Sub 'openToolStripMenuItem_Click


    Private Sub closeToolStripMenuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles closeMenuStripButton.Click

        Me.Close()

    End Sub 'closeToolStripMenuItem_Click

    Private Sub removeToolStripMenuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles removeToolStripButton.Click, removeMenuStripButton.Click, removeContextMenuStripButton.Click


        Dim index As Integer
        For Each index In fileListView.SelectedIndices
            archive.Remove(index)
        Next index
        If fileListView.SelectedIndices.Count = 0 Then
            statusMessage = String.Empty
        End If
        RefreshListView()
        DisplayStatusMessage()

    End Sub 'removeToolStripMenuItem_Click


    Private Sub addToolStripMenuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles addToolStripButton.Click, addMenuStripButton.Click


        Dim names As String() = cFileDialog.AddMode()
        If names Is Nothing Then
            fileListView.Focus()
            Return
        End If
        Dim name As String
        For Each name In names
            Dim index As Integer = archive.CheckFileExists(name)
            If index <> -1 Then
                statusMessage = ZipConstants.FileExistsError
                RefreshListView()
                DisplayStatusMessage()
                fileListView.Focus()
                fileListView.Items(index).Selected = True
            Else
                archive.Add(name)
                RefreshListView()
                DisplayStatusMessage()
            End If
        Next name

    End Sub 'addToolStripMenuItem_Click


    Private Sub extractToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles extractToolStripButton.Click, extractMenuStripButton.Click

        'string dir = GetFolderName();
        CreateDir()
        Dim index As Integer
        For Each index In fileListView.SelectedIndices
            archive.Extract(index, "C:\temp\zipTemp")
        Next index

        If fileListView.SelectedIndices.Count = 0 Then
            statusMessage = String.Empty
        End If
        DisplayStatusMessage()

    End Sub 'extractToolStripMenuItem_Click


    Private Sub extractAllToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles extractAllMenuStripButton.Click


        CreateDir()
        archive.ExtractAll("C:\temp\zipTemp")
        DisplayStatusMessage()

    End Sub 'extractAllToolStripMenuItem_Click


    Private Sub RefreshListView()
        Dim entries As List(Of ZipEntry)
        entries = archive.Entries
        If entries Is Nothing Then
            Return
        End If
        fileListView.Items.Clear()

        EnableExtractRemoveButtons(False)

        Dim entry As ZipEntry
        For Each entry In entries
            Dim index As Integer = entry.Name.LastIndexOf(ZipConstants.BackSlash)
            entry.Name.Substring(0, index)
            Dim lvi As New ListViewItem(entry.Name.Substring(index + 1))
            lvi.SubItems.Add(entry.ModifiedDateTime.ToString(Thread.CurrentThread.CurrentUICulture))

            lvi.SubItems.Add(entry.Size.ToString(Thread.CurrentThread.CurrentUICulture))


            Dim ratio As Integer
            If (entry.Size = 0) Then
                ratio = 0
            Else
                ratio = System.Convert.ToInt32((System.Convert.ToDouble(entry.Size - entry.CompressedSize) / entry.Size * 100))
            End If

            ratio = IIf(ratio < 0, 0, ratio) 'TODO: For performance reasons this should be changed to nested IF statements

            lvi.SubItems.Add(ratio.ToString(Thread.CurrentThread.CurrentUICulture))

            lvi.SubItems.Add(entry.CompressedSize.ToString(Thread.CurrentThread.CurrentUICulture))

            lvi.SubItems.Add(entry.Name.Substring(0, index))
            fileListView.Items.Add(lvi)
        Next entry

    End Sub 'RefreshListView

    Public Sub DisplayStatusMessage()
        'mainStatusStripPanel.Text = statusMessage

    End Sub 'DisplayStatusMessage


    Private Sub ChangeTitle(ByVal name As String, ByVal method As Byte)
        Dim index As Integer = name.LastIndexOf(ZipConstants.BackSlash)
        Dim methodName As String = Nothing
        If method = ZipConstants.GZIP Then
            methodName = ZipConstants.GzipName
        End If
        If method = ZipConstants.DEFLATE Then
            methodName = ZipConstants.DeflateName
        End If
        Me.Text = ZipConstants.Title + "-" + name.Substring(index + 1) + " (" + methodName + ")"

    End Sub 'ChangeTitle


    Private Sub Clear()
        Me.Text = ZipConstants.Title
        EnableExtractRemoveButtons(False)
        EnableControls(False)
        fileListView.Items.Clear()
        RefreshListView()

    End Sub 'Clear


    Private Function GetFolderName() As String
        Dim dir As String = String.Empty
        Dim fbd As New FolderBrowserDialog()
        If fbd.ShowDialog() = MsgBoxResult.OK Then
            dir = fbd.SelectedPath
        End If
        Return dir

    End Function 'GetFolderName


    Private Sub CreateDir()
        If System.IO.Directory.Exists("C:\temp\zipTemp") Then
            Return
        End If
        System.IO.Directory.CreateDirectory("C:\temp\zipTemp")

    End Sub 'CreateDir


    Private Sub fileListView_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles fileListView.SelectedIndexChanged
        If fileListView.SelectedIndices.Count > 0 Then
            EnableExtractRemoveButtons(True)
        Else
            EnableExtractRemoveButtons(False)
        End If

    End Sub 'fileListView_SelectedIndexChanged


    Private Sub contextMenuStripButton_Click(ByVal sender As Object, ByVal e As EventArgs) Handles contextMenuStripButton.Click


        extractToolStripMenuItem_Click(sender, e)

    End Sub 'contextMenuStripButton_Click

    Private Sub fileListView_MouseUp(ByVal sender As Object, ByVal e As MouseEventArgs) Handles fileListView.MouseUp
        If e.Button <> System.Windows.Forms.MouseButtons.Right OrElse fileListView.SelectedIndices.Count = 0 Then
            Return
        End If
        Dim info As ListViewHitTestInfo = fileListView.HitTest(e.X, e.Y)
        If info.Item Is Nothing Then
            Return
        End If
        fileContextMenuStrip.Show(fileListView, e.X, e.Y)

    End Sub 'fileListView_MouseUp


    Private Sub EnableExtractRemoveButtons(ByVal value As Boolean)
        extractMenuStripButton.Enabled = value
        extractToolStripButton.Enabled = value
        removeToolStripButton.Enabled = value
        removeMenuStripButton.Enabled = value

    End Sub 'EnableExtractRemoveButtons
End Class 'CompressionSample
