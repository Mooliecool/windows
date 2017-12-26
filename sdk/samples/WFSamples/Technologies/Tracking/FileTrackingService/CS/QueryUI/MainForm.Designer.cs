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

namespace Microsoft.Samples.Workflow.FileTrackingServiceAndQuerySample
{
    partial class MainForm
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
            this.TextBoxInstanceId = new System.Windows.Forms.TextBox();
            this.ButtonGetWorkflowEvents = new System.Windows.Forms.Button();
            this.ListViewWorkflowEvents = new System.Windows.Forms.ListView();
            this.label1 = new System.Windows.Forms.Label();
            this.LabelWorkflowEventsList = new System.Windows.Forms.Label();
            this.ButtonExit = new System.Windows.Forms.Button();
            this.LabelReadingStatus = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // TextBoxInstanceId
            // 
            this.TextBoxInstanceId.Location = new System.Drawing.Point(96, 22);
            this.TextBoxInstanceId.Name = "TextBoxInstanceId";
            this.TextBoxInstanceId.Size = new System.Drawing.Size(203, 20);
            this.TextBoxInstanceId.TabIndex = 0;
            // 
            // ButtonGetWorkflowEvents
            // 
            this.ButtonGetWorkflowEvents.Location = new System.Drawing.Point(96, 51);
            this.ButtonGetWorkflowEvents.Name = "ButtonGetWorkflowEvents";
            this.ButtonGetWorkflowEvents.Size = new System.Drawing.Size(203, 22);
            this.ButtonGetWorkflowEvents.TabIndex = 1;
            this.ButtonGetWorkflowEvents.Text = "Get Tracked Workflow Events";
            this.ButtonGetWorkflowEvents.Click += new System.EventHandler(this.ButtonGetWorkflowEvents_Click);
            // 
            // ListViewWorkflowEvents
            // 
            this.ListViewWorkflowEvents.Location = new System.Drawing.Point(96, 108);
            this.ListViewWorkflowEvents.Name = "ListViewWorkflowEvents";
            this.ListViewWorkflowEvents.Size = new System.Drawing.Size(109, 128);
            this.ListViewWorkflowEvents.TabIndex = 2;
            this.ListViewWorkflowEvents.UseCompatibleStateImageBehavior = false;
            this.ListViewWorkflowEvents.View = System.Windows.Forms.View.List;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(0, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(90, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Enter Instance ID";
            // 
            // LabelWorkflowEventsList
            // 
            this.LabelWorkflowEventsList.AutoSize = true;
            this.LabelWorkflowEventsList.Location = new System.Drawing.Point(107, 92);
            this.LabelWorkflowEventsList.Name = "LabelWorkflowEventsList";
            this.LabelWorkflowEventsList.Size = new System.Drawing.Size(88, 13);
            this.LabelWorkflowEventsList.TabIndex = 4;
            this.LabelWorkflowEventsList.Text = "Workflow Events";
            // 
            // ButtonExit
            // 
            this.ButtonExit.Location = new System.Drawing.Point(237, 206);
            this.ButtonExit.Name = "ButtonExit";
            this.ButtonExit.Size = new System.Drawing.Size(62, 30);
            this.ButtonExit.TabIndex = 3;
            this.ButtonExit.Text = "Exit";
            this.ButtonExit.Click += new System.EventHandler(this.ButtonExit_Click);
            // 
            // LabelReadingStatus
            // 
            this.LabelReadingStatus.AutoSize = true;
            this.LabelReadingStatus.Location = new System.Drawing.Point(12, 108);
            this.LabelReadingStatus.Name = "LabelReadingStatus";
            this.LabelReadingStatus.Size = new System.Drawing.Size(0, 13);
            this.LabelReadingStatus.TabIndex = 6;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(328, 266);
            this.Controls.Add(this.ListViewWorkflowEvents);
            this.Controls.Add(this.LabelReadingStatus);
            this.Controls.Add(this.ButtonExit);
            this.Controls.Add(this.LabelWorkflowEventsList);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.ButtonGetWorkflowEvents);
            this.Controls.Add(this.TextBoxInstanceId);
            this.Name = "MainForm";
            this.Text = "Simple File Tracking Service Query";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox TextBoxInstanceId;
        private System.Windows.Forms.Button ButtonGetWorkflowEvents;
        private System.Windows.Forms.ListView ListViewWorkflowEvents;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label LabelWorkflowEventsList;
        private System.Windows.Forms.Button ButtonExit;
        private System.Windows.Forms.Label LabelReadingStatus;
    }
}

