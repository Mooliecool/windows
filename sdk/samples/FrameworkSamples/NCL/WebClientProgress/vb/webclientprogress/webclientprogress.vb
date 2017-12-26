'---------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
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
'---------------------------------------------------------------------

Public Class WebClientProgressForm
    Dim isBusy As Boolean

    Private Sub webClient_DownloadFileCompleted( _
        ByVal sender As System.Object, _
        ByVal e As System.ComponentModel.AsyncCompletedEventArgs) _
        Handles webClient.DownloadFileCompleted
        isBusy = False
        downloadButton.Text = "Download"
        If e.Error Is Nothing Then
            MessageBox.Show("Download Complete")
        Else
            MessageBox.Show("Download Not Complete: " & e.Error.Message)
        End If
    End Sub

    Private Sub webClient_DownloadProgressChanged( _
        ByVal sender As System.Object, _
        ByVal e As System.Net.DownloadProgressChangedEventArgs) _
        Handles webClient.DownloadProgressChanged
        downloadProgressBar.Value = e.ProgressPercentage
    End Sub

    Private Sub downloadButton_Click( _
        ByVal sender As System.Object, _
        ByVal e As System.EventArgs) _
        Handles downloadButton.Click
        ' If an operation is pending, then the user has clicked cancel.
        If isBusy Then
            webClient.CancelAsync()
            isBusy = False
            downloadButton.Text = "Download"
            ' Otherwise go ahead and start the download
        Else
            Try
                Dim uri As New Uri(urlTextBox.Text)
                downloadProgressBar.Value = 0
                webClient.DownloadFileAsync(uri, "localfile.htm")
                downloadButton.Text = "Cancel"
                isBusy = True
            Catch ex As UriFormatException
                MessageBox.Show(ex.Message)
            End Try
        End If
    End Sub
End Class
