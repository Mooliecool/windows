'****************************** Module Header ******************************\
' Module Name:	RestService.vb
' Project:		VBAzureServiceBusSLRest
' Copyright (c) Microsoft Corporation.
' 
' This is the service implementation of the WCF REST Service.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System.IO
Imports System.ServiceModel.Web
Imports System.Net

Public Class RestService
	Implements IRestService
	Public Function DownloadFile(ByVal fileName As String) As Stream _
	 Implements IRestService.DownloadFile
		Try
			Using fs As FileStream = File.Open(fileName, FileMode.Open, FileAccess.Read)
				Dim stream As New MemoryStream
				Dim buffer As Byte() = New Byte(fs.Length - 1) {}
				fs.Read(buffer, 0, buffer.Length)
				stream.Write(buffer, 0, buffer.Length)
				stream.Position = 0
				Return stream
			End Using
		Catch ex As IOException
			WebOperationContext.Current.OutgoingResponse.StatusCode = HttpStatusCode.NotFound
			Return New MemoryStream
		End Try
	End Function

	Public Function GetClientAccessPolicy() As Stream _
	 Implements IRestService.GetClientAccessPolicy
		Return Me.DownloadFile("clientaccesspolicy.xml")
	End Function

	Public Function UploadFile(ByVal fileName As String, ByVal content As Stream) As String _
	 Implements IRestService.UploadFile
		' We do not know the length of the HTTP request stream,
		' so we must read the stream using the following code:
		Dim bufferSize As Integer = 4096
		Dim bytesRead As Integer = 1
		Dim totalBytesRead As Integer = 0
		Try
			Using fileStream As FileStream = File.Create(fileName)
				Dim buffer As Byte() = New Byte(bufferSize - 1) {}
				bytesRead = content.Read(buffer, 0, bufferSize)
				Do While (bytesRead > 0)
					fileStream.Write(buffer, 0, bytesRead)
					bytesRead = content.Read(buffer, 0, bufferSize)
					totalBytesRead = (totalBytesRead + bytesRead)
				Loop
			End Using

			' To follow the best practices of REST services, when a resource is created,
			' we return a 201 Created status code instead of the default 200.
			' This means Silverlight clients must use client HTTP stack,
			' instead of the default browser HTTP stack to work with the service.
			Return RestService.WriteResponse(HttpStatusCode.Created, "Created.")
		Catch ex As Exception
			Return RestService.WriteResponse(HttpStatusCode.InternalServerError, ex.Message)
		End Try
	End Function

	Private Shared Function WriteResponse(ByVal statusCode As HttpStatusCode, ByVal message As String) As String
		WebOperationContext.Current.OutgoingResponse.StatusCode = statusCode
		Return message
	End Function

End Class
