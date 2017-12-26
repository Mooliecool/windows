/*************************************************************************************************
 *
 * File: CacheTimerForm.Designer.cs
 *
 * Description: Design-time code for the interface.
 * 
 * See CacheTimerForm.cs for a full description of the sample.
 *
 *     
 *  This file is part of the Microsoft WinfFX SDK Code Samples.
 * 
 *  Copyright (C) Microsoft Corporation.  All rights reserved.
 * 
 * This source code is intended only as a supplement to Microsoft
 * Development Tools and/or on-line documentation.  See these other
 * materials for detailed information regarding Microsoft code samples.
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 * 
 *************************************************************************************************/
namespace TestScriptGeneratorSample
{
    partial class ScriptGeneratorClient
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
            this.btnStart = new System.Windows.Forms.Button();
            this.btnStop = new System.Windows.Forms.Button();
            this.btnGenerateScript = new System.Windows.Forms.Button();
            this.btnStartTarget = new System.Windows.Forms.Button();
            this.tbFeedback = new System.Windows.Forms.RichTextBox();
            this.btnClear = new System.Windows.Forms.Button();
            this.btnCopyToClipboard = new System.Windows.Forms.Button();
            this.btnDumpStore = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnStart
            // 
            this.btnStart.Enabled = false;
            this.btnStart.Location = new System.Drawing.Point(51, 28);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(160, 23);
            this.btnStart.TabIndex = 2;
            this.btnStart.Text = "St&art UI Automation";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // btnStop
            // 
            this.btnStop.Enabled = false;
            this.btnStop.Location = new System.Drawing.Point(220, 28);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(160, 23);
            this.btnStop.TabIndex = 3;
            this.btnStop.Text = "St&op UI Automation";
            this.btnStop.UseVisualStyleBackColor = true;
            this.btnStop.Click += new System.EventHandler(this.btnStop_Click);
            // 
            // btnGenerateScript
            // 
            this.btnGenerateScript.Enabled = false;
            this.btnGenerateScript.Location = new System.Drawing.Point(220, 57);
            this.btnGenerateScript.Name = "btnGenerateScript";
            this.btnGenerateScript.Size = new System.Drawing.Size(110, 23);
            this.btnGenerateScript.TabIndex = 5;
            this.btnGenerateScript.Text = "&Generate Script";
            this.btnGenerateScript.UseVisualStyleBackColor = true;
            this.btnGenerateScript.Click += new System.EventHandler(this.GenerateTextOutput);
            // 
            // btnStartTarget
            // 
            this.btnStartTarget.Location = new System.Drawing.Point(12, -1);
            this.btnStartTarget.Name = "btnStartTarget";
            this.btnStartTarget.Size = new System.Drawing.Size(419, 23);
            this.btnStartTarget.TabIndex = 1;
            this.btnStartTarget.Text = "&Start Target";
            this.btnStartTarget.UseVisualStyleBackColor = true;
            this.btnStartTarget.Click += new System.EventHandler(this.btnStartTarget_Click);
            // 
            // tbFeedback
            // 
            this.tbFeedback.Location = new System.Drawing.Point(11, 92);
            this.tbFeedback.Name = "tbFeedback";
            this.tbFeedback.Size = new System.Drawing.Size(420, 237);
            this.tbFeedback.TabIndex = 7;
            this.tbFeedback.Text = "";
            // 
            // btnClear
            // 
            this.btnClear.Enabled = false;
            this.btnClear.Location = new System.Drawing.Point(204, 343);
            this.btnClear.Name = "btnClear";
            this.btnClear.Size = new System.Drawing.Size(75, 23);
            this.btnClear.TabIndex = 8;
            this.btnClear.Text = "C&lear";
            this.btnClear.UseVisualStyleBackColor = true;
            this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
            // 
            // btnCopyToClipboard
            // 
            this.btnCopyToClipboard.Enabled = false;
            this.btnCopyToClipboard.Location = new System.Drawing.Point(285, 343);
            this.btnCopyToClipboard.Name = "btnCopyToClipboard";
            this.btnCopyToClipboard.Size = new System.Drawing.Size(146, 23);
            this.btnCopyToClipboard.TabIndex = 9;
            this.btnCopyToClipboard.Text = "&Copy Script to Clipboard";
            this.btnCopyToClipboard.UseVisualStyleBackColor = true;
            this.btnCopyToClipboard.Click += new System.EventHandler(this.GenerateTextOutput);
            // 
            // btnDumpStore
            // 
            this.btnDumpStore.Enabled = false;
            this.btnDumpStore.Location = new System.Drawing.Point(101, 57);
            this.btnDumpStore.Name = "btnDumpStore";
            this.btnDumpStore.Size = new System.Drawing.Size(110, 23);
            this.btnDumpStore.TabIndex = 4;
            this.btnDumpStore.Text = "&Dump Store";
            this.btnDumpStore.UseVisualStyleBackColor = true;
            this.btnDumpStore.Click += new System.EventHandler(this.GenerateTextOutput);
            // 
            // ScriptGeneratorClient
            // 
            this.ClientSize = new System.Drawing.Size(443, 378);
            this.Controls.Add(this.btnDumpStore);
            this.Controls.Add(this.btnCopyToClipboard);
            this.Controls.Add(this.btnClear);
            this.Controls.Add(this.tbFeedback);
            this.Controls.Add(this.btnStartTarget);
            this.Controls.Add(this.btnGenerateScript);
            this.Controls.Add(this.btnStop);
            this.Controls.Add(this.btnStart);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "ScriptGeneratorClient";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "UI Automation Automated Script Generator";
            this.TopMost = true;
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.Button btnStop;
        private System.Windows.Forms.Button btnGenerateScript;
        private System.Windows.Forms.Button btnStartTarget;
        private System.Windows.Forms.RichTextBox tbFeedback;
        private System.Windows.Forms.Button btnClear;
        private System.Windows.Forms.Button btnCopyToClipboard;
        private System.Windows.Forms.Button btnDumpStore;
    }
}

