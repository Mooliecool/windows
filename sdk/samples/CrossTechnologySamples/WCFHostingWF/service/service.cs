using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.ComponentModel;
using System.Workflow.Runtime.Hosting;
using System.Threading;
using Microsoft.ServiceModel.Samples;
using System.ServiceModel.Channels;

namespace Microsoft.ServiceModel.Samples
{
    [ServiceContract (CallbackContract=typeof(IEchoableCallback), SessionMode=SessionMode.Required)]
    public interface IEchoable
    {
        [OperationContract(IsOneWay=true)]
        void Echo(string theString);
    }

    [ServiceContract]
    public interface IEchoableCallback
    {
        [OperationContract(IsOneWay=true)]
        void EchoResponse(string response);

        [OperationContract(IsOneWay=true, Action="http://Microsoft.ServiceModel.Samples/EchoFault")]
        void EchoFault(Message fault); 
    }

    [ServiceBehavior(InstanceContextMode = InstanceContextMode.PerSession)]
    class EchoableService : IEchoable
    {
        IEchoableCallback callback;
        MessageVersion version;

        public void Echo(string theString)
        {
            //Aquire the callback from the current OperationContext
            callback = OperationContext.Current.GetCallbackChannel<IEchoableCallback>();
            //grab the version of the incoming message in case we need to create a fault later
            version = OperationContext.Current.IncomingMessageVersion;
            
            Console.WriteLine("WCF: Got {0}", theString);
            
            Dictionary<string, object> parameters = new Dictionary<string, object>();
            parameters.Add("ReceivedData", theString);

            WorkflowRuntime workflowRuntime = new WorkflowRuntime();
            ManualWorkflowSchedulerService scheduler = new ManualWorkflowSchedulerService();
            workflowRuntime.AddService(scheduler);
            workflowRuntime.StartRuntime();

            workflowRuntime.WorkflowCompleted += this.OnWorkflowCompleted;
            workflowRuntime.WorkflowTerminated += this.OnWorkflowTerminated;

            WorkflowInstance instance = workflowRuntime.CreateWorkflow(typeof(Workflow1), parameters);
            
            instance.Start();

            scheduler.RunWorkflow(instance.InstanceId);
        }

        void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Message message =
                            Message.CreateMessage(version, FaultCode.CreateReceiverFaultCode(new FaultCode("wf")),
                            "The workflow aborted unexpectedly",
                            "http://Microsoft.ServiceModel.Samples/EchoFault");
            callback.EchoFault(message); callback.EchoFault(message);
        }

        void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {            
            callback.EchoResponse(e.OutputParameters["ResultData"].ToString());
        }
        
        static void Main(string[] args)
        {
            using (ServiceHost host = new ServiceHost(typeof(EchoableService)))
            {
                host.Open();

                Console.WriteLine("The service is ready.");
                Console.WriteLine("Press <ENTER> to terminate service.");
                Console.ReadLine();
            }
        }  
    }
}
