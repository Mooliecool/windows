namespace CSXmlDigitalSignature
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
            this.btnSign = new System.Windows.Forms.Button();
            this.btnVerify = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.tbxPlaintext = new System.Windows.Forms.TextBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.tbxDigitalSignature = new System.Windows.Forms.TextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.tbxRSAParameters = new System.Windows.Forms.TextBox();
            this.btnChangeXML = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.tbxModifiedMessage = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnSign
            // 
            this.btnSign.Location = new System.Drawing.Point(12, 12);
            this.btnSign.Name = "btnSign";
            this.btnSign.Size = new System.Drawing.Size(90, 32);
            this.btnSign.TabIndex = 1;
            this.btnSign.Text = "Sign";
            this.btnSign.UseVisualStyleBackColor = true;
            this.btnSign.Click += new System.EventHandler(this.btnSign_Click);
            // 
            // btnVerify
            // 
            this.btnVerify.Location = new System.Drawing.Point(12, 127);
            this.btnVerify.Name = "btnVerify";
            this.btnVerify.Size = new System.Drawing.Size(90, 32);
            this.btnVerify.TabIndex = 5;
            this.btnVerify.Text = "Verify";
            this.btnVerify.UseVisualStyleBackColor = true;
            this.btnVerify.Click += new System.EventHandler(this.btnVerify_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.tbxPlaintext);
            this.groupBox1.Location = new System.Drawing.Point(122, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(599, 96);
            this.groupBox1.TabIndex = 6;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Original Message to be signed";
            // 
            // tbxPlaintext
            // 
            this.tbxPlaintext.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxPlaintext.Location = new System.Drawing.Point(3, 16);
            this.tbxPlaintext.Multiline = true;
            this.tbxPlaintext.Name = "tbxPlaintext";
            this.tbxPlaintext.ReadOnly = true;
            this.tbxPlaintext.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxPlaintext.Size = new System.Drawing.Size(593, 77);
            this.tbxPlaintext.TabIndex = 0;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.tbxDigitalSignature);
            this.groupBox3.Location = new System.Drawing.Point(122, 127);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(599, 96);
            this.groupBox3.TabIndex = 7;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Digital Signature(Encrypted Message Digest)";
            // 
            // tbxDigitalSignature
            // 
            this.tbxDigitalSignature.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxDigitalSignature.Location = new System.Drawing.Point(3, 16);
            this.tbxDigitalSignature.Multiline = true;
            this.tbxDigitalSignature.Name = "tbxDigitalSignature";
            this.tbxDigitalSignature.ReadOnly = true;
            this.tbxDigitalSignature.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxDigitalSignature.Size = new System.Drawing.Size(593, 77);
            this.tbxDigitalSignature.TabIndex = 0;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.tbxRSAParameters);
            this.groupBox2.Location = new System.Drawing.Point(24, 363);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(706, 224);
            this.groupBox2.TabIndex = 8;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "RSA parameters";
            // 
            // tbxRSAParameters
            // 
            this.tbxRSAParameters.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxRSAParameters.Location = new System.Drawing.Point(3, 16);
            this.tbxRSAParameters.Multiline = true;
            this.tbxRSAParameters.Name = "tbxRSAParameters";
            this.tbxRSAParameters.ReadOnly = true;
            this.tbxRSAParameters.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxRSAParameters.Size = new System.Drawing.Size(700, 205);
            this.tbxRSAParameters.TabIndex = 0;
            // 
            // btnChangeXML
            // 
            this.btnChangeXML.Location = new System.Drawing.Point(15, 245);
            this.btnChangeXML.Name = "btnChangeXML";
            this.btnChangeXML.Size = new System.Drawing.Size(90, 32);
            this.btnChangeXML.TabIndex = 9;
            this.btnChangeXML.Text = "change Xml ";
            this.btnChangeXML.UseVisualStyleBackColor = true;
            this.btnChangeXML.Click += new System.EventHandler(this.btnChangeXML_Click);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.tbxModifiedMessage);
            this.groupBox4.Location = new System.Drawing.Point(122, 245);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(599, 96);
            this.groupBox4.TabIndex = 10;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Modified Message";
            // 
            // tbxModifiedMessage
            // 
            this.tbxModifiedMessage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxModifiedMessage.Location = new System.Drawing.Point(3, 16);
            this.tbxModifiedMessage.Multiline = true;
            this.tbxModifiedMessage.Name = "tbxModifiedMessage";
            this.tbxModifiedMessage.ReadOnly = true;
            this.tbxModifiedMessage.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxModifiedMessage.Size = new System.Drawing.Size(593, 77);
            this.tbxModifiedMessage.TabIndex = 0;
            // 
            // XMLDigitalSignatureForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(742, 619);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.btnChangeXML);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.btnVerify);
            this.Controls.Add(this.btnSign);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "XMLDigitalSignatureForm";
            this.Text = "XMLDigitalSignature";
            this.Load += new System.EventHandler(this.XMLDigitalSignatureForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnSign;
        private System.Windows.Forms.Button btnVerify;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox tbxPlaintext;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.TextBox tbxDigitalSignature;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox tbxRSAParameters;
        private System.Windows.Forms.Button btnChangeXML;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.TextBox tbxModifiedMessage;
    }
}