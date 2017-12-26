' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Data
Imports System.Configuration
Imports System.Web
Imports System.Web.Security
Imports System.Web.UI
Imports System.Web.UI.WebControls
Imports System.Web.UI.WebControls.WebParts
Imports System.Web.UI.HtmlControls
Imports Microsoft.ServiceModel.Samples
Imports System.ServiceModel
Imports System.IO

Public Class [Global]
    Inherits HttpApplication

    Protected Shared m_client As WeatherServiceClient
    Private Shared clientLock As New Object()

    ' We only want one client per instance of the app because it is inefficient 
    ' to create a new client for every request.
    Public Shared Property Client() As WeatherServiceClient

        Get

            ' lazy init the client
            SyncLock clientLock

                If m_client Is Nothing Then

                    m_client = New WeatherServiceClient()

                End If
                Return m_client

            End SyncLock

        End Get
        Set(ByVal value As WeatherServiceClient)

            SyncLock clientLock

                m_client = Value

            End SyncLock

        End Set

    End Property

    ' Our client is scoped to the lifetime of the HttpApplication so
    ' when the Application_End method is invoked, we should call Close on the client
    Public Sub Application_End(ByVal sender As Object, ByVal e As EventArgs)

        Dim localClient As WeatherServiceClient = m_client

        If localClient IsNot Nothing AndAlso localClient.State = System.ServiceModel.CommunicationState.Opened Then

            localClient.Close()

        End If

    End Sub

End Class
