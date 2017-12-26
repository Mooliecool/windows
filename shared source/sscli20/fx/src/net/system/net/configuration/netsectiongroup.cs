//------------------------------------------------------------------------------
// <copyright file="NetSectionGroup.cs" company="Microsoft Corporation">
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
    using System.Configuration;

    /// <summary>
    /// Summary description for NetSectionGroup.
    /// </summary>
    public sealed class NetSectionGroup : ConfigurationSectionGroup
    {
        public NetSectionGroup() {}

        // public properties
        [ConfigurationProperty(ConfigurationStrings.AuthenticationModulesSectionName)]
        public AuthenticationModulesSection AuthenticationModules
        {
            get { return (AuthenticationModulesSection)Sections[ConfigurationStrings.AuthenticationModulesSectionName]; }
        }

        [ConfigurationProperty(ConfigurationStrings.ConnectionManagementSectionName)]
        public ConnectionManagementSection ConnectionManagement
        {
            get { return (ConnectionManagementSection)Sections[ConfigurationStrings.ConnectionManagementSectionName]; }
        }

        [ConfigurationProperty(ConfigurationStrings.DefaultProxySectionName)]
        public DefaultProxySection DefaultProxy
        {
            get { return (DefaultProxySection)Sections[ConfigurationStrings.DefaultProxySectionName]; }
        }


        static public NetSectionGroup GetSectionGroup(Configuration config)
        {
            if (config == null)
                throw new ArgumentNullException("config");
            return config.GetSectionGroup(ConfigurationStrings.SectionGroupName) as NetSectionGroup;
        }

        [ConfigurationProperty(ConfigurationStrings.RequestCachingSectionName)]
        public RequestCachingSection RequestCaching
        {
            get { return (RequestCachingSection)Sections[ConfigurationStrings.RequestCachingSectionName]; }
        }

        [ConfigurationProperty(ConfigurationStrings.SettingsSectionName)]
        public SettingsSection Settings
        {
            get { return (SettingsSection)Sections[ConfigurationStrings.SettingsSectionName]; }
        }

        [ConfigurationProperty(ConfigurationStrings.WebRequestModulesSectionName)]
        public WebRequestModulesSection WebRequestModules
        {
            get { return (WebRequestModulesSection)Sections[ConfigurationStrings.WebRequestModulesSectionName]; }
        }

    }
}
