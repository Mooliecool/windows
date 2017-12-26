//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.DurableInstancing;
using System.Activities.Statements;
using System.Runtime.DurableInstancing;
using System.Threading;

namespace Microsoft.Samples.Activities
{

    class Program
    {
        static InstanceStore instanceStore;
        static AutoResetEvent instanceUnloaded = new AutoResetEvent(false);
        static Activity activity = CreateWorkflow();
        static Guid id;

        const string readLineBookmark = "ReadLine1";

        static void Main()
        {
            SetupInstanceStore();

            StartAndUnloadInstance();
            LoadAndCompleteInstance();

            Console.WriteLine("Press [Enter] to exit.");
            Console.ReadLine();
        }

        static void StartAndUnloadInstance()
        {
            WorkflowApplication application = new WorkflowApplication(activity);

            application.InstanceStore = instanceStore;

            //returning IdleAction.Unload instructs the WorkflowApplication to persists application state and remove it from memory  
            application.PersistableIdle = (e) =>
                {
                    return PersistableIdleAction.Unload;
                };

            application.Unloaded = (e) =>
                {
                    instanceUnloaded.Set();
                };


            //This call is not required 
            //Calling persist here captures the application durably before it has been started
            application.Persist();
            id = application.Id;
            application.Run();

            instanceUnloaded.WaitOne();
        }

        static void LoadAndCompleteInstance()
        {
            string input = Console.ReadLine();

            WorkflowApplication application = new WorkflowApplication(activity);
            application.InstanceStore = instanceStore;

            application.Completed = (workflowApplicationCompletedEventArgs) =>
            {                
                Console.WriteLine("\nWorkflowApplication has Completed in the {0} state.", workflowApplicationCompletedEventArgs.CompletionState);
            };

            application.Unloaded = (workflowApplicationEventArgs) =>
            {
                Console.WriteLine("WorkflowApplication has Unloaded\n");
                instanceUnloaded.Set();
            };

            application.Load(id);

            //this resumes the bookmark setup by readline
            application.ResumeBookmark(readLineBookmark, input);

            instanceUnloaded.WaitOne();
        }

        static Sequence CreateWorkflow()
        {
            Variable<string> response = new Variable<string>();

            return new Sequence()
            {
                Variables = { response },
                Activities = { 
                        new WriteLine(){
                            Text = new InArgument<string>("What is your name?")},
                        new ReadLine(){ 
                            BookmarkName = readLineBookmark, 
                            Result = new OutArgument<string>(response)},
                        new WriteLine(){
                            Text = new InArgument<string>((context) => "Hello " + response.Get(context))}}
            };
        }

        private static void SetupInstanceStore()
        {
            instanceStore = 
                new SqlWorkflowInstanceStore(@"Data Source=.\SQLEXPRESS;Initial Catalog=SampleInstanceStore;Integrated Security=True;Asynchronous Processing=True");
            
            InstanceHandle handle = instanceStore.CreateInstanceHandle();            
            InstanceView view = instanceStore.Execute(handle, new CreateWorkflowOwnerCommand(), TimeSpan.FromSeconds(30));            
            handle.Free();

            instanceStore.DefaultInstanceOwner = view.InstanceOwner;
        }
    }
}
