'****************************** Module Header ******************************\
' Module Name:    Default.aspx.vb
' Project:        VBASPNETInheritingFromTreeNode
' Copyright (c) Microsoft Corporation
'
' This page shows how to assign/retrieve custom objects to/from the 
' CustomTreeView control.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs)
        If Not IsPostBack Then
            ' Display 10 nodes in the TreeView control.
            For i As Integer = 0 To 9
                Dim treeNode As New CustomTreeNode()

                ' Assign a custom object to the tree node.
                Dim item As New MyItem()
                item.Title = "Object " & i.ToString()
                treeNode.Tag = item

                treeNode.Value = i.ToString()
                treeNode.Text = "Node " & i.ToString()

                CustomTreeView1.Nodes.Add(treeNode)
            Next
        End If
    End Sub

    Protected Sub CustomTreeView1_SelectedNodeChanged(ByVal sender As Object, ByVal e As EventArgs)
        ' Retrieve the object which is stored in the tree node.
        If CustomTreeView1.SelectedNode IsNot Nothing Then
            Dim treeNode As CustomTreeNode = DirectCast(CustomTreeView1.SelectedNode, CustomTreeNode)
            Dim item As MyItem = DirectCast(treeNode.Tag, MyItem)

            lbMessage.Text = String.Format("The selected object is: {0}", item.Title)
        End If
    End Sub

End Class