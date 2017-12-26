Partial Public Class PingClientForm
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
        Me.pingLabel = New System.Windows.Forms.Label
        Me.addressTextBox = New System.Windows.Forms.TextBox
        Me.sendPingButton = New System.Windows.Forms.Button
        Me.pingDetailsTextBox = New System.Windows.Forms.TextBox
        Me.cancelPingButton = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'pingLabel
        '
        Me.pingLabel.AutoSize = True
        Me.pingLabel.Location = New System.Drawing.Point(12, 17)
        Me.pingLabel.Name = "pingLabel"
        Me.pingLabel.Size = New System.Drawing.Size(51, 14)
        Me.pingLabel.TabIndex = 0
        Me.pingLabel.Text = "Name\IP:"
        '
        'addressTextBox
        '
        Me.addressTextBox.Location = New System.Drawing.Point(70, 14)
        Me.addressTextBox.Name = "addressTextBox"
        Me.addressTextBox.Size = New System.Drawing.Size(210, 20)
        Me.addressTextBox.TabIndex = 1
        '
        'sendPingButton
        '
        Me.sendPingButton.Location = New System.Drawing.Point(124, 231)
        Me.sendPingButton.Name = "sendPingButton"
        Me.sendPingButton.TabIndex = 2
        Me.sendPingButton.Text = "Send"
        '
        'pingDetailsTextBox
        '
        Me.pingDetailsTextBox.AutoSize = False
        Me.pingDetailsTextBox.Location = New System.Drawing.Point(12, 42)
        Me.pingDetailsTextBox.Multiline = True
        Me.pingDetailsTextBox.Name = "pingDetailsTextBox"
        Me.pingDetailsTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.pingDetailsTextBox.Size = New System.Drawing.Size(268, 183)
        Me.pingDetailsTextBox.TabIndex = 4
        '
        'cancelPingButton
        '
        Me.cancelPingButton.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.cancelPingButton.Location = New System.Drawing.Point(205, 231)
        Me.cancelPingButton.Name = "cancelPingButton"
        Me.cancelPingButton.TabIndex = 3
        Me.cancelPingButton.Text = "Cancel"
        '
        'PingClientForm
        '
        Me.AcceptButton = Me.sendPingButton
        Me.CancelButton = Me.cancelPingButton
        Me.ClientSize = New System.Drawing.Size(292, 266)
        Me.Controls.Add(Me.cancelPingButton)
        Me.Controls.Add(Me.pingDetailsTextBox)
        Me.Controls.Add(Me.sendPingButton)
        Me.Controls.Add(Me.addressTextBox)
        Me.Controls.Add(Me.pingLabel)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.Name = "PingClientForm"
        Me.Text = "Ping Client"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents pingLabel As System.Windows.Forms.Label
    Friend WithEvents addressTextBox As System.Windows.Forms.TextBox
    Friend WithEvents sendPingButton As System.Windows.Forms.Button
    Friend WithEvents pingDetailsTextBox As System.Windows.Forms.TextBox
    Friend WithEvents cancelPingButton As System.Windows.Forms.Button

End Class
