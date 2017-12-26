namespace Enviroment_GUI
{
    partial class EnviromentGUI
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
            this.panel1 = new System.Windows.Forms.Panel();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.BarrierButton = new System.Windows.Forms.Button();
            this.SourceButton = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.RestartButton = new System.Windows.Forms.Button();
            this.StartButton = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.button1 = new System.Windows.Forms.Button();
            this.MinVPram = new System.Windows.Forms.TextBox();
            this.Clear = new System.Windows.Forms.Button();
            this.Writepram = new System.Windows.Forms.Button();
            this.label10 = new System.Windows.Forms.Label();
            this.SR_param = new System.Windows.Forms.TextBox();
            this.CR_param = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.MV_param = new System.Windows.Forms.TextBox();
            this.G_param = new System.Windows.Forms.TextBox();
            this.B_param = new System.Windows.Forms.TextBox();
            this.W_param = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.AgentsNum = new System.Windows.Forms.TextBox();
            this.GenerateButton = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel1.Location = new System.Drawing.Point(12, 12);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(556, 468);
            this.panel1.TabIndex = 0;
            this.panel1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseDown);
            this.panel1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseMove);
            this.panel1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseUp);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.BarrierButton);
            this.groupBox1.Controls.Add(this.SourceButton);
            this.groupBox1.Location = new System.Drawing.Point(17, 19);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(125, 127);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Objects";
            // 
            // BarrierButton
            // 
            this.BarrierButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.BarrierButton.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.BarrierButton.Image = global::Enviroment_GUI.Properties.Resources.brick_wall;
            this.BarrierButton.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.BarrierButton.Location = new System.Drawing.Point(24, 19);
            this.BarrierButton.Name = "BarrierButton";
            this.BarrierButton.Size = new System.Drawing.Size(77, 33);
            this.BarrierButton.TabIndex = 0;
            this.BarrierButton.Text = "Barrier2";
            this.BarrierButton.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.BarrierButton.UseVisualStyleBackColor = true;
            // 
            // SourceButton
            // 
            this.SourceButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.SourceButton.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.SourceButton.ForeColor = System.Drawing.Color.Snow;
            this.SourceButton.Image = global::Enviroment_GUI.Properties.Resources._4row_radiator;
            this.SourceButton.Location = new System.Drawing.Point(24, 69);
            this.SourceButton.Name = "SourceButton";
            this.SourceButton.Size = new System.Drawing.Size(77, 33);
            this.SourceButton.TabIndex = 0;
            this.SourceButton.Text = "Source";
            this.SourceButton.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.SourceButton.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.RestartButton);
            this.groupBox2.Controls.Add(this.StartButton);
            this.groupBox2.Location = new System.Drawing.Point(23, 361);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(119, 101);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Operations";
            // 
            // RestartButton
            // 
            this.RestartButton.Location = new System.Drawing.Point(9, 59);
            this.RestartButton.Name = "RestartButton";
            this.RestartButton.Size = new System.Drawing.Size(104, 29);
            this.RestartButton.TabIndex = 0;
            this.RestartButton.Text = "Restart";
            this.RestartButton.UseVisualStyleBackColor = true;
            this.RestartButton.Click += new System.EventHandler(this.RestartButton_Click);
            // 
            // StartButton
            // 
            this.StartButton.Enabled = false;
            this.StartButton.Location = new System.Drawing.Point(9, 24);
            this.StartButton.Name = "StartButton";
            this.StartButton.Size = new System.Drawing.Size(104, 29);
            this.StartButton.TabIndex = 0;
            this.StartButton.Text = "Start";
            this.StartButton.UseVisualStyleBackColor = true;
            this.StartButton.Click += new System.EventHandler(this.StartButton_Click);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.groupBox4);
            this.groupBox3.Controls.Add(this.label3);
            this.groupBox3.Controls.Add(this.textBox1);
            this.groupBox3.Controls.Add(this.label1);
            this.groupBox3.Controls.Add(this.AgentsNum);
            this.groupBox3.Controls.Add(this.GenerateButton);
            this.groupBox3.Controls.Add(this.label2);
            this.groupBox3.Controls.Add(this.comboBox1);
            this.groupBox3.Controls.Add(this.groupBox1);
            this.groupBox3.Controls.Add(this.groupBox2);
            this.groupBox3.Location = new System.Drawing.Point(574, 12);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(406, 468);
            this.groupBox3.TabIndex = 5;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Utilities";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.button1);
            this.groupBox4.Controls.Add(this.MinVPram);
            this.groupBox4.Controls.Add(this.Clear);
            this.groupBox4.Controls.Add(this.Writepram);
            this.groupBox4.Controls.Add(this.label10);
            this.groupBox4.Controls.Add(this.SR_param);
            this.groupBox4.Controls.Add(this.CR_param);
            this.groupBox4.Controls.Add(this.label9);
            this.groupBox4.Controls.Add(this.label8);
            this.groupBox4.Controls.Add(this.label7);
            this.groupBox4.Controls.Add(this.label6);
            this.groupBox4.Controls.Add(this.label5);
            this.groupBox4.Controls.Add(this.label4);
            this.groupBox4.Controls.Add(this.MV_param);
            this.groupBox4.Controls.Add(this.G_param);
            this.groupBox4.Controls.Add(this.B_param);
            this.groupBox4.Controls.Add(this.W_param);
            this.groupBox4.Location = new System.Drawing.Point(194, 25);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(206, 390);
            this.groupBox4.TabIndex = 11;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Swarm Particles Parameters";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(24, 362);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(156, 27);
            this.button1.TabIndex = 14;
            this.button1.Text = "Load Default";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // MinVPram
            // 
            this.MinVPram.Location = new System.Drawing.Point(145, 173);
            this.MinVPram.Name = "MinVPram";
            this.MinVPram.Size = new System.Drawing.Size(55, 20);
            this.MinVPram.TabIndex = 13;
            // 
            // Clear
            // 
            this.Clear.Location = new System.Drawing.Point(22, 325);
            this.Clear.Name = "Clear";
            this.Clear.Size = new System.Drawing.Size(158, 28);
            this.Clear.TabIndex = 12;
            this.Clear.Text = "Clear";
            this.Clear.UseVisualStyleBackColor = true;
            this.Clear.Click += new System.EventHandler(this.Clear_Click);
            // 
            // Writepram
            // 
            this.Writepram.Location = new System.Drawing.Point(22, 285);
            this.Writepram.Name = "Writepram";
            this.Writepram.Size = new System.Drawing.Size(159, 25);
            this.Writepram.TabIndex = 11;
            this.Writepram.Text = "Write Parameters ";
            this.Writepram.UseVisualStyleBackColor = true;
            this.Writepram.Click += new System.EventHandler(this.Writepram_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(19, 242);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(80, 13);
            this.label10.TabIndex = 10;
            this.label10.Text = "Sensing Range";
            // 
            // SR_param
            // 
            this.SR_param.Location = new System.Drawing.Point(145, 239);
            this.SR_param.Name = "SR_param";
            this.SR_param.Size = new System.Drawing.Size(56, 20);
            this.SR_param.TabIndex = 9;
            // 
            // CR_param
            // 
            this.CR_param.Location = new System.Drawing.Point(145, 209);
            this.CR_param.Name = "CR_param";
            this.CR_param.Size = new System.Drawing.Size(58, 20);
            this.CR_param.TabIndex = 8;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(19, 212);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(112, 13);
            this.label9.TabIndex = 7;
            this.label9.Text = "Communcation Range";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(19, 170);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(64, 13);
            this.label8.TabIndex = 7;
            this.label8.Text = "Min Velocity";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(19, 132);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(67, 13);
            this.label7.TabIndex = 5;
            this.label7.Text = "Max Velocity";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(19, 97);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(15, 13);
            this.label6.TabIndex = 4;
            this.label6.Text = "G";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(19, 70);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(14, 13);
            this.label5.TabIndex = 4;
            this.label5.Text = "B";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(19, 40);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(18, 13);
            this.label4.TabIndex = 4;
            this.label4.Text = "W";
            // 
            // MV_param
            // 
            this.MV_param.Location = new System.Drawing.Point(141, 133);
            this.MV_param.Name = "MV_param";
            this.MV_param.Size = new System.Drawing.Size(62, 20);
            this.MV_param.TabIndex = 3;
            // 
            // G_param
            // 
            this.G_param.Location = new System.Drawing.Point(141, 101);
            this.G_param.Name = "G_param";
            this.G_param.Size = new System.Drawing.Size(64, 20);
            this.G_param.TabIndex = 2;
            // 
            // B_param
            // 
            this.B_param.Location = new System.Drawing.Point(141, 67);
            this.B_param.Name = "B_param";
            this.B_param.Size = new System.Drawing.Size(63, 20);
            this.B_param.TabIndex = 1;
            // 
            // W_param
            // 
            this.W_param.Location = new System.Drawing.Point(141, 41);
            this.W_param.Name = "W_param";
            this.W_param.Size = new System.Drawing.Size(63, 20);
            this.W_param.TabIndex = 0;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(47, 310);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(53, 13);
            this.label3.TabIndex = 10;
            this.label3.Text = "Iterations:";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(114, 307);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(74, 20);
            this.textBox1.TabIndex = 9;
            this.textBox1.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 240);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(94, 13);
            this.label1.TabIndex = 8;
            this.label1.Text = "Number Of Agents";
            // 
            // AgentsNum
            // 
            this.AgentsNum.Location = new System.Drawing.Point(112, 237);
            this.AgentsNum.Name = "AgentsNum";
            this.AgentsNum.Size = new System.Drawing.Size(74, 20);
            this.AgentsNum.TabIndex = 7;
            // 
            // GenerateButton
            // 
            this.GenerateButton.Location = new System.Drawing.Point(26, 263);
            this.GenerateButton.Name = "GenerateButton";
            this.GenerateButton.Size = new System.Drawing.Size(160, 29);
            this.GenerateButton.TabIndex = 1;
            this.GenerateButton.Text = "Generate";
            this.GenerateButton.UseVisualStyleBackColor = true;
            this.GenerateButton.Click += new System.EventHandler(this.GenerateButton_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(23, 190);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(68, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Source Type";
            // 
            // comboBox1
            // 
            this.comboBox1.FormattingEnabled = true;
            this.comboBox1.Items.AddRange(new object[] {
            "Gaussian Function Source",
            "Multiple Gaussian Function Sources",
            "Multiple Noise Gaussian Function Sources"});
            this.comboBox1.Location = new System.Drawing.Point(26, 206);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(160, 21);
            this.comboBox1.TabIndex = 5;
            this.comboBox1.Text = "Gaussian Function Source";
            // 
            // backgroundWorker1
            // 
            this.backgroundWorker1.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker1_DoWork);
            this.backgroundWorker1.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.back);
            // 
            // EnviromentGUI
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(992, 492);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.panel1);
            this.Name = "EnviromentGUI";
            this.Text = "Enviroment";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.EnviromentGUI_FormClosing);
            this.Load += new System.EventHandler(this.EnviromentGUI_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button BarrierButton;
        private System.Windows.Forms.Button SourceButton;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button RestartButton;
        private System.Windows.Forms.Button StartButton;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox comboBox1;
        private System.Windows.Forms.Button GenerateButton;
        private System.Windows.Forms.TextBox AgentsNum;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox MV_param;
        private System.Windows.Forms.TextBox G_param;
        private System.Windows.Forms.TextBox B_param;
        private System.Windows.Forms.TextBox W_param;
        private System.Windows.Forms.Button Clear;
        private System.Windows.Forms.Button Writepram;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox SR_param;
        private System.Windows.Forms.TextBox CR_param;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox MinVPram;
        private System.Windows.Forms.Button button1;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
    }
}

