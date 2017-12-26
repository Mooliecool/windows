using System.Windows; // Window, RoutedEventArgs

namespace WindowShowActivatedSample
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void showWindowActivateButton_Click(object sender, RoutedEventArgs e)
        {
            ChildWindow tw = new ChildWindow();
            // tw.ShowActivated = true; // true is the default value
            tw.Show();
        }

        private void showWindowNoActivateButton_Click(object sender, RoutedEventArgs e)
        {
            ChildWindow tw = new ChildWindow();
            tw.ShowActivated = false;
            tw.Show();
        }

        // This option doesn't make sense
        private void showDialogBoxActivateButton_Click(object sender, RoutedEventArgs e)
        {
            ChildWindow tw = new ChildWindow();
            // tw.ShowActivated = true; // true is the default value
            tw.ShowDialog();
        }

        // This option doesn't make sense
        private void showDialogBoxNoActivateButton_Click(object sender, RoutedEventArgs e)
        {
            ChildWindow tw = new ChildWindow();
            tw.ShowActivated = false;
            tw.ShowDialog();
        }
    }
}
