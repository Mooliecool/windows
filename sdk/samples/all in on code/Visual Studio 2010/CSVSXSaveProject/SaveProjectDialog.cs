/************************************* Module Header *********************************\
 * Module Name:        SaveProjectDialog.cs
 * Project :           CSVSXSaveProject
 * Copyright (c)       Microsoft Corporation
 *
 * This dialog is used to display the files included in the project, or under the project
 * folder. Users can select the files that need to be copied.
 * 
 * The source is subject to the Microsoft Public License.
 * See http://www.microsoft.com/en-us/openness/default.aspx
 * All other rights reserved
 * 
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*************************************************************************************/

using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using Microsoft.CSVSXSaveProject.Files;
using EnvDTE;
using System.Text;

namespace Microsoft.CSVSXSaveProject
{
    public partial class SaveProjectDialog : Form
    {
        #region Declare variables to store files information.
        /// <summary>
        /// The path of project folder.
        /// </summary>
        public string OriginalFolderPath { get; set; }

        /// <summary>
        /// The path is that you select folder in Folder Browser Dialog.
        /// </summary>
        public string NewFolderPath { get; private set; }

        /// <summary>
        /// The files included in the project, or under the project folder. 
        /// </summary>
        public List<Files.ProjectFileItem> FilesItems { get; set; }

        /// <summary>
        /// Specify whether the new project should be opened.
        /// </summary>
        public bool OpenNewProject
        {
            get
            {
                return chkOpenProject.Checked;
            }
        }

        #endregion

        /// <summary>
        /// Construct the SaveProject Dialog.
        /// </summary>
        public SaveProjectDialog()
        {
            InitializeComponent();

            // Auto generate column is set false.
            this.dgvFiles.AutoGenerateColumns = false;
        }

        /// <summary>
        /// Save project as other directory when you click this button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnSaveAs_Click(object sender, EventArgs e)
        {
            // Get the path of folder that you want to save by selecting in 
            // FolderBrowserDialog.
            using (FolderBrowserDialog dialog = new FolderBrowserDialog())
            {
                // The new folder button is enable.
                dialog.ShowNewFolderButton = true;

                // Get the result that user operated on FolderBrowserDialog.
                var result = dialog.ShowDialog();

                if (result == System.Windows.Forms.DialogResult.OK)
                {
                    // Get the path of folder.
                    this.NewFolderPath = dialog.SelectedPath;

                    // Copy the files that user selected.
                    CopySelectedItems();

                    // Click on OK, and then close this window.
                    this.DialogResult = System.Windows.Forms.DialogResult.OK;
                    this.Close();
                }
                else
                {
                    return;
                }
            }       
        }

        /// <summary>
        /// The save project dialog is load.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SaveProjectDialog_Load(object sender, EventArgs e)
        {
            // Associate with the DataSource with FilesItems.
            dgvFiles.DataSource = FilesItems;

            foreach (DataGridViewRow row in dgvFiles.Rows)
            {
                ProjectFileItem item = row.DataBoundItem as ProjectFileItem;

                row.Cells["colCopy"].ReadOnly = !item.IsUnderProjectFolder;
            }
        }

        #region CreateAndCopyFiles  
        /// <summary>
        /// Copy the files that related to the item in DataGridView that you selected.
        /// </summary>
        private void CopySelectedItems()
        {
            // Get the files information from Solution Explorer.
            List<Files.ProjectFileItem> fileItems =
                dgvFiles.DataSource as List<Files.ProjectFileItem>;

            // Copy the files from original directory to newFolderPath.
            foreach (var fileItem in fileItems)
            {
                if(fileItem.IsUnderProjectFolder && fileItem.NeedCopy)
                {
                   // Get the absolute path of project file that you save as.
                    FileInfo newFile = new FileInfo(string.Format("{0}\\{1}",
                        NewFolderPath,
                        fileItem.FullName.Substring(OriginalFolderPath.Length)));
                    
                    // Create the directory by the file full name.
                    if (!newFile.Directory.Exists)
                    {
                        Directory.CreateDirectory(newFile.Directory.FullName);
                    }
                    
                    // Copy files.
                    fileItem.Fileinfo.CopyTo(newFile.FullName);
                }       
            }
        }

        #endregion

        /// <summary>
        /// Cancel save project to another directory.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnCancel_Click(object sender, EventArgs e)
        {
            // Cancel and close this windows.
            this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Close();
        }  
    }
}
