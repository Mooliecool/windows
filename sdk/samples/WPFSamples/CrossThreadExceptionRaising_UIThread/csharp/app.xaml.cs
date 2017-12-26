using System; // Exception
using System.Text; // StringBuilder
using System.Windows; // Application, MessageBox
using System.Windows.Threading; // Dispatcher, DispatcherUnhandledExceptionEventArgs

namespace SDKSample
{
    public partial class App : Application
    {
        void App_DispatcherUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
        {
            // Display exception message
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("{0}\n", e.Exception.InnerException.Message);
            sb.AppendFormat("{0}\n", e.Exception.Message);
            sb.AppendFormat("Exception handled on main UI thread {0}.", e.Dispatcher.Thread.ManagedThreadId);
            MessageBox.Show(sb.ToString());

            // Keep application running in the face of this exception
            e.Handled = true;
        }
    }
}