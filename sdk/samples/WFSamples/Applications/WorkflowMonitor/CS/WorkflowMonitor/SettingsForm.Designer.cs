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
    internal partial class SettingsForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SettingsForm));
            this.OKButton = new System.Windows.Forms.Button();
            this.Cancel_Button = new System.Windows.Forms.Button();
            this.Machine_Label = new System.Windows.Forms.Label();
            this.Machine_TextBox = new System.Windows.Forms.TextBox();
            this.Database_Label = new System.Windows.Forms.Label();
            this.Database_Textbox = new System.Windows.Forms.TextBox();
            this.Polling_Label = new System.Windows.Forms.Label();
            this.Polling_TextBox = new System.Windows.Forms.TextBox();
            this.Unit_Label = new System.Windows.Forms.Label();
            this.AutoSelectLatest_Checkbox = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // OKButton
            // 
            this.OKButton.Location = new System.Drawing.Point(140, 166);
            this.OKButton.Margin = new System.Windows.Forms.Padding(1, 3, 3, 3);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(75, 23);
            this.OKButton.TabIndex = 0;
            this.OKButton.Text = "OK";
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // Cancel_Button
            // 
            this.Cancel_Button.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Cancel_Button.Location = new System.Drawing.Point(233, 166);
            this.Cancel_Button.Name = "Cancel_Button";
            this.Cancel_Button.Size = new System.Drawing.Size(75, 23);
            this.Cancel_Button.TabIndex = 1;
            this.Cancel_Button.Text = "Cancel";
            this.Cancel_Button.Click += new System.EventHandler(this.Cancel_Button_Click);
            // 
            // Machine_Label
            // 
            this.Machine_Label.AutoSize = true;
            this.Machine_Label.BackColor = System.Drawing.SystemColors.Control;
            this.Machine_Label.Location = new System.Drawing.Point(9, 15);
            this.Machine_Label.Name = "Machine_Label";
            this.Machine_Label.Size = new System.Drawing.Size(65, 13);
            this.Machine_Label.TabIndex = 2;
            this.Machine_Label.Text = "SQL Server:";
            // 
            // Machine_TextBox
            // 
            this.Machine_TextBox.Location = new System.Drawing.Point(111, 15);
            this.Machine_TextBox.Name = "Machine_TextBox";
            this.Machine_TextBox.Size = new System.Drawing.Size(197, 20);
            this.Machine_TextBox.TabIndex = 3;
            this.Machine_TextBox.Text = "localhost";
            // 
            // Database_Label
            // 
            this.Database_Label.AutoSize = true;
            this.Database_Label.BackColor = System.Drawing.SystemColors.Control;
            this.Database_Label.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Database_Label.ForeColor = System.Drawing.Color.Black;
            this.Database_Label.Location = new System.Drawing.Point(9, 51);
            this.Database_Label.Name = "Database_Label";
            this.Database_Label.Size = new System.Drawing.Size(101, 13);
            this.Database_Label.TabIndex = 4;
            this.Database_Label.Text = "Tracking Database:";
            // 
            // Database_Textbox
            // 
            this.Database_Textbox.BackColor = System.Drawing.SystemColors.Window;
            this.Database_Textbox.Location = new System.Drawing.Point(111, 51);
            this.Database_Textbox.Name = "Database_Textbox";
            this.Database_Textbox.Size = new System.Drawing.Size(197, 20);
            this.Database_Textbox.TabIndex = 5;
            this.Database_Textbox.Text = "WorkflowStore";
            // 
            // Polling_Label
            // 
            this.Polling_Label.AutoSize = true;
            this.Polling_Label.BackColor = System.Drawing.SystemColors.Control;
            this.Polling_Label.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Polling_Label.ForeColor = System.Drawing.Color.Black;
            this.Polling_Label.Location = new System.Drawing.Point(9, 90);
            this.Polling_Label.Name = "Polling_Label";
            this.Polling_Label.Size = new System.Drawing.Size(79, 13);
            this.Polling_Label.TabIndex = 6;
            this.Polling_Label.Text = "Polling Interval:";
            // 
            // Polling_TextBox
            // 
            this.Polling_TextBox.Location = new System.Drawing.Point(111, 90);
            this.Polling_TextBox.Name = "Polling_TextBox";
            this.Polling_TextBox.Size = new System.Drawing.Size(47, 20);
            this.Polling_TextBox.TabIndex = 7;
            this.Polling_TextBox.Text = "500";
            // 
            // Unit_Label
            // 
            this.Unit_Label.AutoSize = true;
            this.Unit_Label.Location = new System.Drawing.Point(165, 96);
            this.Unit_Label.Name = "Unit_Label";
            this.Unit_Label.Size = new System.Drawing.Size(20, 13);
            this.Unit_Label.TabIndex = 8;
            this.Unit_Label.Text = "ms";
            // 
            // AutoSelectLatest_Checkbox
            // 
            this.AutoSelectLatest_Checkbox.AutoSize = true;
            this.AutoSelectLatest_Checkbox.Location = new System.Drawing.Point(9, 132);
            this.AutoSelectLatest_Checkbox.Name = "AutoSelectLatest_Checkbox";
            this.AutoSelectLatest_Checkbox.Size = new System.Drawing.Size(179, 17);
            this.AutoSelectLatest_Checkbox.TabIndex = 14;
            this.AutoSelectLatest_Checkbox.Text = "Auto-Select Latest When Polling";
            // 
            // SettingsForm
            // 
            this.AcceptButton = this.OKButton;
            this.CancelButton = this.Cancel_Button;
            this.ClientSize = new System.Drawing.Size(315, 201);
            this.ControlBox = false;
            this.Controls.Add(this.AutoSelectLatest_Checkbox);
            this.Controls.Add(this.Unit_Label);
            this.Controls.Add(this.Polling_TextBox);
            this.Controls.Add(this.Polling_Label);
            this.Controls.Add(this.Database_Textbox);
            this.Controls.Add(this.Database_Label);
            this.Controls.Add(this.Machine_TextBox);
            this.Controls.Add(this.Machine_Label);
            this.Controls.Add(this.Cancel_Button);
            this.Controls.Add(this.OKButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SettingsForm";
            this.Tag = "";
            this.Text = " Settings";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button OKButton;
        private System.Windows.Forms.Button Cancel_Button;
        private System.Windows.Forms.Label Machine_Label;
        private System.Windows.Forms.TextBox Machine_TextBox;
        private System.Windows.Forms.Label Database_Label;
        private System.Windows.Forms.TextBox Database_Textbox;
        private System.Windows.Forms.Label Polling_Label;
        private System.Windows.Forms.TextBox Polling_TextBox;
        private System.Windows.Forms.Label Unit_Label;
        private System.Windows.Forms.CheckBox AutoSelectLatest_Checkbox;


    }
}