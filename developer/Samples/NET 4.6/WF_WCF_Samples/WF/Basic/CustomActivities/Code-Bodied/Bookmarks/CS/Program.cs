//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Hosting;
using System.Activities.Statements;
using System.Collections.Generic;
using System.Threading;

namespace Microsoft.Samples.Bookmarks
{

    class Program
    {
        static Activity CreateWorkflow()
        {
            Variable<string> x = new Variable<string>() { Name = "x" };
            Variable<string> y = new Variable<string>() { Name = "y" };
            Variable<string> z = new Variable<string>() { Name = "z" };

            // Create a workflow with three bookmarks: x, y, z. After all three bookmarks
            // are resumed (in any order), the concatenation of the values provided
            // when the bookmarks were resumed is written to output.
            return new Sequence
            {
                Variables = { x, y, z },
                Activities = 
                {
                    new System.Activities.Statements.Parallel
                    {
                        Branches =
                        {
                            new Read<string>() { BookmarkName = "x", Result = x },
                            new Read<string>() { BookmarkName = "y", Result = y },
                            new Read<string>() { BookmarkName = "z", Result = z }                           
                        }
                    },
                    new WriteLine
                    { 
                        Text = new InArgument<string>((ctx) => "x+y+z=" + x.Get(ctx) + y.Get(ctx) + z.Get(ctx)) 
                    }
                }
            };
        }

        static void Main()
        {
            bool completed = false;
            AutoResetEvent idleOrCompletedEvent = new AutoResetEvent(false);

            Activity workflow = CreateWorkflow();
            WorkflowApplication application = new WorkflowApplication(workflow);
            application.Idle += delegate(WorkflowApplicationIdleEventArgs e)
            {
                idleOrCompletedEvent.Set();
            };
            application.Completed += delegate(WorkflowApplicationCompletedEventArgs e)
            {
                completed = true;
                idleOrCompletedEvent.Set();
            };

            application.Run();

            while (true)
            {
                idleOrCompletedEvent.WaitOne();
                if (!completed)
                {
                    Interact(application);
                }
                else
                {
                    break;
                }
            }            
        }

        static void Interact(WorkflowApplication application)
        {
            IList<BookmarkInfo> bookmarks = application.GetBookmarks();

            while (true)
            {
                Console.Write("Bookmarks:");
                foreach (BookmarkInfo info in bookmarks)
                {
                    Console.Write(" '" + info.BookmarkName + "'");
                }
                Console.WriteLine();

                Console.WriteLine("Enter the name of the bookmark to resume");
                string bookmarkName = Console.ReadLine();

                if (bookmarkName != null && !bookmarkName.Equals(string.Empty))
                {
                    Console.WriteLine("Enter the payload for the bookmark '{0}'", bookmarkName);
                    string bookmarkPayload = Console.ReadLine();

                    BookmarkResumptionResult result = application.ResumeBookmark(bookmarkName, bookmarkPayload);
                    if (result == BookmarkResumptionResult.Success)
                    {
                        return;
                    }
                    else
                    {
                        Console.WriteLine("BookmarkResumptionResult: " + result);
                    }
                }
            }
        }
    }
}
