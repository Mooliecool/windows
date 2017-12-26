'----------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
'  
'   Copyright (C) Microsoft Corporation.  All rights reserved.
'  
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
'  
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'----------------------------------------------------------------------

Imports System
Imports System.IO
Imports System.Windows.Forms
Imports System.Collections.ObjectModel

Public Class MainForm

    Private Sub ButtonExit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonExit.Click
        Close()
    End Sub

    Private Sub ButtonGetWorkflowEvents_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ButtonGetWorkflowEvents.Click
        Try
            If Me.TextBoxInstanceId Is Nothing Or Me.TextBoxInstanceId.Text.Length = 0 Then
                MessageBox.Show("Please Enter a Valid Workflow Instance ID")
            Else

                Dim trackingFile As String = SimpleTrackingChannel.TrackingDataFilePrefix + Me.TextBoxInstanceId.Text.ToString() + ".txt"
                If File.Exists(trackingFile) Then
                    LabelReadingStatus.Text = "reading..."
                    ListViewWorkflowEvents.Clear()
                    Dim workflowEvents As Collection(Of String) = QueryLayer.GetTrackedWorkflowEvents(trackingFile)
                    For Each workflowEvent As String In workflowEvents
                        ListViewWorkflowEvents.Items.Add(workflowEvent)
                    Next
                    LabelReadingStatus.Text = ""
                Else
                    MessageBox.Show("File " + trackingFile + " doesn't exist")
                End If
            End If
        Catch ex As Exception

            MessageBox.Show("Encountered an exception. Exception Source: " + ex.Source.ToString() + ", Exception Message: " + ex.Message.ToString())
        Finally
            LabelReadingStatus.Text = ""
        End Try

    End Sub


End Class
