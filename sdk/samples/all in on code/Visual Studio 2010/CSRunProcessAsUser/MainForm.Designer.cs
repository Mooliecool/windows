namespace CSRunProcessAsUser
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
            this.btnCommand = new System.Windows.Forms.Button();
            this.tbCommand = new System.Windows.Forms.TextBox();
            this.tbPassword = new System.Windows.Forms.TextBox();
            this.tbDomain = new System.Windows.Forms.TextBox();
            this.lbPassword = new System.Windows.Forms.Label();
            this.lbDomain = new System.Windows.Forms.Label();
            this.lbUserName = new System.Windows.Forms.Label();
            this.btnRunCommand = new System.Windows.Forms.Button();
            this.btnCredentialUIPrompt = new System.Windows.Forms.Button();
            this.tbUserName = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // btnCommand
            // 
            this.btnCommand.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnCommand.Location = new System.Drawing.Point(23, 118);
            this.btnCommand.Name = "btnCommand";
            this.btnCommand.Size = new System.Drawing.Size(64, 23);
            this.btnCommand.TabIndex = 5;
            this.btnCommand.Text = "Command...";
            this.btnCommand.Click += new System.EventHandler(this.btnCommand_Click);
            // 
            // tbCommand
            // 
            this.tbCommand.Location = new System.Drawing.Point(87, 118);
            this.tbCommand.Name = "tbCommand";
            this.tbCommand.Size = new System.Drawing.Size(184, 20);
            this.tbCommand.TabIndex = 6;
            // 
            // tbPassword
            // 
            this.tbPassword.Location = new System.Drawing.Point(87, 86);
            this.tbPassword.Name = "tbPassword";
            this.tbPassword.Size = new System.Drawing.Size(184, 20);
            this.tbPassword.TabIndex = 4;
            this.tbPassword.UseSystemPasswordChar = true;
            // 
            // tbDomain
            // 
            this.tbDomain.Location = new System.Drawing.Point(87, 51);
            this.tbDomain.Name = "tbDomain";
            this.tbDomain.Size = new System.Drawing.Size(184, 20);
            this.tbDomain.TabIndex = 3;
            // 
            // lbPassword
            // 
            this.lbPassword.Location = new System.Drawing.Point(23, 86);
            this.lbPassword.Name = "lbPassword";
            this.lbPassword.Size = new System.Drawing.Size(64, 23);
            this.lbPassword.TabIndex = 43;
            this.lbPassword.Text = "Password";
            // 
            // lbDomain
            // 
            this.lbDomain.Location = new System.Drawing.Point(23, 54);
            this.lbDomain.Name = "lbDomain";
            this.lbDomain.Size = new System.Drawing.Size(56, 23);
            this.lbDomain.TabIndex = 42;
            this.lbDomain.Text = "Domain";
            // 
            // lbUserName
            // 
            this.lbUserName.Location = new System.Drawing.Point(23, 22);
            this.lbUserName.Name = "lbUserName";
            this.lbUserName.Size = new System.Drawing.Size(64, 23);
            this.lbUserName.TabIndex = 40;
            this.lbUserName.Text = "User Name";
            // 
            // btnRunCommand
            // 
            this.btnRunCommand.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnRunCommand.Location = new System.Drawing.Point(87, 144);
            this.btnRunCommand.Name = "btnRunCommand";
            this.btnRunCommand.Size = new System.Drawing.Size(128, 24);
            this.btnRunCommand.TabIndex = 7;
            this.btnRunCommand.Text = "Run Command";
            this.btnRunCommand.Click += new System.EventHandler(this.btnRunCommand_Click);
            // 
            // btnCredentialUIPrompt
            // 
            this.btnCredentialUIPrompt.Location = new System.Drawing.Point(277, 17);
            this.btnCredentialUIPrompt.Name = "btnCredentialUIPrompt";
            this.btnCredentialUIPrompt.Size = new System.Drawing.Size(39, 23);
            this.btnCredentialUIPrompt.TabIndex = 2;
            this.btnCredentialUIPrompt.Text = "...";
            this.btnCredentialUIPrompt.UseVisualStyleBackColor = true;
            this.btnCredentialUIPrompt.Click += new System.EventHandler(this.btnCredentialUIPrompt_Click);
            // 
            // tbUserName
            // 
            this.tbUserName.Location = new System.Drawing.Point(87, 19);
            this.tbUserName.Name = "tbUserName";
            this.tbUserName.Size = new System.Drawing.Size(184, 20);
            this.tbUserName.TabIndex = 1;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(330, 198);
            this.Controls.Add(this.tbUserName);
            this.Controls.Add(this.btnCredentialUIPrompt);
            this.Controls.Add(this.btnCommand);
            this.Controls.Add(this.tbCommand);
            this.Controls.Add(this.tbPassword);
            this.Controls.Add(this.tbDomain);
            this.Controls.Add(this.lbPassword);
            this.Controls.Add(this.lbDomain);
            this.Controls.Add(this.lbUserName);
            this.Controls.Add(this.btnRunCommand);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "MainForm";
            this.Text = "RunProcessAsUser";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnCommand;
        private System.Windows.Forms.TextBox tbCommand;
        private System.Windows.Forms.TextBox tbPassword;
        private System.Windows.Forms.TextBox tbDomain;
        private System.Windows.Forms.Label lbPassword;
        private System.Windows.Forms.Label lbDomain;
        private System.Windows.Forms.Label lbUserName;
        private System.Windows.Forms.Button btnRunCommand;
        private System.Windows.Forms.Button btnCredentialUIPrompt;
        private System.Windows.Forms.TextBox tbUserName;
    }
}

