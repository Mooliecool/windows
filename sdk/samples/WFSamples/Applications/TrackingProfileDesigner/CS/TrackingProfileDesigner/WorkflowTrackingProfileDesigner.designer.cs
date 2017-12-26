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

using System.Workflow.Runtime.Tracking;

namespace Microsoft.Samples.Workflow.TrackingProfileDesigner
{
    partial class WorkflowTrackingProfileDesignerForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(WorkflowTrackingProfileDesignerForm));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fromFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.profileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fromSqlToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.profileAsFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toSqlTrackingStoreToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadWorkflowToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveProfileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.panel1 = new System.Windows.Forms.Panel();
            this.conditionToolStrip = new System.Windows.Forms.ToolStrip();
            this.conditionMemberDropDown = new System.Windows.Forms.ToolStripDropDownButton();
            this.conditionEqualityButton = new System.Windows.Forms.ToolStripButton();
            this.conditionValue = new System.Windows.Forms.ToolStripTextBox();
            this.conditionSaveButton = new System.Windows.Forms.ToolStripButton();
            this.conditionCancelButton = new System.Windows.Forms.ToolStripButton();
            this.conditionDeleteButton = new System.Windows.Forms.ToolStripButton();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.workflowDesignerControl1 = new Microsoft.Samples.Workflow.DesignerControl.WorkflowDesignerControl();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.profileMarkup = new System.Windows.Forms.RichTextBox();
            this.annotationToolStrip = new System.Windows.Forms.ToolStrip();
            this.annotationText = new System.Windows.Forms.ToolStripTextBox();
            this.annotateSaveButton = new System.Windows.Forms.ToolStripButton();
            this.annotateCancelButton = new System.Windows.Forms.ToolStripButton();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.trackButton = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.eventsDropDown = new System.Windows.Forms.ToolStripDropDownButton();
            this.workflowEventsDropDown = new System.Windows.Forms.ToolStripDropDownButton();
            this.eventsSeparator = new System.Windows.Forms.ToolStripSeparator();
            this.annotateButton = new System.Windows.Forms.ToolStripButton();
            this.extractDropDown = new System.Windows.Forms.ToolStripDropDownButton();
            this.activityMembersToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.workflowMembersToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.conditionsDropDown = new System.Windows.Forms.ToolStripDropDownButton();
            this.addConditionButton = new System.Windows.Forms.ToolStripMenuItem();
            this.matchDerivedTypes = new System.Windows.Forms.ToolStripButton();
            this.openWorkflowDialog = new System.Windows.Forms.OpenFileDialog();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.openProfileDialog = new System.Windows.Forms.OpenFileDialog();
            this.openAssemblyDialog = new System.Windows.Forms.OpenFileDialog();
            this.menuStrip1.SuspendLayout();
            this.panel1.SuspendLayout();
            this.conditionToolStrip.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.annotationToolStrip.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem1});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(734, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem1
            // 
            this.fileToolStripMenuItem1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openToolStripMenuItem,
            this.saveToolStripMenuItem1,
            this.exitToolStripMenuItem1});
            this.fileToolStripMenuItem1.Name = "fileToolStripMenuItem1";
            this.fileToolStripMenuItem1.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem1.Text = "&File";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fromFileToolStripMenuItem,
            this.profileToolStripMenuItem,
            this.fromSqlToolStripMenuItem});
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.openToolStripMenuItem.Text = "&Open";
            // 
            // fromFileToolStripMenuItem
            // 
            this.fromFileToolStripMenuItem.Name = "fromFileToolStripMenuItem";
            this.fromFileToolStripMenuItem.Size = new System.Drawing.Size(212, 22);
            this.fromFileToolStripMenuItem.Text = "&Workflow From File";
            this.fromFileToolStripMenuItem.Click += new System.EventHandler(this.loadWorkflowToolStripMenuItem_Click);
            // 
            // profileToolStripMenuItem
            // 
            this.profileToolStripMenuItem.Name = "profileToolStripMenuItem";
            this.profileToolStripMenuItem.Size = new System.Drawing.Size(212, 22);
            this.profileToolStripMenuItem.Text = "&Profile From File";
            this.profileToolStripMenuItem.Click += new System.EventHandler(this.loadProfileToolStripMenuItem_Click);
            // 
            // fromSqlToolStripMenuItem
            // 
            this.fromSqlToolStripMenuItem.Name = "fromSqlToolStripMenuItem";
            this.fromSqlToolStripMenuItem.Size = new System.Drawing.Size(212, 22);
            this.fromSqlToolStripMenuItem.Text = "From &SQL Tracking Database";
            this.fromSqlToolStripMenuItem.Click += new System.EventHandler(this.fromSqlToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem1
            // 
            this.saveToolStripMenuItem1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.profileAsFileToolStripMenuItem,
            this.toSqlTrackingStoreToolStripMenuItem});
            this.saveToolStripMenuItem1.Name = "saveToolStripMenuItem1";
            this.saveToolStripMenuItem1.Size = new System.Drawing.Size(152, 22);
            this.saveToolStripMenuItem1.Text = "&Save";
            // 
            // profileAsFileToolStripMenuItem
            // 
            this.profileAsFileToolStripMenuItem.Name = "profileAsFileToolStripMenuItem";
            this.profileAsFileToolStripMenuItem.Size = new System.Drawing.Size(231, 22);
            this.profileAsFileToolStripMenuItem.Text = "Profile as &File";
            this.profileAsFileToolStripMenuItem.Click += new System.EventHandler(this.saveProfileToolStripMenuItem_Click);
            // 
            // toSqlTrackingStoreToolStripMenuItem
            // 
            this.toSqlTrackingStoreToolStripMenuItem.Name = "toSqlTrackingStoreToolStripMenuItem";
            this.toSqlTrackingStoreToolStripMenuItem.Size = new System.Drawing.Size(231, 22);
            this.toSqlTrackingStoreToolStripMenuItem.Text = "Profile to &SQL Tracking Database";
            this.toSqlTrackingStoreToolStripMenuItem.Click += new System.EventHandler(this.saveProfileToSql_Click);
            // 
            // exitToolStripMenuItem1
            // 
            this.exitToolStripMenuItem1.Name = "exitToolStripMenuItem1";
            this.exitToolStripMenuItem1.Size = new System.Drawing.Size(152, 22);
            this.exitToolStripMenuItem1.Text = "E&xit";
            this.exitToolStripMenuItem1.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.loadWorkflowToolStripMenuItem,
            this.saveProfileToolStripMenuItem,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // loadWorkflowToolStripMenuItem
            // 
            this.loadWorkflowToolStripMenuItem.Name = "loadWorkflowToolStripMenuItem";
            this.loadWorkflowToolStripMenuItem.Size = new System.Drawing.Size(145, 22);
            this.loadWorkflowToolStripMenuItem.Text = "&Load Workflow";
            this.loadWorkflowToolStripMenuItem.Click += new System.EventHandler(this.loadWorkflowToolStripMenuItem_Click);
            // 
            // saveProfileToolStripMenuItem
            // 
            this.saveProfileToolStripMenuItem.Name = "saveProfileToolStripMenuItem";
            this.saveProfileToolStripMenuItem.Size = new System.Drawing.Size(145, 22);
            this.saveProfileToolStripMenuItem.Text = "&Save Profile";
            this.saveProfileToolStripMenuItem.Click += new System.EventHandler(this.saveProfileToolStripMenuItem_Click);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(145, 22);
            this.exitToolStripMenuItem.Text = "E&xit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // panel1
            // 
            this.panel1.AutoSize = true;
            this.panel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.panel1.Controls.Add(this.conditionToolStrip);
            this.panel1.Controls.Add(this.tabControl1);
            this.panel1.Controls.Add(this.annotationToolStrip);
            this.panel1.Controls.Add(this.toolStrip1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 24);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(734, 470);
            this.panel1.TabIndex = 1;
            // 
            // conditionToolStrip
            // 
            this.conditionToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.conditionToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.conditionMemberDropDown,
            this.conditionEqualityButton,
            this.conditionValue,
            this.conditionSaveButton,
            this.conditionCancelButton,
            this.conditionDeleteButton});
            this.conditionToolStrip.Location = new System.Drawing.Point(0, 50);
            this.conditionToolStrip.Name = "conditionToolStrip";
            this.conditionToolStrip.Size = new System.Drawing.Size(734, 25);
            this.conditionToolStrip.TabIndex = 6;
            this.conditionToolStrip.Text = "toolStrip2";
            // 
            // conditionMemberDropDown
            // 
            this.conditionMemberDropDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.conditionMemberDropDown.Image = ((System.Drawing.Image)(resources.GetObject("conditionMemberDropDown.Image")));
            this.conditionMemberDropDown.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.conditionMemberDropDown.Name = "conditionMemberDropDown";
            this.conditionMemberDropDown.Size = new System.Drawing.Size(90, 22);
            this.conditionMemberDropDown.Text = "Select member";
            this.conditionMemberDropDown.DropDownItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.conditionMemberDropDown_DropDownItemClicked);
            // 
            // conditionEqualityButton
            // 
            this.conditionEqualityButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.conditionEqualityButton.Image = ((System.Drawing.Image)(resources.GetObject("conditionEqualityButton.Image")));
            this.conditionEqualityButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.conditionEqualityButton.Name = "conditionEqualityButton";
            this.conditionEqualityButton.Size = new System.Drawing.Size(27, 22);
            this.conditionEqualityButton.Text = "==";
            this.conditionEqualityButton.Click += new System.EventHandler(this.conditionEqualityButton_Click);
            // 
            // conditionValue
            // 
            this.conditionValue.Name = "conditionValue";
            this.conditionValue.Size = new System.Drawing.Size(100, 25);
            // 
            // conditionSaveButton
            // 
            this.conditionSaveButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.conditionSaveButton.Image = ((System.Drawing.Image)(resources.GetObject("conditionSaveButton.Image")));
            this.conditionSaveButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.conditionSaveButton.Name = "conditionSaveButton";
            this.conditionSaveButton.Size = new System.Drawing.Size(35, 22);
            this.conditionSaveButton.Text = "Save";
            this.conditionSaveButton.Click += new System.EventHandler(this.conditionSaveButton_Click);
            // 
            // conditionCancelButton
            // 
            this.conditionCancelButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.conditionCancelButton.Image = ((System.Drawing.Image)(resources.GetObject("conditionCancelButton.Image")));
            this.conditionCancelButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.conditionCancelButton.Name = "conditionCancelButton";
            this.conditionCancelButton.Size = new System.Drawing.Size(43, 22);
            this.conditionCancelButton.Text = "Cancel";
            this.conditionCancelButton.Click += new System.EventHandler(this.conditionCancelButton_Click);
            // 
            // conditionDeleteButton
            // 
            this.conditionDeleteButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.conditionDeleteButton.Image = ((System.Drawing.Image)(resources.GetObject("conditionDeleteButton.Image")));
            this.conditionDeleteButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.conditionDeleteButton.Name = "conditionDeleteButton";
            this.conditionDeleteButton.Size = new System.Drawing.Size(42, 22);
            this.conditionDeleteButton.Text = "Delete";
            this.conditionDeleteButton.Click += new System.EventHandler(this.conditionDeleteButton_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 50);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(734, 420);
            this.tabControl1.TabIndex = 4;
            this.tabControl1.Selected += new System.Windows.Forms.TabControlEventHandler(this.tabControl1_Selected);
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.workflowDesignerControl1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(726, 394);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Tracking Profile Designer";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // workflowDesignerControl1
            // 
            this.workflowDesignerControl1.AutoSize = true;
            this.workflowDesignerControl1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.workflowDesignerControl1.BackColor = System.Drawing.SystemColors.Control;
            this.workflowDesignerControl1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.workflowDesignerControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.workflowDesignerControl1.Location = new System.Drawing.Point(3, 3);
            this.workflowDesignerControl1.Name = "workflowDesignerControl1";
            this.workflowDesignerControl1.Size = new System.Drawing.Size(720, 388);
            this.workflowDesignerControl1.TabIndex = 1;
            this.workflowDesignerControl1.WorkflowType = null;
            this.workflowDesignerControl1.Xaml = "";
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.profileMarkup);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(726, 394);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Tracking Profile Markup";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // profileMarkup
            // 
            this.profileMarkup.Dock = System.Windows.Forms.DockStyle.Fill;
            this.profileMarkup.Location = new System.Drawing.Point(3, 3);
            this.profileMarkup.Name = "profileMarkup";
            this.profileMarkup.ReadOnly = true;
            this.profileMarkup.Size = new System.Drawing.Size(720, 388);
            this.profileMarkup.TabIndex = 0;
            this.profileMarkup.Text = "";
            // 
            // annotationToolStrip
            // 
            this.annotationToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.annotationToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.annotationText,
            this.annotateSaveButton,
            this.annotateCancelButton});
            this.annotationToolStrip.Location = new System.Drawing.Point(0, 25);
            this.annotationToolStrip.Name = "annotationToolStrip";
            this.annotationToolStrip.Size = new System.Drawing.Size(734, 25);
            this.annotationToolStrip.TabIndex = 5;
            this.annotationToolStrip.Text = "annotateToolStrip";
            // 
            // annotationText
            // 
            this.annotationText.AutoSize = false;
            this.annotationText.Name = "annotationText";
            this.annotationText.Size = new System.Drawing.Size(300, 25);
            // 
            // annotateSaveButton
            // 
            this.annotateSaveButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.annotateSaveButton.Image = ((System.Drawing.Image)(resources.GetObject("annotateSaveButton.Image")));
            this.annotateSaveButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.annotateSaveButton.Name = "annotateSaveButton";
            this.annotateSaveButton.Size = new System.Drawing.Size(35, 22);
            this.annotateSaveButton.Text = "Save";
            this.annotateSaveButton.Click += new System.EventHandler(this.annotateSaveButton_Click);
            // 
            // annotateCancelButton
            // 
            this.annotateCancelButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.annotateCancelButton.Image = ((System.Drawing.Image)(resources.GetObject("annotateCancelButton.Image")));
            this.annotateCancelButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.annotateCancelButton.Name = "annotateCancelButton";
            this.annotateCancelButton.Size = new System.Drawing.Size(43, 22);
            this.annotateCancelButton.Text = "Cancel";
            this.annotateCancelButton.Click += new System.EventHandler(this.annotateCancelButton_Click);
            // 
            // toolStrip1
            // 
            this.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.trackButton,
            this.toolStripSeparator2,
            this.eventsDropDown,
            this.workflowEventsDropDown,
            this.eventsSeparator,
            this.annotateButton,
            this.extractDropDown,
            this.conditionsDropDown,
            this.matchDerivedTypes});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(734, 25);
            this.toolStrip1.TabIndex = 3;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // trackButton
            // 
            this.trackButton.Image = global::Microsoft.Samples.Workflow.TrackingProfileDesigner.Properties.Resources.track;
            this.trackButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.trackButton.Name = "trackButton";
            this.trackButton.Size = new System.Drawing.Size(53, 22);
            this.trackButton.Text = "Track";
            this.trackButton.Click += new System.EventHandler(this.trackButton_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // eventsDropDown
            // 
            this.eventsDropDown.Image = ((System.Drawing.Image)(resources.GetObject("eventsDropDown.Image")));
            this.eventsDropDown.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.eventsDropDown.Name = "eventsDropDown";
            this.eventsDropDown.Size = new System.Drawing.Size(108, 22);
            this.eventsDropDown.Text = "Activity Events";
            this.eventsDropDown.Click += new System.EventHandler(this.eventsDropDown_Click);
            // 
            // workflowEventsDropDown
            // 
            this.workflowEventsDropDown.Image = ((System.Drawing.Image)(resources.GetObject("workflowEventsDropDown.Image")));
            this.workflowEventsDropDown.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.workflowEventsDropDown.Name = "workflowEventsDropDown";
            this.workflowEventsDropDown.Size = new System.Drawing.Size(117, 22);
            this.workflowEventsDropDown.Text = "Workflow Events";
            this.workflowEventsDropDown.ToolTipText = "Select the events that are tracked for this workflow.";
            this.workflowEventsDropDown.Click += new System.EventHandler(this.workflowEventsDropDown_Click);
            // 
            // eventsSeparator
            // 
            this.eventsSeparator.Name = "eventsSeparator";
            this.eventsSeparator.Size = new System.Drawing.Size(6, 25);
            // 
            // annotateButton
            // 
            this.annotateButton.Image = ((System.Drawing.Image)(resources.GetObject("annotateButton.Image")));
            this.annotateButton.ImageTransparentColor = System.Drawing.Color.White;
            this.annotateButton.Name = "annotateButton";
            this.annotateButton.Size = new System.Drawing.Size(72, 22);
            this.annotateButton.Text = "Annotate";
            this.annotateButton.Click += new System.EventHandler(this.annotateButton_Click);
            // 
            // extractDropDown
            // 
            this.extractDropDown.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.activityMembersToolStripMenuItem,
            this.workflowMembersToolStripMenuItem});
            this.extractDropDown.Image = ((System.Drawing.Image)(resources.GetObject("extractDropDown.Image")));
            this.extractDropDown.ImageTransparentColor = System.Drawing.Color.White;
            this.extractDropDown.Name = "extractDropDown";
            this.extractDropDown.Size = new System.Drawing.Size(97, 22);
            this.extractDropDown.Text = "Extract Data";
            // 
            // activityMembersToolStripMenuItem
            // 
            this.activityMembersToolStripMenuItem.Name = "activityMembersToolStripMenuItem";
            this.activityMembersToolStripMenuItem.Size = new System.Drawing.Size(119, 22);
            this.activityMembersToolStripMenuItem.Text = "&Activity";
            // 
            // workflowMembersToolStripMenuItem
            // 
            this.workflowMembersToolStripMenuItem.Name = "workflowMembersToolStripMenuItem";
            this.workflowMembersToolStripMenuItem.Size = new System.Drawing.Size(119, 22);
            this.workflowMembersToolStripMenuItem.Text = "&Workflow";
            // 
            // conditionsDropDown
            // 
            this.conditionsDropDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.conditionsDropDown.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addConditionButton});
            this.conditionsDropDown.Image = ((System.Drawing.Image)(resources.GetObject("conditionsDropDown.Image")));
            this.conditionsDropDown.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.conditionsDropDown.Name = "conditionsDropDown";
            this.conditionsDropDown.Size = new System.Drawing.Size(70, 22);
            this.conditionsDropDown.Text = "Conditions";
            this.conditionsDropDown.DropDownItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.conditionsDropDown_DropDownItemClicked);
            // 
            // addConditionButton
            // 
            this.addConditionButton.Name = "addConditionButton";
            this.addConditionButton.Size = new System.Drawing.Size(174, 22);
            this.addConditionButton.Text = "Add new condition...";
            // 
            // matchDerivedTypes
            // 
            this.matchDerivedTypes.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.matchDerivedTypes.Image = ((System.Drawing.Image)(resources.GetObject("matchDerivedTypes.Image")));
            this.matchDerivedTypes.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.matchDerivedTypes.Name = "matchDerivedTypes";
            this.matchDerivedTypes.Size = new System.Drawing.Size(112, 22);
            this.matchDerivedTypes.Text = "Match Derived Types";
            this.matchDerivedTypes.Visible = false;
            this.matchDerivedTypes.Click += new System.EventHandler(this.matchDerivedTypes_Click);
            // 
            // openWorkflowDialog
            // 
            this.openWorkflowDialog.Filter = "Workflow libraries (*.dll)|*.dll|Workflow executables (*.exe)|*.exe|All files|*.*";
            this.openWorkflowDialog.Title = "Choose Workflow";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // saveFileDialog1
            // 
            this.saveFileDialog1.Filter = "Workflow Tracking Profiles (*.xml) | *.xml|All files|*.*";
            // 
            // openProfileDialog
            // 
            this.openProfileDialog.Filter = "Workflow Tracking Profiles (*.xml)|*.xml";
            // 
            // openAssemblyDialog
            // 
            this.openAssemblyDialog.Filter = "Assembly (*.dll) | *.dll";
            // 
            // WorkflowTrackingProfileDesignerForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(734, 494);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "WorkflowTrackingProfileDesignerForm";
            this.Text = "Workflow Tracking Profile Designer";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.conditionToolStrip.ResumeLayout(false);
            this.conditionToolStrip.PerformLayout();
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.annotationToolStrip.ResumeLayout(false);
            this.annotationToolStrip.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem loadWorkflowToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveProfileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.OpenFileDialog openWorkflowDialog;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem1;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private DesignerControl.WorkflowDesignerControl workflowDesignerControl1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.RichTextBox profileMarkup;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton trackButton;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripDropDownButton workflowEventsDropDown;
        private System.Windows.Forms.ToolStripSeparator eventsSeparator;
        private System.Windows.Forms.ToolStripButton annotateButton;
        private System.Windows.Forms.ToolStripDropDownButton extractDropDown;
        private System.Windows.Forms.ToolStripButton matchDerivedTypes;
        private System.Windows.Forms.OpenFileDialog openProfileDialog;
        private System.Windows.Forms.ToolStripDropDownButton conditionsDropDown;
        private System.Windows.Forms.ToolStripMenuItem addConditionButton;
        private System.Windows.Forms.ToolStrip annotationToolStrip;
        private System.Windows.Forms.ToolStripTextBox annotationText;
        private System.Windows.Forms.ToolStripButton annotateSaveButton;
        private System.Windows.Forms.ToolStripButton annotateCancelButton;
        private System.Windows.Forms.ToolStrip conditionToolStrip;
        private System.Windows.Forms.ToolStripDropDownButton conditionMemberDropDown;
        private System.Windows.Forms.ToolStripButton conditionEqualityButton;
        private System.Windows.Forms.ToolStripTextBox conditionValue;
        private System.Windows.Forms.ToolStripButton conditionSaveButton;
        private System.Windows.Forms.ToolStripButton conditionCancelButton;
        private System.Windows.Forms.ToolStripButton conditionDeleteButton;
        private System.Windows.Forms.OpenFileDialog openAssemblyDialog;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem fromFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem profileAsFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem profileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem fromSqlToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toSqlTrackingStoreToolStripMenuItem;
        private System.Windows.Forms.ToolStripDropDownButton eventsDropDown;
        private System.Windows.Forms.ToolStripMenuItem activityMembersToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem workflowMembersToolStripMenuItem;
        
    }
}

