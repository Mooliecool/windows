//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------
using System;
using System.Linq;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace Microsoft.Samples.Linq
{
    /// <summary>
    /// Class for SearchForm
    /// </summary>
    public partial class SearchForm : Form
    {
        #region Constructor
        /// <summary>
        /// Constructor
        /// </summary>
        public SearchForm()
        {
            InitializeComponent();
        }
        #endregion

        #region HelperMethods
        /// <summary>
        /// Populates the Nodes of the selected folder in the TreeView
        /// </summary>
        /// <param name="root"></param>
        private static void PopulateNodes(TreeNode root)
        {

            //set the Current Cursor to WaitCursor
            Cursor.Current = Cursors.WaitCursor;

            string dir;

            //if root, change dir  to \, else remove the word root in the FullPath
            if (root.FullPath == "root")
                dir = @"\";
            else
                dir = root.FullPath.Replace("root", @"");


            //LINQ query to get all the files (including directories) in a directory, order them by filename and
            //create IEnumerable<FileInfo> of file system entries(list of files and sub directories inside a directory)
            var q = from file in Directory.GetFileSystemEntries(dir)
                    orderby file
                    select new FileInfo(file);


            //AddNodes is the custom Extension Method defined in ExtensionMethods.cs
            //This extension method enumerates through the list of FileInfo (IEnumerable<FileInfo>) and adds them to the TreeNode root.
            q.AddNodes(root);

            //set Cursor back to Default
            Cursor.Current = Cursors.Default;
        }        

        /// <summary>
        /// Searches through the TreeNodes in the TreeView and yields search hits as it finds it starting from the root of TreeView
        /// </summary>
        /// <param name="root"></param>
        /// <param name="s"></param>
        /// <returns></returns>
        IEnumerable<TreeNode> Search(TreeNode root, string s)
        {
            //if the root's text matches search query, yield return it.
            if (root.Text.ToLower().StartsWith(s))
                yield return root;

            //for every TreeNode in the root's Nodes
            foreach (TreeNode n in root.Nodes)
            {
                //if this node's text matches the search query. yield return it
                if (n.Text.ToLower().StartsWith(s))
                    yield return n;

                //LINQ query to search through the child nodes of node.
                var query = from node in n.Nodes.OfType<TreeNode>()
                        where node.Text.ToLower().StartsWith(s)
                        select node;

                //foreach hit, yield return it
                foreach (TreeNode result in query)
                    yield return result;

                //if the Node n has ChildNodes, then recursively call Search on it.
                if (n.Nodes.Count > 0)
                {
                    IEnumerable<TreeNode> childResults = Search(n, s);

                    //foreach hit, yield return
                    foreach (TreeNode result in childResults)
                        yield return result;
                }

            }
        }

        /// <summary>
        /// clear the highlighting of search results
        /// </summary>
        /// <param name="root"></param>
        private static void ClearHighlighting(TreeNode root)
        {

            //if not null
            if (root != null)
            {
                //set backcolor to transparent
                root.BackColor = Color.Transparent;
                //do this recursively for all the child nodes.
                foreach (TreeNode child in root.Nodes)
                    ClearHighlighting(child);
            }
        }

        #endregion

        #region EventHandlers
        /// <summary>
        /// Load event for Search Form
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SearchForm_Load(object sender, EventArgs e)
        {
            //On load, Populate the treeview with FileSystemEntries of root directory.
            TreeNode root = this.treeView.Nodes.Add(@"root");
            PopulateNodes(root);
        }

        /// <summary>
        /// Click event handler for Exit menu item
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void miExit_Click(object sender, EventArgs e)
        {
            //Close all instances of forms.
            Common.CloseAll();            
            //exit the application
            Application.Exit();
        }

        /// <summary>
        /// AfterSelect event handler for the Tree View
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void treeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            //selected node
            TreeNode n = e.Node;

            //if the selected node is not root and if node is not already populated.
            if (n.Text != "root" && n.Nodes.Count == 0)
            {

                //get full path of File System Entry
                string file = n.FullPath.Replace("root", "");

                FileInfo fi = new FileInfo(file);

                //if the File System Entry is a Directory, then Populate its Nodes.
                if ((int)fi.Attributes != -1 && (fi.Attributes & FileAttributes.Directory) == FileAttributes.Directory)
                {
                    PopulateNodes(n);
                }
            }

        }
        
      
        /// <summary>
        /// KeyUp event handler for Textbox txtSearch
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void txtSearch_KeyUp(object sender, KeyEventArgs e)
        {

            //get the text
            string s = this.txtSearch.Text;

            //if text is empty
            if (string.IsNullOrEmpty(s))
            {
                //clear all highlighting if the search text is empty
                ClearHighlighting(this.treeView.Nodes[0]);
                return;
            }
            //if the key pressed is any of these , then dont do anything
            if (e.Control || e.Shift || e.KeyCode == Keys.Back || e.KeyCode == Keys.Delete ||
                e.KeyCode == Keys.Home || e.KeyCode == Keys.ShiftKey)
                return;

            
            //call Search on the root of the TreeView
            foreach (TreeNode n in this.Search(this.treeView.Nodes[0], s.ToLower(System.Globalization.CultureInfo.CurrentCulture)))
            {
                //change back color of search hits.
                n.BackColor = Color.YellowGreen;

                //make sure search hit is visible
                n.EnsureVisible();

                //if we need to highlight just the first search result, break after you get the first hit
                //else enumerate all search hits and change back color of all tree node hits.
                if (!this.chkHighlight.Checked)
                {
                    break;
                }



            }
        }
       
        /// <summary>
        /// Click event handler of menu item Group View
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void miView_Click(object sender, EventArgs e)
        {
             
            //if Group Form is null, create new GroupForm and Show it.
            if (Common.GroupForm == null)
            {
                Common.GroupForm = new GroupForm();
                Common.GroupForm.Show();
            }
            else //else show the already existing GroupForm
                Common.GroupForm.Show();
        }
        #endregion
    }
}