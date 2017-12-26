
Partial Public Class MainForm
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
        Me.lbKey = New System.Windows.Forms.Label()
        Me.tbHotKey = New System.Windows.Forms.TextBox()
        Me.btnRegister = New System.Windows.Forms.Button()
        Me.btnUnregister = New System.Windows.Forms.Button()
        Me.lbNotice = New System.Windows.Forms.Label()
        Me.SuspendLayout()
        '
        'lbKey
        '
        Me.lbKey.AutoSize = True
        Me.lbKey.Location = New System.Drawing.Point(12, 15)
        Me.lbKey.Name = "lbKey"
        Me.lbKey.Size = New System.Drawing.Size(88, 13)
        Me.lbKey.TabIndex = 0
        Me.lbKey.Text = "Press the Hotkey"
        '
        'tbHotKey
        '
        Me.tbHotKey.Location = New System.Drawing.Point(108, 12)
        Me.tbHotKey.Name = "tbHotKey"
        Me.tbHotKey.Size = New System.Drawing.Size(286, 20)
        Me.tbHotKey.TabIndex = 1
        '
        'btnRegister
        '
        Me.btnRegister.Enabled = False
        Me.btnRegister.Location = New System.Drawing.Point(400, 10)
        Me.btnRegister.Name = "btnRegister"
        Me.btnRegister.Size = New System.Drawing.Size(75, 23)
        Me.btnRegister.TabIndex = 2
        Me.btnRegister.Text = "Register"
        Me.btnRegister.UseVisualStyleBackColor = True
        '
        'btnUnregister
        '
        Me.btnUnregister.Enabled = False
        Me.btnUnregister.Location = New System.Drawing.Point(480, 10)
        Me.btnUnregister.Name = "btnUnregister"
        Me.btnUnregister.Size = New System.Drawing.Size(75, 23)
        Me.btnUnregister.TabIndex = 3
        Me.btnUnregister.Text = "Unregister"
        Me.btnUnregister.UseVisualStyleBackColor = True
        '
        'lbNotice
        '
        Me.lbNotice.AutoSize = True
        Me.lbNotice.Location = New System.Drawing.Point(12, 42)
        Me.lbNotice.Name = "lbNotice"
        Me.lbNotice.Size = New System.Drawing.Size(545, 13)
        Me.lbNotice.TabIndex = 4
        Me.lbNotice.Text = "Please click the textbox and press keys.  The keys must be in combination with th" & _
            "e Ctrl, Shift or Alt (e.g. Ctrl+Alt+T)"
        '
        'MainForm
        '
        Me.ClientSize = New System.Drawing.Size(572, 70)
        Me.Controls.Add(Me.lbNotice)
        Me.Controls.Add(Me.btnUnregister)
        Me.Controls.Add(Me.btnRegister)
        Me.Controls.Add(Me.tbHotKey)
        Me.Controls.Add(Me.lbKey)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.Name = "MainForm"
        Me.Text = "VBRegisterHotkey"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

#End Region

    Private lbKey As Label
    Private WithEvents tbHotKey As TextBox
    Private WithEvents btnRegister As Button
    Private WithEvents btnUnregister As Button
    Private lbNotice As Label
End Class

