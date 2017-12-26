namespace CSVstoWordDocument
{
    partial class PaneControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.BtnInsertCC = new System.Windows.Forms.Button();
            this.btnInsertBuiltIn = new System.Windows.Forms.Button();
            this.btnCCXMLMapping = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // BtnInsertCC
            // 
            this.BtnInsertCC.Location = new System.Drawing.Point(0, 0);
            this.BtnInsertCC.Name = "BtnInsertCC";
            this.BtnInsertCC.Size = new System.Drawing.Size(147, 23);
            this.BtnInsertCC.TabIndex = 0;
            this.BtnInsertCC.Text = "Insert ContentControls";
            this.BtnInsertCC.UseVisualStyleBackColor = true;
            this.BtnInsertCC.Click += new System.EventHandler(this.BtnInsertCC_Click);
            // 
            // btnInsertBuiltIn
            // 
            this.btnInsertBuiltIn.Location = new System.Drawing.Point(0, 29);
            this.btnInsertBuiltIn.Name = "btnInsertBuiltIn";
            this.btnInsertBuiltIn.Size = new System.Drawing.Size(147, 23);
            this.btnInsertBuiltIn.TabIndex = 1;
            this.btnInsertBuiltIn.Text = "Insert Building Blocks";
            this.btnInsertBuiltIn.UseVisualStyleBackColor = true;
            this.btnInsertBuiltIn.Click += new System.EventHandler(this.btnInsertBuiltIn_Click);
            // 
            // btnCCXMLMapping
            // 
            this.btnCCXMLMapping.Location = new System.Drawing.Point(0, 58);
            this.btnCCXMLMapping.Name = "btnCCXMLMapping";
            this.btnCCXMLMapping.Size = new System.Drawing.Size(147, 40);
            this.btnCCXMLMapping.TabIndex = 2;
            this.btnCCXMLMapping.Text = "Content Control XMLMapping ";
            this.btnCCXMLMapping.UseVisualStyleBackColor = true;
            this.btnCCXMLMapping.Click += new System.EventHandler(this.btnCCXMLMapping_Click);
            // 
            // PaneControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnCCXMLMapping);
            this.Controls.Add(this.btnInsertBuiltIn);
            this.Controls.Add(this.BtnInsertCC);
            this.Name = "PaneControl";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button BtnInsertCC;
        private System.Windows.Forms.Button btnInsertBuiltIn;
        private System.Windows.Forms.Button btnCCXMLMapping;
    }
}
