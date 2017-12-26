'INSTANT C# NOTE: Formerly VB project-level imports:
Imports System.Collections
Imports DynamicCondition

<CompilerServices.DesignerGenerated()>
 Partial Public Class MainForm
    Inherits Form

    'Form overrides dispose to clean up the component list.
    <DebuggerNonUserCode()>
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
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
    <DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Dim DataGridViewCellStyle1 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle2 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle3 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Me.dgResult = New System.Windows.Forms.DataGridView()
        Me.btnSearch = New System.Windows.Forms.Button()
        Me.lbTip = New System.Windows.Forms.Label()
        Me.ConditionBuilder1 = New DynamicCondition.ConditionBuilder()
        CType(Me.dgResult, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'dgResult
        '
        Me.dgResult.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        DataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control
        DataGridViewCellStyle1.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        DataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText
        DataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight
        DataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText
        DataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.[True]
        Me.dgResult.ColumnHeadersDefaultCellStyle = DataGridViewCellStyle1
        Me.dgResult.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        DataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window
        DataGridViewCellStyle2.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        DataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText
        DataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight
        DataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText
        DataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.[False]
        Me.dgResult.DefaultCellStyle = DataGridViewCellStyle2
        Me.dgResult.Location = New System.Drawing.Point(0, 152)
        Me.dgResult.Name = "dgResult"
        DataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.Control
        DataGridViewCellStyle3.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        DataGridViewCellStyle3.ForeColor = System.Drawing.SystemColors.WindowText
        DataGridViewCellStyle3.SelectionBackColor = System.Drawing.SystemColors.Highlight
        DataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.HighlightText
        DataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.[True]
        Me.dgResult.RowHeadersDefaultCellStyle = DataGridViewCellStyle3
        Me.dgResult.Size = New System.Drawing.Size(729, 600)
        Me.dgResult.TabIndex = 0
        '
        'btnSearch
        '
        Me.btnSearch.Location = New System.Drawing.Point(426, 123)
        Me.btnSearch.Name = "btnSearch"
        Me.btnSearch.Size = New System.Drawing.Size(75, 23)
        Me.btnSearch.TabIndex = 2
        Me.btnSearch.Text = "Search"
        Me.btnSearch.UseVisualStyleBackColor = True
        '
        'lbTip
        '
        Me.lbTip.AutoSize = True
        Me.lbTip.Location = New System.Drawing.Point(12, 128)
        Me.lbTip.Name = "lbTip"
        Me.lbTip.Size = New System.Drawing.Size(245, 13)
        Me.lbTip.TabIndex = 3
        Me.lbTip.Text = "Click Search to display the results in the grid below"
        '
        'ConditionBuilder1
        '
        Me.ConditionBuilder1.AutoScroll = True
        Me.ConditionBuilder1.Lines = 3
        Me.ConditionBuilder1.Location = New System.Drawing.Point(0, 12)
        Me.ConditionBuilder1.Name = "ConditionBuilder1"
        Me.ConditionBuilder1.OperatorType = DynamicCondition.ConditionBuilder.Compare.[And]
        Me.ConditionBuilder1.Size = New System.Drawing.Size(501, 105)
        Me.ConditionBuilder1.TabIndex = 1
        '
        'MainForm
        '
        Me.ClientSize = New System.Drawing.Size(729, 752)
        Me.Controls.Add(Me.lbTip)
        Me.Controls.Add(Me.btnSearch)
        Me.Controls.Add(Me.ConditionBuilder1)
        Me.Controls.Add(Me.dgResult)
        Me.Name = "MainForm"
        Me.Text = "VBDynamicallyBuildLambdaExpressionWithField"
        Me.WindowState = System.Windows.Forms.FormWindowState.Maximized
        CType(Me.dgResult, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend dgResult As DataGridView
    Friend ConditionBuilder1 As DynamicCondition.ConditionBuilder
    Friend WithEvents btnSearch As Button
    Friend lbTip As Label

End Class

