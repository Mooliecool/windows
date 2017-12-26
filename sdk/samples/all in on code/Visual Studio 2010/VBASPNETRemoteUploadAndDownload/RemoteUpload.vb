'****************************** Module Header ******************************'
' Module Name:  RemoteUpload.vb
' Project:      VBASPNETRemoteUploadAndDownload
' Copyright (c) Microsoft Corporation.
' 
' The VBASPNETRemoteUploadAndDownload sample shows how to upload files to and 
' download files from remote server in an ASP.NET application. 
'
' This project is created by using WebClient and FtpWebRequest object in VB.NET 
' language. Both WebClient and FtpWebRequest classes provide common methods for 
' sending data to URI of server and receiving data from a resource identified 
' by URI as well. When uploading or downloading files, these classes will do 
' webrequest to the URL which user types in.
' 
' The UploadData() method sends a data buffer(without encoding it) to a 
' resource using HTTP or FTP method specified in the method parameter, and then 
' returns the web response from the server. The DownloadData() method posts an 
' HTTP or FTP download request to the remote server and get outputstream from 
' the server.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'***************************************************************************'

Imports System.Net
Imports System.IO


Public MustInherit Class RemoteUpload

    Public Property FileName() As String
        Get
            Return m_FileName
        End Get
        Set(ByVal value As String)
            m_FileName = value
        End Set
    End Property
    Private m_FileName As String


    Public Property UrlString() As String
        Get
            Return m_UrlString
        End Get
        Set(ByVal value As String)
            m_UrlString = value
        End Set
    End Property
    Private m_UrlString As String


    Public Property NewFileName() As String
        Get
            Return m_NewFileName
        End Get
        Set(ByVal value As String)
            m_NewFileName = value
        End Set
    End Property
    Private m_NewFileName As String


    Public Property FileData() As Byte()
        Get
            Return m_FileData
        End Get
        Set(ByVal value As Byte())
            m_FileData = value
        End Set
    End Property
    Private m_FileData As Byte()


    Public Sub New(ByVal fileData As Byte(), ByVal fileName As String, ByVal urlString As String)
        Me.FileData = fileData
        Me.FileName = fileName
        Me.UrlString = If(urlString.EndsWith("/"), urlString, urlString & "/")
        Dim newFileName As String = DateTime.Now.ToString("yyMMddhhmmss") & _
            DateTime.Now.Millisecond.ToString() + Path.GetExtension(Me.FileName)
        Me.UrlString = Me.UrlString & newFileName
    End Sub


    ''' <summary>
    ''' Upload file to remote server
    ''' </summary>
    ''' <returns></returns>
    Public Overridable Function UploadFile() As Boolean
        Return True
    End Function

End Class

''' <summary>
''' HttpUpload class
''' </summary>
Public Class HttpRemoteUpload
    Inherits RemoteUpload

    Public Sub New(ByVal fileData As Byte(), ByVal fileNamePath As String, ByVal urlString As String)
        MyBase.New(fileData, fileNamePath, urlString)
    End Sub

    Public Overrides Function UploadFile() As Boolean
        Dim postData As Byte()
        Try
            postData = Me.FileData
            Using client As New WebClient()
                client.Credentials = CredentialCache.DefaultCredentials
                client.Headers.Add("Content-Type", "application/x-www-form-urlencoded")
                client.UploadData(Me.UrlString, "PUT", postData)
            End Using

            Return True
        Catch ex As Exception
            Throw New Exception("Failed to upload", ex.InnerException)
        End Try

    End Function
End Class


''' <summary>
''' FtpUpload class
''' </summary>
Public Class FtpRemoteUpload
    Inherits RemoteUpload

    Public Sub New(ByVal fileData As Byte(), ByVal fileNamePath As String, ByVal urlString As String)
        MyBase.New(fileData, fileNamePath, urlString)
    End Sub

    Public Overrides Function UploadFile() As Boolean
        Dim reqFTP As FtpWebRequest
        reqFTP = DirectCast(FtpWebRequest.Create(Me.UrlString), FtpWebRequest)
        reqFTP.KeepAlive = True
        reqFTP.Method = WebRequestMethods.Ftp.UploadFile
        reqFTP.UseBinary = True
        reqFTP.ContentLength = Me.FileData.Length

        Dim buffLength As Integer = 2048
        Dim buff As Byte() = New Byte(buffLength - 1) {}
        Dim ms As New MemoryStream(Me.FileData)

        Try
            Dim contenctLength As Integer
            Using strm As Stream = reqFTP.GetRequestStream()
                contenctLength = ms.Read(buff, 0, buffLength)

                While contenctLength > 0
                    strm.Write(buff, 0, contenctLength)
                    contenctLength = ms.Read(buff, 0, buffLength)
                End While

            End Using

            Return True
        Catch ex As Exception
            Throw New Exception("Failed to upload", ex.InnerException)
        End Try
    End Function

End Class
