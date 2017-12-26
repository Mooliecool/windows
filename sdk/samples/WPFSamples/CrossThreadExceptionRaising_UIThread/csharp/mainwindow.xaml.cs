using System; // Exception
using System.Threading; // Thread, ApartmentState
using System.Windows; // Window, RoutedEventArgs
using System.Windows.Threading; // Dispatcher, DispatcherPriority, DispatcherOperationCallback

namespace SDKSample
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            this.Title = string.Format("Running on Main UI Thread {0}", Thread.CurrentThread.ManagedThreadId);
        }

        // THIS EVENT HANDLER RUNS ON THE MAIN UI THREAD
        void startSecondaryUIThreadButton_Click(object sender, RoutedEventArgs e)
        {
            // Creates and starts a secondary thread in a single threaded apartment (STA)
            Thread thread = new Thread(MethodRunningOnSecondaryUIThread);
            thread.SetApartmentState(ApartmentState.STA);
            thread.IsBackground = true;
            thread.Start();
        }

        // THIS METHOD RUNS ON A SECONDARY UI THREAD (THREAD WITH A DISPATCHER)
        void MethodRunningOnSecondaryUIThread()
        {
            try
            {
                // On secondary thread, show a new Window before starting a new Dispatcher
                // ie turn secondary thread into a UI thread
                SecondaryUiThreadWindow window = new SecondaryUiThreadWindow();
                window.Show();
                Dispatcher.Run();
            }
            catch (Exception ex)
            {
                // Dispatch the exception back to the main ui thread and reraise it
                int secondaryUIThreadId = Thread.CurrentThread.ManagedThreadId;
                Application.Current.Dispatcher.Invoke(
                    DispatcherPriority.Send,
                    (DispatcherOperationCallback)delegate(object arg)
                    {
                        // THIS CODE RUNS BACK ON THE MAIN UI THREAD
                        string msg = string.Format("Exception forwarded from secondary UI thread {0}.", secondaryUIThreadId);
                        throw new Exception(msg, ex);
                    }
                    , null);

                // NOTE - Application execution will only continue from this point
                //        onwards if the exception was handled on the main UI thread
                //        by Application.DispatcherUnhandledException

            }
        }
    }
}