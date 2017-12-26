<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class GroupForm
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
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
        Me.btnGroup = New System.Windows.Forms.Button
        Me.txtDir = New System.Windows.Forms.TextBox
        Me.groupMenu = New System.Windows.Forms.MainMenu
        Me.miExit = New System.Windows.Forms.MenuItem
        Me.miSearch = New System.Windows.Forms.MenuItem
        Me.colLMD = New System.Windows.Forms.ColumnHeader
        Me.lvGroup = New System.Windows.Forms.ListView
        Me.colExtension = New System.Windows.Forms.ColumnHeader
        Me.colName = New System.Windows.Forms.ColumnHeader
        Me.label1 = New System.Windows.Forms.Label
        Me.SuspendLayout()
        '
        'btnGroup
        '
        Me.btnGroup.Location = New System.Drawing.Point(6, 29)
        Me.btnGroup.Name = "btnGroup"
        Me.btnGroup.Size = New System.Drawing.Size(233, 20)
        Me.btnGroup.TabIndex = 7
        Me.btnGroup.Text = "Group"
        '
        'txtDir
        '
        Me.txtDir.Location = New System.Drawing.Point(91, 4)
        Me.txtDir.Name = "txtDir"
        Me.txtDir.Size = New System.Drawing.Size(148, 21)
        Me.txtDir.TabIndex = 5
        Me.txtDir.Text = "\windows"
        '
        'groupMenu
        '
        Me.groupMenu.MenuItems.Add(Me.miExit)
        Me.groupMenu.MenuItems.Add(Me.miSearch)
        '
        'miExit
        '
        Me.miExit.Text = "Exit"
        '
        'miSearch
        '
        Me.miSearch.Text = "Search View"
        '
        'colLMD
        '
        Me.colLMD.Text = "Date modified"
        Me.colLMD.Width = 75
        '
        'lvGroup
        '
        Me.lvGroup.Columns.Add(Me.colExtension)
        Me.lvGroup.Columns.Add(Me.colName)
        Me.lvGroup.Columns.Add(Me.colLMD)
        Me.lvGroup.Location = New System.Drawing.Point(5, 55)
        Me.lvGroup.Name = "lvGroup"
        Me.lvGroup.Size = New System.Drawing.Size(234, 209)
        Me.lvGroup.TabIndex = 6
        Me.lvGroup.View = System.Windows.Forms.View.Details
        Me.lvGroup.Visible = False
        '
        'colExtension
        '
        Me.colExtension.Text = "Extension"
        Me.colExtension.Width = 75
        '
        'colName
        '
        Me.colName.Text = "Name"
        Me.colName.Width = 100
        '
        'label1
        '
        Me.label1.Location = New System.Drawing.Point(2, 5)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(83, 20)
        Me.label1.Text = "Directory Path"
        '
        'GroupForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(96.0!, 96.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(240, 268)
        Me.Controls.Add(Me.btnGroup)
        Me.Controls.Add(Me.txtDir)
        Me.Controls.Add(Me.lvGroup)
        Me.Controls.Add(Me.label1)
        Me.Menu = Me.groupMenu
        Me.Name = "GroupForm"
        Me.Text = "GroupForm"
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents btnGroup As System.Windows.Forms.Button
    Private WithEvents txtDir As System.Windows.Forms.TextBox
    Private WithEvents groupMenu As System.Windows.Forms.MainMenu
    Private WithEvents miExit As System.Windows.Forms.MenuItem
    Private WithEvents miSearch As System.Windows.Forms.MenuItem
    Private WithEvents colLMD As System.Windows.Forms.ColumnHeader
    Private WithEvents lvGroup As System.Windows.Forms.ListView
    Private WithEvents colExtension As System.Windows.Forms.ColumnHeader
    Private WithEvents colName As System.Windows.Forms.ColumnHeader
    Private WithEvents label1 As System.Windows.Forms.Label
End Class
