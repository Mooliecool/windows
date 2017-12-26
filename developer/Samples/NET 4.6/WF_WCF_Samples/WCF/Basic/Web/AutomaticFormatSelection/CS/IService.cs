//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.ComponentModel;
using System.ServiceModel;
using System.ServiceModel.Web;

namespace Microsoft.Samples.AutomaticFormatSelection
{
    [ServiceContract]
    public interface IService
    {
        [Description("Simple echo operation over HTTP GET. The response is returned in XML or JSON based on the Accept header and on the format query string parameter.")]
        [WebGet]
        string EchoWithGet(string s);

        [Description("Simple echo operation over HTTP POST. The response is returned in the same format as the request.")]
        [WebInvoke]
        string EchoWithPost(string s);
    }
}
