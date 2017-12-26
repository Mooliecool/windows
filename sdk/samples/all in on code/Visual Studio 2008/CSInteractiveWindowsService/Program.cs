using System;
using System.ServiceProcess;

namespace CSInteractiveWindowsService
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main()
        {
            ServiceBase service = new SampleService();
            ServiceBase.Run(service);
        }
    }
}