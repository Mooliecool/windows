==============================================================================
 ASP.NET APPLICATION : VBASPNETInheritingFromTreeNode Project Overview
==============================================================================

//////////////////////////////////////////////////////////////////////////////
Summary:

In windows forms TreeView, each tree node has a property called "Tag" which can 
be used to store a custom object. Sometimes we want to have the same feature in ASP.NET 
TreeView. This project creates a custom TreeView control named "CustomTreeView" 
to achieve this goal.

//////////////////////////////////////////////////////////////////////////////
Demo the Sample:

Open the page Default.aspx in the browser, you can see a TreeView control in the page.
Select one of the TreeNode, then the custom object which assigned to the selected node
will display. 

//////////////////////////////////////////////////////////////////////////////
Code Logical:

1. Create the custom TreeView.

   Define the custom TreeNode named "CustomTreeNode" with a property named "Tag".
   We are going to store the custom object in that property. In order to save 
   the new property in View State, we override methods LoadViewState() and SaveViewState()
   to achieve saving and retrieving.

        Protected Overrides Sub LoadViewState(ByVal state As Object)
            Dim arrState As Object() = TryCast(state, Object())

            Me.Tag = arrState(0)
            MyBase.LoadViewState(arrState(1))
        End Sub

        Protected Overrides Function SaveViewState() As Object
            Dim arrState As Object() = New Object(1) {}
            arrState(1) = MyBase.SaveViewState()
            arrState(0) = Me.Tag

            Return arrState
        End Function
    
    In the post back, ASP.NET runtime will recreate the TreeView control. To let 
    the TreeView control to create custom TreeNode automatically, we override 
    the helper method CreateNode().

        Protected Overrides Function CreateNode() As TreeNode
            Return New CustomTreeNode(Me, False)
        End Function

2. Define the custom object.

   In order to save the object to View State, the object needs to be serializable.

        <Serializable()> _
        Public Class MyItem
            Public Property Title() As String
                Get
                    Return m_Title
                End Get
                Set(ByVal value As String)
                    m_Title = value
                End Set
            End Property
            Private m_Title As String
        End Class

3. Create the test page.

   Add a CustomTreeView control in the page. In Page_Load() method, create some 
   CustomTreeNodes and assign a custom object to each CustomTreeNode.

   In the CustomTreeView control's SelectedNodeChanged event handler, retrieve 
   the custom object from the selected tree node, and then display it.

//////////////////////////////////////////////////////////////////////////////
References:

TreeNode.Tag Property
http://msdn.microsoft.com/en-us/library/system.windows.forms.treenode.tag.aspx

Control.SaveViewState Method
http://msdn.microsoft.com/en-us/library/system.web.ui.control.saveviewstate.aspx

Control.LoadViewState Method
http://msdn.microsoft.com/en-us/library/system.web.ui.control.loadviewstate.aspx

TreeView.CreateNode Method
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.treeview.createnode.aspx