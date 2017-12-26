using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Channels;
using System.ServiceModel;
using System.ServiceModel.Description;

namespace Microsoft.ServiceModel.Samples
{
    //If this attribute is applied to a Service implementation then it will make it shareable
	sealed class ShareableAttribute : Attribute, IServiceBehavior
	{
        #region IServiceBehavior Members

        public void AddBindingParameters(ServiceDescription description, ServiceHostBase serviceHostBase, System.Collections.ObjectModel.Collection<ServiceEndpoint> endpoints, BindingParameterCollection parameters)
        {
            //No-op
        }

        //Apply the custom IInstanceContextProvider to the EndpointDispatcher.DispatchRuntime
        public void ApplyDispatchBehavior(ServiceDescription description, ServiceHostBase serviceHostBase)
        {
			CalculatorExtension extension = new CalculatorExtension();
            foreach (ChannelDispatcherBase dispatcherBase in serviceHostBase.ChannelDispatchers)
            {
                ChannelDispatcher dispatcher = dispatcherBase as ChannelDispatcher;
                foreach (EndpointDispatcher endpointDispatcher in dispatcher.Endpoints)
                {
                    endpointDispatcher.DispatchRuntime.InstanceContextProvider = extension;
                    endpointDispatcher.DispatchRuntime.MessageInspectors.Add(extension);
                }
            }
        }

        public void Validate(ServiceDescription description, ServiceHostBase serviceHostBase)
        {
            //No-op
        }

        #endregion
    }
}
