==============================================================================
    ASP.NET APPLICATION : CSASPNETInheritingFromTreeNode Project Overview
==============================================================================

//////////////////////////////////////////////////////////////////////////////
Summary:

In windows forms TreeView, each tree node has a property called "Tag" which 
can be used to store a custom object. Sometimes we want to have the same 
feature in ASP.NET TreeView. This project creates a custom TreeView control 
named "CustomTreeView" to achieve this goal.


//////////////////////////////////////////////////////////////////////////////
Demo:

Open the page Default.aspx in the browser, you can see a TreeView control in 
the page. Select one of the TreeNode, then the custom object which assigned to 
the selected node will display. 


//////////////////////////////////////////////////////////////////////////////
Code Logical:

1. Create the custom TreeView.

   Define the custom TreeNode named "CustomTreeNode" with a property named "Tag".
   We are going to store the custom object in that property. In order to save 
   the new property in View State, we override methods LoadViewState() and 
   SaveViewState() to achieve saving and retrieving.

        protected override void LoadViewState(object state)
        {
            object[] arrState = state as object[];

            this.Tag = arrState[0];
            base.LoadViewState(arrState[1]);
        }
        protected override object SaveViewState()
        {
            object[] arrState = new object[2];
            arrState[1] = base.SaveViewState();
            arrState[0] = this.Tag;

            return arrState;
        }
    
    In the post back, ASP.NET runtime will recreate the TreeView control. To let 
    the TreeView control to create custom TreeNode automatically, we override 
    the helper method CreateNode().

        protected override TreeNode CreateNode()
        {
            return new CustomTreeNode(this, false);
        }

2. Define the custom object.

   In order to save the object to View State, the object needs to be serializable.

        [Serializable]
        public class MyItem
        {
            public string Title { get; set; }
        }

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


//////////////////////////////////////////////////////////////////////////////