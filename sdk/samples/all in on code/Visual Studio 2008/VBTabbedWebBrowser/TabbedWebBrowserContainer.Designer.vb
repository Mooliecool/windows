
Partial Public Class TabbedWebBrowserContainer
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

#Region "Component Designer generated code"

    ''' <summary> 
    ''' Required method for Designer support - do not modify 
    ''' the contents of this method with the code editor.
    ''' </summary>
    Private Sub InitializeComponent()
        Me.tabControl = New TabControl()
        Me.SuspendLayout()
        ' 
        ' tabControl
        ' 
        Me.tabControl.Dock = DockStyle.Fill
        Me.tabControl.ItemSize = New Size(58, 28)
        Me.tabControl.Location = New Point(0, 0)
        Me.tabControl.Multiline = True
        Me.tabControl.Name = "tabControl"
        Me.tabControl.SelectedIndex = 0
        Me.tabControl.Size = New Size(800, 600)
        Me.tabControl.TabIndex = 0
        ' 
        ' TabbedWebBrowserContainer
        ' 
        Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = AutoScaleMode.Font
        Me.Controls.Add(Me.tabControl)
        Me.Name = "TabbedWebBrowserContainer"
        Me.Size = New Size(800, 600)
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private tabControl As TabControl
End Class

