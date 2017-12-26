//<snippet10>
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;

namespace HostingAxInWpfWithXaml
{
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {   
            // Get the AxHost wrapper from the WindowsFormsHost control.
            AxWMPLib.AxWindowsMediaPlayer axWmp =
                wfh.Child as AxWMPLib.AxWindowsMediaPlayer;

            // Play a .wav file with the ActiveX control.
            axWmp.URL = @"C:\WINDOWS\Media\Windows XP Startup.wav";
        }
    }
}
//</snippet10>