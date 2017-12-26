//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.ServiceModel;
using System.ServiceModel.Activities;
using System.ServiceModel.Activities.Description;
using System.Workflow.Activities;

namespace Microsoft.Samples.InteropDemo
{

    class Program
    {
        [ServiceContract]
        public interface IWorkflow
        {
            [OperationContract(IsOneWay = true)]
            void Start();
        }
        
        static Activity CreateWorkflow()
        {
            return new Sequence()
            {
                Activities = 
                { 
                    new Sequence()
                    {
                        Activities =
                        {
                            new Receive()
                            {
                                CanCreateInstance = true,
                                OperationName = "Start",
                                ServiceContractName = "IWorkflow"
                            },
                            new WriteLine() { Text = "BEFORE" },
                            new Interop() { ActivityType = typeof(TaskWorkflow) },
                            new WriteLine() { Text = "AFTER\n" },
                            new WriteLine() { Text = "Workflow Complete!" }
                        }
                    }
                }
            };
        }

        static string baseAddress = "http://localhost:8080/Test/";

        static void Main()
        {
            Activity workflow = CreateWorkflow();

            using (System.ServiceModel.Activities.WorkflowServiceHost host = new System.ServiceModel.Activities.WorkflowServiceHost(workflow, new Uri(Program.baseAddress)))
            {
                ExternalDataExchangeService dataExchangeService = new ExternalDataExchangeService();
                TaskService taskService = new TaskService();
                dataExchangeService.AddService(taskService);

                WorkflowRuntimeEndpoint workflowRuntimeEndpoint = new WorkflowRuntimeEndpoint();
                workflowRuntimeEndpoint.AddService(dataExchangeService);
                host.AddServiceEndpoint(workflowRuntimeEndpoint);

                host.AddDefaultEndpoints();
                host.Open();

                IWorkflow proxy = ChannelFactory<IWorkflow>.CreateChannel(new BasicHttpBinding(), new EndpointAddress(Program.baseAddress));
                proxy.Start();

                Console.WriteLine("Workflow starting, press enter when workflow completes.\n");
                Console.ReadLine();
            }
        }
    }
}
