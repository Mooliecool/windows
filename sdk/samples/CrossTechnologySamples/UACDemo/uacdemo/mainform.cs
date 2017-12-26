using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using Microsoft.Win32;
using System.Security.Principal;
using System.Diagnostics;
using Microsoft.SDK.Samples.VistaBridge.Library;
using Microsoft.SDK.Samples.VistaBridge.Services;
using Microsoft.SDK.Samples.VistaBridge.Interop;
using System.Runtime.InteropServices;

namespace UACDemo
{
    public partial class MainForm : Form
    {
        #region Form Initialization
        public MainForm()
        {
            InitializeComponent();
        }

        private void OnFormLoad(object sender, EventArgs e)
        {
            AppDomain.CurrentDomain.SetPrincipalPolicy(
                PrincipalPolicy.WindowsPrincipal);
            txtUser.Text = System.Threading.Thread.CurrentPrincipal.Identity.Name;
            InteropHelper.SetWindowsFormsButtonShield(button1, true);
        }

        #endregion

        #region Write To Program Files
        private void OnWriteToProgramFiles(object sender, EventArgs e)
        {
            try
            {
                string folderPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles), "UACDemo");

                if (!Directory.Exists(folderPath))
                {
                    Directory.CreateDirectory(folderPath);
                }

                string filePath = Path.Combine(folderPath, "log.txt");

                using (TextWriter tw = new StreamWriter(filePath, true))
                {
                    tw.WriteLine("entry added " + DateTime.Now);
                }

                MessageBox.Show("Entry has successfully been added to Log.txt", "Write To Program Files", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (System.Security.SecurityException secEx)
            {
                MessageBox.Show(string.Format("SecurityException: {0}", secEx.Message), "Write To Program Files", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (UnauthorizedAccessException authEx)
            {
                MessageBox.Show(string.Format("UnauthorizedAccessException: {0}", authEx.Message), "Write To Program Files", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Write To Program Files", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        #endregion

        #region Write to HKLM
        private void OnWriteToHKLM(object sender, EventArgs e)
        {
            try
            {
                using (RegistryKey registryKey = Registry.LocalMachine.CreateSubKey(
                    @"Software\UACDemo", RegistryKeyPermissionCheck.ReadWriteSubTree))
                {

                    registryKey.SetValue("SampleValue", "Updated at " + DateTime.Now);
                }

                MessageBox.Show("Registry key has successfully been updated", "Write To Registry HKLM", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (System.Security.SecurityException secEx)
            {
                MessageBox.Show(string.Format("SecurityException: {0}", secEx.Message), "Write To Registry HKLM", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (UnauthorizedAccessException authEx)
            {
                MessageBox.Show(string.Format("UnauthorizedAccessException: {0}", authEx.Message), "Write To Registry HKLM", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Write To Registry HKLM", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        #endregion

        #region IsAdministrator
        private void OnIsAdministrator(object sender, EventArgs e)
        {
            WindowsIdentity wi = WindowsIdentity.GetCurrent();
            WindowsPrincipal wp = new WindowsPrincipal(wi);

            if (wp.IsInRole("Administrators"))
            {
                MessageBox.Show("Yes, you are an Administrator.", "Is Administrator?", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show("No, you are not an Administrator.", "Is Administrator?", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        #endregion

        #region Launch Privileged Exe
        private void OnLaunchPrivilegedExecutable(object sender, EventArgs e)
        {
            int exitCode;
            try
            {
                exitCode = UACManager.ExecutePrivilegedProcess(GetPrivilegedExePath());
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return;
            }

            if (exitCode == 0)
            {
                MessageBox.Show("PrivilegedExe has been run successfully", "Write To Program Files", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show(String.Format("PrivilegedExe has returned with error code: {0}", exitCode),
                    "Write To Program Files", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            
        }
        
        private string GetPrivilegedExePath()
        {
            // Determine path of executable to launch
            // First, look in same folder
            string folder = Path.GetDirectoryName(
                System.Reflection.Assembly.GetEntryAssembly().Location);
            string executablePath = Path.Combine(folder, "PrivilegedExe.exe");

            // If not found in same folder as calling app, 
            // assume it's in a sister VS project in the same solution
            if (!File.Exists(executablePath))
            {
                // Go up three levels in the file system
                folder = Path.GetDirectoryName(folder);
                folder = Path.GetDirectoryName(folder);
                folder = Path.GetDirectoryName(folder);

                // Go to output folder
                folder = Path.Combine(folder, @"PrivilegedExe\bin\debug");
                executablePath = Path.Combine(folder, "PrivilegedExe.exe");
            }

            return (executablePath);
        }
        #endregion
    }
}