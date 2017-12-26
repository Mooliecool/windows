'****************************** Module Header ******************************\
' Project Name:   CSAzureWebRoleBackendProcessing
' Module Name:    Common
' File Name:      BackendProcessor.vb
' Copyright (c) Microsoft Corporation
'
' This class runs a backend task in a new thread.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports Microsoft.WindowsAzure.StorageClient
Imports System.Threading

Public Class BackendProcessor
    ''' <summary>
    ''' Start the running in backend.
    ''' </summary>
    Public Sub Start()
        Dim thread = New Thread(AddressOf Run)
        thread.Start()

        Trace.TraceInformation("BackendProcessor is started.")
    End Sub

    ''' <summary>
    ''' This method serves as the main thread of backend processing.
    ''' The Run method should implement a long-running thread that carries
    ''' out operations
    ''' </summary>
    Public Sub Run()
        While True
            Try
                Dim ds As New DataSources()

                ' Retrieve a new message from the queue.
                Dim msg As CloudQueueMessage = ds.WordQuque.GetMessage(TimeSpan.FromSeconds(1))

                If msg IsNot Nothing Then
                    ' Process the task.
                    Dim messageParts = msg.AsString.Split(New Char() {","c})
                    Dim entry As WordEntry = ds.GetWordEntry(messageParts(0), messageParts(1))

                    entry.Content = entry.Content.ToUpper()
                    entry.IsProcessed = True
                    ds.UpdateWordEntry(entry)

                    ' Delete message only when the task is completed.
                    ds.WordQuque.DeleteMessage(msg)
                    Trace.TraceInformation("Process result ""{0}"".", entry.Content)
                Else
                    Thread.Sleep(1000)
                End If
            Catch ex As Exception
                Trace.TraceError("Exception when processing queue item. Message: '{0}'", ex.Message)
                System.Threading.Thread.Sleep(5000)
            End Try
        End While
    End Sub
End Class