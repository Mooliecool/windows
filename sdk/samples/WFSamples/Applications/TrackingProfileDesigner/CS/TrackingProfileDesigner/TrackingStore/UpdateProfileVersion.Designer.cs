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


namespace Microsoft.Samples.Workflow.TrackingProfileDesigner.TrackingStore
{
    partial class UpdateProfileVersion
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
            this.save = new System.Windows.Forms.Button();
            this.cancel = new System.Windows.Forms.Button();
            this.infoLabel = new System.Windows.Forms.Label();
            this.profileVersion = new System.Windows.Forms.TextBox();
            this.existingProfileLabel = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // save
            // 
            this.save.Location = new System.Drawing.Point(17, 173);
            this.save.Name = "save";
            this.save.Size = new System.Drawing.Size(75, 23);
            this.save.TabIndex = 0;
            this.save.Text = "&OK";
            this.save.UseVisualStyleBackColor = true;
            this.save.Click += new System.EventHandler(this.save_Click);
            // 
            // cancel
            // 
            this.cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancel.Location = new System.Drawing.Point(98, 173);
            this.cancel.Name = "cancel";
            this.cancel.Size = new System.Drawing.Size(75, 23);
            this.cancel.TabIndex = 1;
            this.cancel.Text = "&Cancel";
            this.cancel.UseVisualStyleBackColor = true;
            this.cancel.Click += new System.EventHandler(this.cancel_Click);
            // 
            // infoLabel
            // 
            this.infoLabel.Location = new System.Drawing.Point(12, 9);
            this.infoLabel.Name = "infoLabel";
            this.infoLabel.Size = new System.Drawing.Size(157, 89);
            this.infoLabel.TabIndex = 2;
            this.infoLabel.Text = "A tracking profile with a later version already exists for the workflow type sele" +
                "cted.  Please enter a later version for this profile, or cancel, check the track" +
                "ing service , and try again.";
            // 
            // profileVersion
            // 
            this.profileVersion.Location = new System.Drawing.Point(95, 142);
            this.profileVersion.Name = "profileVersion";
            this.profileVersion.Size = new System.Drawing.Size(75, 20);
            this.profileVersion.TabIndex = 4;
            // 
            // existingProfileLabel
            // 
            this.existingProfileLabel.AutoSize = true;
            this.existingProfileLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.existingProfileLabel.Location = new System.Drawing.Point(14, 98);
            this.existingProfileLabel.Name = "existingProfileLabel";
            this.existingProfileLabel.Size = new System.Drawing.Size(142, 13);
            this.existingProfileLabel.TabIndex = 5;
            this.existingProfileLabel.Text = "Current Profile Version: ";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 145);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(77, 13);
            this.label1.TabIndex = 6;
            this.label1.Text = "Profile Version:";
            // 
            // UpdateProfileVersion
            // 
            this.AcceptButton = this.save;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancel;
            this.ClientSize = new System.Drawing.Size(194, 211);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.existingProfileLabel);
            this.Controls.Add(this.profileVersion);
            this.Controls.Add(this.infoLabel);
            this.Controls.Add(this.cancel);
            this.Controls.Add(this.save);
            this.Name = "UpdateProfileVersion";
            this.Text = "Update Version";
            this.Load += new System.EventHandler(this.SaveTrackingProfile_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button save;
        private System.Windows.Forms.Button cancel;
        private System.Windows.Forms.Label infoLabel;
        private System.Windows.Forms.TextBox profileVersion;
        private System.Windows.Forms.Label existingProfileLabel;
        private System.Windows.Forms.Label label1;
    }
}