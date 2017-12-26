'*************************** Module Header ******************************'
' Module Name:  WCFService.vb
' Project:	    VBAzureWCFServices
' Copyright (c) Microsoft Corporation.
' 
' This class implements WCFService.IContract interface.   
' Methods directly returns information about the current work role.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Option Explicit On
Imports System
Imports System.Collections.Generic
Imports System.Runtime.Serialization
Imports System.ServiceModel
Imports System.Text
Imports WCFContract
Imports Microsoft.WindowsAzure
Imports Microsoft.WindowsAzure.Diagnostics
Imports Microsoft.WindowsAzure.ServiceRuntime

' NOTE: You can use the "Rename" command on the context menu to change the class name "Service1" in code, 
' svc and config file together.
Public Class WCFService
    Implements WCFContract.IContract

    ' Return the current web role's name and instance id
    Public Function GetRoleInfo() As String Implements WCFContract.IContract.GetRoleInfo
        Dim currentRoleInstance As RoleInstance = RoleEnvironment.CurrentRoleInstance
        Dim RoleName As String = currentRoleInstance.Role.Name
        Dim RoleInstanceID As String = currentRoleInstance.Id
        GetRoleInfo = (String.Format("You are talking to role {0}, instance ID {1}.", RoleName, RoleInstanceID))
    End Function


    ' Return the channel between the client & the server
    Public Function GetCommunicationChannel() As String Implements WCFContract.IContract.GetCommunicationChannel
        GetCommunicationChannel = String.Format("We are talking via {0}.", OperationContext.Current.Channel.LocalAddress.Uri.ToString())
    End Function

End Class
