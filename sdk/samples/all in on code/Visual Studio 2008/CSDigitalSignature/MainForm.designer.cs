namespace CSDigitalSignature
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
            this.components = new System.ComponentModel.Container();
            this.btnSign = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.tbxSHA256HashBytes = new System.Windows.Forms.TextBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.tbxDigitalSignature = new System.Windows.Forms.TextBox();
            this.btnVerify = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.tbxModulus = new System.Windows.Forms.TextBox();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.Exponenttbx = new System.Windows.Forms.TextBox();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.gbxPlainText = new System.Windows.Forms.GroupBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.tbxInputMessage = new System.Windows.Forms.TextBox();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.tbxInputInterger = new System.Windows.Forms.TextBox();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.gbxPlainText.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnSign
            // 
            this.btnSign.Location = new System.Drawing.Point(12, 12);
            this.btnSign.Name = "btnSign";
            this.btnSign.Size = new System.Drawing.Size(90, 32);
            this.btnSign.TabIndex = 0;
            this.btnSign.Text = "Sign";
            this.btnSign.UseVisualStyleBackColor = true;
            this.btnSign.Click += new System.EventHandler(this.btnSign_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.tbxSHA256HashBytes);
            this.groupBox2.Location = new System.Drawing.Point(125, 84);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(599, 53);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "SHA256 Hash Bytes of Original Message(message digest)";
            // 
            // tbxSHA256HashBytes
            // 
            this.tbxSHA256HashBytes.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxSHA256HashBytes.Enabled = false;
            this.tbxSHA256HashBytes.Location = new System.Drawing.Point(3, 16);
            this.tbxSHA256HashBytes.Multiline = true;
            this.tbxSHA256HashBytes.Name = "tbxSHA256HashBytes";
            this.tbxSHA256HashBytes.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxSHA256HashBytes.Size = new System.Drawing.Size(593, 34);
            this.tbxSHA256HashBytes.TabIndex = 0;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.tbxDigitalSignature);
            this.groupBox3.Location = new System.Drawing.Point(125, 142);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(599, 53);
            this.groupBox3.TabIndex = 3;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Digital Signature(Encrypted Message Digest)";
            // 
            // tbxDigitalSignature
            // 
            this.tbxDigitalSignature.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxDigitalSignature.Enabled = false;
            this.tbxDigitalSignature.Location = new System.Drawing.Point(3, 16);
            this.tbxDigitalSignature.Multiline = true;
            this.tbxDigitalSignature.Name = "tbxDigitalSignature";
            this.tbxDigitalSignature.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbxDigitalSignature.Size = new System.Drawing.Size(593, 34);
            this.tbxDigitalSignature.TabIndex = 0;
            // 
            // btnVerify
            // 
            this.btnVerify.Location = new System.Drawing.Point(12, 68);
            this.btnVerify.Name = "btnVerify";
            this.btnVerify.Size = new System.Drawing.Size(90, 32);
            this.btnVerify.TabIndex = 4;
            this.btnVerify.Text = "Verify";
            this.btnVerify.UseVisualStyleBackColor = true;
            this.btnVerify.Click += new System.EventHandler(this.btnVerify_Click);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.tableLayoutPanel1);
            this.groupBox4.Location = new System.Drawing.Point(12, 201);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(712, 319);
            this.groupBox4.TabIndex = 5;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "RSA Parameters";
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 1;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Controls.Add(this.groupBox5, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.groupBox6, 0, 1);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(3, 16);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(706, 300);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.tbxModulus);
            this.groupBox5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox5.Location = new System.Drawing.Point(3, 3);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(700, 144);
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
            this.tbxModulus.Size = new System.Drawing.Size(694, 125);
            this.tbxModulus.TabIndex = 0;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.Exponenttbx);
            this.groupBox6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox6.Location = new System.Drawing.Point(3, 153);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(700, 144);
            this.groupBox6.TabIndex = 1;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Exponent";
            // 
            // Exponenttbx
            // 
            this.Exponenttbx.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Exponenttbx.Enabled = false;
            this.Exponenttbx.Location = new System.Drawing.Point(3, 16);
            this.Exponenttbx.Multiline = true;
            this.Exponenttbx.Name = "Exponenttbx";
            this.Exponenttbx.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.Exponenttbx.Size = new System.Drawing.Size(694, 125);
            this.Exponenttbx.TabIndex = 0;
            // 
            // gbxPlainText
            // 
            this.gbxPlainText.Controls.Add(this.groupBox1);
            this.gbxPlainText.Controls.Add(this.groupBox7);
            this.gbxPlainText.Location = new System.Drawing.Point(125, 6);
            this.gbxPlainText.Name = "gbxPlainText";
            this.gbxPlainText.Size = new System.Drawing.Size(632, 72);
            this.gbxPlainText.TabIndex = 11;
            this.gbxPlainText.TabStop = false;
            this.gbxPlainText.Text = "Original data for sign";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.tbxInputMessage);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox1.Location = new System.Drawing.Point(195, 16);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(434, 53);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Input message";
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
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.tbxInputInterger);
            this.groupBox7.Dock = System.Windows.Forms.DockStyle.Left;
            this.groupBox7.Location = new System.Drawing.Point(3, 16);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(192, 53);
            this.groupBox7.TabIndex = 0;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Input interger";
            // 
            // tbxInputInterger
            // 
            this.tbxInputInterger.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbxInputInterger.Location = new System.Drawing.Point(3, 16);
            this.tbxInputInterger.Multiline = true;
            this.tbxInputInterger.Name = "tbxInputInterger";
            this.tbxInputInterger.Size = new System.Drawing.Size(186, 34);
            this.tbxInputInterger.TabIndex = 0;
            // 
            // formDigitalSignature
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(740, 548);
            this.Controls.Add(this.gbxPlainText);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.btnVerify);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.btnSign);
            this.Name = "formDigitalSignature";
            this.Text = "DigitalSignatureForm";
            this.Load += new System.EventHandler(this.DigitalSignatureForm_Load);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.gbxPlainText.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox7.ResumeLayout(false);
            this.groupBox7.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnSign;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button btnVerify;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.TextBox tbxSHA256HashBytes;
        private System.Windows.Forms.TextBox tbxDigitalSignature;
        private System.Windows.Forms.TextBox tbxModulus;
        private System.Windows.Forms.TextBox Exponenttbx;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.GroupBox gbxPlainText;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox tbxInputMessage;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.TextBox tbxInputInterger;

    }
}