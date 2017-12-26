// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Windows;
using System.Data;
using System.Xml;
using System.Configuration;
using System.Collections.Generic;

namespace Microsoft.Samples.MSBuildG
{

    public partial class MyApp : Application
    {       
        void AppStartingUp(object sender, StartupEventArgs e)
        {
            //Retrieve the theme we want to use for this application from settings
            Dictionary<string,ResourceDictionary> themes=new Dictionary<string,ResourceDictionary>();
            themes.Add("Vista",new Resources_Vista());

            string currentTheme = SettingsSystem.Storage.Theme;

            if (!themes.ContainsKey(currentTheme))
            {
                currentTheme = "Vista";
            }

            Application.Current.Resources = themes[currentTheme];
            Window startWindow = new PickerWindow();

            //Set the default BinPath for MS Build to the Framework path
            if (SettingsSystem.Storage.BinPath.Length == 0)
            {
                SettingsSystem.Storage.BinPath = SettingsSystem.FindFrameworkPath();
            }

            startWindow.Show();
        }

    }
}