'****************************** Module Header ******************************\
' Module Name:	MainPage.xaml.vb
' Project:		VBAzureServiceBusSLRest
' Copyright (c) Microsoft Corporation.
' 
' This is the Silverlight client.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System.Net.Browser
Imports System.IO

Partial Public Class MainPage
	Inherits UserControl

	' Change to your namespace.
    Private Const ServiceNamespace As String = "[namespace]"

	Private request As HttpWebRequest
	Private fs As FileStream

	Public Sub New()
		InitializeComponent()
	End Sub

	Private Sub UserControl_Loaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
		' Switch to client HTTP stack to take advantage of features such as status code.
		WebRequest.RegisterPrefix("http://", WebRequestCreator.ClientHttp)
		WebRequest.RegisterPrefix("https://", WebRequestCreator.ClientHttp)
	End Sub

	Private Sub UploadButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
		Dim ofd As New OpenFileDialog
		If ofd.ShowDialog.Value Then
			Me.FileNameTextBox.Text = ofd.File.Name
			Me.fs = ofd.File.OpenRead

			' To make the sample work for large files, please configure the webHttpRelayBinding's max***
			' properties. To make the sample code easier to understand, we use the default settings.
			' WCF configurations will be demonstrated in a separate WCF sample.
			If (fs.Length >= 65535) Then
				MessageBox.Show("The file is too large to be sent using the default WCF settings.")
			Else
				Me.infoTextBlock.Text = "Uploading the file. Please wait..."
				Me.infoTextBlock.Visibility = Visibility.Visible

				' Invoke the WCF REST Service with HTTP POST.
				Me.request = DirectCast(WebRequest.Create(("https://" & ServiceNamespace & ".servicebus.windows.net/file/" & ofd.File.Name)), HttpWebRequest)
				request.Method = "POST"
				request.ContentType = "application/octet-stream"
				request.BeginGetRequestStream(AddressOf BeginGetRequestStream_Complete, Nothing)
			End If
        End If
    End Sub

	' VB 9 doesn't support anonymous lambda expressions, so we have to write a seperate callback method...
	Sub BeginGetRequestStream_Complete(ByVal result1 As IAsyncResult)
		Dim requestStream As Stream = request.EndGetRequestStream(result1)
		Dim buffer As Byte() = New Byte(fs.Length - 1) {}
		fs.Read(buffer, 0, buffer.Length)
		requestStream.Write(buffer, 0, buffer.Length)
		requestStream.Close()
		request.BeginGetResponse(AddressOf BeginGetResponse_Complete, Nothing)
	End Sub

	' VB 9 doesn't support anonymous lambda expressions, so we have to write a seperate callback method...
	Sub BeginGetResponse_Complete(ByVal result2 As IAsyncResult)
		Dim response As HttpWebResponse = DirectCast(request.EndGetResponse(result2), HttpWebResponse)
		Using response.GetResponseStream
			If (response.StatusCode = HttpStatusCode.Created) Then
				Me.Dispatcher.BeginInvoke(AddressOf Dispatcher_BeginInvoke_Complete)
			End If
		End Using
	End Sub

	' VB 9 doesn't support anonymous lambda expressions, so we have to write a seperate callback method...
	Sub Dispatcher_BeginInvoke_Complete()
		Me.infoTextBlock.Visibility = Visibility.Collapsed
		MessageBox.Show("Upload succeeded.")
	End Sub

	Private Sub DownloadButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
		If String.IsNullOrEmpty(Me.FileNameTextBox.Text) Then
			MessageBox.Show("Please type a file name.")
		Else
			Dim sfd As New SaveFileDialog
			If sfd.ShowDialog.Value Then
				Me.infoTextBlock.Text = "Downloading the file. Please wait..."
				Me.infoTextBlock.Visibility = Visibility.Visible
				Dim stream As Stream = sfd.OpenFile
				' Invoke the WCF REST Service with HTTP GET.
				Dim webClient As New WebClient
				AddHandler webClient.OpenReadCompleted, New OpenReadCompletedEventHandler(AddressOf Me.webClient_OpenReadCompleted)
				webClient.OpenReadAsync(New Uri(("https://" & ServiceNamespace & ".servicebus.windows.net/file/" & Me.FileNameTextBox.Text)), stream)
			End If
		End If
	End Sub

	Private Sub webClient_OpenReadCompleted(ByVal sender As Object, ByVal e As OpenReadCompletedEventArgs)
		Try
			Dim fileStream As Stream = DirectCast(e.UserState, Stream)
			Dim buffer As Byte() = New Byte(e.Result.Length - 1) {}
			e.Result.Read(buffer, 0, buffer.Length)
			fileStream.Write(buffer, 0, buffer.Length)
			fileStream.Close()
			Me.infoTextBlock.Visibility = Visibility.Collapsed
			MessageBox.Show("File downloaded. Please check your saved file.")
		Catch
			Me.infoTextBlock.Visibility = Visibility.Collapsed
			MessageBox.Show("File download failed. Please check if the file exits on the server.")
		End Try
	End Sub
End Class