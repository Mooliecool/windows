'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'-----------------------------------------------------------------------
Option Explicit On
Imports System.IO
Imports System.Globalization
Imports System.Diagnostics
Imports System.Threading
Imports System.Windows.Forms
Imports System.Collections.Generic
Namespace Microsoft.Samples.FileDemo
    Public NotInheritable Class FormMain
        Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

        Public Sub New()
            MyBase.New()

            'This call is required by the Windows Form Designer.
            InitializeComponent()

            'Add any initialization after the InitializeComponent() call

        End Sub

        'Form overrides dispose to clean up the component list.
        Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
            If disposing Then
                If Not (components Is Nothing) Then
                    components.Dispose()
                End If
            End If
            MyBase.Dispose(disposing)
        End Sub

        'Required by the Windows Form Designer
        Private components As System.ComponentModel.IContainer

        'NOTE: The following procedure is required by the Windows Form Designer
        'It can be modified using the Windows Form Designer.  
        'Do not modify it using the code editor.
        Friend WithEvents treeMain As System.Windows.Forms.TreeView
        Friend WithEvents columnName As System.Windows.Forms.ColumnHeader
        Friend WithEvents columnSize As System.Windows.Forms.ColumnHeader
        Friend WithEvents columnType As System.Windows.Forms.ColumnHeader
        Friend WithEvents columnDateModified As System.Windows.Forms.ColumnHeader
        Friend WithEvents listMain As System.Windows.Forms.ListView
        Friend WithEvents contextTree As System.Windows.Forms.ContextMenu
        Friend WithEvents ctProperties As System.Windows.Forms.MenuItem
        Friend WithEvents miExplore As System.Windows.Forms.MenuItem
        Friend WithEvents miOpen As System.Windows.Forms.MenuItem
        Friend WithEvents miSearch As System.Windows.Forms.MenuItem
        Friend WithEvents miManage As System.Windows.Forms.MenuItem
        Friend WithEvents miSep1 As System.Windows.Forms.MenuItem
        Friend WithEvents miMapNetwork As System.Windows.Forms.MenuItem
        Friend WithEvents miDisconnectNetwork As System.Windows.Forms.MenuItem
        Friend WithEvents miSharing As System.Windows.Forms.MenuItem
        Friend WithEvents miSep2 As System.Windows.Forms.MenuItem
        Friend WithEvents miDelete As System.Windows.Forms.MenuItem
        Friend WithEvents miRename As System.Windows.Forms.MenuItem
        Friend WithEvents miFormat As System.Windows.Forms.MenuItem
        Friend WithEvents miSep3 As System.Windows.Forms.MenuItem
        Friend WithEvents miCopy As System.Windows.Forms.MenuItem
        Friend WithEvents miSep4 As System.Windows.Forms.MenuItem
        Friend WithEvents miRename2 As System.Windows.Forms.MenuItem
        Friend WithEvents miSep5 As System.Windows.Forms.MenuItem
        Friend WithEvents miProperties As System.Windows.Forms.MenuItem
        Friend WithEvents contextList As System.Windows.Forms.ContextMenu
        Friend WithEvents menuNew As System.Windows.Forms.MenuItem
        Friend WithEvents menuNewFolder As System.Windows.Forms.MenuItem
        Friend WithEvents menuSepNew As System.Windows.Forms.MenuItem
        Friend WithEvents menuNewTextDocument As System.Windows.Forms.MenuItem
        Friend WithEvents ViewMenuItem As System.Windows.Forms.MenuItem
        Friend WithEvents TilesMenuItem As System.Windows.Forms.MenuItem
        Friend WithEvents IconsMenuItem As System.Windows.Forms.MenuItem
        Friend WithEvents ListMenuItem As System.Windows.Forms.MenuItem
        Friend WithEvents DetailsMenuItem As System.Windows.Forms.MenuItem
        Friend WithEvents eventLogMain As System.Diagnostics.EventLog
        Friend WithEvents timerMain As System.Timers.Timer
        Friend WithEvents menuSecurity As System.Windows.Forms.MenuItem
        Friend WithEvents MenuItem2 As System.Windows.Forms.MenuItem
        Friend WithEvents MenuItem3 As System.Windows.Forms.MenuItem
        Friend WithEvents menuProperties As System.Windows.Forms.MenuItem
        Private ilMain As ImageList

        <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
            Me.treeMain = New System.Windows.Forms.TreeView
            Me.contextTree = New System.Windows.Forms.ContextMenu
            Me.miExplore = New System.Windows.Forms.MenuItem
            Me.miOpen = New System.Windows.Forms.MenuItem
            Me.miSearch = New System.Windows.Forms.MenuItem
            Me.miManage = New System.Windows.Forms.MenuItem
            Me.miSep1 = New System.Windows.Forms.MenuItem
            Me.miMapNetwork = New System.Windows.Forms.MenuItem
            Me.miDisconnectNetwork = New System.Windows.Forms.MenuItem
            Me.miSharing = New System.Windows.Forms.MenuItem
            Me.miSep2 = New System.Windows.Forms.MenuItem
            Me.miDelete = New System.Windows.Forms.MenuItem
            Me.miRename = New System.Windows.Forms.MenuItem
            Me.miFormat = New System.Windows.Forms.MenuItem
            Me.miSep3 = New System.Windows.Forms.MenuItem
            Me.miCopy = New System.Windows.Forms.MenuItem
            Me.miSep4 = New System.Windows.Forms.MenuItem
            Me.miRename2 = New System.Windows.Forms.MenuItem
            Me.miSep5 = New System.Windows.Forms.MenuItem
            Me.miProperties = New System.Windows.Forms.MenuItem
            Me.listMain = New System.Windows.Forms.ListView
            Me.columnName = New System.Windows.Forms.ColumnHeader
            Me.columnSize = New System.Windows.Forms.ColumnHeader
            Me.columnType = New System.Windows.Forms.ColumnHeader
            Me.columnDateModified = New System.Windows.Forms.ColumnHeader
            Me.contextList = New System.Windows.Forms.ContextMenu
            Me.menuSecurity = New System.Windows.Forms.MenuItem
            Me.MenuItem2 = New System.Windows.Forms.MenuItem
            Me.ViewMenuItem = New System.Windows.Forms.MenuItem
            Me.TilesMenuItem = New System.Windows.Forms.MenuItem
            Me.IconsMenuItem = New System.Windows.Forms.MenuItem
            Me.ListMenuItem = New System.Windows.Forms.MenuItem
            Me.DetailsMenuItem = New System.Windows.Forms.MenuItem
            Me.menuNew = New System.Windows.Forms.MenuItem
            Me.menuNewFolder = New System.Windows.Forms.MenuItem
            Me.menuSepNew = New System.Windows.Forms.MenuItem
            Me.menuNewTextDocument = New System.Windows.Forms.MenuItem
            Me.MenuItem3 = New System.Windows.Forms.MenuItem
            Me.menuProperties = New System.Windows.Forms.MenuItem
            Me.eventLogMain = New System.Diagnostics.EventLog
            Me.timerMain = New System.Timers.Timer
            CType(Me.eventLogMain, System.ComponentModel.ISupportInitialize).BeginInit()
            CType(Me.timerMain, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.SuspendLayout()
            '
            'treeMain
            '
            Me.treeMain.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                        Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
            Me.treeMain.ContextMenu = Me.contextTree
            Me.treeMain.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.treeMain.HideSelection = False
            Me.treeMain.ImageIndex = -1
            Me.treeMain.Location = New System.Drawing.Point(0, 0)
            Me.treeMain.Name = "treeMain"
            Me.treeMain.SelectedImageIndex = -1
            Me.treeMain.Size = New System.Drawing.Size(252, 459)
            Me.treeMain.TabIndex = 0
            '
            'contextTree
            '
            Me.contextTree.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.miExplore, Me.miOpen, Me.miSearch, Me.miManage, Me.miSep1, Me.miMapNetwork, Me.miDisconnectNetwork, Me.miSharing, Me.miSep2, Me.miDelete, Me.miRename, Me.miFormat, Me.miSep3, Me.miCopy, Me.miSep4, Me.miRename2, Me.miSep5, Me.miProperties})
            Me.contextTree.Name = "contextTree"
            '
            'miExplore
            '
            Me.miExplore.Index = 0
            Me.miExplore.Name = "miExplore"
            Me.miExplore.Text = "Explore"
            '
            'miOpen
            '
            Me.miOpen.Index = 1
            Me.miOpen.Name = "miOpen"
            Me.miOpen.Text = "Open"
            '
            'miSearch
            '
            Me.miSearch.Index = 2
            Me.miSearch.Name = "miSearch"
            Me.miSearch.Text = "Search..."
            '
            'miManage
            '
            Me.miManage.Index = 3
            Me.miManage.Name = "miManage"
            Me.miManage.Text = "Manage"
            '
            'miSep1
            '
            Me.miSep1.Index = 4
            Me.miSep1.Name = "miSep1"
            Me.miSep1.Text = "-"
            '
            'miMapNetwork
            '
            Me.miMapNetwork.Index = 5
            Me.miMapNetwork.Name = "miMapNetwork"
            Me.miMapNetwork.Text = "Map Network Drive"
            '
            'miDisconnectNetwork
            '
            Me.miDisconnectNetwork.Index = 6
            Me.miDisconnectNetwork.Name = "miDisconnectNetwork"
            Me.miDisconnectNetwork.Text = "Disconnect Network Drive"
            '
            'miSharing
            '
            Me.miSharing.Index = 7
            Me.miSharing.Name = "miSharing"
            Me.miSharing.Text = "Sharing and Security..."
            '
            'miSep2
            '
            Me.miSep2.Index = 8
            Me.miSep2.Name = "miSep2"
            Me.miSep2.Text = "-"
            '
            'miDelete
            '
            Me.miDelete.Index = 9
            Me.miDelete.Name = "miDelete"
            Me.miDelete.Text = "Delete"
            '
            'miRename
            '
            Me.miRename.Index = 10
            Me.miRename.Name = "miRename"
            Me.miRename.Text = "Rename"
            '
            'miFormat
            '
            Me.miFormat.Index = 11
            Me.miFormat.Name = "miFormat"
            Me.miFormat.Text = "Format..."
            '
            'miSep3
            '
            Me.miSep3.Index = 12
            Me.miSep3.Name = "miSep3"
            Me.miSep3.Text = "-"
            '
            'miCopy
            '
            Me.miCopy.Index = 13
            Me.miCopy.Name = "miCopy"
            Me.miCopy.Text = "Copy"
            '
            'miSep4
            '
            Me.miSep4.Index = 14
            Me.miSep4.Name = "miSep4"
            Me.miSep4.Text = "-"
            '
            'miRename2
            '
            Me.miRename2.Index = 15
            Me.miRename2.Name = "miRename2"
            Me.miRename2.Text = "Rename"
            '
            'miSep5
            '
            Me.miSep5.Index = 16
            Me.miSep5.Name = "miSep5"
            Me.miSep5.Text = "-"
            '
            'miProperties
            '
            Me.miProperties.Index = 17
            Me.miProperties.Name = "miProperties"
            Me.miProperties.Text = "Properties"
            '
            'listMain
            '
            Me.listMain.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                        Or System.Windows.Forms.AnchorStyles.Left) _
                        Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
            Me.listMain.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.columnName, Me.columnSize, Me.columnType, Me.columnDateModified})
            Me.listMain.ContextMenu = Me.contextList
            Me.listMain.Font = New System.Drawing.Font("Microsoft Sans Serif", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.listMain.HideSelection = False
            Me.listMain.Location = New System.Drawing.Point(259, 0)
            Me.listMain.MultiSelect = False
            Me.listMain.Name = "listMain"
            Me.listMain.Size = New System.Drawing.Size(472, 459)
            Me.listMain.TabIndex = 1
            Me.listMain.View = System.Windows.Forms.View.Details
            '
            'contextList
            '
            Me.contextList.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.menuSecurity, Me.MenuItem2, Me.ViewMenuItem, Me.menuNew, Me.MenuItem3, Me.menuProperties})
            Me.contextList.Name = "contextList"
            '
            'menuSecurity
            '
            Me.menuSecurity.Index = 0
            Me.menuSecurity.Name = "menuSecurity"
            Me.menuSecurity.Text = "Security"
            '
            'MenuItem2
            '
            Me.MenuItem2.Index = 1
            Me.MenuItem2.Name = "MenuItem2"
            Me.MenuItem2.Text = "-"
            '
            'ViewMenuItem
            '
            Me.ViewMenuItem.Index = 2
            Me.ViewMenuItem.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.TilesMenuItem, Me.IconsMenuItem, Me.ListMenuItem, Me.DetailsMenuItem})
            Me.ViewMenuItem.Name = "ViewMenuItem"
            Me.ViewMenuItem.Text = "View"
            '
            'TilesMenuItem
            '
            Me.TilesMenuItem.Index = 0
            Me.TilesMenuItem.Name = "TilesMenuItem"
            Me.TilesMenuItem.Text = "Tiles"
            Me.TilesMenuItem.Visible = False
            '
            'IconsMenuItem
            '
            Me.IconsMenuItem.Index = 1
            Me.IconsMenuItem.Name = "IconsMenuItem"
            Me.IconsMenuItem.Text = "Icons"
            '
            'ListMenuItem
            '
            Me.ListMenuItem.Index = 2
            Me.ListMenuItem.Name = "ListMenuItem"
            Me.ListMenuItem.Text = "List"
            '
            'DetailsMenuItem
            '
            Me.DetailsMenuItem.Index = 3
            Me.DetailsMenuItem.Name = "DetailsMenuItem"
            Me.DetailsMenuItem.Text = "Details"
            '
            'menuNew
            '
            Me.menuNew.Index = 3
            Me.menuNew.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.menuNewFolder, Me.menuSepNew, Me.menuNewTextDocument})
            Me.menuNew.Name = "menuNew"
            Me.menuNew.Text = "New"
            '
            'menuNewFolder
            '
            Me.menuNewFolder.Index = 0
            Me.menuNewFolder.Name = "menuNewFolder"
            Me.menuNewFolder.Text = "Folder"
            '
            'menuSepNew
            '
            Me.menuSepNew.Index = 1
            Me.menuSepNew.Name = "menuSepNew"
            Me.menuSepNew.Text = "-"
            '
            'menuNewTextDocument
            '
            Me.menuNewTextDocument.Index = 2
            Me.menuNewTextDocument.Name = "menuNewTextDocument"
            Me.menuNewTextDocument.Text = "Text Document"
            '
            'MenuItem3
            '
            Me.MenuItem3.Index = 4
            Me.MenuItem3.Name = "MenuItem3"
            Me.MenuItem3.Text = "-"
            '
            'menuProperties
            '
            Me.menuProperties.Index = 5
            Me.menuProperties.Name = "menuProperties"
            Me.menuProperties.Text = "Properties"
            '
            'eventLogMain
            '
            Me.eventLogMain.Log = "ManagedExplorer"
            Me.eventLogMain.Source = "ManagedExplorer"
            Me.eventLogMain.SynchronizingObject = Me
            '
            'timerMain
            '
            Me.timerMain.Interval = 200
            Me.timerMain.SynchronizingObject = Me
            '
            'FormMain
            '
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.ClientSize = New System.Drawing.Size(738, 466)
            Me.Controls.Add(Me.listMain)
            Me.Controls.Add(Me.treeMain)
            Me.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
            Me.Name = "FormMain"
            Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
            Me.Text = "Managed Explorer"
            CType(Me.eventLogMain, System.ComponentModel.ISupportInitialize).EndInit()
            CType(Me.timerMain, System.ComponentModel.ISupportInitialize).EndInit()
            Me.ResumeLayout(False)

        End Sub

#End Region

        Private loaded As Boolean
        Private processRepeating As Boolean
        Private oldName As String
        Private oldAbsoluteDirName As String
        Private newFolder As Boolean
        Private newFile As Boolean
        Private removedAlready As Long

        Private Sub FormMain_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
            timerMain.Enabled = True
        End Sub
        Private Sub RunSetupCode()
            treeMain.Nodes.Clear()
            PopulateList()

            Dim drives() As DriveInfo = DriveInfo.GetDrives()

            SetupListView()

            oldName = ""
            Dim mainNode As TreeNode = treeMain.Nodes.Add("Desktop")
            mainNode.ImageIndex = 1
            mainNode.SelectedImageIndex = 1
            Dim myComputer As TreeNode = mainNode.Nodes.Add("My Computer")
            myComputer.ImageIndex = 6
            myComputer.SelectedImageIndex = 6

            For Each d As DriveInfo In drives
                Dim topNode As TreeNode = myComputer.Nodes.Add(GetNameAndType(d))
                topNode.Tag = d.Name
                topNode.ImageIndex = GetDriveIndex(d)
                topNode.SelectedImageIndex = topNode.ImageIndex

                If d.IsReady Then
                    AddSubNodes(topNode, d.Name, True)
                End If
            Next

            If treeMain.Nodes.Count > 0 Then
                treeMain.SelectedNode = treeMain.Nodes(0)
                If Not treeMain.SelectedNode Is Nothing Then
                    treeMain.SelectedNode.Expand()
                End If
            End If

        End Sub

        Private Sub PopulateList()
            ilMain = New ImageList()
            ilMain.Images.Add(New Bitmap("..\cddrive.bmp"))
            ilMain.Images.Add(New Bitmap("..\controlpanel.bmp"))
            ilMain.Images.Add(New Bitmap("..\floppydrive.bmp"))
            ilMain.Images.Add(New Bitmap("..\localdrive.bmp"))
            ilMain.Images.Add(New Bitmap("..\folderclosed.bmp"))
            ilMain.Images.Add(New Bitmap("..\network.bmp"))
            ilMain.Images.Add(New Bitmap("..\mycomputer.bmp"))
            ilMain.Images.Add(New Bitmap("..\recycle.bmp"))
            ilMain.Images.Add(New Bitmap("..\folderopen.bmp"))
            ilMain.Images.Add(New Bitmap("..\textfile.bmp"))

            listMain.SmallImageList = ilMain
            treeMain.ImageList = ilMain
        End Sub

        Private Sub AddSubNodes(ByVal node As TreeNode, ByVal dirName As String, ByVal iterate As Boolean)
            Try
                For Each dir As String In Directory.GetDirectories(dirName)
                    'check it doesn't already exist
                    Dim found As Boolean
                    For Each tn As TreeNode In node.Nodes
                        If tn.Text = dir.Substring(dir.LastIndexOf("\") + 1) Then
                            found = True
                            Exit For
                        End If
                    Next
                    If Not found Then
                        Dim subNode As TreeNode = node.Nodes.Add(dir.Substring(dir.LastIndexOf("\") + 1))
                        subNode.ImageIndex = 4
                        subNode.SelectedImageIndex = 4
                        subNode.Tag = dir
                        If iterate Then
                            AddSubNodes(subNode, dir, False)
                        End If
                    End If
                Next
            Catch uae As UnauthorizedAccessException
                'If Not loaded Then
                '    'simply(ignore)
                'End If
            Finally
            End Try

        End Sub

        Private Sub treeMain_AfterExpand(ByVal sender As Object, ByVal e As System.Windows.Forms.TreeViewEventArgs) Handles treeMain.AfterExpand
            If Not e.Node.Parent Is Nothing Then
                If e.Node.Parent.Text <> "Desktop" Then
                    For Each tn As TreeNode In e.Node.Nodes
                        AddSubNodes(tn, tn.Tag.ToString(), False)
                    Next
                End If
            End If
        End Sub

        Private Sub treeMain_AfterSelect(ByVal sender As System.Object, ByVal e As System.Windows.Forms.TreeViewEventArgs) Handles treeMain.AfterSelect
            listMain.LabelEdit = False

            If Not e.Node.Parent Is Nothing Then
                If e.Node.Parent.Text <> "Desktop" Then
                    If loaded Then
                        Dim d As DriveInfo = New DriveInfo(e.Node.Tag.ToString())

                        If Not d.IsReady Then
                            MsgBox("Please insert a disk into drive " + d.Name, MsgBoxStyle.OKOnly, "Insert Disk")
                            listMain.Items.Clear()
                            Exit Sub
                        End If
                    Else
                        Exit Sub
                    End If
                    listMain.LabelEdit = True
                    FillList(e.Node)
                Else
                    listMain.Items.Clear()
                    For Each node As TreeNode In e.Node.Nodes
                        Dim lvi As New ListViewItem(node.Text)
                        lvi.ImageIndex = 0
                        listMain.Items.Add(lvi)
                    Next

                End If
            Else
                listMain.Items.Clear()
                Dim lvi As New ListViewItem("My Computer")
                lvi.ImageIndex = 6
                listMain.Items.Add(lvi)
            End If
        End Sub

        Private Sub FillList(ByVal tn As TreeNode)
            Dim lvi As ListViewItem
            listMain.Items.Clear()

            For Each dir As String In Directory.GetDirectories(tn.Tag.ToString())
                lvi = New ListViewItem
                lvi.Text = dir.Substring(dir.LastIndexOf("\") + 1)
                lvi.SubItems.Add("")
                lvi.SubItems.Add("File Folder")
                lvi.SubItems.Add(Directory.GetLastWriteTime(dir).ToString(String.Format(CultureInfo.CurrentCulture, "MM{0}dd{0}yyyy hh{1}mm tt", _
                            Thread.CurrentThread.CurrentCulture.DateTimeFormat.DateSeparator, _
                            Thread.CurrentThread.CurrentCulture.DateTimeFormat.TimeSeparator), CultureInfo.CurrentCulture))
                lvi.ImageIndex = 4
                listMain.Items.Add(lvi)
            Next

            For Each nextFile As String In Directory.GetFiles(tn.Tag.ToString())
                ''' hide system files, good just to protect that data...
                If Not ((File.GetAttributes(nextFile) And FileAttributes.System) = FileAttributes.System) And _
                        Not ((File.GetAttributes(nextFile) And FileAttributes.Hidden) = FileAttributes.Hidden) Then

                    Dim size As Long
                    size = CLng(New FileInfo(nextFile).Length)
                    If size > 0 And size < 1000 Then
                        size = 1
                    ElseIf size > 1000 Then
                        size = CLng(size / 1000)
                    End If

                    lvi = New ListViewItem
                    lvi.Text = Path.GetFileName(nextFile)
                    lvi.SubItems.Add(String.Format(CultureInfo.CurrentCulture, "{0:N0} KB", size))
                    lvi.SubItems.Add("File")
                    lvi.SubItems.Add(File.GetLastWriteTime(nextFile).ToString(String.Format(CultureInfo.CurrentCulture, "MM{0}dd{0}yyyy hh{1}mm tt", _
                                Thread.CurrentThread.CurrentCulture.DateTimeFormat.DateSeparator, _
                                Thread.CurrentThread.CurrentCulture.DateTimeFormat.TimeSeparator), CultureInfo.CurrentCulture))

                    Dim ext As String = Path.GetExtension(nextFile).ToLowerInvariant()
                    If ext = ".txt" Or ext = ".bat" Or ext = ".vb" Or ext = ".cs" Then
                        lvi.ImageIndex = 9
                    End If

                    listMain.Items.Add(lvi)
                End If
            Next
        End Sub

        Private Sub listMain_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles listMain.DoubleClick
            OpenSelectedItems()
        End Sub

        Private Sub OpenAsText(ByVal textFile As String)

            Dim tv As FormTextViewer = New FormTextViewer
            
            tv.FilePath = treeMain.SelectedNode.Tag.ToString() & _
                        "\" & textFile
            'OpenAndReadFile(tv)
            'new code for reading a file: one line!
            tv.textMain.Text = File.ReadAllText(tv.FilePath)

            ' show the form
            tv.Show()
        End Sub

        ' OLD Code for opening and reading contents of a text file
        ' notice the number of lines and complexity
        Private Sub OpenAndReadFile(ByVal Tv As FormTextViewer)
            With Tv
                Dim sr As StreamReader = New StreamReader(.FilePath)
                Try
                    While sr.Peek >= 0
                        .textMain.Text = .textMain.Text + sr.ReadLine() + Environment.NewLine
                    End While
                Finally
                    sr.Close()
                End Try
            End With
        End Sub

        Private Sub listMain_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles listMain.KeyDown
            If e.KeyCode = Keys.Delete Then
                If listMain.SelectedItems.Count = 1 Then
                    If listMain.SelectedItems(0).ImageIndex = 9 Then
                        If MsgBox("Are you sure you want to delete '" & _
                                listMain.SelectedItems(0).Text & "'?", _
                                MsgBoxStyle.YesNo And MsgBoxStyle.Question) = MsgBoxResult.Yes Then
                            Dim fName As String = treeMain.SelectedNode.Tag.ToString().Trim(Path.DirectorySeparatorChar) & Path.DirectorySeparatorChar & listMain.SelectedItems(0).Text
                            File.Delete(fName)
                            eventLogMain.WriteEvent(New EventInstance(105, DemoCategories.Deleted, EventLogEntryType.Warning), fName)

                            listMain.Items.Remove(listMain.SelectedItems(0))
                        End If
                    End If
                End If
            End If
        End Sub

        Private Sub listMain_KeyPress(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyPressEventArgs) Handles listMain.KeyPress
            If e.KeyChar = Chr(13) Then
                OpenSelectedItems()
            End If
        End Sub

        Private Sub OpenSelectedItems()
            For Each indice As Integer In listMain.SelectedIndices
                If listMain.Items(indice).ImageIndex = 9 Then
                    OpenAsText(listMain.Items(indice).Text)
                ElseIf listMain.Items(indice).ImageIndex = 8 Or listMain.Items(indice).ImageIndex = 4 Or _
                       listMain.Items(indice).ImageIndex = 0 Or listMain.Items(indice).ImageIndex = 6 Then
                    ExpandItem(listMain.Items(indice).Text)
                End If
            Next
        End Sub

        Private Sub ExpandItem(ByVal str As String)
            For Each n As TreeNode In treeMain.SelectedNode.Nodes
                If n.Text = str Then
                    treeMain.SelectedNode = n
                    Return
                End If
            Next
        End Sub

        Private Sub FormMain_Activated(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Activated
            loaded = True
        End Sub

        Private Sub contextTree_Popup(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles contextTree.Popup
            Dim IsADir As Boolean = True
            If Not treeMain.SelectedNode.Parent Is Nothing Then
                If treeMain.SelectedNode.Parent.Text = "Desktop" Then
                    IsADir = False
                End If
            End If
            miCopy.Visible = IsADir
            miManage.Visible = Not IsADir
            miMapNetwork.Visible = Not IsADir
            miDisconnectNetwork.Visible = Not IsADir
            miDelete.Visible = Not IsADir
            miRename.Visible = Not IsADir
            miRename2.Visible = IsADir
            miFormat.Visible = IsADir
            miSharing.Visible = IsADir
            miSep3.Visible = IsADir
            miSep4.Visible = IsADir
            miSep5.Visible = IsADir

        End Sub

        Private Sub SetupListView()
            listMain.Columns(0).Text = "Name"
            listMain.Columns(0).Width = CInt((listMain.Width * 0.41) - 5)

            listMain.Columns(1).Text = "Size"
            listMain.Columns(1).TextAlign = System.Windows.Forms.HorizontalAlignment.Right
            listMain.Columns(1).Width = CInt((listMain.Width * 0.15) - 5)

            listMain.Columns(2).Text = "Type"
            listMain.Columns(2).Width = CInt((listMain.Width * 0.16) - 5)

            listMain.Columns(3).Text = "Date Modified"
            listMain.Columns(3).Width = CInt((listMain.Width * 0.28) - 5)

        End Sub

        Private Sub miProperties_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles miProperties.Click
            If Not treeMain.SelectedNode.Tag Is Nothing Then
                curDir = treeMain.SelectedNode.Tag.ToString()
            Else
                curDir = ""
            End If

            If treeMain.SelectedNode.Parent Is Nothing Then
                'do nothing
            ElseIf treeMain.SelectedNode.Parent.Text = "My Computer" Then
                Dim props As DriveProperties = New DriveProperties
                props.ShowDialog()
            ElseIf treeMain.SelectedNode.Parent.Text = "Desktop" Then
                Dim props As EnvironVariables = New EnvironVariables
                props.ShowDialog()
            Else
                ShowFileDirectoryForm(curDir, FileItemType.Directory, DefaultPropertiesView.General, True)
            End If
        End Sub

        Private Sub menuNewFolder_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles menuNewFolder.Click
            Dim lvi As New ListViewItem
            lvi.Text = "New Folder"
            lvi.SubItems.Add("")
            lvi.SubItems.Add("File Folder")
            lvi.SubItems.Add(DateTime.Now.ToString(String.Format(CultureInfo.CurrentCulture, "MM{0}dd{0}yyyy hh{1}mm tt", _
                        Thread.CurrentThread.CurrentCulture.DateTimeFormat.DateSeparator, _
                        Thread.CurrentThread.CurrentCulture.DateTimeFormat.TimeSeparator), CultureInfo.CurrentCulture))
            lvi.ImageIndex = 4
            lvi.Focused = True
            newFolder = True
            listMain.Items.Add(lvi)
            lvi.BeginEdit()
        End Sub

        Private Sub listMain_AfterLabelEdit(ByVal sender As System.Object, ByVal e As System.Windows.Forms.LabelEditEventArgs) Handles listMain.AfterLabelEdit
            If e.Label = Nothing Then
                e.CancelEdit = True
                listMain.Items(e.Item).Selected = True
                oldName = ""
                Exit Sub
            End If

            For Each lvi As ListViewItem In listMain.Items
                If lvi.Index <> e.Item Then
                    If lvi.Text.ToLowerInvariant = e.Label.ToLowerInvariant Then
                        MsgBox(String.Format(CultureInfo.CurrentCulture, "A FileSystemObject with the name {0} already exists. Please choose another name", e.Label), MsgBoxStyle.Information, "Name Conflict")

                        processRepeating = (oldName = "new text document.txt" Or oldName = "")

                        e.CancelEdit = True
                        listMain.Items(e.Item).Selected = True
                        listMain.Items(e.Item).BeginEdit()
                        Exit Sub
                    End If
                End If
            Next

            ' else, add the item...
            If listMain.Items(e.Item).ImageIndex = 4 Then
                If oldName = "" Then
                    Dim dirName As String = treeMain.SelectedNode.Tag.ToString().Trim(Path.DirectorySeparatorChar) & Path.DirectorySeparatorChar & e.Label
                    Directory.CreateDirectory(dirName)
                    eventLogMain.WriteEvent(New EventInstance(101, DemoCategories.Created), dirName)
                    Dim tn As New TreeNode
                    tn.Text = e.Label
                    tn.ImageIndex = 4
                    tn.SelectedImageIndex = 4
                    tn.Tag = treeMain.SelectedNode.Tag.ToString().Trim(Path.DirectorySeparatorChar) & Path.DirectorySeparatorChar & e.Label
                    treeMain.SelectedNode.Nodes.Add(tn)
                    tn.Parent.Expand()
                Else
                    Dim newDir As String = treeMain.SelectedNode.Tag.ToString().Trim(Path.DirectorySeparatorChar) & Path.DirectorySeparatorChar & e.Label
                    Directory.Move(oldName, newDir)
                    eventLogMain.WriteEvent(New EventInstance(102, DemoCategories.Moved), oldName, newDir)

                    For Each tn As TreeNode In treeMain.SelectedNode.Nodes
                        If tn.Text.ToLowerInvariant = oldAbsoluteDirName Then
                            treeMain.SelectedNode.Nodes(tn.Index).Tag = treeMain.SelectedNode.Tag.ToString().Trim(Path.DirectorySeparatorChar) & Path.DirectorySeparatorChar & e.Label
                            treeMain.SelectedNode.Nodes(tn.Index).Text = e.Label
                            treeMain.SelectedNode.Expand()
                            Exit For
                        End If
                    Next
                End If
            ElseIf listMain.Items(e.Item).ImageIndex = 9 Then
                Dim newFileName As String = treeMain.SelectedNode.Tag.ToString().Trim(Path.DirectorySeparatorChar) & Path.DirectorySeparatorChar & e.Label
                If oldName = "" Then
                    Using sw As StreamWriter = File.CreateText(newFileName)
                        sw.Close()
                    End Using
                    eventLogMain.WriteEvent(New EventInstance(103, DemoCategories.Created), newFileName)
                Else
                    File.Move(oldName, newFileName)
                    eventLogMain.WriteEvent(New EventInstance(104, DemoCategories.Moved), oldName, newFileName)
                End If
            End If
        End Sub

        Private Sub menuNewTextDocument_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles menuNewTextDocument.Click
            Dim lvi As New ListViewItem
            lvi.Text = "New Text Document.txt"
            lvi.SubItems.Add(String.Format(CultureInfo.CurrentCulture, "{0:N0} KB", 0))
            lvi.SubItems.Add("File")
            lvi.SubItems.Add(DateTime.Now.ToString(String.Format(CultureInfo.CurrentCulture, "MM{0}dd{0}yyyy hh{1}mm tt", _
                        Thread.CurrentThread.CurrentCulture.DateTimeFormat.DateSeparator, _
                        Thread.CurrentThread.CurrentCulture.DateTimeFormat.TimeSeparator), CultureInfo.CurrentCulture))
            lvi.ImageIndex = 9
            lvi.Focused = True
            newFile = True
            listMain.Items.Add(lvi)
            lvi.BeginEdit()
        End Sub

        Private Sub listMain_BeforeLabelEdit(ByVal sender As System.Object, ByVal e As System.Windows.Forms.LabelEditEventArgs) Handles listMain.BeforeLabelEdit
            If newFolder Then
                newFolder = False
                oldName = ""
                Exit Sub
            ElseIf newFile Then
                newFile = False
                oldName = ""
                Exit Sub
            End If
            If treeMain.SelectedNode.Tag.ToString().EndsWith(Path.DirectorySeparatorChar) Then
                If Not processRepeating Then
                    oldName = treeMain.SelectedNode.Tag.ToString() & listMain.SelectedItems(0).Text

                    Dim lowerName As String = oldName.ToLowerInvariant
                    Dim osLocation As String = Environment.GetFolderPath(Environment.SpecialFolder.System).ToLowerInvariant
                    Dim myMusic As String = Environment.GetFolderPath(Environment.SpecialFolder.MyMusic).ToLowerInvariant
                    Dim myPics As String = Environment.GetFolderPath(Environment.SpecialFolder.MyPictures).ToLowerInvariant
                    Dim progFiles As String = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles).ToLowerInvariant

                    ' protect other directories here also ...
                    If lowerName.Equals(myPics) Or _
                            lowerName.Equals(myMusic) Or _
                            lowerName.Equals(progFiles) Or _
                            lowerName.Equals(osLocation) Or _
                            osLocation.StartsWith(lowerName) Or _
                            lowerName.Equals("system volume information") Or _
                            lowerName.Equals("recycler") Then
                        e.CancelEdit = True
                        oldName = ""
                    Else
                        oldAbsoluteDirName = listMain.SelectedItems(0).Text
                    End If
                Else
                    oldName = ""
                    oldAbsoluteDirName = ""
                    processRepeating = False
                End If
            Else
                If Not processRepeating Then
                    oldName = treeMain.SelectedNode.Tag.ToString() & Path.DirectorySeparatorChar & listMain.SelectedItems(0).Text
                    oldAbsoluteDirName = listMain.SelectedItems(0).Text
                Else
                    oldName = ""
                    oldAbsoluteDirName = ""
                    processRepeating = False
                End If
            End If
        End Sub

        Private Sub timerMain_Elapsed(ByVal sender As System.Object, ByVal e As System.Timers.ElapsedEventArgs) Handles timerMain.Elapsed
            RunSetupCode()
            timerMain.Enabled = False
        End Sub

        Private Sub menuSecurity_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles menuSecurity.Click
            Dim t As FileItemType
            Dim s As String = GetDirectory() & listMain.SelectedItems(0).Text
            If File.Exists(s) Then
                t = FileItemType.File
            ElseIf Directory.Exists(s) Then
                t = FileItemType.Directory
            Else
                t = FileItemType.Other
            End If
            ShowFileDirectoryForm(s, t, DefaultPropertiesView.Security, False)
        End Sub

        Private Function GetDirectory() As String
            If Not treeMain.SelectedNode Is Nothing Then
                If Not treeMain.SelectedNode.Tag Is Nothing Then
                    If Directory.Exists(treeMain.SelectedNode.Tag.ToString()) Then
                        Return treeMain.SelectedNode.Tag.ToString()
                    End If
                End If
            End If
            Return ""
        End Function

        Private Sub menuProperties_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles menuProperties.Click

            Dim t As FileItemType
            Dim s As String = GetDirectory() & listMain.SelectedItems(0).Text
            If File.Exists(s) Then
                t = FileItemType.File
            ElseIf Directory.Exists(s) Then
                t = FileItemType.Directory
            Else
                t = FileItemType.Other
            End If
            ShowFileDirectoryForm(s, t, DefaultPropertiesView.General, False)


        End Sub

        Private Sub ShowFileDirectoryForm(ByVal itemName As String, _
                    ByVal itemType As FileItemType, _
                    ByVal viewType As DefaultPropertiesView, _
                    ByVal fromTree As Boolean)
            Dim fdp As New FileDirectoryProperties()
            Try
                fdp.ItemToManipulate = itemName
                fdp.DefaultView = viewType
                fdp.ItemType = itemType
                fdp.ShowDialog()

                If (fdp.Changes And ChangeType.Name) = ChangeType.Name Then

                    If fromTree Then ' can only be a directory in this situation ...
                        treeMain.SelectedNode.Text = New DirectoryInfo(fdp.ChangedName).Name
                        treeMain.SelectedNode.Tag = fdp.ChangedName
                    Else
                        listMain.SelectedItems(0).Text = New FileInfo(fdp.ChangedName).Name
                    End If
                End If
            Finally
                fdp.Close()
            End Try
        End Sub

        Private Sub miSharing_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles miSharing.Click
            Dim t As FileItemType
            Dim s As String = GetDirectory()

            If Directory.Exists(s) Then
                t = FileItemType.Directory
            Else
                t = FileItemType.Other
            End If
            ShowFileDirectoryForm(s, t, DefaultPropertiesView.Security, True)
        End Sub

        Private Sub contextList_Popup(ByVal sender As Object, ByVal e As System.EventArgs) Handles contextList.Popup
            menuSecurity.Enabled = listMain.SelectedItems.Count > 0
            menuProperties.Enabled = listMain.SelectedItems.Count > 0
            menuNew.Enabled = listMain.SelectedItems.Count <= 0
            ViewMenuItem.Enabled = False
        End Sub
    End Class
End Namespace

