namespace CSWin7TaskbarAppID
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
            this.openSubFormButton = new System.Windows.Forms.Button();
            this.resetSubFormAppIDButton = new System.Windows.Forms.Button();
            this.setSubFormAppIDButton = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // openSubFormButton
            // 
            this.openSubFormButton.Location = new System.Drawing.Point(69, 33);
            this.openSubFormButton.Name = "openSubFormButton";
            this.openSubFormButton.Size = new System.Drawing.Size(98, 23);
            this.openSubFormButton.TabIndex = 0;
            this.openSubFormButton.Text = "Open Sub Form";
            this.openSubFormButton.UseVisualStyleBackColor = true;
            this.openSubFormButton.Click += new System.EventHandler(this.openSubFormButton_Click);
            // 
            // resetSubFormAppIDButton
            // 
            this.resetSubFormAppIDButton.Enabled = false;
            this.resetSubFormAppIDButton.Location = new System.Drawing.Point(109, 31);
            this.resetSubFormAppIDButton.Name = "resetSubFormAppIDButton";
            this.resetSubFormAppIDButton.Size = new System.Drawing.Size(63, 23);
            this.resetSubFormAppIDButton.TabIndex = 1;
            this.resetSubFormAppIDButton.Text = "Reset";
            this.resetSubFormAppIDButton.UseVisualStyleBackColor = true;
            this.resetSubFormAppIDButton.Click += new System.EventHandler(this.resetSubFormAppIDButton_Click);
            // 
            // setSubFormAppIDButton
            // 
            this.setSubFormAppIDButton.Enabled = false;
            this.setSubFormAppIDButton.Location = new System.Drawing.Point(24, 31);
            this.setSubFormAppIDButton.Name = "setSubFormAppIDButton";
            this.setSubFormAppIDButton.Size = new System.Drawing.Size(63, 23);
            this.setSubFormAppIDButton.TabIndex = 3;
            this.setSubFormAppIDButton.Text = "Set";
            this.setSubFormAppIDButton.UseVisualStyleBackColor = true;
            this.setSubFormAppIDButton.Click += new System.EventHandler(this.setSubFormAppIDButton_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.setSubFormAppIDButton);
            this.groupBox1.Controls.Add(this.resetSubFormAppIDButton);
            this.groupBox1.Location = new System.Drawing.Point(229, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(200, 73);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Set/Reset SubForm AppID:";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(477, 105);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.openSubFormButton);
            this.Name = "MainForm";
            this.Text = "MainForm";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button openSubFormButton;
        private System.Windows.Forms.Button resetSubFormAppIDButton;
        private System.Windows.Forms.Button setSubFormAppIDButton;
        private System.Windows.Forms.GroupBox groupBox1;
    }
}

