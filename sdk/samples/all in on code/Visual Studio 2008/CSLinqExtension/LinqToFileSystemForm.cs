/****************************** Module Header ******************************\
* Module Name:    LinqToFileSystemForm.cs
* Project:        CSLinqExtension
* Copyright (c) Microsoft Corporation.
*
* This LinqToFileSystemForm uses the simple LINQ to File System library to 
* group the files in one folder by their extension names and to show the 
* largest files with their sizes and names.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
#endregion


namespace CSLinqExtension
{
    public partial class LinqToFileSystemForm : Form
    {
        public LinqToFileSystemForm()
        {
            InitializeComponent();
        }

        // The target file folder path
        private string filePath;

        // Show FolderBrowserDialog to select a file folder
        private void btnBrowse_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();

            if (dialog.ShowDialog() == DialogResult.OK)
            {
                filePath = dialog.SelectedPath;
                txtBoxPath.Text = filePath;
            }
        }


        #region Group Files by Extension Names
        private void btnGroup_Click(object sender, EventArgs e)
        {
            try
            {
                filePath = txtBoxPath.Text;

                // Check whether the file path is empty
                if (String.IsNullOrEmpty(filePath))
                {
                    MessageBox.Show("Please choose the file folder first!");
                    return;
                }
 
                // Get the all the files under the folder by the GetFiles
                // extension methods, then group the files by the extension
                // names.
                var queryGroupByExt = 
                    from file in LinqToFileSystem.GetFiles(filePath)
                    group file by file.Extension.ToLower() into fileGroup
                    orderby fileGroup.Key
                    select fileGroup;

                // Display the files in TreeView
                foreach (var fg in queryGroupByExt)
                {
                    TreeNode node = treeView1.Nodes.Add(fg.Key);
                    foreach (var f in fg)
                    {
                        node.Nodes.Add(f.Name);
                    }
                }

                treeView1.ExpandAll();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
        #endregion


        #region Show The Largest Files
        private void btnMax_Click(object sender, EventArgs e)
        {
            try
            {
                filePath = txtBoxPath.Text;

                // Check whether the file path is empty
                if (String.IsNullOrEmpty(filePath))
                {
                    MessageBox.Show("Please choose the file folder first!");
                    return;
                }

                // Get the largest files by the LargestFiles extension method
                var largestFileLists = LinqToFileSystem.GetFiles(filePath).
                    LargestFiles();

                // Display the data
                if (largestFileLists.Count() > 0)
                {
                    string text = String.Format("{0} file(s) with max length {1} bytes:", largestFileLists.Count(), largestFileLists.First().Length);

                    foreach (var file in largestFileLists)
                    {
                        text += "\r\n" + file.Name;
                    }

                    MessageBox.Show(text);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
        #endregion
    }
}
