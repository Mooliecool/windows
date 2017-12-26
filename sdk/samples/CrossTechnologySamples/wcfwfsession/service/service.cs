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
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples", SessionMode = SessionMode.Required,
                     CallbackContract = typeof(ICalculatorCallback))]
    public interface ICalculator
    {
        [OperationContract(IsOneWay = true)]
        void GetResult();
        [OperationContract(IsOneWay = true)]
        void AddTo(double n);
        [OperationContract(IsOneWay = true)]
        void SubtractFrom(double n);
        [OperationContract(IsOneWay = true)]
        void MultiplyBy(double n);
        [OperationContract(IsOneWay = true)]
        void DivideBy(double n);
    }

    public interface ICalculatorCallback
    {
        [OperationContract(IsOneWay = true)]
        void Equals(string result);
        [OperationContract(IsOneWay=true, Action="http://Microsoft.ServiceModel.Samples/EchoFault")]
        void EchoFault(Message fault); 
    }

    [ServiceBehavior(InstanceContextMode = InstanceContextMode.PerSession)]
    class CalculatorService : ICalculator, IDisposable
    {
        ICalculatorCallback callback;
        WorkflowInstance instance;
        MessageVersion version;
        MathDataExchangeService mathService;

        public void GetResult()
        {
            Console.WriteLine("Received GetResult"); 
            EnsureWorkflowStarted();
            //Trigger the RequestResult event in the workflow
            mathService.TriggerResultRequested(instance.InstanceId);
        }

        void HandleResultReceived(object sender, ResultEventArgs e)
        {
            //when the workflow signals the service with the result, the service
            //sends the result back over the duplex channel
            Console.WriteLine("Calling back with Result");
            callback.Equals(e.Result);
        }

        public void AddTo(double n)
        {
            Console.WriteLine("Received AddTo");
            ExtendExpression("+", n);
        }

        public void SubtractFrom(double n)
        {
            Console.WriteLine("Received SubtractFrom");
            ExtendExpression("-", n);
        }

        public void MultiplyBy(double n)
        {
            Console.WriteLine("Received MultiplyBy");
            ExtendExpression("*", n);
        }

        public void DivideBy(double n)
        {
            Console.WriteLine("Received DivideBy");
            ExtendExpression("/", n);
        }

        void ExtendExpression(string opcode, double n)
        {
            EnsureWorkflowStarted();
            //Trigger the BuildEquation event in the workflow
            mathService.TriggerExpressionExtended(instance.InstanceId, opcode, n);
        }
        
        void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Message message =
                            Message.CreateMessage(version, FaultCode.CreateReceiverFaultCode(new FaultCode("wf")),
                            "The workflow aborted unexpectedly",
                            "http://Microsoft.ServiceModel.Samples/EchoFault");
            callback.EchoFault(message);
        }

        void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs e)
        {
            Console.WriteLine("The workflow has completed");
        }

        private void EnsureWorkflowStarted()
        {
            if (instance == null)
            {
                // we need to manage the service's subscription to the dataexchangeservice with the lifetime of the workflow

                // Aquire the callback from the current OperationContext
                callback = OperationContext.Current.GetCallbackChannel<ICalculatorCallback>();
                //grab the version of the incoming message in case we need to create a fault later
                version = OperationContext.Current.IncomingMessageVersion;
                
                // retrieve the WorkflowRuntime from our WFServiceHostExtension
                WorkflowRuntime workflowRuntime = OperationContext.Current.Host.Extensions.Find<WFServiceHostExtension>().WorkflowRuntime;

                // grab a reference to our mathService
                ExternalDataExchangeService dataExchangeService = workflowRuntime.GetService<ExternalDataExchangeService>();
                mathService = (MathDataExchangeService)dataExchangeService.GetService(typeof(MathDataExchangeService));

                // if it is null set it up
                if (mathService == null)
                {
                    mathService = new MathDataExchangeService();
                    dataExchangeService.AddService(mathService);
                }

                //register with the ResultReceived event
                mathService.ResultReceived += HandleResultReceived;

                //register to get signaled when the workflow completes
                workflowRuntime.WorkflowCompleted += this.OnWorkflowCompleted;
                workflowRuntime.WorkflowTerminated += this.OnWorkflowTerminated;

                //create and start the Workflow
                instance = workflowRuntime.CreateWorkflow(typeof(Workflow1));

                instance.Start();
            }
        }

        public void Dispose()
        {
            mathService.ResultReceived -= HandleResultReceived;

            instance.WorkflowRuntime.WorkflowCompleted -= this.OnWorkflowCompleted;
            instance.WorkflowRuntime.WorkflowTerminated -= this.OnWorkflowTerminated;
        }
    }
}
