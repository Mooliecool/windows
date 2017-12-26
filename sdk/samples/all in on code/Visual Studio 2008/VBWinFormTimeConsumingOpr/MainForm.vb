'************************************* Module Header **************************************\
' Module Name:  MainForm.VB
' Project:      VBWinFormTimeConsumingOpr
' Copyright (c) Microsoft Corporation.
' 
' The Time-consuming Operation sample demonstrates how to use the BackgroundWorker 
' component to execute a time-consuming operation in the background.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'******************************************************************************************/

Imports System.ComponentModel
Imports System.Threading


Public Class MainForm
    Inherits Form

    Public Sub New()
        MyBase.New()
        InitializeComponent()

    End Sub

    Private Sub frmTimeConsumingOpr_Load(ByVal sender As Object, ByVal e As EventArgs) Handles Me.Load
        'To set up for a background operation, add an event handler for the DoWork event.
        AddHandler Me.backgroundWorkerTimeConsuming.DoWork, AddressOf backgroundWorker1_DoWork

        'To receive notifications of progress updates, handle the ProgressChanged event.
        AddHandler Me.backgroundWorkerTimeConsuming.ProgressChanged, AddressOf backgroundWorker1_ProgressChanged

        'To receive a notification when the operation is completed, handle the 
        'RunWorkerCompleted event. 
        AddHandler Me.backgroundWorkerTimeConsuming.RunWorkerCompleted, AddressOf backgroundWorker1_RunWorkerCompleted

        Me.ProgressBar1.Maximum = 100

        'This line ensures that we can report progress from the BackgroundWorker.  
        ' By default progress reporting is disabled.
        Me.backgroundWorkerTimeConsuming.WorkerReportsProgress = True
    End Sub

    Private Sub backgroundWorker1_DoWork(ByVal sender As Object, ByVal e As DoWorkEventArgs)
        ' Do the time-consuming work here
        Dim i As Integer = 1
        Do While (i <= 100)
            ' Suspend the thread to simulate time-consuming operation.
            Thread.Sleep(100)
            ' Report the current progress, calling this method will fire the
            ' ProgressChanged event.
            Me.backgroundWorkerTimeConsuming.ReportProgress(i, i.ToString)
            i = (i + 1)
        Loop
    End Sub

    Private Sub backgroundWorker1_ProgressChanged(ByVal sender As Object, ByVal e As ProgressChangedEventArgs)
        ' Display the progress.
        Me.ProgressBar1.Value = e.ProgressPercentage
        Me.lbResult.Text = e.UserState.ToString()
    End Sub

    Private Sub backgroundWorker1_RunWorkerCompleted(ByVal sender As Object, ByVal e As RunWorkerCompletedEventArgs)
        MessageBox.Show("Working done!")
    End Sub

    Private Sub btnStart_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnStart.Click
        ' Start the asynchronous operation.
        If Not Me.backgroundWorkerTimeConsuming.IsBusy Then
            Me.backgroundWorkerTimeConsuming.RunWorkerAsync()
        End If
    End Sub
End Class
