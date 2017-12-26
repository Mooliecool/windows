namespace CSEncryption
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
            this.btnSymmetricAlgorithm = new System.Windows.Forms.Button();
            this.btnAsymmetricAlgorithm = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnSymmetricAlgorithm
            // 
            this.btnSymmetricAlgorithm.Location = new System.Drawing.Point(39, 28);
            this.btnSymmetricAlgorithm.Name = "btnSymmetricAlgorithm";
            this.btnSymmetricAlgorithm.Size = new System.Drawing.Size(174, 38);
            this.btnSymmetricAlgorithm.TabIndex = 0;
            this.btnSymmetricAlgorithm.Text = "SymmetricAlgorithm Demo";
            this.btnSymmetricAlgorithm.UseVisualStyleBackColor = true;
            this.btnSymmetricAlgorithm.Click += new System.EventHandler(this.btnSymmetricAlgorithmDemo_Click);
            // 
            // btnAsymmetricAlgorithm
            // 
            this.btnAsymmetricAlgorithm.Location = new System.Drawing.Point(39, 87);
            this.btnAsymmetricAlgorithm.Name = "btnAsymmetricAlgorithm";
            this.btnAsymmetricAlgorithm.Size = new System.Drawing.Size(174, 38);
            this.btnAsymmetricAlgorithm.TabIndex = 1;
            this.btnAsymmetricAlgorithm.Text = "AsymmetricAlgorithm Demo";
            this.btnAsymmetricAlgorithm.UseVisualStyleBackColor = true;
            this.btnAsymmetricAlgorithm.Click += new System.EventHandler(this.btnAsymmetricAlgorithm_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(251, 158);
            this.Controls.Add(this.btnAsymmetricAlgorithm);
            this.Controls.Add(this.btnSymmetricAlgorithm);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "MainForm";
            this.Text = "CSEncryption";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnSymmetricAlgorithm;
        private System.Windows.Forms.Button btnAsymmetricAlgorithm;
    }
}