namespace CSWinFormGroupRadioButtons
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
            this.grp1 = new System.Windows.Forms.GroupBox();
            this.rad2 = new System.Windows.Forms.RadioButton();
            this.rad1 = new System.Windows.Forms.RadioButton();
            this.grp2 = new System.Windows.Forms.GroupBox();
            this.rad4 = new System.Windows.Forms.RadioButton();
            this.rad3 = new System.Windows.Forms.RadioButton();
            this.lb = new System.Windows.Forms.Label();
            this.grp1.SuspendLayout();
            this.grp2.SuspendLayout();
            this.SuspendLayout();
            // 
            // grp1
            // 
            this.grp1.Controls.Add(this.rad2);
            this.grp1.Controls.Add(this.rad1);
            this.grp1.Location = new System.Drawing.Point(13, 13);
            this.grp1.Name = "grp1";
            this.grp1.Size = new System.Drawing.Size(267, 100);
            this.grp1.TabIndex = 0;
            this.grp1.TabStop = false;
            this.grp1.Text = "grp1";
            // 
            // rad2
            // 
            this.rad2.AutoSize = true;
            this.rad2.Location = new System.Drawing.Point(7, 77);
            this.rad2.Name = "rad2";
            this.rad2.Size = new System.Drawing.Size(46, 17);
            this.rad2.TabIndex = 1;
            this.rad2.TabStop = true;
            this.rad2.Text = "rad2";
            this.rad2.UseVisualStyleBackColor = true;
            this.rad2.CheckedChanged += new System.EventHandler(this.radioButton_CheckedChanged);
            // 
            // rad1
            // 
            this.rad1.AutoSize = true;
            this.rad1.Checked = true;
            this.rad1.Location = new System.Drawing.Point(7, 20);
            this.rad1.Name = "rad1";
            this.rad1.Size = new System.Drawing.Size(46, 17);
            this.rad1.TabIndex = 0;
            this.rad1.TabStop = true;
            this.rad1.Text = "rad1";
            this.rad1.UseVisualStyleBackColor = true;
            this.rad1.CheckedChanged += new System.EventHandler(this.radioButton_CheckedChanged);
            // 
            // grp2
            // 
            this.grp2.Controls.Add(this.rad4);
            this.grp2.Controls.Add(this.rad3);
            this.grp2.Location = new System.Drawing.Point(13, 161);
            this.grp2.Name = "grp2";
            this.grp2.Size = new System.Drawing.Size(267, 100);
            this.grp2.TabIndex = 1;
            this.grp2.TabStop = false;
            this.grp2.Text = "grp2";
            // 
            // rad4
            // 
            this.rad4.AutoSize = true;
            this.rad4.Location = new System.Drawing.Point(7, 77);
            this.rad4.Name = "rad4";
            this.rad4.Size = new System.Drawing.Size(46, 17);
            this.rad4.TabIndex = 1;
            this.rad4.TabStop = true;
            this.rad4.Text = "rad4";
            this.rad4.UseVisualStyleBackColor = true;
            this.rad4.CheckedChanged += new System.EventHandler(this.radioButton_CheckedChanged);
            // 
            // rad3
            // 
            this.rad3.AutoSize = true;
            this.rad3.Location = new System.Drawing.Point(7, 20);
            this.rad3.Name = "rad3";
            this.rad3.Size = new System.Drawing.Size(46, 17);
            this.rad3.TabIndex = 0;
            this.rad3.TabStop = true;
            this.rad3.Text = "rad3";
            this.rad3.UseVisualStyleBackColor = true;
            this.rad3.CheckedChanged += new System.EventHandler(this.radioButton_CheckedChanged);
            // 
            // lb
            // 
            this.lb.AutoSize = true;
            this.lb.Location = new System.Drawing.Point(13, 130);
            this.lb.Name = "lb";
            this.lb.Size = new System.Drawing.Size(118, 13);
            this.lb.TabIndex = 2;
            this.lb.Text = "rad1 has been selected";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 273);
            this.Controls.Add(this.lb);
            this.Controls.Add(this.grp2);
            this.Controls.Add(this.grp1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.Text = "CSWinFormGroupRadioButtons";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.grp1.ResumeLayout(false);
            this.grp1.PerformLayout();
            this.grp2.ResumeLayout(false);
            this.grp2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox grp1;
        private System.Windows.Forms.GroupBox grp2;
        private System.Windows.Forms.RadioButton rad2;
        private System.Windows.Forms.RadioButton rad1;
        private System.Windows.Forms.RadioButton rad4;
        private System.Windows.Forms.RadioButton rad3;
        private System.Windows.Forms.Label lb;
    }
}

