namespace Company.CSVSPackageStatusBar
{
    partial class MyControl
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                if(components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }


        #region Component Designer generated code
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnWriteFeedback = new System.Windows.Forms.Button();
            this.btnReadFeedback = new System.Windows.Forms.Button();
            this.btnShowProgressBar = new System.Windows.Forms.Button();
            this.btnShowAnimation = new System.Windows.Forms.Button();
            this.btnUpdateDesignerRegion = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnWriteFeedback
            // 
            this.btnWriteFeedback.Location = new System.Drawing.Point(37, 22);
            this.btnWriteFeedback.Name = "btnWriteFeedback";
            this.btnWriteFeedback.Size = new System.Drawing.Size(309, 23);
            this.btnWriteFeedback.TabIndex = 0;
            this.btnWriteFeedback.Text = "Write Feedback in Status Bar";
            this.btnWriteFeedback.UseVisualStyleBackColor = true;
            this.btnWriteFeedback.Click += new System.EventHandler(this.btnWriteFeedback_Click);
            // 
            // btnReadFeedback
            // 
            this.btnReadFeedback.Location = new System.Drawing.Point(37, 52);
            this.btnReadFeedback.Name = "btnReadFeedback";
            this.btnReadFeedback.Size = new System.Drawing.Size(309, 23);
            this.btnReadFeedback.TabIndex = 1;
            this.btnReadFeedback.Text = "Read Feedback in Status Bar";
            this.btnReadFeedback.UseVisualStyleBackColor = true;
            this.btnReadFeedback.Click += new System.EventHandler(this.btnReadFeedback_Click);
            // 
            // btnShowProgressBar
            // 
            this.btnShowProgressBar.Location = new System.Drawing.Point(37, 82);
            this.btnShowProgressBar.Name = "btnShowProgressBar";
            this.btnShowProgressBar.Size = new System.Drawing.Size(309, 23);
            this.btnShowProgressBar.TabIndex = 2;
            this.btnShowProgressBar.Text = "Show Progress Bar";
            this.btnShowProgressBar.UseVisualStyleBackColor = true;
            this.btnShowProgressBar.Click += new System.EventHandler(this.btnShowProgressBar_Click);
            // 
            // btnShowAnimation
            // 
            this.btnShowAnimation.Location = new System.Drawing.Point(37, 112);
            this.btnShowAnimation.Name = "btnShowAnimation";
            this.btnShowAnimation.Size = new System.Drawing.Size(309, 23);
            this.btnShowAnimation.TabIndex = 3;
            this.btnShowAnimation.Text = "Show Animation in Status Bar";
            this.btnShowAnimation.UseVisualStyleBackColor = true;
            this.btnShowAnimation.Click += new System.EventHandler(this.btnShowAnimation_Click);
            // 
            // btnUpdateDesignerRegion
            // 
            this.btnUpdateDesignerRegion.Location = new System.Drawing.Point(37, 142);
            this.btnUpdateDesignerRegion.Name = "btnUpdateDesignerRegion";
            this.btnUpdateDesignerRegion.Size = new System.Drawing.Size(309, 23);
            this.btnUpdateDesignerRegion.TabIndex = 4;
            this.btnUpdateDesignerRegion.Text = "Update Designer Region";
            this.btnUpdateDesignerRegion.UseVisualStyleBackColor = true;
            this.btnUpdateDesignerRegion.Click += new System.EventHandler(this.btnUpdateDesignerRegion_Click);
            // 
            // MyControl
            // 
            this.BackColor = System.Drawing.SystemColors.Window;
            this.Controls.Add(this.btnUpdateDesignerRegion);
            this.Controls.Add(this.btnShowAnimation);
            this.Controls.Add(this.btnShowProgressBar);
            this.Controls.Add(this.btnReadFeedback);
            this.Controls.Add(this.btnWriteFeedback);
            this.Name = "MyControl";
            this.Size = new System.Drawing.Size(383, 313);
            this.ResumeLayout(false);

        }
        #endregion

        private System.Windows.Forms.Button btnWriteFeedback;
        private System.Windows.Forms.Button btnReadFeedback;
        private System.Windows.Forms.Button btnShowProgressBar;
        private System.Windows.Forms.Button btnShowAnimation;
        private System.Windows.Forms.Button btnUpdateDesignerRegion;

    }
}
