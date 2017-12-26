/****************************** Module Header ******************************\
* Module Name:  CFDInBCLForm.cs
* Project:      CSShellCommonFileDialog
* Copyright (c) Microsoft Corporation.
* 
* Customize Your Open File Dialog
* http://msdn.microsoft.com/en-us/magazine/cc300434.aspx
* 
* Extend OpenFileDialog and SaveFileDialog the easy way
* http://www.codeproject.com/KB/dialog/CustomizeFileDialog.aspx
* 
* How to: Save Files Using the SaveFileDialog Component
* http://msdn.microsoft.com/en-us/library/sfezx97z.aspx
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
using Microsoft.WindowsAPICodePack.Shell;
using System.IO;
#endregion


namespace CSShellCommonFileDialog
{
    public partial class CFDInBCLForm : Form
    {
        public CFDInBCLForm()
        {
            InitializeComponent();
        }

        #region Common Open File Dialogs

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnOpenAFile_Click(object sender, EventArgs e)
        {
            // Create a new common open file dialog
            OpenFileDialog openFileDialog = new OpenFileDialog();

            // (Optional) Set the title of the dialog
            openFileDialog.Title = "Select a File";

            // (Optional) Control the default folder of the file dialog
            // Here we set it as the Music library knownfolder
            openFileDialog.InitialDirectory = KnownFolders.MusicLibrary.Path;

            // (Optional) Specify file types for the file dialog, and set 
            // the selected file type index to Word Document.
            openFileDialog.Filter = "Word Files (*.docx)|*.docx|" + 
                "Text Files (*.txt)|*.txt|All Files (*.*)|*.*";
            openFileDialog.FilterIndex = 1;

            // (Optional) Set the default extension to be added as ".docx"
            openFileDialog.DefaultExt = "docx";

            // Show the dialog
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                // Get the selection from the user
                MessageBox.Show(openFileDialog.FileName, "The selected file is");
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnOpenFiles_Click(object sender, EventArgs e)
        {
            // Create a new common open file dialog
            OpenFileDialog openFileDialog = new OpenFileDialog();

            // Allow mult-selection
            openFileDialog.Multiselect = true;

            // (Optional) Set the title of the dialog
            openFileDialog.Title = "Select Files";

            // Show the dialog
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                // Get the selections from the user
                StringBuilder selectedFiles = new StringBuilder();
                foreach (string fileName in openFileDialog.FileNames)
                {
                    selectedFiles.AppendLine(fileName);
                }
                MessageBox.Show(selectedFiles.ToString(), "The selected files are");
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnOpenAFolder_Click(object sender, EventArgs e)
        {
            // Create a new common open file dialog
            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();

            // Show the dialog
            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                // Get the selection from the user
                MessageBox.Show(folderBrowserDialog.SelectedPath,
                    "The selected folder is");
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        /// <see cref="http://www.codeproject.com/KB/dialog/CustomizeFileDialog.aspx"/>
        private void btnAddCustomControls_Click(object sender, EventArgs e)
        {

        }

        /// <summary>
        /// The Common Places area in the Common File Dialog is extensible. 
        /// This code snippet demonstrates how to extend the Common Places 
        /// area.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAddCommonPlaces_Click(object sender, EventArgs e)
        {
            // Create a new common open file dialog
            OpenFileDialog openFileDialog = new OpenFileDialog();

            // Add the place (the PublicMusic knownfolder) to the default 
            // list in Common File Dialog.
            openFileDialog.CustomPlaces.Add(KnownFolders.PublicMusic.FolderId);
            
            // Show the dialog
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                // You can add your own code here to handle the results.
            }
        }

        #endregion

        #region Common Save File Dialogs

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnSaveAFile_Click(object sender, EventArgs e)
        {
            // Create a new common save file dialog
            SaveFileDialog saveFileDialog = new SaveFileDialog();

            // (Optional) Set the title of the dialog
            saveFileDialog.Title = "Save a File";

            // (Optional) Specify file types for the file dialog, and set 
            // the saved file type index to Word Document.
            saveFileDialog.Filter = "Word Files (*.docx)|*.docx|" +
                "Text Files (*.txt)|*.txt";
            saveFileDialog.FilterIndex = 1;

            // (Optional) Set the default extension to be added as ".docx"
            saveFileDialog.DefaultExt = "docx";

            // (Optional) Display a warning if the user specifies a file name 
            // that aleady exists. This is a default value for the Save dialog.
            saveFileDialog.OverwritePrompt = true;

            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                // Open and save to the file
                using (FileStream fs = (FileStream)saveFileDialog.OpenFile())
                {
                    // Write to the file stream.
                    // ...
                }
                MessageBox.Show(saveFileDialog.FileName, "The saved file is");
            }
        }

        #endregion
    }
}