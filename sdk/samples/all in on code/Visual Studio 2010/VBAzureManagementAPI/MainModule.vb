'****************************** Module Header ******************************\
' Module Name:  MainModule.vb
' Project:      VBAzureManagementAPI
' Copyright (c) Microsoft Corporation.
'
' This file shows how to create a hosted service on Azure via management api.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/


Imports System.Collections.Generic
Imports System.Text
Imports System.Security.Cryptography.X509Certificates
Imports System.Net
Imports System.IO

Module ManagementAPI

    Public Class RequestState
        Const BufferSize As Integer = 1024
        Public RequestData As StringBuilder
        Public BufferRead As Byte()
        Public Request As WebRequest
        Public ResponseStream As Stream

        Public StreamDecode As Decoder = Encoding.UTF8.GetDecoder()   ' Create Decoder for appropriate enconding type.

        Public Sub New()
            BufferRead = New Byte(BufferSize - 1) {}
            RequestData = New StringBuilder([String].Empty)
            Request = Nothing
            ResponseStream = Nothing
        End Sub
    End Class


    Sub Main(ByVal args As String())

        Dim subscriptionId As String = ""      ' Your subscription id


        ' This is the thumbprint of the management certificate
        ' make sure the certificate exists in your personal/Certificates. Please verify it by certmgr.msc
        ' this certificate must be uploaded to Azure Management Portal, in Management Certificates
        ' http://msdn.microsoft.com/en-us/library/gg551726.aspx
        Dim certThumbprint As String = ""

        ' This will be use as the DNS prefix, so if it exists(someone else
        ' might took it) then you will receive 409 conflict error
        Dim hostServiceName As String = ""

        Dim labelName As String = ""   'The Name of the hosted service


        Dim certificateStore As New X509Store(StoreName.My, StoreLocation.CurrentUser)
        certificateStore.Open(OpenFlags.[ReadOnly])
        Dim certs As X509Certificate2Collection = certificateStore.Certificates.Find(X509FindType.FindByThumbprint,
                                                                                     certThumbprint, False)
        If certs.Count = 0 Then
            Console.WriteLine("Couldn't find the certificate with thumbprint:" & certThumbprint)
            Return
        End If

        certificateStore.Close()

        Dim request As HttpWebRequest = DirectCast(HttpWebRequest.Create(
                New Uri("https://management.core.windows.net/" & subscriptionId & "/services/hostedservices")), HttpWebRequest)

        request.Method = "POST"
        request.ClientCertificates.Add(certs(0))
        request.ContentType = "application/xml"
        request.Headers.Add("x-ms-version", "2010-10-28")

        Dim sbRequestXml As StringBuilder = New StringBuilder("?xml version=""1.0"" encoding=""utf-8""?>")
        sbRequestXml.Append("<CreateHostedService xmlns=""http://schemas.microsoft.com/windowsazure"">")
        sbRequestXml.AppendFormat("<ServiceName>{0}</ServiceName>", hostServiceName)
        sbRequestXml.AppendFormat("<Label>{0}</Label>", EncodeToBase64String(labelName))
        sbRequestXml.Append("<Location>Anywhere US</Location>")
        sbRequestXml.Append("</CreateHostedService>")

        Dim formData As Byte() = UTF8Encoding.UTF8.GetBytes(sbRequestXml.ToString())
        request.ContentLength = formData.Length

        Using post As Stream = request.GetRequestStream()
            post.Write(formData, 0, formData.Length)
        End Using

        Console.WriteLine("Creating Hosted Service: " & hostServiceName)

        Try
            Dim state As New RequestState()
            state.Request = request
            Dim result As IAsyncResult = request.BeginGetResponse(New AsyncCallback(AddressOf RespCallback), state)
        Catch ex As Exception
            Console.WriteLine("Error: " & ex.Message)
        End Try

        Console.ReadKey()

    End Sub



    Public Function EncodeToBase64String(ByVal original As String) As String
        Return Convert.ToBase64String(Encoding.UTF8.GetBytes(original))
    End Function


    Private Sub RespCallback(ByVal result As IAsyncResult)

        Dim state As RequestState = DirectCast(result.AsyncState, RequestState)         ' Grab the custom state object
        Dim request As WebRequest = DirectCast(state.Request, WebRequest)

        Dim response As HttpWebResponse = DirectCast(request.EndGetResponse(result), HttpWebResponse) ' Get the Response

        Dim statusCode As String = response.StatusCode.ToString()

        ' A value that uniquely identifies a request made against the Management service.
        ' For an asynchronous operation, 
        Dim reqId As String = response.GetResponseHeader("x-ms-request-id")

        ' You can call get operation status with the value of the header to determine whether the operation is complete, 
        ' has failed, or is still in progress. 

        Console.WriteLine("Creation Return Value: " & statusCode)

        Console.WriteLine("RequestId: " & reqId)
    End Sub
End Module

