'****************************** Module Header ******************************\
' Module Name:    Default.aspx.vb
' Project:        VBASPNETBackgroundWorker
' Copyright (c) Microsoft Corporation
'
' This page displays a TextBox. When the user clicks the Button, the page creates 
' a BackgroundWorker object then starts it by passing the value which is inputed 
' through the TextBox. At last, the BackgroundWorker object is stored in Session 
' State, so that it will keep working even the current request ended.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Imports System.Threading

Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Timer1_Tick(ByVal sender As Object, ByVal e As EventArgs)
        ' Show the progress of current operation.
        Dim worker As BackgroundWorker = DirectCast(Session("worker"), BackgroundWorker)
        If worker IsNot Nothing Then
            ' Display the progress of the operation.
            lbProgress.Text = "Running: " & worker.Progress.ToString() & "%"

            btnStart.Enabled = Not worker.IsRunning
            Timer1.Enabled = worker.IsRunning

            ' Display the result when the operation completed.
            If worker.Progress >= 100 Then
                lbProgress.Text = DirectCast(worker.Result, String)
            End If
        End If
    End Sub

    ''' <summary>
    ''' Create a Background Worker to run the operation when button clicked.
    ''' </summary>
    Protected Sub btnStart_Click(ByVal sender As Object, ByVal e As EventArgs)
        Dim worker As New BackgroundWorker()
        AddHandler worker.DoWork, AddressOf worker_DoWork
        worker.RunWorker({txtParameter.Text})

        ' It needs Session Mode is "InProc"
        ' to keep the Background Worker working.
        Session("worker") = worker

        ' Enable the timer to update the status of the operation.
        Timer1.Enabled = True
    End Sub

    ''' <summary>
    ''' This method is the operation that needs long time to complete.
    ''' </summary>
    Private Sub worker_DoWork(ByRef progress As Integer, ByRef result As Object, ByVal ParamArray arguments As Object())
        ' Get the value which passed to this operation.
        Dim input As String = String.Empty
        If arguments.Length > 0 Then
            input = arguments(0).ToString()
        End If

        ' Need 10 seconds to complete this operation.
        For i As Integer = 0 To 99
            Thread.Sleep(100)

            progress += 1
        Next

        ' The operation is completed.
        progress = 100
        result = "Operation is completed. The input is """ & input & """."
    End Sub

End Class