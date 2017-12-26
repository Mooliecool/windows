//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.Threading;

namespace Microsoft.Samples.GuessingGame
{

    class Program
    {
        static void Main(string[] args)
        {
            bool running = true;

            // create the workflow application and start its execution
            AutoResetEvent syncEvent = new AutoResetEvent(false);
            WorkflowApplication application = new WorkflowApplication(new GuessingGameWF());
            application.Completed = delegate(WorkflowApplicationCompletedEventArgs e) { running = false; syncEvent.Set(); };
            application.Idle = delegate(WorkflowApplicationIdleEventArgs e)
            {
                syncEvent.Set();                
            };
            application.Run();

            // main loop (manages bookmarks)            
            while (running)
            {
                if (!syncEvent.WaitOne(10, false))
                {
                    if (running)
                    {
                        // if there are pending bookmarks...
                        if (HasPendingBookmarks(application))
                        {
                            // get the name of the bookmark
                            string bookmarkName = application.GetBookmarks()[0].BookmarkName;

                            // resume the bookmark (passing the data read from the console)
                            application.ResumeBookmark(bookmarkName, Console.ReadLine());

                            syncEvent.WaitOne();
                        }
                    }
                }
            }

            // wait for user input
			Console.WriteLine("Press enter to exit.");
            Console.ReadLine();
        }

        // Returns true if the WorkflowApplication has any pending bookmark
        static bool HasPendingBookmarks(WorkflowApplication application)
        {
            try
            {
                return application.GetBookmarks().Count > 0;
            }
            catch (WorkflowApplicationCompletedException)
            {
                return false;
            }
        }
    }
}
