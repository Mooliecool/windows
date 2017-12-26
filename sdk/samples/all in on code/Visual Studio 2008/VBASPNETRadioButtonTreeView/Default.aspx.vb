'****************************** Module Header ******************************\
' Module Name:  Default.aspx.vb
' Project:      VBASPNETRadioButtonTreeView
' Copyright (c) Microsoft Corporation
'
' The project shows how to simulate a RadioButton Group within the TreeView 
' control to make the user can only select one item from a note tree. Since 
' there is no build-in feature to achieve this, we use two images to imitate
' the RadioButton and set TreeNode's ImageUrl property as the path of these
' images. When user clicks the item in tree, reset the Checked value and the
' ImageUrl value of the treenode to make it looks lick a selected item of a 
' RadioButton appearance.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' History:
' * 11/6/2009 4:30 PM Bravo Yang Created
'***************************************************************************/


Partial Public Class _Default
    Inherits System.Web.UI.Page


    Protected Sub TreeView1_SelectedNodeChanged(ByVal sender As Object, ByVal e As EventArgs) Handles RadioButtonTreeView.SelectedNodeChanged
        Dim mynode As TreeNode = RadioButtonTreeView.SelectedNode

        'Loop through all the TreeNodes in the same level of the selected one
        'And reset its Checked propery as well as the ImageUrl to the very beginning
        For i As Integer = 0 To mynode.Parent.ChildNodes.Count - 1
            mynode.Parent.ChildNodes(i).ImageUrl = "~/unchecked.gif"
            mynode.Parent.ChildNodes(i).Checked = False
        Next

        'After reset all the TreeNodes, set the selected one to be Checked.
        mynode.ImageUrl = "~/checked.gif"
        mynode.Checked = True

    End Sub

    Protected Sub Button1_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnSubmit.Click
        lbSelectInfo.Text = "Here are the item(s) you choose <br />"

        For i As Integer = 0 To RadioButtonTreeView.CheckedNodes.Count - 1
            lbSelectInfo.Text = lbSelectInfo.Text & "<br />" & RadioButtonTreeView.CheckedNodes(i).Text
        Next
    End Sub


End Class