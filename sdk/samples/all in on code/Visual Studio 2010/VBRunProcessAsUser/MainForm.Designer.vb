Partial Public Class RunProcessAsUser
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
        Me.btnCommand = New Button()
        Me.tbCommand = New TextBox()
        Me.tbPassword = New TextBox()
        Me.tbDomain = New TextBox()
        Me.lbPassword = New Label()
        Me.lbDomain = New Label()
        Me.lbUserName = New Label()
        Me.btnRunCommand = New Button()
        Me.btnCredentialUIPrompt = New Button()
        Me.tbUserName = New TextBox()
        Me.SuspendLayout()
        ' 
        ' btnCommand
        ' 
        Me.btnCommand.FlatStyle = FlatStyle.System
        Me.btnCommand.Location = New Point(23, 118)
        Me.btnCommand.Name = "btnCommand"
        Me.btnCommand.Size = New Size(64, 23)
        Me.btnCommand.TabIndex = 38
        Me.btnCommand.Text = "Command..."
        '			Me.btnCommand.Click += New System.EventHandler(Me.btnCommand_Click)
        ' 
        ' tbCommand
        ' 
        Me.tbCommand.Location = New Point(87, 118)
        Me.tbCommand.Name = "tbCommand"
        Me.tbCommand.Size = New Size(184, 20)
        Me.tbCommand.TabIndex = 39
        ' 
        ' tbPassword
        ' 
        Me.tbPassword.Location = New Point(87, 86)
        Me.tbPassword.Name = "tbPassword"
        Me.tbPassword.Size = New Size(184, 20)
        Me.tbPassword.TabIndex = 37
        ' 
        ' tbDomain
        ' 
        Me.tbDomain.Location = New Point(87, 51)
        Me.tbDomain.Name = "tbDomain"
        Me.tbDomain.Size = New Size(184, 20)
        Me.tbDomain.TabIndex = 36
        ' 
        ' lbPassword
        ' 
        Me.lbPassword.Location = New Point(23, 86)
        Me.lbPassword.Name = "lbPassword"
        Me.lbPassword.Size = New Size(64, 23)
        Me.lbPassword.TabIndex = 43
        Me.lbPassword.Text = "Password"
        ' 
        ' lbDomain
        ' 
        Me.lbDomain.Location = New Point(23, 54)
        Me.lbDomain.Name = "lbDomain"
        Me.lbDomain.Size = New Size(56, 23)
        Me.lbDomain.TabIndex = 42
        Me.lbDomain.Text = "Domain"
        ' 
        ' lbUserName
        ' 
        Me.lbUserName.Location = New Point(23, 22)
        Me.lbUserName.Name = "lbUserName"
        Me.lbUserName.Size = New Size(64, 23)
        Me.lbUserName.TabIndex = 40
        Me.lbUserName.Text = "User Name"
        ' 
        ' btnRunCommand
        ' 
        Me.btnRunCommand.FlatStyle = FlatStyle.System
        Me.btnRunCommand.Location = New Point(87, 144)
        Me.btnRunCommand.Name = "btnRunCommand"
        Me.btnRunCommand.Size = New Size(128, 24)
        Me.btnRunCommand.TabIndex = 41
        Me.btnRunCommand.Text = "Run Command"
        '			Me.btnRunCommand.Click += New System.EventHandler(Me.btnRunCommand_Click)
        ' 
        ' btnCredentialUIPrompt
        ' 
        Me.btnCredentialUIPrompt.Location = New Point(277, 17)
        Me.btnCredentialUIPrompt.Name = "btnCredentialUIPrompt"
        Me.btnCredentialUIPrompt.Size = New Size(39, 23)
        Me.btnCredentialUIPrompt.TabIndex = 44
        Me.btnCredentialUIPrompt.Text = "..."
        Me.btnCredentialUIPrompt.UseVisualStyleBackColor = True
        '			Me.btnCredentialUIPrompt.Click += New System.EventHandler(Me.btnCredentialUIPrompt_Click)
        ' 
        ' tbUserName
        ' 
        Me.tbUserName.Location = New Point(87, 19)
        Me.tbUserName.Name = "tbUserName"
        Me.tbUserName.Size = New Size(184, 20)
        Me.tbUserName.TabIndex = 47
        ' 
        ' RunProcessAsUser
        ' 
        Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = AutoScaleMode.Font
        Me.ClientSize = New Size(330, 198)
        Me.Controls.Add(Me.tbUserName)
        Me.Controls.Add(Me.btnCredentialUIPrompt)
        Me.Controls.Add(Me.btnCommand)
        Me.Controls.Add(Me.tbCommand)
        Me.Controls.Add(Me.tbPassword)
        Me.Controls.Add(Me.tbDomain)
        Me.Controls.Add(Me.lbPassword)
        Me.Controls.Add(Me.lbDomain)
        Me.Controls.Add(Me.lbUserName)
        Me.Controls.Add(Me.btnRunCommand)
        Me.FormBorderStyle = FormBorderStyle.FixedDialog
        Me.Name = "RunProcessAsUser"
        Me.Text = "RunProcessAsUser"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

#End Region

    Private WithEvents btnCommand As Button
    Private tbCommand As TextBox
    Private tbPassword As TextBox
    Private tbDomain As TextBox
    Private lbPassword As Label
    Private lbDomain As Label
    Private lbUserName As Label
    Private WithEvents btnRunCommand As Button
    Private WithEvents btnCredentialUIPrompt As Button
    Private tbUserName As TextBox
End Class


