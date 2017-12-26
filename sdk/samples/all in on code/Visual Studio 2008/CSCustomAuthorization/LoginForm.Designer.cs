partial class LoginForm
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

    internal System.Windows.Forms.Label lbUserName;
    internal System.Windows.Forms.Label lbPassword;
    internal System.Windows.Forms.TextBox tbUserName;
    internal System.Windows.Forms.TextBox tbPassword;
    internal System.Windows.Forms.Button btnOK;
    internal System.Windows.Forms.Button btnCancel;

    //NOTE: The following procedure is required by the Windows Form Designer 
    //It can be modified using the Windows Form Designer. 
    //Do not modify it using the code editor. 
    [System.Diagnostics.DebuggerStepThrough()]
    private void InitializeComponent()
    {
        this.lbUserName = new System.Windows.Forms.Label();
        this.lbPassword = new System.Windows.Forms.Label();
        this.tbUserName = new System.Windows.Forms.TextBox();
        this.tbPassword = new System.Windows.Forms.TextBox();
        this.btnOK = new System.Windows.Forms.Button();
        this.btnCancel = new System.Windows.Forms.Button();
        this.SuspendLayout();
        // 
        // lbUserName
        // 
        this.lbUserName.Location = new System.Drawing.Point(10, 17);
        this.lbUserName.Name = "UsernameLabel";
        this.lbUserName.Size = new System.Drawing.Size(220, 23);
        this.lbUserName.TabIndex = 0;
        this.lbUserName.Text = "&User name";
        this.lbUserName.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
        // 
        // lbPassword
        // 
        this.lbPassword.Location = new System.Drawing.Point(10, 74);
        this.lbPassword.Name = "PasswordLabel";
        this.lbPassword.Size = new System.Drawing.Size(220, 23);
        this.lbPassword.TabIndex = 2;
        this.lbPassword.Text = "&Password";
        this.lbPassword.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
        // 
        // tbUserName
        // 
        this.tbUserName.Location = new System.Drawing.Point(12, 37);
        this.tbUserName.Name = "tbxUsername";
        this.tbUserName.Size = new System.Drawing.Size(220, 20);
        this.tbUserName.TabIndex = 1;
        // 
        // tbPassword
        // 
        this.tbPassword.Location = new System.Drawing.Point(12, 94);
        this.tbPassword.Name = "tbxPassword";
        this.tbPassword.PasswordChar = '*';
        this.tbPassword.Size = new System.Drawing.Size(220, 20);
        this.tbPassword.TabIndex = 3;
        // 
        // btnOK
        // 
        this.btnOK.Location = new System.Drawing.Point(35, 154);
        this.btnOK.Name = "btnOK";
        this.btnOK.Size = new System.Drawing.Size(94, 23);
        this.btnOK.TabIndex = 4;
        this.btnOK.Text = "&OK";
        this.btnOK.Click += new System.EventHandler(this.OK_Click);
        // 
        // btnCancel
        // 
        this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
        this.btnCancel.Location = new System.Drawing.Point(138, 154);
        this.btnCancel.Name = "btnCancel";
        this.btnCancel.Size = new System.Drawing.Size(94, 23);
        this.btnCancel.TabIndex = 5;
        this.btnCancel.Text = "&Cancel";
        this.btnCancel.Click += new System.EventHandler(this.Cancel_Click);
        // 
        // LoginForm
        // 
        this.AcceptButton = this.btnOK;
        this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.CancelButton = this.btnCancel;
        this.ClientSize = new System.Drawing.Size(248, 192);
        this.Controls.Add(this.btnCancel);
        this.Controls.Add(this.btnOK);
        this.Controls.Add(this.tbPassword);
        this.Controls.Add(this.tbUserName);
        this.Controls.Add(this.lbPassword);
        this.Controls.Add(this.lbUserName);
        this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
        this.MaximizeBox = false;
        this.MinimizeBox = false;
        this.Name = "LoginForm";
        this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
        this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
        this.Text = "LoginForm";
        this.ResumeLayout(false);
        this.PerformLayout();

    }
    #endregion
}