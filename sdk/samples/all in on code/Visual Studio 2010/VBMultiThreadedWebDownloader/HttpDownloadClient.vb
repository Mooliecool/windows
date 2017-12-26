'************************** Module Header ******************************'
' Module Name:  HttpDownloadClient.vb
' Project:      VBMultiThreadedWebDownloader
' Copyright (c) Microsoft Corporation.
' 
' This class is used to download files through internet.  It supplies public
' methods to Start, Pause, Resume and Cancel a download. 
' 
' There are two methods to start a download: Download and BeginDownload. The 
' BeginDownload method will download  the file in a background thread using 
' ThreadPool.
' 
' When the download starts, it will check whether the destination file exists. If
' not, create a file with the same size as the file to be downloaded, then
' download the it. The downloaded data is stored in a MemoryStream first, and 
' then written to local file.
' 
' It will fire a DownloadProgressChanged event when it has downloaded a
' specified size of data. It will also fire a DownloadCompleted event if the 
' download is completed or canceled.
' 
' The property DownloadedSize stores the size of downloaded data which will be 
' used to Resume the download.
' 
' The property StartPoint can be used in the multi-thread download scenario, and
' every thread starts to download a specific block of the whole file. 
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
Imports System.Net
Imports System.Text.RegularExpressions
Imports System.Threading


Public Class HttpDownloadClient
    Implements IDownloader
    ' Used when creates or writes a file.
    Private Shared _fileLocker As New Object()

    Private _statusLocker As New Object()


    ' The Url of the file to be downloaded.
    Public Property Url() As Uri Implements IDownloader.Url

    ' The local path to store the file.
    ' If there is no file with the same name, a new file will be created.
    Public Property DownloadPath() As String Implements IDownloader.DownloadPath

    ' Ask the server for the file size and store it.
    ' Use the CheckUrl method to initialize this property internally.
    Public Property TotalSize() As Long Implements IDownloader.TotalSize

    Public Property Credentials() As ICredentials Implements IDownloader.Credentials

    Public Property Proxy() As IWebProxy Implements IDownloader.Proxy

    ''' <summary>
    ''' Specify whether the remote server supports "Accept-Ranges" header.
    ''' Use the CheckUrl method to initialize this property internally.
    ''' </summary>
    Public Property IsRangeSupported() As Boolean Implements IDownloader.IsRangeSupported

    ' The properties StartPoint and EndPoint can be used in the multi-thread download scenario, and
    ' every thread starts to download a specific block of the whole file. 
    Public Property StartPoint() As Long Implements IDownloader.StartPoint

    Public Property EndPoint() As Long Implements IDownloader.EndPoint


    ' The size of downloaded data that has been writen to local file.
    Private _downloadedSize As Long
    Public Property DownloadedSize() As Long Implements IDownloader.DownloadedSize
        Get
            Return _downloadedSize
        End Get
        Private Set(ByVal value As Long)
            _downloadedSize = value
        End Set
    End Property

    Private _cachedSize As Integer
    Public Property CachedSize() As Integer Implements IDownloader.CachedSize
        Get
            Return _cachedSize
        End Get
        Private Set(ByVal value As Integer)
            _cachedSize = value
        End Set
    End Property

    Public Property HasChecked() As Boolean Implements IDownloader.HasChecked


    Private _status As DownloadStatus

    ' If status changed, fire StatusChanged event.
    Public Property Status() As DownloadStatus Implements IDownloader.Status
        Get
            Return _status
        End Get
        Private Set(ByVal value As DownloadStatus)
            SyncLock _statusLocker
                If _status <> value Then
                    _status = value
                    Me.OnStatusChanged(EventArgs.Empty)
                End If
            End SyncLock
        End Set
    End Property

    ' Store the used time spent in downloading data. The value does not include
    ' the paused time and it will only be updated when the download is paused, 
    ' canceled or completed.
    Private _usedTime As New TimeSpan()

    Private _lastStartTime As Date

    ''' <summary>
    ''' If the status is Downloading, then the total time is usedTime. Else the total 
    ''' should include the time used in current download thread.
    ''' </summary>
    Public ReadOnly Property TotalUsedTime() As TimeSpan Implements IDownloader.TotalUsedTime
        Get
            If Me.Status <> DownloadStatus.Downloading Then
                Return _usedTime
            Else
                Return _usedTime.Add(Date.Now.Subtract(_lastStartTime))
            End If
        End Get
    End Property

    ' The time and size in last DownloadProgressChanged event. These two fields
    ' are used to calculate the download speed.
    Private _lastNotificationTime As Date
    Private _lastNotificationDownloadedSize As Int64

    ' If get a number of buffers, then fire DownloadProgressChanged event.
    Public Property BufferCountPerNotification() As Integer _
        Implements IDownloader.BufferCountPerNotification

    ' Set the BufferSize when read data in Response Stream.
    Public Property BufferSize() As Integer Implements IDownloader.BufferSize

    ' The cache size in memory.
    Public Property MaxCacheSize() As Integer Implements IDownloader.MaxCacheSize

    Public Event DownloadProgressChanged As EventHandler(Of DownloadProgressChangedEventArgs) _
        Implements IDownloader.DownloadProgressChanged

    Public Event DownloadCompleted As EventHandler(Of DownloadCompletedEventArgs) _
        Implements IDownloader.DownloadCompleted

    Public Event StatusChanged As EventHandler Implements IDownloader.StatusChanged

    ''' <summary>
    ''' Download the whole file.
    ''' </summary>
    Public Sub New(ByVal url As String)
        Me.New(url, 0)
    End Sub

    ''' <summary>
    ''' Download the file from a start point to the end.
    ''' </summary>
    Public Sub New(ByVal url As String, ByVal startPoint As Long)
        Me.New(url, startPoint, Integer.MaxValue)
    End Sub

    ''' <summary>
    ''' Download a block of the file. The default buffer size is 1KB, memory cache is
    ''' 1MB, and buffer count per notification is 64.
    ''' </summary>
    Public Sub New(ByVal url As String, ByVal startPoint As Long, ByVal endPoint As Long)
        Me.New(url, startPoint, endPoint, 1024, 1048576, 64)
    End Sub

    Public Sub New(ByVal url As String, ByVal startPoint As Long,
                   ByVal endPoint As Long, ByVal bufferSize As Integer,
                   ByVal cacheSize As Integer, ByVal bufferCountPerNotification As Integer)

        Me.StartPoint = startPoint
        Me.EndPoint = endPoint
        Me.BufferSize = bufferSize
        Me.MaxCacheSize = cacheSize
        Me.BufferCountPerNotification = bufferCountPerNotification

        Me.Url = New Uri(url, UriKind.Absolute)

        ' Set the default value of IsRangeSupported.
        Me.IsRangeSupported = True

        ' Set the Initialized status.
        Me._status = DownloadStatus.Initialized
    End Sub



    ''' <summary>
    ''' Check the Uri to find its size, and whether it supports "Pause". 
    ''' </summary>
    Public Sub CheckUrl(<System.Runtime.InteropServices.Out()> ByRef fileName As String) _
        Implements IDownloader.CheckUrl
        fileName = DownloaderHelper.CheckUrl(Me)
    End Sub


    ''' <summary>
    ''' Check whether the destination file exists. If not, create a file with the same
    ''' size as the file to be downloaded.
    ''' </summary>
    Private Sub CheckFileOrCreateFile()
        DownloaderHelper.CheckFileOrCreateFile(Me, _fileLocker)
    End Sub

    Private Sub CheckUrlAndFile(<System.Runtime.InteropServices.Out()> ByRef fileName As String)
        CheckUrl(fileName)
        CheckFileOrCreateFile()

        Me.HasChecked = True
    End Sub

    Private Sub EnsurePropertyValid()
        If Me.StartPoint < 0 Then
            Throw New ArgumentOutOfRangeException("StartPoint cannot be less then 0. ")
        End If

        If Me.EndPoint < Me.StartPoint Then
            Throw New ArgumentOutOfRangeException("EndPoint cannot be less then StartPoint ")
        End If

        If Me.BufferSize < 0 Then
            Throw New ArgumentOutOfRangeException("BufferSize cannot be less then 0. ")
        End If

        If Me.MaxCacheSize < Me.BufferSize Then
            Throw New ArgumentOutOfRangeException("MaxCacheSize cannot be less then BufferSize ")
        End If

        If Me.BufferCountPerNotification <= 0 Then
            Throw New ArgumentOutOfRangeException("BufferCountPerNotification cannot be less then 0. ")
        End If
    End Sub


    ''' <summary>
    ''' Start to download.
    ''' </summary>
    Public Sub Download() Implements IDownloader.Download

        ' Only idle download client can be started.
        If Me.Status <> DownloadStatus.Initialized Then
            Throw New ApplicationException("Only Initialized download client can be started.")
        End If

        Me.Status = DownloadStatus.Waiting

        ' Start to download in the same thread.
        DownloadInternal(Nothing)
    End Sub


    ''' <summary>
    ''' Start to download using ThreadPool.
    ''' </summary>
    Public Sub BeginDownload() Implements IDownloader.BeginDownload

        ' Only idle download client can be started.
        If Me.Status <> DownloadStatus.Initialized Then
            Throw New ApplicationException("Only Initialized download client can be started.")
        End If

        Me.Status = DownloadStatus.Waiting

        ThreadPool.QueueUserWorkItem(AddressOf DownloadInternal)
    End Sub

    ''' <summary>
    ''' Pause the download.
    ''' </summary>
    Public Sub Pause() Implements IDownloader.Pause
        ' Only idle or downloading client can be paused.
        Select Case Me.Status
            Case DownloadStatus.Downloading
                Me.Status = DownloadStatus.Pausing
            Case Else
                Throw New ApplicationException("Only downloading client can be paused.")
        End Select
    End Sub

    ''' <summary>
    ''' Resume the download.
    ''' </summary>
    Public Sub [Resume]() Implements IDownloader.Resume
        ' Only paused client can be resumed.
        If Me.Status <> DownloadStatus.Paused Then
            Throw New ApplicationException("Only paused client can be resumed.")
        End If

        Me.Status = DownloadStatus.Waiting

        ' Start to download in the same thread.
        DownloadInternal(Nothing)
    End Sub

    ''' <summary>
    ''' Resume the download using ThreadPool.
    ''' </summary>
    Public Sub BeginResume() Implements IDownloader.BeginResume
        ' Only paused client can be resumed.
        If Me.Status <> DownloadStatus.Paused Then
            Throw New ApplicationException("Only paused client can be resumed.")
        End If

        Me.Status = DownloadStatus.Waiting

        ThreadPool.QueueUserWorkItem(AddressOf DownloadInternal)
    End Sub

    ''' <summary>
    ''' Cancel the download.
    ''' </summary>
    Public Sub Cancel() Implements IDownloader.Cancel
        If Me.Status = DownloadStatus.Initialized _
            OrElse Me.Status = DownloadStatus.Waiting _
            OrElse Me.Status = DownloadStatus.Completed _
            OrElse Me.Status = DownloadStatus.Paused _
            OrElse Me.Status = DownloadStatus.Canceled Then
            Me.Status = DownloadStatus.Canceled
        ElseIf Me.Status = DownloadStatus.Canceling _
            OrElse Me.Status = DownloadStatus.Pausing _
            OrElse Me.Status = DownloadStatus.Downloading Then
            Me.Status = DownloadStatus.Canceling
        End If
    End Sub


    ''' <summary>
    ''' Download the data using HttpWebRequest. It will read a buffer of bytes from the
    ''' response stream, and store the buffer to a MemoryStream cache first.
    ''' If the cache is full, or the download is paused, canceled or completed, write
    ''' the data in cache to local file.
    ''' </summary>
    Private Sub DownloadInternal(ByVal obj As Object)

        If Me.Status <> DownloadStatus.Waiting Then
            Return
        End If

        Dim webRequest As HttpWebRequest = Nothing
        Dim webResponse As HttpWebResponse = Nothing
        Dim responseStream As Stream = Nothing
        Dim downloadCache As MemoryStream = Nothing
        Me._lastStartTime = Date.Now

        Try

            If Not HasChecked Then
                Dim filename As String = String.Empty
                CheckUrlAndFile(filename)
            End If

            Me.EnsurePropertyValid()

            ' Set the status.
            Me.Status = DownloadStatus.Downloading

            ' Create a request to the file to be  downloaded.
            webRequest = DownloaderHelper.InitializeHttpWebRequest(Me)

            ' Specify the block to download.
            If EndPoint <> Integer.MaxValue Then
                webRequest.AddRange(StartPoint + DownloadedSize, EndPoint)
            Else
                webRequest.AddRange(StartPoint + DownloadedSize)
            End If

            ' Retrieve the response from the server and get the response stream.
            webResponse = CType(webRequest.GetResponse(), HttpWebResponse)

            responseStream = webResponse.GetResponseStream()


            ' Cache data in memory.
            downloadCache = New MemoryStream(Me.MaxCacheSize)

            Dim downloadBuffer(Me.BufferSize - 1) As Byte

            Dim bytesSize As Integer = 0
            CachedSize = 0
            Dim receivedBufferCount As Integer = 0

            ' Download the file until the download is paused, canceled or completed.
            Do

                ' Read a buffer of data from the stream.
                bytesSize = responseStream.Read(downloadBuffer, 0, downloadBuffer.Length)

                ' If the cache is full, or the download is paused, canceled or 
                ' completed, write the data in cache to local file.
                If Me.Status <> DownloadStatus.Downloading OrElse bytesSize = 0 _
                    OrElse Me.MaxCacheSize < CachedSize + bytesSize Then

                    Try
                        ' Write the data in cache to local file.
                        WriteCacheToFile(downloadCache, CachedSize)

                        Me.DownloadedSize += CachedSize

                        ' Stop downloading the file if the download is paused, 
                        ' canceled or completed. 
                        If Me.Status <> DownloadStatus.Downloading OrElse bytesSize = 0 Then
                            Exit Do
                        End If

                        ' Reset cache.
                        downloadCache.Seek(0, SeekOrigin.Begin)
                        CachedSize = 0
                    Catch ex As Exception
                        ' Fire the DownloadCompleted event with the error.
                        Me.OnDownloadCompleted(New DownloadCompletedEventArgs(
                                               Nothing, Me.DownloadedSize,
                                               Me.TotalSize,
                                               Me.TotalUsedTime,
                                               ex))
                        Return
                    End Try

                End If

                ' Write the data from the buffer to the cache in memory.
                downloadCache.Write(downloadBuffer, 0, bytesSize)

                CachedSize += bytesSize

                receivedBufferCount += 1

                ' Fire the DownloadProgressChanged event.
                If receivedBufferCount = Me.BufferCountPerNotification Then
                    InternalDownloadProgressChanged(CachedSize)
                    receivedBufferCount = 0
                End If
            Loop


            ' Update the used time when the current doanload is stopped.
            _usedTime = _usedTime.Add(Date.Now.Subtract(_lastStartTime))

            ' Update the status of the client. Above loop will be stopped when the 
            ' status of the client is pausing, canceling or completed.
            If Me.Status = DownloadStatus.Pausing Then
                Me.Status = DownloadStatus.Paused
            ElseIf Me.Status = DownloadStatus.Canceling Then
                Me.Status = DownloadStatus.Canceled
            Else
                Me.Status = DownloadStatus.Completed
                Return
            End If

        Catch ex As Exception
            ' Fire the DownloadCompleted event with the error.
            Me.OnDownloadCompleted(New DownloadCompletedEventArgs(
                                   Nothing, Me.DownloadedSize,
                                   Me.TotalSize, Me.TotalUsedTime, ex))
            Return
        Finally
            ' When the above code has ended, close the streams.
            If responseStream IsNot Nothing Then
                responseStream.Close()
            End If
            If webResponse IsNot Nothing Then
                webResponse.Close()
            End If
            If downloadCache IsNot Nothing Then
                downloadCache.Close()
            End If
        End Try
    End Sub


    ''' <summary>
    ''' Write the data in cache to local file.
    ''' </summary>
    Private Sub WriteCacheToFile(ByVal downloadCache As MemoryStream, ByVal cachedSize As Integer)
        ' Lock other threads or processes to prevent from writing data to the file.
        SyncLock _fileLocker
            Using fileStream As New FileStream(DownloadPath, FileMode.Open)
                Dim cacheContent(cachedSize - 1) As Byte
                downloadCache.Seek(0, SeekOrigin.Begin)
                downloadCache.Read(cacheContent, 0, cachedSize)
                fileStream.Seek(DownloadedSize + StartPoint, SeekOrigin.Begin)
                fileStream.Write(cacheContent, 0, cachedSize)
            End Using
        End SyncLock
    End Sub

    ''' <summary>
    ''' The method will be called by the OnStatusChanged method.
    ''' </summary>
    ''' <param name="e"></param>
    Protected Overridable Sub OnDownloadCompleted(ByVal e As DownloadCompletedEventArgs)
        If e.Error IsNot Nothing AndAlso Me._status <> DownloadStatus.Canceled Then
            Me.Status = DownloadStatus.Completed
        End If

        RaiseEvent DownloadCompleted(Me, e)
    End Sub

    ''' <summary>
    ''' Calculate the download speed and fire the  DownloadProgressChanged event.
    ''' </summary>
    ''' <param name="cachedSize"></param>
    Private Sub InternalDownloadProgressChanged(ByVal cachedSize As Integer)
        Dim speed As Integer = 0
        Dim current As Date = Date.Now
        Dim interval As TimeSpan = current.Subtract(_lastNotificationTime)

        If interval.TotalSeconds < 60 Then
            speed = CInt(Fix(Math.Floor((Me.DownloadedSize + cachedSize _
                                         - Me._lastNotificationDownloadedSize) / interval.TotalSeconds)))
        End If
        _lastNotificationTime = current
        _lastNotificationDownloadedSize = Me.DownloadedSize + cachedSize

        Me.OnDownloadProgressChanged(New DownloadProgressChangedEventArgs(
                                     Me.DownloadedSize + cachedSize, Me.TotalSize, speed))


    End Sub

    Protected Overridable Sub OnDownloadProgressChanged(ByVal e As DownloadProgressChangedEventArgs)
        RaiseEvent DownloadProgressChanged(Me, e)
    End Sub

    Protected Overridable Sub OnStatusChanged(ByVal e As EventArgs)
        Select Case Me.Status
            Case DownloadStatus.Waiting,
                DownloadStatus.Downloading,
                DownloadStatus.Paused,
                DownloadStatus.Canceled,
                DownloadStatus.Completed
                RaiseEvent StatusChanged(Me, e)
            Case Else
        End Select

        If Me._status = DownloadStatus.Canceled Then
            Dim ex As New Exception("Downloading is canceled by user's request. ")
            Me.OnDownloadCompleted(New DownloadCompletedEventArgs(
                                   Nothing, Me.DownloadedSize,
                                   Me.TotalSize, Me.TotalUsedTime, ex))
        End If

        If Me.Status = DownloadStatus.Completed Then
            Me.OnDownloadCompleted(New DownloadCompletedEventArgs(
                                   New FileInfo(Me.DownloadPath),
                                   Me.DownloadedSize, Me.TotalSize,
                                   Me.TotalUsedTime, Nothing))
        End If
    End Sub
End Class

