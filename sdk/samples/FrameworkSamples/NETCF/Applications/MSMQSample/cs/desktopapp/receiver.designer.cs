//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
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
namespace Microsoft.Samples.MsmqSample
{
    partial class Receiver
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
            this.lBOrders = new System.Windows.Forms.ListBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tBSent = new System.Windows.Forms.TextBox();
            this.tBCDTitle = new System.Windows.Forms.TextBox();
            this.tBCount = new System.Windows.Forms.TextBox();
            this.tBCompany = new System.Windows.Forms.TextBox();
            this.tBContact = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.btnProcess = new System.Windows.Forms.Button();
            this.lblFast = new System.Windows.Forms.Label();
            this.mqOrderQueue = new System.Messaging.MessageQueue();
            this.SuspendLayout();
            // 
            // lBOrders
            // 
            this.lBOrders.FormattingEnabled = true;
            this.lBOrders.Location = new System.Drawing.Point(12, 47);
            this.lBOrders.Name = "lBOrders";
            this.lBOrders.Size = new System.Drawing.Size(166, 95);
            this.lBOrders.TabIndex = 0;
            this.lBOrders.SelectedIndexChanged += new System.EventHandler(this.lBOrders_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 27);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(42, 14);
            this.label1.TabIndex = 1;
            this.label1.Text = "Orders:";
            // 
            // tBSent
            // 
            this.tBSent.Location = new System.Drawing.Point(321, 14);
            this.tBSent.Name = "tBSent";
            this.tBSent.ReadOnly = true;
            this.tBSent.Size = new System.Drawing.Size(200, 20);
            this.tBSent.TabIndex = 2;
            // 
            // tBCDTitle
            // 
            this.tBCDTitle.Location = new System.Drawing.Point(321, 57);
            this.tBCDTitle.Name = "tBCDTitle";
            this.tBCDTitle.ReadOnly = true;
            this.tBCDTitle.TabIndex = 3;
            // 
            // tBCount
            // 
            this.tBCount.Location = new System.Drawing.Point(321, 100);
            this.tBCount.Name = "tBCount";
            this.tBCount.ReadOnly = true;
            this.tBCount.TabIndex = 4;
            // 
            // tBCompany
            // 
            this.tBCompany.Location = new System.Drawing.Point(321, 148);
            this.tBCompany.Name = "tBCompany";
            this.tBCompany.ReadOnly = true;
            this.tBCompany.TabIndex = 5;
            // 
            // tBContact
            // 
            this.tBContact.Location = new System.Drawing.Point(321, 193);
            this.tBContact.Name = "tBContact";
            this.tBContact.ReadOnly = true;
            this.tBContact.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(282, 16);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(27, 14);
            this.label2.TabIndex = 7;
            this.label2.Text = "Sent";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(264, 60);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(45, 14);
            this.label3.TabIndex = 8;
            this.label3.Text = "CD Title";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(275, 103);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(34, 14);
            this.label4.TabIndex = 9;
            this.label4.Text = "Count";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(256, 150);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(53, 14);
            this.label5.TabIndex = 10;
            this.label5.Text = "Company";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(266, 195);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(43, 14);
            this.label6.TabIndex = 11;
            this.label6.Text = "Contact";
            // 
            // btnProcess
            // 
            this.btnProcess.Location = new System.Drawing.Point(332, 231);
            this.btnProcess.Name = "btnProcess";
            this.btnProcess.Size = new System.Drawing.Size(121, 23);
            this.btnProcess.TabIndex = 12;
            this.btnProcess.Text = "Process Order";
            this.btnProcess.Click += new System.EventHandler(this.btnProcess_Click);
            // 
            // lblFast
            // 
            this.lblFast.AutoSize = true;
            this.lblFast.Font = new System.Drawing.Font("Tahoma", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblFast.Location = new System.Drawing.Point(12, 165);
            this.lblFast.Name = "lblFast";
            this.lblFast.Size = new System.Drawing.Size(132, 26);
            this.lblFast.TabIndex = 13;
            this.lblFast.Text = "FAST ORDER";
            this.lblFast.Visible = false;
            // 
            // mqOrderQueue
            // 
            this.mqOrderQueue.Path = ".\\private$\\mqsample";
            this.mqOrderQueue.SynchronizingObject = this;
            // 
            // Receiver
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(535, 266);
            this.Controls.Add(this.lblFast);
            this.Controls.Add(this.btnProcess);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.tBContact);
            this.Controls.Add(this.tBCompany);
            this.Controls.Add(this.tBCount);
            this.Controls.Add(this.tBCDTitle);
            this.Controls.Add(this.tBSent);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.lBOrders);
            this.Name = "Receiver";
            this.Text = "Receiver";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox lBOrders;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tBSent;
        private System.Windows.Forms.TextBox tBCDTitle;
        private System.Windows.Forms.TextBox tBCount;
        private System.Windows.Forms.TextBox tBCompany;
        private System.Windows.Forms.TextBox tBContact;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button btnProcess;
        private System.Windows.Forms.Label lblFast;
        private System.Messaging.MessageQueue mqOrderQueue;
    }
}

