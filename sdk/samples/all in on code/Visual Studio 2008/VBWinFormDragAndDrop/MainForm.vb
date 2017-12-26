'************************************* Module Header **************************************\
' Module Name:  MainForm.vb
' Project:      VBWinFormDragAndDrop
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to perform drag-and-drop operations in a 
' Windows Forms Application.
' 
' For more information about the drag-and-drop operations, see:
' 
'  Performing Drag-and-Drop Operations in Windows Forms
'  http://msdn.microsoft.com/en-us/library/aa984430(VS.71).aspx
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'******************************************************************************************/


Public Class MainForm

    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' Add items to the ListBox
        Me.listBox1.Items.Add("item1")
        Me.listBox1.Items.Add("item2")
        Me.listBox1.Items.Add("item3")
        Me.listBox1.Items.Add("item4")
        Me.listBox1.Items.Add("item5")

        ' Enable dropping on the TreeView
        Me.treeView1.AllowDrop = True
        AddHandler Me.listBox1.MouseDown, New MouseEventHandler(AddressOf Me.listBox1_MouseDown)
        AddHandler Me.treeView1.DragEnter, New DragEventHandler(AddressOf Me.treeView1_DragEnter)
        AddHandler Me.treeView1.DragDrop, New DragEventHandler(AddressOf Me.treeView1_DragDrop)
    End Sub

    Private Sub listBox1_MouseDown(ByVal sender As Object, ByVal e As MouseEventArgs)
        ' In the MouseDown event for the ListBox where the drag will begin, 
        ' use the DoDragDrop method to set the data to be dragged 
        ' and the allowed effect dragging will have.
        If (Not Me.listBox1.SelectedItem Is Nothing) Then
            Me.listBox1.DoDragDrop(Me.listBox1.SelectedItem, DragDropEffects.Copy)
        End If
    End Sub

    Private Sub treeView1_DragDrop(ByVal sender As Object, ByVal e As DragEventArgs)
        ' In the DragDrop event for the TreeView where the drop will occur, 
        ' use the GetData method to retrieve the data being dragged.
        Dim item As String = CStr(e.Data.GetData(e.Data.GetFormats()(0)))
        ' Add the item strib
        Me.treeView1.Nodes.Add(item)
    End Sub

    Private Sub treeView1_DragEnter(ByVal sender As Object, ByVal e As DragEventArgs)
        ' Sets the effect that will happen when the drop occurs to a value 
        ' in the DragDropEffects enumeration.
        e.Effect = DragDropEffects.Copy
    End Sub

End Class
