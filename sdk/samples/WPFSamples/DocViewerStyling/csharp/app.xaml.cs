// DocViewerStyling SDK Sample - app.xaml.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Windows;
using System.Data;
using System.Xml;
using System.Configuration;
using System.Windows.Input;
using System.Windows.Media;

namespace SdkSample
{
    /// <summary>
    /// Interaction logic for app.xaml
    /// </summary>

    public partial class app : Application
    {
        void AppStartingUp(object sender, StartupEventArgs e)
        {
            Window1 mainWindow = new Window1();
            mainWindow.Show();
        }

    }// end:partial class app

}// end:namespace SdkSample