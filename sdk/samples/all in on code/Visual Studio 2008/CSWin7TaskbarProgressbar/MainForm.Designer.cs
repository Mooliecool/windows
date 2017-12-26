namespace CSWin7TaskbarProgressbar
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
            this.components = new System.ComponentModel.Container();
            this.progressBar = new System.Windows.Forms.ProgressBar();
            this.startButton = new System.Windows.Forms.Button();
            this.progreeBarTimer = new System.Windows.Forms.Timer(this.components);
            this.pauseButton = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.flashButton = new System.Windows.Forms.Button();
            this.errorButton = new System.Windows.Forms.Button();
            this.indeterminateButton = new System.Windows.Forms.Button();
            this.flashWindowTimer = new System.Windows.Forms.Timer(this.components);
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // progressBar
            // 
            this.progressBar.Location = new System.Drawing.Point(29, 32);
            this.progressBar.Name = "progressBar";
            this.progressBar.Size = new System.Drawing.Size(428, 23);
            this.progressBar.TabIndex = 0;
            // 
            // startButton
            // 
            this.startButton.Location = new System.Drawing.Point(43, 79);
            this.startButton.Name = "startButton";
            this.startButton.Size = new System.Drawing.Size(75, 23);
            this.startButton.TabIndex = 1;
            this.startButton.Text = "Start";
            this.startButton.UseVisualStyleBackColor = true;
            this.startButton.Click += new System.EventHandler(this.startButton_Click);
            // 
            // progreeBarTimer
            // 
            this.progreeBarTimer.Tick += new System.EventHandler(this.progreeBarTimer_Tick);
            // 
            // pauseButton
            // 
            this.pauseButton.Location = new System.Drawing.Point(124, 79);
            this.pauseButton.Name = "pauseButton";
            this.pauseButton.Size = new System.Drawing.Size(75, 23);
            this.pauseButton.TabIndex = 2;
            this.pauseButton.Text = "Pause";
            this.pauseButton.UseVisualStyleBackColor = true;
            this.pauseButton.Click += new System.EventHandler(this.pauseButton_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.flashButton);
            this.groupBox1.Controls.Add(this.errorButton);
            this.groupBox1.Controls.Add(this.indeterminateButton);
            this.groupBox1.Controls.Add(this.progressBar);
            this.groupBox1.Controls.Add(this.pauseButton);
            this.groupBox1.Controls.Add(this.startButton);
            this.groupBox1.Location = new System.Drawing.Point(33, 26);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(492, 127);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            // 
            // flashButton
            // 
            this.flashButton.Location = new System.Drawing.Point(367, 79);
            this.flashButton.Name = "flashButton";
            this.flashButton.Size = new System.Drawing.Size(75, 23);
            this.flashButton.TabIndex = 5;
            this.flashButton.Text = "Flash";
            this.flashButton.UseVisualStyleBackColor = true;
            this.flashButton.Click += new System.EventHandler(this.flashButton_Click);
            // 
            // errorButton
            // 
            this.errorButton.Location = new System.Drawing.Point(286, 79);
            this.errorButton.Name = "errorButton";
            this.errorButton.Size = new System.Drawing.Size(75, 23);
            this.errorButton.TabIndex = 4;
            this.errorButton.Text = "Error";
            this.errorButton.UseVisualStyleBackColor = true;
            this.errorButton.Click += new System.EventHandler(this.errorButton_Click);
            // 
            // indeterminateButton
            // 
            this.indeterminateButton.Location = new System.Drawing.Point(205, 79);
            this.indeterminateButton.Name = "indeterminateButton";
            this.indeterminateButton.Size = new System.Drawing.Size(75, 23);
            this.indeterminateButton.TabIndex = 3;
            this.indeterminateButton.Text = "Indeterminate";
            this.indeterminateButton.UseVisualStyleBackColor = true;
            this.indeterminateButton.Click += new System.EventHandler(this.indeterminateButton_Click);
            // 
            // flashWindowTimer
            // 
            this.flashWindowTimer.Interval = 400;
            this.flashWindowTimer.Tick += new System.EventHandler(this.flashWindowTimer_Tick);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(559, 200);
            this.Controls.Add(this.groupBox1);
            this.Name = "MainForm";
            this.Text = "Win7 Taskbar Progressbar";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ProgressBar progressBar;
        private System.Windows.Forms.Button startButton;
        private System.Windows.Forms.Timer progreeBarTimer;
        private System.Windows.Forms.Button pauseButton;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button indeterminateButton;
        private System.Windows.Forms.Button errorButton;
        private System.Windows.Forms.Button flashButton;
        private System.Windows.Forms.Timer flashWindowTimer;
    }
}

