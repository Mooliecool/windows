namespace CSSetPowerAvailabilityRequest
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
            this.btnSetDisplayRequired = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.btnSetSystemRequired = new System.Windows.Forms.Button();
            this.btnSetAwayModeRequired = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.tbReason = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // btnSetDisplayRequired
            // 
            this.btnSetDisplayRequired.Location = new System.Drawing.Point(206, 12);
            this.btnSetDisplayRequired.Name = "btnSetDisplayRequired";
            this.btnSetDisplayRequired.Size = new System.Drawing.Size(75, 23);
            this.btnSetDisplayRequired.TabIndex = 0;
            this.btnSetDisplayRequired.Text = "Set";
            this.btnSetDisplayRequired.UseVisualStyleBackColor = true;
            this.btnSetDisplayRequired.Click += new System.EventHandler(this.btnSetDisplayRequired_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 17);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(175, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Prevent display power management";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(66, 47);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(121, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Prevent automatic sleep";
            // 
            // btnSetSystemRequired
            // 
            this.btnSetSystemRequired.Location = new System.Drawing.Point(206, 43);
            this.btnSetSystemRequired.Name = "btnSetSystemRequired";
            this.btnSetSystemRequired.Size = new System.Drawing.Size(75, 23);
            this.btnSetSystemRequired.TabIndex = 3;
            this.btnSetSystemRequired.Text = "Set";
            this.btnSetSystemRequired.UseVisualStyleBackColor = true;
            this.btnSetSystemRequired.Click += new System.EventHandler(this.btnSetSystemRequired_Click);
            // 
            // btnSetAwayModeRequired
            // 
            this.btnSetAwayModeRequired.Location = new System.Drawing.Point(206, 74);
            this.btnSetAwayModeRequired.Name = "btnSetAwayModeRequired";
            this.btnSetAwayModeRequired.Size = new System.Drawing.Size(75, 23);
            this.btnSetAwayModeRequired.TabIndex = 4;
            this.btnSetAwayModeRequired.Text = "Set";
            this.btnSetAwayModeRequired.UseVisualStyleBackColor = true;
            this.btnSetAwayModeRequired.Click += new System.EventHandler(this.btnSetAwayModeRequired_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(83, 78);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(104, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "Enables Away Mode";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 110);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(116, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Power request reason: ";
            // 
            // tbReason
            // 
            this.tbReason.Location = new System.Drawing.Point(135, 107);
            this.tbReason.Name = "tbReason";
            this.tbReason.Size = new System.Drawing.Size(146, 20);
            this.tbReason.TabIndex = 7;
            this.tbReason.Text = "I\'m downloading a file.";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(293, 137);
            this.Controls.Add(this.tbReason);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.btnSetAwayModeRequired);
            this.Controls.Add(this.btnSetSystemRequired);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnSetDisplayRequired);
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.Text = "CSSetPowerAvailabilityRequest";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnSetDisplayRequired;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btnSetSystemRequired;
        private System.Windows.Forms.Button btnSetAwayModeRequired;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tbReason;
    }
}

