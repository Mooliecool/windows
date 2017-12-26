'****************************** Module Header ******************************'
' Module Name:                 MainPage.xaml.vb
' Project:                     VBSL3IsolatedStorage
' Copyright (c) Microsoft Corporation.
' 
' Isolated Storage samples code behind file.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.Windows.Data
Imports System.Windows.Media.Imaging
Imports System.Collections.ObjectModel
Imports System.IO
Imports System.ComponentModel
Imports System.IO.IsolatedStorage

Partial Public Class MainPage
    Inherits UserControl
    Public Sub New()
        InitializeComponent()
        AddHandler Loaded, AddressOf MainPage_Loaded
    End Sub

    ' Initialize application
    Private _isoroot As IsoDirectory
    Private _isofile As IsolatedStorageFile
    Private Sub MainPage_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        ' Load isolated storage file
        _isofile = IsolatedStorageFile.GetUserStoreForApplication()

        ' Display isolated storage file info
        RefreshAvailableSize()

        ' Load isolated storage treeview ViewModel 
        _isoroot = LoadIsoRoot()

        ' Display treeview
        Dim roots = New ObservableCollection(Of IsoFile)()
        roots.Add(_isoroot)
        tvIsolatedStorage.ItemsSource = roots

        ' Retrieve last login date from IsolatedStorageSettings
        If IsolatedStorageSettings.ApplicationSettings.Contains("lastLogin") Then
            Dim [date] = IsolatedStorageSettings.ApplicationSettings("lastLogin").ToString()
            tbAppInfo.Text = "Last time this application ran was: " & [date]
        Else
            tbAppInfo.Text = "Last time this application ran is not known."
        End If
        ' Save login time in IsolatedStorageSettings
        IsolatedStorageSettings.ApplicationSettings("lastLogin") = DateTime.Now

        ' Update operation panel
        UpdateOperationPanel()
    End Sub

#Region "IsolatedStorage Treeview ViewModel method"

    ' Helper method: get parent directory 
    Private Function GetParentDir(ByVal root As IsoDirectory, ByVal child As IsoFile) As IsoDirectory
        If String.IsNullOrEmpty(child.FilePath) Then
            Return Nothing
        Else
            Dim dirs As String() = child.FilePath.Split("/"c)
            Dim cur As IsoDirectory = root
            For i As Integer = 1 To dirs.Length - 2
                Dim [next] As IsoDirectory = TryCast(cur.Children.FirstOrDefault(Function(dir) dir.FileName = dirs(i)), IsoDirectory)
                If [next] IsNot Nothing Then
                    cur = [next]
                Else
                    Return Nothing
                End If
            Next
            Return cur
        End If
    End Function

    ' Load isolated storage view model
    Private Function LoadIsoRoot() As IsoDirectory
        Dim root = New IsoDirectory("Root", Nothing)
        AddFileToDirectory(root, _isofile)
        Return root
    End Function

    ' Add dir/file recursively 
    Private Sub AddFileToDirectory(ByVal dir As IsoDirectory, ByVal isf As IsolatedStorageFile)
        Dim childrendir As String(), childrenfile As String()
        If String.IsNullOrEmpty(dir.FilePath) Then
            childrendir = isf.GetDirectoryNames()
            childrenfile = isf.GetFileNames()
        Else
            childrendir = isf.GetDirectoryNames(dir.FilePath & "/")
            childrenfile = isf.GetFileNames(dir.FilePath & "/")
        End If

        ' Add directory entity
        For Each dirname In childrendir
            Dim childdir = New IsoDirectory(dirname, (dir.FilePath & "/") + dirname)
            AddFileToDirectory(childdir, isf)
            dir.Children.Add(childdir)
        Next

        ' Add file entity
        For Each filename In childrenfile
            dir.Children.Add(New IsoFile(filename, (dir.FilePath & "/") + filename))
        Next
    End Sub
#End Region

#Region "Copy stream on worker thread"

    ' Create workerthread to copy stream
    Private Sub CopyStream(ByVal from As Stream, ByVal [to] As Stream)

        Dim bworker As New BackgroundWorker()
        bworker.WorkerReportsProgress = True
        AddHandler bworker.DoWork, AddressOf bworker_DoWork
        AddHandler bworker.ProgressChanged, AddressOf bworker_ProgressChanged
        AddHandler bworker.RunWorkerCompleted, AddressOf bworker_RunWorkerCompleted

        bworker.RunWorkerAsync(New Stream() {from, [to]})

        ' Show "Copying" panel
        gdDisable.Visibility = Visibility.Visible
        spCopyPanel.Visibility = Visibility.Visible
        gdPlayerPanel.Visibility = Visibility.Collapsed
    End Sub

    ' Handle work complete event
    Private Sub bworker_RunWorkerCompleted(ByVal sender As Object, ByVal e As RunWorkerCompletedEventArgs)
        ' Close "copying" panel
        gdDisable.Visibility = Visibility.Collapsed

        If e.[Error] IsNot Nothing Then
            MessageBox.Show(e.[Error].Message)
        End If
    End Sub

    ' Display progress
    Private Sub bworker_ProgressChanged(ByVal sender As Object, ByVal e As ProgressChangedEventArgs)
        pbCopyProgress.Value = e.ProgressPercentage
    End Sub

    ' Copy stream in worker thread
    Private Sub bworker_DoWork(ByVal sender As Object, ByVal e As DoWorkEventArgs)
        Dim param = TryCast(e.Argument, Stream())

        Dim buffer As Byte() = New Byte(65535) {}
        Dim pos As Integer = 0
        Dim progress As Integer = -1
        While True
            Dim icount As Integer = param(0).Read(buffer, pos, buffer.Length)
            param(1).Write(buffer, 0, icount)
            If icount < buffer.Length Then
                Exit While
            End If

            Dim curprogress As Integer = CInt((param(1).Length * 100 / param(0).Length))
            If curprogress > progress Then
                progress = curprogress
                DirectCast(sender, BackgroundWorker).ReportProgress(progress)
            End If
        End While

        ' Close thread on UI thread
        Dispatcher.BeginInvoke(New MyDelegate(AddressOf CloseStream), param(0), param(1))
    End Sub

    Delegate Sub MyDelegate(ByVal stream1 As Stream, ByVal stream2 As Stream)
    Private Sub CloseStream(ByVal stream1 As Stream, ByVal stream2 As Stream)
        stream1.Close()
        stream2.Close()
        RefreshAvailableSize()
    End Sub

#End Region

#Region "treeview & operation button eventhandler"

    Private Sub RefreshAvailableSize()
        tbQuotaAvailable.Text = String.Format("Current storage Quota is: {0}KB, {1}KB available. The Quota can be increased by user initialized action, such as mouse click eventhandler", _isofile.Quota / 1024, _isofile.AvailableFreeSpace / 1024)
    End Sub

    ' Update operation panel
    Private Sub UpdateOperationPanel()
        Dim item = tvIsolatedStorage.SelectedItem
        If item Is Nothing Then
            spOperationPanel.Visibility = Visibility.Collapsed
        Else
            spOperationPanel.Visibility = Visibility.Visible
            If TypeOf item Is IsoDirectory Then
                bnAddDir.Visibility = Visibility.Visible
                bnAddFile.Visibility = Visibility.Visible
                bnDelete.Visibility = Visibility.Visible
                bnSave.Visibility = Visibility.Collapsed
                bnPlay.Visibility = Visibility.Collapsed
            ElseIf TypeOf item Is IsoFile Then
                bnAddDir.Visibility = Visibility.Collapsed
                bnAddFile.Visibility = Visibility.Collapsed
                bnDelete.Visibility = Visibility.Visible
                bnSave.Visibility = Visibility.Visible
                bnPlay.Visibility = Visibility.Visible
            End If
        End If
    End Sub

    ' Increase quota
    Private Sub bnIncreaseQuota_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Try
            If Not _isofile.IncreaseQuotaTo(_isofile.Quota + 1024 * 1024 * 10) Then
                MessageBox.Show("Increase quota failed.")
            End If
        Catch ex As ArgumentException
            MessageBox.Show(ex.Message)
        End Try

        RefreshAvailableSize()
    End Sub

    ' Add directory.
    Private Sub bnAddDir_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim item = TryCast(tvIsolatedStorage.SelectedItem, IsoDirectory)
        If item IsNot Nothing Then
            Dim newfoldename As String = "Folder_" & Guid.NewGuid().ToString()
            Dim newfolderpath As String = (item.FilePath & "/") + newfoldename

            Try
                ' Check if directory already exist
                If _isofile.DirectoryExists(newfolderpath) Then
                    MessageBox.Show("Folder exist:" & newfolderpath)
                Else
                    _isofile.CreateDirectory(newfolderpath)
                    item.Children.Add(New IsoDirectory(newfoldename, newfolderpath))
                End If
            Catch ex As PathTooLongException
                MessageBox.Show("Due to path length restriction, the directory depth is confined to less than 4.")
            Catch ex As Exception
                MessageBox.Show("Add Folder Failed." & vbLf & "Details: " & ex.Message)
            End Try
        End If
    End Sub

    ' Add File
    Private Sub bnAddFile_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim overrideflag As Boolean = False
        Dim selecteddir = TryCast(tvIsolatedStorage.SelectedItem, IsoDirectory)
        If selecteddir Is Nothing Then
            Exit Sub
        End If

        Dim ofd As New OpenFileDialog()
        Dim result = ofd.ShowDialog()
        If result.HasValue AndAlso result.Value Then
            Dim filename As String = ofd.File.Name
            Dim filepath As String = (selecteddir.FilePath & "/") + filename
            Dim file As New IsoFile(filename, filepath)

            Try
                ' Check if file name is same with directory name
                If _isofile.GetDirectoryNames(filepath).Length > 0 Then
                    MessageBox.Show(String.Format("File name {0} not allowed", filename))
                    Exit Sub
                    ' Check if file name already exist
                ElseIf _isofile.GetFileNames(filepath).Length > 0 Then
                    ' Show message box, ask user if override file
                    Dim mbresult = MessageBox.Show(String.Format("Override the current file: {0} ?", filename), "override warning", MessageBoxButton.OKCancel)
                    If mbresult <> MessageBoxResult.OK Then
                        Exit Sub
                    Else
                        overrideflag = True
                    End If
                End If
            Catch ex As PathTooLongException
                MessageBox.Show("Add file failed. The file path is too long.")
                Exit Sub
            Catch ex As Exception
                MessageBox.Show(ex.Message)
                Exit Sub
            End Try

            ' Check if has enough space
            If _isofile.AvailableFreeSpace < ofd.File.Length Then
                MessageBox.Show("Not enough isolated storage space.")
                Exit Sub
            End If

            Dim isostream As Stream = Nothing
            Dim filestream As Stream = Nothing
            Try
                ' Create isolatedstorage stream
                isostream = _isofile.CreateFile(filepath)
                ' Open file stream
                filestream = ofd.File.OpenRead()
                ' Copy
                ' Note: exception in copy progress won't be catched here.
                CopyStream(filestream, isostream)

                ' Check override
                If overrideflag = False Then
                    selecteddir.Children.Add(file)
                End If
            Catch ex As Exception
                If isostream IsNot Nothing Then
                    isostream.Close()
                End If
                If filestream IsNot Nothing Then
                    filestream.Close()
                End If
                MessageBox.Show(ex.Message)
            End Try
        End If
    End Sub

    ' Delete 
    Private Sub bnDelete_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim item = TryCast(tvIsolatedStorage.SelectedItem, IsoFile)
        If item IsNot Nothing Then
            ' Root
            If String.IsNullOrEmpty(item.FilePath) Then
                MessageBox.Show("Cannot delete root")
                Exit Sub
            End If

            Try
                If TypeOf item Is IsoDirectory Then
                    _isofile.DeleteDirectory(item.FilePath)
                Else
                    _isofile.DeleteFile(item.FilePath)
                End If
                Dim isodirparent = GetParentDir(_isoroot, item)
                If isodirparent IsNot Nothing Then
                    isodirparent.Children.Remove(item)
                End If
            Catch ex As Exception
                MessageBox.Show(ex.Message)
            End Try
        End If

        RefreshAvailableSize()
    End Sub

    ' Save to local
    Private Sub bnSave_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim item = TryCast(tvIsolatedStorage.SelectedItem, IsoFile)
        If item IsNot Nothing Then
            Try
                Dim sfd1 As New SaveFileDialog()

                ' Set file filter
                Dim substr = item.FileName.Split("."c)
                If substr.Length >= 2 Then
                    Dim defaultstr As String = "*." & substr(substr.Length - 1)
                    sfd1.Filter = String.Format("({0})|{1}|(*.*)|*.*", defaultstr, defaultstr)
                Else
                    sfd1.Filter = "(*.*)|*.*"
                End If

                ' Show save fiel dialog
                Dim result = sfd1.ShowDialog()

                If result.HasValue AndAlso result.Value Then
                    ' Open isolatedstorage stream
                    Dim filestream = sfd1.OpenFile()
                    ' Create file stream
                    Dim isostream = _isofile.OpenFile(item.FilePath, FileMode.Open, FileAccess.Read)
                    ' Copy
                    CopyStream(isostream, filestream)
                End If
            Catch ex As Exception
                MessageBox.Show(ex.Message)
            End Try
        End If
    End Sub

    ' Close player panel
    Private Sub bnClosePlayer_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        gdDisable.Visibility = Visibility.Collapsed
        mePlayer.[Stop]()
        mePlayer.Source = Nothing

        If currentplaystream IsNot Nothing Then
            currentplaystream.Close()

        End If
    End Sub

    ' Play
    Private currentplaystream As Stream = Nothing
    Private Sub bnPlay_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Dim item = TryCast(tvIsolatedStorage.SelectedItem, IsoFile)
        If item IsNot Nothing Then
            Try
                Dim stream = _isofile.OpenFile(item.FilePath, FileMode.Open, FileAccess.Read)

                ' Show "player" panel
                gdDisable.Visibility = Visibility.Visible
                spCopyPanel.Visibility = Visibility.Collapsed
                gdPlayerPanel.Visibility = Visibility.Visible

                mePlayer.SetSource(stream)
                currentplaystream = stream
            Catch ex As Exception
                MessageBox.Show(ex.Message)
            End Try
        End If
    End Sub

    ' When treeview selected item changed, refresh operation panel.
    Private Sub TreeView_SelectedItemChanged(ByVal sender As Object, ByVal e As RoutedPropertyChangedEventArgs(Of Object))
        UpdateOperationPanel()
    End Sub
#End Region

End Class


' Isolatedstoarge file obj
Public Class IsoFile
    Private _FilePath As String
    Public Property FilePath() As String
        Get
            Return _FilePath
        End Get
        Set(ByVal value As String)
            _FilePath = value
        End Set
    End Property
    Private _FileName As String
    Public Property FileName() As String
        Get
            Return _FileName
        End Get
        Set(ByVal value As String)
            _FileName = value
        End Set
    End Property

    Private _ContentStream As Stream
    Public Property ContentStream() As Stream
        Get
            Return _ContentStream
        End Get
        Private Set(ByVal value As Stream)
            _ContentStream = value
        End Set
    End Property
    Public Sub New(ByVal strFilename As String, ByVal strPath As String)
        FileName = strFilename
        FilePath = strPath
    End Sub
End Class

' Isolatedstorage directory obj
Public Class IsoDirectory
    Inherits IsoFile
    Private _Children As ObservableCollection(Of IsoFile)
    Public Property Children() As ObservableCollection(Of IsoFile)
        Get
            Return _Children
        End Get
        Private Set(ByVal value As ObservableCollection(Of IsoFile))
            _Children = value
        End Set
    End Property
    Public Sub New(ByVal strFilename As String, ByVal strPath As String)
        MyBase.New(strFilename, strPath)
        Children = New ObservableCollection(Of IsoFile)()
    End Sub
End Class

Public Class ImageConverter
    Implements IValueConverter

    Public Function Convert(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.Convert
        If TypeOf value Is IsoDirectory Then
            Return New BitmapImage(New Uri("/Images/dir.png", UriKind.Relative))
        End If
        Return New BitmapImage(New Uri("/Images/File.png", UriKind.Relative))
    End Function

    Public Function ConvertBack(ByVal value As Object, ByVal targetType As System.Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements System.Windows.Data.IValueConverter.ConvertBack
        Throw New NotImplementedException
    End Function
End Class
