//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

namespace Microsoft.Samples.WF.SecuringWorkFlow.Client
{
    partial class Form1
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
            this.cmdCallWorkflowService = new System.Windows.Forms.Button();
            this.txtRet = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.txtAnswer = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // cmdCallWorkflowService
            // 
            this.cmdCallWorkflowService.Location = new System.Drawing.Point(571, 41);
            this.cmdCallWorkflowService.Name = "cmdCallWorkflowService";
            this.cmdCallWorkflowService.Size = new System.Drawing.Size(66, 56);
            this.cmdCallWorkflowService.TabIndex = 0;
            this.cmdCallWorkflowService.Text = "Ask";
            this.cmdCallWorkflowService.UseVisualStyleBackColor = true;
            this.cmdCallWorkflowService.Click += new System.EventHandler(this.cmdCallWorkflowService_Click);
            // 
            // txtRet
            // 
            this.txtRet.Location = new System.Drawing.Point(117, 41);
            this.txtRet.Multiline = true;
            this.txtRet.Name = "txtRet";
            this.txtRet.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtRet.Size = new System.Drawing.Size(433, 59);
            this.txtRet.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(31, 44);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(77, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Your Question:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(451, 122);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(99, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "The Magic Answer:";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // txtAnswer
            // 
            this.txtAnswer.Enabled = false;
            this.txtAnswer.Location = new System.Drawing.Point(568, 122);
            this.txtAnswer.Name = "txtAnswer";
            this.txtAnswer.Size = new System.Drawing.Size(69, 20);
            this.txtAnswer.TabIndex = 4;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(117, 122);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(272, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "Only those who possess the magic character may ask ...";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(651, 155);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.txtAnswer);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtRet);
            this.Controls.Add(this.cmdCallWorkflowService);
            this.Name = "Form1";
            this.Text = "Client";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button cmdCallWorkflowService;
        private System.Windows.Forms.TextBox txtRet;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtAnswer;
        private System.Windows.Forms.Label label3;
    }
}

