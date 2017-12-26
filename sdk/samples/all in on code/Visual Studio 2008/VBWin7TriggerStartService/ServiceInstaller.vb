'************************************** Module Header **************************************'
' Module Name:  ServiceInstaller.vb
' Project:      VBWin7TriggerStartService
' Copyright (c) Microsoft Corporation.
' 
' In ServiceInstaller, we configure the service to start when a generic USB disk becomes 
' available. It also shows how to trigger-start when the first IP address becomes available, 
' and trigger-stop when the last IP address becomes unavailable. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*******************************************************************************************'

#Region "Imports directives"

Imports System.ComponentModel
Imports System.Configuration.Install
Imports System.ServiceProcess
Imports System.Runtime.InteropServices

#End Region


Public Class ServiceInstaller

    Public Sub New()
        MyBase.New()
        'This call is required by the Component Designer.
        InitializeComponent()
    End Sub


    Private Sub ServiceInstaller1_AfterInstall(ByVal sender As System.Object, ByVal e As System.Configuration.Install.InstallEventArgs) Handles ServiceInstaller1.AfterInstall

        ' If Service Trigger Start is supported on the current system, configure the service 
        ' to trigger start.
        If (ServiceTriggerStart.IsSupported) Then
            Console.WriteLine("Configuring trigger-start service...")
            Try
                ' Set the service to trigger-start when a generic USB disk becomes available.
                ServiceTriggerStart.SetServiceTriggerStartOnUSBArrival( _
                ServiceInstaller1.ServiceName)

                ' [-or-]

                ' Set the service to trigger-start when the first IP address becomes 
                ' available, and trigger-stop when the last IP address becomes unavailable.
                'ServiceTriggerStart.SetServiceTriggerStartOnIPAddressArrival( _
                'ServiceInstaller1.ServiceName)

            Catch ex As Exception
                Console.WriteLine("Service Trigger Start configuration failed with " & ex.Message)
            End Try
        Else
            Console.WriteLine("The current system does not support trigger-start service.")
        End If

    End Sub

End Class