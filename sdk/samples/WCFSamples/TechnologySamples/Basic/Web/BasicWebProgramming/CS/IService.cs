//  Copyright (c) Microsoft Corporation. All rights reserved.

using System.ServiceModel;
using System.ServiceModel.Web;

namespace Microsoft.WebProgrammingModel.Samples
{
    [ServiceContract]
    public interface IService
    {
        [OperationContract]
        [WebGet]
        string EchoWithGet(string s);

        [OperationContract]
        [WebInvoke]
        string EchoWithPost(string s);
    }
}
