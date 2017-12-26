'---------------------------------------------------------------------
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
'---------------------------------------------------------------------

Imports System.Workflow.Runtime.Tracking

Public Class UpdateProfileVersion
    Dim workflowTypeValue As Type
    Dim currentTrackingProfileValue As TrackingProfile
    Dim newTrackingProfileValue As TrackingProfile

    ' Gets or sets the workflow type to be saved.
    Public Property WorkflowType() As Type
        Get
            Return workflowTypeValue
        End Get

        Set(ByVal value As Type)
            workflowTypeValue = value
        End Set
    End Property

    ' Gets or sets the existing tracking profile
    Public Property CurrentTrackingProfile() As TrackingProfile
        Get
            Return currentTrackingProfileValue
        End Get
        Set(ByVal value As TrackingProfile)
            currentTrackingProfileValue = value
        End Set
    End Property

    ' Gets or sets the new tracking profile
    Public Property NewTrackingProfile() As TrackingProfile
        Get
            Return newTrackingProfileValue
        End Get

        Set(ByVal value As TrackingProfile)
            newTrackingProfileValue = value
        End Set
    End Property

    Private Sub SaveTrackingProfileLoad(ByVal sender As Object, ByVal e As EventArgs) Handles Me.Load
        Me.existingProfileLabel.Text += currentTrackingProfileValue.Version.ToString()
    End Sub

    Private Sub SaveClick(ByVal sender As Object, ByVal e As EventArgs) Handles save.Click
        Try
            Me.NewTrackingProfile.Version = New Version(Me.profileVersion.Text)
            If (Me.NewTrackingProfile.Version <= Me.CurrentTrackingProfile.Version) Then
                MessageBox.Show(String.Format("Please enter a version greater than the current version ({0})", Me.CurrentTrackingProfile.Version.ToString()), "Incorrect Version")
                Me.DialogResult = Windows.Forms.DialogResult.OK
                Me.Close()
            End If
        Catch
            MessageBox.Show("Please enter a valid version string (i.e. '1.0.0')", "Incorrect Version")
        End Try
    End Sub

    Private Sub CancelClick(ByVal sender As Object, ByVal e As EventArgs) Handles cancel.Click
        Me.DialogResult = Windows.Forms.DialogResult.Cancel
        Me.Close()
    End Sub
End Class