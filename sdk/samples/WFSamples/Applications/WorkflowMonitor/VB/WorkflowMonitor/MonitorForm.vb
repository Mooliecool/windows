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

Imports Microsoft.VisualBasic.Constants
Imports System
Imports System.Windows.Forms
Imports System.Workflow.ComponentModel
Imports System.Workflow.Runtime.Tracking

Namespace WorkflowMonitor
    Public Class MonitorForm
        Private displayedWorkflows As List(Of SqlTrackingWorkflowInstance) = Nothing
        Private selectedWorkflowEvent As String = String.Empty
        Private workflowInstanceIdToFind As Guid = Guid.Empty

        Private statusFromDateTime As System.DateTime = New System.DateTime(2000, 1, 1)
        Private statusUntilDateTime As System.DateTime = DateTime.Now.AddDays(1)

        Private trackingDataItemValue As TrackingDataItemValue = New TrackingDataItemValue(String.Empty, String.Empty, String.Empty)

        Private monitorSettingsValue As ApplicationSettings
        Private monitorDatabaseServiceValue As DatabaseService

        Private isMonitoring As Boolean = False

        Private workflowStatusList As New Dictionary(Of String, WorkflowStatusInfo)
        Private activityStatusListValue As New Dictionary(Of String, ActivityStatusInfo)

        Private WithEvents workflowViewHost As ViewHost


        Public Sub New()
            InitializeComponent()

            Me.toolStripTextBoxFrom.Text = statusFromDateTime.ToString()
            Me.toolStripTextBoxUntil.Text = statusUntilDateTime.ToString()
            Me.ToolStripTextBoxWorkflowInstanceId.Text = Guid.Empty.ToString()
            Me.listViewWorkflows.ListViewItemSorter = New ListViewItemComparer(0, False)
            Me.listViewActivities.ListViewItemSorter = New ListViewItemComparer(0, False)

            workflowViewHost = New ViewHost(Me)

            Me.trackingSurface.Panel2.SuspendLayout()
            Me.trackingSurface.Panel2.Controls.Clear()
            Me.trackingSurface.Panel2.Controls.Add(viewHostLabel)
            Me.trackingSurface.Panel2.Controls.Add(workflowViewHost)
            Me.trackingSurface.Panel2.Controls.Add(workflowViewErrorText)
            Me.trackingSurface.Panel2.ResumeLayout(True)

            Me.monitorSettingsValue = New ApplicationSettings()
            Me.monitorDatabaseServiceValue = New DatabaseService()
            Me.workflowStatusList = New Dictionary(Of String, WorkflowStatusInfo)()
            Me.activityStatusListValue = New Dictionary(Of String, ActivityStatusInfo)()

            Me.monitorSettingsValue.DatabaseName = monitorDatabaseServiceValue.DatabaseName
            Me.monitorSettingsValue.ServerName = monitorDatabaseServiceValue.ServerName

            'Read app settings
            If monitorSettingsValue.LoadAppSettings(Application.LocalUserAppDataPath + "\workflowmonitor.config") Then
                monitorDatabaseServiceValue.DatabaseName = monitorSettingsValue.DatabaseName
                monitorDatabaseServiceValue.ServerName = monitorSettingsValue.ServerName
            Else
                'If no application settings default to localhost and Tracking for server and database
                monitorSettingsValue.DatabaseName = "Tracking"
                monitorSettingsValue.ServerName = "LocalHost"
                monitorSettingsValue.PollingInterval = 5000
                monitorSettingsValue.AutoSelectLatest = False
                monitorDatabaseServiceValue.DatabaseName = monitorSettingsValue.DatabaseName
                monitorDatabaseServiceValue.ServerName = monitorSettingsValue.ServerName
            End If

            Me.statusLabelDatabaseNamePanel.Text = "Connected to: " + monitorDatabaseServiceValue.ServerName + "/" + monitorDatabaseServiceValue.DatabaseName

            DisplayWorkflows(selectedWorkflowEvent)
        End Sub

        Friend Sub DisplayWorkflowById(ByVal workflowInstanceId As Guid)
            DisplayWorkflows(Nothing, workflowInstanceId, statusFromDateTime, statusUntilDateTime, trackingDataItemValue)
        End Sub

        Friend Sub DisplayWorkflows(ByVal selectedWorkflowEvent As String)
            DisplayWorkflows(selectedWorkflowEvent, Guid.Empty, statusFromDateTime, statusUntilDateTime, trackingDataItemValue)
        End Sub

        Friend Sub DisplayWorkflows(ByVal selectedWorkflowEvent As String, ByVal statusFrom As System.DateTime, ByVal statusUntil As System.DateTime, ByVal trackingDataItemValue As TrackingDataItemValue)
            DisplayWorkflows(selectedWorkflowEvent, Guid.Empty, statusFrom, statusUntil, trackingDataItemValue)
        End Sub

        Friend Sub DisplayWorkflows(ByVal selectedWorkflowEvent As String, ByVal workflowInstanceId As Guid, ByVal statusFrom As System.DateTime, ByVal statusUntil As System.DateTime, ByVal trackingDataItemValue As TrackingDataItemValue)
            ' Try to get all of the workflows from the tracking database
            Dim instance As SqlTrackingWorkflowInstance
            Try
                If ((Not workflowInstanceId = Nothing) And (Not workflowInstanceId = Guid.Empty)) Then
                    displayedWorkflows.Clear()
                    instance = monitorDatabaseServiceValue.GetWorkflow(workflowInstanceId)
                    If Not instance Is Nothing Then
                        displayedWorkflows.Add(monitorDatabaseServiceValue.GetWorkflow(workflowInstanceId))
                    End If
                Else
                    displayedWorkflows = monitorDatabaseServiceValue.GetWorkflows(selectedWorkflowEvent, statusFrom, statusUntil, trackingDataItemValue)
                End If
                listViewWorkflows.Items.Clear()
                workflowStatusList.Clear()

                ' For every workflow instance create a new WorkflowStatusInfo object and store in the workflowStatusList
                ' Also populate the workflow ListView
                For Each sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance In displayedWorkflows
                    Dim workflowType As String = "XAML"
                    If sqlTrackingWorkflowInstance.WorkflowType IsNot Nothing Then
                        workflowType = sqlTrackingWorkflowInstance.WorkflowType.ToString()
                    End If

                    Dim listViewItem As ListViewItem = New ListViewItem(New String() { _
                        sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(), _
                        workflowType, _
                        sqlTrackingWorkflowInstance.Status.ToString()}, -1)

                    listViewWorkflows.Items.Add(listViewItem)

                    workflowStatusList.Add(sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(), _
                                New WorkflowStatusInfo( _
                                    sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(), _
                                    workflowType, _
                                    sqlTrackingWorkflowInstance.Status.ToString(), _
                                    sqlTrackingWorkflowInstance.Initialized.ToString(), _
                                    sqlTrackingWorkflowInstance.WorkflowInstanceId, _
                                    listViewItem))
                Next
                'If there is at least one workflow, populate the Activities list
                If listViewWorkflows.Items.Count > 0 Then
                    Me.listViewWorkflows.Focus()
                    Dim listItem As ListViewItem = listViewWorkflows.Items(0)
                    listItem.Focused = True
                    listItem.Selected = True
                    UpdateTitle()
                    UpdateActivities()
                    Me.statusLabelMonitoringPanel.Text = String.Empty
                End If
                ' Display number of workflow instances
                If displayedWorkflows.Count > 0 Then
                    Me.workflowsLabel.Text = " Workflows - " + displayedWorkflows.Count.ToString() + " records"
                    ShowViewHost(True)
                Else
                    Me.workflowsLabel.Text = " Workflows - No records"
                    ShowViewHost(False)
                End If
                ' Clear all the lists and reset the UI if there are errors
            Catch ex As Exception
                If Not ex.InnerException Is Nothing Then
                    MessageBox.Show(ex.Message + vbCrLf + ex.InnerException.Message + vbCrLf + "Ensure your settings are correct and that you have run SqlTrackingService database schema and logic scripts", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
                Else
                    MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
                End If

                Me.statusLabelMonitoringPanel.Text = "Error"
                ShowSettingsDialog()

                workflowStatusList.Clear()
                listViewWorkflows.Items.Clear()

                activityStatusListValue.Clear()
                listViewActivities.Items.Clear()

                Me.workflowsLabel.Text = "Workflows - No Records"
            End Try
        End Sub

        Friend ReadOnly Property MonitorDatabaseService() As DatabaseService
            Get
                Return monitorDatabaseServiceValue
            End Get
        End Property

        Friend ReadOnly Property ActivityStatusList() As Dictionary(Of String, ActivityStatusInfo)
            Get
                Return activityStatusListValue
            End Get
        End Property

        Friend ReadOnly Property MonitorSettings() As ApplicationSettings
            Get
                Return monitorSettingsValue
            End Get
        End Property

        ' F5 Refresh
        Friend Sub ManualRefresh(ByVal sender As Object, ByVal e As KeyEventArgs) Handles listViewWorkflows.KeyUp, listViewActivities.KeyUp
            If e.KeyCode = Keys.F5 Then
                RefreshView()
            End If
        End Sub

        ' Sort the workflow list if the user clicks the column headers
        Friend Sub OnWorkflowsColumnClick(ByVal sender As Object, ByVal e As ColumnClickEventArgs) Handles listViewWorkflows.ColumnClick
            'If the column is the id column then sort numeric
            If e.Column = 0 Then
                Me.listViewWorkflows.ListViewItemSorter = New ListViewItemComparer(e.Column, False)
            Else
                Me.listViewWorkflows.ListViewItemSorter = New ListViewItemComparer(e.Column, True)
            End If
        End Sub

        ' Sort the activities list if the user clicks the column headers
        Friend Sub OnActivitiesColumnClick(ByVal sender As Object, ByVal e As ColumnClickEventArgs) Handles listViewActivities.ColumnClick
            'If the column is the id column then sort numeric
            If e.Column = 0 Then
                Me.listViewActivities.ListViewItemSorter = New ListViewItemComparer(e.Column, False)
            Else
                Me.listViewActivities.ListViewItemSorter = New ListViewItemComparer(e.Column, True)
            End If
        End Sub

        ' Set the window title to be the name of the workflow if one is selected
        Friend Sub UpdateTitle()
            If listViewWorkflows.SelectedItems.Count = 0 Or Me.displayedWorkflows.Count = 0 Then
                Me.Text = "Workflow Monitor"
                Return
            End If

            Dim currentSelectedWorkflow As ListViewItem = listViewWorkflows.SelectedItems(0)
            If Not currentSelectedWorkflow Is Nothing Then
                Me.Text = currentSelectedWorkflow.SubItems(1).Text + " - Workflow Monitor"
            Else
                Me.Text = "Workflow Monitor"
            End If
        End Sub

        Protected Overrides Sub OnLayout(ByVal levent As System.Windows.Forms.LayoutEventArgs)
            MyBase.OnLayout(levent)

            Me.statusStrip.SuspendLayout()
            Me.statusLabelDatabaseNamePanel.Visible = True
            Me.statusLabelMonitoringPanel.Visible = True

            If Me.Width < Me.statusLabelDatabaseNamePanel.Width + Me.statusLabelMonitoringPanel.Width + 10 Then
                Me.statusLabelDatabaseNamePanel.Visible = False
            End If

            If Me.Width < Me.statusLabelMonitoringPanel.Width + 10 Then
                Me.statusLabelMonitoringPanel.Visible = False
            End If

            Me.statusStrip.ResumeLayout(True)
        End Sub

        Private Sub ToolStripButtonSettings_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles toolStripButtonSettings.Click
            menuSettings_Click(sender, e)
        End Sub

        Private Sub menuSettings_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles menuSettings.Click
            ShowSettingsDialog()
        End Sub


        Friend Sub ShowSettingsDialog()
            Dim applicationSettings As SettingsForm = New SettingsForm(Me)
            applicationSettings.ShowDialog()

            If MonitorSettings.PollingInterval > 0 Then
                timer.Interval = MonitorSettings.PollingInterval
            End If

            Me.statusLabelDatabaseNamePanel.Text = "Connected to: " + monitorDatabaseServiceValue.ServerName + "/" + monitorDatabaseServiceValue.DatabaseName
            If isMonitoring AndAlso MonitorSettings.AutoSelectLatest AndAlso listViewWorkflows.Items.Count > 0 Then
                listViewWorkflows.Focus()
                listViewWorkflows.Items(listViewWorkflows.Items.Count - 1).Selected = True
            End If
        End Sub

        Private Sub ToolStripButtonMonitorOn_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles toolStripButtonMonitorOn.Click
            Monitor(True)
        End Sub

        Private Sub ToolStripButtonMonitorOff_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles toolStripButtonMonitorOff.Click
            Monitor(False)
        End Sub

        Friend Sub MenuStartStop_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles menuStartStop.Click
            If menuStartStop.Text = "Start" Then
                Monitor(True)
            Else
                Monitor(False)
            End If
        End Sub

        Private Sub ToolStripButtonCollapse_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles toolStripButtonCollapse.Click
            Me.workflowViewHost.Expand(False)
        End Sub

        Private Sub ToolStripButtonExpand_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles toolStripButtonExpand.Click
            Me.workflowViewHost.Expand(True)
        End Sub

        Friend Sub Monitor(ByVal toggle As Boolean)
            If toggle Then
                timer.Start()
                menuStartStop.Text = "Stop"
                toolStripButtonMonitorOff.Enabled = True
                toolStripButtonMonitorOn.Enabled = False
                Me.statusLabelMonitoringPanel.Text = "Monitoring"

                If MonitorSettings.AutoSelectLatest AndAlso listViewWorkflows.Items.Count > 0 Then
                    listViewWorkflows.Focus()
                    listViewWorkflows.Items(listViewWorkflows.Items.Count - 1).Selected = True
                End If
                Me.isMonitoring = True
            Else
                timer.Stop()
                Me.isMonitoring = False
                menuStartStop.Text = "Start"
                toolStripButtonMonitorOff.Enabled = False
                toolStripButtonMonitorOn.Enabled = True
                Me.statusLabelMonitoringPanel.Text = String.Empty
            End If
        End Sub

        Private Sub menuExit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles menuExit.Click
            Me.Close()
        End Sub

        'Zoom level combo box handling
        Private Sub ToolStripComboBoxZoom_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles toolStripComboBoxZoom.SelectedIndexChanged
            If workflowViewHost.WorkflowView Is Nothing Then
                Return
            End If

            'Parse the value and set the WorkflowView zoom - set to 100% if invalid
            Dim NewZoom As String = Me.toolStripComboBoxZoom.Text.Trim()
            If NewZoom.EndsWith("%") Then
                NewZoom = NewZoom.Substring(0, NewZoom.Length - 1)
            End If

            If NewZoom.Length > 0 Then
                Try
                    Me.workflowViewHost.WorkflowView.Zoom = Convert.ToInt32(NewZoom)
                Catch
                    Me.workflowViewHost.WorkflowView.Zoom = 100
                End Try
            Else
                Me.workflowViewHost.WorkflowView.Zoom = 100
            End If
        End Sub

        'Refresh info from the database (invoked by F5 refresh and polling timer)
        Friend Sub RefreshView()
            Try
                'Get all of the workflow instances from the database and refresh the status if changed
                displayedWorkflows = monitorDatabaseServiceValue.GetWorkflows(String.Empty, DateTime.MinValue, DateTime.MaxValue, trackingDataItemValue)
                For Each sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance In displayedWorkflows
                    Dim workflowType As String = "XAML"
                    If sqlTrackingWorkflowInstance.WorkflowType IsNot Nothing Then
                        workflowType = sqlTrackingWorkflowInstance.WorkflowType.ToString()
                    End If

                    Dim workflowStatus As WorkflowStatusInfo = New WorkflowStatusInfo(String.Empty, String.Empty, String.Empty, String.Empty, Guid.Empty, New ListViewItem())
                    If workflowStatusList.TryGetValue(sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(), workflowStatus) Then
                        workflowStatus.Status = sqlTrackingWorkflowInstance.Status.ToString()
                        workflowStatus.WorkflowListViewItem.SubItems(2).Text = sqlTrackingWorkflowInstance.Status.ToString()
                    Else
                        Dim listViewItem As ListViewItem = New ListViewItem(New String() { _
                            sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(), _
                            workflowType, _
                            sqlTrackingWorkflowInstance.Status.ToString()}, -1)

                        listViewWorkflows.Items.Add(listViewItem)
                        listViewWorkflows.Focus()

                        workflowStatusList.Add(sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(), _
                                    New WorkflowStatusInfo( _
                                    sqlTrackingWorkflowInstance.WorkflowInstanceInternalId.ToString(), _
                                    workflowType, _
                                    sqlTrackingWorkflowInstance.Status.ToString(), _
                                    sqlTrackingWorkflowInstance.Initialized.ToString(), _
                                    sqlTrackingWorkflowInstance.WorkflowInstanceId, _
                                    listViewItem))
                        If MonitorSettings.AutoSelectLatest Then
                            listViewItem.Selected = True
                        End If
                    End If
                Next

                If displayedWorkflows.Count > 0 Then
                    Me.workflowsLabel.Text = " Workflows - " + displayedWorkflows.Count.ToString() + " records"
                    ShowViewHost(True)
                    ' Update the activity view since the selection may have changed
                    UpdateActivities()
                    
                Else
                    Me.workflowsLabel.Text = " Workflows - no records"
                    ShowViewHost(False)
                    Me.listViewWorkflows.Items.Clear()
                    Me.listViewActivities.Items.Clear()
                    UpdateTitle()
                End If
            Catch ex As Exception
                'Show error dialog if anything bad happen - likely a database error
                Monitor(False)
                MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
                Me.statusLabelMonitoringPanel.Text = "Error"

                workflowStatusList.Clear()
                listViewWorkflows.Items.Clear()

                activityStatusListValue.Clear()
                listViewActivities.Items.Clear()

                Me.workflowsLabel.Text = " Workflows - no records"
            End Try

            workflowViewHost.Refresh()
        End Sub

        ' Refresh the view when the polling timer expires
        Private Sub Timer_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timer.Tick
            RefreshView()
        End Sub

        ' Change the workflow view and activities if the workflow instance selected has changed
        ' It can be changed by the user or if during polling if AutoSelectLatest is true
        Private Sub ListViewWorkflows_ItemSelectionChanged(ByVal sender As System.Object, ByVal e As System.Windows.Forms.ListViewItemSelectionChangedEventArgs) Handles listViewWorkflows.ItemSelectionChanged
            If listViewWorkflows.SelectedItems.Count = 0 Then
                Return
            End If

            Try
                UpdateTitle()

                Dim currentWorkflow As ListViewItem = listViewWorkflows.SelectedItems(0)
                Try
                    Dim workflowInstanceId As Guid = workflowStatusList(currentWorkflow.SubItems(0).Text).InstanceId
                    GetWorkflowDefinition(workflowInstanceId)
                    ShowViewHost(True)
                    Me.toolStripComboBoxZoom.Enabled = True
                    Me.toolStripButtonCollapse.Enabled = True
                    Me.toolStripButtonExpand.Enabled = True
                Catch
                    ShowViewHost(False)
                    Me.toolStripComboBoxZoom.Enabled = False
                    Me.toolStripButtonCollapse.Enabled = False
                    Me.toolStripButtonExpand.Enabled = False
                End Try

                Me.toolStripComboBoxZoom.SelectedIndex = 2

                UpdateActivities()

                If menuStartStop.Text = "Stop" Then
                    Me.statusLabelMonitoringPanel.Text = "Monitoring"
                Else
                    Me.statusLabelMonitoringPanel.Text = String.Empty
                End If

                workflowViewHost.Refresh()
            Catch ex As Exception
                MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
                Me.statusLabelMonitoringPanel.Text = "Error"
            End Try
        End Sub


        ' Turn on or off the workflows and activities details
        Private Sub menuActivityDetailsView_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles menuActivityDetailsView.Click
            If menuActivityDetailsView.Checked Then
                Me.menuActivityDetailsView.Checked = False
                Me.trackingSurface.Panel1Collapsed = True
            Else
                Me.menuActivityDetailsView.Checked = True
                Me.trackingSurface.Panel1Collapsed = False
                Me.trackingSurface.Panel2Collapsed = False
            End If
        End Sub

        ' Query the database via the databaseService and update the activities ListView and activityStatusList
        ' based on the currently selected workflow instance
        Friend Sub UpdateActivities()
            If listViewWorkflows.SelectedItems.Count = 0 Then
                listViewActivities.Items.Clear()
                activityStatusListValue.Clear()
                Return
            End If

            Dim currentWorkflow As ListViewItem = listViewWorkflows.SelectedItems(0)
            If Not currentWorkflow Is Nothing Then
                Dim workflowInstanceId As Guid = workflowStatusList((currentWorkflow.SubItems(0)).Text).InstanceId
                Dim sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance = monitorDatabaseServiceValue.GetWorkflow(workflowInstanceId)

                listViewActivities.Items.Clear()
                activityStatusListValue.Clear()

                ' ActivityEvents list received contain all events for activities in orders in event order
                ' Walking down-up on the ActivityEvents list and keeping the last entry

                For index As Integer = sqlTrackingWorkflowInstance.ActivityEvents.Count To 1 Step -1
                    Dim activityTrackingRecord As ActivityTrackingRecord = sqlTrackingWorkflowInstance.ActivityEvents(index - 1)
                    If (Not activityStatusListValue.ContainsKey(activityTrackingRecord.QualifiedName)) Then
                        Dim latestActivityStatus As ActivityStatusInfo = New ActivityStatusInfo(activityTrackingRecord.QualifiedName, activityTrackingRecord.ExecutionStatus.ToString())
                        activityStatusListValue.Add(activityTrackingRecord.QualifiedName, latestActivityStatus)

                        Dim activitiesListViewItems As String() = New String() { _
                            activityTrackingRecord.EventOrder.ToString(), _
                            activityTrackingRecord.QualifiedName, _
                            activityTrackingRecord.ExecutionStatus.ToString()}
                        Dim li As ListViewItem = New ListViewItem(activitiesListViewItems, -1)
                        listViewActivities.Items.Add(li)
                    End If
                Next
                workflowViewHost.Refresh()
            End If
        End Sub

        'Get the workflow definition from the database and load viewhost
        Friend Sub GetWorkflowDefinition(ByVal workflowInstanceId As Guid)
            Dim sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance = monitorDatabaseServiceValue.GetWorkflow(workflowInstanceId)
            If ((Not sqlTrackingWorkflowInstance Is Nothing) AndAlso (Not sqlTrackingWorkflowInstance.WorkflowDefinition Is Nothing)) Then
                Dim workflowDefinition As Activity = sqlTrackingWorkflowInstance.WorkflowDefinition
                If (Not workflowDefinition Is Nothing) Then
                    workflowViewHost.OpenWorkflow(workflowDefinition)
                End If
            Else
                Throw (New Exception())
            End If
        End Sub

        Private Sub Mainform_Closing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
            Dim path As String = Application.LocalUserAppDataPath + "\workflowmonitor.config"
            monitorSettingsValue.SaveSettings(path)
            MyBase.OnClosed(e)
        End Sub

        Private Sub ToolStripComboBoxWorkflowEvent_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles toolStripComboBoxWorkflowEvent.SelectedIndexChanged
            selectedWorkflowEvent = Me.toolStripComboBoxWorkflowEvent.Text.Trim()
        End Sub

        Private Sub toolStripButtonFindWorkflows_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles toolStripButtonFindWorkflows.Click
            statusFromDateTime = DateTime.Parse(Me.toolStripTextBoxFrom.Text.ToString())
            statusUntilDateTime = DateTime.Parse(Me.toolStripTextBoxUntil.Text.ToString())

            trackingDataItemValue = New TrackingDataItemValue(String.Empty, String.Empty, String.Empty)
            trackingDataItemValue.QualifiedName = Me.toolStripTextBoxArtifactQualifiedId.Text.ToString()
            trackingDataItemValue.FieldName = Me.toolStripTextBoxArtifactKeyName.Text.ToString()
            trackingDataItemValue.DataValue = Me.toolStripTextBoxArtifactKeyValue.Text.ToString()

            If (Not ((trackingDataItemValue.QualifiedName.Equals(String.Empty)) And (trackingDataItemValue.FieldName.Equals(String.Empty)) And (trackingDataItemValue.DataValue.Equals(String.Empty)))) Then
                If ((trackingDataItemValue.QualifiedName.Equals(String.Empty)) Or (trackingDataItemValue.FieldName.Equals(String.Empty)) Or ((trackingDataItemValue.DataValue.Equals(String.Empty)))) Then
                    MessageBox.Show("If you wish to filter by artifacts you need to set the three inputs: Activity Qualified Name, Property Name, and Value.", "Workflow Monitor Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning)
                End If
            End If


            Me.listViewWorkflows.Items.Clear()
            DisplayWorkflows(selectedWorkflowEvent, statusFromDateTime, statusUntilDateTime, trackingDataItemValue)
            UpdateActivities()
        End Sub

        Private Sub ToolStripButtonFindWorkflowById_Click(ByVal sender As System.Object, ByVal e As EventArgs) Handles ToolStripButtonFindWorkflowById.Click
            Try
                workflowInstanceIdToFind = New Guid(Me.ToolStripTextBoxWorkflowInstanceId.Text.ToString())
            Catch ex As System.FormatException
                MessageBox.Show("Please enter a valid Guid for Workflow Instance ID. " + ex.Message, "Workflow Monitor Error")
                Return
            End Try

            If workflowInstanceIdToFind.Equals(Guid.Empty) Then
                MessageBox.Show("Please enter a valid Guid for Workflow Instance ID.", "Workflow Monitor Error")
                Return
            End If
            listViewWorkflows.Items.Clear()
            DisplayWorkflowById(workflowInstanceIdToFind)
            UpdateActivities()
        End Sub

        Private Sub toolStripButtonResetOptions_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles toolStripButtonResetOptions.Click
            ResetWorkflowFilterOptions()
        End Sub

        Private Sub ResetWorkflowFilterOptions()
            'Set selected item in workflow event combo box to All
            Me.toolStripComboBoxWorkflowEvent.SelectedItem = Me.toolStripComboBoxWorkflowEvent.Items(0)
            selectedWorkflowEvent = Me.toolStripComboBoxWorkflowEvent.Text.Trim()

            'Set StatusFrom datetime to 1/1/2000 and StatusUntil to tomorrow
            Me.toolStripTextBoxFrom.Text = New DateTime(2000, 1, 1).ToString()
            Me.toolStripTextBoxUntil.Text = DateTime.Now.AddDays(1).ToString()
            statusFromDateTime = New DateTime(2000, 1, 1)
            statusUntilDateTime = DateTime.Now.AddDays(1)

            'Set filter by tracking data items options to empty strings
            Me.toolStripTextBoxArtifactQualifiedId.Text = String.Empty
            Me.toolStripTextBoxArtifactKeyName.Text = String.Empty
            Me.toolStripTextBoxArtifactKeyValue.Text = String.Empty
            trackingDataItemValue = New TrackingDataItemValue(String.Empty, String.Empty, String.Empty)

            'Set workflow instance ID to empty Guid
            Me.ToolStripTextBoxWorkflowInstanceId.Text = Guid.Empty.ToString()

            'clear workflow items
            listViewWorkflows.Items.Clear()
            'display workflows based on workflow event set earlier to All
            DisplayWorkflows(selectedWorkflowEvent)
            'update activities list
            UpdateActivities()
        End Sub

        Private Sub workflowViewHost_ZoomChanged(ByVal Sender As Object, ByVal e As ZoomChangedEventArgs) Handles workflowViewHost.ZoomChanged
            ' Fires when control is resized via combo box or Fit to Size button.
            ' If the dropdown is already correct, return.
            If (e.Zoom.ToString().Equals(Me.toolStripComboBoxZoom.Text.Trim("%"))) Then
                Return
            End If

            ' Return dropdown to 100 percent
            Me.toolStripComboBoxZoom.SelectedIndex = 2
        End Sub

        Private Sub ListViewActivities_Click(ByVal sender As Object, ByVal e As EventArgs) Handles listViewActivities.Click
            Me.workflowViewHost.HighlightActivity( _
                listViewActivities.SelectedItems(0).SubItems(1).Text)
        End Sub

        Private Sub ShowViewHost(ByVal show As Boolean)
            Me.workflowViewHost.Visible = show
            If displayedWorkflows Is Nothing Then
                Me.workflowViewErrorText.Visible = False
            Else
                Me.workflowViewErrorText.Visible = displayedWorkflows.Count.Equals(0)
            End If
        End Sub
    End Class
End Namespace

