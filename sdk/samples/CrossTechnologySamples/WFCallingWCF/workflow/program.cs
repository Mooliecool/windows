#region Using directives

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;
using Microsoft.ServiceModel.Samples.Properties;

#endregion

namespace Microsoft.ServiceModel.Samples
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);

        static void Main(string[] args)
        {
            string input = "Hello world";
            Console.WriteLine("Input parameter for this workflow is {0}", input);
            Dictionary<string, object> parameters = new Dictionary<string, object>();
            parameters.Add("Input", input);

            WorkflowRuntime workflowRuntime = new WorkflowRuntime();

            workflowRuntime.WorkflowCompleted += new EventHandler<WorkflowCompletedEventArgs>(workflowRuntime_WorkflowCompleted);
            workflowRuntime.WorkflowTerminated += new EventHandler<WorkflowTerminatedEventArgs>(workflowRuntime_WorkflowTerminated);

            WorkflowInstance instance = workflowRuntime.CreateWorkflow(typeof(Workflow1), parameters);
            Console.WriteLine("Starting workflow");
            instance.Start();

            waitHandle.WaitOne();

            Console.WriteLine("Press <ENTER> to terminate program.");
            Console.ReadLine();
        }

        static void workflowRuntime_WorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();

        }

        static void workflowRuntime_WorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            Console.WriteLine("Workflow completed");
            string returnValue = (string)e.OutputParameters["ReturnValue"];
            Console.WriteLine("The ReturnValue for this workflow was {0}", returnValue);
            waitHandle.Set();
        }


    }
}
