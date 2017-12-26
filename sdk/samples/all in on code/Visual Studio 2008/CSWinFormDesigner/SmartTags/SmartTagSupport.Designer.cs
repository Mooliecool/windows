namespace CSWinFormDesigner.SmartTags
{
    partial class SmartTagSupport
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
            this.label1 = new System.Windows.Forms.Label();
            this.uC_SmartTagSupport1 = new CSWinFormDesigner.SmartTags.UC_SmartTagSupport();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Verdana", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(41, 39);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(488, 42);
            this.label1.TabIndex = 1;
            this.label1.Text = "This sample demonstrates how to add smart tag to a control at design time.\r\n\r\nSel" +
                "ect the following control and click the smart tag at the right-top corner.";
            // 
            // uC_SmartTagSupport1
            // 
            this.uC_SmartTagSupport1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(255)))));
            this.uC_SmartTagSupport1.ForeColor = System.Drawing.SystemColors.ControlLightLight;
            this.uC_SmartTagSupport1.Location = new System.Drawing.Point(44, 126);
            this.uC_SmartTagSupport1.Name = "uC_SmartTagSupport1";
            this.uC_SmartTagSupport1.Size = new System.Drawing.Size(174, 123);
            this.uC_SmartTagSupport1.TabIndex = 0;
            // 
            // SmartTagSupport
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(574, 346);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.uC_SmartTagSupport1);
            this.Name = "SmartTagSupport";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private UC_SmartTagSupport uC_SmartTagSupport1;
        private System.Windows.Forms.Label label1;

    }
}