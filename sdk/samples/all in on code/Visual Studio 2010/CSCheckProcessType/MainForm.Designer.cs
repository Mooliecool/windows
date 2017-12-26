namespace CSCheckProcessType
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
        /// <param name="disposing">
        /// true if managed resources should be disposed; otherwise, false.
        /// </param>
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
            this.pnlProcess = new System.Windows.Forms.Panel();
            this.gvProcess = new System.Windows.Forms.DataGridView();
            this.pnlInfo = new System.Windows.Forms.Panel();
            this.btnRefresh = new System.Windows.Forms.Button();
            this.ProcID = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ProcessName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Is64BitProcess = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.IsManaged = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.IsDotNet4 = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.IsWPF = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.IsConsole = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.Remarks = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.pnlProcess.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gvProcess)).BeginInit();
            this.pnlInfo.SuspendLayout();
            this.SuspendLayout();
            // 
            // pnlProcess
            // 
            this.pnlProcess.Controls.Add(this.gvProcess);
            this.pnlProcess.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlProcess.Location = new System.Drawing.Point(0, 44);
            this.pnlProcess.Name = "pnlProcess";
            this.pnlProcess.Size = new System.Drawing.Size(935, 394);
            this.pnlProcess.TabIndex = 0;
            // 
            // gvProcess
            // 
            this.gvProcess.AllowUserToAddRows = false;
            this.gvProcess.AllowUserToDeleteRows = false;
            this.gvProcess.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.gvProcess.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ProcID,
            this.ProcessName,
            this.Is64BitProcess,
            this.IsManaged,
            this.IsDotNet4,
            this.IsWPF,
            this.IsConsole,
            this.Remarks});
            this.gvProcess.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gvProcess.Location = new System.Drawing.Point(0, 0);
            this.gvProcess.Name = "gvProcess";
            this.gvProcess.ReadOnly = true;
            this.gvProcess.RowHeadersVisible = false;
            this.gvProcess.Size = new System.Drawing.Size(935, 394);
            this.gvProcess.TabIndex = 0;
            // 
            // pnlInfo
            // 
            this.pnlInfo.Controls.Add(this.btnRefresh);
            this.pnlInfo.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlInfo.Location = new System.Drawing.Point(0, 0);
            this.pnlInfo.Name = "pnlInfo";
            this.pnlInfo.Size = new System.Drawing.Size(935, 44);
            this.pnlInfo.TabIndex = 1;
            // 
            // btnRefresh
            // 
            this.btnRefresh.Location = new System.Drawing.Point(12, 3);
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Size = new System.Drawing.Size(75, 35);
            this.btnRefresh.TabIndex = 0;
            this.btnRefresh.Text = "Refresh";
            this.btnRefresh.UseVisualStyleBackColor = true;
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            // 
            // ProcID
            // 
            this.ProcID.DataPropertyName = "Id";
            this.ProcID.HeaderText = "ProcID";
            this.ProcID.Name = "ProcID";
            this.ProcID.ReadOnly = true;
            // 
            // ProcessName
            // 
            this.ProcessName.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.ProcessName.DataPropertyName = "ProcessName";
            this.ProcessName.HeaderText = "Name";
            this.ProcessName.Name = "ProcessName";
            this.ProcessName.ReadOnly = true;
            // 
            // Is64BitProcess
            // 
            this.Is64BitProcess.DataPropertyName = "Is64BitProcess";
            this.Is64BitProcess.HeaderText = "Is64BitProcess";
            this.Is64BitProcess.Name = "Is64BitProcess";
            this.Is64BitProcess.ReadOnly = true;
            // 
            // IsManaged
            // 
            this.IsManaged.DataPropertyName = "IsManaged";
            this.IsManaged.HeaderText = "IsManaged";
            this.IsManaged.Name = "IsManaged";
            this.IsManaged.ReadOnly = true;
            // 
            // IsDotNet4
            // 
            this.IsDotNet4.DataPropertyName = "IsDotNet4";
            this.IsDotNet4.HeaderText = "IsDotNet4";
            this.IsDotNet4.Name = "IsDotNet4";
            this.IsDotNet4.ReadOnly = true;
            // 
            // IsWPF
            // 
            this.IsWPF.DataPropertyName = "IsWPF";
            this.IsWPF.HeaderText = "IsWPF";
            this.IsWPF.Name = "IsWPF";
            this.IsWPF.ReadOnly = true;
            // 
            // IsConsole
            // 
            this.IsConsole.DataPropertyName = "IsConsole";
            this.IsConsole.HeaderText = "IsConsole";
            this.IsConsole.Name = "IsConsole";
            this.IsConsole.ReadOnly = true;
            // 
            // Remarks
            // 
            this.Remarks.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.Remarks.DataPropertyName = "Remarks";
            this.Remarks.HeaderText = "Remarks";
            this.Remarks.Name = "Remarks";
            this.Remarks.ReadOnly = true;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(935, 438);
            this.Controls.Add(this.pnlProcess);
            this.Controls.Add(this.pnlInfo);
            this.Name = "MainForm";
            this.Text = "MainForm";
            this.pnlProcess.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.gvProcess)).EndInit();
            this.pnlInfo.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel pnlProcess;
        private System.Windows.Forms.DataGridView gvProcess;
        private System.Windows.Forms.Panel pnlInfo;
        private System.Windows.Forms.Button btnRefresh;
        private System.Windows.Forms.DataGridViewTextBoxColumn ProcID;
        private System.Windows.Forms.DataGridViewTextBoxColumn ProcessName;
        private System.Windows.Forms.DataGridViewCheckBoxColumn Is64BitProcess;
        private System.Windows.Forms.DataGridViewCheckBoxColumn IsManaged;
        private System.Windows.Forms.DataGridViewCheckBoxColumn IsDotNet4;
        private System.Windows.Forms.DataGridViewCheckBoxColumn IsWPF;
        private System.Windows.Forms.DataGridViewCheckBoxColumn IsConsole;
        private System.Windows.Forms.DataGridViewTextBoxColumn Remarks;
    }
}