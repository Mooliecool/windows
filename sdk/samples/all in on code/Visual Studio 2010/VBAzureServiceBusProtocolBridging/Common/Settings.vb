'****************************** Module Header ******************************\
' Project Name:   CSAzureServiceBusProtocolBridging
' Module Name:    Common
' File Name:      Settings.vb
' Copyright (c) Microsoft Corporation
'
' This class contains all settings related to a Service Bus namespace. You need
' input these settings before running this sample.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Public Class Settings
    ''' <summary>
    ''' Please input your Service Bus namespace.
    ''' </summary>
    Public Shared ServiceNamespace As String = ""

    ''' <summary>
    ''' Please input your Service Bus Isser.
    ''' </summary>
    Public Shared IssuerName As String = ""

    ''' <summary>
    ''' Please input your Service Bus Secret.
    ''' </summary>
    Public Shared IssuerSecret As String = ""

    ''' <summary>
    ''' https://{0}.servicebus.windows.net/MyService
    ''' Please input your Service Bus namespace.
    ''' </summary>
    Public Shared ReadOnly Property ServiceEndpoint() As String
        Get
            Return [String].Format("https://{0}.servicebus.windows.net/MyService", ServiceNamespace)
        End Get
    End Property
End Class