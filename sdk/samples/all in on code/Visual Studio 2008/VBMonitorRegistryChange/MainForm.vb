'*************************** Module Header ******************************\
' Module Name:  MainForm.vb
' Project:	    VBMonitorRegistryChange
' Copyright (c) Microsoft Corporation.
' 
' This is the main form of this application. It is used to initialize the UI and 
' handle the events.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************

Imports System.Management
Imports Microsoft.Win32

Partial Public Class MainForm
    Inherits Form

    ' Current status
    Private isMonitoring As Boolean = False

    Private watcher As RegistryWatcher = Nothing

    Public Sub New()
        InitializeComponent()
    End Sub

    Private Sub MainForm_Load(ByVal sender As Object, ByVal e As EventArgs) Handles MyBase.Load

        ' Initialize the data source of cmbHives. Changes to the HKEY_CLASSES_ROOT 
        ' and HKEY_CURRENT_USER hives are not supported by RegistryEvent or classes
        ' derived from it, such as RegistryKeyChangeEvent.        
        cmbHives.DataSource = RegistryWatcher.SupportedHives

    End Sub

    ''' <summary>
    ''' Handle the click event of btnMonitor.
    ''' </summary>
    Private Sub btnMonitor_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnMonitor.Click

        ' If this application is monitoring the registry key, then stop monitoring 
        ' and enable the editors.
        If isMonitoring Then
            Dim success As Boolean = StopMonitor()
            If success Then
                btnMonitor.Text = "Start Monitor"
                cmbHives.Enabled = True
                tbRegkeyPath.ReadOnly = False
                isMonitoring = False
                lstChanges.Items.Add(String.Format("{0} Stop monitoring", Date.Now))
            End If

            ' If this application is idle, then start to monitor and disable the editors.
        Else
            Dim success As Boolean = StartMonitor()
            If success Then
                btnMonitor.Text = "Stop Monitor"
                cmbHives.Enabled = False
                tbRegkeyPath.ReadOnly = True
                isMonitoring = True
                lstChanges.Items.Add(String.Format("{0} Start monitoring", Date.Now))
            End If
        End If

    End Sub

    ''' <summary>
    ''' Check whether the key to be monitored exists, and then 
    ''' start ManagementEventWatcher to watch the RegistryKeyChangeEvent
    ''' </summary>
    ''' <returns>True if the ManagementEventWatcher starts successfully.</returns>
    Private Function StartMonitor() As Boolean
        Dim hive As RegistryKey = TryCast(cmbHives.SelectedValue, RegistryKey)
        Dim keyPath = tbRegkeyPath.Text.Trim()

        Try
            watcher = New RegistryWatcher(hive, keyPath)

            ' The constructor of RegistryWatcher may throw a SecurityException when
            ' the key to monitor does not exist. 
        Catch _ArgumentException As ArgumentException
            MessageBox.Show(_ArgumentException.Message)
            Return False

            ' The constructor of RegistryWatcher may throw a SecurityException when
            ' current user does not have the permission to access the key to monitor. 
        Catch _SecurityException As System.Security.SecurityException
            Dim message As String = String.Format("You do not have permission to access the key {0}\{1}.", hive.Name, keyPath)
            MessageBox.Show(message)
            Return False
        End Try

        Try

            ' Set up the handler that will handle the change event.
            AddHandler watcher.RegistryKeyChangeEvent, AddressOf watcher_RegistryKeyChangeEvent

            ' Start listening for events.
            watcher.Start()
            Return True
        Catch comException As System.Runtime.InteropServices.COMException
            MessageBox.Show("An error occurred: " & comException.Message)
            Return False
        Catch managementException_Renamed As ManagementException
            MessageBox.Show("An error occurred: " & managementException_Renamed.Message)
            Return False
        End Try

    End Function

    ''' <summary>
    ''' Stop listening for events.
    ''' </summary>
    ''' <returns>True if ManagementEventWatcher stops successfully.</returns>
    Private Function StopMonitor() As Boolean
        Try
            watcher.Stop()
            Return True
        Catch _ManagementException As ManagementException
            MessageBox.Show("An error occurred: " & _ManagementException.Message)
            Return False
        Finally
            watcher.Dispose()
        End Try
    End Function

    ''' <summary>
    ''' Handle the RegistryKeyChangeEvent.
    ''' </summary>
    Private Sub watcher_RegistryKeyChangeEvent(ByVal sender As Object, ByVal e As RegistryKeyChangeEventArgs)
        Dim newEventMessage As String = String.Format( _
            "{0} The key {1}\{2} changed", _
            e.TIME_CREATED.ToLocalTime(), _
            e.Hive, _
            e.KeyPath)
        lstChanges.Items.Add(newEventMessage)
    End Sub

End Class
