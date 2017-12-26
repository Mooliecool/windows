'****************************** Module Header ******************************\
' Module Name:    CustomTreeView.vb
' Project:        VBASPNETInheritingFromTreeNode
' Copyright (c) Microsoft Corporation
'
' This file defines a CustomTreeView control which's tree nodes contain a Tag 
' property. The Tag property can be used to store a custom object.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'*****************************************************************************/

Public Class CustomTreeView
    Inherits TreeView
    ''' <summary>
    ''' Returns a new instance of the TreeNode class. The CreateNode is a helper method.
    ''' </summary>
    Protected Overrides Function CreateNode() As TreeNode
        Return New CustomTreeNode(Me, False)
    End Function
End Class

Public Class CustomTreeNode
    Inherits TreeNode
    ''' <summary>
    ''' Gets or sets the object that contains data about the tree node.
    ''' </summary>
    Public Property Tag As Object

    Public Sub New()
        MyBase.New()
    End Sub

    Public Sub New(ByVal owner As TreeView, ByVal isRoot As Boolean)
        MyBase.New(owner, isRoot)
    End Sub

    ''' <summary>
    ''' Restores view-state information from a previous page request that 
    ''' was saved by the SaveViewState method.
    ''' </summary>
    ''' <param name="state">
    ''' An Object that represents the control state to be restored. 
    ''' </param>
    Protected Overrides Sub LoadViewState(ByVal state As Object)
        Dim arrState As Object() = TryCast(state, Object())

        Me.Tag = arrState(0)
        MyBase.LoadViewState(arrState(1))
    End Sub

    ''' <summary>
    ''' Saves any server control view-state changes that have occurred 
    ''' since the time the page was posted back to the server.
    ''' </summary>
    ''' <returns>
    ''' Returns the server control's current view state. If there is no 
    ''' view state associated with the control, this method returns null.
    ''' </returns>
    Protected Overrides Function SaveViewState() As Object
        Dim arrState As Object() = New Object(1) {}
        arrState(1) = MyBase.SaveViewState()
        arrState(0) = Me.Tag

        Return arrState
    End Function
End Class