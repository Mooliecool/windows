namespace CSXmlEncryption
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
            this.btnEncrypt = new System.Windows.Forms.Button();
            this.gbxRSAParameter = new System.Windows.Forms.GroupBox();
            this.tbxRSAParameters = new System.Windows.Forms.TextBox();
            this.gbxPlaintextt = new System.Windows.Forms.GroupBox();
            this.tbxPlaintext = new System.Windows.Forms.TextBox();
            this.gbxCipherTextasText = new System.Windows.Forms.GroupBox();
            this.tbxCipherTextasText = new System.Windows.Forms.TextBox();
            this.btnDecrypt = new System.Windows.Forms.Button();
            this.gbxRecoveredPlaintext = new System.Windows.Forms.GroupBox();
            this.tbxRecoveredPlaintext = new System.Windows.Forms.TextBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.tbxModulus = new System.Windows.Forms.TextBox();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.tbxExponent = new System.Windows.Forms.TextBox();
            this.gbxRSAParameter.SuspendLayout();
            this.gbxPlaintextt.SuspendLayout();
            this.gbxCipherTextasText.SuspendLayout();
            this.gbxRecoveredPlaintext.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnEncrypt
            // 
            this.btnEncrypt.Location = new System.Drawing.Point(28, 205);
            this.btnEncrypt.Name = "btnEncrypt";
            this.btnEncrypt.Size = new System.Drawing.Size(75, 23);
            this.btnEncrypt.TabIndex = 1;
            this.btnEncrypt.Text = "Encrypt";
            this.btnEncrypt.UseVisualStyleBackColor = true;
            this.btnEncrypt.Click += new System.EventHandler(this.btnEncrypt_Click);
            // 
            // gbxRSAParameter
            // 
            this.gbxRSAParameter.Controls.Add(this.tbxRSAParameters);
            this.gbxRSAParameter.Location = new System.Drawing.Point(28, 12);
            this.gbxRSAParameter.Name = "gbxRSAParameter";
            this.gbxRSAParameter.Size = new System.Drawing.Size(638, 119);
            this.gbxRSAParameter.TabIndex = 2;
            this.gbxRSAParameter.TabStop = false;
            this.gbxRSAParameter.Text = "RSA Parameters";
            // 
            // tbxRSAParameters
            // 
            this.tbxRSAParameters.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxRSAParameters.Location = new System.Drawing.Point(3, 16);
            this.tbxRSAParameters.Multiline = true;
            this.tbxRSAParameters.Name = "tbxRSAParameters";
            this.tbxRSAParameters.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxRSAParameters.Size = new System.Drawing.Size(632, 100);
            this.tbxRSAParameters.TabIndex = 0;
            // 
            // gbxPlaintextt
            // 
            this.gbxPlaintextt.Controls.Add(this.tbxPlaintext);
            this.gbxPlaintextt.Location = new System.Drawing.Point(28, 141);
            this.gbxPlaintextt.Name = "gbxPlaintextt";
            this.gbxPlaintextt.Size = new System.Drawing.Size(638, 54);
            this.gbxPlaintextt.TabIndex = 3;
            this.gbxPlaintextt.TabStop = false;
            this.gbxPlaintextt.Text = "Plain Xml";
            // 
            // tbxPlaintext
            // 
            this.tbxPlaintext.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxPlaintext.Location = new System.Drawing.Point(3, 16);
            this.tbxPlaintext.Multiline = true;
            this.tbxPlaintext.Name = "tbxPlaintext";
            this.tbxPlaintext.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxPlaintext.Size = new System.Drawing.Size(632, 35);
            this.tbxPlaintext.TabIndex = 0;
            // 
            // gbxCipherTextasText
            // 
            this.gbxCipherTextasText.Controls.Add(this.tbxCipherTextasText);
            this.gbxCipherTextasText.Location = new System.Drawing.Point(28, 238);
            this.gbxCipherTextasText.Name = "gbxCipherTextasText";
            this.gbxCipherTextasText.Size = new System.Drawing.Size(638, 62);
            this.gbxCipherTextasText.TabIndex = 5;
            this.gbxCipherTextasText.TabStop = false;
            this.gbxCipherTextasText.Text = "Ciphertext displayed as text string";
            // 
            // tbxCipherTextasText
            // 
            this.tbxCipherTextasText.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxCipherTextasText.Location = new System.Drawing.Point(3, 16);
            this.tbxCipherTextasText.Multiline = true;
            this.tbxCipherTextasText.Name = "tbxCipherTextasText";
            this.tbxCipherTextasText.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxCipherTextasText.Size = new System.Drawing.Size(632, 43);
            this.tbxCipherTextasText.TabIndex = 0;
            // 
            // btnDecrypt
            // 
            this.btnDecrypt.Location = new System.Drawing.Point(28, 310);
            this.btnDecrypt.Name = "btnDecrypt";
            this.btnDecrypt.Size = new System.Drawing.Size(75, 23);
            this.btnDecrypt.TabIndex = 7;
            this.btnDecrypt.Text = "Decrypt";
            this.btnDecrypt.UseVisualStyleBackColor = true;
            this.btnDecrypt.Click += new System.EventHandler(this.btnDecrypt_Click);
            // 
            // gbxRecoveredPlaintext
            // 
            this.gbxRecoveredPlaintext.Controls.Add(this.tbxRecoveredPlaintext);
            this.gbxRecoveredPlaintext.Location = new System.Drawing.Point(28, 343);
            this.gbxRecoveredPlaintext.Name = "gbxRecoveredPlaintext";
            this.gbxRecoveredPlaintext.Size = new System.Drawing.Size(638, 81);
            this.gbxRecoveredPlaintext.TabIndex = 8;
            this.gbxRecoveredPlaintext.TabStop = false;
            this.gbxRecoveredPlaintext.Text = "Recovered Plaintext";
            // 
            // tbxRecoveredPlaintext
            // 
            this.tbxRecoveredPlaintext.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxRecoveredPlaintext.Location = new System.Drawing.Point(3, 16);
            this.tbxRecoveredPlaintext.Multiline = true;
            this.tbxRecoveredPlaintext.Name = "tbxRecoveredPlaintext";
            this.tbxRecoveredPlaintext.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxRecoveredPlaintext.Size = new System.Drawing.Size(632, 62);
            this.tbxRecoveredPlaintext.TabIndex = 0;
            // 
            // groupBox4
            // 
            this.groupBox4.Location = new System.Drawing.Point(0, 0);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(200, 100);
            this.groupBox4.TabIndex = 0;
            this.groupBox4.TabStop = false;
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 1;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Controls.Add(this.groupBox5, 0, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(200, 100);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.tbxModulus);
            this.groupBox5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox5.Location = new System.Drawing.Point(3, 3);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(194, 14);
            this.groupBox5.TabIndex = 0;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Modulus";
            // 
            // tbxModulus
            // 
            this.tbxModulus.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxModulus.Enabled = false;
            this.tbxModulus.Location = new System.Drawing.Point(3, 16);
            this.tbxModulus.Multiline = true;
            this.tbxModulus.Name = "tbxModulus";
            this.tbxModulus.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxModulus.Size = new System.Drawing.Size(188, 0);
            this.tbxModulus.TabIndex = 0;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.tbxExponent);
            this.groupBox6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox6.Location = new System.Drawing.Point(3, 153);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(700, 144);
            this.groupBox6.TabIndex = 1;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Exponent";
            // 
            // tbxExponent
            // 
            this.tbxExponent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxExponent.Enabled = false;
            this.tbxExponent.Location = new System.Drawing.Point(3, 16);
            this.tbxExponent.Multiline = true;
            this.tbxExponent.Name = "tbxExponent";
            this.tbxExponent.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxExponent.Size = new System.Drawing.Size(694, 125);
            this.tbxExponent.TabIndex = 0;
            // 
            // formXMLEncryptDecrypt
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(669, 446);
            this.Controls.Add(this.gbxRecoveredPlaintext);
            this.Controls.Add(this.btnDecrypt);
            this.Controls.Add(this.gbxCipherTextasText);
            this.Controls.Add(this.gbxPlaintextt);
            this.Controls.Add(this.gbxRSAParameter);
            this.Controls.Add(this.btnEncrypt);
            this.Name = "formXMLEncryptDecrypt";
            this.Text = "XMLEncryptDecrypt";
            this.Load += new System.EventHandler(this.XMLEncryptDecryptForm_Load);
            this.gbxRSAParameter.ResumeLayout(false);
            this.gbxRSAParameter.PerformLayout();
            this.gbxPlaintextt.ResumeLayout(false);
            this.gbxPlaintextt.PerformLayout();
            this.gbxCipherTextasText.ResumeLayout(false);
            this.gbxCipherTextasText.PerformLayout();
            this.gbxRecoveredPlaintext.ResumeLayout(false);
            this.gbxRecoveredPlaintext.PerformLayout();
            this.tableLayoutPanel1.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnEncrypt;
        private System.Windows.Forms.GroupBox gbxRSAParameter;
        private System.Windows.Forms.TextBox tbxRSAParameters;
        private System.Windows.Forms.GroupBox gbxPlaintextt;
        private System.Windows.Forms.TextBox tbxPlaintext;
        private System.Windows.Forms.GroupBox gbxCipherTextasText;
        private System.Windows.Forms.TextBox tbxCipherTextasText;
        private System.Windows.Forms.Button btnDecrypt;
        private System.Windows.Forms.GroupBox gbxRecoveredPlaintext;
        private System.Windows.Forms.TextBox tbxRecoveredPlaintext;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.TextBox tbxModulus;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.TextBox tbxExponent;
    }
}