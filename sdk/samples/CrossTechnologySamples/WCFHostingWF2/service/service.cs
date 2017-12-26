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
using System.Runtime.Remoting.Messaging;
using System.Runtime.Serialization;

namespace Microsoft.ServiceModel.Samples
{
    //should probably add some faultcontracts here

    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public interface IEchoable
    {
        [FaultContract(typeof(WorkflowAborted))]
        [OperationContract(AsyncPattern = true)]
        IAsyncResult BeginEcho(string theString, AsyncCallback callback, object state);
        string EndEcho(IAsyncResult result);
    }

    class EchoableService : IEchoable
    {
        public IAsyncResult BeginEcho(string theString, AsyncCallback callback, object state)
        {
            Console.WriteLine("WCF: Got {0}", theString);
            return new WorkflowAsyncResult(theString, callback, state);
        }

        public string EndEcho(IAsyncResult result)
        {
            return WorkflowAsyncResult.End(result);
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

    [DataContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    class WorkflowAborted
    {
        [DataMember]
        string Message = "The workflow aborted";
    }

    /// <summary>
    /// Implementation of async workflow invocation 
    /// </summary>
    class WorkflowAsyncResult : TypedAsyncResult<string>
    {
        WorkflowInstance instance;

        public WorkflowAsyncResult(string message, AsyncCallback callback, object state)
            : base(callback, state)
        {
            Dictionary<string, object> parameters = new Dictionary<string, object>();
            parameters.Add("ReceivedData", message);

            //start the workflow
            WorkflowRuntime workflowRuntime = new WorkflowRuntime();
            ManualWorkflowSchedulerService scheduler = new ManualWorkflowSchedulerService();
            workflowRuntime.AddService(scheduler);
            workflowRuntime.StartRuntime();

            workflowRuntime.WorkflowCompleted += this.OnWorkflowCompleted;
            workflowRuntime.WorkflowTerminated += this.OnWorkflowTerminated;

            this.instance = workflowRuntime.CreateWorkflow(typeof(Workflow1), parameters);

            this.instance.Start();

            scheduler.RunWorkflow(this.instance.InstanceId);
        }

        void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            base.Complete(false, new FaultException<WorkflowAborted>(new WorkflowAborted()));
        }

        void CompleteWorkflow(string result, bool synchronous)
        {
            base.Complete(result, synchronous);
        }

        //when the workflow returns call CompleteWorkflow
        void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            try
            {
                CompleteWorkflow(e.OutputParameters["ResultData"].ToString(), false);
            }
            catch (Exception ex)
            {
                base.Complete(false, ex);
            }
        }
    }
}
