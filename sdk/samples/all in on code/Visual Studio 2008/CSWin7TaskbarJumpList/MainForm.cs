/***************************************** Module Header *****************************************\
* Module Name:  MainForm.cs
* Project:      CSWin7TaskbarJumpList
* Copyright (c) Microsoft Corporation.
* 
* The Jump List feature is designed to provide you with quick access to the documents and tasks 
* associated with your applications. You can think of Jump Lists like little application-specific 
* Start menus. Jump Lists can be found on the application icons that appear on the Taskbar when an 
* application is running or on the Start menu in the recently opened programs section. Jump Lists 
* can also be found on the icons of applications that have been specifically pinned to the Taskbar 
* or the Start menu.
* 
* CSWin7TaskbarJumpList example demostrates how to set register Jump List file handle, add items 
* into Recent/Frequent known categories, add/remove user tasks, and add items/links into custom 
* categories in Windows 7 Taskbar Jump List using Taskbar related APIs in Windows API Code Pack.   
* 
* This MainForm can register the .txt file as the application Jump List file handle, add .txt file 
* into Recent/Frequent known categories, add/remove notepad.exe, mspaint.exe, calc.exe as user 
* tasks, and add items/links into custom categories in Windows 7 Taskbar Jump List.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*************************************************************************************************/

#region Using directive
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;
using Microsoft.WindowsAPICodePack.Taskbar;
using System.Reflection;
using Microsoft.WindowsAPICodePack.Shell;
using System.IO;
using System.Diagnostics;
#endregion


namespace CSWin7TaskbarJumpList
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        // The Application ID for the current application. The AppID is necessary since we need 
        // to register the file extension based on the specific AppID
        const string AppID = "All-In-One Code Framework.Win7Taskbar.CSWin7TaskbarJumpList";

        private JumpList _jumpList;  // Jump List instance


        // Jump List custom category instance
        private JumpListCustomCategory _currentCategory;


        // Private readonly property of the Jump List instance
        private JumpList JumpList
        {
            get
            {
                // Create a new Jump List instance if it is null
                if (_jumpList == null)
                {
                    _jumpList = JumpList.CreateJumpList();  // Create Jump List instance

                    // Set the known category based on the radio button checked value
                    _jumpList.KnownCategoryToDisplay = radRecent.Checked ? 
                        JumpListKnownCategoryType.Recent : JumpListKnownCategoryType.Frequent;

                    _jumpList.Refresh();  // Refresh the Jump List
                }
                return _jumpList;
            }
        }


        // Check the Windows version, if it is Windows 7 or Windows Server 2008 R2, set the 
        // AppID and update the UI, otherwise exit the process
        private void MainForm_Load(object sender, EventArgs e)
        {
            // Check whether the current system is Windows 7 or Windows Server 2008 R2
            if (TaskbarManager.IsPlatformSupported)
            {
                // Set the AppID (For detail, please see CSWin7TaskbarAppID example)
                TaskbarManager.Instance.ApplicationId = AppID;

                // Update the UI
                chkNotepad.Checked = true;
                chkCalc.Checked = true;
                chkPaint.Checked = true;
                btnClearTask.Enabled = false;
                btnAddItem.Enabled = false;
                btnAddLink.Enabled = false;
                tbItem.Enabled = false;
                tbLink.Enabled = false;
            }
            else
            {
                MessageBox.Show("Taskbar Application ID is not supported in your operation system!" +
                    "\r\nPlease launch the application in Windows 7 or Windows Server 2008 R2 systems.");
                Application.Exit();  // Exit the current process
            }
        }


        // Register the Jump List file handle for the application
        private void btnRegisterFileType_Click(object sender, EventArgs e)
        {
            // Check whether the application ID has been registered
            if (HelperMethod.IsApplicationRegistered(TaskbarManager.Instance.ApplicationId))
            {
                MessageBox.Show(".txt file type has been registered!");
                return;
            }

            // Check whether the application is runas Admin, since we need the Admin privilege 
            // to modify the HKCR registry values
            if (!HelperMethod.IsAdmin())
            {
                // Ask the user whether to elevate the application
                var result = MessageBox.Show("This operation needs Admin privilege!\r\n" +
                    "Restart and run the application as Admin?", "Warning!", 
                    MessageBoxButtons.YesNo);

                if (result == DialogResult.Yes)
                {
                    try
                    {
                        // Call helper method to restart the application as Admin
                        HelperMethod.RestartApplicationAsAdmin();
                    }
                    catch
                    {
                        return;
                    }
                    Application.Exit();  // Kill the current application instance
                }
                else return;
            }

            // If the application is runas Admin
            try
            {
                // Call helper method to register the .txt file handle
                HelperMethod.RegisterFileAssociations(TaskbarManager.Instance.ApplicationId,
                    false, TaskbarManager.Instance.ApplicationId, 
                    Assembly.GetExecutingAssembly().Location, ".txt");
                MessageBox.Show(".txt file type is registered successfully!");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error registering file type association:\r\n" + ex.Message);
            }
        }


        // Unregister the Jump List file handle
        private void btnUnregisterFileType_Click(object sender, EventArgs e)
        {
            // Check whether the application ID has been registered
            if (!HelperMethod.IsApplicationRegistered(TaskbarManager.Instance.ApplicationId))
            {
                MessageBox.Show(".txt file type has not been registered yet!");
                return;
            }

            // Check whether the application is runas Admin, since we need the Admin privilege  
            // to modify the HKCR registry values
            if (!HelperMethod.IsAdmin())
            {
                // Ask the user whether to elevate the application
                var result = MessageBox.Show("This operation needs Admin privilege!" + 
                    "\r\nRestart and run the application as Admin?", "Warning!", 
                    MessageBoxButtons.YesNo);

                if (result == DialogResult.Yes)
                {
                    try
                    {
                        // Call helper method to restart the application as Admin
                        HelperMethod.RestartApplicationAsAdmin();
                    }
                    catch
                    {
                        return;
                    }
                    Application.Exit();  // Kill the current application instance
                }
                else return;
            }

            // If the application is runas Admin
            try
            {
                // Call helper method to unregister the .txt file handle
                HelperMethod.UnregisterFileAssociations(TaskbarManager.Instance.ApplicationId,
                    false, TaskbarManager.Instance.ApplicationId,
                    Assembly.GetExecutingAssembly().Location, ".txt");
                MessageBox.Show(".txt file type is unregistered successfully!");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error unregistering file type association:\r\n" + ex.Message);
            }
        }


        // Update the known category (Recent/Frequent) based on the Radio button status
        private void radFrequentRecent_CheckedChanged(object sender, EventArgs e)
        {
            JumpList.KnownCategoryToDisplay = radFrequent.Checked ?
                JumpListKnownCategoryType.Frequent : JumpListKnownCategoryType.Recent;
            JumpList.Refresh();
        }
       

        // Open an OpenFileDialog to make the .txt file show in the known category 
        // (Recent/Frequent)
        private void btnChooseFile_Click(object sender, EventArgs e)
        {
            if (recentFileOpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                radRecent.Checked = true;
            }
        }


        // Add user tasks based on the three CheckBox (notepad, paint, calculator)
        private void btnAddTask_Click(object sender, EventArgs e)
        {
            try
            {
                // Retrieve the system folder
                string systemFolder = Environment.GetFolderPath(Environment.SpecialFolder.System);

                // Add the notepad.exe user task
                if (chkNotepad.Checked)
                {
                    // Create a user task instance of the notepad.exe
                    IJumpListTask notepadTask = new JumpListLink(Path.Combine(systemFolder, 
                        "notepad.exe"), "Open Notepad")
                    {
                        // Set the icon
                        IconReference = new IconReference(Path.Combine(systemFolder, 
                            "notepad.exe"), 0)
                    };

                    // Add the notepad user task into the Jump List
                    JumpList.AddUserTasks(notepadTask);
                }

                // Add the calc.exe user task
                if (chkCalc.Checked)
                {
                    // Create a user task instance of the calc.exe
                    IJumpListTask calcTask = new JumpListLink(Path.Combine(systemFolder,
                        "calc.exe"), "Open Calculator")
                    {
                        // Set the icon
                        IconReference = new IconReference(Path.Combine(systemFolder,
                            "calc.exe"), 0)
                    };
                    JumpList.AddUserTasks(calcTask); // Add the calculator into the Jump List
                }

                // Add the mspaint.exe user task and a Jump List separator
                if (chkPaint.Checked)
                {
                    // Create a user task instance of the mspaint.exe
                    IJumpListTask paintTask = new JumpListLink(Path.Combine(systemFolder,
                        "mspaint.exe"), "Open Paint")
                    {
                        // Set the icon
                        IconReference = new IconReference(Path.Combine(systemFolder,
                            "mspaint.exe"), 0)
                    };

                    if (chkNotepad.Checked || chkCalc.Checked)
                    {
                        // Add a Jump List separator and the paint user task
                        JumpList.AddUserTasks(new JumpListSeparator(), paintTask);
                    }
                    else
                    {
                        JumpList.AddUserTasks(paintTask);  // Only add the paint user task
                    }
                }

                // Refresh the Jump List instance and update the UI
                JumpList.Refresh();
                btnClearTask.Enabled = true;
                btnAddTask.Enabled = false;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }


        // Clear all the user tasks
        private void btnClearTask_Click(object sender, EventArgs e)
        {
            try
            {
                JumpList.ClearAllUserTasks();  // Clear all the user tasks

                // Refresh the Jump List instance and update the UI
                JumpList.Refresh();
                btnAddTask.Enabled = true;
                btnClearTask.Enabled = false;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }


        // Create a custom category
        private void btnCreateCategory_Click(object sender, EventArgs e)
        {
            try
            {
                // Create a new custom category based on the category name in tbCategory
                _currentCategory = new JumpListCustomCategory(tbCategory.Text);

                // Add the custom category instance into the Jump List
                JumpList.AddCustomCategories(_currentCategory);

                // Refresh the Jump List instance and update the UI
                JumpList.Refresh();
                btnAddItem.Enabled = true;
                btnAddLink.Enabled = true;
                tbItem.Enabled = true;
                tbLink.Enabled = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }


        // Add a custom shell item to the custom category
        private void btnAddItem_Click(object sender, EventArgs e)
        {
            try
            {
                // Check the if the file name is valid
                if (!HelperMethod.CheckFileName(tbItem.Text))
                    return;

                // Create a .txt file in the temp foler and create a shell item for this file
                JumpListItem jli = new JumpListItem(HelperMethod. GetTempFileName(tbItem.Text));

                // Add the shell item to the custom category
                _currentCategory.AddJumpListItems(jli);

                JumpList.Refresh();  // Refresh the Jump List
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }


        //Add a custom shell link to the custom category
        private void btnAddLink_Click(object sender, EventArgs e)
        {
            try
            {
                // Check the if the file name is valid
                if (!HelperMethod.CheckFileName(tbLink.Text))
                    return;

                // Create a .txt file in the temp foler and create a shell link for this file
                JumpListLink jli = new JumpListLink(HelperMethod.GetTempFileName(tbLink.Text), 
                    tbLink.Text);

                // Add the shell link to the custom category
                _currentCategory.AddJumpListItems(jli);

                JumpList.Refresh();  // Refresh the Jump List
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
    }
}