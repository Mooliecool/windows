using System;
using System.Collections.Generic;
using System.Configuration;
using System.ServiceModel;
using System.ServiceModel.Configuration;
using System.ServiceModel.Description;
using System.Xml;

namespace Microsoft.ServiceModel.Samples
{
    class EndpointValidateElement : BehaviorExtensionElement
    {
        protected override object CreateBehavior()
        {
            return new EndpointValidateBehavior();
        }

        public override Type BehaviorType
        {
            get { return typeof(EndpointValidateBehavior); }
        }
    }
}
