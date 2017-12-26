/****************************** Module Header ******************************\
* Module Name:	ServiceRegistrySettingsElement.cs
* Project:		CSAZWorkflow4ServiceBus
* Copyright (c) Microsoft Corporation.
* 
* This class allows you to config ServiceRegistrySettings in web/app.config.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.ServiceModel.Configuration;
using Microsoft.ServiceBus;
using System.Configuration;

namespace CSAZWorkflow4ServiceBus
{
	/// <summary>
	/// This class allows you to config ServiceRegistrySettings in web/app.config.
	/// </summary>
	public class ServiceRegistrySettingsElement : BehaviorExtensionElement
	{
		public override Type BehaviorType
		{
			get { return typeof(ServiceRegistrySettings); }
		}

		protected override object CreateBehavior()
		{
			return new ServiceRegistrySettings() { DiscoveryMode = this.DiscoveryMode, DisplayName = this.DisplayName };
		}

		[ConfigurationProperty("discoveryMode", DefaultValue = DiscoveryType.Private)]
		public DiscoveryType DiscoveryMode
		{
			get { return (DiscoveryType)this["discoveryMode"]; }
			set { this["discoveryMode"] = value; }
		}

		[ConfigurationProperty("displayName")]
		public string DisplayName
		{
			get { return (string)this["displayName"]; }
			set { this["displayName"] = value; }
		}
	}
}