//  Copyright (c) Microsoft Corporation. All rights reserved.

using System.Collections.Specialized;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Web;

namespace Microsoft.WebProgrammingModel.Samples
{
    public class FormProcessingBehavior : WebHttpBehavior
    {
        protected override IDispatchMessageFormatter GetRequestDispatchFormatter(OperationDescription operationDescription, ServiceEndpoint endpoint)
        {
            //Messages[0] is the request message
            MessagePartDescriptionCollection parts = operationDescription.Messages[0].Body.Parts;

            //This formatter looks for [WebInvoke] operations with that have their last
            //parameter typed as NameValueCollection
            if (operationDescription.Behaviors.Find<WebInvokeAttribute>() != null &&
                parts.Count > 0 &&
                parts[parts.Count - 1].Type == typeof(NameValueCollection))
            {
                return new FormDataRequestFormatter(operationDescription);
            }
            else
            {
                return base.GetRequestDispatchFormatter(operationDescription, endpoint);
            }
        }
    }
}
