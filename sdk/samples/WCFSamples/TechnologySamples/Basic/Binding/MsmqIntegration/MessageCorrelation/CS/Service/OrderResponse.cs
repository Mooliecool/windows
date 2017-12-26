
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.MsmqIntegration;
using Microsoft.ServiceModel.Samples;

namespace Microsoft.ServiceModel.Samples
{
    [System.ServiceModel.ServiceContractAttribute(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public interface IOrderResponse
    {

        [System.ServiceModel.OperationContractAttribute(IsOneWay = true, Action = "*")]
        void SendOrderResponse(MsmqMessage<PurchaseOrder> msg);
    }

    public partial class OrderResponseClient : System.ServiceModel.ClientBase<IOrderResponse>, IOrderResponse
    {

        public OrderResponseClient()
        { }

        public OrderResponseClient(string configurationName)
            : base(configurationName)
        { }

        public OrderResponseClient(System.ServiceModel.Channels.Binding binding, System.ServiceModel.EndpointAddress address)
            : base(binding, address)
        { }

        public void SendOrderResponse(MsmqMessage<PurchaseOrder> msg)
        {
            base.Channel.SendOrderResponse(msg);
        }
    }
}
