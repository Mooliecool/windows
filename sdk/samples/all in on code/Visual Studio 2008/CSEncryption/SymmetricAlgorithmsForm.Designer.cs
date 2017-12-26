namespace CSEncryption
{
    partial class SymmetricAlgorithmForm
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.radiobtnRC2 = new System.Windows.Forms.RadioButton();
            this.radiobtnRijndael = new System.Windows.Forms.RadioButton();
            this.radiobtnTrippleDES = new System.Windows.Forms.RadioButton();
            this.radiobtnDES = new System.Windows.Forms.RadioButton();
            this.btnNewRandomKey = new System.Windows.Forms.Button();
            this.btnNewRandomInitVector = new System.Windows.Forms.Button();
            this.tbxRandomKey = new System.Windows.Forms.TextBox();
            this.tbxRandomInitVector = new System.Windows.Forms.TextBox();
            this.btnEncrypt = new System.Windows.Forms.Button();
            this.btnDecrypt = new System.Windows.Forms.Button();
            this.gbxPlainText = new System.Windows.Forms.GroupBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.tbxInputMessage = new System.Windows.Forms.TextBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.tbxInputInterger = new System.Windows.Forms.TextBox();
            this.gbxCiphertextDisplayedasTextString = new System.Windows.Forms.GroupBox();
            this.tbxPlaintextasBytesArray = new System.Windows.Forms.TextBox();
            this.gbxCiphertextDisplayedasBytesArray = new System.Windows.Forms.GroupBox();
            this.tbxcipherTextasByteArray = new System.Windows.Forms.TextBox();
            this.gbxRecoveredPlainText = new System.Windows.Forms.GroupBox();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.tbxMessagePart = new System.Windows.Forms.TextBox();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.tbxIntegerPart = new System.Windows.Forms.TextBox();
            this.comboboxMode = new System.Windows.Forms.ComboBox();
            this.comboboxPadding = new System.Windows.Forms.ComboBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.groupBox1.SuspendLayout();
            this.gbxPlainText.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.gbxCiphertextDisplayedasTextString.SuspendLayout();
            this.gbxCiphertextDisplayedasBytesArray.SuspendLayout();
            this.gbxRecoveredPlainText.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.radiobtnRC2);
            this.groupBox1.Controls.Add(this.radiobtnRijndael);
            this.groupBox1.Controls.Add(this.radiobtnTrippleDES);
            this.groupBox1.Controls.Add(this.radiobtnDES);
            this.groupBox1.Location = new System.Drawing.Point(24, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(138, 163);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Symmetric Algorithms";
            // 
            // radiobtnRC2
            // 
            this.radiobtnRC2.AutoSize = true;
            this.radiobtnRC2.Location = new System.Drawing.Point(16, 138);
            this.radiobtnRC2.Name = "radiobtnRC2";
            this.radiobtnRC2.Size = new System.Drawing.Size(46, 17);
            this.radiobtnRC2.TabIndex = 8;
            this.radiobtnRC2.TabStop = true;
            this.radiobtnRC2.Text = "RC2";
            this.radiobtnRC2.UseVisualStyleBackColor = true;
            this.radiobtnRC2.CheckedChanged += new System.EventHandler(this.radiobtnSymmetricAlgorithm_CheckedChanged);
            // 
            // radiobtnRijndael
            // 
            this.radiobtnRijndael.AutoSize = true;
            this.radiobtnRijndael.Location = new System.Drawing.Point(16, 99);
            this.radiobtnRijndael.Name = "radiobtnRijndael";
            this.radiobtnRijndael.Size = new System.Drawing.Size(63, 17);
            this.radiobtnRijndael.TabIndex = 7;
            this.radiobtnRijndael.TabStop = true;
            this.radiobtnRijndael.Text = "Rijndael";
            this.radiobtnRijndael.UseVisualStyleBackColor = true;
            this.radiobtnRijndael.CheckedChanged += new System.EventHandler(this.radiobtnSymmetricAlgorithm_CheckedChanged);
            // 
            // radiobtnTrippleDES
            // 
            this.radiobtnTrippleDES.AutoSize = true;
            this.radiobtnTrippleDES.Location = new System.Drawing.Point(16, 60);
            this.radiobtnTrippleDES.Name = "radiobtnTrippleDES";
            this.radiobtnTrippleDES.Size = new System.Drawing.Size(82, 17);
            this.radiobtnTrippleDES.TabIndex = 6;
            this.radiobtnTrippleDES.TabStop = true;
            this.radiobtnTrippleDES.Text = "Tripple DES";
            this.radiobtnTrippleDES.UseVisualStyleBackColor = true;
            this.radiobtnTrippleDES.CheckedChanged += new System.EventHandler(this.radiobtnSymmetricAlgorithm_CheckedChanged);
            // 
            // radiobtnDES
            // 
            this.radiobtnDES.AutoSize = true;
            this.radiobtnDES.Location = new System.Drawing.Point(16, 21);
            this.radiobtnDES.Name = "radiobtnDES";
            this.radiobtnDES.Size = new System.Drawing.Size(47, 17);
            this.radiobtnDES.TabIndex = 5;
            this.radiobtnDES.TabStop = true;
            this.radiobtnDES.Text = "DES";
            this.radiobtnDES.UseVisualStyleBackColor = true;
            this.radiobtnDES.CheckedChanged += new System.EventHandler(this.radiobtnSymmetricAlgorithm_CheckedChanged);
            // 
            // btnNewRandomKey
            // 
            this.btnNewRandomKey.Location = new System.Drawing.Point(182, 12);
            this.btnNewRandomKey.Name = "btnNewRandomKey";
            this.btnNewRandomKey.Size = new System.Drawing.Size(134, 23);
            this.btnNewRandomKey.TabIndex = 1;
            this.btnNewRandomKey.Text = "New Random Key";
            this.btnNewRandomKey.UseVisualStyleBackColor = true;
            this.btnNewRandomKey.Click += new System.EventHandler(this.btnNewRandomKey_Click);
            // 
            // btnNewRandomInitVector
            // 
            this.btnNewRandomInitVector.Location = new System.Drawing.Point(182, 74);
            this.btnNewRandomInitVector.Name = "btnNewRandomInitVector";
            this.btnNewRandomInitVector.Size = new System.Drawing.Size(134, 23);
            this.btnNewRandomInitVector.TabIndex = 2;
            this.btnNewRandomInitVector.Text = "New Random Init Vector";
            this.btnNewRandomInitVector.UseVisualStyleBackColor = true;
            this.btnNewRandomInitVector.Click += new System.EventHandler(this.btnNewRandomInitVector_Click);
            // 
            // tbxRandomKey
            // 
            this.tbxRandomKey.Enabled = false;
            this.tbxRandomKey.Location = new System.Drawing.Point(353, 12);
            this.tbxRandomKey.Multiline = true;
            this.tbxRandomKey.Name = "tbxRandomKey";
            this.tbxRandomKey.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxRandomKey.Size = new System.Drawing.Size(491, 38);
            this.tbxRandomKey.TabIndex = 3;
            // 
            // tbxRandomInitVector
            // 
            this.tbxRandomInitVector.Enabled = false;
            this.tbxRandomInitVector.Location = new System.Drawing.Point(353, 76);
            this.tbxRandomInitVector.Multiline = true;
            this.tbxRandomInitVector.Name = "tbxRandomInitVector";
            this.tbxRandomInitVector.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxRandomInitVector.Size = new System.Drawing.Size(491, 38);
            this.tbxRandomInitVector.TabIndex = 4;
            // 
            // btnEncrypt
            // 
            this.btnEncrypt.Location = new System.Drawing.Point(24, 187);
            this.btnEncrypt.Name = "btnEncrypt";
            this.btnEncrypt.Size = new System.Drawing.Size(132, 23);
            this.btnEncrypt.TabIndex = 7;
            this.btnEncrypt.Text = "Encrypt";
            this.btnEncrypt.UseVisualStyleBackColor = true;
            this.btnEncrypt.Click += new System.EventHandler(this.btnEncrypt_Click);
            // 
            // btnDecrypt
            // 
            this.btnDecrypt.Location = new System.Drawing.Point(21, 275);
            this.btnDecrypt.Name = "btnDecrypt";
            this.btnDecrypt.Size = new System.Drawing.Size(132, 23);
            this.btnDecrypt.TabIndex = 8;
            this.btnDecrypt.Text = "Decrypt";
            this.btnDecrypt.UseVisualStyleBackColor = true;
            this.btnDecrypt.Click += new System.EventHandler(this.btnDecrypt_Click);
            // 
            // gbxPlainText
            // 
            this.gbxPlainText.Controls.Add(this.groupBox5);
            this.gbxPlainText.Controls.Add(this.groupBox4);
            this.gbxPlainText.Location = new System.Drawing.Point(182, 138);
            this.gbxPlainText.Name = "gbxPlainText";
            this.gbxPlainText.Size = new System.Drawing.Size(662, 72);
            this.gbxPlainText.TabIndex = 9;
            this.gbxPlainText.TabStop = false;
            this.gbxPlainText.Text = "Plaintext";
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.tbxInputMessage);
            this.groupBox5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox5.Location = new System.Drawing.Point(195, 16);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(464, 53);
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
            this.tbxInputMessage.Size = new System.Drawing.Size(458, 34);
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
            // gbxCiphertextDisplayedasTextString
            // 
            this.gbxCiphertextDisplayedasTextString.Controls.Add(this.tbxPlaintextasBytesArray);
            this.gbxCiphertextDisplayedasTextString.Location = new System.Drawing.Point(182, 226);
            this.gbxCiphertextDisplayedasTextString.Name = "gbxCiphertextDisplayedasTextString";
            this.gbxCiphertextDisplayedasTextString.Size = new System.Drawing.Size(662, 75);
            this.gbxCiphertextDisplayedasTextString.TabIndex = 10;
            this.gbxCiphertextDisplayedasTextString.TabStop = false;
            this.gbxCiphertextDisplayedasTextString.Text = "Plaintext Displayed as Bytes Array";
            // 
            // tbxPlaintextasBytesArray
            // 
            this.tbxPlaintextasBytesArray.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxPlaintextasBytesArray.Enabled = false;
            this.tbxPlaintextasBytesArray.Location = new System.Drawing.Point(3, 16);
            this.tbxPlaintextasBytesArray.Multiline = true;
            this.tbxPlaintextasBytesArray.Name = "tbxPlaintextasBytesArray";
            this.tbxPlaintextasBytesArray.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxPlaintextasBytesArray.Size = new System.Drawing.Size(656, 56);
            this.tbxPlaintextasBytesArray.TabIndex = 0;
            // 
            // gbxCiphertextDisplayedasBytesArray
            // 
            this.gbxCiphertextDisplayedasBytesArray.Controls.Add(this.tbxcipherTextasByteArray);
            this.gbxCiphertextDisplayedasBytesArray.Location = new System.Drawing.Point(182, 314);
            this.gbxCiphertextDisplayedasBytesArray.Name = "gbxCiphertextDisplayedasBytesArray";
            this.gbxCiphertextDisplayedasBytesArray.Size = new System.Drawing.Size(662, 75);
            this.gbxCiphertextDisplayedasBytesArray.TabIndex = 11;
            this.gbxCiphertextDisplayedasBytesArray.TabStop = false;
            this.gbxCiphertextDisplayedasBytesArray.Text = "Ciphertext Displayed as Bytes Array";
            // 
            // tbxcipherTextasByteArray
            // 
            this.tbxcipherTextasByteArray.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxcipherTextasByteArray.Enabled = false;
            this.tbxcipherTextasByteArray.Location = new System.Drawing.Point(3, 16);
            this.tbxcipherTextasByteArray.Multiline = true;
            this.tbxcipherTextasByteArray.Name = "tbxcipherTextasByteArray";
            this.tbxcipherTextasByteArray.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxcipherTextasByteArray.Size = new System.Drawing.Size(656, 56);
            this.tbxcipherTextasByteArray.TabIndex = 0;
            // 
            // gbxRecoveredPlainText
            // 
            this.gbxRecoveredPlainText.Controls.Add(this.groupBox7);
            this.gbxRecoveredPlainText.Controls.Add(this.groupBox6);
            this.gbxRecoveredPlainText.Location = new System.Drawing.Point(182, 402);
            this.gbxRecoveredPlainText.Name = "gbxRecoveredPlainText";
            this.gbxRecoveredPlainText.Size = new System.Drawing.Size(662, 75);
            this.gbxRecoveredPlainText.TabIndex = 12;
            this.gbxRecoveredPlainText.TabStop = false;
            this.gbxRecoveredPlainText.Text = "Recovered Plaintext";
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.tbxMessagePart);
            this.groupBox7.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox7.Location = new System.Drawing.Point(195, 16);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(464, 56);
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
            this.tbxMessagePart.Size = new System.Drawing.Size(458, 37);
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
            this.tbxIntegerPart.Size = new System.Drawing.Size(186, 37);
            this.tbxIntegerPart.TabIndex = 0;
            // 
            // comboboxMode
            // 
            this.comboboxMode.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboboxMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboboxMode.FormattingEnabled = true;
            this.comboboxMode.Location = new System.Drawing.Point(3, 16);
            this.comboboxMode.Name = "comboboxMode";
            this.comboboxMode.Size = new System.Drawing.Size(126, 21);
            this.comboboxMode.TabIndex = 13;
            this.comboboxMode.SelectedIndexChanged += new System.EventHandler(this.comboboxMode_SelectedIndexChanged);
            // 
            // comboboxPadding
            // 
            this.comboboxPadding.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboboxPadding.FormattingEnabled = true;
            this.comboboxPadding.Location = new System.Drawing.Point(3, 16);
            this.comboboxPadding.Name = "comboboxPadding";
            this.comboboxPadding.Size = new System.Drawing.Size(126, 21);
            this.comboboxPadding.TabIndex = 14;
            this.comboboxPadding.SelectedIndexChanged += new System.EventHandler(this.comboboxPadding_SelectedIndexChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.comboboxMode);
            this.groupBox2.Location = new System.Drawing.Point(24, 345);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(132, 41);
            this.groupBox2.TabIndex = 15;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Mode";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.comboboxPadding);
            this.groupBox3.Location = new System.Drawing.Point(24, 434);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(132, 41);
            this.groupBox3.TabIndex = 3;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Padding";
            // 
            // SymmetricAlgorithmForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(873, 550);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.gbxRecoveredPlainText);
            this.Controls.Add(this.gbxCiphertextDisplayedasBytesArray);
            this.Controls.Add(this.gbxCiphertextDisplayedasTextString);
            this.Controls.Add(this.gbxPlainText);
            this.Controls.Add(this.btnDecrypt);
            this.Controls.Add(this.btnEncrypt);
            this.Controls.Add(this.tbxRandomInitVector);
            this.Controls.Add(this.tbxRandomKey);
            this.Controls.Add(this.btnNewRandomInitVector);
            this.Controls.Add(this.btnNewRandomKey);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "SymmetricAlgorithmForm";
            this.Text = "Symmetric Algorithms";
            this.Load += new System.EventHandler(this.SymmetricAlgorithmForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.gbxPlainText.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.gbxCiphertextDisplayedasTextString.ResumeLayout(false);
            this.gbxCiphertextDisplayedasTextString.PerformLayout();
            this.gbxCiphertextDisplayedasBytesArray.ResumeLayout(false);
            this.gbxCiphertextDisplayedasBytesArray.PerformLayout();
            this.gbxRecoveredPlainText.ResumeLayout(false);
            this.groupBox7.ResumeLayout(false);
            this.groupBox7.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnNewRandomKey;
        private System.Windows.Forms.Button btnNewRandomInitVector;
        private System.Windows.Forms.TextBox tbxRandomKey;
        private System.Windows.Forms.TextBox tbxRandomInitVector;
        private System.Windows.Forms.RadioButton radiobtnRC2;
        private System.Windows.Forms.RadioButton radiobtnRijndael;
        private System.Windows.Forms.RadioButton radiobtnTrippleDES;
        private System.Windows.Forms.RadioButton radiobtnDES;
        private System.Windows.Forms.Button btnEncrypt;
        private System.Windows.Forms.Button btnDecrypt;
        private System.Windows.Forms.GroupBox gbxPlainText;
        private System.Windows.Forms.GroupBox gbxCiphertextDisplayedasTextString;
        private System.Windows.Forms.GroupBox gbxCiphertextDisplayedasBytesArray;
        private System.Windows.Forms.GroupBox gbxRecoveredPlainText;
        private System.Windows.Forms.TextBox tbxPlaintextasBytesArray;
        private System.Windows.Forms.TextBox tbxcipherTextasByteArray;
        private System.Windows.Forms.ComboBox comboboxMode;
        private System.Windows.Forms.ComboBox comboboxPadding;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.TextBox tbxInputMessage;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.TextBox tbxInputInterger;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.TextBox tbxMessagePart;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.TextBox tbxIntegerPart;
    }
}