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

namespace Microsoft.Samples.Workflow.OutlookWorkflowWizard
{
    partial class WizardForm
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
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.outboxRadioButton = new System.Windows.Forms.RadioButton();
            this.sentRadioButton = new System.Windows.Forms.RadioButton();
            this.inboxRadioButton = new System.Windows.Forms.RadioButton();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.ccRadioButton = new System.Windows.Forms.RadioButton();
            this.toRadioButton = new System.Windows.Forms.RadioButton();
            this.bccRadioButton = new System.Windows.Forms.RadioButton();
            this.fromRadioButton = new System.Windows.Forms.RadioButton();
            this.subjectRadioButton = new System.Windows.Forms.RadioButton();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.actionsCheckedListBox = new System.Windows.Forms.CheckedListBox();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.generateWorkflowButton = new System.Windows.Forms.Button();
            this.compileWorkflowButton = new System.Windows.Forms.Button();
            this.startWorkflowButton = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.ifParameterLinkLabel = new System.Windows.Forms.LinkLabel();
            this.actionsSelectionLabel = new System.Windows.Forms.Label();
            this.ifParameterField = new System.Windows.Forms.TextBox();
            this.conditionSelectionLabel = new System.Windows.Forms.Label();
            this.folderSelectionLabel = new System.Windows.Forms.Label();
            this.nextStepButton = new System.Windows.Forms.Button();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabPage5.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage5);
            this.tabControl1.Location = new System.Drawing.Point(6, 29);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(318, 146);
            this.tabControl1.TabIndex = 0;
            this.tabControl1.SelectedIndexChanged += new System.EventHandler(this.tabControl1_SelectedIndexChanged);
            // 
            // tabPage1
            // 
            this.tabPage1.BackColor = System.Drawing.SystemColors.Control;
            this.tabPage1.Controls.Add(this.outboxRadioButton);
            this.tabPage1.Controls.Add(this.sentRadioButton);
            this.tabPage1.Controls.Add(this.inboxRadioButton);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(310, 120);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Folder";
            // 
            // outboxRadioButton
            // 
            this.outboxRadioButton.AutoSize = true;
            this.outboxRadioButton.Location = new System.Drawing.Point(6, 52);
            this.outboxRadioButton.Name = "outboxRadioButton";
            this.outboxRadioButton.Size = new System.Drawing.Size(71, 17);
            this.outboxRadioButton.TabIndex = 2;
            this.outboxRadioButton.TabStop = true;
            this.outboxRadioButton.Text = "In Outbox";
            this.outboxRadioButton.UseVisualStyleBackColor = true;
            this.outboxRadioButton.CheckedChanged += new System.EventHandler(this.FolderSelectionChanged);
            // 
            // sentRadioButton
            // 
            this.sentRadioButton.AutoSize = true;
            this.sentRadioButton.Location = new System.Drawing.Point(6, 29);
            this.sentRadioButton.Name = "sentRadioButton";
            this.sentRadioButton.Size = new System.Drawing.Size(87, 17);
            this.sentRadioButton.TabIndex = 1;
            this.sentRadioButton.TabStop = true;
            this.sentRadioButton.Text = "In Sent Items";
            this.sentRadioButton.UseVisualStyleBackColor = true;
            this.sentRadioButton.CheckedChanged += new System.EventHandler(this.FolderSelectionChanged);
            // 
            // inboxRadioButton
            // 
            this.inboxRadioButton.AutoSize = true;
            this.inboxRadioButton.Location = new System.Drawing.Point(6, 6);
            this.inboxRadioButton.Name = "inboxRadioButton";
            this.inboxRadioButton.Size = new System.Drawing.Size(63, 17);
            this.inboxRadioButton.TabIndex = 0;
            this.inboxRadioButton.TabStop = true;
            this.inboxRadioButton.Text = "In Inbox";
            this.inboxRadioButton.UseVisualStyleBackColor = true;
            this.inboxRadioButton.CheckedChanged += new System.EventHandler(this.FolderSelectionChanged);
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.SystemColors.Control;
            this.tabPage2.Controls.Add(this.ccRadioButton);
            this.tabPage2.Controls.Add(this.toRadioButton);
            this.tabPage2.Controls.Add(this.bccRadioButton);
            this.tabPage2.Controls.Add(this.fromRadioButton);
            this.tabPage2.Controls.Add(this.subjectRadioButton);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(310, 120);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Condition";
            // 
            // ccRadioButton
            // 
            this.ccRadioButton.AutoSize = true;
            this.ccRadioButton.Location = new System.Drawing.Point(6, 75);
            this.ccRadioButton.Name = "ccRadioButton";
            this.ccRadioButton.Size = new System.Drawing.Size(82, 17);
            this.ccRadioButton.TabIndex = 3;
            this.ccRadioButton.Text = "If CC equals";
            this.ccRadioButton.UseVisualStyleBackColor = true;
            this.ccRadioButton.CheckedChanged += new System.EventHandler(this.ConditionSelectionChanged);
            // 
            // toRadioButton
            // 
            this.toRadioButton.AutoSize = true;
            this.toRadioButton.Location = new System.Drawing.Point(6, 52);
            this.toRadioButton.Name = "toRadioButton";
            this.toRadioButton.Size = new System.Drawing.Size(81, 17);
            this.toRadioButton.TabIndex = 2;
            this.toRadioButton.Text = "If To equals";
            this.toRadioButton.UseVisualStyleBackColor = true;
            this.toRadioButton.CheckedChanged += new System.EventHandler(this.ConditionSelectionChanged);
            // 
            // bccRadioButton
            // 
            this.bccRadioButton.AutoSize = true;
            this.bccRadioButton.Location = new System.Drawing.Point(6, 98);
            this.bccRadioButton.Name = "bccRadioButton";
            this.bccRadioButton.Size = new System.Drawing.Size(89, 17);
            this.bccRadioButton.TabIndex = 4;
            this.bccRadioButton.Text = "If BCC equals";
            this.bccRadioButton.UseVisualStyleBackColor = true;
            this.bccRadioButton.CheckedChanged += new System.EventHandler(this.ConditionSelectionChanged);
            // 
            // fromRadioButton
            // 
            this.fromRadioButton.AutoSize = true;
            this.fromRadioButton.Location = new System.Drawing.Point(6, 29);
            this.fromRadioButton.Name = "fromRadioButton";
            this.fromRadioButton.Size = new System.Drawing.Size(91, 17);
            this.fromRadioButton.TabIndex = 1;
            this.fromRadioButton.Text = "If From equals";
            this.fromRadioButton.UseVisualStyleBackColor = true;
            this.fromRadioButton.CheckedChanged += new System.EventHandler(this.ConditionSelectionChanged);
            // 
            // subjectRadioButton
            // 
            this.subjectRadioButton.AutoSize = true;
            this.subjectRadioButton.Location = new System.Drawing.Point(6, 6);
            this.subjectRadioButton.Name = "subjectRadioButton";
            this.subjectRadioButton.Size = new System.Drawing.Size(104, 17);
            this.subjectRadioButton.TabIndex = 0;
            this.subjectRadioButton.Text = "If Subject equals";
            this.subjectRadioButton.UseVisualStyleBackColor = true;
            this.subjectRadioButton.CheckedChanged += new System.EventHandler(this.ConditionSelectionChanged);
            // 
            // tabPage3
            // 
            this.tabPage3.BackColor = System.Drawing.SystemColors.Control;
            this.tabPage3.Controls.Add(this.actionsCheckedListBox);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage3.Size = new System.Drawing.Size(310, 120);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Actions";
            // 
            // actionsCheckedListBox
            // 
            this.actionsCheckedListBox.BackColor = System.Drawing.SystemColors.Control;
            this.actionsCheckedListBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.actionsCheckedListBox.CheckOnClick = true;
            this.actionsCheckedListBox.FormattingEnabled = true;
            this.actionsCheckedListBox.Items.AddRange(new object[] {
            "Send Auto-Reply Email",
            "Create Outlook Note",
            "Create Outlook Task"});
            this.actionsCheckedListBox.Location = new System.Drawing.Point(4, 7);
            this.actionsCheckedListBox.Name = "actionsCheckedListBox";
            this.actionsCheckedListBox.Size = new System.Drawing.Size(300, 45);
            this.actionsCheckedListBox.TabIndex = 0;
            this.actionsCheckedListBox.SelectedIndexChanged += new System.EventHandler(this.ActionSelectionChanged);
            // 
            // tabPage5
            // 
            this.tabPage5.Controls.Add(this.generateWorkflowButton);
            this.tabPage5.Controls.Add(this.compileWorkflowButton);
            this.tabPage5.Controls.Add(this.startWorkflowButton);
            this.tabPage5.Location = new System.Drawing.Point(4, 22);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage5.Size = new System.Drawing.Size(310, 120);
            this.tabPage5.TabIndex = 4;
            this.tabPage5.Text = "Execute";
            // 
            // generateWorkflowButton
            // 
            this.generateWorkflowButton.Location = new System.Drawing.Point(75, 6);
            this.generateWorkflowButton.Name = "generateWorkflowButton";
            this.generateWorkflowButton.Size = new System.Drawing.Size(160, 23);
            this.generateWorkflowButton.TabIndex = 0;
            this.generateWorkflowButton.Text = "Generate Workflow Definition";
            this.generateWorkflowButton.Click += new System.EventHandler(this.GenerateWorkflowButton);
            // 
            // compileWorkflowButton
            // 
            this.compileWorkflowButton.Enabled = false;
            this.compileWorkflowButton.Location = new System.Drawing.Point(75, 49);
            this.compileWorkflowButton.Name = "compileWorkflowButton";
            this.compileWorkflowButton.Size = new System.Drawing.Size(160, 23);
            this.compileWorkflowButton.TabIndex = 1;
            this.compileWorkflowButton.Text = "Compile Workflow Definition";
            this.compileWorkflowButton.Click += new System.EventHandler(this.CompileWorkflowButton);
            // 
            // startWorkflowButton
            // 
            this.startWorkflowButton.Enabled = false;
            this.startWorkflowButton.Location = new System.Drawing.Point(75, 91);
            this.startWorkflowButton.Name = "startWorkflowButton";
            this.startWorkflowButton.Size = new System.Drawing.Size(160, 23);
            this.startWorkflowButton.TabIndex = 2;
            this.startWorkflowButton.Text = "Start Workflow Definition";
            this.startWorkflowButton.Click += new System.EventHandler(this.StartWorkflowButton_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.BackColor = System.Drawing.Color.Transparent;
            this.groupBox1.Controls.Add(this.ifParameterLinkLabel);
            this.groupBox1.Controls.Add(this.actionsSelectionLabel);
            this.groupBox1.Controls.Add(this.ifParameterField);
            this.groupBox1.Controls.Add(this.conditionSelectionLabel);
            this.groupBox1.Controls.Add(this.folderSelectionLabel);
            this.groupBox1.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.groupBox1.ForeColor = System.Drawing.Color.Black;
            this.groupBox1.Location = new System.Drawing.Point(6, 179);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(1);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(1);
            this.groupBox1.Size = new System.Drawing.Size(318, 124);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Workflow Description";
            // 
            // ifParameterLinkLabel
            // 
            this.ifParameterLinkLabel.AutoSize = true;
            this.ifParameterLinkLabel.Location = new System.Drawing.Point(90, 44);
            this.ifParameterLinkLabel.Name = "ifParameterLinkLabel";
            this.ifParameterLinkLabel.Size = new System.Drawing.Size(49, 13);
            this.ifParameterLinkLabel.TabIndex = 2;
            this.ifParameterLinkLabel.TabStop = true;
            this.ifParameterLinkLabel.Text = "linkLabel";
            this.ifParameterLinkLabel.UseMnemonic = false;
            this.ifParameterLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.ifParameterLinkLabel_Click);
            // 
            // actionsSelectionLabel
            // 
            this.actionsSelectionLabel.ForeColor = System.Drawing.Color.DarkGreen;
            this.actionsSelectionLabel.Location = new System.Drawing.Point(58, 68);
            this.actionsSelectionLabel.Name = "actionsSelectionLabel";
            this.actionsSelectionLabel.Size = new System.Drawing.Size(256, 48);
            this.actionsSelectionLabel.TabIndex = 4;
            this.actionsSelectionLabel.Text = "L3";
            // 
            // ifParameterField
            // 
            this.ifParameterField.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.ifParameterField.Location = new System.Drawing.Point(119, 42);
            this.ifParameterField.Name = "ifParameterField";
            this.ifParameterField.Size = new System.Drawing.Size(172, 20);
            this.ifParameterField.TabIndex = 3;
            this.ifParameterField.Visible = false;
            this.ifParameterField.Leave += new System.EventHandler(this.ifParameterField_Leave);
            this.ifParameterField.KeyDown += new System.Windows.Forms.KeyEventHandler(this.ifParameterField_KeyDown);
            // 
            // conditionSelectionLabel
            // 
            this.conditionSelectionLabel.AutoSize = true;
            this.conditionSelectionLabel.ForeColor = System.Drawing.Color.Maroon;
            this.conditionSelectionLabel.Location = new System.Drawing.Point(34, 44);
            this.conditionSelectionLabel.Name = "conditionSelectionLabel";
            this.conditionSelectionLabel.Size = new System.Drawing.Size(19, 13);
            this.conditionSelectionLabel.TabIndex = 1;
            this.conditionSelectionLabel.Text = "L2";
            // 
            // folderSelectionLabel
            // 
            this.folderSelectionLabel.AutoSize = true;
            this.folderSelectionLabel.ForeColor = System.Drawing.SystemColors.Desktop;
            this.folderSelectionLabel.Location = new System.Drawing.Point(10, 20);
            this.folderSelectionLabel.Name = "folderSelectionLabel";
            this.folderSelectionLabel.Size = new System.Drawing.Size(19, 13);
            this.folderSelectionLabel.TabIndex = 0;
            this.folderSelectionLabel.Text = "L1";
            // 
            // nextStepButton
            // 
            this.nextStepButton.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.nextStepButton.Location = new System.Drawing.Point(127, 317);
            this.nextStepButton.Name = "nextStepButton";
            this.nextStepButton.Size = new System.Drawing.Size(75, 23);
            this.nextStepButton.TabIndex = 2;
            this.nextStepButton.Text = "Next";
            this.nextStepButton.Click += new System.EventHandler(this.nextStepButton_Click);
            // 
            // splitContainer1
            // 
            this.splitContainer1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.nextStepButton);
            this.splitContainer1.Panel1.Controls.Add(this.tabControl1);
            this.splitContainer1.Panel1.Controls.Add(this.groupBox1);
            this.splitContainer1.Size = new System.Drawing.Size(693, 461);
            this.splitContainer1.SplitterDistance = 331;
            this.splitContainer1.TabIndex = 2;
            this.splitContainer1.Text = "splitContainer1";
            // 
            // WizardForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(693, 461);
            this.Controls.Add(this.splitContainer1);
            this.Name = "WizardForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Workflow Wizard Application";
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.tabPage3.ResumeLayout(false);
            this.tabPage5.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.TabPage tabPage5;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Label actionsSelectionLabel;
        private System.Windows.Forms.Label conditionSelectionLabel;
        private System.Windows.Forms.Label folderSelectionLabel;
        private System.Windows.Forms.Button nextStepButton;
        private System.Windows.Forms.TextBox ifParameterField;
        private System.Windows.Forms.CheckedListBox actionsCheckedListBox;
        private System.Windows.Forms.Button startWorkflowButton;
        private System.Windows.Forms.Button compileWorkflowButton;
        private System.Windows.Forms.RadioButton outboxRadioButton;
        private System.Windows.Forms.RadioButton sentRadioButton;
        private System.Windows.Forms.RadioButton inboxRadioButton;
        private System.Windows.Forms.RadioButton ccRadioButton;
        private System.Windows.Forms.RadioButton toRadioButton;
        private System.Windows.Forms.RadioButton bccRadioButton;
        private System.Windows.Forms.RadioButton fromRadioButton;
        private System.Windows.Forms.RadioButton subjectRadioButton;
        private System.Windows.Forms.LinkLabel ifParameterLinkLabel;
        private System.Windows.Forms.Button generateWorkflowButton;

    }
}

