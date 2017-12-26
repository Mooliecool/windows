namespace CSLinqExtension
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
            this.btnLinqToCSV = new System.Windows.Forms.Button();
            this.btnLinqToFileSystem = new System.Windows.Forms.Button();
            this.btnLinqToReflection = new System.Windows.Forms.Button();
            this.btnLinqToString = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.txtBoxIntro = new System.Windows.Forms.TextBox();
            this.lblIntroduction = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnLinqToCSV
            // 
            this.btnLinqToCSV.Location = new System.Drawing.Point(41, 46);
            this.btnLinqToCSV.Name = "btnLinqToCSV";
            this.btnLinqToCSV.Size = new System.Drawing.Size(130, 30);
            this.btnLinqToCSV.TabIndex = 0;
            this.btnLinqToCSV.Text = "LINQ To CSV";
            this.btnLinqToCSV.UseVisualStyleBackColor = true;
            this.btnLinqToCSV.MouseLeave += new System.EventHandler(this.btn_MouseLeave);
            this.btnLinqToCSV.Click += new System.EventHandler(this.btnLinqToCSV_Click);
            this.btnLinqToCSV.MouseEnter += new System.EventHandler(this.btnLinqToCSV_MouseEnter);
            // 
            // btnLinqToFileSystem
            // 
            this.btnLinqToFileSystem.Location = new System.Drawing.Point(41, 120);
            this.btnLinqToFileSystem.Name = "btnLinqToFileSystem";
            this.btnLinqToFileSystem.Size = new System.Drawing.Size(130, 30);
            this.btnLinqToFileSystem.TabIndex = 1;
            this.btnLinqToFileSystem.Text = "LINQ To File System";
            this.btnLinqToFileSystem.UseVisualStyleBackColor = true;
            this.btnLinqToFileSystem.MouseLeave += new System.EventHandler(this.btn_MouseLeave);
            this.btnLinqToFileSystem.Click += new System.EventHandler(this.btnLinqToFileSystem_Click);
            this.btnLinqToFileSystem.MouseEnter += new System.EventHandler(this.btnLinqToFileSystem_MouseEnter);
            // 
            // btnLinqToReflection
            // 
            this.btnLinqToReflection.Location = new System.Drawing.Point(41, 194);
            this.btnLinqToReflection.Name = "btnLinqToReflection";
            this.btnLinqToReflection.Size = new System.Drawing.Size(130, 30);
            this.btnLinqToReflection.TabIndex = 2;
            this.btnLinqToReflection.Text = "LINQ To Reflection";
            this.btnLinqToReflection.UseVisualStyleBackColor = true;
            this.btnLinqToReflection.MouseLeave += new System.EventHandler(this.btn_MouseLeave);
            this.btnLinqToReflection.Click += new System.EventHandler(this.btnLinqToReflection_Click);
            this.btnLinqToReflection.MouseEnter += new System.EventHandler(this.btnLinqToReflection_MouseEnter);
            // 
            // btnLinqToString
            // 
            this.btnLinqToString.Location = new System.Drawing.Point(41, 268);
            this.btnLinqToString.Name = "btnLinqToString";
            this.btnLinqToString.Size = new System.Drawing.Size(130, 30);
            this.btnLinqToString.TabIndex = 3;
            this.btnLinqToString.Text = "LINQ To String";
            this.btnLinqToString.UseVisualStyleBackColor = true;
            this.btnLinqToString.MouseLeave += new System.EventHandler(this.btn_MouseLeave);
            this.btnLinqToString.Click += new System.EventHandler(this.btnLinqToString_Click);
            this.btnLinqToString.MouseEnter += new System.EventHandler(this.btnLinqToString_MouseEnter);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.groupBox2);
            this.groupBox1.Controls.Add(this.btnLinqToCSV);
            this.groupBox1.Controls.Add(this.btnLinqToFileSystem);
            this.groupBox1.Controls.Add(this.btnLinqToString);
            this.groupBox1.Controls.Add(this.btnLinqToReflection);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(535, 377);
            this.groupBox1.TabIndex = 5;
            this.groupBox1.TabStop = false;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.txtBoxIntro);
            this.groupBox2.Controls.Add(this.lblIntroduction);
            this.groupBox2.Location = new System.Drawing.Point(204, 20);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(312, 338);
            this.groupBox2.TabIndex = 5;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Introduction";
            // 
            // txtBoxIntro
            // 
            this.txtBoxIntro.Enabled = false;
            this.txtBoxIntro.Location = new System.Drawing.Point(17, 61);
            this.txtBoxIntro.Multiline = true;
            this.txtBoxIntro.Name = "txtBoxIntro";
            this.txtBoxIntro.Size = new System.Drawing.Size(281, 262);
            this.txtBoxIntro.TabIndex = 1;
            // 
            // lblIntroduction
            // 
            this.lblIntroduction.AutoSize = true;
            this.lblIntroduction.Location = new System.Drawing.Point(29, 35);
            this.lblIntroduction.Name = "lblIntroduction";
            this.lblIntroduction.Size = new System.Drawing.Size(0, 12);
            this.lblIntroduction.TabIndex = 0;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(559, 401);
            this.Controls.Add(this.groupBox1);
            this.Name = "MainForm";
            this.Text = "CSLinqExtension";
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnLinqToCSV;
        private System.Windows.Forms.Button btnLinqToFileSystem;
        private System.Windows.Forms.Button btnLinqToReflection;
        private System.Windows.Forms.Button btnLinqToString;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label lblIntroduction;
        private System.Windows.Forms.TextBox txtBoxIntro;
    }
}