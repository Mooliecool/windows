'************************************ Module Header **************************************\
'* Module Name:	CreateTreeViewFromDataTable.vb
'* Project:		VBWinFormTreeViewLoad
'* Copyright (c) Microsoft Corporation.
'* 
'* This module will create the TreeView.
'* 
'* For more information about the TreeView control, see:
'* 
'*  Windows Forms TreeView control
'*  http://msdn.microsoft.com/en-us/library/ch6etkw4.aspx
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\*****************************************************************************************


Imports System
Imports System.Collections.Generic
Imports System.Windows.Forms
Imports System.Data

Class CreateTreeViewFromDataTable
    ' This Dictionary will identify each List<TreeNode>
    ' And the List<TreeNode> will restore the all TreeNode from a same parentNode
    Private Shared dic As Dictionary(Of Integer, List(Of TreeNode))

    Public Shared Sub BuildTree(ByVal dt As DataTable, ByVal treeView As TreeView, ByVal expandAll As [Boolean], ByVal displayName As String, ByVal nodeId As String, ByVal parentId As String)
        ' Clear the TreeView if there are another datas in this TreeView
        treeView.Nodes.Clear()

        dic = New Dictionary(Of Integer, List(Of TreeNode))()

        Dim node As TreeNode = Nothing

        For Each row As DataRow In dt.Rows

            ' Restore each record into a TreeNode
            node = New TreeNode(row(displayName).ToString())
            node.Tag = row(nodeId)

            ' The parentId of the TreeView's root is "" in the DataTable
            ' So if the parentId is a "", then it is the root
            ' Otherwise it is only a common TreeNode 
            If row(parentId).ToString() <> "" Then
                Dim _parentId As Integer = Convert.ToInt32(row(parentId))

                ' If there's exists a List<TreeNode> was identified by this _parentId
                ' Then we need put this node into this identified List<TreeNode>
                If dic.ContainsKey(_parentId) Then
                    dic(_parentId).Add(node)
                Else
                    ' Otherwise
                    ' We will Add a new record into the Dictionary<int, List<TreeNode>>
                    dic.Add(_parentId, New List(Of TreeNode)())

                    ' Then put this node into the new List<TreeNode>
                    dic(_parentId).Add(node)
                End If
            Else
                ' Take this node into the place of the TreeView's root
                treeView.Nodes.Add(node)
            End If
        Next

        ' After collect and identify each collection with their parentId
        ' We will go on building this tree with the founded root node
        SearchChildNodes(treeView.Nodes(0))

        If expandAll Then
            ' Expand the TreeView
            treeView.ExpandAll()
        End If
    End Sub
    Private Shared Sub SearchChildNodes(ByVal parentNode As TreeNode)

        If Not dic.ContainsKey(Convert.ToInt32(parentNode.Tag)) Then
            ' If there's no a identified collection by this parentId
            ' We will do nothing and return directly
            Return
        End If

        ' Put the identified collection by this parentId into the tree as this node's children
        parentNode.Nodes.AddRange(dic(Convert.ToInt32(parentNode.Tag)).ToArray())

        ' See these children nodes as parent nodes
        For Each _parentNode As TreeNode In dic(Convert.ToInt32(parentNode.Tag)).ToArray()
            ' Then go to find the identified collection by these id
            SearchChildNodes(_parentNode)
        Next
    End Sub
End Class
