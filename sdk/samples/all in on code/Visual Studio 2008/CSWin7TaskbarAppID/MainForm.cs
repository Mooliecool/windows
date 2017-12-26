/****************************** Module Header ******************************\
* Module Name:  MainForm.cs
* Project:      CSWin7TaskbarAppID
* Copyright (c) Microsoft Corporation.
* 
* Application User Model IDs (AppUserModelIDs) are used extensively by the 
* taskbar in Windows 7 and later systems to associate processes, files, and 
* windows with a particular application. In some cases, it is sufficient to 
* rely on the internal AppUserModelID assigned to a process by the system. 
* However, an application that owns multiple processes or an application that 
* is running in a host process might need to explicitly identify itself so 
* that it can group its otherwise disparate windows under a single taskbar 
* button and control the contents of that application's Jump List.
* 
* CSWin7TaskbarAppID example demostrates how to set process level Application
* User Model IDs (AppUserModelIDs or AppIDs) and modify the AppIDs for a 
* specific window using Taskbar related APIs in Windows API Code Pack.
* 
* This MainForm sets a process level AppID for the current process.  It can 
* create some SubForms and set specific AppIDs for these SubForms, so the
* SubForms will be in different group of the MainForm's Taskbar button.  
* It also can modify the SubForms AppIDs back to the MainForm's AppID, so 
* the SubForms Taskbar buttons will be in the same group of theMainForm.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directive
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using Microsoft.WindowsAPICodePack.Taskbar;
#endregion

namespace CSWin7TaskbarAppID
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        // The AppID for the current process
        const string MainFormAppID = "All-In-One Code Framework.CSWin7AppID";

        // The AppID for the SubForms
        const string SubFormAppID = MainFormAppID + ".SubForm";

        // A generic List holding the SubForm references
        public List<SubForm> SubFormList { get; set; }


        // Check the Windows version, then set the current process's AppID or
        // exit the current process
        private void MainForm_Load(object sender, EventArgs e)
        {
            // Check whether the current system is Windows 7 or 
            // Windows Server 2008 R2
            if (TaskbarManager.IsPlatformSupported)
            {
                // Set the AppID for the current process, it calls the 
                // Windows API SetCurrentProcessExplicitAppUserModelID
                // TaskbarManager.Instance represents an instance of the
                // Windows Taskbar
                TaskbarManager.Instance.ApplicationId = MainFormAppID;

                // Set the Title of the MainForm to the AppID
                this.Text = TaskbarManager.Instance.ApplicationId;

                // Initialize the list holding the SubForms references
                SubFormList = new List<SubForm>();
            }
            else
            {
                MessageBox.Show("Taskbar Application ID is not supported in" 
                    + " your operation system!" + Environment.NewLine +
                    "Please launch the application in Windows 7 or " + 
                    "Windows Server 2008 R2 systems.");

                // Exit the current process
                Application.Exit();
            }
        }


        // Create a SubForm and set a new AppID for it
        private void openSubFormButton_Click(object sender, EventArgs e)
        {
            // Create a new SubForm
            SubForm subForm = new SubForm();

            // Set the SubForm's AppID, it calls Windows API
            // SHGetPropertyStoreForWindow
            TaskbarManager.Instance.SetApplicationIdForSpecificWindow(
                subForm.Handle, SubFormAppID);

            // Set the SubForm Title to the new AppID
            subForm.Text = SubFormAppID;

            // Display the SubForm
            subForm.Show();

            // Add this SubForm's reference into list
            SubFormList.Add(subForm);

            // Update the buttons' enable status
            resetSubFormAppIDButton.Enabled = true;
            setSubFormAppIDButton.Enabled = false;
        }


        // Set all the SubForms's AppIDs to the SubFormAppID
        private void setSubFormAppIDButton_Click(object sender, EventArgs e)
        {
            // Set all the SubForms's AppIDs and update the button enable 
            // status
            if (SetAllSubFormAppIDs(SubFormAppID))
            {
                resetSubFormAppIDButton.Enabled = true;
            }
            setSubFormAppIDButton.Enabled = false;
        }


        // Reset all the SubForm's AppIDs to the MainFormAppID
        private void resetSubFormAppIDButton_Click(object sender, EventArgs e)
        {
            // Set all the SubForms's AppIDs and update the button enable 
            // status
            if (SetAllSubFormAppIDs(MainFormAppID))
            {
                setSubFormAppIDButton.Enabled = true;
            }
            resetSubFormAppIDButton.Enabled = false;
        }


        /// <summary>
        /// Set all the SubForms' AppID
        /// </summary>
        /// <param name="AppID">The AppID to be set</param>
        /// <returns>Whether the operation successes</returns>
        private bool SetAllSubFormAppIDs(string AppID)
        {
            // Check whether there are any SubForms exist
            if (SubFormList.Count > 0)
            {
                foreach (var subForm in SubFormList)
                {
                    // Set each SubForm's AppID, it calls Windows API
                    // SHGetPropertyStoreForWindow
                    TaskbarManager.Instance.SetApplicationIdForSpecificWindow(
                        subForm.Handle, AppID);

                    // Set the SubForm Title to the new AppID 
                    subForm.Text = AppID;
                }
                return true;
            }
            else
            {
                MessageBox.Show("No SubForms now!");
                return false;
            }
        }
    }
}
