namespace CSImageFullScreenSlideShow
{
    partial class Settings
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
            this.lbTimerInternal = new System.Windows.Forms.Label();
            this.dtpInternal = new System.Windows.Forms.NumericUpDown();
            this.lbMilliseconds = new System.Windows.Forms.Label();
            this.btnConfirm = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.dtpInternal)).BeginInit();
            this.SuspendLayout();
            // 
            // lbTimerInternal
            // 
            this.lbTimerInternal.AutoSize = true;
            this.lbTimerInternal.Location = new System.Drawing.Point(29, 22);
            this.lbTimerInternal.Name = "lbTimerInternal";
            this.lbTimerInternal.Size = new System.Drawing.Size(77, 13);
            this.lbTimerInternal.TabIndex = 0;
            this.lbTimerInternal.Text = "Timer Internal：";
            // 
            // dtpInternal
            // 
            this.dtpInternal.Location = new System.Drawing.Point(112, 20);
            this.dtpInternal.Maximum = new decimal(new int[] {
            10000000,
            0,
            0,
            0});
            this.dtpInternal.Name = "dtpInternal";
            this.dtpInternal.Size = new System.Drawing.Size(120, 20);
            this.dtpInternal.TabIndex = 1;
            // 
            // lbMilliseconds
            // 
            this.lbMilliseconds.AutoSize = true;
            this.lbMilliseconds.Location = new System.Drawing.Point(248, 22);
            this.lbMilliseconds.Name = "lbMilliseconds";
            this.lbMilliseconds.Size = new System.Drawing.Size(64, 13);
            this.lbMilliseconds.TabIndex = 2;
            this.lbMilliseconds.Text = "Milliseconds";
            // 
            // btnConfirm
            // 
            this.btnConfirm.Location = new System.Drawing.Point(75, 54);
            this.btnConfirm.Name = "btnConfirm";
            this.btnConfirm.Size = new System.Drawing.Size(75, 23);
            this.btnConfirm.TabIndex = 3;
            this.btnConfirm.Text = "Confirm";
            this.btnConfirm.UseVisualStyleBackColor = true;
            this.btnConfirm.Click += new System.EventHandler(this.btnConfirm_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Location = new System.Drawing.Point(177, 54);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 4;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // Settings
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(343, 89);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnConfirm);
            this.Controls.Add(this.lbMilliseconds);
            this.Controls.Add(this.dtpInternal);
            this.Controls.Add(this.lbTimerInternal);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "Settings";
            this.Text = "Settings";
            ((System.ComponentModel.ISupportInitialize)(this.dtpInternal)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lbTimerInternal;
        private System.Windows.Forms.NumericUpDown dtpInternal;
        private System.Windows.Forms.Label lbMilliseconds;
        private System.Windows.Forms.Button btnConfirm;
        private System.Windows.Forms.Button btnCancel;
    }
}