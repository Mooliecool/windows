using System;
using System.Windows;
using System.Data;
using System.Xml;
using System.Configuration;

namespace ApplicationShutdownSample
{
    public partial class App : Application
    {
        public App()
            : base()
        {
            this.ShutdownMode = ShutdownMode.OnExplicitShutdown;
        }

        void App_Exit(object sender, ExitEventArgs e)
        {
            Console.WriteLine("Exit");
        }
    }
}