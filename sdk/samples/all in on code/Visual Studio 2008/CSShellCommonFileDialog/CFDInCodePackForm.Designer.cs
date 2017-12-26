namespace CSShellCommonFileDialog
{
    partial class CFDInCodePackForm
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
            this.grpFileOpenDialogs = new System.Windows.Forms.GroupBox();
            this.btnAddCommonPlaces = new System.Windows.Forms.Button();
            this.btnAddCustomControls = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.btnOpenAFolder = new System.Windows.Forms.Button();
            this.btnOpenFiles = new System.Windows.Forms.Button();
            this.btnOpenAFile = new System.Windows.Forms.Button();
            this.grpFileSaveDialogs = new System.Windows.Forms.GroupBox();
            this.btnSaveAFile = new System.Windows.Forms.Button();
            this.grpFileOpenDialogs.SuspendLayout();
            this.grpFileSaveDialogs.SuspendLayout();
            this.SuspendLayout();
            // 
            // grpFileOpenDialogs
            // 
            this.grpFileOpenDialogs.Controls.Add(this.btnAddCommonPlaces);
            this.grpFileOpenDialogs.Controls.Add(this.btnAddCustomControls);
            this.grpFileOpenDialogs.Controls.Add(this.label1);
            this.grpFileOpenDialogs.Controls.Add(this.btnOpenAFolder);
            this.grpFileOpenDialogs.Controls.Add(this.btnOpenFiles);
            this.grpFileOpenDialogs.Controls.Add(this.btnOpenAFile);
            this.grpFileOpenDialogs.Location = new System.Drawing.Point(13, 13);
            this.grpFileOpenDialogs.Name = "grpFileOpenDialogs";
            this.grpFileOpenDialogs.Size = new System.Drawing.Size(303, 130);
            this.grpFileOpenDialogs.TabIndex = 0;
            this.grpFileOpenDialogs.TabStop = false;
            this.grpFileOpenDialogs.Text = "Common Open File Dialog";
            // 
            // btnAddCommonPlaces
            // 
            this.btnAddCommonPlaces.Location = new System.Drawing.Point(153, 86);
            this.btnAddCommonPlaces.Name = "btnAddCommonPlaces";
            this.btnAddCommonPlaces.Size = new System.Drawing.Size(138, 30);
            this.btnAddCommonPlaces.TabIndex = 6;
            this.btnAddCommonPlaces.Text = "Add Common Places";
            this.btnAddCommonPlaces.UseVisualStyleBackColor = true;
            this.btnAddCommonPlaces.Click += new System.EventHandler(this.btnAddCommonPlaces_Click);
            // 
            // btnAddCustomControls
            // 
            this.btnAddCustomControls.Location = new System.Drawing.Point(9, 86);
            this.btnAddCustomControls.Name = "btnAddCustomControls";
            this.btnAddCustomControls.Size = new System.Drawing.Size(138, 30);
            this.btnAddCustomControls.TabIndex = 5;
            this.btnAddCustomControls.Text = "Add Custom Controls";
            this.btnAddCustomControls.UseVisualStyleBackColor = true;
            this.btnAddCustomControls.Click += new System.EventHandler(this.btnAddCustomControls_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 63);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(142, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Customized Open File Dialog";
            // 
            // btnOpenAFolder
            // 
            this.btnOpenAFolder.Location = new System.Drawing.Point(201, 23);
            this.btnOpenAFolder.Name = "btnOpenAFolder";
            this.btnOpenAFolder.Size = new System.Drawing.Size(90, 30);
            this.btnOpenAFolder.TabIndex = 2;
            this.btnOpenAFolder.Text = "Open a Folder";
            this.btnOpenAFolder.UseVisualStyleBackColor = true;
            this.btnOpenAFolder.Click += new System.EventHandler(this.btnOpenAFolder_Click);
            // 
            // btnOpenFiles
            // 
            this.btnOpenFiles.Location = new System.Drawing.Point(105, 23);
            this.btnOpenFiles.Name = "btnOpenFiles";
            this.btnOpenFiles.Size = new System.Drawing.Size(90, 30);
            this.btnOpenFiles.TabIndex = 1;
            this.btnOpenFiles.Text = "Open Files";
            this.btnOpenFiles.UseVisualStyleBackColor = true;
            this.btnOpenFiles.Click += new System.EventHandler(this.btnOpenFiles_Click);
            // 
            // btnOpenAFile
            // 
            this.btnOpenAFile.Location = new System.Drawing.Point(9, 23);
            this.btnOpenAFile.Name = "btnOpenAFile";
            this.btnOpenAFile.Size = new System.Drawing.Size(90, 30);
            this.btnOpenAFile.TabIndex = 0;
            this.btnOpenAFile.Text = "Open a File";
            this.btnOpenAFile.UseVisualStyleBackColor = true;
            this.btnOpenAFile.Click += new System.EventHandler(this.btnOpenAFile_Click);
            // 
            // grpFileSaveDialogs
            // 
            this.grpFileSaveDialogs.Controls.Add(this.btnSaveAFile);
            this.grpFileSaveDialogs.Location = new System.Drawing.Point(12, 157);
            this.grpFileSaveDialogs.Name = "grpFileSaveDialogs";
            this.grpFileSaveDialogs.Size = new System.Drawing.Size(303, 106);
            this.grpFileSaveDialogs.TabIndex = 1;
            this.grpFileSaveDialogs.TabStop = false;
            this.grpFileSaveDialogs.Text = "Common Save File Dialog";
            // 
            // btnSaveAFile
            // 
            this.btnSaveAFile.Location = new System.Drawing.Point(9, 23);
            this.btnSaveAFile.Name = "btnSaveAFile";
            this.btnSaveAFile.Size = new System.Drawing.Size(90, 30);
            this.btnSaveAFile.TabIndex = 0;
            this.btnSaveAFile.Text = "Save a File";
            this.btnSaveAFile.UseVisualStyleBackColor = true;
            this.btnSaveAFile.Click += new System.EventHandler(this.btnSaveAFile_Click);
            // 
            // CFDInCodePackForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(327, 275);
            this.Controls.Add(this.grpFileSaveDialogs);
            this.Controls.Add(this.grpFileOpenDialogs);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "CFDInCodePackForm";
            this.Text = "Common File Dialog in Code Pack";
            this.grpFileOpenDialogs.ResumeLayout(false);
            this.grpFileOpenDialogs.PerformLayout();
            this.grpFileSaveDialogs.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox grpFileOpenDialogs;
        private System.Windows.Forms.Button btnOpenAFile;
        private System.Windows.Forms.Button btnOpenFiles;
        private System.Windows.Forms.Button btnOpenAFolder;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnAddCustomControls;
        private System.Windows.Forms.Button btnAddCommonPlaces;
        private System.Windows.Forms.GroupBox grpFileSaveDialogs;
        private System.Windows.Forms.Button btnSaveAFile;

    }
}