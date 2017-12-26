using Microsoft.SDK.Samples.VistaBridge.Library;

namespace CommandLinkTestApp
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
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.txtRetVal = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.btnCall = new System.Windows.Forms.Button();
            this.txtParam = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.cmbOp = new System.Windows.Forms.ComboBox();
            this.commandLink1 = new CommandLinkWinForms();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(22, 25);
            this.label1.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(0, 25);
            this.label1.TabIndex = 1;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.txtRetVal);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.btnCall);
            this.groupBox1.Controls.Add(this.txtParam);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.cmbOp);
            this.groupBox1.Location = new System.Drawing.Point(14, 12);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(5);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(5);
            this.groupBox1.Size = new System.Drawing.Size(468, 226);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = " Command Link Operations";
            // 
            // txtRetVal
            // 
            this.txtRetVal.Location = new System.Drawing.Point(151, 115);
            this.txtRetVal.Margin = new System.Windows.Forms.Padding(5);
            this.txtRetVal.Name = "txtRetVal";
            this.txtRetVal.ReadOnly = true;
            this.txtRetVal.Size = new System.Drawing.Size(298, 33);
            this.txtRetVal.TabIndex = 6;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(17, 122);
            this.label4.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(122, 25);
            this.label4.TabIndex = 5;
            this.label4.Text = "Return value:";
            // 
            // btnCall
            // 
            this.btnCall.Location = new System.Drawing.Point(281, 158);
            this.btnCall.Margin = new System.Windows.Forms.Padding(5);
            this.btnCall.Name = "btnCall";
            this.btnCall.Size = new System.Drawing.Size(168, 45);
            this.btnCall.TabIndex = 4;
            this.btnCall.Text = "Call Operation";
            this.btnCall.UseVisualStyleBackColor = true;
            this.btnCall.Click += new System.EventHandler(this.OnCallOperation);
            // 
            // txtParam
            // 
            this.txtParam.Location = new System.Drawing.Point(151, 74);
            this.txtParam.Margin = new System.Windows.Forms.Padding(5);
            this.txtParam.Name = "txtParam";
            this.txtParam.Size = new System.Drawing.Size(298, 33);
            this.txtParam.TabIndex = 3;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(17, 79);
            this.label3.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(111, 25);
            this.label3.TabIndex = 2;
            this.label3.Text = "Parameters:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(17, 37);
            this.label2.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(101, 25);
            this.label2.TabIndex = 1;
            this.label2.Text = "Operation:";
            // 
            // cmbOp
            // 
            this.cmbOp.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbOp.FormattingEnabled = true;
            this.cmbOp.Items.AddRange(new object[] {
            "Set Command Text",
            "Set Note Text",
            "Get Note Text",
            "Set Shield Icon"});
            this.cmbOp.Location = new System.Drawing.Point(151, 32);
            this.cmbOp.Margin = new System.Windows.Forms.Padding(5);
            this.cmbOp.Name = "cmbOp";
            this.cmbOp.Size = new System.Drawing.Size(298, 33);
            this.cmbOp.TabIndex = 0;
            // 
            // commandLink1
            // 
            this.commandLink1.BackColor = System.Drawing.SystemColors.ControlDark;
            this.commandLink1.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.commandLink1.ForeColor = System.Drawing.Color.Maroon;
            this.commandLink1.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.commandLink1.Location = new System.Drawing.Point(14, 257);
            this.commandLink1.Margin = new System.Windows.Forms.Padding(5);
            this.commandLink1.Name = "commandLink1";
            this.commandLink1.NoteText = "";
            this.commandLink1.Size = new System.Drawing.Size(289, 74);
            this.commandLink1.TabIndex = 3;
            this.commandLink1.Text = "commandLink1";
            this.commandLink1.UseVisualStyleBackColor = false;
            this.commandLink1.Click += new System.EventHandler(this.OnClick);
            // 
            // MainForm
            // 
            this.AcceptButton = this.btnCall;
            this.AutoScaleDimensions = new System.Drawing.SizeF(11F, 25F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.ClientSize = new System.Drawing.Size(503, 400);
            this.Controls.Add(this.commandLink1);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label1);
            this.Font = new System.Drawing.Font("Segoe UI", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Margin = new System.Windows.Forms.Padding(5);
            this.Name = "MainForm";
            this.Text = "Command Link Test App";
            this.Load += new System.EventHandler(this.OnFormLoad);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnCall;
        private System.Windows.Forms.TextBox txtParam;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cmbOp;
        private CommandLinkWinForms commandLink1;
        private System.Windows.Forms.TextBox txtRetVal;
        private System.Windows.Forms.Label label4;




    }
}

