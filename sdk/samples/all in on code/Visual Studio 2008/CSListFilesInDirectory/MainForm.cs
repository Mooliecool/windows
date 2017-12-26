/************************************* Module Header **************************************\
* Module Name:  MainForm.cs
* Project:      CSListFilesInDirectory
* Copyright (c) Microsoft Corporation.
* 
* The CSListFilesInDirectory project demonstrates how to implement an IEnumerable<string>
* that utilizes the Win32 File Management functions to enable application to get files and
* sub-directories in a specified directory one item a time.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/7/2009 8:00 PM Jie Wang Created
\******************************************************************************************/

#region Using directives
using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
#endregion


namespace CSListFilesInDirectory
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void btnList_Click(object sender, EventArgs e)
        {
            try
            {
                int i = 0;
                DirectoryEnumerator.Mode mode =
                    (chkDir.Checked ? DirectoryEnumerator.Mode.Directory : 0) |
                    (chkFiles.Checked ? DirectoryEnumerator.Mode.File : 0);

                lstFiles.Items.Clear();

                string pattern = Path.Combine(txtDir.Text.Trim(), txtPattern.Text.Trim());
                DirectoryEnumerator de = new DirectoryEnumerator(pattern, mode);
                
                foreach (string file in de) // Enumerate items in the directory
                {
                    i++; // Increase the count
                    lstFiles.Items.Add(file);
                    Application.DoEvents();
                }

                lblCount.Text = string.Format("{0:#,##0} Item{1}", i, i != 1 ? "s" : string.Empty);
            }
            catch (Win32Exception winEx)
            {
                MessageBox.Show(winEx.Message, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void txtDir_TextChanged(object sender, EventArgs e)
        {
            // Enable the List button if the target directory exists
            btnList.Enabled = Directory.Exists(txtDir.Text.Trim());
        }

        private void chkModes_CheckedChanged(object sender, EventArgs e)
        {
            // Enable the List button if at least one check box is selected
            btnList.Enabled = chkDir.Checked || chkFiles.Checked;
        }
    }
}
