'************************** Module Header ******************************'
' Module Name:  MainForm.vb
' Project:      VBWebDownloader
' Copyright (c) Microsoft Corporation.
' 
' This is the main form of this application. It is used to initialize the UI and 
' handle the events.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System.IO
Imports System.Linq


Partial Public Class MainForm
    Inherits Form
    Private _downloader As IDownloader = Nothing

    Private _lastNotificationTime As Date

    Public Property FileToDownload() As String
        Get
            Return tbURL.Text
        End Get
        Set(ByVal value As String)
            tbURL.Text = value
        End Set
    End Property

    Public Property DownloadPath() As String
        Get
            Return tbPath.Text
        End Get
        Set(ByVal value As String)
            tbPath.Text = value
        End Set
    End Property

    Public Sub New()
        InitializeComponent()

        Dim args() As String = Environment.GetCommandLineArgs()
        If args.Length > 1 Then
            Dim url As Uri = Nothing
            Dim result As Boolean = Uri.TryCreate(args.Last(), UriKind.Absolute, url)
            If result Then
                Me.FileToDownload = url.ToString()
            End If
        End If
    End Sub

    Private Sub btnCheckUrl_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnCheckUrl.Click
        ' Initialize an instance of HttpDownloadClient.
        _downloader = New HttpDownloadClient(tbURL.Text)

        ' Register the events of HttpDownloadClient.
        AddHandler _downloader.DownloadCompleted, AddressOf DownloadCompleted
        AddHandler _downloader.DownloadProgressChanged, AddressOf DownloadProgressChanged
        AddHandler _downloader.StatusChanged, AddressOf StatusChanged

        Try
            Dim filename As String = String.Empty
            _downloader.CheckUrl(filename)

            If String.IsNullOrEmpty(filename) Then
                Me.DownloadPath = String.Format( _
                    "{0}\{1}", _
                    Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), _
                    _downloader.Url.Segments.Last())
            Else
                Me.DownloadPath = String.Format( _
                    "{0}\{1}", _
                    Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), _
                    filename)
            End If

            Me.tbURL.Enabled = False
            Me.btnCheckUrl.Enabled = False

            Me.tbPath.Enabled = True
            Me.btnDownload.Enabled = True


        Catch
            ' If there is any exception, like System.Net.WebException or 
            ' System.Net.ProtocolViolationException, it means that there may be an 
            ' error while reading the information of the file and it cannot be 
            ' downloaded. 
            MessageBox.Show("There is an error while get the information of the file." _
                            & " Please make sure the url is accessible.")
        End Try
    End Sub

    ''' <summary>
    ''' Handle btnDownload Click event.
    ''' </summary>
    Private Sub btnDownload_Click(ByVal sender As Object, ByVal e As EventArgs) _
        Handles btnDownload.Click

        Try

            ' Check whether the file exists.
            If File.Exists(tbPath.Text.Trim()) Then
                Dim message As String = "There is already a file with the same name, " _
                                        & "do you want to delete it? " & "If not, please change the local path. "
                Dim result = MessageBox.Show(message, "File name conflict: " _
                                             & tbPath.Text.Trim(), MessageBoxButtons.OKCancel)

                If result = DialogResult.OK Then
                    File.Delete(tbPath.Text.Trim())
                Else
                    Return
                End If
            End If

            ' Construct the temporary file path.
            Dim tempPath As String = tbPath.Text.Trim() & ".tmp"

            ' Delete the temporary file if it already exists.
            If File.Exists(tempPath) Then
                File.Delete(tempPath)
            End If

            ' Initialize an instance of HttpDownloadClient.
            ' Store the file to a temporary file first.
            _downloader.DownloadPath = tempPath



            ' Start to download file.
            _downloader.BeginDownload()
        Catch ex As Exception
            MessageBox.Show(ex.Message)
        End Try

    End Sub

    ''' <summary>
    ''' Handle StatusChanged event.
    ''' </summary>
    Private Sub StatusChanged(ByVal sender As Object, ByVal e As EventArgs)
        Me.Invoke(New EventHandler(AddressOf StatusChangedHanlder), sender, e)
    End Sub


    Private Sub StatusChangedHanlder(ByVal sender As Object, ByVal e As EventArgs)
        ' Refresh the status.
        lbStatus.Text = _downloader.Status.ToString()

        ' Refresh the buttons.
        Select Case _downloader.Status
            Case DownloadStatus.Waiting
                btnCheckUrl.Enabled = False
                btnCancel.Enabled = True
                btnDownload.Enabled = False
                btnPause.Enabled = False
                tbPath.Enabled = False
                tbURL.Enabled = False
            Case DownloadStatus.Canceled, DownloadStatus.Completed
                btnCheckUrl.Enabled = True
                btnCancel.Enabled = True
                btnDownload.Enabled = False
                btnPause.Enabled = False
                tbPath.Enabled = False
                tbURL.Enabled = True
            Case DownloadStatus.Downloading
                btnCheckUrl.Enabled = False
                btnCancel.Enabled = True
                btnDownload.Enabled = False
                btnPause.Enabled = True And _downloader.IsRangeSupported
                tbPath.Enabled = False
                tbURL.Enabled = False
            Case DownloadStatus.Paused
                btnCheckUrl.Enabled = False
                btnCancel.Enabled = True
                btnDownload.Enabled = False

                ' The "Resume" button.
                btnPause.Enabled = True And _downloader.IsRangeSupported
                tbPath.Enabled = False
                tbURL.Enabled = False
        End Select

        If _downloader.Status = DownloadStatus.Paused Then
            lbSummary.Text = String.Format( _
                "Received: {0}KB, Total: {1}KB, Time: {2}:{3}:{4}", _
                _downloader.DownloadedSize \ 1024, _
                _downloader.TotalSize \ 1024, _
                _downloader.TotalUsedTime.Hours, _
                _downloader.TotalUsedTime.Minutes, _
                _downloader.TotalUsedTime.Seconds)

            btnPause.Text = "Resume"
        Else
            btnPause.Text = "Pause"
        End If


    End Sub



    ''' <summary>
    ''' Handle DownloadProgressChanged event.
    ''' </summary>
    Private Sub DownloadProgressChanged(ByVal sender As Object, _
                                        ByVal e As DownloadProgressChangedEventArgs)
        Me.Invoke(New EventHandler(Of DownloadProgressChangedEventArgs)(AddressOf DownloadProgressChangedHanlder), sender, e)
    End Sub

    Private Sub DownloadProgressChangedHanlder(ByVal sender As Object, _
                                               ByVal e As DownloadProgressChangedEventArgs)
        ' Refresh the summary every second.
        If Date.Now > _lastNotificationTime.AddSeconds(1) Then
            lbSummary.Text = String.Format( _
                "Received: {0}KB, Total: {1}KB, Speed: {2}KB/s", _
                e.ReceivedSize \ 1024, _
                e.TotalSize \ 1024, _
                e.DownloadSpeed \ 1024)
            prgDownload.Value = CInt(e.ReceivedSize * 100 \ e.TotalSize)
            _lastNotificationTime = Date.Now
        End If
    End Sub


    ''' <summary>
    ''' Handle DownloadCompleted event.
    ''' </summary>
    Private Sub DownloadCompleted(ByVal sender As Object, ByVal e As DownloadCompletedEventArgs)
        Me.Invoke(New EventHandler(Of DownloadCompletedEventArgs)(AddressOf DownloadCompletedHanlder), sender, e)
    End Sub

    Private Sub DownloadCompletedHanlder(ByVal sender As Object, _
                                         ByVal e As DownloadCompletedEventArgs)
        If e.Error Is Nothing Then
            lbSummary.Text = String.Format( _
                "Received: {0}KB, Total: {1}KB, Time: {2}:{3}:{4}", _
                e.DownloadedSize \ 1024, _
                e.TotalSize \ 1024, _
                e.TotalTime.Hours, _
                e.TotalTime.Minutes, _
                e.TotalTime.Seconds)

            If File.Exists(tbPath.Text.Trim()) Then
                File.Delete(tbPath.Text.Trim())
            End If

            File.Move(tbPath.Text.Trim() & ".tmp", tbPath.Text.Trim())
            prgDownload.Value = 100
        Else
            lbSummary.Text = e.Error.Message
            If File.Exists(tbPath.Text.Trim() & ".tmp") Then
                File.Delete(tbPath.Text.Trim() & ".tmp")
            End If

            If File.Exists(tbPath.Text.Trim()) Then
                File.Delete(tbPath.Text.Trim())
            End If

            prgDownload.Value = 0
        End If
    End Sub

    ''' <summary>
    ''' Handle btnCancel Click event.
    ''' </summary>
    Private Sub btnCancel_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnCancel.Click
        If _downloader IsNot Nothing Then
            _downloader.Cancel()
        End If

    End Sub

    ''' <summary>
    ''' Handle btnPause Click event.
    ''' </summary>
    Private Sub btnPause_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnPause.Click
        If _downloader.Status = DownloadStatus.Paused Then
            _downloader.BeginResume()
        ElseIf _downloader.Status = DownloadStatus.Downloading Then
            _downloader.Pause()
        End If
    End Sub


End Class

