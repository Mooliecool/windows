using System; // Exception, EventArgs
using System.Threading; // Thread
using System.Windows; // Window, RoutedEventArgs
using System.Windows.Threading; // Dispatcher

namespace SDKSample
{
    // THIS WINDOW RUNS ON A SECONDARY UI THREAD
    public partial class SecondaryUiThreadWindow : Window
    {
        public SecondaryUiThreadWindow()
        {
            InitializeComponent();

            this.Title = string.Format("Running on Secondary UI Thread {0}", Thread.CurrentThread.ManagedThreadId);
        }

        void raiseExceptionOnSecondaryUIThreadButton_Click(object sender, RoutedEventArgs e)
        {
            // Raise an exception on the secondary UI thread
            string msg = string.Format("Exception raised on secondary UI thread {0}.", Dispatcher.CurrentDispatcher.Thread.ManagedThreadId);
            throw new Exception(msg);
        }

        void SecondaryUiThreadWindow_Closed(object sender, EventArgs e)
        {
            // End this thread of execution
            Dispatcher.CurrentDispatcher.InvokeShutdown();
        }
    }
}