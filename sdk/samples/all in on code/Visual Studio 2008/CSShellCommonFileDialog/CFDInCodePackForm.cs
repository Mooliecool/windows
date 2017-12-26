/****************************** Module Header ******************************\
* Module Name:  CFDInCodePackForm.cs
* Project:      CSShellCommonFileDialog
* Copyright (c) Microsoft Corporation.
* 
* 
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
using Microsoft.WindowsAPICodePack.Dialogs;
using Microsoft.WindowsAPICodePack.Shell;
using Microsoft.WindowsAPICodePack.Dialogs.Controls;
using System.IO;
#endregion


namespace CSShellCommonFileDialog
{
    public partial class CFDInCodePackForm : Form
    {
        public CFDInCodePackForm()
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
            CommonOpenFileDialog openFileDialog = new CommonOpenFileDialog();

            // (Optional) Set the title of the dialog
            openFileDialog.Title = "Select a File";

            // (Optional) Control the default folder of the file dialog
            // Here we set it as the Music library knownfolder
            openFileDialog.InitialDirectoryShellContainer = 
                KnownFolders.MusicLibrary as ShellContainer;

            // (Optional) Specify file types for the file dialog
            openFileDialog.Filters.Add(new CommonFileDialogFilter(
                "Word Files", "*.docx"));
            openFileDialog.Filters.Add(new CommonFileDialogFilter(
                "Text Files", "*.txt"));
            openFileDialog.Filters.Add(new CommonFileDialogFilter(
                "All Files", "*.*"));

            // (Optional) Set the default extension to be added as ".docx"
            openFileDialog.DefaultExtension = "docx";

            // Show the dialog
            if (openFileDialog.ShowDialog() == CommonFileDialogResult.OK)
            {
                // Get the selection from the user
                ShellObject shellObj = openFileDialog.FileAsShellObject;
                MessageBox.Show(shellObj.ParsingName, "The selected file is");
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
            CommonOpenFileDialog openFileDialog = new CommonOpenFileDialog();

            // Set the dialog as a folder picker
            openFileDialog.IsFolderPicker = true;

            // (Optional) Set the title of the dialog
            openFileDialog.Title = "Select a Folder";

            // Show the dialog
            if (openFileDialog.ShowDialog() == CommonFileDialogResult.OK)
            {
                // Get the selection from the user
                ShellObject shellObj = openFileDialog.FileAsShellObject;
                MessageBox.Show(shellObj.ParsingName, "The selected folder is");
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
            CommonOpenFileDialog openFileDialog = new CommonOpenFileDialog();

            // Allow mult-selection
            openFileDialog.Multiselect = true;

            // (Optional) Set the title of the dialog
            openFileDialog.Title = "Select Files";

            // Show the dialog
            if (openFileDialog.ShowDialog() == CommonFileDialogResult.OK)
            {
                // Get the selections from the user
                StringBuilder selectedFiles = new StringBuilder();
                foreach (ShellObject shellObj in openFileDialog.FilesAsShellObject)
                {
                    selectedFiles.AppendLine(shellObj.ParsingName);
                }
                MessageBox.Show(selectedFiles.ToString(), "The selected files are");
            }
        }

        /// <summary>
        /// This code snippet demonstrates how to add custom controls in the Common File 
        /// Dialog.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnAddCustomControls_Click(object sender, EventArgs e)
        {
            // Create a new common open file dialog
            CommonOpenFileDialog openFileDialog = new CommonOpenFileDialog();

            // Prepare the controls to be added and the relationship among 
            // the controls
            CommonFileDialogGroupBox grp = new CommonFileDialogGroupBox(
                "grp", "Change Title to ");
            CommonFileDialogRadioButtonList radl = new 
                CommonFileDialogRadioButtonList("radl");
            grp.Items.Add(radl);
            radl.Items.Add(new CommonFileDialogRadioButtonListItem("Windows Vista"));
            radl.Items.Add(new CommonFileDialogRadioButtonListItem("Windows 7"));
            radl.SelectedIndexChanged += new EventHandler(radl_SelectedIndexChanged);

            // Add the firt-level control to the CommonFileDialog.Controls 
            // collection to apply the changes of controls.
            openFileDialog.Controls.Add(grp);

            // Show the dialog
            if (openFileDialog.ShowDialog() == CommonFileDialogResult.OK)
            {
                // Get the value of the controls added to the dialog
                if (radl.SelectedIndex >= 0)
                {
                    MessageBox.Show(radl.Items[radl.SelectedIndex].Text, 
                        "The selected Title is");
                }
                else
                {
                    MessageBox.Show("No Title was selected");
                }
            }
        }

        /// <summary>
        /// This method gets called when selection happens to an dialog 
        /// control item, radio-button. For sample sake, we change the dialog 
        /// title according to the current selection in the readio button 
        /// list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void radl_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Get the radio button list control that triggers the selection-
            // changed event
            CommonFileDialogRadioButtonList radl = sender as CommonFileDialogRadioButtonList;
            if (radl != null)
            {
                // Get the parent common file dialog object
                CommonOpenFileDialog openFileDialog = radl.HostingDialog 
                    as CommonOpenFileDialog;
                if (openFileDialog != null)
                {
                    // Change the title of the dialog based on the radio 
                    // button selection
                    openFileDialog.Title = radl.Items[radl.SelectedIndex].Text;
                }
            }
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
            CommonOpenFileDialog openFileDialog = new CommonOpenFileDialog();

            // Add the place (the PublicMusic knownfolder) to the bottom of 
            // default list in Common File Dialog.
            openFileDialog.AddPlace(KnownFolders.PublicMusic as ShellContainer, 
                FileDialogAddPlaceLocation.Bottom);

            // Show the dialog
            if (openFileDialog.ShowDialog() == CommonFileDialogResult.OK)
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
            CommonSaveFileDialog saveFileDialog = new CommonSaveFileDialog();

            // (Optional) Set the title of the dialog
            saveFileDialog.Title = "Save a File";

            // (Optional) Specify file types for the file dialog
            saveFileDialog.Filters.Add(new CommonFileDialogFilter(
                "Word Files", "*.docx"));
            saveFileDialog.Filters.Add(new CommonFileDialogFilter(
                "Text Files", "*.txt"));

            // (Optional) Set the default extension to be added as ".docx"
            saveFileDialog.DefaultExtension = "docx";

            // (Optional) Display a warning if the user specifies a file name 
            // that aleady exists. This is a default value for the Save dialog.
            saveFileDialog.OverwritePrompt = true;

            // Show the dialog
            if (saveFileDialog.ShowDialog() == CommonFileDialogResult.OK)
            {
                // Open and save to the file
                using (FileStream fs = new FileStream(saveFileDialog.FileName, 
                    FileMode.Create))
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
