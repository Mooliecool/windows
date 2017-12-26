/****************************** Module Header ******************************\
* Module Name:  MainForm.cs
* Project:      CSWin7TaskbarOverlayIcons
* Copyright (c) Microsoft Corporation.
* 
* Windows 7 Taskbar introduces Overlay Icons, which makes your application can 
* provide contextual status information to the user even if the application’s 
* window is not shown.  The user doesn’t even have to look at the thumbnail 
* or the live preview of your app – the taskbar button itself can reveal 
* whether you have any interesting status updates..
* 
* CSWin7TaskbarOverlayIcons example demostrates how to set and clear Taskbar
* Overlay Icons using Taskbar related APIs in Windows API Code Pack.
* 
* This MainForm enable the user select whether to show the Taskbar Overlay
* Icon and which icon to be displayed. 
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
using Microsoft.WindowsAPICodePack.Taskbar;
#endregion

namespace CSWin7TaskbarOverlayIcons
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }


        // Check the Windows version, if the system is not Windows 7 or
        // Windows Server 2008 R2, exit the current process.
        private void MainForm_Load(object sender, EventArgs e)
        {
            if (!TaskbarManager.IsPlatformSupported)
            {
                MessageBox.Show("Overlay Icon is not supported in your " + 
                    "operation system!" + Environment.NewLine + "Please " + 
                    "launch the application in Windows 7 or " + 
                    "Windows Server 2008 R2 systems.");
                
                // Exit the current process
                Application.Exit();
            }
        }


        // Status changed, the Overlay Icon should be updated
        private void statusComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            ShowOrHideOverlayIcon();
        }


        // Whether to show Overlay Icon is changed
        private void showIconCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            ShowOrHideOverlayIcon();
        }


        /// <summary>
        /// Show, hide and modify Taskbar button Overlay Icons
        /// </summary>
        private void ShowOrHideOverlayIcon()
        {
            // Show or hide the Overlay Icon
            if (showIconCheckBox.Checked)
            {
                Icon icon = null;

                // Select Overlay Icon image based on the selected status
                switch (statusComboBox.SelectedIndex)
                {
                    case 0:
                        icon = Properties.Resources.Available;
                        break;
                    case 1:
                        icon = Properties.Resources.Away;
                        break;
                    case 2:
                        icon = Properties.Resources.Offline;
                        break;
                    default:
                        MessageBox.Show("Please set the Status to show the Overlay Icon!");
                        break;
                }

                // Set the Taskbar Overlay Icon
                TaskbarManager.Instance.SetOverlayIcon(icon, statusComboBox.SelectedIndex.ToString());
            }
            else
                // Hide the Taskbar Overlay Icon
                TaskbarManager.Instance.SetOverlayIcon(null, null);
                
        }
    }
}
