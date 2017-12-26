//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.Threading;

namespace Microsoft.Samples.PowerShell.DesignerClient
{

    class Program
    {
        static void Main(string[] args)
        {
            AutoResetEvent syncEvent = new AutoResetEvent(false);

            WorkflowApplication myApplication = new WorkflowApplication(new Sequence1());

            myApplication.Completed = delegate(WorkflowApplicationCompletedEventArgs e)
            {
                syncEvent.Set();
            };

            myApplication.OnUnhandledException = delegate(WorkflowApplicationUnhandledExceptionEventArgs e)
            {
                Console.WriteLine(e.UnhandledException.ToString());
                syncEvent.Set();
                return UnhandledExceptionAction.Terminate;
            };

            myApplication.Aborted = delegate(WorkflowApplicationAbortedEventArgs e)
            {
                Console.WriteLine(e.Reason);
                syncEvent.Set();
            };

            myApplication.Run();

            while (true)
            {
                if (myApplication.GetBookmarks().Count > 0)
                {
                    string bookmarkName = myApplication.GetBookmarks()[0].BookmarkName;
                    string input = Console.ReadLine().Trim();
                    myApplication.ResumeBookmark(bookmarkName, input);

                    // Exit out of this loop only when the user has entered a non-empty string.
                    if (!String.IsNullOrEmpty(input))
                    {
                        break;
                    }
                }
            }

            syncEvent.WaitOne();

            System.Console.WriteLine("The program has ended. Please press [ENTER] to exit...");
            System.Console.ReadLine();
        }
    }
}
