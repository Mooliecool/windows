namespace CSLinqExtension
{
    partial class LinqToStringForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LinqToStringForm));
            this.txtBoxDigit = new System.Windows.Forms.TextBox();
            this.btnDigits = new System.Windows.Forms.Button();
            this.txtSearchSource = new System.Windows.Forms.TextBox();
            this.btnSearchOccurence = new System.Windows.Forms.Button();
            this.btnSearchSentence = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label2 = new System.Windows.Forms.Label();
            this.txtBoxSearchItems = new System.Windows.Forms.TextBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.txtBoxOccurence = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // txtBoxDigit
            // 
            this.txtBoxDigit.Location = new System.Drawing.Point(20, 24);
            this.txtBoxDigit.Name = "txtBoxDigit";
            this.txtBoxDigit.Size = new System.Drawing.Size(214, 20);
            this.txtBoxDigit.TabIndex = 0;
            this.txtBoxDigit.Text = "A1B2C3D4E5F6G7H8I9J0";
            // 
            // btnDigits
            // 
            this.btnDigits.Location = new System.Drawing.Point(467, 24);
            this.btnDigits.Name = "btnDigits";
            this.btnDigits.Size = new System.Drawing.Size(87, 25);
            this.btnDigits.TabIndex = 1;
            this.btnDigits.Text = "Show Digits";
            this.btnDigits.UseVisualStyleBackColor = true;
            this.btnDigits.Click += new System.EventHandler(this.btnDigist_Click);
            // 
            // txtSearchSource
            // 
            this.txtSearchSource.Location = new System.Drawing.Point(20, 25);
            this.txtSearchSource.Multiline = true;
            this.txtSearchSource.Name = "txtSearchSource";
            this.txtSearchSource.Size = new System.Drawing.Size(534, 161);
            this.txtSearchSource.TabIndex = 2;
            this.txtSearchSource.Text = resources.GetString("txtSearchSource.Text");
            // 
            // btnSearchOccurence
            // 
            this.btnSearchOccurence.Location = new System.Drawing.Point(447, 23);
            this.btnSearchOccurence.Name = "btnSearchOccurence";
            this.btnSearchOccurence.Size = new System.Drawing.Size(75, 25);
            this.btnSearchOccurence.TabIndex = 3;
            this.btnSearchOccurence.Text = "Search";
            this.btnSearchOccurence.UseVisualStyleBackColor = true;
            this.btnSearchOccurence.Click += new System.EventHandler(this.btnSearchCount_Click);
            // 
            // btnSearchSentence
            // 
            this.btnSearchSentence.Location = new System.Drawing.Point(447, 21);
            this.btnSearchSentence.Name = "btnSearchSentence";
            this.btnSearchSentence.Size = new System.Drawing.Size(75, 25);
            this.btnSearchSentence.TabIndex = 4;
            this.btnSearchSentence.Text = "Search";
            this.btnSearchSentence.UseVisualStyleBackColor = true;
            this.btnSearchSentence.Click += new System.EventHandler(this.btnSearchSentence_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.txtBoxDigit);
            this.groupBox1.Controls.Add(this.btnDigits);
            this.groupBox1.Location = new System.Drawing.Point(22, 24);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(580, 69);
            this.groupBox1.TabIndex = 5;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Show Digits in the String";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.groupBox4);
            this.groupBox2.Controls.Add(this.groupBox3);
            this.groupBox2.Controls.Add(this.txtSearchSource);
            this.groupBox2.Location = new System.Drawing.Point(22, 117);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(580, 391);
            this.groupBox2.TabIndex = 6;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Search Word";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.label2);
            this.groupBox4.Controls.Add(this.txtBoxSearchItems);
            this.groupBox4.Controls.Add(this.btnSearchSentence);
            this.groupBox4.Location = new System.Drawing.Point(20, 288);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(534, 64);
            this.groupBox4.TabIndex = 8;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Query for Sentences that Contain a Specified Set of Words";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(21, 27);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(72, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Search Items:";
            // 
            // txtBoxSearchItems
            // 
            this.txtBoxSearchItems.Location = new System.Drawing.Point(116, 22);
            this.txtBoxSearchItems.Name = "txtBoxSearchItems";
            this.txtBoxSearchItems.Size = new System.Drawing.Size(202, 20);
            this.txtBoxSearchItems.TabIndex = 5;
            this.txtBoxSearchItems.Text = "framework,skeleton,Microsoft";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.label1);
            this.groupBox3.Controls.Add(this.txtBoxOccurence);
            this.groupBox3.Controls.Add(this.btnSearchOccurence);
            this.groupBox3.Location = new System.Drawing.Point(20, 210);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(534, 61);
            this.groupBox3.TabIndex = 7;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Count Occurences of a Word in the String ";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(20, 28);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(67, 13);
            this.label1.TabIndex = 6;
            this.label1.Text = "Search Item:";
            // 
            // txtBoxOccurence
            // 
            this.txtBoxOccurence.Location = new System.Drawing.Point(116, 25);
            this.txtBoxOccurence.Name = "txtBoxOccurence";
            this.txtBoxOccurence.Size = new System.Drawing.Size(202, 20);
            this.txtBoxOccurence.TabIndex = 5;
            this.txtBoxOccurence.Text = "you";
            // 
            // LinqToStringForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(630, 531);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "LinqToStringForm";
            this.Text = "LINQ To String";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox txtBoxDigit;
        private System.Windows.Forms.Button btnDigits;
        private System.Windows.Forms.TextBox txtSearchSource;
        private System.Windows.Forms.Button btnSearchOccurence;
        private System.Windows.Forms.Button btnSearchSentence;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox txtBoxOccurence;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtBoxSearchItems;
    }
}