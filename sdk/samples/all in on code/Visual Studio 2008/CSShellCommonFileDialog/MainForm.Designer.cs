namespace CSShellCommonFileDialog
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
            this.btnCFDInBCL = new System.Windows.Forms.Button();
            this.btnCFDInCodePack = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnCFDInBCL
            // 
            this.btnCFDInBCL.Location = new System.Drawing.Point(67, 54);
            this.btnCFDInBCL.Name = "btnCFDInBCL";
            this.btnCFDInBCL.Size = new System.Drawing.Size(165, 30);
            this.btnCFDInBCL.TabIndex = 0;
            this.btnCFDInBCL.Text = "Common File Dialog in BCL";
            this.btnCFDInBCL.UseVisualStyleBackColor = true;
            this.btnCFDInBCL.Click += new System.EventHandler(this.btnCFDInBCL_Click);
            // 
            // btnCFDInCodePack
            // 
            this.btnCFDInCodePack.Location = new System.Drawing.Point(67, 105);
            this.btnCFDInCodePack.Name = "btnCFDInCodePack";
            this.btnCFDInCodePack.Size = new System.Drawing.Size(165, 49);
            this.btnCFDInCodePack.TabIndex = 1;
            this.btnCFDInCodePack.Text = "Common File Dialog in Windows API Code Pack";
            this.btnCFDInCodePack.UseVisualStyleBackColor = true;
            this.btnCFDInCodePack.Click += new System.EventHandler(this.btnCFDInCodePack_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(300, 226);
            this.Controls.Add(this.btnCFDInCodePack);
            this.Controls.Add(this.btnCFDInBCL);
            this.Name = "MainForm";
            this.Text = "CSShellCommonFileDialog";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnCFDInBCL;
        private System.Windows.Forms.Button btnCFDInCodePack;
    }
}

