using System;
using System.ServiceModel.Configuration;

namespace Microsoft.ServiceModel.Samples
{
    public class InternetClientValidatorElement : BehaviorExtensionElement
    {
        public override Type BehaviorType
        {
            get { return typeof(InternetClientValidatorBehavior); }
        }

        protected override object CreateBehavior()
        {
            return new InternetClientValidatorBehavior();
        }
    }
}
