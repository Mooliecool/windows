'***************************** Module Header ******************************\
'* Module Name:	WorkerRole.vb
'* Project:		CSWorkerRoleHostingWCF
'* Copyright (c) Microsoft Corporation.
'* 
'* This Worker Role hosts a WCF service that exposes two tcp endpoints. One is
'* for metadata. Another is for MyService.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************

Imports System.Net
Imports System.Threading
Imports Microsoft.WindowsAzure.Diagnostics
Imports Microsoft.WindowsAzure.ServiceRuntime
Imports System.ServiceModel
Imports System.ServiceModel.Description
Imports System.ServiceModel.Channels

Public Class WorkerRole
    Inherits RoleEntryPoint

    Public Overrides Sub Run()
        ' This is a sample worker implementation. Replace with your logic.
        ' Trace.WriteLine("WorkerRoleHostingWCF entry point called", "Information");

        ' Main logic

        Using host As ServiceHost = New ServiceHost(GetType(MyService))

            Dim ip As String = RoleEnvironment.CurrentRoleInstance.InstanceEndpoints("tcpinput").IPEndpoint.Address.ToString()
            Dim tcpport As Integer = RoleEnvironment.CurrentRoleInstance.InstanceEndpoints("tcpinput").IPEndpoint.Port
            Dim mexport As Integer = RoleEnvironment.CurrentRoleInstance.InstanceEndpoints("mexinput").IPEndpoint.Port

            ' Add a metadatabehavior for client proxy generation
            ' The metadata is exposed via net.tcp
            Dim metadatabehavior As ServiceMetadataBehavior = New ServiceMetadataBehavior()
            host.Description.Behaviors.Add(metadatabehavior)
            Dim mexBinding As Binding = MetadataExchangeBindings.CreateMexTcpBinding()
            Dim mexlistenurl As String = String.Format("net.tcp://{0}:{1}/MyServiceMetaDataEndpoint", ip, mexport)
            Dim mexendpointurl As String = String.Format("net.tcp://{0}:{1}/MyServiceMetaDataEndpoint", RoleEnvironment.GetConfigurationSettingValue("Domain"), 8001)
            host.AddServiceEndpoint(GetType(IMetadataExchange), mexBinding, mexendpointurl, New Uri(mexlistenurl))

            ' Add the endpoint for MyService
            Dim listenurl As String = String.Format("net.tcp://{0}:{1}/MyServiceEndpoint", ip, tcpport)
            Dim endpointurl As String = String.Format("net.tcp://{0}:{1}/MyServiceEndpoint", RoleEnvironment.GetConfigurationSettingValue("Domain"), 9001)
            host.AddServiceEndpoint(GetType(IMyService), New NetTcpBinding(SecurityMode.None), endpointurl, New Uri(listenurl))
            host.Open()

            Do
                Thread.Sleep(1800000)
                'Trace.WriteLine("Working", "Information");
            Loop
        End Using
    End Sub



    Public Overrides Function OnStart() As Boolean

        ' Set the maximum number of concurrent connections 
        ServicePointManager.DefaultConnectionLimit = 12

        ' To avoid potential errors diagnostic is disabled.
        ' DiagnosticMonitor.Start("DiagnosticsConnectionString");

        ' For information on handling configuration changes
        ' see the MSDN topic at http://go.microsoft.com/fwlink/?LinkId=166357.
        AddHandler RoleEnvironment.Changing, AddressOf RoleEnvironmentChanging

        Return MyBase.OnStart()

    End Function

    Private Sub RoleEnvironmentChanging(ByVal sender As Object, ByVal e As RoleEnvironmentChangingEventArgs)

        ' If a configuration setting is changing
        If (e.Changes.Any(Function(change) TypeOf change Is RoleEnvironmentConfigurationSettingChange)) Then
            ' Set e.Cancel to true to restart this role instance
            e.Cancel = True
        End If

    End Sub

End Class
