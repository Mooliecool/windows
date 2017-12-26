namespace CSVstoExcelWorkbook
{
    partial class CourseQueryPane
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
            this.components = new System.ComponentModel.Container();
            this.lblTitle = new System.Windows.Forms.Label();
            this.lblName = new System.Windows.Forms.Label();
            this.cmdQuery = new System.Windows.Forms.Button();
            this.cboName = new System.Windows.Forms.ComboBox();
            this.schoolDataSet = new CSVstoExcelWorkbook.SchoolDataSet();
            this.studentListBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.studentListTableAdapter = new CSVstoExcelWorkbook.SchoolDataSetTableAdapters.StudentListTableAdapter();
            ((System.ComponentModel.ISupportInitialize)(this.schoolDataSet)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.studentListBindingSource)).BeginInit();
            this.SuspendLayout();
            // 
            // lblTitle
            // 
            this.lblTitle.AutoSize = true;
            this.lblTitle.Font = new System.Drawing.Font("Arial Unicode MS", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblTitle.Location = new System.Drawing.Point(4, 4);
            this.lblTitle.Name = "lblTitle";
            this.lblTitle.Size = new System.Drawing.Size(103, 16);
            this.lblTitle.TabIndex = 0;
            this.lblTitle.Text = "Query Courses";
            // 
            // lblName
            // 
            this.lblName.AutoSize = true;
            this.lblName.Location = new System.Drawing.Point(4, 34);
            this.lblName.Name = "lblName";
            this.lblName.Size = new System.Drawing.Size(38, 13);
            this.lblName.TabIndex = 1;
            this.lblName.Text = "Name:";
            // 
            // cmdQuery
            // 
            this.cmdQuery.Location = new System.Drawing.Point(100, 57);
            this.cmdQuery.Name = "cmdQuery";
            this.cmdQuery.Size = new System.Drawing.Size(75, 23);
            this.cmdQuery.TabIndex = 3;
            this.cmdQuery.Text = "Query";
            this.cmdQuery.UseVisualStyleBackColor = true;
            this.cmdQuery.Click += new System.EventHandler(this.cmdQuery_Click);
            // 
            // cboName
            // 
            this.cboName.DataSource = this.studentListBindingSource;
            this.cboName.DisplayMember = "StudentName";
            this.cboName.FormattingEnabled = true;
            this.cboName.Location = new System.Drawing.Point(48, 31);
            this.cboName.Name = "cboName";
            this.cboName.Size = new System.Drawing.Size(127, 21);
            this.cboName.TabIndex = 4;
            // 
            // schoolDataSet
            // 
            this.schoolDataSet.DataSetName = "SchoolDataSet";
            this.schoolDataSet.SchemaSerializationMode = System.Data.SchemaSerializationMode.IncludeSchema;
            // 
            // studentListBindingSource
            // 
            this.studentListBindingSource.DataMember = "StudentList";
            this.studentListBindingSource.DataSource = this.schoolDataSet;
            // 
            // studentListTableAdapter
            // 
            this.studentListTableAdapter.ClearBeforeFill = true;
            // 
            // CourseQueryPane
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.cboName);
            this.Controls.Add(this.cmdQuery);
            this.Controls.Add(this.lblName);
            this.Controls.Add(this.lblTitle);
            this.Name = "CourseQueryPane";
            this.Size = new System.Drawing.Size(193, 94);
            this.Load += new System.EventHandler(this.CourseQueryPane_Load);
            ((System.ComponentModel.ISupportInitialize)(this.schoolDataSet)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.studentListBindingSource)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lblTitle;
        private System.Windows.Forms.Label lblName;
        private System.Windows.Forms.Button cmdQuery;
        private System.Windows.Forms.ComboBox cboName;
        private System.Windows.Forms.BindingSource studentListBindingSource;
        private SchoolDataSet schoolDataSet;
        private CSVstoExcelWorkbook.SchoolDataSetTableAdapters.StudentListTableAdapter studentListTableAdapter;


    }
}
