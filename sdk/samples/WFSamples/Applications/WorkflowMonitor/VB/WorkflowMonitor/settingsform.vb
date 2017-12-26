'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

' Settings dialog implementation - allows user to set application settings
' such as server and database names

Namespace WorkflowMonitor
    Public Class SettingsForm

        Private monitorSettings As ApplicationSettings

        Public Sub New(ByVal owner As MonitorForm)
            InitializeComponent()

            Me.Owner = owner
            Me.monitorSettings = owner.MonitorSettings

            Me.Database_Textbox.Text = monitorSettings.DatabaseName
            Me.Machine_TextBox.Text = monitorSettings.ServerName
            Me.Polling_TextBox.Text = monitorSettings.PollingInterval.ToString()
            Me.AutoSelectLatest_Checkbox.Checked = monitorSettings.AutoSelectLatest
        End Sub

        Private Sub OKButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles OKButton.Click
            Dim previousPollingInterval As Integer = monitorSettings.PollingInterval

            'Try to convert pollling interval to an int32 - if an error, revert to previous setting
            Try
                monitorSettings.DatabaseName = Me.Database_Textbox.Text
                monitorSettings.ServerName = Me.Machine_TextBox.Text
                monitorSettings.AutoSelectLatest = Me.AutoSelectLatest_Checkbox.Checked
                Dim pollingInterval As Integer = System.Convert.ToInt32(Polling_TextBox.Text)
                If pollingInterval > 0 Then
                    monitorSettings.PollingInterval = pollingInterval
                Else
                    MessageBox.Show("Polling interval must be greater than 0.", "", MessageBoxButtons.OK, MessageBoxIcon.Error)
                    Return
                End If
            Catch
                monitorSettings.PollingInterval = previousPollingInterval
            End Try

            CType(Owner, MonitorForm).MonitorDatabaseService.DatabaseName = monitorSettings.DatabaseName
            CType(Owner, MonitorForm).MonitorDatabaseService.ServerName = monitorSettings.ServerName

            Me.Close()
        End Sub

        Private Sub Cancel_Button_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Cancel_Button.Click
            Me.Close()
        End Sub
    End Class
End Namespace
