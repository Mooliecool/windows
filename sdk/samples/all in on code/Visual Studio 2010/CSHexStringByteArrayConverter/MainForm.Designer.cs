namespace CSHexStringByteArrayConverter
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
            this.group1 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.lbHexTip = new System.Windows.Forms.Label();
            this.btnConvertHexStringToBytes = new System.Windows.Forms.Button();
            this.cmbByteArrayResult = new System.Windows.Forms.ComboBox();
            this.btnReverseCopytoClipboard = new System.Windows.Forms.Button();
            this.tbHexStringInput = new System.Windows.Forms.TextBox();
            this.lbForwardBypteArray = new System.Windows.Forms.Label();
            this.lbForwardHexString = new System.Windows.Forms.Label();
            this.btnPasteFromClipboard = new System.Windows.Forms.Button();
            this.group2 = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.lbAnotherHexTip = new System.Windows.Forms.Label();
            this.btnClearBytes = new System.Windows.Forms.Button();
            this.tbByteToAdd = new System.Windows.Forms.TextBox();
            this.btnAddByte = new System.Windows.Forms.Button();
            this.btnConvertBytesToHexString = new System.Windows.Forms.Button();
            this.cmbByteArrayInput = new System.Windows.Forms.ComboBox();
            this.tbHexStringResult = new System.Windows.Forms.TextBox();
            this.lbByteArrayInput = new System.Windows.Forms.Label();
            this.lbReverseHexString = new System.Windows.Forms.Label();
            this.group1.SuspendLayout();
            this.group2.SuspendLayout();
            this.SuspendLayout();
            // 
            // group1
            // 
            this.group1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.group1.Controls.Add(this.label1);
            this.group1.Controls.Add(this.lbHexTip);
            this.group1.Controls.Add(this.btnConvertHexStringToBytes);
            this.group1.Controls.Add(this.cmbByteArrayResult);
            this.group1.Controls.Add(this.btnReverseCopytoClipboard);
            this.group1.Controls.Add(this.tbHexStringInput);
            this.group1.Controls.Add(this.lbForwardBypteArray);
            this.group1.Controls.Add(this.lbForwardHexString);
            this.group1.Location = new System.Drawing.Point(12, 12);
            this.group1.Name = "group1";
            this.group1.Size = new System.Drawing.Size(424, 104);
            this.group1.TabIndex = 0;
            this.group1.TabStop = false;
            this.group1.Text = "Hex String -> Byte Array";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(100, 65);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(18, 13);
            this.label1.TabIndex = 10;
            this.label1.Text = "0x";
            // 
            // lbHexTip
            // 
            this.lbHexTip.AutoSize = true;
            this.lbHexTip.Location = new System.Drawing.Point(100, 33);
            this.lbHexTip.Name = "lbHexTip";
            this.lbHexTip.Size = new System.Drawing.Size(18, 13);
            this.lbHexTip.TabIndex = 6;
            this.lbHexTip.Text = "0x";
            // 
            // btnConvertHexStringToBytes
            // 
            this.btnConvertHexStringToBytes.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnConvertHexStringToBytes.Location = new System.Drawing.Point(271, 28);
            this.btnConvertHexStringToBytes.Name = "btnConvertHexStringToBytes";
            this.btnConvertHexStringToBytes.Size = new System.Drawing.Size(136, 23);
            this.btnConvertHexStringToBytes.TabIndex = 4;
            this.btnConvertHexStringToBytes.Text = "Hex String -> Byte Array";
            this.btnConvertHexStringToBytes.UseVisualStyleBackColor = true;
            this.btnConvertHexStringToBytes.Click += new System.EventHandler(this.btnConvertHexStringToByteArray_Click);
            // 
            // cmbByteArrayResult
            // 
            this.cmbByteArrayResult.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbByteArrayResult.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbByteArrayResult.FormatString = "X2";
            this.cmbByteArrayResult.FormattingEnabled = true;
            this.cmbByteArrayResult.Location = new System.Drawing.Point(118, 61);
            this.cmbByteArrayResult.Name = "cmbByteArrayResult";
            this.cmbByteArrayResult.Size = new System.Drawing.Size(147, 21);
            this.cmbByteArrayResult.TabIndex = 3;
            // 
            // btnReverseCopytoClipboard
            // 
            this.btnReverseCopytoClipboard.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnReverseCopytoClipboard.Location = new System.Drawing.Point(271, 60);
            this.btnReverseCopytoClipboard.Name = "btnReverseCopytoClipboard";
            this.btnReverseCopytoClipboard.Size = new System.Drawing.Size(136, 23);
            this.btnReverseCopytoClipboard.TabIndex = 9;
            this.btnReverseCopytoClipboard.Text = "Copy to Clipboard";
            this.btnReverseCopytoClipboard.UseVisualStyleBackColor = true;
            this.btnReverseCopytoClipboard.Click += new System.EventHandler(this.btnCopytoClipboard_Click);
            // 
            // tbHexStringInput
            // 
            this.tbHexStringInput.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbHexStringInput.Location = new System.Drawing.Point(118, 30);
            this.tbHexStringInput.Name = "tbHexStringInput";
            this.tbHexStringInput.Size = new System.Drawing.Size(147, 20);
            this.tbHexStringInput.TabIndex = 2;
            this.tbHexStringInput.Text = "FF00EE11";
            // 
            // lbForwardBypteArray
            // 
            this.lbForwardBypteArray.AutoSize = true;
            this.lbForwardBypteArray.Location = new System.Drawing.Point(29, 64);
            this.lbForwardBypteArray.Name = "lbForwardBypteArray";
            this.lbForwardBypteArray.Size = new System.Drawing.Size(58, 13);
            this.lbForwardBypteArray.TabIndex = 1;
            this.lbForwardBypteArray.Text = "Byte Array:";
            // 
            // lbForwardHexString
            // 
            this.lbForwardHexString.AutoSize = true;
            this.lbForwardHexString.Location = new System.Drawing.Point(29, 33);
            this.lbForwardHexString.Name = "lbForwardHexString";
            this.lbForwardHexString.Size = new System.Drawing.Size(59, 13);
            this.lbForwardHexString.TabIndex = 0;
            this.lbForwardHexString.Text = "Hex String:";
            // 
            // btnPasteFromClipboard
            // 
            this.btnPasteFromClipboard.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnPasteFromClipboard.Location = new System.Drawing.Point(271, 56);
            this.btnPasteFromClipboard.Name = "btnPasteFromClipboard";
            this.btnPasteFromClipboard.Size = new System.Drawing.Size(136, 23);
            this.btnPasteFromClipboard.TabIndex = 5;
            this.btnPasteFromClipboard.Text = "Paste From Clipboard";
            this.btnPasteFromClipboard.UseVisualStyleBackColor = true;
            this.btnPasteFromClipboard.Click += new System.EventHandler(this.btnPasteFromClipboard_Click);
            // 
            // group2
            // 
            this.group2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.group2.Controls.Add(this.label3);
            this.group2.Controls.Add(this.label2);
            this.group2.Controls.Add(this.lbAnotherHexTip);
            this.group2.Controls.Add(this.btnPasteFromClipboard);
            this.group2.Controls.Add(this.btnClearBytes);
            this.group2.Controls.Add(this.tbByteToAdd);
            this.group2.Controls.Add(this.btnAddByte);
            this.group2.Controls.Add(this.btnConvertBytesToHexString);
            this.group2.Controls.Add(this.cmbByteArrayInput);
            this.group2.Controls.Add(this.tbHexStringResult);
            this.group2.Controls.Add(this.lbByteArrayInput);
            this.group2.Controls.Add(this.lbReverseHexString);
            this.group2.Location = new System.Drawing.Point(12, 122);
            this.group2.Name = "group2";
            this.group2.Size = new System.Drawing.Size(424, 166);
            this.group2.TabIndex = 1;
            this.group2.TabStop = false;
            this.group2.Text = "Byte Array -> Hex String";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(100, 126);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(18, 13);
            this.label3.TabIndex = 13;
            this.label3.Text = "0x";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(100, 62);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(18, 13);
            this.label2.TabIndex = 11;
            this.label2.Text = "0x";
            // 
            // lbAnotherHexTip
            // 
            this.lbAnotherHexTip.AutoSize = true;
            this.lbAnotherHexTip.Location = new System.Drawing.Point(100, 30);
            this.lbAnotherHexTip.Name = "lbAnotherHexTip";
            this.lbAnotherHexTip.Size = new System.Drawing.Size(18, 13);
            this.lbAnotherHexTip.TabIndex = 12;
            this.lbAnotherHexTip.Text = "0x";
            // 
            // btnClearBytes
            // 
            this.btnClearBytes.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnClearBytes.Location = new System.Drawing.Point(209, 25);
            this.btnClearBytes.Name = "btnClearBytes";
            this.btnClearBytes.Size = new System.Drawing.Size(57, 23);
            this.btnClearBytes.TabIndex = 6;
            this.btnClearBytes.Text = "Clear";
            this.btnClearBytes.UseVisualStyleBackColor = true;
            this.btnClearBytes.Click += new System.EventHandler(this.btnClearBytes_Click);
            // 
            // tbByteToAdd
            // 
            this.tbByteToAdd.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbByteToAdd.Location = new System.Drawing.Point(118, 27);
            this.tbByteToAdd.Name = "tbByteToAdd";
            this.tbByteToAdd.Size = new System.Drawing.Size(26, 20);
            this.tbByteToAdd.TabIndex = 11;
            // 
            // btnAddByte
            // 
            this.btnAddByte.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnAddByte.Location = new System.Drawing.Point(149, 25);
            this.btnAddByte.Name = "btnAddByte";
            this.btnAddByte.Size = new System.Drawing.Size(58, 23);
            this.btnAddByte.TabIndex = 10;
            this.btnAddByte.Text = "Add";
            this.btnAddByte.UseVisualStyleBackColor = true;
            this.btnAddByte.Click += new System.EventHandler(this.btnAddByte_Click);
            // 
            // btnConvertBytesToHexString
            // 
            this.btnConvertBytesToHexString.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnConvertBytesToHexString.Location = new System.Drawing.Point(271, 87);
            this.btnConvertBytesToHexString.Name = "btnConvertBytesToHexString";
            this.btnConvertBytesToHexString.Size = new System.Drawing.Size(136, 23);
            this.btnConvertBytesToHexString.TabIndex = 8;
            this.btnConvertBytesToHexString.Text = "Byte Array -> Hex String";
            this.btnConvertBytesToHexString.UseVisualStyleBackColor = true;
            this.btnConvertBytesToHexString.Click += new System.EventHandler(this.btnConvertByteArrayToHexString_Click);
            // 
            // cmbByteArrayInput
            // 
            this.cmbByteArrayInput.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbByteArrayInput.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbByteArrayInput.FormatString = "X2";
            this.cmbByteArrayInput.FormattingEnabled = true;
            this.cmbByteArrayInput.Location = new System.Drawing.Point(118, 58);
            this.cmbByteArrayInput.Name = "cmbByteArrayInput";
            this.cmbByteArrayInput.Size = new System.Drawing.Size(147, 21);
            this.cmbByteArrayInput.TabIndex = 7;
            // 
            // tbHexStringResult
            // 
            this.tbHexStringResult.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbHexStringResult.Location = new System.Drawing.Point(118, 123);
            this.tbHexStringResult.Name = "tbHexStringResult";
            this.tbHexStringResult.ReadOnly = true;
            this.tbHexStringResult.Size = new System.Drawing.Size(147, 20);
            this.tbHexStringResult.TabIndex = 6;
            // 
            // lbByteArrayInput
            // 
            this.lbByteArrayInput.AutoSize = true;
            this.lbByteArrayInput.Location = new System.Drawing.Point(29, 31);
            this.lbByteArrayInput.Name = "lbByteArrayInput";
            this.lbByteArrayInput.Size = new System.Drawing.Size(58, 13);
            this.lbByteArrayInput.TabIndex = 5;
            this.lbByteArrayInput.Text = "Byte Array:";
            // 
            // lbReverseHexString
            // 
            this.lbReverseHexString.AutoSize = true;
            this.lbReverseHexString.Location = new System.Drawing.Point(29, 126);
            this.lbReverseHexString.Name = "lbReverseHexString";
            this.lbReverseHexString.Size = new System.Drawing.Size(59, 13);
            this.lbReverseHexString.TabIndex = 4;
            this.lbReverseHexString.Text = "Hex String:";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(448, 300);
            this.Controls.Add(this.group2);
            this.Controls.Add(this.group1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "MainForm";
            this.Text = "CSHexStringByteArrayConverter";
            this.group1.ResumeLayout(false);
            this.group1.PerformLayout();
            this.group2.ResumeLayout(false);
            this.group2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox group1;
        private System.Windows.Forms.Button btnConvertHexStringToBytes;
        private System.Windows.Forms.ComboBox cmbByteArrayResult;
        private System.Windows.Forms.TextBox tbHexStringInput;
        private System.Windows.Forms.Label lbForwardBypteArray;
        private System.Windows.Forms.Label lbForwardHexString;
        private System.Windows.Forms.GroupBox group2;
        private System.Windows.Forms.Button btnReverseCopytoClipboard;
        private System.Windows.Forms.Button btnConvertBytesToHexString;
        private System.Windows.Forms.ComboBox cmbByteArrayInput;
        private System.Windows.Forms.TextBox tbHexStringResult;
        private System.Windows.Forms.Label lbByteArrayInput;
        private System.Windows.Forms.Label lbReverseHexString;
        private System.Windows.Forms.Button btnAddByte;
        private System.Windows.Forms.TextBox tbByteToAdd;
        private System.Windows.Forms.Button btnClearBytes;
        private System.Windows.Forms.Button btnPasteFromClipboard;
        private System.Windows.Forms.Label lbHexTip;
        private System.Windows.Forms.Label lbAnotherHexTip;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
    }
}

