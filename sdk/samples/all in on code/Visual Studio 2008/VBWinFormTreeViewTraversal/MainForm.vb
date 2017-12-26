'************************************* Module Header **************************************\
' Module Name:	MainForm.vb
' Project:		VBWinFormTreeViewTraversal
' Copyright (c) Microsoft Corporation.
' 
' This example demonstrates how to perform TreeView nodes travel and find
' a special node
' 
' For more information about the TreeView control, see:
' 
'  Windows Forms TreeView control
'  http://msdn.microsoft.com/en-us/library/ch6etkw4.aspx
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
#Region "Members"
    Private currentMathWholeWord As Boolean
    Private currentSearchText As String
    Private foundNodes As List(Of TreeNode)
    Private selectedIndex As Integer
#End Region

#Region "Constructor"
    Public Sub New()
        InitializeComponent()
        Me.currentSearchText = ""
        Me.foundNodes = New List(Of TreeNode)
        Me.selectedIndex = 0
    End Sub
#End Region

#Region "Events"
    Private Sub btnClear_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClear.Click
        Me.listBox1.Items.Clear()
    End Sub

    Private Sub btnTravel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTravel.Click
        Me.listBox1.Items.Clear()
        Dim allNodes As New List(Of TreeNode)
        allNodes = Me.FindNode(Me.treeView1, "", False)
        Dim tn As TreeNode
        For Each tn In allNodes
            Me.listBox1.Items.Add(tn.Text)
        Next
    End Sub

    Private Sub btnFindNext_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFindNext.Click
        If ((Me.currentSearchText <> Me.textBox1.Text) OrElse (Me.currentMathWholeWord <> Me.ckMatchWholeWord.Checked)) Then
            Me.currentSearchText = Me.textBox1.Text
            Me.currentMathWholeWord = Me.ckMatchWholeWord.Checked
            Me.foundNodes.Clear()
            Me.foundNodes = Me.FindNode(Me.treeView1, Me.currentSearchText, Me.currentMathWholeWord)
            Me.selectedIndex = 0
        End If
        If (Me.selectedIndex < Me.foundNodes.Count) Then
            Me.treeView1.SelectedNode = Me.foundNodes.Item(Me.selectedIndex)
            Me.selectedIndex += 1
            Me.treeView1.Focus()
        Else
            Me.selectedIndex = 0
        End If
    End Sub
#End Region

#Region "FindNode method"
    Private Function FindNode(ByVal treeView As TreeView, ByVal nodeText As String, ByVal matchWholeWord As Boolean) As List(Of TreeNode)
        Dim lstFoundNode As New List(Of TreeNode)
        Dim nodeStack As New Stack(Of TreeNode)
        Dim i As Integer = 0
        Do While (i < treeView.Nodes.Count)
            nodeStack.Push(treeView.Nodes.Item(i))
            i += 1
        Loop
        Do While (nodeStack.Count <> 0)
            Dim treeNode As TreeNode = nodeStack.Pop
            If matchWholeWord Then
                If (treeNode.Text = nodeText) Then
                    lstFoundNode.Add(treeNode)
                End If
            ElseIf treeNode.Text.Contains(nodeText) Then
                lstFoundNode.Add(treeNode)
            End If
            For j As Integer = 0 To treeNode.Nodes.Count - 1
                nodeStack.Push(treeNode.Nodes.Item(j))
            Next j
        Loop
        Return lstFoundNode
    End Function
#End Region

End Class
