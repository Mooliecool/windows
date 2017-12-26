//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.ComponentModel;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Web;

namespace Microsoft.Samples.AdvancedFormatSelection
{
    [ServiceContract]
    public interface IService
    {
        [Description("Simple echo operation over HTTP GET. The request should include a comma-seperated list of items via the 'list' query string parameter.  The response is returned as either JPEG, XHTML, Atom, XML or JSON based on the 'format' query string parameter or the Accept Header of the request.")]
        [WebGet]
        Message EchoListWithGet(string list);
    }
}
