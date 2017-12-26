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

Namespace WorkflowMonitor
    <Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
    Partial Public Class MonitorForm
        Inherits System.Windows.Forms.Form

        'Form overrides dispose to clean up the component list.
        <System.Diagnostics.DebuggerNonUserCode()> _
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
            MyBase.Dispose(disposing)
        End Sub

        'Required by the Windows Form Designer
        Private components As System.ComponentModel.IContainer

        'NOTE: The following procedure is required by the Windows Form Designer
        'It can be modified using the Windows Form Designer.  
        'Do not modify it using the code editor.
        <System.Diagnostics.DebuggerStepThrough()> _
        Private Sub InitializeComponent()
            Me.components = New System.ComponentModel.Container
            Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(MonitorForm))
            Me.workflowViewErrorText = New System.Windows.Forms.Label
            Me.trackingSurface = New System.Windows.Forms.SplitContainer
            Me.workflowDetails = New System.Windows.Forms.SplitContainer
            Me.listViewWorkflows = New System.Windows.Forms.ListView
            Me.workflowsIdHeader = New System.Windows.Forms.ColumnHeader("Id")
            Me.workflowsNameHeader = New System.Windows.Forms.ColumnHeader("Name")
            Me.workflowsStatusHeader = New System.Windows.Forms.ColumnHeader("Status")
            Me.workflowsLabel = New System.Windows.Forms.Label
            Me.listViewActivities = New System.Windows.Forms.ListView
            Me.activitiesIdHeader = New System.Windows.Forms.ColumnHeader("Id")
            Me.activitiesNameHeader = New System.Windows.Forms.ColumnHeader("Name")
            Me.activitiesStatusHeader = New System.Windows.Forms.ColumnHeader("Status")
            Me.activitiesLabel = New System.Windows.Forms.Label
            Me.viewHostLabel = New System.Windows.Forms.Label
            Me.timer = New System.Windows.Forms.Timer(Me.components)
            Me.menuStartStop = New System.Windows.Forms.MenuItem
            Me.menuMonitor = New System.Windows.Forms.MenuItem
            Me.menuSettings = New System.Windows.Forms.MenuItem
            Me.menuWorkflowView = New System.Windows.Forms.MenuItem
            Me.menuActivityDetailsView = New System.Windows.Forms.MenuItem
            Me.menuFile = New System.Windows.Forms.MenuItem
            Me.menuExit = New System.Windows.Forms.MenuItem
            Me.mainMenu = New System.Windows.Forms.MainMenu(Me.components)
            Me.menuView = New System.Windows.Forms.MenuItem
            Me.statusStrip = New System.Windows.Forms.StatusStrip
            Me.statusLabelDatabaseNamePanel = New System.Windows.Forms.ToolStripStatusLabel
            Me.statusLabelMonitoringPanel = New System.Windows.Forms.ToolStripStatusLabel
            Me.menuViewSeparator1 = New System.Windows.Forms.MenuItem
            Me.toolStripMain = New System.Windows.Forms.ToolStrip
            Me.toolStripButtonSettings = New System.Windows.Forms.ToolStripButton
            Me.ToolStripSeparator1 = New System.Windows.Forms.ToolStripSeparator
            Me.toolStripButtonMonitorOn = New System.Windows.Forms.ToolStripButton
            Me.toolStripButtonMonitorOff = New System.Windows.Forms.ToolStripButton
            Me.ToolStripSeparator2 = New System.Windows.Forms.ToolStripSeparator
            Me.toolStripButtonCollapse = New System.Windows.Forms.ToolStripButton
            Me.toolStripButtonExpand = New System.Windows.Forms.ToolStripButton
            Me.toolStripComboBoxZoom = New System.Windows.Forms.ToolStripComboBox
            Me.toolStripWorkflows = New System.Windows.Forms.ToolStrip
            Me.ToolStripLabelWorkflowStatus = New System.Windows.Forms.ToolStripLabel
            Me.toolStripComboBoxWorkflowEvent = New System.Windows.Forms.ToolStripComboBox
            Me.toolStripLabelFrom = New System.Windows.Forms.ToolStripLabel
            Me.toolStripTextBoxFrom = New System.Windows.Forms.ToolStripTextBox
            Me.toolStripLabelUntil = New System.Windows.Forms.ToolStripLabel
            Me.toolStripTextBoxUntil = New System.Windows.Forms.ToolStripTextBox
            Me.ToolStripLabelActivityName = New System.Windows.Forms.ToolStripLabel
            Me.toolStripTextBoxArtifactQualifiedId = New System.Windows.Forms.ToolStripTextBox
            Me.toolStripLabelProperty = New System.Windows.Forms.ToolStripLabel
            Me.toolStripTextBoxArtifactKeyName = New System.Windows.Forms.ToolStripTextBox
            Me.toolStripLabelArtifactKeyValue = New System.Windows.Forms.ToolStripLabel
            Me.toolStripTextBoxArtifactKeyValue = New System.Windows.Forms.ToolStripTextBox
            Me.toolStripButtonFindWorkflows = New System.Windows.Forms.ToolStripButton
            Me.ToolStripLabelWorkflowInstanceId = New System.Windows.Forms.ToolStripLabel
            Me.ToolStripTextBoxWorkflowInstanceId = New System.Windows.Forms.ToolStripTextBox
            Me.ToolStripButtonFindWorkflowById = New System.Windows.Forms.ToolStripButton
            Me.toolStripButtonResetOptions = New System.Windows.Forms.ToolStripButton
            Me.trackingSurface.Panel1.SuspendLayout()
            Me.trackingSurface.Panel2.SuspendLayout()
            Me.trackingSurface.SuspendLayout()
            Me.workflowDetails.Panel1.SuspendLayout()
            Me.workflowDetails.Panel2.SuspendLayout()
            Me.workflowDetails.SuspendLayout()
            Me.statusStrip.SuspendLayout()
            Me.toolStripMain.SuspendLayout()
            Me.toolStripWorkflows.SuspendLayout()
            Me.SuspendLayout()
            '
            'workflowViewErrorText
            '
            Me.workflowViewErrorText.BackColor = System.Drawing.SystemColors.GradientInactiveCaption
            Me.workflowViewErrorText.Dock = System.Windows.Forms.DockStyle.Fill
            Me.workflowViewErrorText.Location = New System.Drawing.Point(0, 15)
            Me.workflowViewErrorText.Name = "workflowViewErrorText"
            Me.workflowViewErrorText.Size = New System.Drawing.Size(640, 616)
            Me.workflowViewErrorText.TabIndex = 0
            Me.workflowViewErrorText.Text = resources.GetString("workflowViewErrorText.Text")
            Me.workflowViewErrorText.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
            '
            'trackingSurface
            '
            Me.trackingSurface.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
            Me.trackingSurface.Dock = System.Windows.Forms.DockStyle.Fill
            Me.trackingSurface.Location = New System.Drawing.Point(0, 50)
            Me.trackingSurface.Name = "trackingSurface"
            '
            'trackingSurface.Panel1
            '
            Me.trackingSurface.Panel1.Controls.Add(Me.workflowDetails)
            '
            'trackingSurface.Panel2
            '
            Me.trackingSurface.Panel2.Controls.Add(Me.workflowViewErrorText)
            Me.trackingSurface.Panel2.Controls.Add(Me.viewHostLabel)
            Me.trackingSurface.Size = New System.Drawing.Size(1016, 635)
            Me.trackingSurface.SplitterDistance = 368
            Me.trackingSurface.TabIndex = 3
            '
            'workflowDetails
            '
            Me.workflowDetails.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
            Me.workflowDetails.Dock = System.Windows.Forms.DockStyle.Fill
            Me.workflowDetails.Location = New System.Drawing.Point(0, 0)
            Me.workflowDetails.Name = "workflowDetails"
            Me.workflowDetails.Orientation = System.Windows.Forms.Orientation.Horizontal
            '
            'workflowDetails.Panel1
            '
            Me.workflowDetails.Panel1.Controls.Add(Me.listViewWorkflows)
            Me.workflowDetails.Panel1.Controls.Add(Me.workflowsLabel)
            '
            'workflowDetails.Panel2
            '
            Me.workflowDetails.Panel2.Controls.Add(Me.listViewActivities)
            Me.workflowDetails.Panel2.Controls.Add(Me.activitiesLabel)
            Me.workflowDetails.Size = New System.Drawing.Size(368, 635)
            Me.workflowDetails.SplitterDistance = 316
            Me.workflowDetails.TabIndex = 0
            '
            'listViewWorkflows
            '
            Me.listViewWorkflows.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.workflowsIdHeader, Me.workflowsNameHeader, Me.workflowsStatusHeader})
            Me.listViewWorkflows.Dock = System.Windows.Forms.DockStyle.Fill
            Me.listViewWorkflows.FullRowSelect = True
            Me.listViewWorkflows.HideSelection = False
            Me.listViewWorkflows.LabelWrap = False
            Me.listViewWorkflows.Location = New System.Drawing.Point(0, 15)
            Me.listViewWorkflows.Margin = New System.Windows.Forms.Padding(2, 3, 3, 3)
            Me.listViewWorkflows.MultiSelect = False
            Me.listViewWorkflows.Name = "listViewWorkflows"
            Me.listViewWorkflows.ShowGroups = False
            Me.listViewWorkflows.Size = New System.Drawing.Size(364, 297)
            Me.listViewWorkflows.TabIndex = 0
            Me.listViewWorkflows.UseCompatibleStateImageBehavior = False
            Me.listViewWorkflows.View = System.Windows.Forms.View.Details
            '
            'workflowsIdHeader
            '
            Me.workflowsIdHeader.Text = "Id"
            Me.workflowsIdHeader.Width = 35
            '
            'workflowsNameHeader
            '
            Me.workflowsNameHeader.Text = "Name"
            Me.workflowsNameHeader.Width = 150
            '
            'workflowsStatusHeader
            '
            Me.workflowsStatusHeader.Text = "Status"
            Me.workflowsStatusHeader.Width = 100
            '
            'workflowsLabel
            '
            Me.workflowsLabel.BackColor = System.Drawing.SystemColors.GradientInactiveCaption
            Me.workflowsLabel.Dock = System.Windows.Forms.DockStyle.Top
            Me.workflowsLabel.Location = New System.Drawing.Point(0, 0)
            Me.workflowsLabel.Name = "workflowsLabel"
            Me.workflowsLabel.Size = New System.Drawing.Size(364, 15)
            Me.workflowsLabel.TabIndex = 1
            Me.workflowsLabel.Text = " Workflows"
            '
            'listViewActivities
            '
            Me.listViewActivities.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.activitiesIdHeader, Me.activitiesNameHeader, Me.activitiesStatusHeader})
            Me.listViewActivities.Dock = System.Windows.Forms.DockStyle.Fill
            Me.listViewActivities.LabelWrap = False
            Me.listViewActivities.Location = New System.Drawing.Point(0, 15)
            Me.listViewActivities.Margin = New System.Windows.Forms.Padding(2, 3, 3, 3)
            Me.listViewActivities.MultiSelect = False
            Me.listViewActivities.Name = "listViewActivities"
            Me.listViewActivities.ShowGroups = False
            Me.listViewActivities.Size = New System.Drawing.Size(364, 296)
            Me.listViewActivities.TabIndex = 0
            Me.listViewActivities.UseCompatibleStateImageBehavior = False
            Me.listViewActivities.View = System.Windows.Forms.View.Details
            '
            'activitiesIdHeader
            '
            Me.activitiesIdHeader.Text = "Id"
            Me.activitiesIdHeader.Width = 35
            '
            'activitiesNameHeader
            '
            Me.activitiesNameHeader.Text = "Name"
            Me.activitiesNameHeader.Width = 150
            '
            'activitiesStatusHeader
            '
            Me.activitiesStatusHeader.Text = "Status"
            Me.activitiesStatusHeader.Width = 100
            '
            'activitiesLabel
            '
            Me.activitiesLabel.BackColor = System.Drawing.SystemColors.GradientInactiveCaption
            Me.activitiesLabel.Dock = System.Windows.Forms.DockStyle.Top
            Me.activitiesLabel.Location = New System.Drawing.Point(0, 0)
            Me.activitiesLabel.Name = "activitiesLabel"
            Me.activitiesLabel.Size = New System.Drawing.Size(364, 15)
            Me.activitiesLabel.TabIndex = 1
            Me.activitiesLabel.Text = "Activities"
            '
            'viewHostLabel
            '
            Me.viewHostLabel.BackColor = System.Drawing.SystemColors.GradientInactiveCaption
            Me.viewHostLabel.Dock = System.Windows.Forms.DockStyle.Top
            Me.viewHostLabel.Location = New System.Drawing.Point(0, 0)
            Me.viewHostLabel.Name = "viewHostLabel"
            Me.viewHostLabel.Size = New System.Drawing.Size(640, 15)
            Me.viewHostLabel.TabIndex = 1
            Me.viewHostLabel.Text = "Workflow View"
            '
            'timer
            '
            Me.timer.Interval = 5000
            '
            'menuStartStop
            '
            Me.menuStartStop.Index = 0
            Me.menuStartStop.Text = "Start"
            '
            'menuMonitor
            '
            Me.menuMonitor.Index = 2
            Me.menuMonitor.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.menuStartStop, Me.menuSettings})
            Me.menuMonitor.Text = "&Monitor"
            '
            'menuSettings
            '
            Me.menuSettings.Index = 1
            Me.menuSettings.Text = "Settings"
            '
            'menuWorkflowView
            '
            Me.menuWorkflowView.Checked = True
            Me.menuWorkflowView.Index = -1
            Me.menuWorkflowView.Text = "&Workflow"
            '
            'menuActivityDetailsView
            '
            Me.menuActivityDetailsView.Checked = True
            Me.menuActivityDetailsView.Index = 0
            Me.menuActivityDetailsView.Text = "&Workflow Details"
            '
            'menuFile
            '
            Me.menuFile.Index = 0
            Me.menuFile.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.menuExit})
            Me.menuFile.Text = "&File"
            '
            'menuExit
            '
            Me.menuExit.Index = 0
            Me.menuExit.Text = "E&xit"
            '
            'mainMenu
            '
            Me.mainMenu.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.menuFile, Me.menuView, Me.menuMonitor})
            '
            'menuView
            '
            Me.menuView.Index = 1
            Me.menuView.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.menuActivityDetailsView})
            Me.menuView.Text = "&View"
            '
            'statusStrip
            '
            Me.statusStrip.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.statusLabelDatabaseNamePanel, Me.statusLabelMonitoringPanel})
            Me.statusStrip.Location = New System.Drawing.Point(0, 685)
            Me.statusStrip.Name = "statusStrip"
            Me.statusStrip.Size = New System.Drawing.Size(1016, 28)
            Me.statusStrip.TabIndex = 4
            '
            'statusLabelDatabaseNamePanel
            '
            Me.statusLabelDatabaseNamePanel.AutoSize = False
            Me.statusLabelDatabaseNamePanel.Name = "statusLabelDatabaseNamePanel"
            Me.statusLabelDatabaseNamePanel.Size = New System.Drawing.Size(250, 23)
            Me.statusLabelDatabaseNamePanel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
            '
            'statusLabelMonitoringPanel
            '
            Me.statusLabelMonitoringPanel.AutoSize = False
            Me.statusLabelMonitoringPanel.Name = "statusLabelMonitoringPanel"
            Me.statusLabelMonitoringPanel.Size = New System.Drawing.Size(65, 23)
            Me.statusLabelMonitoringPanel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
            '
            'menuViewSeparator1
            '
            Me.menuViewSeparator1.Index = -1
            Me.menuViewSeparator1.Text = "-"
            '
            'toolStripMain
            '
            Me.toolStripMain.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.toolStripButtonSettings, Me.ToolStripSeparator1, Me.toolStripButtonMonitorOn, Me.toolStripButtonMonitorOff, Me.ToolStripSeparator2, Me.toolStripButtonCollapse, Me.toolStripButtonExpand, Me.toolStripComboBoxZoom})
            Me.toolStripMain.Location = New System.Drawing.Point(0, 0)
            Me.toolStripMain.Name = "toolStripMain"
            Me.toolStripMain.Size = New System.Drawing.Size(1016, 25)
            Me.toolStripMain.TabIndex = 5
            Me.toolStripMain.Text = "ToolStrip1"
            '
            'toolStripButtonSettings
            '
            Me.toolStripButtonSettings.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
            Me.toolStripButtonSettings.Image = CType(resources.GetObject("toolStripButtonSettings.Image"), System.Drawing.Image)
            Me.toolStripButtonSettings.ImageTransparentColor = System.Drawing.Color.Cyan
            Me.toolStripButtonSettings.Name = "toolStripButtonSettings"
            Me.toolStripButtonSettings.Size = New System.Drawing.Size(23, 22)
            Me.toolStripButtonSettings.Text = "Settings"
            '
            'ToolStripSeparator1
            '
            Me.ToolStripSeparator1.Name = "ToolStripSeparator1"
            Me.ToolStripSeparator1.Size = New System.Drawing.Size(6, 25)
            '
            'toolStripButtonMonitorOn
            '
            Me.toolStripButtonMonitorOn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
            Me.toolStripButtonMonitorOn.Image = CType(resources.GetObject("toolStripButtonMonitorOn.Image"), System.Drawing.Image)
            Me.toolStripButtonMonitorOn.ImageTransparentColor = System.Drawing.Color.Cyan
            Me.toolStripButtonMonitorOn.Name = "toolStripButtonMonitorOn"
            Me.toolStripButtonMonitorOn.Size = New System.Drawing.Size(23, 22)
            Me.toolStripButtonMonitorOn.Text = "ToolStripButton1"
            Me.toolStripButtonMonitorOn.ToolTipText = "Monitor On"
            '
            'toolStripButtonMonitorOff
            '
            Me.toolStripButtonMonitorOff.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
            Me.toolStripButtonMonitorOff.Enabled = False
            Me.toolStripButtonMonitorOff.Image = CType(resources.GetObject("toolStripButtonMonitorOff.Image"), System.Drawing.Image)
            Me.toolStripButtonMonitorOff.ImageTransparentColor = System.Drawing.Color.Cyan
            Me.toolStripButtonMonitorOff.Name = "toolStripButtonMonitorOff"
            Me.toolStripButtonMonitorOff.Size = New System.Drawing.Size(23, 22)
            Me.toolStripButtonMonitorOff.Text = "ToolStripButton1"
            Me.toolStripButtonMonitorOff.ToolTipText = "Monitor Off"
            '
            'ToolStripSeparator2
            '
            Me.ToolStripSeparator2.Name = "ToolStripSeparator2"
            Me.ToolStripSeparator2.Size = New System.Drawing.Size(6, 25)
            '
            'toolStripButtonCollapse
            '
            Me.toolStripButtonCollapse.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
            Me.toolStripButtonCollapse.Enabled = False
            Me.toolStripButtonCollapse.Image = CType(resources.GetObject("toolStripButtonCollapse.Image"), System.Drawing.Image)
            Me.toolStripButtonCollapse.ImageTransparentColor = System.Drawing.Color.Cyan
            Me.toolStripButtonCollapse.Name = "toolStripButtonCollapse"
            Me.toolStripButtonCollapse.Size = New System.Drawing.Size(23, 22)
            Me.toolStripButtonCollapse.Text = "ToolStripButton1"
            Me.toolStripButtonCollapse.ToolTipText = "Collapse All"
            '
            'toolStripButtonExpand
            '
            Me.toolStripButtonExpand.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
            Me.toolStripButtonExpand.Enabled = False
            Me.toolStripButtonExpand.Image = CType(resources.GetObject("toolStripButtonExpand.Image"), System.Drawing.Image)
            Me.toolStripButtonExpand.ImageTransparentColor = System.Drawing.Color.Cyan
            Me.toolStripButtonExpand.Name = "toolStripButtonExpand"
            Me.toolStripButtonExpand.Size = New System.Drawing.Size(23, 22)
            Me.toolStripButtonExpand.Text = "ToolStripButton1"
            Me.toolStripButtonExpand.ToolTipText = "Expand All"
            '
            'toolStripComboBoxZoom
            '
            Me.toolStripComboBoxZoom.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
            Me.toolStripComboBoxZoom.Enabled = False
            Me.toolStripComboBoxZoom.Items.AddRange(New Object() {"50%", "75%", "100%", "150%", "200%", "300%", "400%"})
            Me.toolStripComboBoxZoom.Name = "toolStripComboBoxZoom"
            Me.toolStripComboBoxZoom.Size = New System.Drawing.Size(75, 25)
            Me.toolStripComboBoxZoom.ToolTipText = "Workflow Zoom Level"
            '
            'toolStripWorkflows
            '
            Me.toolStripWorkflows.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ToolStripLabelWorkflowStatus, Me.toolStripComboBoxWorkflowEvent, Me.toolStripLabelFrom, Me.toolStripTextBoxFrom, Me.toolStripLabelUntil, Me.toolStripTextBoxUntil, Me.ToolStripLabelActivityName, Me.toolStripTextBoxArtifactQualifiedId, Me.toolStripLabelProperty, Me.toolStripTextBoxArtifactKeyName, Me.toolStripLabelArtifactKeyValue, Me.toolStripTextBoxArtifactKeyValue, Me.toolStripButtonFindWorkflows, Me.ToolStripLabelWorkflowInstanceId, Me.ToolStripTextBoxWorkflowInstanceId, Me.ToolStripButtonFindWorkflowById, Me.toolStripButtonResetOptions})
            Me.toolStripWorkflows.Location = New System.Drawing.Point(0, 25)
            Me.toolStripWorkflows.Name = "toolStripWorkflows"
            Me.toolStripWorkflows.Size = New System.Drawing.Size(1016, 25)
            Me.toolStripWorkflows.TabIndex = 6
            Me.toolStripWorkflows.Text = "ToolStrip1"
            '
            'ToolStripLabelWorkflowStatus
            '
            Me.ToolStripLabelWorkflowStatus.Name = "ToolStripLabelWorkflowStatus"
            Me.ToolStripLabelWorkflowStatus.Size = New System.Drawing.Size(86, 22)
            Me.ToolStripLabelWorkflowStatus.Text = "Workflow Status"
            '
            'toolStripComboBoxWorkflowEvent
            '
            Me.toolStripComboBoxWorkflowEvent.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
            Me.toolStripComboBoxWorkflowEvent.Items.AddRange(New Object() {"All", "Created", "Completed", "Running", "Suspended", "Terminated"})
            Me.toolStripComboBoxWorkflowEvent.Name = "toolStripComboBoxWorkflowEvent"
            Me.toolStripComboBoxWorkflowEvent.Size = New System.Drawing.Size(75, 25)
            Me.toolStripComboBoxWorkflowEvent.ToolTipText = "Workflow Event"
            '
            'toolStripLabelFrom
            '
            Me.toolStripLabelFrom.Name = "toolStripLabelFrom"
            Me.toolStripLabelFrom.Size = New System.Drawing.Size(31, 22)
            Me.toolStripLabelFrom.Text = "From"
            Me.toolStripLabelFrom.ToolTipText = "From"
            '
            'toolStripTextBoxFrom
            '
            Me.toolStripTextBoxFrom.Name = "toolStripTextBoxFrom"
            Me.toolStripTextBoxFrom.Size = New System.Drawing.Size(60, 25)
            '
            'toolStripLabelUntil
            '
            Me.toolStripLabelUntil.Name = "toolStripLabelUntil"
            Me.toolStripLabelUntil.Size = New System.Drawing.Size(28, 22)
            Me.toolStripLabelUntil.Text = "Until"
            '
            'toolStripTextBoxUntil
            '
            Me.toolStripTextBoxUntil.Name = "toolStripTextBoxUntil"
            Me.toolStripTextBoxUntil.Size = New System.Drawing.Size(60, 25)
            '
            'ToolStripLabelActivityName
            '
            Me.ToolStripLabelActivityName.Name = "ToolStripLabelActivityName"
            Me.ToolStripLabelActivityName.Size = New System.Drawing.Size(73, 22)
            Me.ToolStripLabelActivityName.Text = "Activity Name"
            Me.ToolStripLabelActivityName.ToolTipText = "Activity Name"
            '
            'toolStripTextBoxArtifactQualifiedId
            '
            Me.toolStripTextBoxArtifactQualifiedId.Name = "toolStripTextBoxArtifactQualifiedId"
            Me.toolStripTextBoxArtifactQualifiedId.Size = New System.Drawing.Size(60, 25)
            '
            'toolStripLabelProperty
            '
            Me.toolStripLabelProperty.Name = "toolStripLabelProperty"
            Me.toolStripLabelProperty.Size = New System.Drawing.Size(49, 22)
            Me.toolStripLabelProperty.Text = "Property"
            Me.toolStripLabelProperty.ToolTipText = "Property"
            '
            'toolStripTextBoxArtifactKeyName
            '
            Me.toolStripTextBoxArtifactKeyName.Name = "toolStripTextBoxArtifactKeyName"
            Me.toolStripTextBoxArtifactKeyName.Size = New System.Drawing.Size(60, 25)
            '
            'toolStripLabelArtifactKeyValue
            '
            Me.toolStripLabelArtifactKeyValue.Name = "toolStripLabelArtifactKeyValue"
            Me.toolStripLabelArtifactKeyValue.Size = New System.Drawing.Size(33, 22)
            Me.toolStripLabelArtifactKeyValue.Text = "Value"
            '
            'toolStripTextBoxArtifactKeyValue
            '
            Me.toolStripTextBoxArtifactKeyValue.Name = "toolStripTextBoxArtifactKeyValue"
            Me.toolStripTextBoxArtifactKeyValue.Size = New System.Drawing.Size(60, 25)
            '
            'toolStripButtonFindWorkflows
            '
            Me.toolStripButtonFindWorkflows.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
            Me.toolStripButtonFindWorkflows.Image = CType(resources.GetObject("toolStripButtonFindWorkflows.Image"), System.Drawing.Image)
            Me.toolStripButtonFindWorkflows.ImageTransparentColor = System.Drawing.Color.Cyan
            Me.toolStripButtonFindWorkflows.Name = "toolStripButtonFindWorkflows"
            Me.toolStripButtonFindWorkflows.Size = New System.Drawing.Size(23, 22)
            Me.toolStripButtonFindWorkflows.Text = "ToolStripButton1"
            Me.toolStripButtonFindWorkflows.ToolTipText = "Find Workflows"
            '
            'ToolStripLabelWorkflowInstanceId
            '
            Me.ToolStripLabelWorkflowInstanceId.Name = "ToolStripLabelWorkflowInstanceId"
            Me.ToolStripLabelWorkflowInstanceId.Size = New System.Drawing.Size(111, 22)
            Me.ToolStripLabelWorkflowInstanceId.Text = "Workflow Instance ID"
            '
            'ToolStripTextBoxWorkflowInstanceId
            '
            Me.ToolStripTextBoxWorkflowInstanceId.Name = "ToolStripTextBoxWorkflowInstanceId"
            Me.ToolStripTextBoxWorkflowInstanceId.Size = New System.Drawing.Size(130, 25)
            '
            'ToolStripButtonFindWorkflowById
            '
            Me.ToolStripButtonFindWorkflowById.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
            Me.ToolStripButtonFindWorkflowById.Image = CType(resources.GetObject("ToolStripButtonFindWorkflowById.Image"), System.Drawing.Image)
            Me.ToolStripButtonFindWorkflowById.ImageTransparentColor = System.Drawing.Color.Cyan
            Me.ToolStripButtonFindWorkflowById.Name = "ToolStripButtonFindWorkflowById"
            Me.ToolStripButtonFindWorkflowById.Size = New System.Drawing.Size(23, 22)
            Me.ToolStripButtonFindWorkflowById.Text = "ToolStripButton1"
            Me.ToolStripButtonFindWorkflowById.ToolTipText = "Find Workflows"
            '
            'toolStripButtonResetOptions
            '
            Me.toolStripButtonResetOptions.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
            Me.toolStripButtonResetOptions.Image = CType(resources.GetObject("toolStripButtonResetOptions.Image"), System.Drawing.Image)
            Me.toolStripButtonResetOptions.ImageTransparentColor = System.Drawing.Color.Cyan
            Me.toolStripButtonResetOptions.Name = "toolStripButtonResetOptions"
            Me.toolStripButtonResetOptions.Size = New System.Drawing.Size(23, 22)
            Me.toolStripButtonResetOptions.Text = "ToolStripButton1"
            Me.toolStripButtonResetOptions.ToolTipText = "Reset Filter Workflow Options"
            '
            'MonitorForm
            '
            Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.ClientSize = New System.Drawing.Size(1016, 713)
            Me.Controls.Add(Me.trackingSurface)
            Me.Controls.Add(Me.toolStripWorkflows)
            Me.Controls.Add(Me.toolStripMain)
            Me.Controls.Add(Me.statusStrip)
            Me.Menu = Me.mainMenu
            Me.Name = "MonitorForm"
            Me.Text = "Workflow Monitor"
            Me.trackingSurface.Panel1.ResumeLayout(False)
            Me.trackingSurface.Panel2.ResumeLayout(False)
            Me.trackingSurface.ResumeLayout(False)
            Me.workflowDetails.Panel1.ResumeLayout(False)
            Me.workflowDetails.Panel2.ResumeLayout(False)
            Me.workflowDetails.ResumeLayout(False)
            Me.statusStrip.ResumeLayout(False)
            Me.statusStrip.PerformLayout()
            Me.toolStripMain.ResumeLayout(False)
            Me.toolStripMain.PerformLayout()
            Me.toolStripWorkflows.ResumeLayout(False)
            Me.toolStripWorkflows.PerformLayout()
            Me.ResumeLayout(False)
            Me.PerformLayout()

        End Sub
        Friend WithEvents workflowViewErrorText As System.Windows.Forms.Label
        Friend WithEvents trackingSurface As System.Windows.Forms.SplitContainer
        Friend WithEvents workflowDetails As System.Windows.Forms.SplitContainer
        Friend WithEvents workflowsLabel As System.Windows.Forms.Label
        Friend WithEvents listViewWorkflows As System.Windows.Forms.ListView
        Friend WithEvents workflowsIdHeader As System.Windows.Forms.ColumnHeader
        Friend WithEvents workflowsNameHeader As System.Windows.Forms.ColumnHeader
        Friend WithEvents workflowsStatusHeader As System.Windows.Forms.ColumnHeader
        Friend WithEvents listViewActivities As System.Windows.Forms.ListView
        Friend WithEvents activitiesIdHeader As System.Windows.Forms.ColumnHeader
        Friend WithEvents activitiesNameHeader As System.Windows.Forms.ColumnHeader
        Friend WithEvents activitiesStatusHeader As System.Windows.Forms.ColumnHeader
        Friend WithEvents activitiesLabel As System.Windows.Forms.Label
        Friend WithEvents viewHostLabel As System.Windows.Forms.Label
        Friend WithEvents timer As System.Windows.Forms.Timer
        Friend WithEvents menuStartStop As System.Windows.Forms.MenuItem
        Friend WithEvents menuMonitor As System.Windows.Forms.MenuItem
        Friend WithEvents menuSettings As System.Windows.Forms.MenuItem
        Friend WithEvents menuWorkflowView As System.Windows.Forms.MenuItem
        Friend WithEvents menuActivityDetailsView As System.Windows.Forms.MenuItem
        Friend WithEvents menuFile As System.Windows.Forms.MenuItem
        Friend WithEvents menuExit As System.Windows.Forms.MenuItem
        Friend WithEvents mainMenu As System.Windows.Forms.MainMenu
        Friend WithEvents menuView As System.Windows.Forms.MenuItem
        Friend WithEvents statusStrip As System.Windows.Forms.StatusStrip
        Friend WithEvents statusLabelDatabaseNamePanel As System.Windows.Forms.ToolStripStatusLabel
        Friend WithEvents statusLabelMonitoringPanel As System.Windows.Forms.ToolStripStatusLabel
        Friend WithEvents menuViewSeparator1 As System.Windows.Forms.MenuItem
        Friend WithEvents toolStripMain As System.Windows.Forms.ToolStrip
        Friend WithEvents toolStripButtonSettings As System.Windows.Forms.ToolStripButton
        Friend WithEvents ToolStripSeparator1 As System.Windows.Forms.ToolStripSeparator
        Friend WithEvents toolStripButtonMonitorOn As System.Windows.Forms.ToolStripButton
        Friend WithEvents toolStripButtonMonitorOff As System.Windows.Forms.ToolStripButton
        Friend WithEvents ToolStripSeparator2 As System.Windows.Forms.ToolStripSeparator
        Friend WithEvents toolStripButtonCollapse As System.Windows.Forms.ToolStripButton
        Friend WithEvents toolStripButtonExpand As System.Windows.Forms.ToolStripButton
        Friend WithEvents toolStripComboBoxZoom As System.Windows.Forms.ToolStripComboBox
        Friend WithEvents toolStripWorkflows As System.Windows.Forms.ToolStrip
        Friend WithEvents ToolStripLabelWorkflowStatus As System.Windows.Forms.ToolStripLabel
        Friend WithEvents toolStripComboBoxWorkflowEvent As System.Windows.Forms.ToolStripComboBox
        Friend WithEvents toolStripLabelFrom As System.Windows.Forms.ToolStripLabel
        Friend WithEvents toolStripTextBoxFrom As System.Windows.Forms.ToolStripTextBox
        Friend WithEvents toolStripLabelUntil As System.Windows.Forms.ToolStripLabel
        Friend WithEvents toolStripTextBoxUntil As System.Windows.Forms.ToolStripTextBox
        Friend WithEvents ToolStripLabelActivityName As System.Windows.Forms.ToolStripLabel
        Friend WithEvents toolStripTextBoxArtifactQualifiedId As System.Windows.Forms.ToolStripTextBox
        Friend WithEvents toolStripLabelProperty As System.Windows.Forms.ToolStripLabel
        Friend WithEvents toolStripTextBoxArtifactKeyName As System.Windows.Forms.ToolStripTextBox
        Friend WithEvents toolStripLabelArtifactKeyValue As System.Windows.Forms.ToolStripLabel
        Friend WithEvents toolStripButtonFindWorkflows As System.Windows.Forms.ToolStripButton
        Friend WithEvents toolStripTextBoxArtifactKeyValue As System.Windows.Forms.ToolStripTextBox
        Friend WithEvents ToolStripLabelWorkflowInstanceId As System.Windows.Forms.ToolStripLabel
        Friend WithEvents ToolStripTextBoxWorkflowInstanceId As System.Windows.Forms.ToolStripTextBox
        Friend WithEvents ToolStripButtonFindWorkflowById As System.Windows.Forms.ToolStripButton
        Friend WithEvents toolStripButtonResetOptions As System.Windows.Forms.ToolStripButton

    End Class
End Namespace

