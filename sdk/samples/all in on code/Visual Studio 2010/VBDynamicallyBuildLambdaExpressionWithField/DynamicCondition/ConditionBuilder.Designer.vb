'INSTANT C# NOTE: Formerly VB project-level imports:
Imports System.Collections

Imports DynamicCondition
Imports System.ComponentModel

<CompilerServices.DesignerGenerated()>
 Partial Public Class ConditionBuilder
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
        Me.ConditionLine1 = New DynamicCondition.ConditionLine()
        Me.SuspendLayout()
        '
        'ConditionLine1
        '
        Me.ConditionLine1.Anchor = CType(((AnchorStyles.Top Or AnchorStyles.Left) Or AnchorStyles.Right), AnchorStyles)
        Me.ConditionLine1.AutoScroll = True
        Me.ConditionLine1.DataSource = Nothing
        Me.ConditionLine1.DataType = Nothing
        Me.ConditionLine1.Location = New Point(3, 3)
        Me.ConditionLine1.Name = "ConditionLine1"
        Me.ConditionLine1.OperatorType = DynamicCondition.DynamicQuery.Condition.Compare.And
        Me.ConditionLine1.Size = New Size(456, 32)
        Me.ConditionLine1.TabIndex = 0
        '
        'ConditionBuilder
        '
        Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = AutoScaleMode.Font
        Me.AutoScroll = True
        Me.Controls.Add(Me.ConditionLine1)
        Me.Name = "ConditionBuilder"
        Me.Size = New Size(462, 37)
        Me.ResumeLayout(False)

        'INSTANT C# NOTE: Converted event handler wireups:
        '			MyBase.Load += New System.EventHandler(ConditionBuilder_Load)

    End Sub
    Friend ConditionLine1 As ConditionLine

End Class

