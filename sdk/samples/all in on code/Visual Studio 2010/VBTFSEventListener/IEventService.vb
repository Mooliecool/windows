'*************************** Module Header ******************************'
' Module Name:  IEventService.vb
' Project:	    VBTFSEventListener
' Copyright (c) Microsoft Corporation.
' 
' Define the interface that all TFS event listener must implement. 
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System.ServiceModel

<ServiceContract(Namespace:="http://schemas.microsoft.com/TeamFoundation/2005/06/Services/Notification/03")>
Public Interface IEventService
    <OperationContract(Action:="http://schemas.microsoft.com/TeamFoundation/2005/06/Services/Notification/03/Notify")>
    <XmlSerializerFormat(Style:=OperationFormatStyle.Document)>
    Sub Notify(ByVal eventXml As String, ByVal tfsIdentityXml As String)
End Interface
