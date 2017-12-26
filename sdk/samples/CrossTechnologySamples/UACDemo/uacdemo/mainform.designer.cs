namespace UACDemo
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
           this.label4 = new System.Windows.Forms.Label();
           this.txtUser = new System.Windows.Forms.TextBox();
           this.label3 = new System.Windows.Forms.Label();
           this.label2 = new System.Windows.Forms.Label();
           this.label1 = new System.Windows.Forms.Label();
           this.button3 = new System.Windows.Forms.Button();
           this.button2 = new System.Windows.Forms.Button();
           this.button1 = new System.Windows.Forms.Button();
           this.SuspendLayout();
           // 
           // label4
           // 
           this.label4.AutoSize = true;
           this.label4.Location = new System.Drawing.Point(293, 271);
           this.label4.Name = "label4";
           this.label4.Size = new System.Drawing.Size(288, 25);
           this.label4.TabIndex = 18;
           this.label4.Text = "Check for group membership";
           // 
           // txtUser
           // 
           this.txtUser.Location = new System.Drawing.Point(158, 22);
           this.txtUser.Name = "txtUser";
           this.txtUser.ReadOnly = true;
           this.txtUser.Size = new System.Drawing.Size(380, 31);
           this.txtUser.TabIndex = 17;
           // 
           // label3
           // 
           this.label3.AutoSize = true;
           this.label3.Location = new System.Drawing.Point(15, 25);
           this.label3.Name = "label3";
           this.label3.Size = new System.Drawing.Size(137, 25);
           this.label3.TabIndex = 16;
           this.label3.Text = "Current user:";
           // 
           // label2
           // 
           this.label2.AutoSize = true;
           this.label2.Location = new System.Drawing.Point(293, 184);
           this.label2.Name = "label2";
           this.label2.Size = new System.Drawing.Size(497, 25);
           this.label2.TabIndex = 15;
           this.label2.Text = "Write to HKLM\\SOFTWARE\\UACDemo\\SampleKey";
           // 
           // label1
           // 
           this.label1.AutoSize = true;
           this.label1.Location = new System.Drawing.Point(293, 91);
           this.label1.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
           this.label1.Name = "label1";
           this.label1.Size = new System.Drawing.Size(578, 25);
           this.label1.TabIndex = 14;
           this.label1.Text = "Write a file to %sysdrive%\\Program Files\\UACDemo\\Log.txt";
           // 
           // button3
           // 
           this.button3.Location = new System.Drawing.Point(15, 261);
           this.button3.Margin = new System.Windows.Forms.Padding(6);
           this.button3.Name = "button3";
           this.button3.Size = new System.Drawing.Size(260, 44);
           this.button3.TabIndex = 13;
           this.button3.Text = "Is Administrator?";
           this.button3.UseVisualStyleBackColor = true;
           this.button3.Click += new System.EventHandler(this.OnIsAdministrator);
           // 
           // button2
           // 
           this.button2.FlatStyle = System.Windows.Forms.FlatStyle.System;
           this.button2.Location = new System.Drawing.Point(15, 174);
           this.button2.Margin = new System.Windows.Forms.Padding(6);
           this.button2.Name = "button2";
           this.button2.Size = new System.Drawing.Size(260, 44);
           this.button2.TabIndex = 12;
           this.button2.Text = "Write to Registry HKLM";
           this.button2.UseVisualStyleBackColor = true;
           this.button2.Click += new System.EventHandler(this.OnWriteToHKLM);
           // 
           // button1
           // 
           this.button1.FlatStyle = System.Windows.Forms.FlatStyle.System;
           this.button1.Location = new System.Drawing.Point(15, 81);
           this.button1.Margin = new System.Windows.Forms.Padding(6);
           this.button1.Name = "button1";
           this.button1.Size = new System.Drawing.Size(260, 44);
           this.button1.TabIndex = 11;
           this.button1.Text = "Write to Program Files";
           this.button1.UseVisualStyleBackColor = true;
           this.button1.Click += new System.EventHandler(this.OnLaunchPrivilegedExecutable);
           // 
           // MainForm
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(12F, 25F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.ClientSize = new System.Drawing.Size(888, 342);
           this.Controls.Add(this.label4);
           this.Controls.Add(this.txtUser);
           this.Controls.Add(this.label3);
           this.Controls.Add(this.label2);
           this.Controls.Add(this.label1);
           this.Controls.Add(this.button3);
           this.Controls.Add(this.button2);
           this.Controls.Add(this.button1);
           this.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.Margin = new System.Windows.Forms.Padding(6);
           this.Name = "MainForm";
           this.Text = "User Account Control - Simple Demo";
           this.Load += new System.EventHandler(this.OnFormLoad);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtUser;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button1;
    }
}

