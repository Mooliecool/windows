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


namespace Microsoft.Samples.Workflow.WorkflowChooser
{
    partial class WorkflowSelectionForm
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
            this.workflowViewPanel = new System.Windows.Forms.Panel();
            this.okButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.workflowListView = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.descriptionHeader = new System.Windows.Forms.Label();
            this.workflowDescription = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // workflowViewPanel
            // 
            this.workflowViewPanel.Location = new System.Drawing.Point(142, 12);
            this.workflowViewPanel.Name = "workflowViewPanel";
            this.workflowViewPanel.Size = new System.Drawing.Size(200, 238);
            this.workflowViewPanel.TabIndex = 0;
            // 
            // okButton
            // 
            this.okButton.Location = new System.Drawing.Point(186, 256);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(75, 23);
            this.okButton.TabIndex = 1;
            this.okButton.Text = "&OK";
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.Location = new System.Drawing.Point(267, 256);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 2;
            this.cancelButton.Text = "&Cancel";
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // workflowListView
            // 
            this.workflowListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
            this.workflowListView.Location = new System.Drawing.Point(13, 13);
            this.workflowListView.MultiSelect = false;
            this.workflowListView.Name = "workflowListView";
            this.workflowListView.Size = new System.Drawing.Size(121, 107);
            this.workflowListView.TabIndex = 3;
            this.workflowListView.UseCompatibleStateImageBehavior = false;
            this.workflowListView.View = System.Windows.Forms.View.Details;
            this.workflowListView.SelectedIndexChanged += new System.EventHandler(this.workflowListView_SelectedIndexChanged);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Name = "columnHeader1";
            this.columnHeader1.Text = "Workflow";
            this.columnHeader1.Width = 116;
            // 
            // descriptionHeader
            // 
            this.descriptionHeader.AutoSize = true;
            this.descriptionHeader.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.5F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.descriptionHeader.Location = new System.Drawing.Point(13, 127);
            this.descriptionHeader.Name = "descriptionHeader";
            this.descriptionHeader.Size = new System.Drawing.Size(88, 15);
            this.descriptionHeader.TabIndex = 4;
            this.descriptionHeader.Text = "Description: ";
            // 
            // workflowDescription
            // 
            this.workflowDescription.Location = new System.Drawing.Point(13, 144);
            this.workflowDescription.Name = "workflowDescription";
            this.workflowDescription.Size = new System.Drawing.Size(111, 106);
            this.workflowDescription.TabIndex = 5;
            this.workflowDescription.Text = "This is the workflow description.";
            this.workflowDescription.Visible = false;
            // 
            // WorkflowSelectionForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(354, 287);
            this.Controls.Add(this.workflowDescription);
            this.Controls.Add(this.descriptionHeader);
            this.Controls.Add(this.workflowListView);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.workflowViewPanel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "WorkflowSelectionForm";
            this.Text = "Choose Workflow...";
            this.Load += new System.EventHandler(this.WorkflowSelectionForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel workflowViewPanel;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.ListView workflowListView;
        private System.Windows.Forms.Label descriptionHeader;
        private System.Windows.Forms.Label workflowDescription;
        private System.Windows.Forms.ColumnHeader columnHeader1;
    }
}