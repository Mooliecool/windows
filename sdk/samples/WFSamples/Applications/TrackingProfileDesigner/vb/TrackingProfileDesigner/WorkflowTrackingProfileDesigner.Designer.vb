'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
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

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class WorkflowTrackingProfileDesignerForm
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(WorkflowTrackingProfileDesignerForm))
        Me.menuStrip1 = New System.Windows.Forms.MenuStrip
        Me.FileToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.openToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.fromFileToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.profileToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.fromSqlToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.saveToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.profileAsFileToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.toSqlTrackingStoreToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.exitToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.toolStrip1 = New System.Windows.Forms.ToolStrip
        Me.trackButton = New System.Windows.Forms.ToolStripButton
        Me.ToolStripSeparator1 = New System.Windows.Forms.ToolStripSeparator
        Me.eventsDropDown = New System.Windows.Forms.ToolStripDropDownButton
        Me.workflowEventsDropDown = New System.Windows.Forms.ToolStripDropDownButton
        Me.eventsSeparator = New System.Windows.Forms.ToolStripSeparator
        Me.annotateButton = New System.Windows.Forms.ToolStripButton
        Me.extractDropDown = New System.Windows.Forms.ToolStripDropDownButton
        Me.activityMembersToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.workflowMembersToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.conditionsDropDown = New System.Windows.Forms.ToolStripDropDownButton
        Me.addConditionButton = New System.Windows.Forms.ToolStripMenuItem
        Me.matchDerivedTypes = New System.Windows.Forms.ToolStripButton
        Me.annotationToolStrip = New System.Windows.Forms.ToolStrip
        Me.annotationText = New System.Windows.Forms.ToolStripTextBox
        Me.annotateSaveButton = New System.Windows.Forms.ToolStripButton
        Me.annotateCancelButton = New System.Windows.Forms.ToolStripButton
        Me.conditionToolStrip = New System.Windows.Forms.ToolStrip
        Me.conditionMemberDropDown = New System.Windows.Forms.ToolStripDropDownButton
        Me.conditionEqualityButton = New System.Windows.Forms.ToolStripButton
        Me.conditionValue = New System.Windows.Forms.ToolStripTextBox
        Me.conditionSaveButton = New System.Windows.Forms.ToolStripButton
        Me.conditionCancelButton = New System.Windows.Forms.ToolStripButton
        Me.conditionDeleteButton = New System.Windows.Forms.ToolStripButton
        Me.saveFileDialog1 = New System.Windows.Forms.SaveFileDialog
        Me.openProfileDialog = New System.Windows.Forms.OpenFileDialog
        Me.openWorkflowDialog = New System.Windows.Forms.OpenFileDialog
        Me.openAssemblyDialog = New System.Windows.Forms.OpenFileDialog
        Me.panel1 = New System.Windows.Forms.Panel
        Me.tabControl1 = New System.Windows.Forms.TabControl
        Me.TabPage1 = New System.Windows.Forms.TabPage
        Me.WorkflowDesignerControl1 = New WorkflowDesignerControl
        Me.TabPage2 = New System.Windows.Forms.TabPage
        Me.profileMarkup = New System.Windows.Forms.RichTextBox
        Me.menuStrip1.SuspendLayout()
        Me.toolStrip1.SuspendLayout()
        Me.annotationToolStrip.SuspendLayout()
        Me.conditionToolStrip.SuspendLayout()
        Me.panel1.SuspendLayout()
        Me.tabControl1.SuspendLayout()
        Me.TabPage1.SuspendLayout()
        Me.TabPage2.SuspendLayout()
        Me.SuspendLayout()
        '
        'menuStrip1
        '
        Me.menuStrip1.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.FileToolStripMenuItem})
        Me.menuStrip1.Location = New System.Drawing.Point(0, 0)
        Me.menuStrip1.Name = "menuStrip1"
        Me.menuStrip1.Size = New System.Drawing.Size(734, 24)
        Me.menuStrip1.TabIndex = 0
        Me.menuStrip1.Text = "MenuStrip1"
        '
        'FileToolStripMenuItem
        '
        Me.FileToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.openToolStripMenuItem, Me.saveToolStripMenuItem, Me.exitToolStripMenuItem})
        Me.FileToolStripMenuItem.Name = "FileToolStripMenuItem"
        Me.FileToolStripMenuItem.Size = New System.Drawing.Size(35, 20)
        Me.FileToolStripMenuItem.Text = "&File"
        '
        'openToolStripMenuItem
        '
        Me.openToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.fromFileToolStripMenuItem, Me.profileToolStripMenuItem, Me.fromSqlToolStripMenuItem})
        Me.openToolStripMenuItem.Name = "openToolStripMenuItem"
        Me.openToolStripMenuItem.Size = New System.Drawing.Size(100, 22)
        Me.openToolStripMenuItem.Text = "&Open"
        '
        'fromFileToolStripMenuItem
        '
        Me.fromFileToolStripMenuItem.Name = "fromFileToolStripMenuItem"
        Me.fromFileToolStripMenuItem.Size = New System.Drawing.Size(212, 22)
        Me.fromFileToolStripMenuItem.Text = "&Workflow From File"
        '
        'profileToolStripMenuItem
        '
        Me.profileToolStripMenuItem.Name = "profileToolStripMenuItem"
        Me.profileToolStripMenuItem.Size = New System.Drawing.Size(212, 22)
        Me.profileToolStripMenuItem.Text = "&Profile From File"
        '
        'fromSqlToolStripMenuItem
        '
        Me.fromSqlToolStripMenuItem.Name = "fromSqlToolStripMenuItem"
        Me.fromSqlToolStripMenuItem.Size = New System.Drawing.Size(212, 22)
        Me.fromSqlToolStripMenuItem.Text = "From &SQL Tracking Database"
        '
        'saveToolStripMenuItem
        '
        Me.saveToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.profileAsFileToolStripMenuItem, Me.toSqlTrackingStoreToolStripMenuItem})
        Me.saveToolStripMenuItem.Name = "saveToolStripMenuItem"
        Me.saveToolStripMenuItem.Size = New System.Drawing.Size(100, 22)
        Me.saveToolStripMenuItem.Text = "&Save"
        '
        'profileAsFileToolStripMenuItem
        '
        Me.profileAsFileToolStripMenuItem.Name = "profileAsFileToolStripMenuItem"
        Me.profileAsFileToolStripMenuItem.Size = New System.Drawing.Size(231, 22)
        Me.profileAsFileToolStripMenuItem.Text = "Profile as &File"
        '
        'toSqlTrackingStoreToolStripMenuItem
        '
        Me.toSqlTrackingStoreToolStripMenuItem.Name = "toSqlTrackingStoreToolStripMenuItem"
        Me.toSqlTrackingStoreToolStripMenuItem.Size = New System.Drawing.Size(231, 22)
        Me.toSqlTrackingStoreToolStripMenuItem.Text = "Profile to &SQL Tracking Database"
        '
        'exitToolStripMenuItem
        '
        Me.exitToolStripMenuItem.Name = "exitToolStripMenuItem"
        Me.exitToolStripMenuItem.Size = New System.Drawing.Size(100, 22)
        Me.exitToolStripMenuItem.Text = "E&xit"
        '
        'toolStrip1
        '
        Me.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden
        Me.toolStrip1.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.trackButton, Me.ToolStripSeparator1, Me.eventsDropDown, Me.workflowEventsDropDown, Me.eventsSeparator, Me.annotateButton, Me.extractDropDown, Me.conditionsDropDown, Me.matchDerivedTypes})
        Me.toolStrip1.Location = New System.Drawing.Point(0, 0)
        Me.toolStrip1.Name = "toolStrip1"
        Me.toolStrip1.Size = New System.Drawing.Size(734, 25)
        Me.toolStrip1.TabIndex = 3
        Me.toolStrip1.Text = "ToolStrip1"
        '
        'trackButton
        '
        Me.trackButton.Image = CType(resources.GetObject("trackButton.Image"), System.Drawing.Image)
        Me.trackButton.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.trackButton.Name = "trackButton"
        Me.trackButton.Size = New System.Drawing.Size(53, 22)
        Me.trackButton.Text = "Track"
        '
        'ToolStripSeparator1
        '
        Me.ToolStripSeparator1.Name = "ToolStripSeparator1"
        Me.ToolStripSeparator1.Size = New System.Drawing.Size(6, 25)
        '
        'eventsDropDown
        '
        Me.eventsDropDown.Image = CType(resources.GetObject("eventsDropDown.Image"), System.Drawing.Image)
        Me.eventsDropDown.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.eventsDropDown.Name = "eventsDropDown"
        Me.eventsDropDown.Size = New System.Drawing.Size(108, 22)
        Me.eventsDropDown.Text = "Activity Events"
        '
        'workflowEventsDropDown
        '
        Me.workflowEventsDropDown.Image = CType(resources.GetObject("workflowEventsDropDown.Image"), System.Drawing.Image)
        Me.workflowEventsDropDown.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.workflowEventsDropDown.Name = "workflowEventsDropDown"
        Me.workflowEventsDropDown.Size = New System.Drawing.Size(117, 22)
        Me.workflowEventsDropDown.Text = "Workflow Events"
        '
        'eventsSeparator
        '
        Me.eventsSeparator.Name = "eventsSeparator"
        Me.eventsSeparator.Size = New System.Drawing.Size(6, 25)
        '
        'annotateButton
        '
        Me.annotateButton.Image = CType(resources.GetObject("annotateButton.Image"), System.Drawing.Image)
        Me.annotateButton.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.annotateButton.Name = "annotateButton"
        Me.annotateButton.Size = New System.Drawing.Size(72, 22)
        Me.annotateButton.Text = "Annotate"
        '
        'extractDropDown
        '
        Me.extractDropDown.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.activityMembersToolStripMenuItem, Me.workflowMembersToolStripMenuItem})
        Me.extractDropDown.Image = CType(resources.GetObject("extractDropDown.Image"), System.Drawing.Image)
        Me.extractDropDown.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.extractDropDown.Name = "extractDropDown"
        Me.extractDropDown.Size = New System.Drawing.Size(97, 22)
        Me.extractDropDown.Text = "Extract Data"
        '
        'activityMembersToolStripMenuItem
        '
        Me.activityMembersToolStripMenuItem.Name = "activityMembersToolStripMenuItem"
        Me.activityMembersToolStripMenuItem.Size = New System.Drawing.Size(119, 22)
        Me.activityMembersToolStripMenuItem.Text = "&Activity"
        '
        'workflowMembersToolStripMenuItem
        '
        Me.workflowMembersToolStripMenuItem.Name = "workflowMembersToolStripMenuItem"
        Me.workflowMembersToolStripMenuItem.Size = New System.Drawing.Size(119, 22)
        Me.workflowMembersToolStripMenuItem.Text = "&Workflow"
        '
        'conditionsDropDown
        '
        Me.conditionsDropDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text
        Me.conditionsDropDown.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.addConditionButton})
        Me.conditionsDropDown.Image = CType(resources.GetObject("conditionsDropDown.Image"), System.Drawing.Image)
        Me.conditionsDropDown.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.conditionsDropDown.Name = "conditionsDropDown"
        Me.conditionsDropDown.Size = New System.Drawing.Size(70, 22)
        Me.conditionsDropDown.Text = "Conditions"
        '
        'addConditionButton
        '
        Me.addConditionButton.Name = "addConditionButton"
        Me.addConditionButton.Size = New System.Drawing.Size(174, 22)
        Me.addConditionButton.Text = "Add new condition..."
        '
        'matchDerivedTypes
        '
        Me.matchDerivedTypes.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text
        Me.matchDerivedTypes.Image = CType(resources.GetObject("matchDerivedTypes.Image"), System.Drawing.Image)
        Me.matchDerivedTypes.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.matchDerivedTypes.Name = "matchDerivedTypes"
        Me.matchDerivedTypes.Size = New System.Drawing.Size(112, 22)
        Me.matchDerivedTypes.Text = "Match Derived Types"
        Me.matchDerivedTypes.Visible = False
        '
        'annotationToolStrip
        '
        Me.annotationToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden
        Me.annotationToolStrip.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.annotationText, Me.annotateSaveButton, Me.annotateCancelButton})
        Me.annotationToolStrip.Location = New System.Drawing.Point(0, 25)
        Me.annotationToolStrip.Name = "annotationToolStrip"
        Me.annotationToolStrip.Size = New System.Drawing.Size(734, 25)
        Me.annotationToolStrip.TabIndex = 4
        Me.annotationToolStrip.Text = "annotateToolStrip"
        '
        'annotationText
        '
        Me.annotationText.AutoSize = False
        Me.annotationText.Name = "annotationText"
        Me.annotationText.Size = New System.Drawing.Size(300, 25)
        '
        'annotateSaveButton
        '
        Me.annotateSaveButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text
        Me.annotateSaveButton.Image = CType(resources.GetObject("annotateSaveButton.Image"), System.Drawing.Image)
        Me.annotateSaveButton.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.annotateSaveButton.Name = "annotateSaveButton"
        Me.annotateSaveButton.Size = New System.Drawing.Size(35, 22)
        Me.annotateSaveButton.Text = "Save"
        '
        'annotateCancelButton
        '
        Me.annotateCancelButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text
        Me.annotateCancelButton.Image = CType(resources.GetObject("annotateCancelButton.Image"), System.Drawing.Image)
        Me.annotateCancelButton.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.annotateCancelButton.Name = "annotateCancelButton"
        Me.annotateCancelButton.Size = New System.Drawing.Size(43, 22)
        Me.annotateCancelButton.Text = "Cancel"
        '
        'conditionToolStrip
        '
        Me.conditionToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden
        Me.conditionToolStrip.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.conditionMemberDropDown, Me.conditionEqualityButton, Me.conditionValue, Me.conditionSaveButton, Me.conditionCancelButton, Me.conditionDeleteButton})
        Me.conditionToolStrip.Location = New System.Drawing.Point(0, 50)
        Me.conditionToolStrip.Name = "conditionToolStrip"
        Me.conditionToolStrip.Size = New System.Drawing.Size(734, 25)
        Me.conditionToolStrip.TabIndex = 6
        Me.conditionToolStrip.Text = "toolStrip2"
        '
        'conditionMemberDropDown
        '
        Me.conditionMemberDropDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text
        Me.conditionMemberDropDown.Image = CType(resources.GetObject("conditionMemberDropDown.Image"), System.Drawing.Image)
        Me.conditionMemberDropDown.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.conditionMemberDropDown.Name = "conditionMemberDropDown"
        Me.conditionMemberDropDown.Size = New System.Drawing.Size(90, 22)
        Me.conditionMemberDropDown.Text = "Select member"
        '
        'conditionEqualityButton
        '
        Me.conditionEqualityButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text
        Me.conditionEqualityButton.Image = CType(resources.GetObject("conditionEqualityButton.Image"), System.Drawing.Image)
        Me.conditionEqualityButton.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.conditionEqualityButton.Name = "conditionEqualityButton"
        Me.conditionEqualityButton.Size = New System.Drawing.Size(27, 22)
        Me.conditionEqualityButton.Text = "=="
        '
        'conditionValue
        '
        Me.conditionValue.Name = "conditionValue"
        Me.conditionValue.Size = New System.Drawing.Size(100, 25)
        '
        'conditionSaveButton
        '
        Me.conditionSaveButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text
        Me.conditionSaveButton.Image = CType(resources.GetObject("conditionSaveButton.Image"), System.Drawing.Image)
        Me.conditionSaveButton.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.conditionSaveButton.Name = "conditionSaveButton"
        Me.conditionSaveButton.Size = New System.Drawing.Size(35, 22)
        Me.conditionSaveButton.Text = "Save"
        '
        'conditionCancelButton
        '
        Me.conditionCancelButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text
        Me.conditionCancelButton.Image = CType(resources.GetObject("conditionCancelButton.Image"), System.Drawing.Image)
        Me.conditionCancelButton.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.conditionCancelButton.Name = "conditionCancelButton"
        Me.conditionCancelButton.Size = New System.Drawing.Size(43, 22)
        Me.conditionCancelButton.Text = "Cancel"
        '
        'conditionDeleteButton
        '
        Me.conditionDeleteButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text
        Me.conditionDeleteButton.Image = CType(resources.GetObject("conditionDeleteButton.Image"), System.Drawing.Image)
        Me.conditionDeleteButton.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.conditionDeleteButton.Name = "conditionDeleteButton"
        Me.conditionDeleteButton.Size = New System.Drawing.Size(42, 22)
        Me.conditionDeleteButton.Text = "Delete"
        '
        'saveFileDialog1
        '
        Me.saveFileDialog1.Filter = "Workflow Tracking Profiles (*.xml) | *.xml|All files|*.*"
        '
        'openProfileDialog
        '
        Me.openProfileDialog.Filter = "Workflow Tracking Profiles (*.xml)|*.xml"
        '
        'openWorkflowDialog
        '
        Me.openWorkflowDialog.Filter = "Workflow libraries (*.dll)|*.dll|Workflow executables (*.exe)|*.exe|All files|*.*"
        Me.openWorkflowDialog.Title = "Choose Workflow"
        '
        'openAssemblyDialog
        '
        Me.openAssemblyDialog.Filter = "Assembly (*.dll) | *.dll"
        '
        'panel1
        '
        Me.panel1.AutoSize = True
        Me.panel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink
        Me.panel1.Controls.Add(Me.conditionToolStrip)
        Me.panel1.Controls.Add(Me.tabControl1)
        Me.panel1.Controls.Add(Me.annotationToolStrip)
        Me.panel1.Controls.Add(Me.toolStrip1)
        Me.panel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.panel1.Location = New System.Drawing.Point(0, 24)
        Me.panel1.Name = "panel1"
        Me.panel1.Size = New System.Drawing.Size(734, 470)
        Me.panel1.TabIndex = 1
        '
        'tabControl1
        '
        Me.tabControl1.Controls.Add(Me.TabPage1)
        Me.tabControl1.Controls.Add(Me.TabPage2)
        Me.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tabControl1.Location = New System.Drawing.Point(0, 50)
        Me.tabControl1.Name = "tabControl1"
        Me.tabControl1.SelectedIndex = 0
        Me.tabControl1.Size = New System.Drawing.Size(734, 420)
        Me.tabControl1.TabIndex = 4
        '
        'TabPage1
        '
        Me.TabPage1.Controls.Add(Me.WorkflowDesignerControl1)
        Me.TabPage1.Location = New System.Drawing.Point(4, 22)
        Me.TabPage1.Name = "TabPage1"
        Me.TabPage1.Padding = New System.Windows.Forms.Padding(3)
        Me.TabPage1.Size = New System.Drawing.Size(726, 394)
        Me.TabPage1.TabIndex = 0
        Me.TabPage1.Text = "TrackingProfileDesigner"
        Me.TabPage1.UseVisualStyleBackColor = True
        '
        'WorkflowDesignerControl1
        '
        Me.WorkflowDesignerControl1.AutoSize = True
        Me.WorkflowDesignerControl1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink
        Me.WorkflowDesignerControl1.BackColor = System.Drawing.SystemColors.Control
        Me.WorkflowDesignerControl1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.WorkflowDesignerControl1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.WorkflowDesignerControl1.Location = New System.Drawing.Point(3, 3)
        Me.WorkflowDesignerControl1.Name = "WorkflowDesignerControl1"
        Me.WorkflowDesignerControl1.Size = New System.Drawing.Size(720, 388)
        Me.WorkflowDesignerControl1.TabIndex = 0
        Me.WorkflowDesignerControl1.WorkflowType = Nothing
        Me.WorkflowDesignerControl1.Xaml = ""
        '
        'TabPage2
        '
        Me.TabPage2.Controls.Add(Me.profileMarkup)
        Me.TabPage2.Location = New System.Drawing.Point(4, 22)
        Me.TabPage2.Name = "TabPage2"
        Me.TabPage2.Padding = New System.Windows.Forms.Padding(3)
        Me.TabPage2.Size = New System.Drawing.Size(726, 394)
        Me.TabPage2.TabIndex = 1
        Me.TabPage2.Text = "Tracking Profile Markup"
        Me.TabPage2.UseVisualStyleBackColor = True
        '
        'profileMarkup
        '
        Me.profileMarkup.Dock = System.Windows.Forms.DockStyle.Fill
        Me.profileMarkup.Location = New System.Drawing.Point(3, 3)
        Me.profileMarkup.Name = "profileMarkup"
        Me.profileMarkup.Size = New System.Drawing.Size(720, 388)
        Me.profileMarkup.TabIndex = 0
        Me.profileMarkup.Text = ""
        '
        'WorkflowTrackingProfileDesignerForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(734, 494)
        Me.Controls.Add(Me.panel1)
        Me.Controls.Add(Me.menuStrip1)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MainMenuStrip = Me.menuStrip1
        Me.Name = "WorkflowTrackingProfileDesignerForm"
        Me.Text = "Workflow Tracking Profile Designer"
        Me.menuStrip1.ResumeLayout(False)
        Me.menuStrip1.PerformLayout()
        Me.toolStrip1.ResumeLayout(False)
        Me.toolStrip1.PerformLayout()
        Me.annotationToolStrip.ResumeLayout(False)
        Me.annotationToolStrip.PerformLayout()
        Me.conditionToolStrip.ResumeLayout(False)
        Me.conditionToolStrip.PerformLayout()
        Me.panel1.ResumeLayout(False)
        Me.panel1.PerformLayout()
        Me.tabControl1.ResumeLayout(False)
        Me.TabPage1.ResumeLayout(False)
        Me.TabPage1.PerformLayout()
        Me.TabPage2.ResumeLayout(False)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents menuStrip1 As System.Windows.Forms.MenuStrip
    Friend WithEvents FileToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents openToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents fromFileToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents profileToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents fromSqlToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents saveToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents profileAsFileToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents toSqlTrackingStoreToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents exitToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Private WithEvents toolStrip1 As System.Windows.Forms.ToolStrip
    Private WithEvents trackButton As System.Windows.Forms.ToolStripButton
    Friend WithEvents ToolStripSeparator1 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents eventsDropDown As System.Windows.Forms.ToolStripDropDownButton
    Private WithEvents workflowEventsDropDown As System.Windows.Forms.ToolStripDropDownButton
    Friend WithEvents annotateButton As System.Windows.Forms.ToolStripButton
    Private WithEvents extractDropDown As System.Windows.Forms.ToolStripDropDownButton
    Private WithEvents conditionsDropDown As System.Windows.Forms.ToolStripDropDownButton
    Friend WithEvents addConditionButton As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents eventsSeparator As System.Windows.Forms.ToolStripSeparator
    Private WithEvents annotationToolStrip As System.Windows.Forms.ToolStrip
    Private WithEvents annotationText As System.Windows.Forms.ToolStripTextBox
    Private WithEvents annotateSaveButton As System.Windows.Forms.ToolStripButton
    Private WithEvents annotateCancelButton As System.Windows.Forms.ToolStripButton
    Private WithEvents conditionToolStrip As System.Windows.Forms.ToolStrip
    Private WithEvents conditionMemberDropDown As System.Windows.Forms.ToolStripDropDownButton
    Private WithEvents conditionEqualityButton As System.Windows.Forms.ToolStripButton
    Private WithEvents conditionValue As System.Windows.Forms.ToolStripTextBox
    Private WithEvents conditionSaveButton As System.Windows.Forms.ToolStripButton
    Private WithEvents conditionCancelButton As System.Windows.Forms.ToolStripButton
    Private WithEvents matchDerivedTypes As System.Windows.Forms.ToolStripButton
    Friend WithEvents conditionDeleteButton As System.Windows.Forms.ToolStripButton
    Private WithEvents saveFileDialog1 As System.Windows.Forms.SaveFileDialog
    Private WithEvents openProfileDialog As System.Windows.Forms.OpenFileDialog
    Private WithEvents openWorkflowDialog As System.Windows.Forms.OpenFileDialog
    Private WithEvents openAssemblyDialog As System.Windows.Forms.OpenFileDialog
    Private WithEvents panel1 As System.Windows.Forms.Panel
    Private WithEvents tabControl1 As System.Windows.Forms.TabControl
    Friend WithEvents TabPage1 As System.Windows.Forms.TabPage
    Friend WithEvents TabPage2 As System.Windows.Forms.TabPage
    Friend WithEvents WorkflowDesignerControl1 As WorkflowDesignerControl
    Private WithEvents profileMarkup As System.Windows.Forms.RichTextBox
    Private WithEvents activityMembersToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents workflowMembersToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem

End Class
