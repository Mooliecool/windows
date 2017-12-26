<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class SearchForm
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(SearchForm))
        Me.label1 = New System.Windows.Forms.Label
        Me.txtSearch = New System.Windows.Forms.TextBox
        Me.searchMenu = New System.Windows.Forms.MainMenu
        Me.miExit = New System.Windows.Forms.MenuItem
        Me.miView = New System.Windows.Forms.MenuItem
        Me.tvImages = New System.Windows.Forms.ImageList
        Me.treeView = New System.Windows.Forms.TreeView
        Me.SuspendLayout()
        '
        'label1
        '
        Me.label1.Location = New System.Drawing.Point(2, 8)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(77, 20)
        Me.label1.Text = "SpotSearch"
        '
        'txtSearch
        '
        Me.txtSearch.Location = New System.Drawing.Point(85, 8)
        Me.txtSearch.Name = "txtSearch"
        Me.txtSearch.Size = New System.Drawing.Size(151, 21)
        Me.txtSearch.TabIndex = 9
        '
        'searchMenu
        '
        Me.searchMenu.MenuItems.Add(Me.miExit)
        Me.searchMenu.MenuItems.Add(Me.miView)
        '
        'miExit
        '
        Me.miExit.Text = "Exit"
        '
        'miView
        '
        Me.miView.Text = "Group View"
        Me.tvImages.Images.Clear()
        Me.tvImages.Images.Add(CType(resources.GetObject("resource"), System.Drawing.Image))
        Me.tvImages.Images.Add(CType(resources.GetObject("resource1"), System.Drawing.Image))
        '
        'treeView
        '
        Me.treeView.ImageIndex = 0
        Me.treeView.ImageList = Me.tvImages
        Me.treeView.Location = New System.Drawing.Point(2, 35)
        Me.treeView.Name = "treeView"
        Me.treeView.SelectedImageIndex = 0
        Me.treeView.Size = New System.Drawing.Size(234, 226)
        Me.treeView.TabIndex = 8
        '
        'SearchForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(96.0!, 96.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(240, 268)
        Me.Controls.Add(Me.label1)
        Me.Controls.Add(Me.txtSearch)
        Me.Controls.Add(Me.treeView)
        Me.Menu = Me.searchMenu
        Me.Name = "SearchForm"
        Me.Text = "Search Form"
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents label1 As System.Windows.Forms.Label
    Private WithEvents txtSearch As System.Windows.Forms.TextBox
    Private WithEvents searchMenu As System.Windows.Forms.MainMenu
    Private WithEvents miExit As System.Windows.Forms.MenuItem
    Private WithEvents miView As System.Windows.Forms.MenuItem
    Private WithEvents tvImages As System.Windows.Forms.ImageList
    Private WithEvents treeView As System.Windows.Forms.TreeView
End Class
