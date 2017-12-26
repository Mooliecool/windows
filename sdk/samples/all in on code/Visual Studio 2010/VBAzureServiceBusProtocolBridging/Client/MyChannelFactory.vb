'****************************** Module Header ******************************\
' Project Name:   CSAzureServiceBusProtocolBridging
' Module Name:    Client
' File Name:      MyChannelFactory.vb
' Copyright (c) Microsoft Corporation
'
' This is a wrapper for calling WCF service.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports Common
Imports System.ServiceModel

Public Class MyChannelFactory
    Implements IDisposable
    Private channel As IMyService = Nothing
    Private channelFactory As ChannelFactory(Of IMyService) = Nothing

    Public ReadOnly Property MyService() As IMyService
        Get
            Return channel
        End Get
    End Property
    Public ReadOnly Property EndpointAddress() As String
        Get
            Return channelFactory.Endpoint.Address.ToString()
        End Get
    End Property

    Public Sub New(endpointName As String, endpointAddress As String)
        channelFactory = New ChannelFactory(Of IMyService)(endpointName, New EndpointAddress(endpointAddress))
        channel = channelFactory.CreateChannel()
        DirectCast(channel, ICommunicationObject).Open()
    End Sub

    Public Sub Dispose() Implements IDisposable.Dispose
        If channel IsNot Nothing Then
            DirectCast(channel, ICommunicationObject).Close()
        End If
        If channelFactory IsNot Nothing Then
            channelFactory.Close()
        End If
    End Sub
End Class