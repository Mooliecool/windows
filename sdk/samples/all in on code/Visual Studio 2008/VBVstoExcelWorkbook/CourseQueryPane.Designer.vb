<System.ComponentModel.ToolboxItemAttribute(False)> _
Partial Public Class CourseQueryPane
    Inherits System.Windows.Forms.UserControl

    <System.Diagnostics.DebuggerNonUserCode()> _
    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

    End Sub

    'UserControl overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Me.cboName = New System.Windows.Forms.ComboBox
        Me.studentListBindingSource = New System.Windows.Forms.BindingSource(Me.components)
        Me.SchoolDataSet = New VBVstoExcelWorkbook.SchoolDataSet
        Me.cmdQuery = New System.Windows.Forms.Button
        Me.lblName = New System.Windows.Forms.Label
        Me.lblTitle = New System.Windows.Forms.Label
        Me.StudentListTableAdapter = New VBVstoExcelWorkbook.SchoolDataSetTableAdapters.StudentListTableAdapter
        CType(Me.studentListBindingSource, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.SchoolDataSet, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'cboName
        '
        Me.cboName.DataSource = Me.studentListBindingSource
        Me.cboName.DisplayMember = "StudentName"
        Me.cboName.FormattingEnabled = True
        Me.cboName.Location = New System.Drawing.Point(48, 31)
        Me.cboName.Name = "cboName"
        Me.cboName.Size = New System.Drawing.Size(127, 21)
        Me.cboName.TabIndex = 8
        '
        'studentListBindingSource
        '
        Me.studentListBindingSource.DataMember = "StudentList"
        Me.studentListBindingSource.DataSource = Me.SchoolDataSet
        '
        'SchoolDataSet
        '
        Me.SchoolDataSet.DataSetName = "SchoolDataSet"
        Me.SchoolDataSet.SchemaSerializationMode = System.Data.SchemaSerializationMode.IncludeSchema
        '
        'cmdQuery
        '
        Me.cmdQuery.Location = New System.Drawing.Point(100, 57)
        Me.cmdQuery.Name = "cmdQuery"
        Me.cmdQuery.Size = New System.Drawing.Size(75, 23)
        Me.cmdQuery.TabIndex = 7
        Me.cmdQuery.Text = "Query"
        Me.cmdQuery.UseVisualStyleBackColor = True
        '
        'lblName
        '
        Me.lblName.AutoSize = True
        Me.lblName.Location = New System.Drawing.Point(4, 34)
        Me.lblName.Name = "lblName"
        Me.lblName.Size = New System.Drawing.Size(38, 13)
        Me.lblName.TabIndex = 6
        Me.lblName.Text = "Name:"
        '
        'lblTitle
        '
        Me.lblTitle.AutoSize = True
        Me.lblTitle.Font = New System.Drawing.Font("Arial Unicode MS", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblTitle.Location = New System.Drawing.Point(4, 4)
        Me.lblTitle.Name = "lblTitle"
        Me.lblTitle.Size = New System.Drawing.Size(103, 16)
        Me.lblTitle.TabIndex = 5
        Me.lblTitle.Text = "Query Courses"
        '
        'StudentListTableAdapter
        '
        Me.StudentListTableAdapter.ClearBeforeFill = True
        '
        'CourseQueryPane
        '
        Me.Controls.Add(Me.cboName)
        Me.Controls.Add(Me.cmdQuery)
        Me.Controls.Add(Me.lblName)
        Me.Controls.Add(Me.lblTitle)
        Me.Name = "CourseQueryPane"
        Me.Size = New System.Drawing.Size(193, 94)
        CType(Me.studentListBindingSource, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.SchoolDataSet, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents cboName As System.Windows.Forms.ComboBox
    Private WithEvents cmdQuery As System.Windows.Forms.Button
    Private WithEvents lblName As System.Windows.Forms.Label
    Private WithEvents lblTitle As System.Windows.Forms.Label
    Private WithEvents studentListBindingSource As System.Windows.Forms.BindingSource
    Friend WithEvents SchoolDataSet As VBVstoExcelWorkbook.SchoolDataSet
    Friend WithEvents StudentListTableAdapter As VBVstoExcelWorkbook.SchoolDataSetTableAdapters.StudentListTableAdapter


End Class
