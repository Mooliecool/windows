//------------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

namespace Microsoft.ServiceModel.Samples
{
    using System;
    using System.Configuration;
    using System.Globalization;
    using System.Text;
    using System.ComponentModel;
    using System.Net;
    using System.ServiceModel;
    using System.ServiceModel.Channels;
    using System.ServiceModel.Configuration;
    using System.ServiceModel.Security;

	public class WSEHttpBindingConfigurationElement : StandardBindingElement
	{

		protected override Type BindingElementType
		{
			get { throw new Exception("The method or operation is not implemented."); }
		}

		protected override void OnApplyConfiguration(Binding binding)
		{
			throw new Exception("The method or operation is not implemented.");
		}
}


}
