using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
namespace Microsoft.SDK.Samples.VistaBridge.Library.Network
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            StringBuilder s = new StringBuilder();
            s.AppendFormat("Is the system connected: {0}\n",
                NetworkListManager.IsConnected);
            s.AppendFormat("Is there an internet connection: {0}\n",
                NetworkListManager.IsConnectedToInternet);
            s.AppendFormat("System connectivity: {0}\n",
    NetworkListManager.Connectivity);
            s.AppendLine();
            NetworkCollection nets = 
                NetworkListManager.GetNetworks(NetworkConnectivityLevels.All);

            foreach (Network n in nets)
            {
                s.AppendFormat ("Name: {0}\nDescription: {1}\nDomain type: {2}\nIs connected: {3}\n",
                    n.Name, n.Description, n.DomainType, n.IsConnected);
                s.AppendFormat("Is connected to internet: {0}\nNetwork ID: {1}\nCategory: {2}\n",
                    n.IsConnectedToInternet,n.NetworkId,n.Category.ToString());
                s.AppendFormat(
                    "Created time: {0} \nConnnected time: {1}\nConnectivity: {2}\n",
                     n.CreatedTime,n.ConnectedTime, n.Connectivity.ToString());
                NetworkConnectionCollection connects = n.Connections;
                foreach (NetworkConnection nc in connects)
                {
                    s.AppendFormat("\n\tConnection ID: {0}\n\tDomain: {1}\n\tIs connected: {2}\n\tIs connected to internet: {3}\n", 
                        nc.ConnectionId, nc.DomainType, nc.IsConnected, nc.IsConnectedToInternet);
                    s.AppendFormat("\tAdapter ID: {0}\n\tConnectivity: {1}\n",
                        nc.AdapterId, nc.Connectivity);
                }
                s.AppendLine();
            }
            
            textBox1.Text = s.ToString();
        }
    }
}
