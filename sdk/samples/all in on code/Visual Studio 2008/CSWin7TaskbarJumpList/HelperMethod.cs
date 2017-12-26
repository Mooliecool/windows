/************************************** Module Header *************************************\
* Module Name:  HelperMethods.cs
* Project:      CSWin7TaskbarJumpList
* Copyright (c) Microsoft Corporation.
* 
* The file contains the helper methods tohandle Admin session check, restart application to 
* elevate the user session, register/unregister application ID and file handle, validate 
* file name, and create files under system temp folder.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/

#region Using directive
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Win32;
using System.Windows.Forms;
using System.IO;
using System.Security.Principal;
using System.Diagnostics;
#endregion


namespace CSWin7TaskbarJumpList
{
    public static class HelperMethod
    {
        // Static registry key (HKCU or HKCR)
        static RegistryKey classesRoot;

        // Helper method to check whether the current application is runas Admin
        public static bool IsAdmin()
        {
            WindowsIdentity id = WindowsIdentity.GetCurrent();
            WindowsPrincipal principal = new WindowsPrincipal(id);
            return principal.IsInRole(WindowsBuiltInRole.Administrator);
        }

        // Helper method to restart the current application to evelate it
        public static void RestartApplicationAsAdmin()
        {
            ProcessStartInfo proc = new ProcessStartInfo();
            proc.UseShellExecute = true;
            // Get the current application directory
            proc.WorkingDirectory = Environment.CurrentDirectory;
            // Get the current application executable file path
            proc.FileName = Application.ExecutablePath;
            proc.Verb = "runas";  // Elevate the privilege
            Process.Start(proc);  // Restart the current process
        }

        // Helper method to check whether the application has registered some file handle
        public static bool IsApplicationRegistered(string appId)
        {
            try
            {
                // Open the current application's AppID key under HKCR
                using (RegistryKey progIdKey = Registry.ClassesRoot.OpenSubKey(appId))
                {
                    return progIdKey != null;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, ex.Source);
            }
            return false;
        }

        // Call InternalRegisterFileAssociations to register the file handle
        public static void RegisterFileAssociations(string progId, bool registerInHKCU, 
            string appId, string openWith, params string[] extensions)
        {
            InternalRegisterFileAssociations(false, progId, registerInHKCU, appId, openWith, extensions);
        }

        // Call InternalRegisterFileAssociations method to unregister the file handle
        public static void UnregisterFileAssociations(string progId, bool registerInHKCU,
            string appId, string openWith, params string[] extensions)
        {
            InternalRegisterFileAssociations(true, progId, registerInHKCU, appId, openWith, extensions);
        }

        // Private helper method to register/unregister application file handle
        private static void InternalRegisterFileAssociations(bool unregister, string progId,
            bool registerInHKCU, string appId, string openWith, string[] associationsToRegister)
        {
            try
            {
                // Check whether to register the file handle under HKCU or HKCR
                if (registerInHKCU)
                    classesRoot = Registry.CurrentUser.OpenSubKey(@"Software\Classes");
                else
                    classesRoot = Registry.ClassesRoot;

                // First of all, unregister the file handle
                Array.ForEach(associationsToRegister,
                    assoc => UnregisterFileAssociation(progId, assoc));
                UnregisterProgId(progId);

                // Register the application ID and the file handle for each file extension
                if (!unregister)
                {
                    RegisterProgId(progId, appId, openWith);
                    Array.ForEach(associationsToRegister, 
                        assoc => RegisterFileAssociation(progId, assoc));
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }
        }

        // Register the application ID
        private static void RegisterProgId(string progId, string appId, string openWith)
        {
            RegistryKey progIdKey = classesRoot.CreateSubKey(progId);
            progIdKey.SetValue("FriendlyTypeName", "@shell32.dll,-8975");
            progIdKey.SetValue("DefaultIcon", "@shell32.dll,-47");
            progIdKey.SetValue("CurVer", progId);
            progIdKey.SetValue("AppUserModelID", appId);
            RegistryKey shell = progIdKey.CreateSubKey("shell");
            shell.SetValue(String.Empty, "Open");
            shell = shell.CreateSubKey("Open");
            shell = shell.CreateSubKey("Command");
            shell.SetValue(String.Empty, openWith);
            shell.Close();
            progIdKey.Close();
        }

        // Unregister the application ID
        private static void UnregisterProgId(string progId)
        {
            classesRoot.DeleteSubKeyTree(progId);
        }

        // Register the file handle
        private static void RegisterFileAssociation(string progId, string extension)
        {
            RegistryKey openWithKey = classesRoot.CreateSubKey(Path.Combine(extension, "OpenWithProgIds"));
            openWithKey.SetValue(progId, String.Empty);
            openWithKey.Close();
        }

        // Unregister the file handle
        private static void UnregisterFileAssociation(string progId,
            string extension)
        {
            RegistryKey openWithKey = classesRoot.CreateSubKey(Path.Combine(extension, "OpenWithProgIds"));
            openWithKey.DeleteValue(progId);
            openWithKey.Close();
        }

        // A helper method to check whether a file name is valid
        public static bool CheckFileName(string fileName)
        {
            if (fileName.IndexOfAny(Path.GetInvalidFileNameChars()) != -1)
            {
                MessageBox.Show("Please use only characters that are allowed in file names.");
                return false;
            }
            return true;
        }

        // A helper method to create a file in the temp folder
        public static string GetTempFileName(string fileName)
        {
            string path = Path.Combine(System.IO.Path.GetTempPath(), fileName + ".txt");
            File.Create(path).Close();  // Ensure the file exists
            return path;
        }
    }
}
