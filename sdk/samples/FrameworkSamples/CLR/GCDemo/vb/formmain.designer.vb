Partial Public Class formMain
    Inherits System.Windows.Forms.Form

    <System.Diagnostics.DebuggerNonUserCode()> _
    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

    End Sub

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
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
        Me.labelAllocation = New System.Windows.Forms.Label
        Me.treeMain = New System.Windows.Forms.TreeView
        Me.groupPressure = New System.Windows.Forms.GroupBox
        Me.radioPressureYes = New System.Windows.Forms.RadioButton
        Me.radioPressureNo = New System.Windows.Forms.RadioButton
        Me.buttonClear = New System.Windows.Forms.Button
        Me.buttonRun = New System.Windows.Forms.Button
        Me.buttonExit = New System.Windows.Forms.Button
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.labelAllocations = New System.Windows.Forms.Label
        Me.labelAmount = New System.Windows.Forms.Label
        Me.numericAllocations = New System.Windows.Forms.NumericUpDown
        Me.comboAmount = New System.Windows.Forms.ComboBox
        Me.groupCollections = New System.Windows.Forms.GroupBox
        Me.labelTotalCount = New System.Windows.Forms.Label
        Me.labelCurCount = New System.Windows.Forms.Label
        Me.labelTotCount = New System.Windows.Forms.Label
        Me.labelCount = New System.Windows.Forms.Label
        Me.labelDesc = New System.Windows.Forms.Label
        Me.groupPressure.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        CType(Me.numericAllocations, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.groupCollections.SuspendLayout()
        Me.SuspendLayout()
        '
        'labelAllocation
        '
        Me.labelAllocation.AutoSize = True
        Me.labelAllocation.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.labelAllocation.Location = New System.Drawing.Point(12, 12)
        Me.labelAllocation.Name = "labelAllocation"
        Me.labelAllocation.Size = New System.Drawing.Size(86, 13)
        Me.labelAllocation.TabIndex = 0
        Me.labelAllocation.Text = "labelAllocation"
        '
        'treeMain
        '
        Me.treeMain.Location = New System.Drawing.Point(12, 30)
        Me.treeMain.Name = "treeMain"
        Me.treeMain.Size = New System.Drawing.Size(306, 362)
        Me.treeMain.TabIndex = 1
        '
        'groupPressure
        '
        Me.groupPressure.Controls.Add(Me.radioPressureYes)
        Me.groupPressure.Controls.Add(Me.radioPressureNo)
        Me.groupPressure.Location = New System.Drawing.Point(344, 30)
        Me.groupPressure.Name = "groupPressure"
        Me.groupPressure.Size = New System.Drawing.Size(114, 48)
        Me.groupPressure.TabIndex = 2
        Me.groupPressure.TabStop = False
        Me.groupPressure.Text = "Add Pressure"
        '
        'radioPressureYes
        '
        Me.radioPressureYes.AutoSize = True
        Me.radioPressureYes.Location = New System.Drawing.Point(58, 19)
        Me.radioPressureYes.Name = "radioPressureYes"
        Me.radioPressureYes.Size = New System.Drawing.Size(39, 17)
        Me.radioPressureYes.TabIndex = 1
        Me.radioPressureYes.TabStop = False
        Me.radioPressureYes.Text = "Yes"
        '
        'radioPressureNo
        '
        Me.radioPressureNo.AutoSize = True
        Me.radioPressureNo.Checked = True
        Me.radioPressureNo.Location = New System.Drawing.Point(17, 19)
        Me.radioPressureNo.Name = "radioPressureNo"
        Me.radioPressureNo.Size = New System.Drawing.Size(35, 17)
        Me.radioPressureNo.TabIndex = 0
        Me.radioPressureNo.Text = "No"
        '
        'buttonClear
        '
        Me.buttonClear.Location = New System.Drawing.Point(324, 340)
        Me.buttonClear.Name = "buttonClear"
        Me.buttonClear.Size = New System.Drawing.Size(75, 23)
        Me.buttonClear.TabIndex = 3
        Me.buttonClear.Text = "Clear"
        '
        'buttonRun
        '
        Me.buttonRun.Location = New System.Drawing.Point(324, 369)
        Me.buttonRun.Name = "buttonRun"
        Me.buttonRun.Size = New System.Drawing.Size(75, 23)
        Me.buttonRun.TabIndex = 4
        Me.buttonRun.Text = "Run"
        '
        'buttonExit
        '
        Me.buttonExit.Location = New System.Drawing.Point(405, 369)
        Me.buttonExit.Name = "buttonExit"
        Me.buttonExit.Size = New System.Drawing.Size(75, 23)
        Me.buttonExit.TabIndex = 5
        Me.buttonExit.Text = "E&xit"
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.labelAllocations)
        Me.GroupBox1.Controls.Add(Me.labelAmount)
        Me.GroupBox1.Controls.Add(Me.numericAllocations)
        Me.GroupBox1.Controls.Add(Me.comboAmount)
        Me.GroupBox1.Location = New System.Drawing.Point(324, 84)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(156, 76)
        Me.GroupBox1.TabIndex = 6
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "groupVariables"
        '
        'labelAllocations
        '
        Me.labelAllocations.AutoSize = True
        Me.labelAllocations.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.labelAllocations.Location = New System.Drawing.Point(9, 48)
        Me.labelAllocations.Name = "labelAllocations"
        Me.labelAllocations.Size = New System.Drawing.Size(65, 13)
        Me.labelAllocations.TabIndex = 4
        Me.labelAllocations.Text = "Allocations"
        '
        'labelAmount
        '
        Me.labelAmount.AutoSize = True
        Me.labelAmount.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.labelAmount.Location = New System.Drawing.Point(9, 22)
        Me.labelAmount.Name = "labelAmount"
        Me.labelAmount.Size = New System.Drawing.Size(45, 13)
        Me.labelAmount.TabIndex = 3
        Me.labelAmount.Text = "Amount"
        '
        'numericAllocations
        '
        Me.numericAllocations.Location = New System.Drawing.Point(82, 46)
        Me.numericAllocations.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.numericAllocations.Name = "numericAllocations"
        Me.numericAllocations.Size = New System.Drawing.Size(59, 20)
        Me.numericAllocations.TabIndex = 1
        Me.numericAllocations.Value = New Decimal(New Integer() {9, 0, 0, 0})
        '
        'comboAmount
        '
        Me.comboAmount.FormattingEnabled = True
        Me.comboAmount.Items.AddRange(New Object() {"1000", "5000", "10000", "100000", "100000000"})
        Me.comboAmount.Location = New System.Drawing.Point(56, 19)
        Me.comboAmount.Name = "comboAmount"
        Me.comboAmount.Size = New System.Drawing.Size(93, 21)
        Me.comboAmount.TabIndex = 0
        '
        'groupCollections
        '
        Me.groupCollections.Controls.Add(Me.labelTotalCount)
        Me.groupCollections.Controls.Add(Me.labelCurCount)
        Me.groupCollections.Controls.Add(Me.labelTotCount)
        Me.groupCollections.Controls.Add(Me.labelCount)
        Me.groupCollections.Location = New System.Drawing.Point(325, 167)
        Me.groupCollections.Name = "groupCollections"
        Me.groupCollections.Size = New System.Drawing.Size(155, 59)
        Me.groupCollections.TabIndex = 7
        Me.groupCollections.TabStop = False
        Me.groupCollections.Text = "Collection Count"
        '
        'labelTotalCount
        '
        Me.labelTotalCount.AutoSize = True
        Me.labelTotalCount.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.labelTotalCount.Location = New System.Drawing.Point(8, 38)
        Me.labelTotalCount.Name = "labelTotalCount"
        Me.labelTotalCount.Size = New System.Drawing.Size(32, 13)
        Me.labelTotalCount.TabIndex = 3
        Me.labelTotalCount.Text = "Total"
        '
        'labelCurCount
        '
        Me.labelCurCount.AutoSize = True
        Me.labelCurCount.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.labelCurCount.Location = New System.Drawing.Point(8, 17)
        Me.labelCurCount.Name = "labelCurCount"
        Me.labelCurCount.Size = New System.Drawing.Size(44, 13)
        Me.labelCurCount.TabIndex = 2
        Me.labelCurCount.Text = "Current"
        '
        'labelTotCount
        '
        Me.labelTotCount.AutoSize = True
        Me.labelTotCount.Location = New System.Drawing.Point(96, 38)
        Me.labelTotCount.Name = "labelTotCount"
        Me.labelTotCount.Size = New System.Drawing.Size(9, 13)
        Me.labelTotCount.TabIndex = 1
        Me.labelTotCount.Text = "0"
        '
        'labelCount
        '
        Me.labelCount.AutoSize = True
        Me.labelCount.Location = New System.Drawing.Point(96, 17)
        Me.labelCount.Name = "labelCount"
        Me.labelCount.Size = New System.Drawing.Size(9, 13)
        Me.labelCount.TabIndex = 0
        Me.labelCount.Text = "0"
        '
        'labelDesc
        '
        Me.labelDesc.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.labelDesc.Location = New System.Drawing.Point(324, 232)
        Me.labelDesc.Name = "labelDesc"
        Me.labelDesc.Size = New System.Drawing.Size(156, 102)
        Me.labelDesc.TabIndex = 8
        Me.labelDesc.Text = "labelDesc"
        Me.labelDesc.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'formMain
        '
        Me.ClientSize = New System.Drawing.Size(487, 402)
        Me.Controls.Add(Me.labelDesc)
        Me.Controls.Add(Me.groupCollections)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.buttonExit)
        Me.Controls.Add(Me.buttonRun)
        Me.Controls.Add(Me.buttonClear)
        Me.Controls.Add(Me.groupPressure)
        Me.Controls.Add(Me.treeMain)
        Me.Controls.Add(Me.labelAllocation)
        Me.Name = "formMain"
        Me.Text = "GCDemo"
        Me.groupPressure.ResumeLayout(False)
        Me.groupPressure.PerformLayout()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        CType(Me.numericAllocations, System.ComponentModel.ISupportInitialize).EndInit()
        Me.groupCollections.ResumeLayout(False)
        Me.groupCollections.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents labelAllocation As System.Windows.Forms.Label
    Friend WithEvents treeMain As System.Windows.Forms.TreeView
    Friend WithEvents groupPressure As System.Windows.Forms.GroupBox
    Friend WithEvents radioPressureYes As System.Windows.Forms.RadioButton
    Friend WithEvents radioPressureNo As System.Windows.Forms.RadioButton
    Friend WithEvents buttonClear As System.Windows.Forms.Button
    Friend WithEvents buttonRun As System.Windows.Forms.Button
    Friend WithEvents buttonExit As System.Windows.Forms.Button
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents numericAllocations As System.Windows.Forms.NumericUpDown
    Friend WithEvents comboAmount As System.Windows.Forms.ComboBox
    Friend WithEvents groupCollections As System.Windows.Forms.GroupBox
    Friend WithEvents labelCurCount As System.Windows.Forms.Label
    Friend WithEvents labelTotCount As System.Windows.Forms.Label
    Friend WithEvents labelCount As System.Windows.Forms.Label
    Friend WithEvents labelTotalCount As System.Windows.Forms.Label
    Friend WithEvents labelDesc As System.Windows.Forms.Label
    Friend WithEvents labelAllocations As System.Windows.Forms.Label
    Friend WithEvents labelAmount As System.Windows.Forms.Label

End Class
