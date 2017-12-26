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
        Me.components = New System.ComponentModel.Container
        Me.groupBox1 = New System.Windows.Forms.GroupBox
        Me.flashButton = New System.Windows.Forms.Button
        Me.errorButton = New System.Windows.Forms.Button
        Me.indeterminateButton = New System.Windows.Forms.Button
        Me.progressBar = New System.Windows.Forms.ProgressBar
        Me.pauseButton = New System.Windows.Forms.Button
        Me.startButton = New System.Windows.Forms.Button
        Me.flashWindowTimer = New System.Windows.Forms.Timer(Me.components)
        Me.progreeBarTimer = New System.Windows.Forms.Timer(Me.components)
        Me.groupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'groupBox1
        '
        Me.groupBox1.Controls.Add(Me.flashButton)
        Me.groupBox1.Controls.Add(Me.errorButton)
        Me.groupBox1.Controls.Add(Me.indeterminateButton)
        Me.groupBox1.Controls.Add(Me.progressBar)
        Me.groupBox1.Controls.Add(Me.pauseButton)
        Me.groupBox1.Controls.Add(Me.startButton)
        Me.groupBox1.Location = New System.Drawing.Point(54, 30)
        Me.groupBox1.Name = "groupBox1"
        Me.groupBox1.Size = New System.Drawing.Size(492, 127)
        Me.groupBox1.TabIndex = 4
        Me.groupBox1.TabStop = False
        '
        'flashButton
        '
        Me.flashButton.Location = New System.Drawing.Point(367, 79)
        Me.flashButton.Name = "flashButton"
        Me.flashButton.Size = New System.Drawing.Size(75, 23)
        Me.flashButton.TabIndex = 5
        Me.flashButton.Text = "Flash"
        Me.flashButton.UseVisualStyleBackColor = True
        '
        'errorButton
        '
        Me.errorButton.Location = New System.Drawing.Point(286, 79)
        Me.errorButton.Name = "errorButton"
        Me.errorButton.Size = New System.Drawing.Size(75, 23)
        Me.errorButton.TabIndex = 4
        Me.errorButton.Text = "Error"
        Me.errorButton.UseVisualStyleBackColor = True
        '
        'indeterminateButton
        '
        Me.indeterminateButton.Location = New System.Drawing.Point(205, 79)
        Me.indeterminateButton.Name = "indeterminateButton"
        Me.indeterminateButton.Size = New System.Drawing.Size(75, 23)
        Me.indeterminateButton.TabIndex = 3
        Me.indeterminateButton.Text = "Indeterminate"
        Me.indeterminateButton.UseVisualStyleBackColor = True
        '
        'progressBar
        '
        Me.progressBar.Location = New System.Drawing.Point(29, 32)
        Me.progressBar.Name = "progressBar"
        Me.progressBar.Size = New System.Drawing.Size(428, 23)
        Me.progressBar.TabIndex = 0
        '
        'pauseButton
        '
        Me.pauseButton.Location = New System.Drawing.Point(124, 79)
        Me.pauseButton.Name = "pauseButton"
        Me.pauseButton.Size = New System.Drawing.Size(75, 23)
        Me.pauseButton.TabIndex = 2
        Me.pauseButton.Text = "Pause"
        Me.pauseButton.UseVisualStyleBackColor = True
        '
        'startButton
        '
        Me.startButton.Location = New System.Drawing.Point(43, 79)
        Me.startButton.Name = "startButton"
        Me.startButton.Size = New System.Drawing.Size(75, 23)
        Me.startButton.TabIndex = 1
        Me.startButton.Text = "Start"
        Me.startButton.UseVisualStyleBackColor = True
        '
        'flashWindowTimer
        '
        Me.flashWindowTimer.Interval = 400
        '
        'progreeBarTimer
        '
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(595, 186)
        Me.Controls.Add(Me.groupBox1)
        Me.Name = "MainForm"
        Me.Text = "Win7 Taskbar Progressbar"
        Me.groupBox1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents groupBox1 As System.Windows.Forms.GroupBox
    Private WithEvents flashButton As System.Windows.Forms.Button
    Private WithEvents errorButton As System.Windows.Forms.Button
    Private WithEvents indeterminateButton As System.Windows.Forms.Button
    Private WithEvents progressBar As System.Windows.Forms.ProgressBar
    Private WithEvents pauseButton As System.Windows.Forms.Button
    Private WithEvents startButton As System.Windows.Forms.Button
    Private WithEvents flashWindowTimer As System.Windows.Forms.Timer
    Private WithEvents progreeBarTimer As System.Windows.Forms.Timer

End Class
