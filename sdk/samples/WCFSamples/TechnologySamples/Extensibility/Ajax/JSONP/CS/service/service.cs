//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System.ServiceModel;
using System.ServiceModel.Web;
using System.Runtime.Serialization;

namespace Microsoft.Ajax.Samples
{
    //Service contract
    [ServiceContract()]
    public interface ICustomerService
    {
        [OperationContract]
        [WebGet(ResponseFormat = WebMessageFormat.Json)]
        [JSONPBehavior(callback = "method")]
        Customer GetCustomer(string method);
    }

    [DataContract()]
    public class Customer
    {
        public Customer() { }
        [DataMember]
        public string Name;
        [DataMember]
        public string Address;
    }

    //Service Implementation
    public class CustomerService : ICustomerService
    {
        public CustomerService() { }

        public Customer GetCustomer(string method)
        {
           return new Customer() { Name="Bob", Address="1 Example Way"};
        }
    }
}
