<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class RelatedTableUpdateForm
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Dim departmentIDLabel As System.Windows.Forms.Label
        Dim nameLabel As System.Windows.Forms.Label
        Dim budgetLabel As System.Windows.Forms.Label
        Dim startDateLabel As System.Windows.Forms.Label
        Dim administratorLabel As System.Windows.Forms.Label
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(RelatedTableUpdateForm))
        Me.departmentIDTextBox = New System.Windows.Forms.TextBox
        Me.nameTextBox = New System.Windows.Forms.TextBox
        Me.budgetTextBox = New System.Windows.Forms.TextBox
        Me.startDateDateTimePicker = New System.Windows.Forms.DateTimePicker
        Me.administratorTextBox = New System.Windows.Forms.TextBox
        Me.SQLServer2005DBDataSet = New VBUseADONET.SQLServer2005DBDataSet
        Me.DepartmentBindingSource = New System.Windows.Forms.BindingSource(Me.components)
        Me.DepartmentTableAdapter = New VBUseADONET.SQLServer2005DBDataSetTableAdapters.DepartmentTableAdapter
        Me.TableAdapterManager = New VBUseADONET.SQLServer2005DBDataSetTableAdapters.TableAdapterManager
        Me.CourseTableAdapter = New VBUseADONET.SQLServer2005DBDataSetTableAdapters.CourseTableAdapter
        Me.DepartmentBindingNavigator = New System.Windows.Forms.BindingNavigator(Me.components)
        Me.BindingNavigatorAddNewItem = New System.Windows.Forms.ToolStripButton
        Me.BindingNavigatorCountItem = New System.Windows.Forms.ToolStripLabel
        Me.BindingNavigatorDeleteItem = New System.Windows.Forms.ToolStripButton
        Me.BindingNavigatorMoveFirstItem = New System.Windows.Forms.ToolStripButton
        Me.BindingNavigatorMovePreviousItem = New System.Windows.Forms.ToolStripButton
        Me.BindingNavigatorSeparator = New System.Windows.Forms.ToolStripSeparator
        Me.BindingNavigatorPositionItem = New System.Windows.Forms.ToolStripTextBox
        Me.BindingNavigatorSeparator1 = New System.Windows.Forms.ToolStripSeparator
        Me.BindingNavigatorMoveNextItem = New System.Windows.Forms.ToolStripButton
        Me.BindingNavigatorMoveLastItem = New System.Windows.Forms.ToolStripButton
        Me.BindingNavigatorSeparator2 = New System.Windows.Forms.ToolStripSeparator
        Me.DepartmentBindingNavigatorSaveItem = New System.Windows.Forms.ToolStripButton
        Me.CourseBindingSource = New System.Windows.Forms.BindingSource(Me.components)
        Me.CourseDataGridView = New System.Windows.Forms.DataGridView
        Me.DataGridViewTextBoxColumn1 = New System.Windows.Forms.DataGridViewTextBoxColumn
        Me.DataGridViewTextBoxColumn2 = New System.Windows.Forms.DataGridViewTextBoxColumn
        Me.DataGridViewTextBoxColumn3 = New System.Windows.Forms.DataGridViewTextBoxColumn
        Me.DataGridViewTextBoxColumn4 = New System.Windows.Forms.DataGridViewTextBoxColumn
        Me.DepartmentBindingNavigatorSaveItem2 = New System.Windows.Forms.ToolStripButton
        departmentIDLabel = New System.Windows.Forms.Label
        nameLabel = New System.Windows.Forms.Label
        budgetLabel = New System.Windows.Forms.Label
        startDateLabel = New System.Windows.Forms.Label
        administratorLabel = New System.Windows.Forms.Label
        CType(Me.SQLServer2005DBDataSet, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.DepartmentBindingSource, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.DepartmentBindingNavigator, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.DepartmentBindingNavigator.SuspendLayout()
        CType(Me.CourseBindingSource, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.CourseDataGridView, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'departmentIDLabel
        '
        departmentIDLabel.AutoSize = True
        departmentIDLabel.Location = New System.Drawing.Point(130, 51)
        departmentIDLabel.Name = "departmentIDLabel"
        departmentIDLabel.Size = New System.Drawing.Size(89, 12)
        departmentIDLabel.TabIndex = 13
        departmentIDLabel.Text = "Department ID:"
        '
        'nameLabel
        '
        nameLabel.AutoSize = True
        nameLabel.Location = New System.Drawing.Point(130, 75)
        nameLabel.Name = "nameLabel"
        nameLabel.Size = New System.Drawing.Size(35, 12)
        nameLabel.TabIndex = 15
        nameLabel.Text = "Name:"
        '
        'budgetLabel
        '
        budgetLabel.AutoSize = True
        budgetLabel.Location = New System.Drawing.Point(130, 99)
        budgetLabel.Name = "budgetLabel"
        budgetLabel.Size = New System.Drawing.Size(47, 12)
        budgetLabel.TabIndex = 17
        budgetLabel.Text = "Budget:"
        '
        'startDateLabel
        '
        startDateLabel.AutoSize = True
        startDateLabel.Location = New System.Drawing.Point(130, 124)
        startDateLabel.Name = "startDateLabel"
        startDateLabel.Size = New System.Drawing.Size(71, 12)
        startDateLabel.TabIndex = 19
        startDateLabel.Text = "Start Date:"
        '
        'administratorLabel
        '
        administratorLabel.AutoSize = True
        administratorLabel.Location = New System.Drawing.Point(130, 147)
        administratorLabel.Name = "administratorLabel"
        administratorLabel.Size = New System.Drawing.Size(89, 12)
        administratorLabel.TabIndex = 21
        administratorLabel.Text = "Administrator:"
        '
        'departmentIDTextBox
        '
        Me.departmentIDTextBox.Location = New System.Drawing.Point(215, 48)
        Me.departmentIDTextBox.Name = "departmentIDTextBox"
        Me.departmentIDTextBox.Size = New System.Drawing.Size(200, 21)
        Me.departmentIDTextBox.TabIndex = 14
        '
        'nameTextBox
        '
        Me.nameTextBox.Location = New System.Drawing.Point(215, 72)
        Me.nameTextBox.Name = "nameTextBox"
        Me.nameTextBox.Size = New System.Drawing.Size(200, 21)
        Me.nameTextBox.TabIndex = 16
        '
        'budgetTextBox
        '
        Me.budgetTextBox.Location = New System.Drawing.Point(215, 96)
        Me.budgetTextBox.Name = "budgetTextBox"
        Me.budgetTextBox.Size = New System.Drawing.Size(200, 21)
        Me.budgetTextBox.TabIndex = 18
        '
        'startDateDateTimePicker
        '
        Me.startDateDateTimePicker.Location = New System.Drawing.Point(215, 120)
        Me.startDateDateTimePicker.Name = "startDateDateTimePicker"
        Me.startDateDateTimePicker.Size = New System.Drawing.Size(200, 21)
        Me.startDateDateTimePicker.TabIndex = 20
        '
        'administratorTextBox
        '
        Me.administratorTextBox.Location = New System.Drawing.Point(215, 144)
        Me.administratorTextBox.Name = "administratorTextBox"
        Me.administratorTextBox.Size = New System.Drawing.Size(200, 21)
        Me.administratorTextBox.TabIndex = 22
        '
        'SQLServer2005DBDataSet
        '
        Me.SQLServer2005DBDataSet.DataSetName = "SQLServer2005DBDataSet"
        Me.SQLServer2005DBDataSet.SchemaSerializationMode = System.Data.SchemaSerializationMode.IncludeSchema
        '
        'DepartmentBindingSource
        '
        Me.DepartmentBindingSource.DataMember = "Department"
        Me.DepartmentBindingSource.DataSource = Me.SQLServer2005DBDataSet
        '
        'DepartmentTableAdapter
        '
        Me.DepartmentTableAdapter.ClearBeforeFill = True
        '
        'TableAdapterManager
        '
        Me.TableAdapterManager.BackupDataSetBeforeUpdate = False
        Me.TableAdapterManager.CourseGradeTableAdapter = Nothing
        Me.TableAdapterManager.CourseInstructorTableAdapter = Nothing
        Me.TableAdapterManager.CourseTableAdapter = Me.CourseTableAdapter
        Me.TableAdapterManager.DepartmentTableAdapter = Me.DepartmentTableAdapter
        Me.TableAdapterManager.OfficeAssignmentTableAdapter = Nothing
        Me.TableAdapterManager.OnlineCourseTableAdapter = Nothing
        Me.TableAdapterManager.OnsiteCourseTableAdapter = Nothing
        Me.TableAdapterManager.PersonTableAdapter = Nothing
        Me.TableAdapterManager.UpdateOrder = VBUseADONET.SQLServer2005DBDataSetTableAdapters.TableAdapterManager.UpdateOrderOption.InsertUpdateDelete
        '
        'CourseTableAdapter
        '
        Me.CourseTableAdapter.ClearBeforeFill = True
        '
        'DepartmentBindingNavigator
        '
        Me.DepartmentBindingNavigator.AddNewItem = Me.BindingNavigatorAddNewItem
        Me.DepartmentBindingNavigator.BindingSource = Me.DepartmentBindingSource
        Me.DepartmentBindingNavigator.CountItem = Me.BindingNavigatorCountItem
        Me.DepartmentBindingNavigator.DeleteItem = Me.BindingNavigatorDeleteItem
        Me.DepartmentBindingNavigator.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.BindingNavigatorMoveFirstItem, Me.BindingNavigatorMovePreviousItem, Me.BindingNavigatorSeparator, Me.BindingNavigatorPositionItem, Me.BindingNavigatorCountItem, Me.BindingNavigatorSeparator1, Me.BindingNavigatorMoveNextItem, Me.BindingNavigatorMoveLastItem, Me.BindingNavigatorSeparator2, Me.BindingNavigatorAddNewItem, Me.BindingNavigatorDeleteItem, Me.DepartmentBindingNavigatorSaveItem, Me.DepartmentBindingNavigatorSaveItem2})
        Me.DepartmentBindingNavigator.Location = New System.Drawing.Point(0, 0)
        Me.DepartmentBindingNavigator.MoveFirstItem = Me.BindingNavigatorMoveFirstItem
        Me.DepartmentBindingNavigator.MoveLastItem = Me.BindingNavigatorMoveLastItem
        Me.DepartmentBindingNavigator.MoveNextItem = Me.BindingNavigatorMoveNextItem
        Me.DepartmentBindingNavigator.MovePreviousItem = Me.BindingNavigatorMovePreviousItem
        Me.DepartmentBindingNavigator.Name = "DepartmentBindingNavigator"
        Me.DepartmentBindingNavigator.PositionItem = Me.BindingNavigatorPositionItem
        Me.DepartmentBindingNavigator.Size = New System.Drawing.Size(607, 25)
        Me.DepartmentBindingNavigator.TabIndex = 23
        Me.DepartmentBindingNavigator.Text = "BindingNavigator1"
        '
        'BindingNavigatorAddNewItem
        '
        Me.BindingNavigatorAddNewItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.BindingNavigatorAddNewItem.Image = CType(resources.GetObject("BindingNavigatorAddNewItem.Image"), System.Drawing.Image)
        Me.BindingNavigatorAddNewItem.Name = "BindingNavigatorAddNewItem"
        Me.BindingNavigatorAddNewItem.RightToLeftAutoMirrorImage = True
        Me.BindingNavigatorAddNewItem.Size = New System.Drawing.Size(23, 22)
        Me.BindingNavigatorAddNewItem.Text = "Add new"
        '
        'BindingNavigatorCountItem
        '
        Me.BindingNavigatorCountItem.Name = "BindingNavigatorCountItem"
        Me.BindingNavigatorCountItem.Size = New System.Drawing.Size(39, 22)
        Me.BindingNavigatorCountItem.Text = "of {0}"
        Me.BindingNavigatorCountItem.ToolTipText = "Total number of items"
        '
        'BindingNavigatorDeleteItem
        '
        Me.BindingNavigatorDeleteItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.BindingNavigatorDeleteItem.Image = CType(resources.GetObject("BindingNavigatorDeleteItem.Image"), System.Drawing.Image)
        Me.BindingNavigatorDeleteItem.Name = "BindingNavigatorDeleteItem"
        Me.BindingNavigatorDeleteItem.RightToLeftAutoMirrorImage = True
        Me.BindingNavigatorDeleteItem.Size = New System.Drawing.Size(23, 22)
        Me.BindingNavigatorDeleteItem.Text = "Delete"
        '
        'BindingNavigatorMoveFirstItem
        '
        Me.BindingNavigatorMoveFirstItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.BindingNavigatorMoveFirstItem.Image = CType(resources.GetObject("BindingNavigatorMoveFirstItem.Image"), System.Drawing.Image)
        Me.BindingNavigatorMoveFirstItem.Name = "BindingNavigatorMoveFirstItem"
        Me.BindingNavigatorMoveFirstItem.RightToLeftAutoMirrorImage = True
        Me.BindingNavigatorMoveFirstItem.Size = New System.Drawing.Size(23, 22)
        Me.BindingNavigatorMoveFirstItem.Text = "Move first"
        '
        'BindingNavigatorMovePreviousItem
        '
        Me.BindingNavigatorMovePreviousItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.BindingNavigatorMovePreviousItem.Image = CType(resources.GetObject("BindingNavigatorMovePreviousItem.Image"), System.Drawing.Image)
        Me.BindingNavigatorMovePreviousItem.Name = "BindingNavigatorMovePreviousItem"
        Me.BindingNavigatorMovePreviousItem.RightToLeftAutoMirrorImage = True
        Me.BindingNavigatorMovePreviousItem.Size = New System.Drawing.Size(23, 22)
        Me.BindingNavigatorMovePreviousItem.Text = "Move previous"
        '
        'BindingNavigatorSeparator
        '
        Me.BindingNavigatorSeparator.Name = "BindingNavigatorSeparator"
        Me.BindingNavigatorSeparator.Size = New System.Drawing.Size(6, 25)
        '
        'BindingNavigatorPositionItem
        '
        Me.BindingNavigatorPositionItem.AccessibleName = "Position"
        Me.BindingNavigatorPositionItem.AutoSize = False
        Me.BindingNavigatorPositionItem.Name = "BindingNavigatorPositionItem"
        Me.BindingNavigatorPositionItem.Size = New System.Drawing.Size(50, 23)
        Me.BindingNavigatorPositionItem.Text = "0"
        Me.BindingNavigatorPositionItem.ToolTipText = "Current position"
        '
        'BindingNavigatorSeparator1
        '
        Me.BindingNavigatorSeparator1.Name = "BindingNavigatorSeparator1"
        Me.BindingNavigatorSeparator1.Size = New System.Drawing.Size(6, 25)
        '
        'BindingNavigatorMoveNextItem
        '
        Me.BindingNavigatorMoveNextItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.BindingNavigatorMoveNextItem.Image = CType(resources.GetObject("BindingNavigatorMoveNextItem.Image"), System.Drawing.Image)
        Me.BindingNavigatorMoveNextItem.Name = "BindingNavigatorMoveNextItem"
        Me.BindingNavigatorMoveNextItem.RightToLeftAutoMirrorImage = True
        Me.BindingNavigatorMoveNextItem.Size = New System.Drawing.Size(23, 22)
        Me.BindingNavigatorMoveNextItem.Text = "Move next"
        '
        'BindingNavigatorMoveLastItem
        '
        Me.BindingNavigatorMoveLastItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.BindingNavigatorMoveLastItem.Image = CType(resources.GetObject("BindingNavigatorMoveLastItem.Image"), System.Drawing.Image)
        Me.BindingNavigatorMoveLastItem.Name = "BindingNavigatorMoveLastItem"
        Me.BindingNavigatorMoveLastItem.RightToLeftAutoMirrorImage = True
        Me.BindingNavigatorMoveLastItem.Size = New System.Drawing.Size(23, 22)
        Me.BindingNavigatorMoveLastItem.Text = "Move last"
        '
        'BindingNavigatorSeparator2
        '
        Me.BindingNavigatorSeparator2.Name = "BindingNavigatorSeparator2"
        Me.BindingNavigatorSeparator2.Size = New System.Drawing.Size(6, 25)
        '
        'DepartmentBindingNavigatorSaveItem
        '
        Me.DepartmentBindingNavigatorSaveItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.DepartmentBindingNavigatorSaveItem.Image = CType(resources.GetObject("DepartmentBindingNavigatorSaveItem.Image"), System.Drawing.Image)
        Me.DepartmentBindingNavigatorSaveItem.Name = "DepartmentBindingNavigatorSaveItem"
        Me.DepartmentBindingNavigatorSaveItem.Size = New System.Drawing.Size(23, 22)
        Me.DepartmentBindingNavigatorSaveItem.Text = "Save Data"
        '
        'CourseBindingSource
        '
        Me.CourseBindingSource.DataMember = "Course"
        Me.CourseBindingSource.DataSource = Me.SQLServer2005DBDataSet
        '
        'CourseDataGridView
        '
        Me.CourseDataGridView.AutoGenerateColumns = False
        Me.CourseDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.CourseDataGridView.Columns.AddRange(New System.Windows.Forms.DataGridViewColumn() {Me.DataGridViewTextBoxColumn1, Me.DataGridViewTextBoxColumn2, Me.DataGridViewTextBoxColumn3, Me.DataGridViewTextBoxColumn4})
        Me.CourseDataGridView.DataSource = Me.CourseBindingSource
        Me.CourseDataGridView.Location = New System.Drawing.Point(66, 203)
        Me.CourseDataGridView.Name = "CourseDataGridView"
        Me.CourseDataGridView.RowTemplate.Height = 23
        Me.CourseDataGridView.Size = New System.Drawing.Size(442, 203)
        Me.CourseDataGridView.TabIndex = 23
        '
        'DataGridViewTextBoxColumn1
        '
        Me.DataGridViewTextBoxColumn1.DataPropertyName = "CourseID"
        Me.DataGridViewTextBoxColumn1.HeaderText = "CourseID"
        Me.DataGridViewTextBoxColumn1.Name = "DataGridViewTextBoxColumn1"
        '
        'DataGridViewTextBoxColumn2
        '
        Me.DataGridViewTextBoxColumn2.DataPropertyName = "Title"
        Me.DataGridViewTextBoxColumn2.HeaderText = "Title"
        Me.DataGridViewTextBoxColumn2.Name = "DataGridViewTextBoxColumn2"
        '
        'DataGridViewTextBoxColumn3
        '
        Me.DataGridViewTextBoxColumn3.DataPropertyName = "Credits"
        Me.DataGridViewTextBoxColumn3.HeaderText = "Credits"
        Me.DataGridViewTextBoxColumn3.Name = "DataGridViewTextBoxColumn3"
        '
        'DataGridViewTextBoxColumn4
        '
        Me.DataGridViewTextBoxColumn4.DataPropertyName = "DepartmentID"
        Me.DataGridViewTextBoxColumn4.HeaderText = "DepartmentID"
        Me.DataGridViewTextBoxColumn4.Name = "DataGridViewTextBoxColumn4"
        '
        'DepartmentBindingNavigatorSaveItem2
        '
        Me.DepartmentBindingNavigatorSaveItem2.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.DepartmentBindingNavigatorSaveItem2.Image = CType(resources.GetObject("DepartmentBindingNavigatorSaveItem2.Image"), System.Drawing.Image)
        Me.DepartmentBindingNavigatorSaveItem2.Name = "DepartmentBindingNavigatorSaveItem2"
        Me.DepartmentBindingNavigatorSaveItem2.Size = New System.Drawing.Size(23, 22)
        Me.DepartmentBindingNavigatorSaveItem2.Text = "Save Data"
        '
        'RelatedTableUpdateForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(607, 462)
        Me.Controls.Add(Me.CourseDataGridView)
        Me.Controls.Add(Me.DepartmentBindingNavigator)
        Me.Controls.Add(departmentIDLabel)
        Me.Controls.Add(nameLabel)
        Me.Controls.Add(budgetLabel)
        Me.Controls.Add(Me.departmentIDTextBox)
        Me.Controls.Add(Me.nameTextBox)
        Me.Controls.Add(Me.budgetTextBox)
        Me.Controls.Add(startDateLabel)
        Me.Controls.Add(Me.startDateDateTimePicker)
        Me.Controls.Add(administratorLabel)
        Me.Controls.Add(Me.administratorTextBox)
        Me.Name = "RelatedTableUpdateForm"
        Me.Text = "Form1"
        CType(Me.SQLServer2005DBDataSet, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.DepartmentBindingSource, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.DepartmentBindingNavigator, System.ComponentModel.ISupportInitialize).EndInit()
        Me.DepartmentBindingNavigator.ResumeLayout(False)
        Me.DepartmentBindingNavigator.PerformLayout()
        CType(Me.CourseBindingSource, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.CourseDataGridView, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents departmentIDTextBox As System.Windows.Forms.TextBox
    Private WithEvents nameTextBox As System.Windows.Forms.TextBox
    Private WithEvents budgetTextBox As System.Windows.Forms.TextBox
    Private WithEvents startDateDateTimePicker As System.Windows.Forms.DateTimePicker
    Private WithEvents administratorTextBox As System.Windows.Forms.TextBox
    Friend WithEvents SQLServer2005DBDataSet As VBUseADONET.SQLServer2005DBDataSet
    Friend WithEvents DepartmentBindingSource As System.Windows.Forms.BindingSource
    Friend WithEvents DepartmentTableAdapter As VBUseADONET.SQLServer2005DBDataSetTableAdapters.DepartmentTableAdapter
    Friend WithEvents TableAdapterManager As VBUseADONET.SQLServer2005DBDataSetTableAdapters.TableAdapterManager
    Friend WithEvents DepartmentBindingNavigator As System.Windows.Forms.BindingNavigator
    Friend WithEvents BindingNavigatorAddNewItem As System.Windows.Forms.ToolStripButton
    Friend WithEvents BindingNavigatorCountItem As System.Windows.Forms.ToolStripLabel
    Friend WithEvents BindingNavigatorDeleteItem As System.Windows.Forms.ToolStripButton
    Friend WithEvents BindingNavigatorMoveFirstItem As System.Windows.Forms.ToolStripButton
    Friend WithEvents BindingNavigatorMovePreviousItem As System.Windows.Forms.ToolStripButton
    Friend WithEvents BindingNavigatorSeparator As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents BindingNavigatorPositionItem As System.Windows.Forms.ToolStripTextBox
    Friend WithEvents BindingNavigatorSeparator1 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents BindingNavigatorMoveNextItem As System.Windows.Forms.ToolStripButton
    Friend WithEvents BindingNavigatorMoveLastItem As System.Windows.Forms.ToolStripButton
    Friend WithEvents BindingNavigatorSeparator2 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents DepartmentBindingNavigatorSaveItem As System.Windows.Forms.ToolStripButton
    Friend WithEvents CourseTableAdapter As VBUseADONET.SQLServer2005DBDataSetTableAdapters.CourseTableAdapter
    Friend WithEvents CourseBindingSource As System.Windows.Forms.BindingSource
    Friend WithEvents CourseDataGridView As System.Windows.Forms.DataGridView
    Friend WithEvents DataGridViewTextBoxColumn1 As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents DataGridViewTextBoxColumn2 As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents DataGridViewTextBoxColumn3 As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents DataGridViewTextBoxColumn4 As System.Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents DepartmentBindingNavigatorSaveItem2 As System.Windows.Forms.ToolStripButton
End Class
