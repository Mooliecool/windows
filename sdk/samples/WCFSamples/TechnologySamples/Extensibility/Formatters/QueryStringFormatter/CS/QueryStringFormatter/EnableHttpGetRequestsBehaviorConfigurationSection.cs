
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

namespace Microsoft.ServiceModel.Samples
{
    using System;
    using System.Configuration;
    using System.ServiceModel;
    using System.ServiceModel.Configuration;

    public class EnableHttpGetRequestsBehaviorElement : BehaviorExtensionElement
    {
        protected override object CreateBehavior()
        {
            return new EnableHttpGetRequestsBehavior();
        }

        public override Type BehaviorType
        {
            get { return typeof(EnableHttpGetRequestsBehavior); }
        }
    }
}
