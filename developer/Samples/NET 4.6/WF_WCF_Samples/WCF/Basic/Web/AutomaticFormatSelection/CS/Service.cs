//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Net;
using System.ServiceModel.Web;

namespace Microsoft.Samples.AutomaticFormatSelection
{
    public class Service : IService
    {
        public string EchoWithGet(string s)
        {
            // if a format query string parameter has been specified, set the response format to that. If no such
            // query string parameter exists the Accept header will be used
            string formatQueryStringValue = WebOperationContext.Current.IncomingRequest.UriTemplateMatch.QueryParameters["format"];
            if (!string.IsNullOrEmpty(formatQueryStringValue))
            {
                if (formatQueryStringValue.Equals("xml", System.StringComparison.OrdinalIgnoreCase))
                {
                    WebOperationContext.Current.OutgoingResponse.Format = WebMessageFormat.Xml;
                }
                else if (formatQueryStringValue.Equals("json", System.StringComparison.OrdinalIgnoreCase))
                {
                    WebOperationContext.Current.OutgoingResponse.Format = WebMessageFormat.Json;
                }
                else
                {
                    throw new WebFaultException<string>(string.Format("Unsupported format '{0}'", formatQueryStringValue), HttpStatusCode.BadRequest);
                }
            }
            return "You said " + s;
        }

        public string EchoWithPost(string s)
        {
            return "You said " + s;
        }
    }
}
