'****************************** Module Header ******************************\
' Module Name:    ClientAdapter.vb
' Project:        VBASPNETReverseAJAX
' Copyright (c) Microsoft Corporation
'
' ClientAdapter class manages multiple client instances. The presentation layer 
' calls its methods to easily send and receive messages.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports System.Collections.Generic

''' <summary>
''' This class is used to send events/messages to multiple clients.
''' </summary>
Public Class ClientAdapter
    ''' <summary>
    ''' The recipient list.
    ''' </summary>
    Private recipients As New Dictionary(Of String, Client)()

    ''' <summary>
    ''' Send a message to a particular recipient.
    ''' </summary>
    Public Sub SendMessage(ByVal message As Message)
        If recipients.ContainsKey(message.RecipientName) Then
            Dim client As Client = recipients(message.RecipientName)

            client.EnqueueMessage(message)
        End If
    End Sub

    ''' <summary>
    ''' Called by a individual recipient to wait and receive a message.
    ''' </summary>
    ''' <returns>The message content</returns>
    Public Function GetMessage(ByVal userName As String) As String
        Dim messageContent As String = String.Empty

        If recipients.ContainsKey(userName) Then
            Dim client As Client = recipients(userName)

            messageContent = client.DequeueMessage().MessageContent
        End If

        Return messageContent
    End Function

    ''' <summary>
    ''' Join a user to the recipient list.
    ''' </summary>
    Public Sub Join(ByVal userName As String)
        recipients(userName) = New Client()
    End Sub

    ''' <summary>
    ''' Singleton pattern.
    ''' This pattern will ensure there is only one instance of this class in the system.
    ''' </summary>
    Public Shared Instance As New ClientAdapter()
    Private Sub New()
    End Sub
End Class