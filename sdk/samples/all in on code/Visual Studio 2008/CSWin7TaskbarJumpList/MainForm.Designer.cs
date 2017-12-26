namespace CSWin7TaskbarJumpList
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
            this.btnRegisterFileType = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btnUnregisterFileType = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.btnChooseFile = new System.Windows.Forms.Button();
            this.radRecent = new System.Windows.Forms.RadioButton();
            this.radFrequent = new System.Windows.Forms.RadioButton();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.chkPaint = new System.Windows.Forms.CheckBox();
            this.chkCalc = new System.Windows.Forms.CheckBox();
            this.chkNotepad = new System.Windows.Forms.CheckBox();
            this.btnClearTask = new System.Windows.Forms.Button();
            this.btnAddTask = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.btnAddLink = new System.Windows.Forms.Button();
            this.tbLink = new System.Windows.Forms.TextBox();
            this.btnAddItem = new System.Windows.Forms.Button();
            this.tbItem = new System.Windows.Forms.TextBox();
            this.tbCategory = new System.Windows.Forms.TextBox();
            this.btnCreateCategory = new System.Windows.Forms.Button();
            this.recentFileOpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnRegisterFileType
            // 
            this.btnRegisterFileType.Location = new System.Drawing.Point(20, 19);
            this.btnRegisterFileType.Name = "btnRegisterFileType";
            this.btnRegisterFileType.Size = new System.Drawing.Size(115, 41);
            this.btnRegisterFileType.TabIndex = 0;
            this.btnRegisterFileType.Text = "Register File Type";
            this.btnRegisterFileType.UseVisualStyleBackColor = true;
            this.btnRegisterFileType.Click += new System.EventHandler(this.btnRegisterFileType_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.btnUnregisterFileType);
            this.groupBox1.Controls.Add(this.btnRegisterFileType);
            this.groupBox1.Location = new System.Drawing.Point(28, 22);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(334, 82);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "File Type Registration";
            // 
            // btnUnregisterFileType
            // 
            this.btnUnregisterFileType.Location = new System.Drawing.Point(192, 19);
            this.btnUnregisterFileType.Name = "btnUnregisterFileType";
            this.btnUnregisterFileType.Size = new System.Drawing.Size(115, 41);
            this.btnUnregisterFileType.TabIndex = 1;
            this.btnUnregisterFileType.Text = "Unregister File Type";
            this.btnUnregisterFileType.UseVisualStyleBackColor = true;
            this.btnUnregisterFileType.Click += new System.EventHandler(this.btnUnregisterFileType_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.btnChooseFile);
            this.groupBox2.Controls.Add(this.radRecent);
            this.groupBox2.Controls.Add(this.radFrequent);
            this.groupBox2.Location = new System.Drawing.Point(28, 110);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(334, 78);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Frequent/Recent Category";
            // 
            // btnChooseFile
            // 
            this.btnChooseFile.Location = new System.Drawing.Point(223, 19);
            this.btnChooseFile.Name = "btnChooseFile";
            this.btnChooseFile.Size = new System.Drawing.Size(84, 23);
            this.btnChooseFile.TabIndex = 4;
            this.btnChooseFile.Text = "Choose File...";
            this.btnChooseFile.UseVisualStyleBackColor = true;
            this.btnChooseFile.Click += new System.EventHandler(this.btnChooseFile_Click);
            // 
            // radRecent
            // 
            this.radRecent.AutoSize = true;
            this.radRecent.Location = new System.Drawing.Point(20, 42);
            this.radRecent.Name = "radRecent";
            this.radRecent.Size = new System.Drawing.Size(60, 17);
            this.radRecent.TabIndex = 3;
            this.radRecent.TabStop = true;
            this.radRecent.Text = "Recent";
            this.radRecent.UseVisualStyleBackColor = true;
            this.radRecent.CheckedChanged += new System.EventHandler(this.radFrequentRecent_CheckedChanged);
            // 
            // radFrequent
            // 
            this.radFrequent.AutoSize = true;
            this.radFrequent.Location = new System.Drawing.Point(20, 19);
            this.radFrequent.Name = "radFrequent";
            this.radFrequent.Size = new System.Drawing.Size(67, 17);
            this.radFrequent.TabIndex = 0;
            this.radFrequent.TabStop = true;
            this.radFrequent.Text = "Frequent";
            this.radFrequent.UseVisualStyleBackColor = true;
            this.radFrequent.CheckedChanged += new System.EventHandler(this.radFrequentRecent_CheckedChanged);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.chkPaint);
            this.groupBox3.Controls.Add(this.chkCalc);
            this.groupBox3.Controls.Add(this.chkNotepad);
            this.groupBox3.Controls.Add(this.btnClearTask);
            this.groupBox3.Controls.Add(this.btnAddTask);
            this.groupBox3.Location = new System.Drawing.Point(28, 194);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(334, 89);
            this.groupBox3.TabIndex = 3;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Tasks";
            // 
            // chkPaint
            // 
            this.chkPaint.AutoSize = true;
            this.chkPaint.Location = new System.Drawing.Point(20, 58);
            this.chkPaint.Name = "chkPaint";
            this.chkPaint.Size = new System.Drawing.Size(49, 17);
            this.chkPaint.TabIndex = 4;
            this.chkPaint.Text = "paint";
            this.chkPaint.UseVisualStyleBackColor = true;
            // 
            // chkCalc
            // 
            this.chkCalc.AutoSize = true;
            this.chkCalc.Location = new System.Drawing.Point(20, 39);
            this.chkCalc.Name = "chkCalc";
            this.chkCalc.Size = new System.Drawing.Size(72, 17);
            this.chkCalc.TabIndex = 3;
            this.chkCalc.Text = "calculator";
            this.chkCalc.UseVisualStyleBackColor = true;
            // 
            // chkNotepad
            // 
            this.chkNotepad.AutoSize = true;
            this.chkNotepad.Location = new System.Drawing.Point(20, 20);
            this.chkNotepad.Name = "chkNotepad";
            this.chkNotepad.Size = new System.Drawing.Size(65, 17);
            this.chkNotepad.TabIndex = 2;
            this.chkNotepad.Text = "notepad";
            this.chkNotepad.UseVisualStyleBackColor = true;
            // 
            // btnClearTask
            // 
            this.btnClearTask.Location = new System.Drawing.Point(223, 34);
            this.btnClearTask.Name = "btnClearTask";
            this.btnClearTask.Size = new System.Drawing.Size(84, 23);
            this.btnClearTask.TabIndex = 1;
            this.btnClearTask.Text = "Clear Tasks";
            this.btnClearTask.UseVisualStyleBackColor = true;
            this.btnClearTask.Click += new System.EventHandler(this.btnClearTask_Click);
            // 
            // btnAddTask
            // 
            this.btnAddTask.Location = new System.Drawing.Point(135, 34);
            this.btnAddTask.Name = "btnAddTask";
            this.btnAddTask.Size = new System.Drawing.Size(82, 23);
            this.btnAddTask.TabIndex = 0;
            this.btnAddTask.Text = "Add Tasks";
            this.btnAddTask.UseVisualStyleBackColor = true;
            this.btnAddTask.Click += new System.EventHandler(this.btnAddTask_Click);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.btnAddLink);
            this.groupBox4.Controls.Add(this.tbLink);
            this.groupBox4.Controls.Add(this.btnAddItem);
            this.groupBox4.Controls.Add(this.tbItem);
            this.groupBox4.Controls.Add(this.tbCategory);
            this.groupBox4.Controls.Add(this.btnCreateCategory);
            this.groupBox4.Location = new System.Drawing.Point(28, 289);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(334, 89);
            this.groupBox4.TabIndex = 4;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Categories";
            // 
            // btnAddLink
            // 
            this.btnAddLink.Location = new System.Drawing.Point(242, 47);
            this.btnAddLink.Name = "btnAddLink";
            this.btnAddLink.Size = new System.Drawing.Size(65, 23);
            this.btnAddLink.TabIndex = 9;
            this.btnAddLink.Text = "Add Link";
            this.btnAddLink.UseVisualStyleBackColor = true;
            this.btnAddLink.Click += new System.EventHandler(this.btnAddLink_Click);
            // 
            // tbLink
            // 
            this.tbLink.Location = new System.Drawing.Point(172, 49);
            this.tbLink.Name = "tbLink";
            this.tbLink.Size = new System.Drawing.Size(68, 20);
            this.tbLink.TabIndex = 8;
            this.tbLink.Text = "Link1";
            // 
            // btnAddItem
            // 
            this.btnAddItem.Location = new System.Drawing.Point(98, 47);
            this.btnAddItem.Name = "btnAddItem";
            this.btnAddItem.Size = new System.Drawing.Size(65, 23);
            this.btnAddItem.TabIndex = 7;
            this.btnAddItem.Text = "Add Item";
            this.btnAddItem.UseVisualStyleBackColor = true;
            this.btnAddItem.Click += new System.EventHandler(this.btnAddItem_Click);
            // 
            // tbItem
            // 
            this.tbItem.Location = new System.Drawing.Point(20, 49);
            this.tbItem.Name = "tbItem";
            this.tbItem.Size = new System.Drawing.Size(77, 20);
            this.tbItem.TabIndex = 6;
            this.tbItem.Text = "Item1";
            // 
            // tbCategory
            // 
            this.tbCategory.Location = new System.Drawing.Point(20, 22);
            this.tbCategory.Name = "tbCategory";
            this.tbCategory.Size = new System.Drawing.Size(77, 20);
            this.tbCategory.TabIndex = 5;
            this.tbCategory.Text = "MyCategory";
            // 
            // btnCreateCategory
            // 
            this.btnCreateCategory.Location = new System.Drawing.Point(98, 20);
            this.btnCreateCategory.Name = "btnCreateCategory";
            this.btnCreateCategory.Size = new System.Drawing.Size(65, 23);
            this.btnCreateCategory.TabIndex = 0;
            this.btnCreateCategory.Text = "Create";
            this.btnCreateCategory.UseVisualStyleBackColor = true;
            this.btnCreateCategory.Click += new System.EventHandler(this.btnCreateCategory_Click);
            // 
            // recentFileOpenFileDialog
            // 
            this.recentFileOpenFileDialog.Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*\"";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(385, 411);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "MainForm";
            this.Text = "Win7 Taskbar JumpList";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnRegisterFileType;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.RadioButton radFrequent;
        private System.Windows.Forms.Button btnChooseFile;
        private System.Windows.Forms.RadioButton radRecent;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.CheckBox chkNotepad;
        private System.Windows.Forms.Button btnClearTask;
        private System.Windows.Forms.Button btnAddTask;
        private System.Windows.Forms.CheckBox chkPaint;
        private System.Windows.Forms.CheckBox chkCalc;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.TextBox tbCategory;
        private System.Windows.Forms.Button btnCreateCategory;
        private System.Windows.Forms.Button btnAddLink;
        private System.Windows.Forms.TextBox tbLink;
        private System.Windows.Forms.Button btnAddItem;
        private System.Windows.Forms.TextBox tbItem;
        private System.Windows.Forms.Button btnUnregisterFileType;
        private System.Windows.Forms.OpenFileDialog recentFileOpenFileDialog;
    }
}

