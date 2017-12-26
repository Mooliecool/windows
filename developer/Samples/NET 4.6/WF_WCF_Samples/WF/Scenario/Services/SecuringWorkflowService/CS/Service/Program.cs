//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.ServiceModel.Activities;

namespace Microsoft.Samples.WF.SecuringWorkFlow
{
   
    class MyWorkflowService
    {
        static WorkflowService GetService()
        {
            Variable<string> incomingMessage = new Variable<string> { Name = "inmessage" };
            Variable<int> outgoingMessage = new Variable<int> { Name = "outmessage" };
            Receive receiveSecureData = new Receive
            {
                OperationName = "AskQuestion",
                ServiceContractName = "ISecuredService",
                CanCreateInstance = true,
                Content = ReceiveContent.Create(new OutArgument<string>(incomingMessage))
            };
            Sequence SecuredWorkFlow = new Sequence()
            {
                Variables = { incomingMessage, outgoingMessage },
                Activities =
                {
                    receiveSecureData,
                    new WriteLine
                    {
                        Text = new InArgument<string>(env =>("Message received: " + incomingMessage.Get(env)))
                    },
                    new SendReply
                    {
                        Request = receiveSecureData,
                        Content = SendContent.Create(new InArgument<int>(4))
                    }
                }
            };

            WorkflowService service = new WorkflowService
            {
                Name = "SecuredService",
                Body = SecuredWorkFlow,
                ConfigurationName = "SecuredService"
            };
            return service;
        }

        static void Main(string[] args)
        {
            Console.Title = "Service";
            Console.WriteLine("Starting up");

            System.ServiceModel.Activities.WorkflowServiceHost host = new WorkflowServiceHost(GetService(), new Uri("http://localhost:8000/"));
            host.Open();

            Console.WriteLine("Press any key to exit");
            Console.ReadLine();

            host.Close();

        }
    }
}
