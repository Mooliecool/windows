namespace CSCOMClient
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.axMFCActiveX1 = new AxMFCActiveXLib.AxMFCActiveX();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.bnGetFloatProperty = new System.Windows.Forms.Button();
            this.bnSetFloatProperty = new System.Windows.Forms.Button();
            this.tbFloatProperty = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.axMFCActiveX1)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // axMFCActiveX1
            // 
            this.axMFCActiveX1.Enabled = true;
            this.axMFCActiveX1.Location = new System.Drawing.Point(0, 12);
            this.axMFCActiveX1.Name = "axMFCActiveX1";
            this.axMFCActiveX1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axMFCActiveX1.OcxState")));
            this.axMFCActiveX1.Size = new System.Drawing.Size(272, 95);
            this.axMFCActiveX1.TabIndex = 0;
            this.axMFCActiveX1.FloatPropertyChanging += new AxMFCActiveXLib._DMFCActiveXEvents_FloatPropertyChangingEventHandler(this.axMFCActiveX1_FloatPropertyChanging);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.bnGetFloatProperty);
            this.groupBox1.Controls.Add(this.bnSetFloatProperty);
            this.groupBox1.Controls.Add(this.tbFloatProperty);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(12, 113);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(250, 64);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Operation";
            // 
            // bnGetFloatProperty
            // 
            this.bnGetFloatProperty.Location = new System.Drawing.Point(211, 28);
            this.bnGetFloatProperty.Name = "bnGetFloatProperty";
            this.bnGetFloatProperty.Size = new System.Drawing.Size(33, 23);
            this.bnGetFloatProperty.TabIndex = 3;
            this.bnGetFloatProperty.Text = "Get";
            this.bnGetFloatProperty.UseVisualStyleBackColor = true;
            this.bnGetFloatProperty.Click += new System.EventHandler(this.bnGetFloatProperty_Click);
            // 
            // bnSetFloatProperty
            // 
            this.bnSetFloatProperty.Location = new System.Drawing.Point(179, 28);
            this.bnSetFloatProperty.Name = "bnSetFloatProperty";
            this.bnSetFloatProperty.Size = new System.Drawing.Size(32, 23);
            this.bnSetFloatProperty.TabIndex = 2;
            this.bnSetFloatProperty.Text = "Set";
            this.bnSetFloatProperty.UseVisualStyleBackColor = true;
            this.bnSetFloatProperty.Click += new System.EventHandler(this.bnSetFloatProperty_Click);
            // 
            // tbFloatProperty
            // 
            this.tbFloatProperty.Location = new System.Drawing.Point(85, 30);
            this.tbFloatProperty.MaxLength = 10;
            this.tbFloatProperty.Name = "tbFloatProperty";
            this.tbFloatProperty.Size = new System.Drawing.Size(91, 20);
            this.tbFloatProperty.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 33);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(72, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "FloatProperty:";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(275, 197);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.axMFCActiveX1);
            this.Name = "MainForm";
            this.Text = "CSCOMClient";
            ((System.ComponentModel.ISupportInitialize)(this.axMFCActiveX1)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private AxMFCActiveXLib.AxMFCActiveX axMFCActiveX1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbFloatProperty;
        private System.Windows.Forms.Button bnGetFloatProperty;
        private System.Windows.Forms.Button bnSetFloatProperty;
    }
}