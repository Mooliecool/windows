using System;
using System.ServiceModel;
using System.ServiceModel.Web;
using System.ServiceModel.Channels;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Configuration;
using System.ServiceModel.Description;


namespace Microsoft.Ajax.Samples
{

    public class JSONPBehavior : Attribute, IOperationBehavior
    {
        public string callback;
        #region IOperationBehavior Members
        public void AddBindingParameters(
          OperationDescription operationDescription, BindingParameterCollection bindingParameters
        )
        { return; }

        public void ApplyClientBehavior(OperationDescription operationDescription, ClientOperation clientOperation)
        {
            clientOperation.ParameterInspectors.Add(new Inspector(callback));
        }

        public void ApplyDispatchBehavior(OperationDescription operationDescription, DispatchOperation dispatchOperation)
        {
            dispatchOperation.ParameterInspectors.Add(new Inspector(callback));
        }

        public void Validate(OperationDescription operationDescription) { return; }

        #endregion
        
        //Parameter inspector
        class Inspector : IParameterInspector
        {
            string callback;
            public Inspector(string callback)
            {
                this.callback = callback;
            }

            public void AfterCall(string operationName, object[] outputs, object returnValue, object correlationState)
            {
            }

            public object BeforeCall(string operationName, object[] inputs)
            {
                string methodName = WebOperationContext.Current.IncomingRequest.UriTemplateMatch.QueryParameters[callback];
                if(methodName !=null)
                {                    
                    //System.ServiceModel.OperationContext.Current.OutgoingMessageProperties["wrapper"] = inputs[0];
                    JSONPMessageProperty property = new JSONPMessageProperty()
                    {
                        MethodName = methodName
                    };
                    OperationContext.Current.OutgoingMessageProperties.Add(JSONPMessageProperty.Name, property);
                }
                return null;
            }
        }

    }
}
