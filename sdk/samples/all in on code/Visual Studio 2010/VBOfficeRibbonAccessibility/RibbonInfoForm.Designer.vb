<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class RibbonInfoForm
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
        Me.btnExecuteControl = New System.Windows.Forms.Button()
        Me.btnListChildControls = New System.Windows.Forms.Button()
        Me.btnListChildGroups = New System.Windows.Forms.Button()
        Me.lbControls = New System.Windows.Forms.ListBox()
        Me.lbGroups = New System.Windows.Forms.ListBox()
        Me.lbTabs = New System.Windows.Forms.ListBox()
        Me.labelForm = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.SuspendLayout()
        '
        'btnExecuteControl
        '
        Me.btnExecuteControl.Location = New System.Drawing.Point(401, 300)
        Me.btnExecuteControl.Name = "btnExecuteControl"
        Me.btnExecuteControl.Size = New System.Drawing.Size(133, 23)
        Me.btnExecuteControl.TabIndex = 14
        Me.btnExecuteControl.Text = "Execute selected control"
        Me.btnExecuteControl.UseVisualStyleBackColor = True
        '
        'btnListChildControls
        '
        Me.btnListChildControls.Location = New System.Drawing.Point(211, 300)
        Me.btnListChildControls.Name = "btnListChildControls"
        Me.btnListChildControls.Size = New System.Drawing.Size(133, 23)
        Me.btnListChildControls.TabIndex = 13
        Me.btnListChildControls.Text = "List child controls"
        Me.btnListChildControls.UseVisualStyleBackColor = True
        '
        'btnListChildGroups
        '
        Me.btnListChildGroups.Location = New System.Drawing.Point(21, 300)
        Me.btnListChildGroups.Name = "btnListChildGroups"
        Me.btnListChildGroups.Size = New System.Drawing.Size(133, 23)
        Me.btnListChildGroups.TabIndex = 12
        Me.btnListChildGroups.Text = "List child groups"
        Me.btnListChildGroups.UseVisualStyleBackColor = True
        '
        'lbControls
        '
        Me.lbControls.FormattingEnabled = True
        Me.lbControls.Location = New System.Drawing.Point(401, 77)
        Me.lbControls.Name = "lbControls"
        Me.lbControls.Size = New System.Drawing.Size(133, 212)
        Me.lbControls.TabIndex = 11
        '
        'lbGroups
        '
        Me.lbGroups.FormattingEnabled = True
        Me.lbGroups.Location = New System.Drawing.Point(211, 77)
        Me.lbGroups.Name = "lbGroups"
        Me.lbGroups.Size = New System.Drawing.Size(133, 212)
        Me.lbGroups.TabIndex = 10
        '
        'lbTabs
        '
        Me.lbTabs.FormattingEnabled = True
        Me.lbTabs.Location = New System.Drawing.Point(21, 77)
        Me.lbTabs.Name = "lbTabs"
        Me.lbTabs.Size = New System.Drawing.Size(133, 212)
        Me.lbTabs.TabIndex = 9
        '
        'labelForm
        '
        Me.labelForm.AutoSize = True
        Me.labelForm.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.labelForm.Location = New System.Drawing.Point(18, 19)
        Me.labelForm.Name = "labelForm"
        Me.labelForm.Size = New System.Drawing.Size(397, 16)
        Me.labelForm.TabIndex = 8
        Me.labelForm.Text = "Ribbon Information via Microsoft Active Accessibility API"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(18, 59)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(34, 13)
        Me.Label1.TabIndex = 16
        Me.Label1.Text = "Tabs:"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(210, 59)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(44, 13)
        Me.Label2.TabIndex = 17
        Me.Label2.Text = "Groups:"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(399, 59)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(48, 13)
        Me.Label3.TabIndex = 18
        Me.Label3.Text = "Controls:"
        '
        'RibbonInfoForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(560, 347)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.btnExecuteControl)
        Me.Controls.Add(Me.btnListChildControls)
        Me.Controls.Add(Me.btnListChildGroups)
        Me.Controls.Add(Me.lbControls)
        Me.Controls.Add(Me.lbGroups)
        Me.Controls.Add(Me.lbTabs)
        Me.Controls.Add(Me.labelForm)
        Me.Name = "RibbonInfoForm"
        Me.Text = "Ribbon Information (VBOfficeRibbonAccessibility)"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents btnExecuteControl As System.Windows.Forms.Button
    Private WithEvents btnListChildControls As System.Windows.Forms.Button
    Private WithEvents btnListChildGroups As System.Windows.Forms.Button
    Private WithEvents lbControls As System.Windows.Forms.ListBox
    Private WithEvents lbGroups As System.Windows.Forms.ListBox
    Private WithEvents lbTabs As System.Windows.Forms.ListBox
    Private WithEvents labelForm As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
End Class
