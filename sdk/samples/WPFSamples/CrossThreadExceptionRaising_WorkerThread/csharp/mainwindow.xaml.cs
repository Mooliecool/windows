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

        void startSecondaryWorkerThreadButton_Click(object sender, RoutedEventArgs e)
        {
            // Creates and starts a secondary thread in a single threaded apartment (STA)
            Thread thread = new Thread(methodRunningOnSecondaryWorkerThread);
            thread.SetApartmentState(ApartmentState.STA);
            thread.IsBackground = true;
            thread.Start();
        }

        // THIS METHOD RUNS ON A SECONDARY WORKER THREAD (THREAD WITHOUT A DISPATCHER)
        void methodRunningOnSecondaryWorkerThread()
        {
            try
            {
                WorkerMethod();
            }
            catch (Exception ex)
            {
                // Dispatch the exception back to the main UI thread. Then, reraise
                // the exception on the main UI thread and handle it from the handler 
                // the Application object's DispatcherUnhandledException event.
                int secondaryWorkerThreadId = Thread.CurrentThread.ManagedThreadId;
                Application.Current.Dispatcher.Invoke(
                    DispatcherPriority.Send,
                    (DispatcherOperationCallback)delegate(object arg)
                    {
                        // THIS CODE RUNS BACK ON THE MAIN UI THREAD
                        string msg = string.Format("Exception forwarded from secondary worker thread {0}.", secondaryWorkerThreadId);
                        throw new Exception(msg, ex);
                    }
                    , null);

                // NOTE - Application execution will only continue from this point
                //        onwards if the exception was handled on the main UI thread.
                //        by Application.DispatcherUnhandledException
            }
        }

        void WorkerMethod()
        {
            // This method would do real processing on the secondary worker thread.
            // For the purposes of this sample, it throws an exception
            string msg = string.Format("Exception raised secondary on worker thread {0}.", Dispatcher.CurrentDispatcher.Thread.ManagedThreadId);
            throw new Exception(msg);
        }
    }
}