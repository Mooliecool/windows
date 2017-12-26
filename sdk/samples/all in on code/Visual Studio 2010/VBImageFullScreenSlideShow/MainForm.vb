'******************************** Module Header *****************************************'
' Module Name:  MainForm.vb
' Project:      VBImageFullScreenSlideShow
' Copyright (c) Microsoft Corporation.
'
' The sample demonstrates how to Show slides full screen and how to modify the internal of
' these image slides playing.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'****************************************************************************************'

Imports System.IO

Partial Public Class MainForm
    Inherits Form
    Private imageFiles() As String = Nothing

    ' Image index
    Private selected As Integer = 0
    Private begin As Integer = 0
    Private [end] As Integer = 0

    Private fullScreenHelper As New FullScreen()

    Public Sub New()
        InitializeComponent()

        Me.btnPrevious.Enabled = False
        Me.btnNext.Enabled = False
        Me.btnImageSlideShow.Enabled = False
    End Sub

    ''' <summary>
    ''' Select the image folder.
    ''' </summary>
    Private Sub btnOpenFolder_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnOpenFolder.Click
        If Me.imageFolderBrowserDlg.ShowDialog() = DialogResult.OK Then
            Me.imageFiles = GetFiles(Me.imageFolderBrowserDlg.SelectedPath, _
                                     "*.jpg;*.jpeg;*.png;*.bmp;*.tif;*.tiff;*.gif")

            Me.selected = 0
            Me.begin = 0
            Me.end = imageFiles.Length

            If Me.imageFiles.Length = 0 Then
                MessageBox.Show("No image can be found")

                Me.btnPrevious.Enabled = False
                Me.btnNext.Enabled = False
                Me.btnImageSlideShow.Enabled = False
            Else
                ShowImage(imageFiles(selected), pictureBox)

                Me.btnPrevious.Enabled = True
                Me.btnNext.Enabled = True
                Me.btnImageSlideShow.Enabled = True
            End If
        End If
    End Sub

    Public Shared Function GetFiles(ByVal path As String, ByVal searchPattern As String) As String()
        Dim patterns() As String = searchPattern.Split(";"c)
        Dim files As New List(Of String)()
        For Each filter As String In patterns
            ' Iterate through the directory tree and ignore the 
            ' DirectoryNotFoundException or UnauthorizedAccessException 
            ' exceptions. 
            ' http://msdn.microsoft.com/en-us/library/bb513869.aspx

            ' Data structure to hold names of subfolders to be
            ' examined for files.
            Dim dirs As New Stack(Of String)(20)

            If Not Directory.Exists(path) Then
                Throw New ArgumentException()
            End If
            dirs.Push(path)

            Do While dirs.Count > 0
                Dim currentDir As String = dirs.Pop()
                Dim subDirs() As String
                Try
                    subDirs = Directory.GetDirectories(currentDir)
                    ' An UnauthorizedAccessException exception will be thrown 
                    ' if we do not have discovery permission on a folder or 
                    ' file. It may or may not be acceptable to ignore the 
                    ' exception and continue enumerating the remaining files 
                    ' and folders. It is also possible (but unlikely) that a 
                    ' DirectoryNotFound exception will be raised. This will 
                    ' happen if currentDir has been deleted by another 
                    ' application or thread after our call to Directory.Exists. 
                    ' The choice of which exceptions to catch depends entirely 
                    ' on the specific task you are intending to perform and 
                    ' also on how much you know with certainty about the 
                    ' systems on which this code will run.
                Catch e As UnauthorizedAccessException
                    Continue Do
                Catch e As DirectoryNotFoundException
                    Continue Do
                End Try

                Try
                    files.AddRange(Directory.GetFiles(currentDir, filter))
                Catch e As UnauthorizedAccessException
                    Continue Do
                Catch e As DirectoryNotFoundException
                    Continue Do
                End Try

                ' Push the subdirectories onto the stack for traversal.
                ' This could also be done before handing the files.
                For Each str As String In subDirs
                    dirs.Push(str)
                Next str
            Loop
        Next filter

        Return files.ToArray()
    End Function

    ''' <summary>
    ''' Click the "Previous" button to navigate to the previous image.
    ''' </summary>
    Private Sub btnPrevious_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnPrevious.Click
        If Me.imageFiles Is Nothing OrElse Me.imageFiles.Length = 0 Then
            MessageBox.Show("Please select the images to slideshow!")
            Return
        End If
        ShowPrevImage()
    End Sub

    ''' <summary>
    ''' Click the "Next" button to navigate to the next image.
    ''' </summary>
    Private Sub btnNext_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnNext.Click
        If Me.imageFiles Is Nothing OrElse Me.imageFiles.Length = 0 Then
            MessageBox.Show("Please select the images to slideshow!")
            Return
        End If
        ShowNextImage()
    End Sub

    ''' <summary>
    ''' Image slideshow.
    ''' </summary>
    Private Sub btnImageSlideShow_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnImageSlideShow.Click
        If Me.imageFiles Is Nothing OrElse Me.imageFiles.Length = 0 Then
            MessageBox.Show("Please select the images to slideshow!")
            Return
        End If

        If timer.Enabled = True Then
            Me.timer.Enabled = False
            Me.btnOpenFolder.Enabled = True
            Me.btnImageSlideShow.Text = "Start Slideshow"
        Else
            Me.timer.Enabled = True
            Me.btnOpenFolder.Enabled = False
            Me.btnImageSlideShow.Text = "Stop Slideshow"
        End If
    End Sub

    ''' <summary>
    ''' Show the next image at every regular intervals.
    ''' </summary>
    Private Sub timer_Tick(ByVal sender As Object, ByVal e As EventArgs) Handles timer.Tick
        ShowNextImage()
    End Sub

    ''' <summary>
    ''' Show child windows to alternate the settings about Timer control.
    ''' </summary>
    Private Sub btnSetting_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnSetting.Click
        Dim frmSettings As New Setting(timer)
        frmSettings.ShowDialog()
    End Sub

    ''' <summary>
    ''' Enter or leave the full screen mode.
    ''' </summary>
    Private Sub btnFullScreen_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnFullScreen.Click
        If Not Me.fullScreenHelper.IsFullScreen Then
            ' Hide the buttons and max the slideshow panel.
            Me.gbButtons.Visible = False
            Me.pnlSlideShow.Dock = DockStyle.Fill

            fullScreenHelper.EnterFullScreen(Me)
        End If
    End Sub

    ''' <summary>
    ''' Respond to the keystroke "ESC".
    ''' </summary>
    Protected Overrides Function ProcessCmdKey(ByRef msg As Message, ByVal keyData As Keys) As Boolean
        If keyData = Keys.Escape Then
            If Me.fullScreenHelper.IsFullScreen Then
                ' Unhide the buttons and restore the slideshow panel.
                Me.gbButtons.Visible = True
                Me.pnlSlideShow.Dock = DockStyle.Top

                fullScreenHelper.LeaveFullScreen(Me)
            End If
            Return True
        Else
            Return MyBase.ProcessCmdKey(msg, keyData)
        End If
    End Function

    ''' <summary>
    ''' Show the image in the PictureBox.
    ''' </summary>
    Public Shared Sub ShowImage(ByVal path As String, ByVal pct As PictureBox)
        pct.ImageLocation = path
    End Sub

    ''' <summary>
    ''' Show the previous image.
    ''' </summary>
    Private Sub ShowPrevImage()
        Me.selected -= 1
        ShowImage(Me.imageFiles((Me.selected) Mod Me.imageFiles.Length), Me.pictureBox)
    End Sub

    ''' <summary>
    ''' Show the next image.
    ''' </summary>
    Private Sub ShowNextImage()
        Me.selected += 1
        ShowImage(Me.imageFiles((Me.selected) Mod Me.imageFiles.Length), Me.pictureBox)
    End Sub
End Class

