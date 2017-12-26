//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

namespace Microsoft.Samples.Workflow.WorkflowMonitor
{
    partial class Mainform
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Mainform));
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.statusLabelDatabaseName = new System.Windows.Forms.ToolStripStatusLabel();
            this.statusLabelMonitoring = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripMain = new System.Windows.Forms.ToolStrip();
            this.toolStripButtonSettings = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButtonMonitorOn = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonMonitorOff = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButtonCollapse = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonExpand = new System.Windows.Forms.ToolStripButton();
            this.toolStripComboBoxZoom = new System.Windows.Forms.ToolStripComboBox();
            this.listViewWorkflows = new System.Windows.Forms.ListView();
            this.workflowsIdHeader = new System.Windows.Forms.ColumnHeader();
            this.workflowsNameHeader = new System.Windows.Forms.ColumnHeader();
            this.workflowsStatusHeader = new System.Windows.Forms.ColumnHeader();
            this.workflowsLabel = new System.Windows.Forms.Label();
            this.viewHostLabel = new System.Windows.Forms.Label();
            this.trackingSurface = new System.Windows.Forms.SplitContainer();
            this.workflowDetails = new System.Windows.Forms.SplitContainer();
            this.listViewActivities = new System.Windows.Forms.ListView();
            this.activitiesIdHeader = new System.Windows.Forms.ColumnHeader();
            this.activitiesNameHeader = new System.Windows.Forms.ColumnHeader();
            this.activitiesStatusHeader = new System.Windows.Forms.ColumnHeader();
            this.activitiesLabel = new System.Windows.Forms.Label();
            this.workflowViewErrorText = new System.Windows.Forms.Label();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.mainMenu = new System.Windows.Forms.MenuStrip();
            this.menuFile = new System.Windows.Forms.ToolStripMenuItem();
            this.menuExit = new System.Windows.Forms.ToolStripMenuItem();
            this.menuView = new System.Windows.Forms.ToolStripMenuItem();
            this.menuActivityDetailsView = new System.Windows.Forms.ToolStripMenuItem();
            this.menuMonitor = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStartStop = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSettings = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripWorkflows = new System.Windows.Forms.ToolStrip();
            this.toolStripLabelWorkflowStatus = new System.Windows.Forms.ToolStripLabel();
            this.toolStripComboBoxWorkflowEvent = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripLabelFrom = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBoxFrom = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripLabelUntil = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBoxUntil = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripLabelActivityname = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBoxArtifactQualifiedId = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripLabelProperty = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBoxArtifactKeyName = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripLabelArtifactKeyValue = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBoxArtifactKeyValue = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripButtonFindWorkflows = new System.Windows.Forms.ToolStripButton();
            this.toolStripLabelWorkflowInstanceId = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBoxWorkflowInstanceId = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripButtonFindWorkflowById = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonResetOptions = new System.Windows.Forms.ToolStripButton();
            this.statusStrip.SuspendLayout();
            this.toolStripMain.SuspendLayout();
            this.trackingSurface.Panel1.SuspendLayout();
            this.trackingSurface.Panel2.SuspendLayout();
            this.trackingSurface.SuspendLayout();
            this.workflowDetails.Panel1.SuspendLayout();
            this.workflowDetails.Panel2.SuspendLayout();
            this.workflowDetails.SuspendLayout();
            this.mainMenu.SuspendLayout();
            this.toolStripWorkflows.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip
            // 
            this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusLabelDatabaseName,
            this.statusLabelMonitoring});
            this.statusStrip.Location = new System.Drawing.Point(0, 712);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(1016, 22);
            this.statusStrip.TabIndex = 4;
            this.statusStrip.Text = "statusStrip1";
            // 
            // statusLabelDatabaseName
            // 
            this.statusLabelDatabaseName.AutoSize = false;
            this.statusLabelDatabaseName.Name = "statusLabelDatabaseName";
            this.statusLabelDatabaseName.Size = new System.Drawing.Size(250, 17);
            this.statusLabelDatabaseName.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // statusLabelMonitoring
            // 
            this.statusLabelMonitoring.AutoSize = false;
            this.statusLabelMonitoring.Name = "statusLabelMonitoring";
            this.statusLabelMonitoring.Size = new System.Drawing.Size(65, 17);
            this.statusLabelMonitoring.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // toolStripMain
            // 
            this.toolStripMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButtonSettings,
            this.toolStripSeparator1,
            this.toolStripButtonMonitorOn,
            this.toolStripButtonMonitorOff,
            this.toolStripSeparator2,
            this.toolStripButtonCollapse,
            this.toolStripButtonExpand,
            this.toolStripComboBoxZoom});
            this.toolStripMain.Location = new System.Drawing.Point(0, 49);
            this.toolStripMain.Name = "toolStripMain";
            this.toolStripMain.Size = new System.Drawing.Size(1016, 25);
            this.toolStripMain.TabIndex = 2;
            this.toolStripMain.Text = "toolStrip1";
            // 
            // toolStripButtonSettings
            // 
            this.toolStripButtonSettings.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonSettings.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonSettings.Image")));
            this.toolStripButtonSettings.ImageTransparentColor = System.Drawing.Color.Cyan;
            this.toolStripButtonSettings.Name = "toolStripButtonSettings";
            this.toolStripButtonSettings.Size = new System.Drawing.Size(23, 22);
            this.toolStripButtonSettings.Text = "Settings";
            this.toolStripButtonSettings.Click += new System.EventHandler(this.ToolStripButtonSettings_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripButtonMonitorOn
            // 
            this.toolStripButtonMonitorOn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonMonitorOn.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonMonitorOn.Image")));
            this.toolStripButtonMonitorOn.ImageTransparentColor = System.Drawing.Color.Aqua;
            this.toolStripButtonMonitorOn.Name = "toolStripButtonMonitorOn";
            this.toolStripButtonMonitorOn.Size = new System.Drawing.Size(23, 22);
            this.toolStripButtonMonitorOn.Text = "Monitor On";
            this.toolStripButtonMonitorOn.Click += new System.EventHandler(this.ToolStripButtonMonitorOn_Click);
            // 
            // toolStripButtonMonitorOff
            // 
            this.toolStripButtonMonitorOff.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonMonitorOff.Enabled = false;
            this.toolStripButtonMonitorOff.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonMonitorOff.Image")));
            this.toolStripButtonMonitorOff.ImageTransparentColor = System.Drawing.Color.Aqua;
            this.toolStripButtonMonitorOff.Name = "toolStripButtonMonitorOff";
            this.toolStripButtonMonitorOff.Size = new System.Drawing.Size(23, 22);
            this.toolStripButtonMonitorOff.Text = "Monitor Off";
            this.toolStripButtonMonitorOff.Click += new System.EventHandler(this.ToolStripButtonMonitorOff_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripButtonCollapse
            // 
            this.toolStripButtonCollapse.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonCollapse.Enabled = false;
            this.toolStripButtonCollapse.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonCollapse.Image")));
            this.toolStripButtonCollapse.ImageTransparentColor = System.Drawing.Color.Aqua;
            this.toolStripButtonCollapse.Name = "toolStripButtonCollapse";
            this.toolStripButtonCollapse.Size = new System.Drawing.Size(23, 22);
            this.toolStripButtonCollapse.Text = "Collapse All";
            this.toolStripButtonCollapse.Click += new System.EventHandler(this.ToolStripButtonCollapse_Click);
            // 
            // toolStripButtonExpand
            // 
            this.toolStripButtonExpand.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonExpand.Enabled = false;
            this.toolStripButtonExpand.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonExpand.Image")));
            this.toolStripButtonExpand.ImageTransparentColor = System.Drawing.Color.Aqua;
            this.toolStripButtonExpand.Name = "toolStripButtonExpand";
            this.toolStripButtonExpand.Size = new System.Drawing.Size(23, 22);
            this.toolStripButtonExpand.Text = "Expand All";
            this.toolStripButtonExpand.Click += new System.EventHandler(this.ToolStripButtonExpand_Click);
            // 
            // toolStripComboBoxZoom
            // 
            this.toolStripComboBoxZoom.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.toolStripComboBoxZoom.Enabled = false;
            this.toolStripComboBoxZoom.Items.AddRange(new object[] {
            "50%",
            "75%",
            "100%",
            "150%",
            "200%",
            "300%",
            "400%"});
            this.toolStripComboBoxZoom.Name = "toolStripComboBoxZoom";
            this.toolStripComboBoxZoom.Size = new System.Drawing.Size(75, 25);
            this.toolStripComboBoxZoom.ToolTipText = "Workflow Zoom Level";
            this.toolStripComboBoxZoom.SelectedIndexChanged += new System.EventHandler(this.ToolStripComboBoxZoom_SelectedIndexChanged);
            // 
            // listViewWorkflows
            // 
            this.listViewWorkflows.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.workflowsIdHeader,
            this.workflowsNameHeader,
            this.workflowsStatusHeader});
            this.listViewWorkflows.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listViewWorkflows.FullRowSelect = true;
            this.listViewWorkflows.HideSelection = false;
            this.listViewWorkflows.LabelWrap = false;
            this.listViewWorkflows.Location = new System.Drawing.Point(0, 15);
            this.listViewWorkflows.Margin = new System.Windows.Forms.Padding(2, 3, 3, 3);
            this.listViewWorkflows.MultiSelect = false;
            this.listViewWorkflows.Name = "listViewWorkflows";
            this.listViewWorkflows.ShowGroups = false;
            this.listViewWorkflows.Size = new System.Drawing.Size(364, 298);
            this.listViewWorkflows.TabIndex = 0;
            this.listViewWorkflows.UseCompatibleStateImageBehavior = false;
            this.listViewWorkflows.View = System.Windows.Forms.View.Details;
            this.listViewWorkflows.SelectedIndexChanged += new System.EventHandler(this.listViewWorkflows_SelectedIndexChanged);
            this.listViewWorkflows.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.OnWorkflowsColumnClick);
            this.listViewWorkflows.ItemSelectionChanged += new System.Windows.Forms.ListViewItemSelectionChangedEventHandler(this.ListViewWorkflows_ItemSelectionChanged);
            this.listViewWorkflows.KeyUp += new System.Windows.Forms.KeyEventHandler(this.ManualRefresh);
            // 
            // workflowsIdHeader
            // 
            this.workflowsIdHeader.Text = "Id";
            this.workflowsIdHeader.Width = 35;
            // 
            // workflowsNameHeader
            // 
            this.workflowsNameHeader.Text = "Name";
            this.workflowsNameHeader.Width = 150;
            // 
            // workflowsStatusHeader
            // 
            this.workflowsStatusHeader.Text = "Status";
            this.workflowsStatusHeader.Width = 100;
            // 
            // workflowsLabel
            // 
            this.workflowsLabel.BackColor = System.Drawing.SystemColors.GradientInactiveCaption;
            this.workflowsLabel.Dock = System.Windows.Forms.DockStyle.Top;
            this.workflowsLabel.Location = new System.Drawing.Point(0, 0);
            this.workflowsLabel.Name = "workflowsLabel";
            this.workflowsLabel.Size = new System.Drawing.Size(364, 15);
            this.workflowsLabel.TabIndex = 3;
            this.workflowsLabel.Text = "Workflows";
            // 
            // viewHostLabel
            // 
            this.viewHostLabel.BackColor = System.Drawing.SystemColors.GradientInactiveCaption;
            this.viewHostLabel.Dock = System.Windows.Forms.DockStyle.Top;
            this.viewHostLabel.Location = new System.Drawing.Point(0, 0);
            this.viewHostLabel.Name = "viewHostLabel";
            this.viewHostLabel.Size = new System.Drawing.Size(640, 15);
            this.viewHostLabel.TabIndex = 4;
            this.viewHostLabel.Text = "Workflow View";
            // 
            // trackingSurface
            // 
            this.trackingSurface.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.trackingSurface.Dock = System.Windows.Forms.DockStyle.Fill;
            this.trackingSurface.Location = new System.Drawing.Point(0, 74);
            this.trackingSurface.Name = "trackingSurface";
            // 
            // trackingSurface.Panel1
            // 
            this.trackingSurface.Panel1.Controls.Add(this.workflowDetails);
            // 
            // trackingSurface.Panel2
            // 
            this.trackingSurface.Panel2.Controls.Add(this.workflowViewErrorText);
            this.trackingSurface.Panel2.Controls.Add(this.viewHostLabel);
            this.trackingSurface.Size = new System.Drawing.Size(1016, 638);
            this.trackingSurface.SplitterDistance = 368;
            this.trackingSurface.TabIndex = 5;
            // 
            // workflowDetails
            // 
            this.workflowDetails.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.workflowDetails.Dock = System.Windows.Forms.DockStyle.Fill;
            this.workflowDetails.Location = new System.Drawing.Point(0, 0);
            this.workflowDetails.Name = "workflowDetails";
            this.workflowDetails.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // workflowDetails.Panel1
            // 
            this.workflowDetails.Panel1.Controls.Add(this.listViewWorkflows);
            this.workflowDetails.Panel1.Controls.Add(this.workflowsLabel);
            // 
            // workflowDetails.Panel2
            // 
            this.workflowDetails.Panel2.Controls.Add(this.listViewActivities);
            this.workflowDetails.Panel2.Controls.Add(this.activitiesLabel);
            this.workflowDetails.Size = new System.Drawing.Size(368, 638);
            this.workflowDetails.SplitterDistance = 317;
            this.workflowDetails.TabIndex = 0;
            // 
            // listViewActivities
            // 
            this.listViewActivities.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.activitiesIdHeader,
            this.activitiesNameHeader,
            this.activitiesStatusHeader});
            this.listViewActivities.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listViewActivities.FullRowSelect = true;
            this.listViewActivities.LabelWrap = false;
            this.listViewActivities.Location = new System.Drawing.Point(0, 15);
            this.listViewActivities.Margin = new System.Windows.Forms.Padding(2, 3, 3, 3);
            this.listViewActivities.MultiSelect = false;
            this.listViewActivities.Name = "listViewActivities";
            this.listViewActivities.ShowGroups = false;
            this.listViewActivities.Size = new System.Drawing.Size(364, 298);
            this.listViewActivities.TabIndex = 2;
            this.listViewActivities.UseCompatibleStateImageBehavior = false;
            this.listViewActivities.View = System.Windows.Forms.View.Details;
            this.listViewActivities.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.OnActivitiesColumnClick);
            this.listViewActivities.KeyUp += new System.Windows.Forms.KeyEventHandler(this.ManualRefresh);
            this.listViewActivities.Click += new System.EventHandler(this.ListViewActivities_Click);
            // 
            // activitiesIdHeader
            // 
            this.activitiesIdHeader.Text = "Id";
            this.activitiesIdHeader.Width = 35;
            // 
            // activitiesNameHeader
            // 
            this.activitiesNameHeader.Text = "Name";
            this.activitiesNameHeader.Width = 150;
            // 
            // activitiesStatusHeader
            // 
            this.activitiesStatusHeader.Text = "Status";
            this.activitiesStatusHeader.Width = 100;
            // 
            // activitiesLabel
            // 
            this.activitiesLabel.BackColor = System.Drawing.SystemColors.GradientInactiveCaption;
            this.activitiesLabel.Dock = System.Windows.Forms.DockStyle.Top;
            this.activitiesLabel.Location = new System.Drawing.Point(0, 0);
            this.activitiesLabel.Name = "activitiesLabel";
            this.activitiesLabel.Size = new System.Drawing.Size(364, 15);
            this.activitiesLabel.TabIndex = 1;
            this.activitiesLabel.Text = "Activities";
            // 
            // workflowViewErrorText
            // 
            this.workflowViewErrorText.BackColor = System.Drawing.SystemColors.GradientInactiveCaption;
            this.workflowViewErrorText.Dock = System.Windows.Forms.DockStyle.Fill;
            this.workflowViewErrorText.Location = new System.Drawing.Point(0, 15);
            this.workflowViewErrorText.Name = "workflowViewErrorText";
            this.workflowViewErrorText.Size = new System.Drawing.Size(640, 619);
            this.workflowViewErrorText.TabIndex = 5;
            this.workflowViewErrorText.Text = resources.GetString("workflowViewErrorText.Text");
            this.workflowViewErrorText.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // timer
            // 
            this.timer.Interval = 5000;
            this.timer.Tick += new System.EventHandler(this.Timer_Tick);
            // 
            // mainMenu
            // 
            this.mainMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuFile,
            this.menuView,
            this.menuMonitor});
            this.mainMenu.Location = new System.Drawing.Point(0, 0);
            this.mainMenu.Name = "mainMenu";
            this.mainMenu.Size = new System.Drawing.Size(1016, 24);
            this.mainMenu.TabIndex = 6;
            this.mainMenu.Text = "menuStrip1";
            // 
            // menuFile
            // 
            this.menuFile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuExit});
            this.menuFile.Name = "menuFile";
            this.menuFile.Size = new System.Drawing.Size(35, 20);
            this.menuFile.Text = "&File";
            // 
            // menuExit
            // 
            this.menuExit.Name = "menuExit";
            this.menuExit.Size = new System.Drawing.Size(103, 22);
            this.menuExit.Text = "E&xit";
            this.menuExit.Click += new System.EventHandler(this.MenuExit_Click);
            // 
            // menuView
            // 
            this.menuView.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuActivityDetailsView});
            this.menuView.Name = "menuView";
            this.menuView.Size = new System.Drawing.Size(41, 20);
            this.menuView.Text = "&View";
            // 
            // menuActivityDetailsView
            // 
            this.menuActivityDetailsView.Checked = true;
            this.menuActivityDetailsView.CheckState = System.Windows.Forms.CheckState.Checked;
            this.menuActivityDetailsView.Name = "menuActivityDetailsView";
            this.menuActivityDetailsView.Size = new System.Drawing.Size(165, 22);
            this.menuActivityDetailsView.Text = "&Workflow Details";
            this.menuActivityDetailsView.Click += new System.EventHandler(this.MenuActivityDetailsView_Click);
            // 
            // menuMonitor
            // 
            this.menuMonitor.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuStartStop,
            this.menuSettings});
            this.menuMonitor.Name = "menuMonitor";
            this.menuMonitor.Size = new System.Drawing.Size(55, 20);
            this.menuMonitor.Text = "&Monitor";
            // 
            // menuStartStop
            // 
            this.menuStartStop.Name = "menuStartStop";
            this.menuStartStop.Size = new System.Drawing.Size(124, 22);
            this.menuStartStop.Text = "Start";
            this.menuStartStop.Click += new System.EventHandler(this.MenuStartStop_Click);
            // 
            // menuSettings
            // 
            this.menuSettings.Name = "menuSettings";
            this.menuSettings.Size = new System.Drawing.Size(124, 22);
            this.menuSettings.Text = "Settings";
            this.menuSettings.Click += new System.EventHandler(this.MenuSettings_Click);
            // 
            // toolStripWorkflows
            // 
            this.toolStripWorkflows.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabelWorkflowStatus,
            this.toolStripComboBoxWorkflowEvent,
            this.toolStripLabelFrom,
            this.toolStripTextBoxFrom,
            this.toolStripLabelUntil,
            this.toolStripTextBoxUntil,
            this.toolStripLabelActivityname,
            this.toolStripTextBoxArtifactQualifiedId,
            this.toolStripLabelProperty,
            this.toolStripTextBoxArtifactKeyName,
            this.toolStripLabelArtifactKeyValue,
            this.toolStripTextBoxArtifactKeyValue,
            this.toolStripButtonFindWorkflows,
            this.toolStripLabelWorkflowInstanceId,
            this.toolStripTextBoxWorkflowInstanceId,
            this.toolStripButtonFindWorkflowById,
            this.toolStripButtonResetOptions});
            this.toolStripWorkflows.Location = new System.Drawing.Point(0, 24);
            this.toolStripWorkflows.Name = "toolStripWorkflows";
            this.toolStripWorkflows.Size = new System.Drawing.Size(1016, 25);
            this.toolStripWorkflows.TabIndex = 7;
            this.toolStripWorkflows.Text = "toolStrip1";
            // 
            // toolStripLabelWorkflowStatus
            // 
            this.toolStripLabelWorkflowStatus.Name = "toolStripLabelWorkflowStatus";
            this.toolStripLabelWorkflowStatus.Size = new System.Drawing.Size(86, 22);
            this.toolStripLabelWorkflowStatus.Text = "Workflow Status";
            // 
            // toolStripComboBoxWorkflowEvent
            // 
            this.toolStripComboBoxWorkflowEvent.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.toolStripComboBoxWorkflowEvent.Items.AddRange(new object[] {
            "All",
            "Created",
            "Completed",
            "Running",
            "Suspended",
            "Terminated"});
            this.toolStripComboBoxWorkflowEvent.Name = "toolStripComboBoxWorkflowEvent";
            this.toolStripComboBoxWorkflowEvent.Size = new System.Drawing.Size(75, 25);
            this.toolStripComboBoxWorkflowEvent.ToolTipText = "Workflow Event";
            this.toolStripComboBoxWorkflowEvent.SelectedIndexChanged += new System.EventHandler(this.ToolStripComboBoxWorkflowEvent_SelectedIndexChanged);
            // 
            // toolStripLabelFrom
            // 
            this.toolStripLabelFrom.Name = "toolStripLabelFrom";
            this.toolStripLabelFrom.Size = new System.Drawing.Size(31, 22);
            this.toolStripLabelFrom.Text = "From";
            this.toolStripLabelFrom.ToolTipText = "From";
            // 
            // toolStripTextBoxFrom
            // 
            this.toolStripTextBoxFrom.Name = "toolStripTextBoxFrom";
            this.toolStripTextBoxFrom.Size = new System.Drawing.Size(60, 25);
            // 
            // toolStripLabelUntil
            // 
            this.toolStripLabelUntil.Name = "toolStripLabelUntil";
            this.toolStripLabelUntil.Size = new System.Drawing.Size(28, 22);
            this.toolStripLabelUntil.Text = "Until";
            this.toolStripLabelUntil.ToolTipText = "Until";
            // 
            // toolStripTextBoxUntil
            // 
            this.toolStripTextBoxUntil.Name = "toolStripTextBoxUntil";
            this.toolStripTextBoxUntil.Size = new System.Drawing.Size(60, 25);
            // 
            // toolStripLabelActivityname
            // 
            this.toolStripLabelActivityname.Name = "toolStripLabelActivityname";
            this.toolStripLabelActivityname.Size = new System.Drawing.Size(73, 22);
            this.toolStripLabelActivityname.Text = "Activity Name";
            this.toolStripLabelActivityname.ToolTipText = "AActivity Name";
            // 
            // toolStripTextBoxArtifactQualifiedId
            // 
            this.toolStripTextBoxArtifactQualifiedId.Name = "toolStripTextBoxArtifactQualifiedId";
            this.toolStripTextBoxArtifactQualifiedId.Size = new System.Drawing.Size(60, 25);
            // 
            // toolStripLabelProperty
            // 
            this.toolStripLabelProperty.Name = "toolStripLabelProperty";
            this.toolStripLabelProperty.Size = new System.Drawing.Size(49, 22);
            this.toolStripLabelProperty.Text = "Property";
            this.toolStripLabelProperty.ToolTipText = "Property";
            // 
            // toolStripTextBoxArtifactKeyName
            // 
            this.toolStripTextBoxArtifactKeyName.Name = "toolStripTextBoxArtifactKeyName";
            this.toolStripTextBoxArtifactKeyName.Size = new System.Drawing.Size(60, 25);
            // 
            // toolStripLabelArtifactKeyValue
            // 
            this.toolStripLabelArtifactKeyValue.Name = "toolStripLabelArtifactKeyValue";
            this.toolStripLabelArtifactKeyValue.Size = new System.Drawing.Size(33, 22);
            this.toolStripLabelArtifactKeyValue.Text = "Value";
            this.toolStripLabelArtifactKeyValue.ToolTipText = "Value";
            // 
            // toolStripTextBoxArtifactKeyValue
            // 
            this.toolStripTextBoxArtifactKeyValue.Name = "toolStripTextBoxArtifactKeyValue";
            this.toolStripTextBoxArtifactKeyValue.Size = new System.Drawing.Size(60, 25);
            // 
            // toolStripButtonFindWorkflows
            // 
            this.toolStripButtonFindWorkflows.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonFindWorkflows.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonFindWorkflows.Image")));
            this.toolStripButtonFindWorkflows.ImageTransparentColor = System.Drawing.Color.Aqua;
            this.toolStripButtonFindWorkflows.Name = "toolStripButtonFindWorkflows";
            this.toolStripButtonFindWorkflows.Size = new System.Drawing.Size(23, 22);
            this.toolStripButtonFindWorkflows.Text = "Find Workflows";
            this.toolStripButtonFindWorkflows.Click += new System.EventHandler(this.ToolStripButtonFindWorkflows_Click);
            // 
            // toolStripLabelWorkflowInstanceId
            // 
            this.toolStripLabelWorkflowInstanceId.Name = "toolStripLabelWorkflowInstanceId";
            this.toolStripLabelWorkflowInstanceId.Size = new System.Drawing.Size(111, 22);
            this.toolStripLabelWorkflowInstanceId.Text = "Workflow Instance ID";
            this.toolStripLabelWorkflowInstanceId.ToolTipText = "Workflow Instance ID";
            // 
            // toolStripTextBoxWorkflowInstanceId
            // 
            this.toolStripTextBoxWorkflowInstanceId.Name = "toolStripTextBoxWorkflowInstanceId";
            this.toolStripTextBoxWorkflowInstanceId.Size = new System.Drawing.Size(130, 25);
            // 
            // toolStripButtonFindWorkflowById
            // 
            this.toolStripButtonFindWorkflowById.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonFindWorkflowById.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonFindWorkflowById.Image")));
            this.toolStripButtonFindWorkflowById.ImageTransparentColor = System.Drawing.Color.Aqua;
            this.toolStripButtonFindWorkflowById.Name = "toolStripButtonFindWorkflowById";
            this.toolStripButtonFindWorkflowById.Size = new System.Drawing.Size(23, 22);
            this.toolStripButtonFindWorkflowById.Text = "Find Workflows";
            this.toolStripButtonFindWorkflowById.Click += new System.EventHandler(this.ToolStripButtonFindWorkflowById_Click);
            // 
            // toolStripButtonResetOptions
            // 
            this.toolStripButtonResetOptions.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonResetOptions.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonResetOptions.Image")));
            this.toolStripButtonResetOptions.ImageTransparentColor = System.Drawing.Color.Aqua;
            this.toolStripButtonResetOptions.Name = "toolStripButtonResetOptions";
            this.toolStripButtonResetOptions.Size = new System.Drawing.Size(23, 22);
            this.toolStripButtonResetOptions.Text = "Reset Workflow Filter Options";
            this.toolStripButtonResetOptions.Click += new System.EventHandler(this.ToolStripButtonResetOptions_Click);
            // 
            // Mainform
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1016, 734);
            this.Controls.Add(this.trackingSurface);
            this.Controls.Add(this.toolStripMain);
            this.Controls.Add(this.toolStripWorkflows);
            this.Controls.Add(this.statusStrip);
            this.Controls.Add(this.mainMenu);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Mainform";
            this.Text = "Workflow Monitor";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Mainform_Closing);
            this.statusStrip.ResumeLayout(false);
            this.statusStrip.PerformLayout();
            this.toolStripMain.ResumeLayout(false);
            this.toolStripMain.PerformLayout();
            this.trackingSurface.Panel1.ResumeLayout(false);
            this.trackingSurface.Panel2.ResumeLayout(false);
            this.trackingSurface.ResumeLayout(false);
            this.workflowDetails.Panel1.ResumeLayout(false);
            this.workflowDetails.Panel2.ResumeLayout(false);
            this.workflowDetails.ResumeLayout(false);
            this.mainMenu.ResumeLayout(false);
            this.mainMenu.PerformLayout();
            this.toolStripWorkflows.ResumeLayout(false);
            this.toolStripWorkflows.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.ToolStripStatusLabel statusLabelDatabaseName;
        private System.Windows.Forms.ToolStripStatusLabel statusLabelMonitoring;
        private System.Windows.Forms.ToolStrip toolStripMain;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton toolStripButtonMonitorOn;
        private System.Windows.Forms.ToolStripButton toolStripButtonMonitorOff;
        private System.Windows.Forms.ToolStripButton toolStripButtonSettings;
        private System.Windows.Forms.ToolStripButton toolStripButtonCollapse;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton toolStripButtonExpand;
        private System.Windows.Forms.ToolStripComboBox toolStripComboBoxZoom;
        private System.Windows.Forms.ListView listViewWorkflows;
        private System.Windows.Forms.ColumnHeader workflowsIdHeader;
        private System.Windows.Forms.ColumnHeader workflowsNameHeader;
        private System.Windows.Forms.ColumnHeader workflowsStatusHeader;
        private System.Windows.Forms.Label workflowsLabel;
        private System.Windows.Forms.Label viewHostLabel;
        private System.Windows.Forms.SplitContainer trackingSurface;
        private System.Windows.Forms.SplitContainer workflowDetails;
        private System.Windows.Forms.Label activitiesLabel;
        private System.Windows.Forms.Label workflowViewErrorText;
        private System.Windows.Forms.ListView listViewActivities;
        private System.Windows.Forms.ColumnHeader activitiesIdHeader;
        private System.Windows.Forms.ColumnHeader activitiesNameHeader;
        private System.Windows.Forms.ColumnHeader activitiesStatusHeader;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.MenuStrip mainMenu;
        private System.Windows.Forms.ToolStripMenuItem menuFile;
        private System.Windows.Forms.ToolStripMenuItem menuExit;
        private System.Windows.Forms.ToolStripMenuItem menuView;
        private System.Windows.Forms.ToolStripMenuItem menuActivityDetailsView;
        private System.Windows.Forms.ToolStripMenuItem menuMonitor;
        private System.Windows.Forms.ToolStripMenuItem menuStartStop;
        private System.Windows.Forms.ToolStripMenuItem menuSettings;
        private System.Windows.Forms.ToolStrip toolStripWorkflows;
        private System.Windows.Forms.ToolStripLabel toolStripLabelWorkflowStatus;
        private System.Windows.Forms.ToolStripComboBox toolStripComboBoxWorkflowEvent;
        private System.Windows.Forms.ToolStripLabel toolStripLabelFrom;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBoxFrom;
        private System.Windows.Forms.ToolStripLabel toolStripLabelUntil;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBoxUntil;
        private System.Windows.Forms.ToolStripButton toolStripButtonFindWorkflows;
        private System.Windows.Forms.ToolStripLabel toolStripLabelActivityname;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBoxArtifactQualifiedId;
        private System.Windows.Forms.ToolStripLabel toolStripLabelProperty;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBoxArtifactKeyName;
        private System.Windows.Forms.ToolStripLabel toolStripLabelArtifactKeyValue;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBoxArtifactKeyValue;
        private System.Windows.Forms.ToolStripLabel toolStripLabelWorkflowInstanceId;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBoxWorkflowInstanceId;
        private System.Windows.Forms.ToolStripButton toolStripButtonFindWorkflowById;
        private System.Windows.Forms.ToolStripButton toolStripButtonResetOptions;
    }
}