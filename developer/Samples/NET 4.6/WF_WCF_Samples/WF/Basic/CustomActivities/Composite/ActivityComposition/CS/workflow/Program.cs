//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.XamlIntegration;
using System.IO;
using System.Threading;

namespace Microsoft.Samples.Activities
{

    class Program
    {
        static void Main(string[] args)
        {
            ManualResetEvent workflowCompleted = new ManualResetEvent(false);
            ManualResetEvent workflowIdled = new ManualResetEvent(false);

            Activity activity;

            using (Stream stream = File.OpenRead("Program.xaml"))
            {
                activity = ActivityXamlServices.Load(stream);
                stream.Close();
            }

            //create the WorkflowApplication using the Activity loaded from Program.xaml
            WorkflowApplication application = new WorkflowApplication(activity);

            //set up the Completed and Idle callbacks
            application.Completed = workflowCompletedEventArgs => workflowCompleted.Set();
            application.Idle = (e) =>
            {
                workflowIdled.Set();                
            };

            //run the program
            application.Run();

            while (true)
            {
                string input = Console.ReadLine();

                workflowIdled.Reset();

                //provide the Console input to the Workflow
                application.ResumeBookmark("PromptBookmark", input);

                //wait for either the Idle or the Completion signal
                int signalled = WaitHandle.WaitAny(new WaitHandle[] { workflowCompleted, workflowIdled });

                //if completion was signalled, then we are done.
                if (signalled == 0)
                {                    
                    break;
                }
            }

            Console.WriteLine();
            Console.WriteLine("Press [ENTER] to exit.");

            Console.ReadLine();
        }
    }
}
