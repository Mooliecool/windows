<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class MainForm
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
        Me.btnWriteLocalAppDataLow = New System.Windows.Forms.Button
        Me.btnWriteLocalAppData = New System.Windows.Forms.Button
        Me.label2 = New System.Windows.Forms.Label
        Me.btnCreateLowProcess = New System.Windows.Forms.Button
        Me.lbIntegrityLevel = New System.Windows.Forms.Label
        Me.label1 = New System.Windows.Forms.Label
        Me.SuspendLayout()
        '
        'btnWriteLocalAppDataLow
        '
        Me.btnWriteLocalAppDataLow.Location = New System.Drawing.Point(7, 116)
        Me.btnWriteLocalAppDataLow.Name = "btnWriteLocalAppDataLow"
        Me.btnWriteLocalAppDataLow.Size = New System.Drawing.Size(196, 23)
        Me.btnWriteLocalAppDataLow.TabIndex = 11
        Me.btnWriteLocalAppDataLow.Text = "Write to the LocalAppDataLow folder"
        Me.btnWriteLocalAppDataLow.UseVisualStyleBackColor = True
        '
        'btnWriteLocalAppData
        '
        Me.btnWriteLocalAppData.Location = New System.Drawing.Point(7, 86)
        Me.btnWriteLocalAppData.Name = "btnWriteLocalAppData"
        Me.btnWriteLocalAppData.Size = New System.Drawing.Size(196, 23)
        Me.btnWriteLocalAppData.TabIndex = 10
        Me.btnWriteLocalAppData.Text = "Write to the LocalAppData folder"
        Me.btnWriteLocalAppData.UseVisualStyleBackColor = True
        '
        'label2
        '
        Me.label2.AutoSize = True
        Me.label2.Location = New System.Drawing.Point(7, 64)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(36, 13)
        Me.label2.TabIndex = 9
        Me.label2.Text = "Tests:"
        '
        'btnCreateLowProcess
        '
        Me.btnCreateLowProcess.Location = New System.Drawing.Point(7, 29)
        Me.btnCreateLowProcess.Name = "btnCreateLowProcess"
        Me.btnCreateLowProcess.Size = New System.Drawing.Size(196, 23)
        Me.btnCreateLowProcess.TabIndex = 8
        Me.btnCreateLowProcess.Text = "Launch myself at low integrity level"
        Me.btnCreateLowProcess.UseVisualStyleBackColor = True
        '
        'lbIntegrityLevel
        '
        Me.lbIntegrityLevel.AutoSize = True
        Me.lbIntegrityLevel.Location = New System.Drawing.Point(122, 8)
        Me.lbIntegrityLevel.Name = "lbIntegrityLevel"
        Me.lbIntegrityLevel.Size = New System.Drawing.Size(0, 13)
        Me.lbIntegrityLevel.TabIndex = 7
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(7, 8)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(110, 13)
        Me.label1.TabIndex = 6
        Me.label1.Text = "Curent Integrity Level:"
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(213, 147)
        Me.Controls.Add(Me.btnWriteLocalAppDataLow)
        Me.Controls.Add(Me.btnWriteLocalAppData)
        Me.Controls.Add(Me.label2)
        Me.Controls.Add(Me.btnCreateLowProcess)
        Me.Controls.Add(Me.lbIntegrityLevel)
        Me.Controls.Add(Me.label1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "MainForm"
        Me.Text = "VBCreateLowIntegrityProcess"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Private WithEvents btnWriteLocalAppDataLow As System.Windows.Forms.Button
    Private WithEvents btnWriteLocalAppData As System.Windows.Forms.Button
    Private WithEvents label2 As System.Windows.Forms.Label
    Private WithEvents btnCreateLowProcess As System.Windows.Forms.Button
    Private WithEvents lbIntegrityLevel As System.Windows.Forms.Label
    Private WithEvents label1 As System.Windows.Forms.Label

End Class
