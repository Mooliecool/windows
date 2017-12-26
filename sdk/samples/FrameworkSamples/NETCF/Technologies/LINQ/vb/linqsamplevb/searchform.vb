''---------------------------------------------------------------------
''  This file is part of the Microsoft .NET Framework SDK Code Samples.
'' 
''  Copyright (C) Microsoft Corporation.  All rights reserved.
'' 
''This source code is intended only as a supplement to Microsoft
''Development Tools and/or on-line documentation.  See these other
''materials for detailed information regarding Microsoft code samples.
'' 
''THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
''KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
''IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
''PARTICULAR PURPOSE.
''---------------------------------------------------------------------

Imports System.IO
Imports System.Linq
Imports System.Globalization

Public Class SearchForm


#Region "HelperMethods"
    ''' <summary>
    ''' Populates the Nodes of the selected folder in the TreeView
    ''' </summary>
    ''' <param name="root"></param>
    Private Shared Sub PopulateNodes(ByVal root As TreeNode)


        'set the Current Cursor to WaitCursor
        Cursor.Current = Cursors.WaitCursor

        Dim dir As String

        'if root, change dir  to \, else remove the word root in the FullPath
        If (root.FullPath = "root") Then
            dir = "\"
        Else
            dir = root.FullPath.Replace("root", "")
        End If


        'LINQ query to get all the files (including directories) in a directory, order them by filename and
        'create IEnumerable<FileInfo> of file system entries(list of files and sub directories inside a directory)
        Dim q = From file In Directory.GetFileSystemEntries(dir) _
                Order By file _
                Select New FileInfo(file)



        'AddNodes is the custom Extension Method defined in ExtensionMethods.cs
        'This extension method enumerates through the list of FileInfo (IEnumerable<FileInfo>) and adds them to the TreeNode root.
        q.AddNodes(root)

        'set Cursor back to Default
        Cursor.Current = Cursors.Default
    End Sub


    ''' <summary>
    ''' Searches through the TreeNodes in the TreeView and returns search hits as it finds it starting from the root of TreeView
    ''' </summary>
    ''' <param name="root"></param>
    ''' <param name="searchQuery"></param>
    ''' <returns></returns>
    Function Search(ByVal root As TreeNode, ByVal searchQuery As String) As TreeNode

        'if the root's text matches search query, return it.
        If root.Text.ToLower(CultureInfo.CurrentCulture).StartsWith(searchQuery) Then Return root

        'for every TreeNode in the root's Nodes
        For Each n As TreeNode In root.Nodes

            'if this node's text matches the search query. return it
            If n.Text.ToLower(CultureInfo.CurrentCulture).StartsWith(searchQuery) Then Return n


            'LINQ query to search through the child nodes of node.
            Dim query = From node In n.Nodes.OfType(Of TreeNode)() _
                            Where node.Text.ToLower().StartsWith(searchQuery) _
                            Select node


            'for each hit,  return it
            For Each result As TreeNode In query
                Return result
            Next

            'if the Node n has ChildNodes, then recursively call Search on it.
            If (n.Nodes.Count > 0) Then
                Dim childResult As TreeNode = Search(n, searchQuery)
                Return childResult

            End If


        Next
        Return Nothing

    End Function


    ''' <summary>
    ''' clear the highlighting of search results
    ''' </summary>
    ''' <param name="root"></param>
    Private Shared Sub ClearHighlighting(ByVal root As TreeNode)
    
        'if not null
        If (Not root Is Nothing) Then
            'set backcolor to transparent
            root.BackColor = Color.Transparent
            'do this recursively for all the child nodes.
            For Each child In root.Nodes
                ClearHighlighting(child)
            Next
        End If
    End Sub



#End Region

#Region "EventHandlers"


    ''' <summary>
    ''' AfterSelect event handler for the Tree View
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub treeView_AfterSelect(ByVal sender As System.Object, ByVal e As System.Windows.Forms.TreeViewEventArgs) Handles treeView.AfterSelect
        'selected node
        Dim n As TreeNode = e.Node

        'if the selected node is not root and if node is not already populated.
        If n.Text <> "root" And n.Nodes.Count = 0 Then

            'get full path of File System Entry
            Dim file As String = n.FullPath.Replace("root", "")

            Dim fi As FileInfo = New FileInfo(file)


            'if the File System Entry is a Directory, then Populate its Nodes.
            If (fi.Attributes And FileAttributes.Directory = FileAttributes.Directory) Then

                PopulateNodes(n)
            End If

        End If

    End Sub
    ''' <summary>
    ''' Click event handler of menu item Group View
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub miView_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles miView.Click
        'if Group Form is null, create new GroupForm and Show it.
        If (Common.GroupForm Is Nothing) Then

            Common.GroupForm = New GroupForm()
            Common.GroupForm.Show()

        Else 'else show the already existing GroupForm
            Common.GroupForm.Show()
        End If

    End Sub

    ''' <summary>
    ''' KeyUp event handler for Textbox txtSearch
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub txtSearch_KeyUp(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles txtSearch.KeyUp


        'get the text
        Dim s As String = txtSearch.Text

        'if text is empty
        If s.Trim() = String.Empty Then
            'clear all highlighting if the search text is empty
            ClearHighlighting(treeView.Nodes(0))
            Return
        End If

        'if the key pressed is any of these , then dont do anything
        If e.Control Or e.Shift Or e.KeyCode = Keys.Back Or e.KeyCode = Keys.Delete Or _
            e.KeyCode = Keys.Home Or e.KeyCode = Keys.ShiftKey Then
            Return
        End If


        'call Search on the root of the TreeView
        Dim n As TreeNode = Search(treeView.Nodes(0), s.ToLower(CultureInfo.CurrentCulture))
        If Not n Is Nothing Then
            'change back color of search hits.
            n.BackColor = Color.YellowGreen
            'make sure search hit is visible
            n.EnsureVisible()
        End If


    End Sub

    ''' <summary>
    ''' Load event for Search Form
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub SearchForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        'On load, Populate the treeview with FileSystemEntries of root directory.
        Dim root As TreeNode = treeView.Nodes.Add("root")
        PopulateNodes(root)
    End Sub

    ''' <summary>
    ''' Click event handler for Exit menu item
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub miExit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles miExit.Click
        'Close all instances of forms.
        Common.CloseAll()
        'exit the application
        Application.Exit()
    End Sub

#End Region
End Class

