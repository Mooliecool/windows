//------------------------------------------------------------------------------
// <copyright file="WebProxyScriptElement.cs" company="Microsoft Corporation">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.Net.Configuration
{
    using System;
    using System.Configuration;
    using System.Security.Permissions;

    public sealed class WebProxyScriptElement : ConfigurationElement
    {
        public WebProxyScriptElement()
        {
            this.properties.Add(this.downloadTimeout);
            /* Not used with Managed JScript
            this.properties.Add(this.executionTimeout);
            */
        }

        protected override void PostDeserialize()
        {
            // Perf optimization. If the configuration is coming from machine.config
            // It is safe and we don't need to check for permissions.
            if (EvaluationContext.IsMachineLevel)
                return;

            try {
                ExceptionHelper.WebPermissionUnrestricted.Demand();
            } catch (Exception exception) {

                throw new ConfigurationErrorsException(
                              SR.GetString(SR.net_config_element_permission,
                                           ConfigurationStrings.WebProxyScript),
                              exception);
            }
        }

        [ConfigurationProperty(ConfigurationStrings.DownloadTimeout, DefaultValue = "00:02:00")]
        public TimeSpan DownloadTimeout
        {
            get { return (TimeSpan) this[this.downloadTimeout]; }
            set { this[this.downloadTimeout] = value; }
        }

/* Not used with Managed JScript
        [ConfigurationProperty(ConfigurationStrings.ExecutionTimeout, DefaultValue = "00:00:05")]
        public TimeSpan ExecutionTimeout
        {
            get { return (TimeSpan) this[this.executionTimeout]; }
            set { this[this.executionTimeout] = value; }
        }
*/

        protected override ConfigurationPropertyCollection Properties
        {
            get 
            {
                return this.properties;
            }
        }

        ConfigurationPropertyCollection properties = new ConfigurationPropertyCollection();

        readonly ConfigurationProperty downloadTimeout =
            new ConfigurationProperty(ConfigurationStrings.DownloadTimeout, 
                                      typeof(TimeSpan), 
                                      TimeSpan.FromMinutes(1), 
                                      ConfigurationPropertyOptions.None);

/* Not used with Managed JScript
        readonly ConfigurationProperty executionTimeout =
            new ConfigurationProperty(ConfigurationStrings.ExecutionTimeout, 
                                      typeof(TimeSpan), 
                                      TimeSpan.FromSeconds(5), 
                                      ConfigurationPropertyOptions.None);
*/
    }
}
