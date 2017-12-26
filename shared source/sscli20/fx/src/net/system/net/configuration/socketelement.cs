//------------------------------------------------------------------------------
// <copyright file="SocketElement.cs" company="Microsoft Corporation">
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
    using System.Reflection;
    using System.Security.Permissions;

    public sealed class SocketElement : ConfigurationElement
    {
        public SocketElement()
        {
            this.properties.Add(this.alwaysUseCompletionPortsForAccept);
            this.properties.Add(this.alwaysUseCompletionPortsForConnect);
        }

        protected override void PostDeserialize()
        {
            // Perf optimization. If the configuration is coming from machine.config
            // It is safe and we don't need to check for permissions.
            if (EvaluationContext.IsMachineLevel)
                return;

            try {
                ExceptionHelper.UnrestrictedSocketPermission.Demand();
            } catch (Exception exception) {
                throw new ConfigurationErrorsException(
                              SR.GetString(SR.net_config_element_permission, 
                                           ConfigurationStrings.Socket),
                              exception);
            }
        }

        [ConfigurationProperty(ConfigurationStrings.AlwaysUseCompletionPortsForAccept, DefaultValue = false)]
        public bool AlwaysUseCompletionPortsForAccept
        {
            get { return (bool)this[this.alwaysUseCompletionPortsForAccept]; }
            set { this[this.alwaysUseCompletionPortsForAccept] = value; }
        }

        [ConfigurationProperty(ConfigurationStrings.AlwaysUseCompletionPortsForConnect, DefaultValue = false)]
        public bool AlwaysUseCompletionPortsForConnect
        {
            get { return (bool)this[this.alwaysUseCompletionPortsForConnect]; }
            set { this[this.alwaysUseCompletionPortsForConnect] = value; }
        }

        protected override ConfigurationPropertyCollection Properties
        {
            get 
            {
                return this.properties;
            }
        }

        ConfigurationPropertyCollection properties = new ConfigurationPropertyCollection();

        readonly ConfigurationProperty alwaysUseCompletionPortsForConnect =
            new ConfigurationProperty(ConfigurationStrings.AlwaysUseCompletionPortsForConnect, typeof(bool), false,
                    ConfigurationPropertyOptions.None);

        readonly ConfigurationProperty alwaysUseCompletionPortsForAccept =
            new ConfigurationProperty(ConfigurationStrings.AlwaysUseCompletionPortsForAccept, typeof(bool), false,
                    ConfigurationPropertyOptions.None);

    }
}

