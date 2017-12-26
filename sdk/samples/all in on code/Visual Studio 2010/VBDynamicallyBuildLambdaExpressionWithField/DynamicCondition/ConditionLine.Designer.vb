'INSTANT C# NOTE: Formerly VB project-level imports:
Imports System.Collections

Imports System.Reflection
Imports DynamicCondition
'using DynamicCondition.DynamicQuery.Condition;

<CompilerServices.DesignerGenerated()>
 Partial Friend Class ConditionLine
    Inherits UserControl

    'UserControl overrides dispose to clean up the component list.
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
        Me.Panel1 = New Panel()
        Me.lb = New Label()
        Me.cmbColumn = New ComboBox()
        Me.cmbOperator = New ComboBox()
        Me.pnlValue = New Panel()
        Me.tbValue = New TextBox()
        Me.chkValue = New CheckBox()
        Me.dtpValue = New DateTimePicker()
        Me.Panel1.SuspendLayout()
        Me.pnlValue.SuspendLayout()
        Me.SuspendLayout()
        ' 
        ' Panel1
        ' 
        Me.Panel1.Controls.Add(Me.lb)
        Me.Panel1.Controls.Add(Me.cmbColumn)
        Me.Panel1.Controls.Add(Me.cmbOperator)
        Me.Panel1.Location = New Point(3, 3)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New Size(242, 29)
        Me.Panel1.TabIndex = 6
        ' 
        ' lb
        ' 
        Me.lb.Font = New Font("Microsoft Sans Serif", 8.25F, FontStyle.Bold, GraphicsUnit.Point, (CByte(0)))
        Me.lb.ForeColor = Color.Blue
        Me.lb.Location = New Point(3, 3)
        Me.lb.Name = "lb"
        Me.lb.Size = New Size(40, 20)
        Me.lb.TabIndex = 6
        Me.lb.Text = "AND"
        Me.lb.TextAlign = ContentAlignment.BottomCenter
        '			Me.lb.Click += New System.EventHandler(Me.lblOperator_Click)
        ' 
        ' cmbColumn
        ' 
        Me.cmbColumn.AutoCompleteMode = AutoCompleteMode.Suggest
        Me.cmbColumn.AutoCompleteSource = AutoCompleteSource.ListItems
        Me.cmbColumn.DropDownStyle = ComboBoxStyle.DropDownList
        Me.cmbColumn.FormattingEnabled = True
        Me.cmbColumn.Location = New Point(49, 3)
        Me.cmbColumn.Name = "cmbColumn"
        Me.cmbColumn.Size = New Size(100, 21)
        Me.cmbColumn.TabIndex = 0
        '			Me.cmbColumn.SelectedIndexChanged += New System.EventHandler(Me.cboColumn_SelectedIndexChanged)
        ' 
        ' cmbOperator
        ' 
        Me.cmbOperator.DropDownStyle = ComboBoxStyle.DropDownList
        Me.cmbOperator.FormattingEnabled = True
        Me.cmbOperator.Location = New Point(155, 3)
        Me.cmbOperator.Name = "cmbOperator"
        Me.cmbOperator.Size = New Size(78, 21)
        Me.cmbOperator.TabIndex = 1
        ' 
        ' pnlValue
        ' 
        Me.pnlValue.Anchor = (CType(((AnchorStyles.Top Or AnchorStyles.Left) Or AnchorStyles.Right), AnchorStyles))
        Me.pnlValue.Controls.Add(Me.tbValue)
        Me.pnlValue.Controls.Add(Me.chkValue)
        Me.pnlValue.Controls.Add(Me.dtpValue)
        Me.pnlValue.Location = New Point(251, 3)
        Me.pnlValue.Name = "pnlValue"
        Me.pnlValue.Size = New Size(206, 29)
        Me.pnlValue.TabIndex = 7
        ' 
        ' tbValue
        ' 
        Me.tbValue.Anchor = (CType(((AnchorStyles.Top Or AnchorStyles.Left) Or AnchorStyles.Right), AnchorStyles))
        Me.tbValue.BorderStyle = BorderStyle.FixedSingle
        Me.tbValue.Location = New Point(3, 3)
        Me.tbValue.Name = "tbValue"
        Me.tbValue.Size = New Size(203, 20)
        Me.tbValue.TabIndex = 4
        ' 
        ' chkValue
        ' 
        Me.chkValue.AutoSize = True
        Me.chkValue.Location = New Point(3, 3)
        Me.chkValue.Name = "chkValue"
        Me.chkValue.Size = New Size(15, 14)
        Me.chkValue.TabIndex = 2
        Me.chkValue.UseVisualStyleBackColor = True
        ' 
        ' dtpValue
        ' 
        Me.dtpValue.Anchor = (CType(((AnchorStyles.Top Or AnchorStyles.Left) Or AnchorStyles.Right), AnchorStyles))
        Me.dtpValue.Location = New Point(3, 3)
        Me.dtpValue.Name = "dtpValue"
        Me.dtpValue.Size = New Size(203, 20)
        Me.dtpValue.TabIndex = 3
        Me.dtpValue.Visible = False
        ' 
        ' ConditionLine
        ' 
        Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = AutoScaleMode.Font
        Me.Controls.Add(Me.Panel1)
        Me.Controls.Add(Me.pnlValue)
        Me.Name = "ConditionLine"
        Me.Size = New Size(460, 35)
        '			Me.Load += New System.EventHandler(Me.ConditionLine_Load)
        Me.Panel1.ResumeLayout(False)
        Me.pnlValue.ResumeLayout(False)
        Me.pnlValue.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend Panel1 As Panel
    Friend WithEvents lb As Label
    Friend WithEvents cmbColumn As ComboBox
    Friend cmbOperator As ComboBox
    Friend pnlValue As Panel
    Friend tbValue As TextBox
    Friend chkValue As CheckBox
    Friend dtpValue As DateTimePicker

End Class

