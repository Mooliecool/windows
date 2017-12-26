'*************************** Module Header ******************************'
' Module Name:  Serivice2.svc.vb
' Project:	    VBAzureWCFServices
' Copyright (c) Microsoft Corporation.
' 
' This class implements WCFService.IContract interface.   Methods talks to all 
' instances of workrole1 to return the instance name and communication channel. 
' These data are returned to the client together with the instance name/communication 
' channel of the current web role instance.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************'

Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Runtime.Serialization
Imports System.ServiceModel
Imports System.Text
Imports WCFContract
Imports Microsoft.WindowsAzure
Imports Microsoft.WindowsAzure.Diagnostics
Imports Microsoft.WindowsAzure.ServiceRuntime
' NOTE: You can use the "Rename" command on the context menu to change the class name "Service2" in code, svc and config file together.
Public Class Service2
    Implements IContract


    ' Return the current web role's name and instance id
    Public Function GetRoleInfo() As String Implements IContract.GetRoleInfo
        Dim currentRoleInstance As RoleInstance = RoleEnvironment.CurrentRoleInstance
        Dim roleName As String = currentRoleInstance.Role.Name
        Dim roleInstanceID As String = currentRoleInstance.Id
        GetRoleInfo = (String.Format("You are talking to role {0}, instance ID {1}.", roleName, roleInstanceID))


        Dim thisWR As String = String.Format("You are talking to the workroles via role {0}, instance ID {1}.", roleName, roleInstanceID)

        ' Contact the workrole and get its info
        Dim workRoleInfo As String = String.Empty
        Dim sb As New StringBuilder()

        Dim roles = RoleEnvironment.Roles("WorkerRole1")
        For Each instance In roles.Instances

            Dim WorkRoleInternalEndPoint As RoleInstanceEndpoint = instance.InstanceEndpoints("Internal")
            Dim binding As New NetTcpBinding(SecurityMode.None, False)
            Dim myEndpoint As New EndpointAddress(String.Format("net.tcp://{0}/Internal", WorkRoleInternalEndPoint.IPEndpoint))

            Dim myChanFac As New ChannelFactory(Of IContract)(binding, myEndpoint)
            Dim myClient As IContract = myChanFac.CreateChannel()
            sb.Append(myClient.GetRoleInfo() + vbCrLf)

        Next
        workRoleInfo = sb.ToString()

        GetRoleInfo = (thisWR + vbCrLf + workRoleInfo)
    End Function


    ' Return the channel between the client & the server
    Public Function GetCommunicationChannel() As String Implements IContract.GetCommunicationChannel
        Dim thisWebRoleChannel As String = String.Format("You are talking to the workroles via {0}.", OperationContext.Current.Channel.LocalAddress.Uri.ToString())

        ' Contact the workrole and get the channel info
        Dim workRoleChannel As String = String.Empty
        Dim sb As New StringBuilder()

        Dim roles = RoleEnvironment.Roles("WorkerRole1")
        For Each instance In roles.Instances

            Dim WorkRoleInternalEndPoint As RoleInstanceEndpoint = instance.InstanceEndpoints("Internal")
            Dim binding As New NetTcpBinding(SecurityMode.None, False)
            Dim myEndpoint As New EndpointAddress(String.Format("net.tcp://{0}/Internal", WorkRoleInternalEndPoint.IPEndpoint))

            Dim myChanFac As New ChannelFactory(Of IContract)(binding, myEndpoint)
            Dim myClient As IContract = myChanFac.CreateChannel()
            sb.Append(myClient.GetCommunicationChannel() + vbCrLf)

        Next
        workRoleChannel = sb.ToString()

        GetCommunicationChannel = (thisWebRoleChannel + vbCrLf + workRoleChannel)

    End Function

End Class
