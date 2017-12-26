
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
        Dim resources As New System.ComponentModel.ComponentResourceManager(GetType(MainForm))
        Dim xmlViewerSettings1 As New XMLViewerSettings()
        Me.pnlMenu = New Panel()
        Me.lbNote = New Label()
        Me.btnProcess = New Button()
        Me.viewer = New XMLViewer()
        Me.pnlMenu.SuspendLayout()
        Me.SuspendLayout()
        ' 
        ' pnlMenu
        ' 
        Me.pnlMenu.Controls.Add(Me.lbNote)
        Me.pnlMenu.Controls.Add(Me.btnProcess)
        Me.pnlMenu.Dock = DockStyle.Top
        Me.pnlMenu.Location = New Point(0, 0)
        Me.pnlMenu.Name = "pnlMenu"
        Me.pnlMenu.Size = New Size(775, 79)
        Me.pnlMenu.TabIndex = 1
        ' 
        ' lbNote
        ' 
        Me.lbNote.AutoSize = True
        Me.lbNote.Location = New Point(12, 9)
        Me.lbNote.Name = "lbNote"
        Me.lbNote.Size = New Size(389, 65)
        Me.lbNote.TabIndex = 2
        Me.lbNote.Text = resources.GetString("lbNote.Text")
        ' 
        ' btnProcess
        ' 
        Me.btnProcess.Location = New Point(420, 38)
        Me.btnProcess.Name = "btnProcess"
        Me.btnProcess.Size = New Size(75, 23)
        Me.btnProcess.TabIndex = 1
        Me.btnProcess.Text = "Process"
        Me.btnProcess.UseVisualStyleBackColor = True
        '			Me.btnProcess.Click += New System.EventHandler(Me.btnProcess_Click)
        ' 
        ' viewer
        ' 
        Me.viewer.Dock = DockStyle.Fill
        Me.viewer.Location = New Point(0, 79)
        Me.viewer.Name = "viewer"
        xmlViewerSettings1.AttributeKey = Color.Red
        xmlViewerSettings1.AttributeValue = Color.Blue
        xmlViewerSettings1.Element = Color.DarkRed
        xmlViewerSettings1.Tag = Color.Blue
        xmlViewerSettings1.Value = Color.Black
        Me.viewer.Settings = xmlViewerSettings1
        Me.viewer.Size = New Size(775, 381)
        Me.viewer.TabIndex = 0
        Me.viewer.Text = "<?xml version=""1.0"" encoding=""utf-8"" ?><html><head><title>My home page</title></h" & "ead><body bgcolor=""000000"" text=""ff0000"">Hello World!</body></html>" & vbLf
        ' 
        ' MainForm
        ' 
        Me.AutoScaleDimensions = New SizeF(6.0F, 13.0F)
        Me.AutoScaleMode = AutoScaleMode.Font
        Me.ClientSize = New Size(775, 460)
        Me.Controls.Add(Me.viewer)
        Me.Controls.Add(Me.pnlMenu)
        Me.Name = "MainForm"
        Me.Text = "SimpleXMLViewer"
        Me.pnlMenu.ResumeLayout(False)
        Me.pnlMenu.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private viewer As XMLViewer
    Private pnlMenu As Panel
    Private WithEvents btnProcess As Button
    Private lbNote As Label
End Class

