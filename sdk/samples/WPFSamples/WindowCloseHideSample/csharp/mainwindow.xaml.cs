using System.ComponentModel; // CancelEventArgs
using System.Windows; // Window, RoutedEventArgs

namespace WindowCloseHideSample
{
    public partial class MainWindow : Window
    {
        // To hide/show a window, rather than reinstantiate a window
        // each time it needs to be opened, we need to retain the instance
        // in a class-scoped variable
        private ChildWindow childWindow;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void showChildWindowButton_Click(object sender, RoutedEventArgs e)
        {
            // Create the window if it's not already created
            if (this.childWindow == null) this.childWindow = new ChildWindow();

            // Show the window
            this.childWindow.Show();
        }

        private void Window_Closing(object sender, CancelEventArgs e)
        {
            // Close the child window only when this window closes
            this.childWindow.Close();
        }
    }
}
