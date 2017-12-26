/********************************** Module Header **********************************\
* Module Name:  MainForm.cs
* Project:      CSRunProcessAsUser
* Copyright (c) Microsoft Corporation.
* 
* The MainForm.cs file is backstage code which works for CSRunProcessAsUser main form.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***********************************************************************************/

using System;
using System.Windows.Forms;
using System.Diagnostics;
using System.Security;
using System.Runtime.InteropServices;
using System.ComponentModel;


namespace CSRunProcessAsUser
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Input execution command line using OpenFileDialog object
        /// </summary>
        private void btnCommand_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog ofdOpen = new OpenFileDialog())
            {
                if (ofdOpen.ShowDialog(this) == DialogResult.OK)
                {
                    tbCommand.Text = ofdOpen.FileName;
                }
            }
        }

        /// <summary>
        /// Run the process as the specified user using Process.Start.
        /// </summary>
        private void btnRunCommand_Click(object sender, EventArgs e)
        {
            try
            {
                // Check the parameters.
                if (!string.IsNullOrEmpty(tbUserName.Text) &&
                    !string.IsNullOrEmpty(tbPassword.Text) &&
                    !string.IsNullOrEmpty(tbCommand.Text))
                {
                    SecureString password = StringToSecureString(tbPassword.Text);
                    Process proc = Process.Start(
                        this.tbCommand.Text,
                        this.tbUserName.Text,
                        password,
                        this.tbDomain.Text);

                    ProcessStarted(proc.Id);

                    proc.EnableRaisingEvents = true;
                    proc.Exited += new EventHandler(ProcessExited);
                }
                else
                {
                    MessageBox.Show("Please fill in the user name, password and command");
                }
            }
            catch (Win32Exception w32e)
            {
                ProcessStartFailed(w32e.Message);
            }
        }

        /// <summary>
        /// Triggered when the target process is started.
        /// </summary>
        private void ProcessStarted(int processId)
        {
            MessageBox.Show("Process " + processId.ToString() + " started");
        }

        /// <summary>
        /// Triggered when the target process is exited.
        /// </summary>
        private void ProcessExited(object sender, EventArgs e)
        {
            Process proc = sender as Process;
            if (proc != null)
            {
                MessageBox.Show("Process " + proc.Id.ToString() + " exited");
            }
        }

        /// <summary>
        /// Triggered when the target process failed to be started.
        /// </summary>
        private void ProcessStartFailed(string error)
        {
            MessageBox.Show(error, "Process failed to start",
                MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        /// <summary>
        /// Demonstrate how to popup the credential prompt in order to input 
        /// the user credential.
        /// </summary>
        private void btnCredentialUIPrompt_Click(object sender, EventArgs e)
        {
            try
            {
                using (Kerr.PromptForCredential dialog = new Kerr.PromptForCredential())
                {
                    dialog.Title = "Please specify the user";
                    dialog.DoNotPersist = true;
                    dialog.ShowSaveCheckBox = false;
                    dialog.TargetName = Environment.MachineName;
                    dialog.ExpectConfirmation = true;

                    if (DialogResult.OK == dialog.ShowDialog(this))
                    {
                        tbPassword.Text = SecureStringToString(dialog.Password);
                        string[] strSplit = dialog.UserName.Split('\\');
                        if (strSplit.Length == 2)
                        {
                            this.tbUserName.Text = strSplit[1];
                            this.tbDomain.Text = strSplit[0];
                        }
                        else
                        {
                            this.tbUserName.Text = dialog.UserName;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString(), "Error");
            }
        }

        /// <summary>
        /// Helper function. It converts SecureString to String
        /// </summary>
        static String SecureStringToString(SecureString secureStr)
        {
            IntPtr bstr = Marshal.SecureStringToBSTR(secureStr);
            try
            {
                return Marshal.PtrToStringBSTR(bstr);
            }
            finally
            {
                Marshal.FreeBSTR(bstr);
            }
        }

        /// <summary>
        /// Helper function. It converts String to SecureString
        /// </summary>
        static SecureString StringToSecureString(String str)
        {
            SecureString secureStr = new SecureString();
            char[] chars = str.ToCharArray();
            for (int i = 0; i < chars.Length; i++)
            {
                secureStr.AppendChar(chars[i]);
            }
            return secureStr;
        }
    }
}
