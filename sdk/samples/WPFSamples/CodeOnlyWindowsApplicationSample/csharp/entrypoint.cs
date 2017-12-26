using System;

namespace CodeOnlyWindowsApplicationSample
{
    public class EntryPoint
    {
        // Entry point function

        // WPF applications must run in a single-threaded apartment
        [STAThread]
        public static void Main()
        {
            // Start the WPF application
            App app = new App();
            app.Run();
        }
    }
}
