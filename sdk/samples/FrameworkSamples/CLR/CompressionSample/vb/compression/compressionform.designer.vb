

Partial Public Class CompressionForm
    Inherits System.Windows.Forms.Form
    '/ <summary>
    '/ Required method for Designer support - do not modify
    '/ the contents of this method with the code editor.
    '/ </summary>
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Me.mainToolStrip = New System.Windows.Forms.ToolStrip
        Me.newGzipToolStripButton = New System.Windows.Forms.ToolStripButton
        Me.newDeflateToolStripButton = New System.Windows.Forms.ToolStripButton
        Me.openToolStripButton = New System.Windows.Forms.ToolStripButton
        Me.newToolStripSeparator = New System.Windows.Forms.ToolStripSeparator
        Me.addToolStripButton = New System.Windows.Forms.ToolStripButton
        Me.removeToolStripButton = New System.Windows.Forms.ToolStripButton
        Me.extractToolStripButton = New System.Windows.Forms.ToolStripButton
        Me.addToolStripSeparator = New System.Windows.Forms.ToolStripSeparator
        Me.newGzipMenuToolStripButton = New System.Windows.Forms.ToolStripMenuItem
        Me.newDeflateMenuToolStripButton = New System.Windows.Forms.ToolStripMenuItem
        Me.mainStatusStrip = New System.Windows.Forms.StatusStrip
        Me.fileListView = New System.Windows.Forms.ListView
        Me.mainMenuStrip = New System.Windows.Forms.MenuStrip
        Me.fileToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.openMenuStripButton = New System.Windows.Forms.ToolStripMenuItem
        Me.toolStripSeparator1 = New System.Windows.Forms.ToolStripSeparator
        Me.closeMenuStripButton = New System.Windows.Forms.ToolStripMenuItem
        Me.actionsToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.addMenuStripButton = New System.Windows.Forms.ToolStripMenuItem
        Me.removeMenuStripButton = New System.Windows.Forms.ToolStripMenuItem
        Me.extractMenuStripButton = New System.Windows.Forms.ToolStripMenuItem
        Me.extractAllMenuStripButton = New System.Windows.Forms.ToolStripMenuItem
        Me.fileContextMenuStrip = New System.Windows.Forms.ContextMenuStrip(Me.components)
        Me.contextMenuStripButton = New System.Windows.Forms.ToolStripMenuItem
        Me.removeContextMenuStripButton = New System.Windows.Forms.ToolStripMenuItem
        Me.mainToolStrip.SuspendLayout()
        Me.mainMenuStrip.SuspendLayout()
        Me.fileContextMenuStrip.SuspendLayout()
        Me.SuspendLayout()
        '
        'mainToolStrip
        '
        Me.mainToolStrip.Anchor = System.Windows.Forms.AnchorStyles.Right
        Me.mainToolStrip.Dock = System.Windows.Forms.DockStyle.None
        Me.mainToolStrip.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.newGzipToolStripButton, Me.newDeflateToolStripButton, Me.openToolStripButton, Me.newToolStripSeparator, Me.addToolStripButton, Me.removeToolStripButton, Me.extractToolStripButton, Me.addToolStripSeparator})
        Me.mainToolStrip.Location = New System.Drawing.Point(3, 3)
        Me.mainToolStrip.Name = "mainToolStrip"
        Me.mainToolStrip.Size = New System.Drawing.Size(397, 25)
        Me.mainToolStrip.TabIndex = 1
        Me.mainToolStrip.Text = "toolStrip1"
        '
        'newGzipToolStripButton
        '
        Me.newGzipToolStripButton.Name = "newGzipToolStripButton"
        Me.newGzipToolStripButton.Text = "New Gzip"
        '
        'newDeflateToolStripButton
        '
        Me.newDeflateToolStripButton.Name = "newDeflateToolStripButton"
        Me.newDeflateToolStripButton.Text = "New Deflate"
        '
        'openToolStripButton
        '
        Me.openToolStripButton.Name = "openToolStripButton"
        Me.openToolStripButton.Text = "Open"
        '
        'newToolStripSeparator
        '
        Me.newToolStripSeparator.Name = "newToolStripSeparator"
        '
        'addToolStripButton
        '
        Me.addToolStripButton.Name = "addToolStripButton"
        Me.addToolStripButton.Text = "Add"
        '
        'removeToolStripButton
        '
        Me.removeToolStripButton.Name = "removeToolStripButton"
        Me.removeToolStripButton.Text = "Remove"
        '
        'extractToolStripButton
        '
        Me.extractToolStripButton.Name = "extractToolStripButton"
        Me.extractToolStripButton.Text = "Extract"
        '
        'addToolStripSeparator
        '
        Me.addToolStripSeparator.Name = "addToolStripSeparator"
        '
        'newGzipMenuToolStripButton
        '
        Me.newGzipMenuToolStripButton.Image = CompressionSample.My.Resources.Resources.NewGzipToolStripMenuItem_Image
        Me.newGzipMenuToolStripButton.Name = "newGzipMenuToolStripButton"
        Me.newGzipMenuToolStripButton.Text = "New Gzip"
        '
        'newDeflateMenuToolStripButton
        '
        Me.newDeflateMenuToolStripButton.Image = CompressionSample.My.Resources.Resources.NewDeflateToolStripMenuItem_Image
        Me.newDeflateMenuToolStripButton.ImageTransparentColor = System.Drawing.Color.Transparent
        Me.newDeflateMenuToolStripButton.Name = "newDeflateMenuToolStripButton"
        Me.newDeflateMenuToolStripButton.Text = "New Deflate"
        '
        'mainStatusStrip
        '
        Me.mainStatusStrip.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.Table
        Me.mainStatusStrip.Location = New System.Drawing.Point(9, 534)
        Me.mainStatusStrip.Name = "mainStatusStrip"
        Me.mainStatusStrip.Size = New System.Drawing.Size(769, 23)
        Me.mainStatusStrip.TabIndex = 5
        Me.mainStatusStrip.Text = "statusStrip1"
        '
        'fileListView
        '
        Me.fileListView.Dock = System.Windows.Forms.DockStyle.Fill
        Me.fileListView.FullRowSelect = True
        Me.fileListView.Location = New System.Drawing.Point(9, 33)
        Me.fileListView.Name = "fileListView"
        Me.fileListView.ShowItemToolTips = True
        Me.fileListView.Size = New System.Drawing.Size(769, 501)
        Me.fileListView.TabIndex = 6
        '
        'mainMenuStrip
        '
        Me.mainMenuStrip.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.fileToolStripMenuItem, Me.actionsToolStripMenuItem})
        Me.mainMenuStrip.Location = New System.Drawing.Point(9, 9)
        Me.mainMenuStrip.Name = "mainMenuStrip"
        Me.mainMenuStrip.Size = New System.Drawing.Size(769, 24)
        Me.mainMenuStrip.TabIndex = 7
        Me.mainMenuStrip.Text = "menuStrip1"
        '
        'fileToolStripMenuItem
        '
        Me.fileToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.newGzipMenuToolStripButton, Me.newDeflateMenuToolStripButton, Me.openMenuStripButton, Me.toolStripSeparator1, Me.closeMenuStripButton})
        Me.fileToolStripMenuItem.Name = "fileToolStripMenuItem"
        Me.fileToolStripMenuItem.Text = "&File"
        '
        'openMenuStripButton
        '
        Me.openMenuStripButton.Image = CompressionSample.My.Resources.Resources.OpenToolStripMenuItem_Image
        Me.openMenuStripButton.Name = "openMenuStripButton"
        Me.openMenuStripButton.Text = "Open"
        '
        'toolStripSeparator1
        '
        Me.toolStripSeparator1.Name = "toolStripSeparator1"
        '
        'closeMenuStripButton
        '
        Me.closeMenuStripButton.Name = "closeMenuStripButton"
        Me.closeMenuStripButton.Text = "Exit"
        '
        'actionsToolStripMenuItem
        '
        Me.actionsToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.addMenuStripButton, Me.removeMenuStripButton, Me.extractMenuStripButton, Me.extractAllMenuStripButton})
        Me.actionsToolStripMenuItem.Name = "actionsToolStripMenuItem"
        Me.actionsToolStripMenuItem.Text = "&Actions"
        '
        'addMenuStripButton
        '
        Me.addMenuStripButton.Image = CompressionSample.My.Resources.Resources.CopyToolStripMenuItem_Image
        Me.addMenuStripButton.Name = "addMenuStripButton"
        Me.addMenuStripButton.Text = "Add"
        '
        'removeMenuStripButton
        '
        Me.removeMenuStripButton.Image = CompressionSample.My.Resources.Resources.CutToolStripMenuItem_Image
        Me.removeMenuStripButton.Name = "removeMenuStripButton"
        Me.removeMenuStripButton.Text = "Remove"
        '
        'extractMenuStripButton
        '
        Me.extractMenuStripButton.Image = CompressionSample.My.Resources.Resources.UndoToolStripMenuItem_Image
        Me.extractMenuStripButton.Name = "extractMenuStripButton"
        Me.extractMenuStripButton.Text = "Extract"
        '
        'extractAllMenuStripButton
        '
        Me.extractAllMenuStripButton.Image = CompressionSample.My.Resources.Resources.UndoToolStripMenuItem_Image
        Me.extractAllMenuStripButton.Name = "extractAllMenuStripButton"
        Me.extractAllMenuStripButton.Text = "Extract All"
        '
        'fileContextMenuStrip
        '
        Me.fileContextMenuStrip.BackColor = System.Drawing.SystemColors.Menu
        Me.fileContextMenuStrip.Enabled = True
        Me.fileContextMenuStrip.GripMargin = New System.Windows.Forms.Padding(2)
        Me.fileContextMenuStrip.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.contextMenuStripButton, Me.removeContextMenuStripButton})
        Me.fileContextMenuStrip.Location = New System.Drawing.Point(32, 77)
        Me.fileContextMenuStrip.Name = "fileContextMenuStrip"
        Me.fileContextMenuStrip.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.fileContextMenuStrip.Size = New System.Drawing.Size(103, 48)
        '
        'contextMenuStripButton
        '
        Me.contextMenuStripButton.Name = "contextMenuStripButton"
        Me.contextMenuStripButton.Text = "Extract"
        '
        'removeContextMenuStripButton
        '
        Me.removeContextMenuStripButton.Name = "removeContextMenuStripButton"
        Me.removeContextMenuStripButton.Text = "Remove"
        '
        'CompressionForm
        '
        Me.ClientSize = New System.Drawing.Size(787, 566)
        Me.Controls.Add(Me.fileListView)
        Me.Controls.Add(Me.mainStatusStrip)
        Me.Controls.Add(Me.mainMenuStrip)
        Me.Name = "CompressionForm"
        Me.Padding = New System.Windows.Forms.Padding(9)
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "CompressionForm"
        Me.mainToolStrip.ResumeLayout(False)
        Me.mainMenuStrip.ResumeLayout(False)
        Me.fileContextMenuStrip.ResumeLayout(False)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub 'InitializeComponent


    '/ <summary>
    '/ Clean up any resources being used.
    '/ </summary>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If Not (archive Is Nothing) Then
            archive.Close()
        End If
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)

    End Sub 'Dispose

    Private mainToolStrip As System.Windows.Forms.ToolStrip
    Private mainStatusStrip As System.Windows.Forms.StatusStrip
    Private WithEvents fileListView As System.Windows.Forms.ListView
    Private WithEvents openToolStripButton As System.Windows.Forms.ToolStripButton
    Private WithEvents addToolStripButton As System.Windows.Forms.ToolStripButton
    Private WithEvents extractToolStripButton As System.Windows.Forms.ToolStripButton
    Private addToolStripSeparator As System.Windows.Forms.ToolStripSeparator
    Private newToolStripSeparator As System.Windows.Forms.ToolStripSeparator
    Private Shadows mainMenuStrip As System.Windows.Forms.MenuStrip
    Private actionsToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Private fileToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Private WithEvents openMenuStripButton As System.Windows.Forms.ToolStripMenuItem
    Private WithEvents closeMenuStripButton As System.Windows.Forms.ToolStripMenuItem
    Private WithEvents addMenuStripButton As System.Windows.Forms.ToolStripMenuItem
    Private WithEvents removeMenuStripButton As System.Windows.Forms.ToolStripMenuItem
    Private WithEvents extractMenuStripButton As System.Windows.Forms.ToolStripMenuItem
    Private WithEvents extractAllMenuStripButton As System.Windows.Forms.ToolStripMenuItem
    Private toolStripSeparator1 As System.Windows.Forms.ToolStripSeparator
    Private WithEvents newGzipToolStripButton As System.Windows.Forms.ToolStripButton
    Private WithEvents newDeflateToolStripButton As System.Windows.Forms.ToolStripButton
    Private WithEvents newGzipMenuToolStripButton As System.Windows.Forms.ToolStripMenuItem
    Private WithEvents newDeflateMenuToolStripButton As System.Windows.Forms.ToolStripMenuItem
    Private WithEvents fileContextMenuStrip As System.Windows.Forms.ContextMenuStrip
    Private WithEvents contextMenuStripButton As System.Windows.Forms.ToolStripMenuItem
    Private WithEvents removeContextMenuStripButton As System.Windows.Forms.ToolStripMenuItem
    Private WithEvents removeToolStripButton As System.Windows.Forms.ToolStripButton
End Class 'CompressionSample 

