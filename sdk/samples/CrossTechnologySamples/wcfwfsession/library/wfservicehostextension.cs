using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;
using System.Workflow.Runtime;
using System.Workflow.Activities;

namespace Microsoft.ServiceModel.Samples
{
    public class WFServiceHostExtension : IExtension<ServiceHostBase>, IDisposable
    {

        private WorkflowRuntime workflowRuntime;
        private string workflowServicesConfig;

        public WFServiceHostExtension() { }

        public WFServiceHostExtension(string WorkflowServicesConfig)
        {
            workflowServicesConfig = WorkflowServicesConfig;
        }


        void IExtension<ServiceHostBase>.Attach(ServiceHostBase owner)
        {
            // When this Extension is attached within the Service Host, create a
            // ...new instance of the WorkflowServiceContainer
            if (workflowServicesConfig != null)
                workflowRuntime = new WorkflowRuntime(workflowServicesConfig);
            else
                workflowRuntime = new WorkflowRuntime();

            workflowRuntime.ServicesExceptionNotHandled += new EventHandler<ServicesExceptionNotHandledEventArgs>(workflowRuntime_ServicesExceptionNotHandled);

            ExternalDataExchangeService dataExchangeService = new ExternalDataExchangeService();
            workflowRuntime.AddService(dataExchangeService);

            // Start the services associated with the container
            workflowRuntime.StartRuntime();
        }

        void workflowRuntime_ServicesExceptionNotHandled(object sender, ServicesExceptionNotHandledEventArgs e)
        {
            Console.WriteLine("ServicesExceptionNotHandled");
        }

        void IExtension<ServiceHostBase>.Detach(ServiceHostBase owner)
        {
            // When this Indigo Extension is detached, then just stop the WSC
            workflowRuntime.StopRuntime();
        }


        public WorkflowRuntime WorkflowRuntime
        {
            get
            {
                return workflowRuntime;
            }
        }

        #region IDisposable Members

        public void Dispose()
        {
            workflowRuntime.Dispose();
        }

        #endregion
    }
}
