namespace Microsoft.Samples.FTPExplorer
{
    partial class CertificateValidationForm
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
            this.cmdNo = new System.Windows.Forms.Button();
            this.cmdYes = new System.Windows.Forms.Button();
            this.cmdViewCertificate = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // cmdNo
            // 
            this.cmdNo.BackColor = System.Drawing.Color.Bisque;
            this.cmdNo.FlatAppearance.BorderColor = System.Drawing.Color.DarkBlue;
            this.cmdNo.DialogResult = System.Windows.Forms.DialogResult.No;
            this.cmdNo.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmdNo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cmdNo.ForeColor = System.Drawing.Color.DarkBlue;
            this.cmdNo.Location = new System.Drawing.Point(33, 101);
            this.cmdNo.Name = "cmdNo";
            this.cmdNo.Size = new System.Drawing.Size(102, 23);
            this.cmdNo.TabIndex = 0;
            this.cmdNo.Text = "&No";
            // 
            // cmdYes
            // 
            this.cmdYes.BackColor = System.Drawing.Color.Bisque;
            this.cmdYes.FlatAppearance.BorderColor = System.Drawing.Color.DarkBlue;
            this.cmdYes.DialogResult = System.Windows.Forms.DialogResult.Yes;
            this.cmdYes.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmdYes.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cmdYes.ForeColor = System.Drawing.Color.DarkBlue;
            this.cmdYes.Location = new System.Drawing.Point(156, 101);
            this.cmdYes.Name = "cmdYes";
            this.cmdYes.Size = new System.Drawing.Size(102, 23);
            this.cmdYes.TabIndex = 1;
            this.cmdYes.Text = "&Yes";
            // 
            // cmdViewCertificate
            // 
            this.cmdViewCertificate.BackColor = System.Drawing.Color.Bisque;
            this.cmdViewCertificate.FlatAppearance.BorderColor = System.Drawing.Color.DarkBlue;
            this.cmdViewCertificate.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cmdViewCertificate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cmdViewCertificate.ForeColor = System.Drawing.Color.DarkBlue;
            this.cmdViewCertificate.Location = new System.Drawing.Point(274, 101);
            this.cmdViewCertificate.Name = "cmdViewCertificate";
            this.cmdViewCertificate.Size = new System.Drawing.Size(102, 23);
            this.cmdViewCertificate.TabIndex = 2;
            this.cmdViewCertificate.Text = "&View Certificate";
            this.cmdViewCertificate.Click += new System.EventHandler(this.cmdViewCertificate_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(41, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 14);
            this.label1.TabIndex = 3;
            this.label1.Text = "label1";
            // 
            // CertificateValidationForm
            // 
            this.ClientSize = new System.Drawing.Size(392, 134);
            this.ControlBox = false;
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cmdViewCertificate);
            this.Controls.Add(this.cmdYes);
            this.Controls.Add(this.cmdNo);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "CertificateValidationForm";
            this.Text = "Server Certificate Validation Error";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button cmdNo;
        private System.Windows.Forms.Button cmdYes;
        private System.Windows.Forms.Button cmdViewCertificate;
        private System.Windows.Forms.Label label1;
    }
}