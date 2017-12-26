using System;
using System.Collections.Generic;
using System.ServiceModel;
using System.Text;

namespace Microsoft.ServiceModel.Samples.Client
{
    [ServiceContract]
    public interface IEchoService
    {
        [OperationContract]
        string Echo(string message);
    }
}
