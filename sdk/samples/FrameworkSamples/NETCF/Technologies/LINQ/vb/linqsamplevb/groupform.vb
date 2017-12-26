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

Imports System.Windows.Forms
Imports System.Linq
Imports System.IO

Public Class GroupForm

#Region "HelperMethods"
    ''' <summary>
    ''' Populates the List View with list of File System entries grouped by their extension
    ''' </summary>
    Private Sub PopulteListView()
        'Clear the list view items
        lvGroup.Items.Clear()

        'LINQ query that lets you group the list of File System Entries by their extension
        'group has Key = Extension, Files = list of files of that Extension
        'this query creates new Anonymous Type with two members Extension and Files
        Dim query = From f In Directory.GetFileSystemEntries(txtDir.Text) _
                    Let fi = New FileInfo(f) _
                    Group f By key = fi.Extension Into g = Group _
                    Select New With {.Extension = key, .Files = g}



            'for every item in the query result
        For Each v In query

            Dim ext As String
            'get the extension
            If (v.Extension.Trim() = String.Empty) Then
                ext = "Folder"
            Else
                ext = v.Extension
            End If

            'create new List View Item (Extension)
            Dim lviExtension As ListViewItem = New ListViewItem(ext)
            lviExtension.SubItems.Add("")
            lviExtension.SubItems.Add("")

            'add it to the ListView Items
            lvGroup.Items.Add(lviExtension)

            'for every file that belongs to the current Extension
            For Each file In v.Files

                'Create new ListViewItem and add the file's Name and LastWriteTime
                Dim lviFile As ListViewItem = New ListViewItem("")
                Dim fi As FileInfo = New FileInfo(file)
                lviFile.SubItems.Add(fi.Name)
                lviFile.SubItems.Add(fi.LastWriteTime.ToShortDateString())

                'add it to the ListView Items
                lvGroup.Items.Add(lviFile)
            Next

        Next

        'make the List View visible
        lvGroup.Visible = True
    End Sub


#End Region

#Region "EventHandlers"
    ''' <summary>
    ''' Click event handler for button Group
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub btnGroup_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGroup.Click
        'if text entered is empty, return
        If txtDir.Text.Trim() = String.Empty Then Return

        'if Directory does not exist, return
        If Not Directory.Exists(txtDir.Text) Then Return

        'set the Current Cursor to WaitCursor	
        Cursor.Current = Cursors.WaitCursor

        'Group the directory by Extension
        PopulteListView()

        'set Cursor back to Default
        Cursor.Current = Cursors.Default
    End Sub

    ''' <summary>
    ''' Click Event handler for menu item Exit
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub miExit_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles miExit.Click
        'Close all instances of forms.
        Common.CloseAll()

        'exit the Application
        Application.Exit()
    End Sub

    ''' <summary>
    ''' Click event handler for menu item Search View
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub miSearch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles miSearch.Click
        'if SearchForm is null, create new SearchForm and Show it.
        If Common.SearchForm Is Nothing Then

            Common.SearchForm = New SearchForm()
            Common.SearchForm.Show()

        Else 'else show the already existing SearchForm
            Common.SearchForm.Show()
        End If

    End Sub

#End Region

End Class