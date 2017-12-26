namespace CSEncryption
{
    partial class AsymmetricAlgorithmForm
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
            this.btnNewRSAParameters = new System.Windows.Forms.Button();
            this.gbxRSAParameter = new System.Windows.Forms.GroupBox();
            this.tbxRSAParameters = new System.Windows.Forms.TextBox();
            this.btnEncrypt = new System.Windows.Forms.Button();
            this.gbxCipherTextasText = new System.Windows.Forms.GroupBox();
            this.tbxPlaintextasBytesArray = new System.Windows.Forms.TextBox();
            this.gbxCiphertextasByteArray = new System.Windows.Forms.GroupBox();
            this.tbxCiphertextasByteArray = new System.Windows.Forms.TextBox();
            this.btnDecrypt = new System.Windows.Forms.Button();
            this.gbxPlainText = new System.Windows.Forms.GroupBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.tbxInputMessage = new System.Windows.Forms.TextBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.tbxInputInterger = new System.Windows.Forms.TextBox();
            this.gbxRecoveredPlainText = new System.Windows.Forms.GroupBox();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.tbxMessagePart = new System.Windows.Forms.TextBox();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.tbxIntegerPart = new System.Windows.Forms.TextBox();
            this.gbxRSAParameter.SuspendLayout();
            this.gbxCipherTextasText.SuspendLayout();
            this.gbxCiphertextasByteArray.SuspendLayout();
            this.gbxPlainText.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.gbxRecoveredPlainText.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnNewRSAParameters
            // 
            this.btnNewRSAParameters.Location = new System.Drawing.Point(30, 9);
            this.btnNewRSAParameters.Name = "btnNewRSAParameters";
            this.btnNewRSAParameters.Size = new System.Drawing.Size(75, 23);
            this.btnNewRSAParameters.TabIndex = 0;
            this.btnNewRSAParameters.Text = "New RSA Parameters";
            this.btnNewRSAParameters.UseVisualStyleBackColor = true;
            this.btnNewRSAParameters.Click += new System.EventHandler(this.btnNewRSAParameters_Click);
            // 
            // gbxRSAParameter
            // 
            this.gbxRSAParameter.Controls.Add(this.tbxRSAParameters);
            this.gbxRSAParameter.Location = new System.Drawing.Point(30, 43);
            this.gbxRSAParameter.Name = "gbxRSAParameter";
            this.gbxRSAParameter.Size = new System.Drawing.Size(638, 88);
            this.gbxRSAParameter.TabIndex = 1;
            this.gbxRSAParameter.TabStop = false;
            this.gbxRSAParameter.Text = "RSA Parameters";
            // 
            // tbxRSAParameters
            // 
            this.tbxRSAParameters.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxRSAParameters.Location = new System.Drawing.Point(3, 16);
            this.tbxRSAParameters.Multiline = true;
            this.tbxRSAParameters.Name = "tbxRSAParameters";
            this.tbxRSAParameters.ReadOnly = true;
            this.tbxRSAParameters.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxRSAParameters.Size = new System.Drawing.Size(632, 69);
            this.tbxRSAParameters.TabIndex = 0;
            // 
            // btnEncrypt
            // 
            this.btnEncrypt.Location = new System.Drawing.Point(30, 225);
            this.btnEncrypt.Name = "btnEncrypt";
            this.btnEncrypt.Size = new System.Drawing.Size(75, 23);
            this.btnEncrypt.TabIndex = 3;
            this.btnEncrypt.Text = "Encrypt";
            this.btnEncrypt.UseVisualStyleBackColor = true;
            this.btnEncrypt.Click += new System.EventHandler(this.btnEncrypt_Click);
            // 
            // gbxCipherTextasText
            // 
            this.gbxCipherTextasText.Controls.Add(this.tbxPlaintextasBytesArray);
            this.gbxCipherTextasText.Location = new System.Drawing.Point(30, 259);
            this.gbxCipherTextasText.Name = "gbxCipherTextasText";
            this.gbxCipherTextasText.Size = new System.Drawing.Size(632, 62);
            this.gbxCipherTextasText.TabIndex = 4;
            this.gbxCipherTextasText.TabStop = false;
            this.gbxCipherTextasText.Text = "PlainText displayed as Byte array";
            // 
            // tbxPlaintextasBytesArray
            // 
            this.tbxPlaintextasBytesArray.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxPlaintextasBytesArray.Location = new System.Drawing.Point(3, 16);
            this.tbxPlaintextasBytesArray.Multiline = true;
            this.tbxPlaintextasBytesArray.Name = "tbxPlaintextasBytesArray";
            this.tbxPlaintextasBytesArray.ReadOnly = true;
            this.tbxPlaintextasBytesArray.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxPlaintextasBytesArray.Size = new System.Drawing.Size(626, 43);
            this.tbxPlaintextasBytesArray.TabIndex = 0;
            // 
            // gbxCiphertextasByteArray
            // 
            this.gbxCiphertextasByteArray.Controls.Add(this.tbxCiphertextasByteArray);
            this.gbxCiphertextasByteArray.Location = new System.Drawing.Point(30, 332);
            this.gbxCiphertextasByteArray.Name = "gbxCiphertextasByteArray";
            this.gbxCiphertextasByteArray.Size = new System.Drawing.Size(632, 54);
            this.gbxCiphertextasByteArray.TabIndex = 5;
            this.gbxCiphertextasByteArray.TabStop = false;
            this.gbxCiphertextasByteArray.Text = "Ciphertext displayed as Byte Array";
            // 
            // tbxCiphertextasByteArray
            // 
            this.tbxCiphertextasByteArray.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxCiphertextasByteArray.Location = new System.Drawing.Point(3, 16);
            this.tbxCiphertextasByteArray.Multiline = true;
            this.tbxCiphertextasByteArray.Name = "tbxCiphertextasByteArray";
            this.tbxCiphertextasByteArray.ReadOnly = true;
            this.tbxCiphertextasByteArray.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxCiphertextasByteArray.Size = new System.Drawing.Size(626, 35);
            this.tbxCiphertextasByteArray.TabIndex = 0;
            // 
            // btnDecrypt
            // 
            this.btnDecrypt.Location = new System.Drawing.Point(30, 397);
            this.btnDecrypt.Name = "btnDecrypt";
            this.btnDecrypt.Size = new System.Drawing.Size(75, 23);
            this.btnDecrypt.TabIndex = 6;
            this.btnDecrypt.Text = "Decrypt";
            this.btnDecrypt.UseVisualStyleBackColor = true;
            this.btnDecrypt.Click += new System.EventHandler(this.btnDecrypt_Click);
            // 
            // gbxPlainText
            // 
            this.gbxPlainText.Controls.Add(this.groupBox5);
            this.gbxPlainText.Controls.Add(this.groupBox4);
            this.gbxPlainText.Location = new System.Drawing.Point(30, 142);
            this.gbxPlainText.Name = "gbxPlainText";
            this.gbxPlainText.Size = new System.Drawing.Size(632, 72);
            this.gbxPlainText.TabIndex = 10;
            this.gbxPlainText.TabStop = false;
            this.gbxPlainText.Text = "Plaintext";
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.tbxInputMessage);
            this.groupBox5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox5.Location = new System.Drawing.Point(195, 16);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(434, 53);
            this.groupBox5.TabIndex = 1;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Input message";
            // 
            // tbxInputMessage
            // 
            this.tbxInputMessage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxInputMessage.Location = new System.Drawing.Point(3, 16);
            this.tbxInputMessage.Multiline = true;
            this.tbxInputMessage.Name = "tbxInputMessage";
            this.tbxInputMessage.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxInputMessage.Size = new System.Drawing.Size(428, 34);
            this.tbxInputMessage.TabIndex = 0;
            this.tbxInputMessage.TextChanged += new System.EventHandler(this.tbxInputMessage_TextChanged);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.tbxInputInterger);
            this.groupBox4.Dock = System.Windows.Forms.DockStyle.Left;
            this.groupBox4.Location = new System.Drawing.Point(3, 16);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(192, 53);
            this.groupBox4.TabIndex = 0;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Input interger";
            // 
            // tbxInputInterger
            // 
            this.tbxInputInterger.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxInputInterger.Location = new System.Drawing.Point(3, 16);
            this.tbxInputInterger.Multiline = true;
            this.tbxInputInterger.Name = "tbxInputInterger";
            this.tbxInputInterger.Size = new System.Drawing.Size(186, 34);
            this.tbxInputInterger.TabIndex = 0;
            this.tbxInputInterger.TextChanged += new System.EventHandler(this.tbxInputInterger_TextChanged);
            // 
            // gbxRecoveredPlainText
            // 
            this.gbxRecoveredPlainText.Controls.Add(this.groupBox7);
            this.gbxRecoveredPlainText.Controls.Add(this.groupBox6);
            this.gbxRecoveredPlainText.Location = new System.Drawing.Point(30, 431);
            this.gbxRecoveredPlainText.Name = "gbxRecoveredPlainText";
            this.gbxRecoveredPlainText.Size = new System.Drawing.Size(632, 75);
            this.gbxRecoveredPlainText.TabIndex = 13;
            this.gbxRecoveredPlainText.TabStop = false;
            this.gbxRecoveredPlainText.Text = "Recovered Plaintext";
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.tbxMessagePart);
            this.groupBox7.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox7.Location = new System.Drawing.Point(195, 16);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(434, 56);
            this.groupBox7.TabIndex = 2;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "message Part";
            // 
            // tbxMessagePart
            // 
            this.tbxMessagePart.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxMessagePart.Location = new System.Drawing.Point(3, 16);
            this.tbxMessagePart.Multiline = true;
            this.tbxMessagePart.Name = "tbxMessagePart";
            this.tbxMessagePart.ReadOnly = true;
            this.tbxMessagePart.Size = new System.Drawing.Size(428, 37);
            this.tbxMessagePart.TabIndex = 0;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.tbxIntegerPart);
            this.groupBox6.Dock = System.Windows.Forms.DockStyle.Left;
            this.groupBox6.Location = new System.Drawing.Point(3, 16);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(192, 56);
            this.groupBox6.TabIndex = 1;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Interger part";
            // 
            // tbxIntegerPart
            // 
            this.tbxIntegerPart.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxIntegerPart.Location = new System.Drawing.Point(3, 16);
            this.tbxIntegerPart.Multiline = true;
            this.tbxIntegerPart.Name = "tbxIntegerPart";
            this.tbxIntegerPart.ReadOnly = true;
            this.tbxIntegerPart.Size = new System.Drawing.Size(186, 37);
            this.tbxIntegerPart.TabIndex = 0;
            // 
            // formAsymmetricAlgorithm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(682, 540);
            this.Controls.Add(this.gbxRecoveredPlainText);
            this.Controls.Add(this.gbxPlainText);
            this.Controls.Add(this.btnDecrypt);
            this.Controls.Add(this.gbxCiphertextasByteArray);
            this.Controls.Add(this.gbxCipherTextasText);
            this.Controls.Add(this.btnEncrypt);
            this.Controls.Add(this.gbxRSAParameter);
            this.Controls.Add(this.btnNewRSAParameters);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "formAsymmetricAlgorithm";
            this.Text = "AsymmetricAlgorithm";
            this.Load += new System.EventHandler(this.AsymmetricAlgorithmForm_Load);
            this.gbxRSAParameter.ResumeLayout(false);
            this.gbxRSAParameter.PerformLayout();
            this.gbxCipherTextasText.ResumeLayout(false);
            this.gbxCipherTextasText.PerformLayout();
            this.gbxCiphertextasByteArray.ResumeLayout(false);
            this.gbxCiphertextasByteArray.PerformLayout();
            this.gbxPlainText.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.gbxRecoveredPlainText.ResumeLayout(false);
            this.groupBox7.ResumeLayout(false);
            this.groupBox7.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnNewRSAParameters;
        private System.Windows.Forms.GroupBox gbxRSAParameter;
        private System.Windows.Forms.Button btnEncrypt;
        private System.Windows.Forms.GroupBox gbxCipherTextasText;
        private System.Windows.Forms.GroupBox gbxCiphertextasByteArray;
        private System.Windows.Forms.Button btnDecrypt;
        private System.Windows.Forms.TextBox tbxRSAParameters;
        private System.Windows.Forms.TextBox tbxPlaintextasBytesArray;
        private System.Windows.Forms.TextBox tbxCiphertextasByteArray;
        private System.Windows.Forms.GroupBox gbxPlainText;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.TextBox tbxInputMessage;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.TextBox tbxInputInterger;
        private System.Windows.Forms.GroupBox gbxRecoveredPlainText;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.TextBox tbxMessagePart;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.TextBox tbxIntegerPart;

    }
}