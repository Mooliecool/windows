using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace ApplicationShutdownSample
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>

    public partial class MainWindow : Window
    {

        public MainWindow()
        {
            InitializeComponent();
        }

        void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            this.shutdownModeListBox.Items.Add("OnLastWindowClose");
            this.shutdownModeListBox.Items.Add("OnExplicitShutdown");
            this.shutdownModeListBox.Items.Add("OnMainWindowClose");
            this.shutdownModeListBox.SelectedValue = "OnLastWindowClose";
            this.shutdownModeListBox.SelectionChanged += new SelectionChangedEventHandler(shutdownModeListBox_SelectionChanged);
            Application.Current.ShutdownMode = ShutdownMode.OnLastWindowClose;
        }

        void shutdownModeListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Application.Current.ShutdownMode = (ShutdownMode)Enum.Parse(typeof(ShutdownMode), this.shutdownModeListBox.SelectedValue.ToString());
        }

        void newWindowButton_Click(object sender, RoutedEventArgs e)
        {
            (new ChildWindow()).Show();
        }

        void explicitShutdownButton_Click(object sender, RoutedEventArgs e)
        {
            int exitCode = 0;
            int.TryParse(this.appExitCodeTextBox.Text, out exitCode);
            Application.Current.Shutdown(exitCode);
        }
    }
}