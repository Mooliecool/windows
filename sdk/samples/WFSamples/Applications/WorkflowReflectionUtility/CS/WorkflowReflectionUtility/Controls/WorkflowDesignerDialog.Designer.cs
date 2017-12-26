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

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility.Dialogs
{
    partial class WorkflowDesignerDialog
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
            this.workflowViewPanel1 = new Microsoft.Samples.Workflow.WorkflowReflectionUtility.Components.WorkflowViewPanel();
            this.SuspendLayout();
            // 
            // workflowViewPanel1
            // 
            this.workflowViewPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.workflowViewPanel1.Location = new System.Drawing.Point(0, 0);
            this.workflowViewPanel1.Name = "workflowViewPanel1";
            this.workflowViewPanel1.Size = new System.Drawing.Size(515, 430);
            this.workflowViewPanel1.TabIndex = 0;
            // 
            // WorkflowDesignerDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(515, 430);
            this.Controls.Add(this.workflowViewPanel1);
            this.Name = "WorkflowDesignerDialog";
            this.Text = "WorkflowDesignerDialog";
            this.ResumeLayout(false);

        }

        #endregion

        private Microsoft.Samples.Workflow.WorkflowReflectionUtility.Components.WorkflowViewPanel workflowViewPanel1;
    }
}