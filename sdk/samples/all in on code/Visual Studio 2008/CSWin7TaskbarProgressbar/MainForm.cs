/****************************** Module Header ******************************\
* Module Name:  MainForm.cs
* Project:      CSWin7TaskbarProgressBar
* Copyright (c) Microsoft Corporation.
* 
* Windows 7 Taskbar introduces Taskbar Progress Bar, which makes your 
* application can provide contextual status information to the user even if 
* the application’s window is not shown.  The user doesn’t even have to 
* look at the thumbnail or the live preview of your app – the taskbar button 
* itself can reveal whether you have any interesting status updates..
* 
* CSWin7TaskbarProgressBar example demostrates how to set Taskbar Progress
* Bar state (normal, pause, indeterminate, error) and value, and flash window.  
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
using System.Runtime.InteropServices;
#endregion

namespace CSWin7TaskbarProgressbar
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        // The max flash window count
        const int MAX_FLASH_WINDOW_COUNT = 6;

        // The max progress bar value
        const int MAX_PROGRESSBAR_VALUE = 100;

        // The current flash window count
        private int count = 0;


        // P/Invoke the Windows API FlashWindow to flash Taskbar button
        [DllImport("user32.dll")]
        private static extern bool FlashWindow(IntPtr hwnd, bool bInvert);


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


        // Start the timer to let the WinForm ProgressBar and Taskbar button
        // Progress Bar
        private void startButton_Click(object sender, EventArgs e)
        {
            progreeBarTimer.Enabled = true;
        }


        // Update the WinForm Progress Bar and Taskbar button ProgressBar
        // value every 100ms
        private void progreeBarTimer_Tick(object sender, EventArgs e)
        {
            UpdateProgress();
        }

        
        // Stop the timer to pause the WinForm ProgressBar and Taskbar
        // button Progress Bar
        private void pauseButton_Click(object sender, EventArgs e)
        {
            // Stop the timer
            progreeBarTimer.Enabled = false;

            // Set the Taskbar button Progress Bar state to Paused
            TaskbarManager.Instance.SetProgressState(
                TaskbarProgressBarState.Paused);

            // Set the Taskbar button Progress Bar value
            TaskbarManager.Instance.SetProgressValue(
                progressBar.Value, MAX_PROGRESSBAR_VALUE);
        }


        // Stop the timer and to pause the WinForm ProgressBar and make the
        // Taskbar Progress Bar state to Indeterminate
        private void indeterminateButton_Click(object sender, EventArgs e)
        {
            // Stop the timer
            progreeBarTimer.Enabled = false;

            // Set the Taskbar button Progress Bar state to Indeterminate
            TaskbarManager.Instance.SetProgressState(
                TaskbarProgressBarState.Indeterminate);
        }


        // Stop the timer and to pause the WinForm ProgressBar and make the
        // Taskbar Progress Bar state to Error
        private void errorButton_Click(object sender, EventArgs e)
        {
            // Stop the timer
            progreeBarTimer.Enabled = false;

            // Set the Taskbar button Progress Bar state to Error
            TaskbarManager.Instance.SetProgressState(
                TaskbarProgressBarState.Error);

            // Set the Taskbar button Progress Bar value
            TaskbarManager.Instance.SetProgressValue(
                progressBar.Value, MAX_PROGRESSBAR_VALUE);
        }


        // Make the Taskbar button flash several times
        private void flashButton_Click(object sender, EventArgs e)
        {
            flashWindowTimer.Enabled = true;
        }


        // Make the Taskbar button flash (MAX_FLASH_WINDOW_COUNT / 2) times
        private void flashWindowTimer_Tick(object sender, EventArgs e)
        {
            if (count++ < MAX_FLASH_WINDOW_COUNT)
            {
                // Make the window flash or return to the original state
                FlashWindow(this.Handle, (count % 2) == 0);
            }
            else
            {
                // Restore the count variable
                count = 0;

                // Stop the timer
                flashWindowTimer.Enabled = false;
            }
        }


        /// <summary>
        /// Update the WinForm ProgressBar and Taskbar button Progress Bar
        /// value
        /// </summary>
        private void UpdateProgress()
        {
            if (progressBar.Value < MAX_PROGRESSBAR_VALUE)
            {
                // Increase the WinForm ProgressBar value
                progressBar.Value += 1;
            }
            else
            {
                // Clear the WinForm ProgressBar value
                progressBar.Value = 0;
            }

            // Set the Taskbar button Progress Bar state to Normal
            TaskbarManager.Instance.SetProgressState(
                TaskbarProgressBarState.Normal);

            // Set the Taskbar button Progress Bar value
            TaskbarManager.Instance.SetProgressValue(
                progressBar.Value, MAX_PROGRESSBAR_VALUE);
        }
    }
}
