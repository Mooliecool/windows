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


namespace Microsoft.Samples.Workflow.SpeechApplication
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.label1 = new System.Windows.Forms.Label();
            this.txtPhoneMessage = new System.Windows.Forms.RichTextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.button5 = new System.Windows.Forms.Button();
            this.button6 = new System.Windows.Forms.Button();
            this.button7 = new System.Windows.Forms.Button();
            this.button8 = new System.Windows.Forms.Button();
            this.button9 = new System.Windows.Forms.Button();
            this.buttonstar = new System.Windows.Forms.Button();
            this.button0 = new System.Windows.Forms.Button();
            this.buttonpound = new System.Windows.Forms.Button();
            this.btnStartWorkflow = new System.Windows.Forms.Button();
            this.richTextBox2 = new System.Windows.Forms.RichTextBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(25, 24);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(70, 24);
            this.label1.TabIndex = 0;
            this.label1.Text = "Prompt";
            // 
            // txtPhoneMessage
            // 
            this.txtPhoneMessage.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtPhoneMessage.Location = new System.Drawing.Point(29, 60);
            this.txtPhoneMessage.Name = "txtPhoneMessage";
            this.txtPhoneMessage.Size = new System.Drawing.Size(626, 229);
            this.txtPhoneMessage.TabIndex = 1;
            this.txtPhoneMessage.Text = "";
            // 
            // button1
            // 
            this.button1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button1.Location = new System.Drawing.Point(29, 364);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(43, 36);
            this.button1.TabIndex = 2;
            this.button1.Text = "1";
            this.button1.Click += new System.EventHandler(this.btnPhone_Click);
            // 
            // button2
            // 
            this.button2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button2.Location = new System.Drawing.Point(78, 364);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(43, 36);
            this.button2.TabIndex = 3;
            this.button2.Text = "2";
            this.button2.Click += new System.EventHandler(this.btnPhone_Click);
            // 
            // button3
            // 
            this.button3.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button3.Location = new System.Drawing.Point(127, 364);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(39, 36);
            this.button3.TabIndex = 4;
            this.button3.Text = "3";
            this.button3.Click += new System.EventHandler(this.btnPhone_Click);
            // 
            // button4
            // 
            this.button4.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button4.Location = new System.Drawing.Point(29, 416);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(43, 35);
            this.button4.TabIndex = 5;
            this.button4.Text = "4";
            this.button4.Click += new System.EventHandler(this.btnPhone_Click);
            // 
            // button5
            // 
            this.button5.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button5.Location = new System.Drawing.Point(78, 416);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(43, 35);
            this.button5.TabIndex = 6;
            this.button5.Text = "5";
            this.button5.Click += new System.EventHandler(this.btnPhone_Click);
            // 
            // button6
            // 
            this.button6.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button6.Location = new System.Drawing.Point(127, 416);
            this.button6.Name = "button6";
            this.button6.Size = new System.Drawing.Size(39, 35);
            this.button6.TabIndex = 7;
            this.button6.Text = "6";
            this.button6.Click += new System.EventHandler(this.btnPhone_Click);
            // 
            // button7
            // 
            this.button7.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button7.Location = new System.Drawing.Point(29, 466);
            this.button7.Name = "button7";
            this.button7.Size = new System.Drawing.Size(43, 36);
            this.button7.TabIndex = 8;
            this.button7.Text = "7";
            this.button7.Click += new System.EventHandler(this.btnPhone_Click);
            // 
            // button8
            // 
            this.button8.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button8.Location = new System.Drawing.Point(78, 466);
            this.button8.Name = "button8";
            this.button8.Size = new System.Drawing.Size(43, 36);
            this.button8.TabIndex = 9;
            this.button8.Text = "8";
            this.button8.Click += new System.EventHandler(this.btnPhone_Click);
            // 
            // button9
            // 
            this.button9.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button9.Location = new System.Drawing.Point(127, 466);
            this.button9.Name = "button9";
            this.button9.Size = new System.Drawing.Size(39, 36);
            this.button9.TabIndex = 10;
            this.button9.Text = "9";
            this.button9.Click += new System.EventHandler(this.btnPhone_Click);
            // 
            // buttonstar
            // 
            this.buttonstar.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buttonstar.Location = new System.Drawing.Point(29, 518);
            this.buttonstar.Name = "buttonstar";
            this.buttonstar.Size = new System.Drawing.Size(43, 34);
            this.buttonstar.TabIndex = 11;
            this.buttonstar.Text = "*";
            this.buttonstar.Click += new System.EventHandler(this.btnPhone_Click);
            // 
            // button0
            // 
            this.button0.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button0.Location = new System.Drawing.Point(78, 518);
            this.button0.Name = "button0";
            this.button0.Size = new System.Drawing.Size(43, 34);
            this.button0.TabIndex = 12;
            this.button0.Text = "0";
            this.button0.Click += new System.EventHandler(this.btnPhone_Click);
            // 
            // buttonpound
            // 
            this.buttonpound.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buttonpound.Location = new System.Drawing.Point(127, 518);
            this.buttonpound.Name = "buttonpound";
            this.buttonpound.Size = new System.Drawing.Size(39, 34);
            this.buttonpound.TabIndex = 13;
            this.buttonpound.Text = "#";
            this.buttonpound.Click += new System.EventHandler(this.btnPhone_Click);
            // 
            // btnStartWorkflow
            // 
            this.btnStartWorkflow.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnStartWorkflow.Location = new System.Drawing.Point(228, 311);
            this.btnStartWorkflow.Name = "btnStartWorkflow";
            this.btnStartWorkflow.Size = new System.Drawing.Size(210, 35);
            this.btnStartWorkflow.TabIndex = 14;
            this.btnStartWorkflow.Text = "Start Phone Session";
            this.btnStartWorkflow.Click += new System.EventHandler(this.btnStartWorkflow_Click);
            // 
            // richTextBox2
            // 
            this.richTextBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.richTextBox2.Location = new System.Drawing.Point(228, 364);
            this.richTextBox2.Name = "richTextBox2";
            this.richTextBox2.Size = new System.Drawing.Size(427, 244);
            this.richTextBox2.TabIndex = 15;
            this.richTextBox2.Text = resources.GetString("richTextBox2.Text");
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(676, 622);
            this.Controls.Add(this.richTextBox2);
            this.Controls.Add(this.btnStartWorkflow);
            this.Controls.Add(this.buttonpound);
            this.Controls.Add(this.button0);
            this.Controls.Add(this.buttonstar);
            this.Controls.Add(this.button9);
            this.Controls.Add(this.button8);
            this.Controls.Add(this.button7);
            this.Controls.Add(this.button6);
            this.Controls.Add(this.button5);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.txtPhoneMessage);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "Phone Menu System Demo using State Machine Workflow";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.RichTextBox txtPhoneMessage;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.Button button6;
        private System.Windows.Forms.Button button7;
        private System.Windows.Forms.Button button8;
        private System.Windows.Forms.Button button9;
        private System.Windows.Forms.Button buttonstar;
        private System.Windows.Forms.Button button0;
        private System.Windows.Forms.Button buttonpound;
        private System.Windows.Forms.Button btnStartWorkflow;
        private System.Windows.Forms.RichTextBox richTextBox2;

    }
}

