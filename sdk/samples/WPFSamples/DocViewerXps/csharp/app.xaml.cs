// DocViewerXps SDK Sample - App.xaml.cs
// Copyright (c) Microsoft Corporation. All rights reserved. 

using System;
using System.Windows;

namespace SDKSample
{
    /// <summary>
    ///   Interaction logic for app.xaml</summary>
    public partial class app : Application
    {
        void AppStartingUp(object sender, StartupEventArgs e)
        {
            Window1 mainWindow = new Window1();
            mainWindow.Show();
        }

    }
}