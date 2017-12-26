/****************************** Module Header ******************************\
* Module Name:    LinqToReflectionForm.cs
* Project:        CSLinqExtension
* Copyright (c) Microsoft Corporation.
*
* This LinqToReflectionForm uses the simple LINQ to Reflection library to 
* display the methods of the exported class in an assembly grouped by return 
* value type.
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
    public partial class LinqToReflectionForm : Form
    {
        public LinqToReflectionForm()
        {
            InitializeComponent();
        }

        // The file name of the CSV file
        private string filePath;

        // Show OpenFileDialog to select an .NET assembly
        private void btnBrowse_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                filePath = openFileDialog1.FileName;
                txtBoxPath.Text = filePath;
            }
        }


        #region Group The Methods by Return Value Type
        private void buttonGroup_Click(object sender, EventArgs e)
        {
            try
            {
                filePath = txtBoxPath.Text;

                // Check whether the file path is empty
                if (String.IsNullOrEmpty(filePath))
                {
                    MessageBox.Show("Please select a .NET assembly file first!");
                    return;
                }

                // Call the GroupMethodByReturnValue to get the grouped methods
                var methods = LinqToReflection.GroupMethodsByReturnValue(
                    filePath);

                // Display the data in the TreeView
                foreach (var group in methods)
                {
                    TreeNode node = treeView1.Nodes.Add(group.Key);
                    foreach (var mi in group)
                    {
                        node.Nodes.Add(mi.Name);
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
    }
}
