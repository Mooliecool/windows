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
    public partial class OrderSender : System.Windows.Forms.Form
    {
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.mainMenu1 = new System.Windows.Forms.MainMenu();
            this.menuItemExit = new System.Windows.Forms.MenuItem();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPageSend = new System.Windows.Forms.TabPage();
            this.btnSubmit = new System.Windows.Forms.Button();
            this.chkFast = new System.Windows.Forms.CheckBox();
            this.dateTimePicker1 = new System.Windows.Forms.DateTimePicker();
            this.tBContact = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.tBCompName = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.tBQty = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.comboCD = new System.Windows.Forms.ComboBox();
            this.tabPageProcessed = new System.Windows.Forms.TabPage();
            this.listBoxProc = new System.Windows.Forms.ListBox();
            this.btnDisplay = new System.Windows.Forms.Button();
            this.label5 = new System.Windows.Forms.Label();
            this.mqdemoAck = new System.Messaging.MessageQueue();
            this.textBoxServer = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.tabControl1.SuspendLayout();
            this.tabPageSend.SuspendLayout();
            this.tabPageProcessed.SuspendLayout();
            this.SuspendLayout();
            // 
            // mainMenu1
            // 
            this.mainMenu1.MenuItems.Add(this.menuItemExit);
            // 
            // menuItemExit
            // 
            this.menuItemExit.Text = "Exit";
            this.menuItemExit.Click += new System.EventHandler(this.menuItemExit_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPageSend);
            this.tabControl1.Controls.Add(this.tabPageProcessed);
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(240, 254);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPageSend
            // 
            this.tabPageSend.BackColor = System.Drawing.Color.Yellow;
            this.tabPageSend.Controls.Add(this.label6);
            this.tabPageSend.Controls.Add(this.btnSubmit);
            this.tabPageSend.Controls.Add(this.textBoxServer);
            this.tabPageSend.Controls.Add(this.chkFast);
            this.tabPageSend.Controls.Add(this.dateTimePicker1);
            this.tabPageSend.Controls.Add(this.tBContact);
            this.tabPageSend.Controls.Add(this.label4);
            this.tabPageSend.Controls.Add(this.tBCompName);
            this.tabPageSend.Controls.Add(this.label3);
            this.tabPageSend.Controls.Add(this.tBQty);
            this.tabPageSend.Controls.Add(this.label2);
            this.tabPageSend.Controls.Add(this.label1);
            this.tabPageSend.Controls.Add(this.comboCD);
            this.tabPageSend.Location = new System.Drawing.Point(0, 0);
            this.tabPageSend.Name = "tabPageSend";
            this.tabPageSend.Size = new System.Drawing.Size(240, 231);
            this.tabPageSend.Text = "Send";
            // 
            // btnSubmit
            // 
            this.btnSubmit.Location = new System.Drawing.Point(129, 200);
            this.btnSubmit.Name = "btnSubmit";
            this.btnSubmit.TabIndex = 21;
            this.btnSubmit.Text = "Submit";
            this.btnSubmit.Click += new System.EventHandler(this.btnSubmit_Click);
            // 
            // chkFast
            // 
            this.chkFast.BackColor = System.Drawing.Color.Yellow;
            this.chkFast.Location = new System.Drawing.Point(10, 200);
            this.chkFast.Name = "chkFast";
            this.chkFast.Size = new System.Drawing.Size(94, 20);
            this.chkFast.TabIndex = 20;
            this.chkFast.Text = "Fast Order";
            // 
            // dateTimePicker1
            // 
            this.dateTimePicker1.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.dateTimePicker1.CalendarFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular);
            this.dateTimePicker1.Location = new System.Drawing.Point(10, 161);
            this.dateTimePicker1.Name = "dateTimePicker1";
            this.dateTimePicker1.Size = new System.Drawing.Size(222, 22);
            this.dateTimePicker1.TabIndex = 19;
            // 
            // tBContact
            // 
            this.tBContact.Location = new System.Drawing.Point(103, 126);
            this.tBContact.Name = "tBContact";
            this.tBContact.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.tBContact.Size = new System.Drawing.Size(130, 21);
            this.tBContact.TabIndex = 18;
            // 
            // label4
            // 
            this.label4.BackColor = System.Drawing.Color.Yellow;
            this.label4.Location = new System.Drawing.Point(10, 126);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(92, 20);
            this.label4.Text = "Contact Name:";
            // 
            // tBCompName
            // 
            this.tBCompName.Location = new System.Drawing.Point(103, 93);
            this.tBCompName.Name = "tBCompName";
            this.tBCompName.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.tBCompName.Size = new System.Drawing.Size(131, 21);
            this.tBCompName.TabIndex = 17;
            // 
            // label3
            // 
            this.label3.BackColor = System.Drawing.Color.Yellow;
            this.label3.Location = new System.Drawing.Point(7, 95);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(95, 20);
            this.label3.Text = "Company Name:";
            // 
            // tBQty
            // 
            this.tBQty.Location = new System.Drawing.Point(101, 60);
            this.tBQty.Name = "tBQty";
            this.tBQty.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.tBQty.Size = new System.Drawing.Size(131, 21);
            this.tBQty.TabIndex = 16;
            // 
            // label2
            // 
            this.label2.BackColor = System.Drawing.Color.Yellow;
            this.label2.Location = new System.Drawing.Point(9, 61);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(83, 20);
            this.label2.Text = "Quantity:";
            // 
            // label1
            // 
            this.label1.BackColor = System.Drawing.Color.Yellow;
            this.label1.Location = new System.Drawing.Point(9, 30);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(86, 20);
            this.label1.Text = "CD to Order:";
            // 
            // comboCD
            // 
            this.comboCD.Items.Add("Destiny Fulfilled");
            this.comboCD.Items.Add("Stardust");
            this.comboCD.Items.Add("Confessions");
            this.comboCD.Items.Add("Blue Angel");
            this.comboCD.Items.Add("Slow Jam");
            this.comboCD.Location = new System.Drawing.Point(101, 28);
            this.comboCD.Name = "comboCD";
            this.comboCD.Size = new System.Drawing.Size(131, 22);
            this.comboCD.TabIndex = 15;
            // 
            // tabPageProcessed
            // 
            this.tabPageProcessed.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(255)))));
            this.tabPageProcessed.Controls.Add(this.listBoxProc);
            this.tabPageProcessed.Controls.Add(this.btnDisplay);
            this.tabPageProcessed.Controls.Add(this.label5);
            this.tabPageProcessed.Location = new System.Drawing.Point(0, 0);
            this.tabPageProcessed.Name = "tabPageProcessed";
            this.tabPageProcessed.Size = new System.Drawing.Size(192, 74);
            this.tabPageProcessed.Text = "Processed";
            // 
            // listBoxProc
            // 
            this.listBoxProc.Location = new System.Drawing.Point(22, 30);
            this.listBoxProc.Name = "listBoxProc";
            this.listBoxProc.Size = new System.Drawing.Size(179, 142);
            this.listBoxProc.TabIndex = 4;
            // 
            // btnDisplay
            // 
            this.btnDisplay.Location = new System.Drawing.Point(72, 185);
            this.btnDisplay.Name = "btnDisplay";
            this.btnDisplay.TabIndex = 2;
            this.btnDisplay.Text = "Display";
            this.btnDisplay.Click += new System.EventHandler(this.btnDisplay_Click);
            // 
            // label5
            // 
            this.label5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(255)))));
            this.label5.Location = new System.Drawing.Point(22, 7);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(130, 20);
            this.label5.Text = "Processed Orders";
            // 
            // mqdemoAck
            // 
            this.mqdemoAck.DefaultPropertiesToSend.AcknowledgeType = System.Messaging.AcknowledgeTypes.None;
            this.mqdemoAck.DefaultPropertiesToSend.Priority = System.Messaging.MessagePriority.Normal;
            this.mqdemoAck.Formatter = new System.Messaging.XmlMessageFormatter(new string[0]);
            // 
            // textBoxServer
            // 
            this.textBoxServer.Location = new System.Drawing.Point(101, 1);
            this.textBoxServer.Name = "textBoxServer";
            this.textBoxServer.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.textBoxServer.Size = new System.Drawing.Size(100, 21);
            this.textBoxServer.TabIndex = 1;
            // 
            // label6
            // 
            this.label6.BackColor = System.Drawing.Color.Aqua;
            this.label6.Location = new System.Drawing.Point(34, 2);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(45, 20);
            this.label6.Text = "Server:";
            // 
            // OrderSender
            // 
            this.AutoScroll = true;
            this.BackColor = System.Drawing.Color.Red;
            this.ClientSize = new System.Drawing.Size(240, 268);
            this.Controls.Add(this.tabControl1);
            this.Menu = this.mainMenu1;
            this.Name = "OrderSender";
            this.Text = "CD Order";
            this.tabControl1.ResumeLayout(false);
            this.tabPageSend.ResumeLayout(false);
            this.tabPageProcessed.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
        }

        private System.Windows.Forms.MainMenu mainMenu1;
        private System.Windows.Forms.MenuItem menuItemExit;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPageSend;
        private System.Windows.Forms.Button btnSubmit;
        private System.Windows.Forms.CheckBox chkFast;
        private System.Windows.Forms.DateTimePicker dateTimePicker1;
        private System.Windows.Forms.TextBox tBContact;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tBCompName;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox tBQty;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox comboCD;
        private System.Windows.Forms.TabPage tabPageProcessed;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button btnDisplay;
        private System.Messaging.MessageQueue mqdemoAck;
        private System.Windows.Forms.ListBox listBoxProc;
        private System.Windows.Forms.TextBox textBoxServer;
        private System.Windows.Forms.Label label6;
    }
}

