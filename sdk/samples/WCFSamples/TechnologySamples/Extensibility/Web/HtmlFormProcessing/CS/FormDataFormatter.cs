//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Specialized;
using System.IO;
using System.Linq;
using System.Reflection;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Web;

namespace Microsoft.WebProgrammingModel.Samples
{
    class FormDataRequestFormatter : IDispatchMessageFormatter
    {
        OperationDescription operation;

        internal FormDataRequestFormatter( OperationDescription operation )
        {
            if (operation.BeginMethod != null || operation.EndMethod != null)
                throw new InvalidOperationException("The async programming model is not supported by this formatter.");

            this.operation = operation;
        }

        public void DeserializeRequest(System.ServiceModel.Channels.Message message, object[] parameters)
        {
            if (WebOperationContext.Current.IncomingRequest.ContentType != "application/x-www-form-urlencoded")
                throw new InvalidDataException("Unexpected content type");

            Stream s = StreamMessageHelper.GetStream(message);
            string formData = new StreamReader(s).ReadToEnd();

            NameValueCollection parsedForm = System.Web.HttpUtility.ParseQueryString(formData);

            UriTemplateMatch match = message.Properties["UriTemplateMatchResults"] as UriTemplateMatch;

            ParameterInfo[] paramInfos = operation.SyncMethod.GetParameters();

            var binder = CreateParameterBinder( match );
            object[] values = (from p in paramInfos
                               select binder(p)).ToArray<Object>();

            values[paramInfos.Length - 1] = parsedForm;

            values.CopyTo(parameters, 0);
        }

        private Func<ParameterInfo, object> CreateParameterBinder(UriTemplateMatch match)
        {
            QueryStringConverter converter = new QueryStringConverter();

            return delegate( ParameterInfo pi )
            {
                string value = match.BoundVariables[pi.Name];

                if (converter.CanConvert(pi.ParameterType) && value != null)
                    return converter.ConvertStringToValue(value, pi.ParameterType);
                else

                return value;
            };
        }

        public System.ServiceModel.Channels.Message SerializeReply(System.ServiceModel.Channels.MessageVersion messageVersion, object[] parameters, object result)
        {
            throw new NotSupportedException();
        }
    }
}
