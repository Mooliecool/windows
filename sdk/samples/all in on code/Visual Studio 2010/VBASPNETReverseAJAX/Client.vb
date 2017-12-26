'****************************** Module Header ******************************\
' Module Name:    Client.vb
' Project:        VBASPNETReverseAJAX
' Copyright (c) Microsoft Corporation
'
' Client class is used to synchronize the message sending and the message receiving.
' When DequeueMessage method is called, the method will wait until a new message 
' is inserted by calling EnqueueMessage method. This class benefits ManualResetEvent
' class to achieve the synchronism. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports System.Collections.Generic
Imports System.Threading

''' <summary>
''' This class represents a web client which can receive messages.
''' </summary>
Public Class Client
    Private messageEvent As New ManualResetEvent(False)
    Private messageQueue As New Queue(Of Message)()

    ''' <summary>
    ''' This method is called by a sender to send a message to this client.
    ''' </summary>
    ''' <param name="message">the new message</param>
    Public Sub EnqueueMessage(ByVal message As Message)
        SyncLock messageQueue
            messageQueue.Enqueue(message)

            ' Set a new message event.
            messageEvent.[Set]()
        End SyncLock
    End Sub

    ''' <summary>
    ''' This method is called by the client to receive messages from the message queue.
    ''' If no message, it will wait until a new message is inserted.
    ''' </summary>
    ''' <returns>the unread message</returns>
    Public Function DequeueMessage() As Message
        ' Wait until a new message.
        messageEvent.WaitOne()

        SyncLock messageQueue
            If messageQueue.Count = 1 Then
                messageEvent.Reset()
            End If
            Return messageQueue.Dequeue()
        End SyncLock
    End Function
End Class