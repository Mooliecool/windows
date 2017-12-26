'*************************** Module Header ******************************'
' Module Name:  IContract.vb
' Project:	    VBAzureWCFServices
' Copyright (c) Microsoft Corporation.
' 
' Service Contract.
'  
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System
Imports System.ServiceModel

<ServiceContract()>
Public Interface IContract
    ' This operating returns the server information, including the role's name and instance id
    <OperationContract()>
    Function GetRoleInfo() As String

    ' This operation returns the information of the channel being used between the client & the server.
    <OperationContract()>
    Function GetCommunicationChannel() As String
End Interface


