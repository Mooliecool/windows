using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Forms.Integration;
using System.Windows.Interop;
using System.Windows.Controls.Primitives;
using System.Windows.Navigation;
using System.Collections.Generic;
using Microsoft.SDK.Samples.VistaBridge.Controls;


namespace Wizard
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : AeroWizard
    {
        public Window1()
        {      
            InitializeComponent();
        }

        void OnClick(object sender, RoutedEventArgs e)
        {          
            NextPage();
        }       
    }
}