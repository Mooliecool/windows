/****************************** Module Header ******************************\
* Module Name:    Default.aspx.cs
* Project:        CSASPNETInheritingFromTreeNode
* Copyright (c) Microsoft Corporation
*
* This page shows how to assign/retrieve custom objects to/from the 
* CustomTreeView control.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
\*****************************************************************************/

using System;


namespace CSASPNETInheritingFromTreeNode
{
    public partial class Default : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                // Display 10 nodes in the TreeView control.
                for (int i = 0; i < 10; i++)
                {
                    CustomTreeNode treeNode = new CustomTreeNode();

                    // Assign a custom object to the tree node.
                    MyItem item = new MyItem();
                    item.Title = "Object " + i.ToString();
                    treeNode.Tag = item;

                    treeNode.Value = i.ToString();
                    treeNode.Text = "Node " + i.ToString();

                    CustomTreeView1.Nodes.Add(treeNode);
                }
            }
        }
        protected void CustomTreeView1_SelectedNodeChanged(object sender, EventArgs e)
        {
            // Retrieve the object which is stored in the tree node.
            if (CustomTreeView1.SelectedNode != null)
            {
                CustomTreeNode treeNode = (CustomTreeNode)CustomTreeView1.SelectedNode;
                MyItem item = (MyItem)treeNode.Tag;

                lbMessage.Text = string.Format("The selected object is: {0}", item.Title);
            }
        }
    }
}