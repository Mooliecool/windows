namespace Microsoft.CSVSPackageAddReferenceTab
{
    partial class ReferencePageDialog
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
            this.ReferenceListView = new System.Windows.Forms.ListView();
            this.columnName = new System.Windows.Forms.ColumnHeader();
            this.columnPath = new System.Windows.Forms.ColumnHeader();
            this.SuspendLayout();
            // 
            // ReferenceListView
            // 
            this.ReferenceListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnName,
            this.columnPath});
            this.ReferenceListView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.ReferenceListView.Location = new System.Drawing.Point(0, 0);
            this.ReferenceListView.Name = "ReferenceListView";
            this.ReferenceListView.Size = new System.Drawing.Size(414, 289);
            this.ReferenceListView.TabIndex = 0;
            this.ReferenceListView.UseCompatibleStateImageBehavior = false;
            this.ReferenceListView.View = System.Windows.Forms.View.Details;
            this.ReferenceListView.DoubleClick += new System.EventHandler(this.ReferenceListView_DoubleClick);
            this.ReferenceListView.ItemSelectionChanged += new System.Windows.Forms.ListViewItemSelectionChangedEventHandler(this.ReferenceListView_ItemSelectionChanged);
            // 
            // columnName
            // 
            this.columnName.Text = "Component";
            // 
            // columnPath
            // 
            this.columnPath.Text = "Path";
            // 
            // ReferencePageDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.ReferenceListView);
            this.Name = "ReferencePageDialog";
            this.Size = new System.Drawing.Size(414, 289);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView ReferenceListView;
        public System.Windows.Forms.ColumnHeader columnName;
        public System.Windows.Forms.ColumnHeader columnPath;

    }
}
