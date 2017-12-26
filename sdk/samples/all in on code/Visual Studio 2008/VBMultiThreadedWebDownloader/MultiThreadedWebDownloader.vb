'************************** Module Header ******************************'
' Module Name:  MultiThreadedWebDownloader.cs
' Project:      VBMultiThreadedWebDownloader
' Copyright (c) Microsoft Corporation.
' 
' This class is used to download files through internet using multiple threads. 
' It supplies public  methods to Start, Pause, Resume and Cancel a download. 
' 
' Before the download starts, the remote server should be checked 
' whether it supports "Accept-Ranges" header.
' 
' When the download starts, it will check whether the destination file exists. If
' not, create a file with the same size as the file to be downloaded, then
' creates multiple HttpDownloadClients to download the file in background threads.
' 
' It will fire a DownloadProgressChanged event when it has downloaded a
' specified size of data. It will also fire a DownloadCompleted event if the 
' download is completed or canceled.
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
Imports System.Net
Imports System.Text.RegularExpressions
Imports System.Threading


Public Class MultiThreadedWebDownloader
    Implements IDownloader
    ' Used while calculating download speed.
    Private Shared _locker As New Object()


    ''' <summary>
    ''' The Url of the file to be downloaded. 
    ''' </summary>
    Private _url As Uri
    Public Property Url() As Uri Implements IDownloader.Url
        Get
            Return _url
        End Get
        Private Set(ByVal value As Uri)
            _url = value
        End Set
    End Property

    Dim _credentials As ICredentials
    Public Property Credentials() As ICredentials Implements IDownloader.Credentials
        Get
            Return _credentials
        End Get
        Set(ByVal value As ICredentials)
            _credentials = value
        End Set
    End Property

    Dim _isRangeSupported As Boolean

    ''' <summary>
    ''' Specify whether the remote server supports "Accept-Ranges" header.
    ''' Use the CheckUrl method to initialize this property internally.
    ''' </summary>
    Public Property IsRangeSupported() As Boolean Implements IDownloader.IsRangeSupported
        Get
            Return _isRangeSupported
        End Get
        Set(ByVal value As Boolean)
            _isRangeSupported = value
        End Set
    End Property

    Dim _totalSize As Long

    ' Ask the server for the file size and store it.
    ' Use the CheckUrl method to initialize this property internally.
    Public Property TotalSize() As Long Implements IDownloader.TotalSize
        Get
            Return _totalSize
        End Get
        Set(ByVal value As Long)
            _totalSize = value
        End Set
    End Property

    Dim _startPoint As Long

    ' The properties StartPoint and EndPoint can be used in the multi-thread download scenario, and
    ' every thread starts to download a specific block of the whole file. 
    Public Property StartPoint() As Long Implements IDownloader.StartPoint
        Get
            Return _startPoint
        End Get
        Set(ByVal value As Long)
            _startPoint = value
        End Set
    End Property

   
    Dim _endPoint As Long

    ''' <summary>
    ''' This property is a member of IDownloader interface.
    ''' </summary>
    Public Property EndPoint() As Long Implements IDownloader.EndPoint
        Get
            Return _endPoint
        End Get
        Set(ByVal value As Long)
            _endPoint = value
        End Set
    End Property

    Dim _downloadPath As String

    ' The local path to store the file.
    ' If there is no file with the same name, a new file will be created.
    Public Property DownloadPath() As String Implements IDownloader.DownloadPath
        Get
            Return _downloadPath
        End Get
        Set(ByVal value As String)
            _downloadPath = value
        End Set
    End Property

    Dim _proxy As IWebProxy

    ''' <summary>
    ''' The Proxy of all the download client.
    ''' </summary>
    Public Property Proxy() As IWebProxy Implements IDownloader.Proxy
        Get
            Return _proxy
        End Get
        Set(ByVal value As IWebProxy)
            _proxy = value
        End Set
    End Property

    ''' <summary>
    ''' The downloaded size of the file.
    ''' </summary>
    Public Property DownloadedSize() As Long Implements IDownloader.DownloadedSize
        Get
            Return Me._downloadClients.Sum(Function(client) client.DownloadedSize)
        End Get
        Set(ByVal value As Long)

        End Set
    End Property

    Public Property CachedSize() As Integer Implements IDownloader.CachedSize
        Get
            Return Me._downloadClients.Sum(Function(client) client.CachedSize)
        End Get
        Set(ByVal value As Integer)

        End Set
    End Property

    ' Store the used time spent in downloading data. The value does not include
    ' the paused time and it will only be updated when the download is paused, 
    ' canceled or completed.
    Private _usedTime As New TimeSpan()

    Private _lastStartTime As Date

    ''' <summary>
    ''' If the status is Downloading, then the total time is usedTime. Else the 
    ''' total should include the time used in current download thread.
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

    Private _lastNotificationDownloadedSize As Long

    Dim _bufferCountPerNotification As Integer

    ' If get a number of buffers, then fire DownloadProgressChanged event.
    Public Property BufferCountPerNotification() As Integer Implements IDownloader.BufferCountPerNotification
        Get
            Return _bufferCountPerNotification
        End Get
        Set(ByVal value As Integer)
            _bufferCountPerNotification = value
        End Set
    End Property

    Dim _bufferSize As Integer

    ' Set the BufferSize when read data in Response Stream.
    Public Property BufferSize() As Integer Implements IDownloader.BufferSize
        Get
            Return _bufferSize
        End Get
        Set(ByVal value As Integer)
            _bufferSize = value
        End Set
    End Property

    Dim _maxCacheSize As Integer

    ' The cache size in memory.
    Public Property MaxCacheSize() As Integer Implements IDownloader.MaxCacheSize
        Get
            Return _maxCacheSize
        End Get
        Set(ByVal value As Integer)
            _maxCacheSize = value
        End Set
    End Property

    Private _status As DownloadStatus

    ''' <summary>
    ''' If status changed, fire StatusChanged event.
    ''' </summary>
    Public Property Status() As DownloadStatus Implements IDownloader.Status
        Get
            Return _status
        End Get

        Private Set(ByVal value As DownloadStatus)
            If _status <> value Then
                _status = value
                Me.OnStatusChanged(EventArgs.Empty)
            End If
        End Set
    End Property

    Dim _maxThreadCount As Integer

    ''' <summary>
    ''' The max threads count. The real threads count number is the min value of this
    ''' value and TotalSize / MaxCacheSize.
    ''' </summary>
    Public Property MaxThreadCount() As Integer
        Get
            Return _maxThreadCount
        End Get
        Set(ByVal value As Integer)
            _maxThreadCount = value
        End Set
    End Property

    Dim _hasChecked As Boolean
    Public Property HasChecked() As Boolean Implements IDownloader.HasChecked
        Get
            Return _hasChecked
        End Get
        Set(ByVal value As Boolean)
            _hasChecked = value
        End Set
    End Property

    ' The HttpDownloadClients to download the file. Each client uses one thread to
    ' download part of the file.
    Private _downloadClients As List(Of HttpDownloadClient) = Nothing

    Public ReadOnly Property DownloadThreadsCount() As Integer
        Get
            If _downloadClients IsNot Nothing Then
                Return _downloadClients.Count
            Else
                Return 0
            End If
        End Get
    End Property

    Public Event DownloadProgressChanged As EventHandler(Of DownloadProgressChangedEventArgs) Implements IDownloader.DownloadProgressChanged

    Public Event DownloadCompleted As EventHandler(Of DownloadCompletedEventArgs) Implements IDownloader.DownloadCompleted

    Public Event StatusChanged As EventHandler Implements IDownloader.StatusChanged

    ''' <summary>
    ''' Download the whole file. The default buffer size is 1KB, memory cache is
    ''' 1MB, buffer count per notification is 64, threads count is the double of 
    ''' logic processors count.
    ''' </summary>
    Public Sub New(ByVal url As String)
        Me.New(url, 1024, 1048576, 64, Environment.ProcessorCount * 2)
    End Sub

    Public Sub New(ByVal url As String, _
                   ByVal bufferSize As Integer, _
                   ByVal cacheSize As Integer, _
                   ByVal bufferCountPerNotification As Integer, _
                   ByVal maxThreadCount As Integer)

        Me.Url = New Uri(url)
        Me.StartPoint = 0
        Me.EndPoint = Long.MaxValue
        Me.BufferSize = bufferSize
        Me.MaxCacheSize = cacheSize
        Me.BufferCountPerNotification = bufferCountPerNotification

        Me.MaxThreadCount = MaxThreadCount

        ' Set the maximum number of concurrent connections allowed by 
        ' a ServicePoint object
        ServicePointManager.DefaultConnectionLimit = MaxThreadCount

        ' Initialize the HttpDownloadClient list.
        _downloadClients = New List(Of HttpDownloadClient)()

        ' Set the Initialized status.
        Me.Status = DownloadStatus.Initialized
    End Sub


    Public Sub CheckUrlAndFile(<System.Runtime.InteropServices.Out()> ByRef fileName As String)
        CheckUrl(fileName)
        CheckFileOrCreateFile()

        Me.HasChecked = True
    End Sub

    ''' <summary>
    ''' Check the Uri to find its size, and whether it supports "Pause". 
    ''' </summary>
    Public Sub CheckUrl(<System.Runtime.InteropServices.Out()> ByRef fileName As String) Implements IDownloader.CheckUrl
        fileName = DownloaderHelper.CheckUrl(Me)
    End Sub

    ''' <summary>
    ''' Check whether the destination file exists. If  not, create a file with the same
    ''' size as the file to be downloaded.
    ''' </summary>
    Private Sub CheckFileOrCreateFile()
        DownloaderHelper.CheckFileOrCreateFile(Me, _locker)
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

        If Me.MaxThreadCount < 1 Then
            Throw New ArgumentOutOfRangeException("maxThreadCount cannot be less than 1. ")
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

        Me.EnsurePropertyValid()

        ' Set the status.
        Me.Status = DownloadStatus.Downloading

        If Not Me.HasChecked Then
            Dim filename As String = Nothing
            Me.CheckUrlAndFile(filename)
        End If

        Dim client As New HttpDownloadClient(Me.Url.AbsoluteUri, _
                                             0, _
                                             Long.MaxValue, _
                                             Me.BufferSize, _
                                             Me.BufferCountPerNotification * Me.BufferSize, _
                                             Me.BufferCountPerNotification)

        ' Set the HasChecked flag, so the client will not check the URL again.
        client.TotalSize = Me.TotalSize
        client.DownloadPath = Me.DownloadPath
        client.HasChecked = True
        client.Credentials = Me.Credentials
        client.Proxy = Me.Proxy

        ' Register the events of HttpDownloadClients.
        AddHandler client.DownloadProgressChanged, AddressOf client_DownloadProgressChanged
        AddHandler client.StatusChanged, AddressOf client_StatusChanged
        AddHandler client.DownloadCompleted, AddressOf client_DownloadCompleted

        Me._downloadClients.Add(client)
        client.Download()
    End Sub



    ''' <summary>
    ''' Start to download.
    ''' </summary>
    Public Sub BeginDownload() Implements IDownloader.BeginDownload

        ' Only idle download client can be started.
        If Me.Status <> DownloadStatus.Initialized Then
            Throw New ApplicationException("Only Initialized download client can be started.")
        End If

        Me.Status = DownloadStatus.Waiting

        ThreadPool.QueueUserWorkItem(AddressOf DownloadInternal)
    End Sub

    Private Sub DownloadInternal(ByVal obj As Object)

        If Me.Status <> DownloadStatus.Waiting Then
            Return
        End If

        Try
            Me.EnsurePropertyValid()

            ' Set the status.
            Me.Status = DownloadStatus.Downloading

            If Not Me.HasChecked Then
                Dim filename As String = Nothing
                Me.CheckUrlAndFile(filename)
            End If



            ' If the file does not support "Accept-Ranges" header, then create one 
            ' HttpDownloadClient to download the file in a single thread, else create
            ' multiple HttpDownloadClients to download the file.
            If Not IsRangeSupported Then
                Dim client As New HttpDownloadClient(Me.Url.AbsoluteUri, _
                                                     0, _
                                                     Long.MaxValue, _
                                                     Me.BufferSize, _
                                                     Me.BufferCountPerNotification * Me.BufferSize, _
                                                     Me.BufferCountPerNotification)

                ' Set the HasChecked flag, so the client will not check the URL again.
                client.TotalSize = Me.TotalSize
                client.DownloadPath = Me.DownloadPath
                client.HasChecked = True
                client.Credentials = Me.Credentials
                client.Proxy = Me.Proxy

                Me._downloadClients.Add(client)
            Else
                ' Calculate the block size for each client to download.
                Dim maxSizePerThread As Integer = CInt(Fix(Math.Ceiling(CDbl(Me.TotalSize) / Me.MaxThreadCount)))
                If maxSizePerThread < Me.MaxCacheSize Then
                    maxSizePerThread = Me.MaxCacheSize
                End If

                Dim leftSizeToDownload As Long = Me.TotalSize

                ' The real threads count number is the min value of MaxThreadCount and 
                ' TotalSize / MaxCacheSize.              
                Dim threadsCount As Integer = CInt(Fix(Math.Ceiling(CDbl(Me.TotalSize) / maxSizePerThread)))

                For i As Integer = 0 To threadsCount - 1
                    Dim endPoint As Long = maxSizePerThread * (i + 1) - 1
                    Dim sizeToDownload As Long = maxSizePerThread

                    If endPoint > Me.TotalSize Then
                        endPoint = Me.TotalSize - 1
                        sizeToDownload = endPoint - maxSizePerThread * i
                    End If

                    ' Download a block of the whole file.
                    Dim client As New HttpDownloadClient(Me.Url.AbsoluteUri, maxSizePerThread * i, endPoint)

                    ' Set the HasChecked flag, so the client will not check the URL again.
                    client.DownloadPath = Me.DownloadPath
                    client.HasChecked = True
                    client.TotalSize = sizeToDownload
                    client.Credentials = Me.Credentials
                    client.Proxy = Me.Proxy
                    Me._downloadClients.Add(client)
                Next i
            End If

            ' Set the lastStartTime to calculate the used time.
            _lastStartTime = Date.Now

            ' Start all HttpDownloadClients.
            For Each client In Me._downloadClients
                If Me.Proxy IsNot Nothing Then
                    client.Proxy = Me.Proxy
                End If

                ' Register the events of HttpDownloadClients.
                AddHandler client.DownloadProgressChanged, AddressOf client_DownloadProgressChanged
                AddHandler client.StatusChanged, AddressOf client_StatusChanged
                AddHandler client.DownloadCompleted, AddressOf client_DownloadCompleted


                client.BeginDownload()
            Next client
        Catch ex As Exception
            Me.Cancel()
            Me.OnDownloadCompleted(New DownloadCompletedEventArgs(Nothing, Me.DownloadedSize, Me.TotalSize, Me.TotalUsedTime, ex))
        End Try
    End Sub

    ''' <summary>
    ''' Pause the download.
    ''' </summary>
    Public Sub Pause() Implements IDownloader.Pause
        ' Only downloading downloader can be paused.
        If Me.Status <> DownloadStatus.Downloading Then
            Throw New ApplicationException("Only downloading downloader can be paused.")
        End If

        Me.Status = DownloadStatus.Pausing

        ' Pause all the HttpDownloadClients. If all of the clients are paused,
        ' the status of the downloader will be changed to Paused.
        For Each client In Me._downloadClients
            If client.Status = DownloadStatus.Downloading Then
                client.Pause()
            End If
        Next client
    End Sub


    ''' <summary>
    ''' Resume the download.
    ''' </summary>
    Public Sub [Resume]() Implements IDownloader.Resume
        ' Only paused downloader can be paused.
        If Me.Status <> DownloadStatus.Paused Then
            Throw New ApplicationException("Only paused downloader can be resumed. ")
        End If

        ' Set the lastStartTime to calculate the used time.
        _lastStartTime = Date.Now

        ' Set the downloading status.
        Me.Status = DownloadStatus.Waiting

        ' Resume all HttpDownloadClients.
        For Each client In Me._downloadClients
            If client.Status <> DownloadStatus.Completed Then
                client.Resume()
            End If
        Next client
    End Sub

    ''' <summary>
    ''' Resume the download.
    ''' </summary>
    Public Sub BeginResume() Implements IDownloader.BeginResume
        ' Only paused downloader can be paused.
        If Me.Status <> DownloadStatus.Paused Then
            Throw New ApplicationException("Only paused downloader can be resumed. ")
        End If

        ' Set the lastStartTime to calculate the used time.
        _lastStartTime = Date.Now

        ' Set the downloading status.
        Me.Status = DownloadStatus.Waiting

        ' Resume all HttpDownloadClients.
        For Each client In Me._downloadClients
            If client.Status <> DownloadStatus.Completed Then
                client.BeginResume()
            End If
        Next client

    End Sub

    ''' <summary>
    ''' Cancel the download
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

        ' Cancel all HttpDownloadClients.
        For Each client In Me._downloadClients
            client.Cancel()
        Next client

    End Sub

    ''' <summary>
    ''' Handle the StatusChanged event of all the HttpDownloadClients.
    ''' </summary>
    Private Sub client_StatusChanged(ByVal sender As Object, ByVal e As EventArgs)

        ' If all the clients are completed, then the status of this downloader is 
        ' completed.
        If Me._downloadClients.All(Function(client) client.Status = DownloadStatus.Completed) Then
            Me.Status = DownloadStatus.Completed

            ' If all the clients are Canceled, then the status of this downloader is 
            ' Canceled.
        ElseIf Me._downloadClients.All(Function(client) client.Status = DownloadStatus.Canceled) Then
            Me.Status = DownloadStatus.Canceled
        Else

            ' The completed clients will not be taken into consideration.
            Dim nonCompletedClients = _
            Me._downloadClients.Where(Function(client) client.Status <> DownloadStatus.Completed)

            ' If all the nonCompletedClients are Waiting, then the status of this 
            ' downloader is Waiting.
            If nonCompletedClients.All(Function(client) client.Status = DownloadStatus.Waiting) Then
                Me.Status = DownloadStatus.Waiting

                ' If all the nonCompletedClients are Paused, then the status of this 
                ' downloader is Paused.
            ElseIf nonCompletedClients.All(Function(client) client.Status = DownloadStatus.Paused) Then
                Me.Status = DownloadStatus.Paused
            ElseIf Me.Status <> DownloadStatus.Pausing AndAlso Me.Status <> DownloadStatus.Canceling Then
                Me.Status = DownloadStatus.Downloading
            End If
        End If

    End Sub

    ''' <summary>
    ''' Handle the DownloadProgressChanged event of all the HttpDownloadClients, and 
    ''' calculate the download speed.
    ''' </summary>
    Private Sub client_DownloadProgressChanged(ByVal sender As Object, _
                                               ByVal e As DownloadProgressChangedEventArgs)
        SyncLock _locker
            Dim speed As Integer = 0
            Dim current As Date = Date.Now
            Dim interval As TimeSpan = current.Subtract(_lastNotificationTime)

            If interval.TotalSeconds < 60 And interval.TotalSeconds > 0 Then
                speed = CInt(Fix(Math.Floor((Me.DownloadedSize + Me.CachedSize _
                                                 - Me._lastNotificationDownloadedSize) / interval.TotalSeconds)))
            End If

            _lastNotificationTime = current
            _lastNotificationDownloadedSize = Me.DownloadedSize + Me.CachedSize

            Dim downloadProgressChangedEventArgs = New DownloadProgressChangedEventArgs(DownloadedSize, TotalSize, speed)
            Me.OnDownloadProgressChanged(downloadProgressChangedEventArgs)
        End SyncLock
    End Sub

    ''' <summary>
    ''' Handle the DownloadCompleted event of all the HttpDownloadClients.
    ''' </summary>
    Private Sub client_DownloadCompleted(ByVal sender As Object, _
                                         ByVal e As DownloadCompletedEventArgs)
        If e.Error IsNot Nothing AndAlso Me.Status <> DownloadStatus.Canceling _
            AndAlso Me.Status <> DownloadStatus.Canceled Then
            Me.Cancel()
            Me.OnDownloadCompleted( _
                New DownloadCompletedEventArgs(Nothing, Me.DownloadedSize, _
                                               Me.TotalSize, Me.TotalUsedTime, e.Error))
        End If
    End Sub

    ''' <summary>
    ''' Raise DownloadProgressChanged event. If the status is Completed, then raise
    ''' DownloadCompleted event.
    ''' </summary>
    ''' <param name="e"></param>
    Protected Overridable Sub OnDownloadProgressChanged(ByVal e As DownloadProgressChangedEventArgs)
        RaiseEvent DownloadProgressChanged(Me, e)
    End Sub

    ''' <summary>
    ''' Raise StatusChanged event.
    ''' </summary>
    Protected Overridable Sub OnStatusChanged(ByVal e As EventArgs)

        Select Case Me.Status
            Case DownloadStatus.Waiting, _
                DownloadStatus.Downloading, _
                DownloadStatus.Paused, _
                DownloadStatus.Canceled, _
                DownloadStatus.Completed
                RaiseEvent StatusChanged(Me, e)
            Case Else
        End Select

        If Me.Status = DownloadStatus.Paused _
            OrElse Me.Status = DownloadStatus.Canceled _
            OrElse Me.Status = DownloadStatus.Completed Then
            Me._usedTime = Me._usedTime.Add(DateTime.Now - _lastStartTime)
        End If

        If Me.Status = DownloadStatus.Canceled Then
            Dim ex As New Exception("Downloading is canceled by user's request. ")
            Me.OnDownloadCompleted( _
                New DownloadCompletedEventArgs(Nothing, Me.DownloadedSize, _
                                               Me.TotalSize, Me.TotalUsedTime, ex))
        End If

        If Me.Status = DownloadStatus.Completed Then
            Me.OnDownloadCompleted( _
                New DownloadCompletedEventArgs(New FileInfo(Me.DownloadPath), _
                                               Me.DownloadedSize, _
                                               Me.TotalSize, _
                                               Me.TotalUsedTime, _
                                               Nothing))
        End If
    End Sub

    ''' <summary>
    ''' Raise DownloadCompleted event.
    ''' </summary>
    Protected Overridable Sub OnDownloadCompleted(ByVal e As DownloadCompletedEventArgs)
        RaiseEvent DownloadCompleted(Me, e)
    End Sub
End Class

