//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.Xml;

namespace Microsoft.WorkflowServices.Samples
{
    [ServiceContract]
    public interface IOrder
    {
        [OperationContract(IsInitiating=true)]
        string SubmitOrder(PurchaseOrder po, IDictionary<string,string> context);
    }

    [ServiceContract]
    public interface IOrderDetails
    {
        [OperationContract]
        string OrderDetails(PurchaseOrder po, ShippingQuote quote);
    }

    [ServiceContract]
    public interface IShippingRequest
    {
        [OperationContract(IsInitiating = true)]
        string RequestShippingQuote(PurchaseOrder po, IDictionary<string, string> context);
    }

    [ServiceContract]
    public interface IShippingQuote
    {
        [OperationContract(IsOneWay = true)]
        void ShippingQuote(ShippingQuote quote);
    }

    [DataContract]
    public class PurchaseOrder
    {
        [DataMember]
        public int OrderId;
        [DataMember]
        public int Amount;
        [DataMember]
        public string CustomerName;
    }

    [DataContract]
    public class ShippingQuote
    {
        [DataMember]
        public int ShippingCost;
        [DataMember]
        public DateTime EstimatedShippingDate;
    }
}
