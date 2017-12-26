'***************************** Module Header ******************************\
'* Module Name:    UploadFile.vb
'* Project:        VBASPNETFileUploadStatus
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to display the upload status and progress without
'* a third part component like ActiveX control, Flash or Silverlight.
'* 
'* This is a class for the uploading file in the request entity. 
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\****************************************************************************


Imports System
Imports System.IO
Imports System.Web


Public Class UploadFile
    Private _cachePath As String = Nothing
    Private _cacheLength As Integer = 1024 * 1024 * 5

    Public Property FileName() As String
        Get
            Return m_FileName
        End Get
        Private Set(ByVal value As String)
            m_FileName = value
        End Set
    End Property
    Private m_FileName As String
    Public Property ContentType() As String
        Get
            Return m_ContentType
        End Get
        Private Set(ByVal value As String)
            m_ContentType = value
        End Set
    End Property
    Private m_ContentType As String
    Public Property ClientPath() As String
        Get
            Return m_ClientPath
        End Get
        Private Set(ByVal value As String)
            m_ClientPath = value
        End Set
    End Property
    Private m_ClientPath As String
    Private Property Data() As Byte()
        Get
            Return m_Data
        End Get
        Set(ByVal value As Byte())
            m_Data = value
        End Set
    End Property
    Private m_Data As Byte()

    Private ReadOnly _defaultUploadFolder As String = "UploadedFiles"

    Public Sub New(ByVal clientPath__1 As String,
                   ByVal contentType__2 As String)
        Data = New Byte(-1) {}
        ClientPath = clientPath__1
        ContentType = contentType__2
        _cachePath = HttpContext.Current.Server.MapPath(
            "uploadcaching") + "\" + Guid.NewGuid().ToString()
        FileName = New FileInfo(clientPath__1).Name
        Dim cache_file As New FileInfo(_cachePath)
        If Not cache_file.Directory.Exists Then
            cache_file.Directory.Create()
        End If
    End Sub

    ' For large files we need to read and store the data partially.
    ' And this method is used to combine the partial data blocks.
    Friend Sub AppendData(ByVal data As Byte())
        Me.Data = BinaryHelper.Combine(Me.Data, data)
        If Me.Data.Length > _cacheLength Then
            CacheData()
        End If
    End Sub

    ' We could store the data which has already read into 
    ' disk to release the data in memory.
    Private Sub CacheData()
        If Me.Data IsNot Nothing AndAlso Me.Data.Length > 0 Then

            Using fs As New FileStream(_cachePath,
                                       FileMode.Append,
                                       FileAccess.Write)
                fs.Write(Data, 0, Data.Length)
                Me.Data = New Byte(-1) {}
            End Using
        End If
    End Sub

    ' Clear the template file
    Friend Sub ClearCache()
        If File.Exists(_cachePath) Then
            File.Delete(_cachePath)
        End If
    End Sub


    ' Save the uploaded file to the correct path.
    Public Sub Save(ByVal path As String)
        If Me.Data.Length > 0 Then
            CacheData()
        End If

        If [String].IsNullOrEmpty(path) Then
            path = HttpContext.Current.Server.MapPath(
                _defaultUploadFolder + "\" + FileName)
        End If
        If File.Exists(path) Then
            File.Delete(path)
        End If

        Dim dir = New FileInfo(path).Directory
        If Not dir.Exists Then
            dir.Create()
        End If

        ' Move the cached file to the correct path.
        File.Move(_cachePath, path)
    End Sub

End Class


