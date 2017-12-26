
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
        Me.lbRegKeyPath = New Label()
        Me.tbRegkeyPath = New TextBox()
        Me.btnMonitor = New Button()
        Me.lstChanges = New ListBox()
        Me.panel1 = New Panel()
        Me.cmbHives = New ComboBox()
        Me.panel2 = New Panel()
        Me.panel1.SuspendLayout()
        Me.panel2.SuspendLayout()
        Me.SuspendLayout()
        ' 
        ' lbRegKeyPath
        ' 
        Me.lbRegKeyPath.AutoSize = True
        Me.lbRegKeyPath.Location = New Point(11, 21)
        Me.lbRegKeyPath.Name = "lbRegKeyPath"
        Me.lbRegKeyPath.Size = New Size(91, 13)
        Me.lbRegKeyPath.TabIndex = 0
        Me.lbRegKeyPath.Text = "Registry Key Path"
        ' 
        ' tbRegkeyPath
        ' 
        Me.tbRegkeyPath.Location = New Point(264, 18)
        Me.tbRegkeyPath.Name = "tbRegkeyPath"
        Me.tbRegkeyPath.Size = New Size(491, 20)
        Me.tbRegkeyPath.TabIndex = 1
        Me.tbRegkeyPath.Text = "SOFTWARE\\Microsoft"
        ' 
        ' btnMonitor
        ' 
        Me.btnMonitor.Location = New Point(761, 16)
        Me.btnMonitor.Name = "btnMonitor"
        Me.btnMonitor.Size = New Size(78, 23)
        Me.btnMonitor.TabIndex = 2
        Me.btnMonitor.Text = "Start Monitor"
        Me.btnMonitor.UseVisualStyleBackColor = True
        '			Me.btnMonitor.Click += New System.EventHandler(Me.btnMonitor_Click)
        ' 
        ' lstChanges
        ' 
        Me.lstChanges.Dock = DockStyle.Fill
        Me.lstChanges.FormattingEnabled = True
        Me.lstChanges.Location = New Point(0, 0)
        Me.lstChanges.Name = "lstChanges"
        Me.lstChanges.Size = New Size(851, 348)
        Me.lstChanges.TabIndex = 3
        ' 
        ' panel1
        ' 
        Me.panel1.Controls.Add(Me.cmbHives)
        Me.panel1.Controls.Add(Me.lbRegKeyPath)
        Me.panel1.Controls.Add(Me.tbRegkeyPath)
        Me.panel1.Controls.Add(Me.btnMonitor)
        Me.panel1.Dock = DockStyle.Top
        Me.panel1.Location = New Point(0, 0)
        Me.panel1.Name = "panel1"
        Me.panel1.Size = New Size(851, 43)
        Me.panel1.TabIndex = 4
        ' 
        ' cmbHives
        ' 
        Me.cmbHives.DropDownStyle = ComboBoxStyle.DropDownList
        Me.cmbHives.FormattingEnabled = True
        Me.cmbHives.Location = New Point(108, 17)
        Me.cmbHives.Name = "cmbHives"
        Me.cmbHives.Size = New Size(150, 21)
        Me.cmbHives.TabIndex = 3
        ' 
        ' panel2
        ' 
        Me.panel2.Controls.Add(Me.lstChanges)
        Me.panel2.Dock = DockStyle.Fill
        Me.panel2.Location = New Point(0, 43)
        Me.panel2.Name = "panel2"
        Me.panel2.Size = New Size(851, 348)
        Me.panel2.TabIndex = 5
        ' 
        ' MainForm
        ' 
        Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = AutoScaleMode.Font
        Me.ClientSize = New Size(851, 391)
        Me.Controls.Add(Me.panel2)
        Me.Controls.Add(Me.panel1)
        Me.MinimumSize = New Size(867, 429)
        Me.Name = "MainForm"
        Me.Text = "Registry Monitor"
        '			Me.Load += New System.EventHandler(Me.MainForm_Load)
        Me.panel1.ResumeLayout(False)
        Me.panel1.PerformLayout()
        Me.panel2.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private lbRegKeyPath As Label
    Private tbRegkeyPath As TextBox
    Private WithEvents btnMonitor As Button
    Private lstChanges As ListBox
    Private panel1 As Panel
    Private panel2 As Panel
    Private cmbHives As ComboBox
End Class

