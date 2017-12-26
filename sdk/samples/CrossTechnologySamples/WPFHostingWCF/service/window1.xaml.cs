using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;


namespace Microsoft.ServiceModel.Samples
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : Window, IDisplay
    {

        public Window1()
        {
            InitializeComponent();
        }

        public void StartServiceClick(object sender, EventArgs e)
        {
            Write(string.Format("Starting Calculator Service\n"));

            //initialize the service with a reference to this window
            CalculatorService calculator = new CalculatorService(this);
            //start the service
            MyServiceHost.StartService(calculator);
        }

        public void StopServiceClick(object sender, EventArgs e)
        {
            Write(string.Format("Stopping Calculator Service\n"));
            MyServiceHost.StopService();
        }

        public void Write(string value)
        {
            this.textBox1.Text += value;
        }

    }
}