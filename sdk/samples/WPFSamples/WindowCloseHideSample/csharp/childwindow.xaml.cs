using System; // DateTime
using System.ComponentModel; // CancelEventArgs
using System.Windows; // Window, DependencyPropertyChangedEventArgs

namespace WindowCloseHideSample
{
    public partial class ChildWindow : Window
    {
        private bool close;

        public ChildWindow()
        {
            InitializeComponent();

            // Get date/time when window is first shown
            this.firstShownTextBlock.Text = DateTime.Now.ToString();
        }

        // Shadow Window.Close to make sure we bypass the Hide call in 
        // the Closing event handler
        public new void Close()
        {
            this.close = true;
            base.Close();
        }

        private void Window_IsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            // Get date/time the window is shown now (ie when it becomes visible)
            if ((bool)e.NewValue == true)
            {
                this.shownThisTimeTextBlock.Text = DateTime.Now.ToString();
            }
        }

        private void Window_Closing(object sender, CancelEventArgs e)
        {
            // If Close() was called, close the window (instead of hiding it)
            if (this.close) return;

            // Hide the window (instead of closing it)
            e.Cancel = true;
            this.Hide();
        }
    }
}
