'****************************** Module Header ******************************'
' Module Name:  Default.aspx.vb
' Project:      VBASPNETRemoteUploadAndDownload
' Copyright (c) Microsoft Corporation.
' 
' Create RemoteUpload instance, input the parameter of Upload file name and 
' server url address. 
' Use UploadFile method to upload file to remote server.
' 
' Create RemoteDownload instance, input the parameter of Download file name 
' and server url address.
' Use DownloadFile method to download file from remote server.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'


Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

    End Sub


    Protected Sub btnUpload_Click(ByVal sender As Object, ByVal e As EventArgs)
        Dim uploadClient As RemoteUpload = Nothing
        If Me.rbUploadList.SelectedIndex = 0 Then
            uploadClient = New HttpRemoteUpload(Me.FileUpload.FileBytes, _
                Me.FileUpload.PostedFile.FileName, Me.tbUploadUrl.Text)
        Else
            uploadClient = New FtpRemoteUpload(Me.FileUpload.FileBytes, _
                Me.FileUpload.PostedFile.FileName, Me.tbUploadUrl.Text)
        End If

        If uploadClient.UploadFile() Then
            Response.Write("Upload is complete")
        Else
            Response.Write("Failed to upload")
        End If
    End Sub


    Protected Sub btnDownLoad_Click(ByVal sender As Object, ByVal e As EventArgs)
        Dim downloadClient As RemoteDownload = Nothing
        If Me.rbDownloadList.SelectedIndex = 0 Then
            downloadClient = New HttpRemoteDownload(Me.tbDownloadUrl.Text, Me.tbDownloadPath.Text)
        Else
            downloadClient = New FtpRemoteDownload(Me.tbDownloadUrl.Text, Me.tbDownloadPath.Text)
        End If

        If downloadClient.DownloadFile() Then
            Response.Write("Download is complete")
        Else
            Response.Write("Failed to download")
        End If
    End Sub

End Class