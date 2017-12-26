namespace CSWin7TaskbarOverlayIcons
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
            this.statusComboBox = new System.Windows.Forms.ComboBox();
            this.showIconCheckBox = new System.Windows.Forms.CheckBox();
            this.statusLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // statusComboBox
            // 
            this.statusComboBox.FormattingEnabled = true;
            this.statusComboBox.Items.AddRange(new object[] {
            "Available",
            "Away",
            "Appear Offline"});
            this.statusComboBox.Location = new System.Drawing.Point(113, 63);
            this.statusComboBox.Name = "statusComboBox";
            this.statusComboBox.Size = new System.Drawing.Size(121, 21);
            this.statusComboBox.TabIndex = 0;
            this.statusComboBox.SelectedIndexChanged += new System.EventHandler(this.statusComboBox_SelectedIndexChanged);
            // 
            // showIconCheckBox
            // 
            this.showIconCheckBox.AutoSize = true;
            this.showIconCheckBox.Location = new System.Drawing.Point(78, 23);
            this.showIconCheckBox.Name = "showIconCheckBox";
            this.showIconCheckBox.Size = new System.Drawing.Size(121, 17);
            this.showIconCheckBox.TabIndex = 1;
            this.showIconCheckBox.Text = "Show Overlay Icons";
            this.showIconCheckBox.UseVisualStyleBackColor = true;
            this.showIconCheckBox.CheckedChanged += new System.EventHandler(this.showIconCheckBox_CheckedChanged);
            // 
            // statusLabel
            // 
            this.statusLabel.AutoSize = true;
            this.statusLabel.Location = new System.Drawing.Point(39, 66);
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(59, 13);
            this.statusLabel.TabIndex = 2;
            this.statusLabel.Text = "Set Status:";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(285, 107);
            this.Controls.Add(this.statusLabel);
            this.Controls.Add(this.showIconCheckBox);
            this.Controls.Add(this.statusComboBox);
            this.Name = "MainForm";
            this.Text = "Win7 Taskbar OverlayIcons";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox statusComboBox;
        private System.Windows.Forms.CheckBox showIconCheckBox;
        private System.Windows.Forms.Label statusLabel;
    }
}

