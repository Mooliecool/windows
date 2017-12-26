'****************************** Module Header ******************************\
' Module Name:	IRestService.vb
' Project:		VBAzureServiceBusSLRest
' Copyright (c) Microsoft Corporation.
' 
' This is the service contract of the WCF REST Service.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

Imports System.ServiceModel.Web
Imports System.ServiceModel
Imports System.IO

<ServiceContract()> _
Public Interface IRestService
	''' <summary>
	''' Usage: https://namespace.servicebus.windows.net/file/filename
	''' </summary>
	<WebGet(UriTemplate:="/file/{fileName}"), OperationContract()> _
	  Function DownloadFile(ByVal fileName As String) As Stream

	''' <summary>
	''' Usage: https://namespace.servicebus.windows.net/clientaccesspolicy.xml
	''' </summary>
	<WebGet(UriTemplate:="/clientaccesspolicy.xml"), OperationContract()> _
 Function GetClientAccessPolicy() As Stream

	''' <summary>
	''' Usage: https://namespace.servicebus.windows.net/file/filename
	''' Note this is a POST operation, which cannot be invoked from a browser.
	''' </summary>
	<WebInvoke(UriTemplate:="/file/{fileName}", Method:="POST", BodyStyle:=WebMessageBodyStyle.Bare), OperationContract()> _
 Function UploadFile(ByVal fileName As String, ByVal content As Stream) As String

End Interface
