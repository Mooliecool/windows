
//-----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//-----------------------------------------------------------------

using System.Collections.Generic;
using System.ServiceModel;
using System.ServiceModel.Description;

namespace Microsoft.Samples.ConfigHierarchicalModel
{

    // Define a service contract
    [ServiceContract(Namespace="http://Microsoft.Samples.ConfigHierarchicalModel")]
    public interface IDesc
    {
        [OperationContract]
        List<string> ListEndpoints();

        [OperationContract]
        List<string> ListServiceBehaviors();
        
        [OperationContract]
        List<string> ListEndpointBehaviors();
        
        [OperationContract]
        bool HttpGetEnabled();
        
        [OperationContract]
        bool IncludeExceptionDetailInFaults();
        
        [OperationContract]
        string HttpGetUrl();
        
        [OperationContract]
        string HttpHelpPageUrl();
    }

    // Service class which implements the service contract
    public class Service : IDesc
    {
        public List<string> ListEndpoints()
        {
            List<string> listEndpoints = new List<string>();
            foreach (ServiceEndpoint endpoint in OperationContext.Current.Host.Description.Endpoints)
            {
                listEndpoints.Add("<" + endpoint.Address.ToString() + ", " + endpoint.Binding.ToString() + ", " + endpoint.Contract.ToString() + ">");
            }
            return listEndpoints;
        }

        public List<string> ListServiceBehaviors()
        {
            List<string> listServiceBehaviors = new List<string>();
            foreach (IServiceBehavior serviceBehavior in OperationContext.Current.Host.Description.Behaviors)
            {
                listServiceBehaviors.Add(serviceBehavior.ToString());
            }
            return listServiceBehaviors;
        }

        public List<string> ListEndpointBehaviors()
        {
            List<string> listEndpointBehaviors = new List<string>();
            foreach (IEndpointBehavior endpointBehavior in OperationContext.Current.Host.Description.Endpoints[0].Behaviors)
            {
                listEndpointBehaviors.Add(endpointBehavior.ToString());
            }
            return listEndpointBehaviors;
        }

        public bool HttpGetEnabled()
        {
            ServiceMetadataBehavior behavior = (ServiceMetadataBehavior)OperationContext.Current.Host.Description.Behaviors.Find<ServiceMetadataBehavior>();
            return behavior == null ? false : behavior.HttpGetEnabled;
        }

        public string HttpGetUrl()
        {
            ServiceMetadataBehavior behavior = (ServiceMetadataBehavior)OperationContext.Current.Host.Description.Behaviors.Find<ServiceMetadataBehavior>();
            return behavior == null ? "" : behavior.HttpGetUrl.ToString();
        }

        public bool IncludeExceptionDetailInFaults()
        {
            ServiceDebugBehavior behavior = (ServiceDebugBehavior)OperationContext.Current.Host.Description.Behaviors.Find<ServiceDebugBehavior>();
            return behavior == null ? false : behavior.IncludeExceptionDetailInFaults;
        }

        public string HttpHelpPageUrl()
        {
            ServiceDebugBehavior behavior = (ServiceDebugBehavior)OperationContext.Current.Host.Description.Behaviors.Find<ServiceDebugBehavior>();
            return behavior == null ? "" : behavior.HttpHelpPageUrl.ToString();
        }
    }
}
