<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class Books
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
    Private mainMenu1 As System.Windows.Forms.MainMenu

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.mainMenu1 = New System.Windows.Forms.MainMenu
        Me.mnuExit = New System.Windows.Forms.MenuItem
        Me.tpGroupView = New System.Windows.Forms.TabPage
        Me.cbGroupBy = New System.Windows.Forms.ComboBox
        Me.label2 = New System.Windows.Forms.Label
        Me.btnGo = New System.Windows.Forms.Button
        Me.wbGroupByResults = New System.Windows.Forms.WebBrowser
        Me.btnSearch = New System.Windows.Forms.Button
        Me.mainMenu = New System.Windows.Forms.MainMenu
        Me.miExit = New System.Windows.Forms.MenuItem
        Me.tpUnifiedSearch = New System.Windows.Forms.TabPage
        Me.cbUnifiedSearch = New System.Windows.Forms.ComboBox
        Me.wbUnifiedSearchResults = New System.Windows.Forms.WebBrowser
        Me.btnAction = New System.Windows.Forms.Button
        Me.lblMessage = New System.Windows.Forms.Label
        Me.lblBookSearchCount = New System.Windows.Forms.Label
        Me.dgResults = New System.Windows.Forms.DataGrid
        Me.dgTableStyle = New System.Windows.Forms.DataGridTableStyle
        Me.dgcolumnTitle = New System.Windows.Forms.DataGridTextBoxColumn
        Me.dgcolumnAuthor = New System.Windows.Forms.DataGridTextBoxColumn
        Me.dgcolumnPrice = New System.Windows.Forms.DataGridTextBoxColumn
        Me.tpBookSearch = New System.Windows.Forms.TabPage
        Me.label1 = New System.Windows.Forms.Label
        Me.txtBookName = New System.Windows.Forms.TextBox
        Me.tabControl = New System.Windows.Forms.TabControl
        Me.tpGroupView.SuspendLayout()
        Me.tpUnifiedSearch.SuspendLayout()
        Me.tpBookSearch.SuspendLayout()
        Me.tabControl.SuspendLayout()
        Me.SuspendLayout()
        '
        'mainMenu1
        '
        Me.mainMenu1.MenuItems.Add(Me.mnuExit)
        '
        'mnuExit
        '
        Me.mnuExit.Text = "Exit"
        '
        'tpGroupView
        '
        Me.tpGroupView.Controls.Add(Me.cbGroupBy)
        Me.tpGroupView.Controls.Add(Me.label2)
        Me.tpGroupView.Controls.Add(Me.btnGo)
        Me.tpGroupView.Controls.Add(Me.wbGroupByResults)
        Me.tpGroupView.Location = New System.Drawing.Point(0, 0)
        Me.tpGroupView.Name = "tpGroupView"
        Me.tpGroupView.Size = New System.Drawing.Size(240, 245)
        Me.tpGroupView.Text = "Group View"
        '
        'cbGroupBy
        '
        Me.cbGroupBy.Items.Add("Author")
        Me.cbGroupBy.Items.Add("Subject")
        Me.cbGroupBy.Items.Add("Style")
        Me.cbGroupBy.Location = New System.Drawing.Point(71, 14)
        Me.cbGroupBy.Name = "cbGroupBy"
        Me.cbGroupBy.Size = New System.Drawing.Size(121, 22)
        Me.cbGroupBy.TabIndex = 4
        '
        'label2
        '
        Me.label2.Font = New System.Drawing.Font("Tahoma", 8.0!, System.Drawing.FontStyle.Bold)
        Me.label2.Location = New System.Drawing.Point(4, 16)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(61, 20)
        Me.label2.Text = "Group by"
        '
        'btnGo
        '
        Me.btnGo.Location = New System.Drawing.Point(198, 14)
        Me.btnGo.Name = "btnGo"
        Me.btnGo.Size = New System.Drawing.Size(39, 20)
        Me.btnGo.TabIndex = 1
        Me.btnGo.Text = "Go"
        '
        'wbGroupByResults
        '
        Me.wbGroupByResults.Location = New System.Drawing.Point(0, 58)
        Me.wbGroupByResults.Name = "wbGroupByResults"
        Me.wbGroupByResults.Size = New System.Drawing.Size(240, 187)
        '
        'btnSearch
        '
        Me.btnSearch.Location = New System.Drawing.Point(7, 31)
        Me.btnSearch.Name = "btnSearch"
        Me.btnSearch.Size = New System.Drawing.Size(225, 20)
        Me.btnSearch.TabIndex = 2
        Me.btnSearch.Text = "Search"
        '
        'mainMenu
        '
        Me.mainMenu.MenuItems.Add(Me.miExit)
        '
        'miExit
        '
        Me.miExit.Text = "Exit"
        '
        'tpUnifiedSearch
        '
        Me.tpUnifiedSearch.Controls.Add(Me.cbUnifiedSearch)
        Me.tpUnifiedSearch.Controls.Add(Me.wbUnifiedSearchResults)
        Me.tpUnifiedSearch.Controls.Add(Me.btnAction)
        Me.tpUnifiedSearch.Controls.Add(Me.lblMessage)
        Me.tpUnifiedSearch.Location = New System.Drawing.Point(0, 0)
        Me.tpUnifiedSearch.Name = "tpUnifiedSearch"
        Me.tpUnifiedSearch.Size = New System.Drawing.Size(232, 242)
        Me.tpUnifiedSearch.Text = "Unified Search"
        '
        'cbUnifiedSearch
        '
        Me.cbUnifiedSearch.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDown
        Me.cbUnifiedSearch.Location = New System.Drawing.Point(8, 30)
        Me.cbUnifiedSearch.Name = "cbUnifiedSearch"
        Me.cbUnifiedSearch.Size = New System.Drawing.Size(216, 22)
        Me.cbUnifiedSearch.TabIndex = 6
        '
        'wbUnifiedSearchResults
        '
        Me.wbUnifiedSearchResults.Location = New System.Drawing.Point(0, 83)
        Me.wbUnifiedSearchResults.Name = "wbUnifiedSearchResults"
        Me.wbUnifiedSearchResults.Size = New System.Drawing.Size(240, 159)
        '
        'btnAction
        '
        Me.btnAction.Location = New System.Drawing.Point(8, 57)
        Me.btnAction.Name = "btnAction"
        Me.btnAction.Size = New System.Drawing.Size(216, 20)
        Me.btnAction.TabIndex = 2
        Me.btnAction.Text = "btnAction"
        '
        'lblMessage
        '
        Me.lblMessage.Location = New System.Drawing.Point(8, 8)
        Me.lblMessage.Name = "lblMessage"
        Me.lblMessage.Size = New System.Drawing.Size(216, 18)
        Me.lblMessage.Text = "Type in keywords"
        '
        'lblBookSearchCount
        '
        Me.lblBookSearchCount.Location = New System.Drawing.Point(8, 54)
        Me.lblBookSearchCount.Name = "lblBookSearchCount"
        Me.lblBookSearchCount.Size = New System.Drawing.Size(224, 20)
        Me.lblBookSearchCount.Text = "lblRecCount"
        Me.lblBookSearchCount.Visible = False
        '
        'dgResults
        '
        Me.dgResults.BackgroundColor = System.Drawing.Color.FromArgb(CType(CType(128, Byte), Integer), CType(CType(128, Byte), Integer), CType(CType(128, Byte), Integer))
        Me.dgResults.Location = New System.Drawing.Point(8, 80)
        Me.dgResults.Name = "dgResults"
        Me.dgResults.Size = New System.Drawing.Size(225, 162)
        Me.dgResults.TabIndex = 3
        Me.dgResults.TableStyles.Add(Me.dgTableStyle)
        Me.dgResults.Visible = False
        '
        'dgTableStyle
        '
        Me.dgTableStyle.GridColumnStyles.Add(Me.dgcolumnTitle)
        Me.dgTableStyle.GridColumnStyles.Add(Me.dgcolumnAuthor)
        Me.dgTableStyle.GridColumnStyles.Add(Me.dgcolumnPrice)
        '
        'dgcolumnTitle
        '
        Me.dgcolumnTitle.Format = ""
        Me.dgcolumnTitle.FormatInfo = Nothing
        Me.dgcolumnTitle.HeaderText = "Title"
        Me.dgcolumnTitle.MappingName = "Title"
        Me.dgcolumnTitle.Width = 100
        '
        'dgcolumnAuthor
        '
        Me.dgcolumnAuthor.Format = ""
        Me.dgcolumnAuthor.FormatInfo = Nothing
        Me.dgcolumnAuthor.HeaderText = "Author"
        Me.dgcolumnAuthor.MappingName = "Author"
        '
        'dgcolumnPrice
        '
        Me.dgcolumnPrice.Format = "c"
        Me.dgcolumnPrice.FormatInfo = Nothing
        Me.dgcolumnPrice.HeaderText = "Price"
        Me.dgcolumnPrice.MappingName = "Price"
        '
        'tpBookSearch
        '
        Me.tpBookSearch.Controls.Add(Me.lblBookSearchCount)
        Me.tpBookSearch.Controls.Add(Me.dgResults)
        Me.tpBookSearch.Controls.Add(Me.btnSearch)
        Me.tpBookSearch.Controls.Add(Me.label1)
        Me.tpBookSearch.Controls.Add(Me.txtBookName)
        Me.tpBookSearch.Location = New System.Drawing.Point(0, 0)
        Me.tpBookSearch.Name = "tpBookSearch"
        Me.tpBookSearch.Size = New System.Drawing.Size(240, 245)
        Me.tpBookSearch.Text = "Book Search"
        '
        'label1
        '
        Me.label1.Location = New System.Drawing.Point(8, 8)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(42, 20)
        Me.label1.Text = "Title"
        '
        'txtBookName
        '
        Me.txtBookName.Location = New System.Drawing.Point(56, 7)
        Me.txtBookName.Name = "txtBookName"
        Me.txtBookName.Size = New System.Drawing.Size(177, 21)
        Me.txtBookName.TabIndex = 0
        '
        'tabControl
        '
        Me.tabControl.Controls.Add(Me.tpBookSearch)
        Me.tabControl.Controls.Add(Me.tpGroupView)
        Me.tabControl.Controls.Add(Me.tpUnifiedSearch)
        Me.tabControl.Location = New System.Drawing.Point(0, 0)
        Me.tabControl.Name = "tabControl"
        Me.tabControl.SelectedIndex = 0
        Me.tabControl.Size = New System.Drawing.Size(240, 268)
        Me.tabControl.TabIndex = 1
        '
        'Books
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(96.0!, 96.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(240, 268)
        Me.Controls.Add(Me.tabControl)
        Me.Menu = Me.mainMenu
        Me.Name = "Books"
        Me.Text = "Books"
        Me.tpGroupView.ResumeLayout(False)
        Me.tpUnifiedSearch.ResumeLayout(False)
        Me.tpBookSearch.ResumeLayout(False)
        Me.tabControl.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents tpGroupView As System.Windows.Forms.TabPage
    Private WithEvents cbGroupBy As System.Windows.Forms.ComboBox
    Private WithEvents label2 As System.Windows.Forms.Label
    Private WithEvents btnGo As System.Windows.Forms.Button
    Private WithEvents wbGroupByResults As System.Windows.Forms.WebBrowser
    Private WithEvents btnSearch As System.Windows.Forms.Button
    Private WithEvents mainMenu As System.Windows.Forms.MainMenu
    Private WithEvents miExit As System.Windows.Forms.MenuItem
    Private WithEvents tpUnifiedSearch As System.Windows.Forms.TabPage
    Private WithEvents cbUnifiedSearch As System.Windows.Forms.ComboBox
    Private WithEvents wbUnifiedSearchResults As System.Windows.Forms.WebBrowser
    Private WithEvents btnAction As System.Windows.Forms.Button
    Private WithEvents lblMessage As System.Windows.Forms.Label
    Private WithEvents lblBookSearchCount As System.Windows.Forms.Label
    Private WithEvents dgResults As System.Windows.Forms.DataGrid
    Private WithEvents dgTableStyle As System.Windows.Forms.DataGridTableStyle
    Private WithEvents dgcolumnTitle As System.Windows.Forms.DataGridTextBoxColumn
    Private WithEvents dgcolumnAuthor As System.Windows.Forms.DataGridTextBoxColumn
    Private WithEvents dgcolumnPrice As System.Windows.Forms.DataGridTextBoxColumn
    Private WithEvents tpBookSearch As System.Windows.Forms.TabPage
    Private WithEvents label1 As System.Windows.Forms.Label
    Private WithEvents txtBookName As System.Windows.Forms.TextBox
    Private WithEvents tabControl As System.Windows.Forms.TabControl
    Friend WithEvents mnuExit As System.Windows.Forms.MenuItem

End Class
