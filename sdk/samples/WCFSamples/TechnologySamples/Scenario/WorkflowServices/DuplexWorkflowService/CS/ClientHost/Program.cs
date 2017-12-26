//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ServiceModel;
using System.ServiceModel.Description;
using System.Workflow.Runtime;

namespace Microsoft.WorkflowServices.Samples
{
    [ServiceBehavior(InstanceContextMode=InstanceContextMode.Single)]
    class ClientHost : IReverseContract
    {
        static void Main(string[] args)
        {
            LocalWorkflowServiceHost localHost = new LocalWorkflowServiceHost(typeof(ClientWorkflow),new ClientHost());
            
            localHost.Description.Behaviors.Find<WorkflowRuntimeBehavior>().WorkflowRuntime.WorkflowTerminated += delegate(object sender, WorkflowTerminatedEventArgs e) { Console.WriteLine("WorkflowTerminated: " + e.Exception.Message); };
            localHost.Description.Behaviors.Find<WorkflowRuntimeBehavior>().WorkflowRuntime.WorkflowCompleted += delegate(object sender, WorkflowCompletedEventArgs e) { Console.WriteLine("WorkflowCompleted: " + e.WorkflowInstance.InstanceId.ToString()); };
            localHost.Open();
            Console.WriteLine("LocalWorkflowServiceHost is ready.");

            IHostForwardContract channel = localHost.CreateLocalChannel<IHostForwardContract>();
            if (!localHost.RecoveredContext)
            {   // new Workflow
                string returnUri = localHost.BaseAddresses[0].ToString();
                channel.BeginWork(returnUri);
                localHost.MaintainContext((IClientChannel)channel);
                Console.WriteLine("Press <enter> to submit first WorkItem.");
            }
            else
            {   // continuation of existing Workflow
                Console.WriteLine("Press <enter> to submit another WorkItem.");
            }
            Console.ReadLine();

            Random generator = new Random();
            while (true)
            {
                int id = generator.Next(100,999);
                channel.SubmitWorkItem("WorkItem" + id.ToString());
                Console.WriteLine("Submitting: WorkItem" + id.ToString());
                Console.WriteLine("Submit another WorkItem? (Y)es or (N)o");
                string input = Console.ReadLine();
                if (input.Length == 0 || input[0] != 'Y')
                    break;
            }

            Console.WriteLine("Completing Work...");

            channel.WorkComplete();
            ((IClientChannel)channel).Close();

            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Press <enter> to exit.");
            Console.ResetColor();
            Console.ReadLine();

            localHost.Close();
        }


#region IReverseContract Members

        public void WorkItemComplete(WorkItem item)
        {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("FirstPart: " + item.FirstPart);
            Console.Write("PartsList: ");
            for (int i = 0; i < item.PartsList.Count; i++)
            {
                if (i > 0)
                    Console.Write(",");
                Console.Write(item.PartsList[i].ToString());
            }
            Console.WriteLine();
            Console.WriteLine("LastPart: " + item.LastPart);
            Console.WriteLine();
            Console.ResetColor();
        }

#endregion

    }
}
