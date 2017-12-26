/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:		CSWFLocalService
* Copyright (c) Microsoft Corporation.
* 
* Game Workflow Host
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 11/2/2009 8:50 PM Andrew Zhu Created
\***************************************************************************/

using System;
using System.Threading;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;
using System.Workflow.Activities;


namespace CSWFLocalService
{
    class Program
    {
        static void Main(string[] args)
        {
            Program game = new Program();
            game.RunGameWF();
        }

        //Shared with RunGameWF() and  
        //HandleMessageFromServiceToHostEvent(object o, MessageEventArgs e) 
        public GuessNumberGameService gngs;
        public Guid instanceId;


        /////////////////////////////////////////////////////////////////////
        // Create a workflow runtime a run game workflow in it
        //

        public void RunGameWF()
        {
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                AutoResetEvent waitHandle = new AutoResetEvent(false);
                workflowRuntime.WorkflowCompleted += delegate(object sender, 
                    WorkflowCompletedEventArgs e) { waitHandle.Set(); };
                workflowRuntime.WorkflowTerminated += delegate(object sender, 
                                               WorkflowTerminatedEventArgs e)
                {
                    Console.WriteLine(e.Exception.Message);
                    Console.WriteLine(e.Exception.StackTrace);
                    waitHandle.Set();
                };

                //Add GuessNumberGameService to runtime.
                ExternalDataExchangeService externalService = 
                    new ExternalDataExchangeService();
                workflowRuntime.AddService(externalService);
                gngs = new GuessNumberGameService();
                externalService.AddService(gngs);
                
                //subscribe MessageFromServiceToHostEvent to 
                //HandleMessageFromServiceToHostEvent
                //Message will be sent to host from local service when the 
                //event is raised.
                gngs.MessageFromServiceToHostEvent += 
                    HandleMessageFromServiceToHostEvent;
                
                WorkflowInstance instance = 
                    workflowRuntime.CreateWorkflow(typeof(GuessNumberGameWF));
                instanceId = instance.InstanceId;
                instance.Start();

                waitHandle.WaitOne();
            }
        }

        public void HandleMessageFromServiceToHostEvent(object o, MessageEventArgs e)     
        {
            Console.WriteLine(e.Message);
            if (e.Message.Equals("right"))
            {
                return;
            }
            string InMessage = Console.ReadLine();
            gngs.OnMessageFromHostToWFEvent(instanceId, InMessage);
        }
    }
}
