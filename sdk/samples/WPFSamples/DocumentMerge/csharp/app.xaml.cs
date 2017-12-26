// DocumentMerge SDK Sample - App.xaml.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Windows;
using System.Data;
using System.Xml;
using System.Configuration;

namespace SdkSample
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>

    public partial class App : Application
    {
        void AppStartup(object sender, StartupEventArgs args)
        {
            Window1 page = new Window1();
            page.Show();
        }

    }
}