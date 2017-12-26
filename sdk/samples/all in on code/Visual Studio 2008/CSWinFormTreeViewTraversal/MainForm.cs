/************************************* Module Header **************************************\
* Module Name:	MainForm.cs
* Project:		CSWinFormTreeViewTraversal
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to perform TreeView nodes travel and find
* a special node
* 
* For more information about the TreeView control, see:
* 
*  Windows Forms TreeView control
*  http://msdn.microsoft.com/en-us/library/ch6etkw4.aspx
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/

#region Namespace
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
#endregion

namespace CSWinFormTreeViewTraversal
{
    public partial class MainForm : Form
    {
        #region Members
        // Store current search text in textBox1
        private string currentSearchText;
        // Store match state
        private bool currentMathWholeWord;
        // Store found nodes
        private List<TreeNode> foundNodes;
        // Current selected node
        private int selectedIndex;
        #endregion

        #region Constructor
        public MainForm()
        {
            InitializeComponent();
            currentSearchText = "";
            foundNodes = new List<TreeNode>();
            selectedIndex = 0;
        }
        #endregion

        #region Events
        private void btnTravel_Click(object sender, EventArgs e)
        {
            // List all nodes of TreeView
            this.listBox1.Items.Clear();
            List<TreeNode> allNodes = new List<TreeNode>();
            allNodes = FindNode(this.treeView1, "", false);
            foreach (TreeNode tn in allNodes)
            {
                listBox1.Items.Add(tn.Text);
            }
        }

        private void btnClear_Click(object sender, EventArgs e)
        {
            this.listBox1.Items.Clear();
        }

        private void btnFindNext_Click(object sender, EventArgs e)
        {
            // When the search condition changes, call FindNode to research
            if (currentSearchText != textBox1.Text || currentMathWholeWord != ckMatchWholeWord.Checked)
            {
                currentSearchText = textBox1.Text;
                currentMathWholeWord = ckMatchWholeWord.Checked;
                foundNodes.Clear();
                foundNodes = FindNode(this.treeView1, currentSearchText, currentMathWholeWord);
                selectedIndex = 0;
            }
            if (selectedIndex < foundNodes.Count)
            {
                // Select found nodes one by one
                treeView1.SelectedNode = foundNodes[selectedIndex++];
                treeView1.Focus();
            }
            else
            {
                selectedIndex = 0;
            }
        }
        #endregion

        #region FindNode method
        private List<TreeNode> FindNode(TreeView treeView, string nodeText, bool matchWholeWord)
        {
            // Store the found node
            List<TreeNode> lstFoundNode = new List<TreeNode>();

            // Travel stack
            Stack<TreeNode> nodeStack = new Stack<TreeNode>();
            for (int i = 0; i < treeView.Nodes.Count; i++)
            {
                nodeStack.Push(treeView.Nodes[i]);
            }

            while (nodeStack.Count != 0)
            {
                TreeNode treeNode = nodeStack.Pop();
                if (matchWholeWord)
                {
                    if (treeNode.Text == nodeText)
                    {
                        lstFoundNode.Add(treeNode);
                    }
                }
                else
                {
                    if (treeNode.Text.Contains(nodeText))
                    {
                        lstFoundNode.Add(treeNode);
                    }
                }

                for (int i = 0; i < treeNode.Nodes.Count; i++)
                {
                    nodeStack.Push(treeNode.Nodes[i]);
                }
            }
            return lstFoundNode;
        }
        #endregion
    }
}
