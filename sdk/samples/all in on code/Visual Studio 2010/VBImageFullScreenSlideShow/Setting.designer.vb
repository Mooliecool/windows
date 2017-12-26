Partial Public Class Setting
    ''' <summary>
    ''' Required designer variable.
    ''' </summary>
    Private components As System.ComponentModel.IContainer = Nothing

    ''' <summary>
    ''' Clean up any resources being used.
    ''' </summary>
    ''' <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso (components IsNot Nothing) Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

#Region "Windows Form Designer generated code"

    ''' <summary>
    ''' Required method for Designer support - do not modify
    ''' the contents of this method with the code editor.
    ''' </summary>
    Private Sub InitializeComponent()
        Me.lbTimerInternal = New Label()
        Me.dtpInternal = New NumericUpDown()
        Me.lbMilliseconds = New Label()
        Me.btnConfirm = New Button()
        Me.btnCancel = New Button()
        CType(Me.dtpInternal, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        ' 
        ' lbTimerInternal
        ' 
        Me.lbTimerInternal.AutoSize = True
        Me.lbTimerInternal.Location = New Point(29, 22)
        Me.lbTimerInternal.Name = "lbTimerInternal"
        Me.lbTimerInternal.Size = New Size(77, 13)
        Me.lbTimerInternal.TabIndex = 0
        Me.lbTimerInternal.Text = "Timer Internal："
        ' 
        ' dtpInternal
        ' 
        Me.dtpInternal.Location = New Point(112, 20)
        Me.dtpInternal.Maximum = New Decimal(New Integer() {10000000, 0, 0, 0})
        Me.dtpInternal.Name = "dtpInternal"
        Me.dtpInternal.Size = New Size(120, 20)
        Me.dtpInternal.TabIndex = 1
        ' 
        ' lbMilliseconds
        ' 
        Me.lbMilliseconds.AutoSize = True
        Me.lbMilliseconds.Location = New Point(248, 22)
        Me.lbMilliseconds.Name = "lbMilliseconds"
        Me.lbMilliseconds.Size = New Size(64, 13)
        Me.lbMilliseconds.TabIndex = 2
        Me.lbMilliseconds.Text = "Milliseconds"
        ' 
        ' btnConfirm
        ' 
        Me.btnConfirm.Location = New Point(75, 54)
        Me.btnConfirm.Name = "btnConfirm"
        Me.btnConfirm.Size = New Size(75, 23)
        Me.btnConfirm.TabIndex = 3
        Me.btnConfirm.Text = "Confirm"
        Me.btnConfirm.UseVisualStyleBackColor = True
        '			Me.btnConfirm.Click += New System.EventHandler(Me.btnConfirm_Click)
        ' 
        ' btnCancel
        ' 
        Me.btnCancel.Location = New Point(177, 54)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New Size(75, 23)
        Me.btnCancel.TabIndex = 4
        Me.btnCancel.Text = "Cancel"
        Me.btnCancel.UseVisualStyleBackColor = True
        '			Me.btnCancel.Click += New System.EventHandler(Me.btnCancel_Click)
        ' 
        ' Settings
        ' 
        Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = AutoScaleMode.Font
        Me.ClientSize = New Size(343, 89)
        Me.Controls.Add(Me.btnCancel)
        Me.Controls.Add(Me.btnConfirm)
        Me.Controls.Add(Me.lbMilliseconds)
        Me.Controls.Add(Me.dtpInternal)
        Me.Controls.Add(Me.lbTimerInternal)
        Me.FormBorderStyle = FormBorderStyle.FixedDialog
        Me.Name = "Settings"
        Me.Text = "Settings"
        CType(Me.dtpInternal, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

#End Region

    Private lbTimerInternal As Label
    Private dtpInternal As NumericUpDown
    Private lbMilliseconds As Label
    Private WithEvents btnConfirm As Button
    Private WithEvents btnCancel As Button
End Class
