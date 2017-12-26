namespace ExecuteScript
{
    partial class ExecuteScriptClient
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
            this.tbFeedback = new System.Windows.Forms.RichTextBox();
            this.btnExecuteScript = new System.Windows.Forms.Button();
            this.btnStartTarget = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // tbFeedback
            // 
            this.tbFeedback.Location = new System.Drawing.Point(12, 12);
            this.tbFeedback.Name = "tbFeedback";
            this.tbFeedback.Size = new System.Drawing.Size(268, 215);
            this.tbFeedback.TabIndex = 1;
            this.tbFeedback.Text = "";
            // 
            // btnExecuteScript
            // 
            this.btnExecuteScript.Enabled = false;
            this.btnExecuteScript.Location = new System.Drawing.Point(192, 231);
            this.btnExecuteScript.Name = "btnExecuteScript";
            this.btnExecuteScript.Size = new System.Drawing.Size(88, 23);
            this.btnExecuteScript.TabIndex = 3;
            this.btnExecuteScript.Text = "&Execute script";
            this.btnExecuteScript.UseVisualStyleBackColor = true;
            this.btnExecuteScript.Click += new System.EventHandler(this.ExecuteScript_Click);
            // 
            // btnStartTarget
            // 
            this.btnStartTarget.Location = new System.Drawing.Point(100, 231);
            this.btnStartTarget.Name = "btnStartTarget";
            this.btnStartTarget.Size = new System.Drawing.Size(88, 23);
            this.btnStartTarget.TabIndex = 2;
            this.btnStartTarget.Text = "&Start target";
            this.btnStartTarget.UseVisualStyleBackColor = true;
            this.btnStartTarget.Click += new System.EventHandler(this.StartTarget_Click);
            // 
            // ExecuteScriptClient
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Controls.Add(this.btnStartTarget);
            this.Controls.Add(this.btnExecuteScript);
            this.Controls.Add(this.tbFeedback);
            this.Name = "ExecuteScriptClient";
            this.Text = "Execute Script Client";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.RichTextBox tbFeedback;
        private System.Windows.Forms.Button btnExecuteScript;
        private System.Windows.Forms.Button btnStartTarget;

    }
}

