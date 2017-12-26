using CSUseADONET;
partial class RelatedTableUpdateForm
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
        System.Windows.Forms.Label departmentIDLabel;
        System.Windows.Forms.Label nameLabel;
        System.Windows.Forms.Label budgetLabel;
        System.Windows.Forms.Label startDateLabel;
        System.Windows.Forms.Label administratorLabel;
        System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(RelatedTableUpdateForm));
        this.sQLServer2005DBDataSet = new CSUseADONET.SQLServer2005DBDataSet();
        this.departmentBindingSource = new System.Windows.Forms.BindingSource(this.components);
        this.departmentTableAdapter = new CSUseADONET.SQLServer2005DBDataSetTableAdapters.DepartmentTableAdapter();
        this.tableAdapterManager = new CSUseADONET.SQLServer2005DBDataSetTableAdapters.TableAdapterManager();
        this.courseTableAdapter = new CSUseADONET.SQLServer2005DBDataSetTableAdapters.CourseTableAdapter();
        this.departmentBindingNavigator = new System.Windows.Forms.BindingNavigator(this.components);
        this.bindingNavigatorAddNewItem = new System.Windows.Forms.ToolStripButton();
        this.bindingNavigatorCountItem = new System.Windows.Forms.ToolStripLabel();
        this.bindingNavigatorDeleteItem = new System.Windows.Forms.ToolStripButton();
        this.bindingNavigatorMoveFirstItem = new System.Windows.Forms.ToolStripButton();
        this.bindingNavigatorMovePreviousItem = new System.Windows.Forms.ToolStripButton();
        this.bindingNavigatorSeparator = new System.Windows.Forms.ToolStripSeparator();
        this.bindingNavigatorPositionItem = new System.Windows.Forms.ToolStripTextBox();
        this.bindingNavigatorSeparator1 = new System.Windows.Forms.ToolStripSeparator();
        this.bindingNavigatorMoveNextItem = new System.Windows.Forms.ToolStripButton();
        this.bindingNavigatorMoveLastItem = new System.Windows.Forms.ToolStripButton();
        this.bindingNavigatorSeparator2 = new System.Windows.Forms.ToolStripSeparator();
        this.departmentBindingNavigatorSaveItem = new System.Windows.Forms.ToolStripButton();
        this.departmentBindingNavigatorSaveItem2 = new System.Windows.Forms.ToolStripButton();
        this.departmentIDTextBox = new System.Windows.Forms.TextBox();
        this.nameTextBox = new System.Windows.Forms.TextBox();
        this.budgetTextBox = new System.Windows.Forms.TextBox();
        this.startDateDateTimePicker = new System.Windows.Forms.DateTimePicker();
        this.administratorTextBox = new System.Windows.Forms.TextBox();
        this.courseBindingSource = new System.Windows.Forms.BindingSource(this.components);
        this.courseDataGridView = new System.Windows.Forms.DataGridView();
        this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
        this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
        this.dataGridViewTextBoxColumn3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
        this.dataGridViewTextBoxColumn4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
        departmentIDLabel = new System.Windows.Forms.Label();
        nameLabel = new System.Windows.Forms.Label();
        budgetLabel = new System.Windows.Forms.Label();
        startDateLabel = new System.Windows.Forms.Label();
        administratorLabel = new System.Windows.Forms.Label();
        ((System.ComponentModel.ISupportInitialize)(this.sQLServer2005DBDataSet)).BeginInit();
        ((System.ComponentModel.ISupportInitialize)(this.departmentBindingSource)).BeginInit();
        ((System.ComponentModel.ISupportInitialize)(this.departmentBindingNavigator)).BeginInit();
        this.departmentBindingNavigator.SuspendLayout();
        ((System.ComponentModel.ISupportInitialize)(this.courseBindingSource)).BeginInit();
        ((System.ComponentModel.ISupportInitialize)(this.courseDataGridView)).BeginInit();
        this.SuspendLayout();
        // 
        // departmentIDLabel
        // 
        departmentIDLabel.AutoSize = true;
        departmentIDLabel.Location = new System.Drawing.Point(130, 55);
        departmentIDLabel.Name = "departmentIDLabel";
        departmentIDLabel.Size = new System.Drawing.Size(79, 13);
        departmentIDLabel.TabIndex = 1;
        departmentIDLabel.Text = "Department ID:";
        // 
        // nameLabel
        // 
        nameLabel.AutoSize = true;
        nameLabel.Location = new System.Drawing.Point(130, 81);
        nameLabel.Name = "nameLabel";
        nameLabel.Size = new System.Drawing.Size(38, 13);
        nameLabel.TabIndex = 3;
        nameLabel.Text = "Name:";
        // 
        // budgetLabel
        // 
        budgetLabel.AutoSize = true;
        budgetLabel.Location = new System.Drawing.Point(130, 107);
        budgetLabel.Name = "budgetLabel";
        budgetLabel.Size = new System.Drawing.Size(44, 13);
        budgetLabel.TabIndex = 5;
        budgetLabel.Text = "Budget:";
        // 
        // startDateLabel
        // 
        startDateLabel.AutoSize = true;
        startDateLabel.Location = new System.Drawing.Point(130, 134);
        startDateLabel.Name = "startDateLabel";
        startDateLabel.Size = new System.Drawing.Size(58, 13);
        startDateLabel.TabIndex = 7;
        startDateLabel.Text = "Start Date:";
        // 
        // administratorLabel
        // 
        administratorLabel.AutoSize = true;
        administratorLabel.Location = new System.Drawing.Point(130, 159);
        administratorLabel.Name = "administratorLabel";
        administratorLabel.Size = new System.Drawing.Size(70, 13);
        administratorLabel.TabIndex = 9;
        administratorLabel.Text = "Administrator:";
        // 
        // sQLServer2005DBDataSet
        // 
        this.sQLServer2005DBDataSet.DataSetName = "SQLServer2005DBDataSet";
        this.sQLServer2005DBDataSet.SchemaSerializationMode = System.Data.SchemaSerializationMode.IncludeSchema;
        // 
        // departmentBindingSource
        // 
        this.departmentBindingSource.DataMember = "Department";
        this.departmentBindingSource.DataSource = this.sQLServer2005DBDataSet;
        // 
        // departmentTableAdapter
        // 
        this.departmentTableAdapter.ClearBeforeFill = true;
        // 
        // tableAdapterManager
        // 
        this.tableAdapterManager.BackupDataSetBeforeUpdate = false;
        this.tableAdapterManager.CourseGradeTableAdapter = null;
        this.tableAdapterManager.CourseInstructorTableAdapter = null;
        this.tableAdapterManager.CourseTableAdapter = this.courseTableAdapter;
        this.tableAdapterManager.DepartmentTableAdapter = this.departmentTableAdapter;
        this.tableAdapterManager.OfficeAssignmentTableAdapter = null;
        this.tableAdapterManager.OnlineCourseTableAdapter = null;
        this.tableAdapterManager.OnsiteCourseTableAdapter = null;
        this.tableAdapterManager.PersonTableAdapter = null;
        this.tableAdapterManager.UpdateOrder = CSUseADONET.SQLServer2005DBDataSetTableAdapters.TableAdapterManager.UpdateOrderOption.InsertUpdateDelete;
        // 
        // courseTableAdapter
        // 
        this.courseTableAdapter.ClearBeforeFill = true;
        // 
        // departmentBindingNavigator
        // 
        this.departmentBindingNavigator.AddNewItem = this.bindingNavigatorAddNewItem;
        this.departmentBindingNavigator.BindingSource = this.departmentBindingSource;
        this.departmentBindingNavigator.CountItem = this.bindingNavigatorCountItem;
        this.departmentBindingNavigator.DeleteItem = this.bindingNavigatorDeleteItem;
        this.departmentBindingNavigator.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.bindingNavigatorMoveFirstItem,
            this.bindingNavigatorMovePreviousItem,
            this.bindingNavigatorSeparator,
            this.bindingNavigatorPositionItem,
            this.bindingNavigatorCountItem,
            this.bindingNavigatorSeparator1,
            this.bindingNavigatorMoveNextItem,
            this.bindingNavigatorMoveLastItem,
            this.bindingNavigatorSeparator2,
            this.bindingNavigatorAddNewItem,
            this.bindingNavigatorDeleteItem,
            this.departmentBindingNavigatorSaveItem,
            this.departmentBindingNavigatorSaveItem2});
        this.departmentBindingNavigator.Location = new System.Drawing.Point(0, 0);
        this.departmentBindingNavigator.MoveFirstItem = this.bindingNavigatorMoveFirstItem;
        this.departmentBindingNavigator.MoveLastItem = this.bindingNavigatorMoveLastItem;
        this.departmentBindingNavigator.MoveNextItem = this.bindingNavigatorMoveNextItem;
        this.departmentBindingNavigator.MovePreviousItem = this.bindingNavigatorMovePreviousItem;
        this.departmentBindingNavigator.Name = "departmentBindingNavigator";
        this.departmentBindingNavigator.PositionItem = this.bindingNavigatorPositionItem;
        this.departmentBindingNavigator.Size = new System.Drawing.Size(607, 25);
        this.departmentBindingNavigator.TabIndex = 0;
        this.departmentBindingNavigator.Text = "bindingNavigator1";
        // 
        // bindingNavigatorAddNewItem
        // 
        this.bindingNavigatorAddNewItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
        this.bindingNavigatorAddNewItem.Image = ((System.Drawing.Image)(resources.GetObject("bindingNavigatorAddNewItem.Image")));
        this.bindingNavigatorAddNewItem.Name = "bindingNavigatorAddNewItem";
        this.bindingNavigatorAddNewItem.RightToLeftAutoMirrorImage = true;
        this.bindingNavigatorAddNewItem.Size = new System.Drawing.Size(23, 22);
        this.bindingNavigatorAddNewItem.Text = "Add new";
        // 
        // bindingNavigatorCountItem
        // 
        this.bindingNavigatorCountItem.Name = "bindingNavigatorCountItem";
        this.bindingNavigatorCountItem.Size = new System.Drawing.Size(36, 22);
        this.bindingNavigatorCountItem.Text = "of {0}";
        this.bindingNavigatorCountItem.ToolTipText = "Total number of items";
        // 
        // bindingNavigatorDeleteItem
        // 
        this.bindingNavigatorDeleteItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
        this.bindingNavigatorDeleteItem.Image = ((System.Drawing.Image)(resources.GetObject("bindingNavigatorDeleteItem.Image")));
        this.bindingNavigatorDeleteItem.Name = "bindingNavigatorDeleteItem";
        this.bindingNavigatorDeleteItem.RightToLeftAutoMirrorImage = true;
        this.bindingNavigatorDeleteItem.Size = new System.Drawing.Size(23, 22);
        this.bindingNavigatorDeleteItem.Text = "Delete";
        // 
        // bindingNavigatorMoveFirstItem
        // 
        this.bindingNavigatorMoveFirstItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
        this.bindingNavigatorMoveFirstItem.Image = ((System.Drawing.Image)(resources.GetObject("bindingNavigatorMoveFirstItem.Image")));
        this.bindingNavigatorMoveFirstItem.Name = "bindingNavigatorMoveFirstItem";
        this.bindingNavigatorMoveFirstItem.RightToLeftAutoMirrorImage = true;
        this.bindingNavigatorMoveFirstItem.Size = new System.Drawing.Size(23, 22);
        this.bindingNavigatorMoveFirstItem.Text = "Move first";
        // 
        // bindingNavigatorMovePreviousItem
        // 
        this.bindingNavigatorMovePreviousItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
        this.bindingNavigatorMovePreviousItem.Image = ((System.Drawing.Image)(resources.GetObject("bindingNavigatorMovePreviousItem.Image")));
        this.bindingNavigatorMovePreviousItem.Name = "bindingNavigatorMovePreviousItem";
        this.bindingNavigatorMovePreviousItem.RightToLeftAutoMirrorImage = true;
        this.bindingNavigatorMovePreviousItem.Size = new System.Drawing.Size(23, 22);
        this.bindingNavigatorMovePreviousItem.Text = "Move previous";
        // 
        // bindingNavigatorSeparator
        // 
        this.bindingNavigatorSeparator.Name = "bindingNavigatorSeparator";
        this.bindingNavigatorSeparator.Size = new System.Drawing.Size(6, 25);
        // 
        // bindingNavigatorPositionItem
        // 
        this.bindingNavigatorPositionItem.AccessibleName = "Position";
        this.bindingNavigatorPositionItem.AutoSize = false;
        this.bindingNavigatorPositionItem.Name = "bindingNavigatorPositionItem";
        this.bindingNavigatorPositionItem.Size = new System.Drawing.Size(50, 21);
        this.bindingNavigatorPositionItem.Text = "0";
        this.bindingNavigatorPositionItem.ToolTipText = "Current position";
        // 
        // bindingNavigatorSeparator1
        // 
        this.bindingNavigatorSeparator1.Name = "bindingNavigatorSeparator1";
        this.bindingNavigatorSeparator1.Size = new System.Drawing.Size(6, 25);
        // 
        // bindingNavigatorMoveNextItem
        // 
        this.bindingNavigatorMoveNextItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
        this.bindingNavigatorMoveNextItem.Image = ((System.Drawing.Image)(resources.GetObject("bindingNavigatorMoveNextItem.Image")));
        this.bindingNavigatorMoveNextItem.Name = "bindingNavigatorMoveNextItem";
        this.bindingNavigatorMoveNextItem.RightToLeftAutoMirrorImage = true;
        this.bindingNavigatorMoveNextItem.Size = new System.Drawing.Size(23, 22);
        this.bindingNavigatorMoveNextItem.Text = "Move next";
        // 
        // bindingNavigatorMoveLastItem
        // 
        this.bindingNavigatorMoveLastItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
        this.bindingNavigatorMoveLastItem.Image = ((System.Drawing.Image)(resources.GetObject("bindingNavigatorMoveLastItem.Image")));
        this.bindingNavigatorMoveLastItem.Name = "bindingNavigatorMoveLastItem";
        this.bindingNavigatorMoveLastItem.RightToLeftAutoMirrorImage = true;
        this.bindingNavigatorMoveLastItem.Size = new System.Drawing.Size(23, 22);
        this.bindingNavigatorMoveLastItem.Text = "Move last";
        // 
        // bindingNavigatorSeparator2
        // 
        this.bindingNavigatorSeparator2.Name = "bindingNavigatorSeparator2";
        this.bindingNavigatorSeparator2.Size = new System.Drawing.Size(6, 25);
        // 
        // departmentBindingNavigatorSaveItem
        // 
        this.departmentBindingNavigatorSaveItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
        this.departmentBindingNavigatorSaveItem.Image = ((System.Drawing.Image)(resources.GetObject("departmentBindingNavigatorSaveItem.Image")));
        this.departmentBindingNavigatorSaveItem.Name = "departmentBindingNavigatorSaveItem";
        this.departmentBindingNavigatorSaveItem.Size = new System.Drawing.Size(23, 22);
        this.departmentBindingNavigatorSaveItem.Text = "Use TableAdapterManager to Update the Related Tables";
        this.departmentBindingNavigatorSaveItem.ToolTipText = "Use TableAdapterManager to Update the Related Tables";
        this.departmentBindingNavigatorSaveItem.Click += new System.EventHandler(this.departmentBindingNavigatorSaveItem_Click);
        // 
        // departmentBindingNavigatorSaveItem2
        // 
        this.departmentBindingNavigatorSaveItem2.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
        this.departmentBindingNavigatorSaveItem2.Image = ((System.Drawing.Image)(resources.GetObject("departmentBindingNavigatorSaveItem2.Image")));
        this.departmentBindingNavigatorSaveItem2.Name = "departmentBindingNavigatorSaveItem2";
        this.departmentBindingNavigatorSaveItem2.Size = new System.Drawing.Size(23, 22);
        this.departmentBindingNavigatorSaveItem2.Text = "Writing Codes Manually to Update Related Tables";
        this.departmentBindingNavigatorSaveItem2.ToolTipText = "Writing Codes Manually to Update Related Tables";
        this.departmentBindingNavigatorSaveItem2.Click += new System.EventHandler(this.departmentBindingNavigatorSaveItem2_Click);
        // 
        // departmentIDTextBox
        // 
        this.departmentIDTextBox.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.departmentBindingSource, "DepartmentID", true));
        this.departmentIDTextBox.Location = new System.Drawing.Point(215, 52);
        this.departmentIDTextBox.Name = "departmentIDTextBox";
        this.departmentIDTextBox.Size = new System.Drawing.Size(200, 20);
        this.departmentIDTextBox.TabIndex = 2;
        // 
        // nameTextBox
        // 
        this.nameTextBox.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.departmentBindingSource, "Name", true));
        this.nameTextBox.Location = new System.Drawing.Point(215, 78);
        this.nameTextBox.Name = "nameTextBox";
        this.nameTextBox.Size = new System.Drawing.Size(200, 20);
        this.nameTextBox.TabIndex = 4;
        // 
        // budgetTextBox
        // 
        this.budgetTextBox.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.departmentBindingSource, "Budget", true));
        this.budgetTextBox.Location = new System.Drawing.Point(215, 104);
        this.budgetTextBox.Name = "budgetTextBox";
        this.budgetTextBox.Size = new System.Drawing.Size(200, 20);
        this.budgetTextBox.TabIndex = 6;
        // 
        // startDateDateTimePicker
        // 
        this.startDateDateTimePicker.DataBindings.Add(new System.Windows.Forms.Binding("Value", this.departmentBindingSource, "StartDate", true));
        this.startDateDateTimePicker.Location = new System.Drawing.Point(215, 130);
        this.startDateDateTimePicker.Name = "startDateDateTimePicker";
        this.startDateDateTimePicker.Size = new System.Drawing.Size(200, 20);
        this.startDateDateTimePicker.TabIndex = 8;
        // 
        // administratorTextBox
        // 
        this.administratorTextBox.DataBindings.Add(new System.Windows.Forms.Binding("Text", this.departmentBindingSource, "Administrator", true));
        this.administratorTextBox.Location = new System.Drawing.Point(215, 156);
        this.administratorTextBox.Name = "administratorTextBox";
        this.administratorTextBox.Size = new System.Drawing.Size(200, 20);
        this.administratorTextBox.TabIndex = 10;
        // 
        // courseBindingSource
        // 
        this.courseBindingSource.DataMember = "Course";
        this.courseBindingSource.DataSource = this.sQLServer2005DBDataSet;
        this.courseBindingSource.AddingNew += new System.ComponentModel.AddingNewEventHandler(this.courseBindingSource_AddingNew);
        // 
        // courseDataGridView
        // 
        this.courseDataGridView.AutoGenerateColumns = false;
        this.courseDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
        this.courseDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn1,
            this.dataGridViewTextBoxColumn2,
            this.dataGridViewTextBoxColumn3,
            this.dataGridViewTextBoxColumn4});
        this.courseDataGridView.DataSource = this.courseBindingSource;
        this.courseDataGridView.Location = new System.Drawing.Point(66, 220);
        this.courseDataGridView.Name = "courseDataGridView";
        this.courseDataGridView.Size = new System.Drawing.Size(442, 220);
        this.courseDataGridView.TabIndex = 11;
        // 
        // dataGridViewTextBoxColumn1
        // 
        this.dataGridViewTextBoxColumn1.DataPropertyName = "CourseID";
        this.dataGridViewTextBoxColumn1.HeaderText = "CourseID";
        this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
        // 
        // dataGridViewTextBoxColumn2
        // 
        this.dataGridViewTextBoxColumn2.DataPropertyName = "Title";
        this.dataGridViewTextBoxColumn2.HeaderText = "Title";
        this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
        // 
        // dataGridViewTextBoxColumn3
        // 
        this.dataGridViewTextBoxColumn3.DataPropertyName = "Credits";
        this.dataGridViewTextBoxColumn3.HeaderText = "Credits";
        this.dataGridViewTextBoxColumn3.Name = "dataGridViewTextBoxColumn3";
        // 
        // dataGridViewTextBoxColumn4
        // 
        this.dataGridViewTextBoxColumn4.DataPropertyName = "DepartmentID";
        this.dataGridViewTextBoxColumn4.HeaderText = "DepartmentID";
        this.dataGridViewTextBoxColumn4.Name = "dataGridViewTextBoxColumn4";
        // 
        // RelatedTableUpdateForm
        // 
        this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
        this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
        this.ClientSize = new System.Drawing.Size(607, 500);
        this.Controls.Add(this.courseDataGridView);
        this.Controls.Add(departmentIDLabel);
        this.Controls.Add(this.departmentIDTextBox);
        this.Controls.Add(nameLabel);
        this.Controls.Add(this.nameTextBox);
        this.Controls.Add(budgetLabel);
        this.Controls.Add(this.budgetTextBox);
        this.Controls.Add(startDateLabel);
        this.Controls.Add(this.startDateDateTimePicker);
        this.Controls.Add(administratorLabel);
        this.Controls.Add(this.administratorTextBox);
        this.Controls.Add(this.departmentBindingNavigator);
        this.Name = "RelatedTableUpdateForm";
        this.Text = "Form1";
        this.Load += new System.EventHandler(this.RelatedTableUpdateForm_Load);
        ((System.ComponentModel.ISupportInitialize)(this.sQLServer2005DBDataSet)).EndInit();
        ((System.ComponentModel.ISupportInitialize)(this.departmentBindingSource)).EndInit();
        ((System.ComponentModel.ISupportInitialize)(this.departmentBindingNavigator)).EndInit();
        this.departmentBindingNavigator.ResumeLayout(false);
        this.departmentBindingNavigator.PerformLayout();
        ((System.ComponentModel.ISupportInitialize)(this.courseBindingSource)).EndInit();
        ((System.ComponentModel.ISupportInitialize)(this.courseDataGridView)).EndInit();
        this.ResumeLayout(false);
        this.PerformLayout();

    }

    #endregion

    private SQLServer2005DBDataSet sQLServer2005DBDataSet;
    private System.Windows.Forms.BindingSource departmentBindingSource;
    private CSUseADONET.SQLServer2005DBDataSetTableAdapters.DepartmentTableAdapter departmentTableAdapter;
    private CSUseADONET.SQLServer2005DBDataSetTableAdapters.TableAdapterManager tableAdapterManager;
    private System.Windows.Forms.BindingNavigator departmentBindingNavigator;
    private System.Windows.Forms.ToolStripButton bindingNavigatorAddNewItem;
    private System.Windows.Forms.ToolStripLabel bindingNavigatorCountItem;
    private System.Windows.Forms.ToolStripButton bindingNavigatorDeleteItem;
    private System.Windows.Forms.ToolStripButton bindingNavigatorMoveFirstItem;
    private System.Windows.Forms.ToolStripButton bindingNavigatorMovePreviousItem;
    private System.Windows.Forms.ToolStripSeparator bindingNavigatorSeparator;
    private System.Windows.Forms.ToolStripTextBox bindingNavigatorPositionItem;
    private System.Windows.Forms.ToolStripSeparator bindingNavigatorSeparator1;
    private System.Windows.Forms.ToolStripButton bindingNavigatorMoveNextItem;
    private System.Windows.Forms.ToolStripButton bindingNavigatorMoveLastItem;
    private System.Windows.Forms.ToolStripSeparator bindingNavigatorSeparator2;
    private System.Windows.Forms.ToolStripButton departmentBindingNavigatorSaveItem;
    private CSUseADONET.SQLServer2005DBDataSetTableAdapters.CourseTableAdapter courseTableAdapter;
    private System.Windows.Forms.TextBox departmentIDTextBox;
    private System.Windows.Forms.TextBox nameTextBox;
    private System.Windows.Forms.TextBox budgetTextBox;
    private System.Windows.Forms.DateTimePicker startDateDateTimePicker;
    private System.Windows.Forms.TextBox administratorTextBox;
    private System.Windows.Forms.BindingSource courseBindingSource;
    private System.Windows.Forms.ToolStripButton departmentBindingNavigatorSaveItem2;
    private System.Windows.Forms.DataGridView courseDataGridView;
    private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
    private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
    private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
    private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn4;
}

