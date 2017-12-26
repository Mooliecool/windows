/****************************** Module Header ******************************\
* Module Name:    CustomTreeView.cs
* Project:        CSASPNETInheritingFromTreeNode
* Copyright (c) Microsoft Corporation
*
* This file defines a CustomTreeView control which's tree nodes contain a Tag 
* property. The Tag property can be used to store a custom object.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
\*****************************************************************************/

using System.Web.UI.WebControls;


namespace CSASPNETInheritingFromTreeNode
{
    public class CustomTreeView : TreeView
    {
        /// <summary>
        /// Returns a new instance of the TreeNode class. The CreateNode is a helper method.
        /// </summary>
        protected override TreeNode CreateNode()
        {
            return new CustomTreeNode(this, false);
        }
    }

    public class CustomTreeNode : TreeNode
    {
        /// <summary>
        /// Gets or sets the object that contains data about the tree node.
        /// </summary>
        public object Tag { get; set; }

        public CustomTreeNode() : base()
        {
        }

        public CustomTreeNode(TreeView owner, bool isRoot) : base(owner, isRoot)
        {
        }

        /// <summary>
        /// Restores view-state information from a previous page request that 
        /// was saved by the SaveViewState method.
        /// </summary>
        /// <param name="state">
        /// An Object that represents the control state to be restored. 
        /// </param>
        protected override void LoadViewState(object state)
        {
            object[] arrState = state as object[];

            this.Tag = arrState[0];
            base.LoadViewState(arrState[1]);
        }

        /// <summary>
        /// Saves any server control view-state changes that have occurred 
        /// since the time the page was posted back to the server.
        /// </summary>
        /// <returns>
        /// Returns the server control's current view state. If there is no 
        /// view state associated with the control, this method returns null.
        /// </returns>
        protected override object SaveViewState()
        {
            object[] arrState = new object[2];
            arrState[1] = base.SaveViewState();
            arrState[0] = this.Tag;

            return arrState;
        }
    }
}
