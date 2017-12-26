
using System;
using System.Windows;
using System.Data;
using System.Xml;
using System.Configuration;
using System.Windows.Media.Imaging;
using System.Windows.Media;

namespace Microsoft.Samples.Graphics.UsingVisualBrush
{
    /// <summary>
    /// Interaction logic for Application.xaml
    /// </summary>

    public partial class app : Application
    {
    
        public app()
        {

        }

        void AppStartingUp(object sender, StartupEventArgs e)
        {

            Window mainWindow = new SampleViewer();
            mainWindow.Show();
               
        }

    }
}