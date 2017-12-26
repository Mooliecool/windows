using System;
using System.Data;
using System.Windows;
using System.Windows.Data;
using System.Configuration;

namespace Ribbon
{

    public partial class app : Application
    {

        private void ApplicationStartingUp(object sender, StartupEventArgs e)
        {
            Window1 mainWindow = new Window1();
            mainWindow.Show();
        }

    }
}