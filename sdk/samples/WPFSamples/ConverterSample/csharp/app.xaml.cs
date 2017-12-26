using System;
using System.Windows;
using System.Data;
using System.Xml;
using System.Configuration;
using System.IO;

namespace ConverterSample
{
    /// <summary>
    /// Interaction logic for app.xaml
    /// </summary>

    public partial class app : Application
    {
    
    
        void AppStartingUp(object sender, StartupEventArgs e)
        {
            AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(CurrentDomain_UnhandledException);

            Window1 mainWindow = new Window1();
            mainWindow.Show();

            ///Displays the values of the variables that will be used
            mainWindow.ShowVars();

        }
        
        private void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs args)
        {
            
            try {
                StreamWriter wr = new StreamWriter("error.txt");
                wr.Write(args.ExceptionObject.ToString());
                wr.Close();
            
            }catch
            {
            
            }
            
            
            MessageBox.Show("Unhandled exception: " + args.ExceptionObject.ToString());
        }             

    }
}