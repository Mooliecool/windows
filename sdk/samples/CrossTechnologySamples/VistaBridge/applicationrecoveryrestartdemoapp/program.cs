using System;
using System.Timers;
using System.Diagnostics;
using Microsoft.SDK.Samples.VistaBridge.Library.AppRecoveryRestart;

namespace Server2008.DeveloperStory.ARRExamples
{
    class ARRDemoApp
    {
        static void Main(string[] args)
        {
            RegisterForRecovery();
            RegisterForRestart();

            // SetupTimerNotifyForRestart sets a timer to 
            // beep when 60 seconds have elapsed, indicating that
            // WER will restart the program after a crash.
            // WER will not restart applications that crash
            // within 60 seconds of startup.
            SetupTimerNotifyForRestart();

            // If we started with /restart command line argument 
            // then we were automatically restarted and should
            // try to resume the previous session.
            if (args.Length > 0 && args[0] == "/restart")
            {
                RecoverLastSession(args[0]);
            }
            // Main loop of activities that the user can select.
            bool done = false;
            do
            {
                string s = GetUserChoiceFromMenu();
                switch (s)
                {
                    case "1": DisplayRecoverySettings();
                        break;
                    case "2": DisplayRestartSettings();
                        break;
                    case "3": Crash();
                        break;
                    case "q": done = true;
                        break;
                }
            }
            while (done == false);
        }

        private static void SetupTimerNotifyForRestart()
        {
            // Beep when 60 seconds has elapsed.
            Timer notify = new Timer(60000);
            notify.Elapsed += new ElapsedEventHandler(NotifyUser);
            notify.AutoReset = false; // Only beep once.
            notify.Enabled = true;
        }

        private static void NotifyUser(object source, ElapsedEventArgs e)
        {
            Console.Beep();
        }

        private static string GetUserChoiceFromMenu()
        {
            string banner = "-----------------------------------";

            // Display the main menu and get the user's input.
            Console.WriteLine();
            Console.WriteLine(banner);
            Console.WriteLine("ARR Demo Main Menu");
            Console.WriteLine(banner);
            Console.WriteLine("1 - Show Recovery settings");
            Console.WriteLine("2 - Show Restart settings");
            Console.WriteLine("3 - Crash this application");
            Console.WriteLine("q - Exit this program");
            Console.WriteLine();
            Console.Write("Enter your option: ");

            string s = Console.ReadLine().ToLower();
            if (s != "1" && s != "2" && s != "3" && s != "q")
            {
                Console.WriteLine("Invalid Option: {0}", s);
                return GetUserChoiceFromMenu();
            }
            return s;
        }

        private static void Crash()
        {
            Environment.FailFast("ARR Demo intentional crash.");
        }
        private static void RegisterForRestart()
        {
            // Register for automatic restart if the 
            // application was terminated for any reason
            // other than a system reboot or a system update.
            ArrManager.RegisterForApplicationRestart(
                new RestartSettings(
               "/restart",
               RestartRestrictions.NotOnReboot | RestartRestrictions.NotOnPatch));
        }

        private static void RegisterForRecovery()
        {
            RecoverySettings settings = new RecoverySettings(
                new RecoveryCallback(RecoveryProcedure),
                new RecoveryData(Environment.UserName),
                4000);

            ArrManager.RegisterForApplicationRecovery(
                settings);
        }

        // This method is invoked by WER. 
        private static int RecoveryProcedure(RecoveryData parameter)
        {
            Console.WriteLine("Recovery in progress for {0}",
                parameter.CurrentUser);

            // Do recovery work here.
            for (int i = 0; i < 4; i++)
            {
                // Signal to WER that the recovery
                // is still in progress.
                PingSystem();
                // Simulate long running recovery.
                System.Threading.Thread.Sleep(3000);
            }
            // Indicate that recovery work is done.
            Console.WriteLine("Application shutting down...");
            ArrManager.ApplicationRecoveryFinished(true);
            return 0;
        }

        // This method is called periodically to ensure
        // that WER knows that recovery is still in progress.
        private static void PingSystem()
        {
            // Find out if the user canceled recovery.
            bool isCanceled =
                ArrManager.ApplicationRecoveryInProgress();

            if (isCanceled)
            {
                Console.WriteLine("Recovery has been canceled by user.");
                Environment.Exit(2);
            }
            Console.WriteLine(" / ");
        }

        // This method gets called by main when the 
        // commandline arguments indicate that this
        // application was automatically restarted 
        // by WER.
        private static void RecoverLastSession(string command)
        {
            Console.WriteLine("Recovery in progress {0}", command);
            // Perform application state restoration 
            // actions here.
        }

        private static void DisplayRecoverySettings()
        {
            RecoverySettings settings =
                ArrManager.ApplicationRecoverySettings(
                    Process.GetCurrentProcess().Handle);

            Console.WriteLine(settings.ToString());
            Console.WriteLine();
        }

        private static void DisplayRestartSettings()
        {
            RestartSettings settings =
                ArrManager.ApplicationRestartSettings(
                    Process.GetCurrentProcess().Handle);

            Console.WriteLine(settings.ToString());
            Console.WriteLine();
        }
    }
}