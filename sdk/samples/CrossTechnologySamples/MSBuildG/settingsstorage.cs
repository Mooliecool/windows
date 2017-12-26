// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Configuration;
using System.Windows.Forms;
using Microsoft.Win32;

namespace Microsoft.Samples.MSBuildG
{
    // Summary:
    // Enumeration for the window which will be shown at startup.
    
    public enum StartupWindow { Picker, Wizard, Designer };

    // Summary:
    // Custom "Singleton" class for storing user settings and other system-wide information
    
    public sealed class SettingsSystem
    {
        private static SettingsStorage defaultInstance = new SettingsStorage();

        private SettingsSystem()
        {
        }

        public static SettingsStorage Storage
        {
            get
            {
                return defaultInstance;
            }
        }


        // Summary:
        // Determines the path of the current .NET Framework installed on the system
        
        public static string FindFrameworkPath()
        {
            string frameworkVersion = "v" + System.Environment.Version.Major.ToString() + "." + System.Environment.Version.Minor.ToString() + "." + System.Environment.Version.Build.ToString();

            string installPath = "";

            using (RegistryKey installKey = Microsoft.Win32.Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Microsoft\.NETFramework"))
            {
                installPath = installKey.GetValue("InstallRoot").ToString();
            }

            return (installPath + frameworkVersion);
        }

        // Summary:
        // Gets the path for an icon.
        
        public static Uri GetIcon(string iconName)
        {
            if (iconName == null)
            {
                throw new ArgumentNullException("iconName");
            }

            return (new System.Uri(@"pack://application:,,,/Icons/" + iconName, UriKind.RelativeOrAbsolute));
        }
    }

    public class SettingsStorage : ApplicationSettingsBase
    {

        [UserScopedSetting]
        [DefaultSettingValue("")]
        public string BinPath
        {
            get { return (string)this["BinPath"]; }
            set { this["BinPath"] = value; }
        }

        [UserScopedSetting]
        [DefaultSettingValue("Vista")]
        public string Theme
        {
            get { return (string)this["Theme"]; }
            set { this["Theme"] = value; }
        }
    }
}
