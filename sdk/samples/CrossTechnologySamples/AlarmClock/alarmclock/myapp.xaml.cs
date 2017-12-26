using System;
using System.Windows;
using System.Data;
using System.Xml;
using System.Configuration;
using System.Windows.Media;

namespace Microsoft.Samples.WinFX.AlarmClock
{
    /// <summary>
    /// Interaction logic for MyApp.xaml
    /// </summary>

    public partial class MyApp : Application
    {
        void AppStartup(object sender, StartupEventArgs e)
        {
            TraditionalClock tradClock = new TraditionalClock();
            tradClock.Opacity = .6f;
            tradClock.Show();
        }

        void clockWindow_Closed(object sender, EventArgs e)
        {
            Application.Current.Shutdown(0);
        }
    }
}