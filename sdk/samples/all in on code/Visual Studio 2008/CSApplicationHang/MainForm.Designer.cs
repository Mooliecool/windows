namespace CSApplicationHang
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
            this.btnSpinning = new System.Windows.Forms.Button();
            this.btnDeadlock = new System.Windows.Forms.Button();
            this.btnContention = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnSpinning
            // 
            this.btnSpinning.Location = new System.Drawing.Point(70, 69);
            this.btnSpinning.Name = "btnSpinning";
            this.btnSpinning.Size = new System.Drawing.Size(75, 23);
            this.btnSpinning.TabIndex = 0;
            this.btnSpinning.Text = "Spinning";
            this.btnSpinning.UseVisualStyleBackColor = true;
            this.btnSpinning.Click += new System.EventHandler(this.btnSpinning_Click);
            // 
            // btnDeadlock
            // 
            this.btnDeadlock.Location = new System.Drawing.Point(70, 30);
            this.btnDeadlock.Name = "btnDeadlock";
            this.btnDeadlock.Size = new System.Drawing.Size(75, 23);
            this.btnDeadlock.TabIndex = 1;
            this.btnDeadlock.Text = "Deadlock";
            this.btnDeadlock.UseVisualStyleBackColor = true;
            this.btnDeadlock.Click += new System.EventHandler(this.btnDeadlock_Click);
            // 
            // btnContention
            // 
            this.btnContention.Location = new System.Drawing.Point(70, 108);
            this.btnContention.Name = "btnContention";
            this.btnContention.Size = new System.Drawing.Size(75, 23);
            this.btnContention.TabIndex = 2;
            this.btnContention.Text = "Contention";
            this.btnContention.UseVisualStyleBackColor = true;
            this.btnContention.Click += new System.EventHandler(this.btnContention_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.Controls.Add(this.btnContention);
            this.Controls.Add(this.btnDeadlock);
            this.Controls.Add(this.btnSpinning);
            this.Name = "MainForm";
            this.Text = "CSApplicationHang";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnSpinning;
        private System.Windows.Forms.Button btnDeadlock;
        private System.Windows.Forms.Button btnContention;
    }
}

