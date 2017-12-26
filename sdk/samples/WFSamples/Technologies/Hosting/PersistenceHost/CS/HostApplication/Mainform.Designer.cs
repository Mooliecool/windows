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

namespace Microsoft.Samples.Workflow.PersistenceHost
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
            this.GroupBoxCreateNewRequest = new System.Windows.Forms.GroupBox();
            this.ButtonCreateNewRequest = new System.Windows.Forms.Button();
            this.TextBoxApprover = new System.Windows.Forms.TextBox();
            this.Label1 = new System.Windows.Forms.Label();
            this.GroupBoxExistingRequests = new System.Windows.Forms.GroupBox();
            this.ListViewExistingRequests = new System.Windows.Forms.ListView();
            this.ColumnHeaderApprover = new System.Windows.Forms.ColumnHeader();
            this.ColumnHeaderRequestID = new System.Windows.Forms.ColumnHeader();
            this.ContextMenuListViewExistingWorkflows = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.ApproveDocumentToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.GroupBoxCreateNewRequest.SuspendLayout();
            this.GroupBoxExistingRequests.SuspendLayout();
            this.ContextMenuListViewExistingWorkflows.SuspendLayout();
            this.SuspendLayout();
            // 
            // GroupBoxCreateNewRequest
            // 
            this.GroupBoxCreateNewRequest.Controls.Add(this.ButtonCreateNewRequest);
            this.GroupBoxCreateNewRequest.Controls.Add(this.TextBoxApprover);
            this.GroupBoxCreateNewRequest.Controls.Add(this.Label1);
            this.GroupBoxCreateNewRequest.Location = new System.Drawing.Point(13, 13);
            this.GroupBoxCreateNewRequest.Name = "GroupBoxCreateNewRequest";
            this.GroupBoxCreateNewRequest.Size = new System.Drawing.Size(380, 71);
            this.GroupBoxCreateNewRequest.TabIndex = 0;
            this.GroupBoxCreateNewRequest.TabStop = false;
            this.GroupBoxCreateNewRequest.Text = "Create New Request";
            // 
            // ButtonCreateNewRequest
            // 
            this.ButtonCreateNewRequest.Location = new System.Drawing.Point(256, 35);
            this.ButtonCreateNewRequest.Name = "ButtonCreateNewRequest";
            this.ButtonCreateNewRequest.Size = new System.Drawing.Size(118, 23);
            this.ButtonCreateNewRequest.TabIndex = 2;
            this.ButtonCreateNewRequest.Text = "Create";
            this.ButtonCreateNewRequest.UseVisualStyleBackColor = true;
            this.ButtonCreateNewRequest.Click += new System.EventHandler(this.ButtonCreateNewRequest_Click);
            // 
            // TextBoxApprover
            // 
            this.TextBoxApprover.Location = new System.Drawing.Point(10, 37);
            this.TextBoxApprover.Name = "TextBoxApprover";
            this.TextBoxApprover.Size = new System.Drawing.Size(240, 20);
            this.TextBoxApprover.TabIndex = 0;
            // 
            // Label1
            // 
            this.Label1.AutoSize = true;
            this.Label1.Location = new System.Drawing.Point(7, 20);
            this.Label1.Name = "Label1";
            this.Label1.Size = new System.Drawing.Size(50, 13);
            this.Label1.TabIndex = 0;
            this.Label1.Text = "Approver";
            // 
            // GroupBoxExistingRequests
            // 
            this.GroupBoxExistingRequests.Controls.Add(this.ListViewExistingRequests);
            this.GroupBoxExistingRequests.Location = new System.Drawing.Point(13, 91);
            this.GroupBoxExistingRequests.Name = "GroupBoxExistingRequests";
            this.GroupBoxExistingRequests.Size = new System.Drawing.Size(380, 250);
            this.GroupBoxExistingRequests.TabIndex = 1;
            this.GroupBoxExistingRequests.TabStop = false;
            this.GroupBoxExistingRequests.Text = "Existing Requests";
            // 
            // ListViewExistingRequests
            // 
            this.ListViewExistingRequests.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.ColumnHeaderApprover,
            this.ColumnHeaderRequestID});
            this.ListViewExistingRequests.ContextMenuStrip = this.ContextMenuListViewExistingWorkflows;
            this.ListViewExistingRequests.FullRowSelect = true;
            this.ListViewExistingRequests.Location = new System.Drawing.Point(7, 20);
            this.ListViewExistingRequests.MultiSelect = false;
            this.ListViewExistingRequests.Name = "ListViewExistingRequests";
            this.ListViewExistingRequests.Size = new System.Drawing.Size(367, 224);
            this.ListViewExistingRequests.TabIndex = 0;
            this.ListViewExistingRequests.UseCompatibleStateImageBehavior = false;
            this.ListViewExistingRequests.View = System.Windows.Forms.View.Details;
            // 
            // ColumnHeaderApprover
            // 
            this.ColumnHeaderApprover.Text = "Approver";
            this.ColumnHeaderApprover.Width = 111;
            // 
            // ColumnHeaderRequestID
            // 
            this.ColumnHeaderRequestID.Text = "Request ID";
            this.ColumnHeaderRequestID.Width = 243;
            // 
            // ContextMenuListViewExistingWorkflows
            // 
            this.ContextMenuListViewExistingWorkflows.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ApproveDocumentToolStripMenuItem});
            this.ContextMenuListViewExistingWorkflows.Name = "ContextMenuListViewExistingWorkflows";
            this.ContextMenuListViewExistingWorkflows.Size = new System.Drawing.Size(167, 26);
            // 
            // ApproveDocumentToolStripMenuItem
            // 
            this.ApproveDocumentToolStripMenuItem.Name = "ApproveDocumentToolStripMenuItem";
            this.ApproveDocumentToolStripMenuItem.Size = new System.Drawing.Size(166, 22);
            this.ApproveDocumentToolStripMenuItem.Text = "Approve Document";
            this.ApproveDocumentToolStripMenuItem.Click += new System.EventHandler(this.ApproveDocumentToolStripMenuItem_Click);
            // 
            // Mainform
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(405, 353);
            this.Controls.Add(this.GroupBoxExistingRequests);
            this.Controls.Add(this.GroupBoxCreateNewRequest);
            this.Name = "Mainform";
            this.Text = "Document Approval";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Mainform_FormClosing);
            this.GroupBoxCreateNewRequest.ResumeLayout(false);
            this.GroupBoxCreateNewRequest.PerformLayout();
            this.GroupBoxExistingRequests.ResumeLayout(false);
            this.ContextMenuListViewExistingWorkflows.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox GroupBoxCreateNewRequest;
        private System.Windows.Forms.Button ButtonCreateNewRequest;
        private System.Windows.Forms.TextBox TextBoxApprover;
        private System.Windows.Forms.Label Label1;
        private System.Windows.Forms.GroupBox GroupBoxExistingRequests;
        private System.Windows.Forms.ListView ListViewExistingRequests;
        private System.Windows.Forms.ColumnHeader ColumnHeaderRequestID;
        private System.Windows.Forms.ColumnHeader ColumnHeaderApprover;
        private System.Windows.Forms.ContextMenuStrip ContextMenuListViewExistingWorkflows;
        private System.Windows.Forms.ToolStripMenuItem ApproveDocumentToolStripMenuItem;
    }
}

