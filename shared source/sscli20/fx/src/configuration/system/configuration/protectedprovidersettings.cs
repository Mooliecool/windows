//------------------------------------------------------------------------------
// <copyright file="ProtectedProviderSettings.cs" company="Microsoft">
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

namespace System.Configuration
{
    using System.Collections;
    using System.Collections.Specialized;
    using System.Xml;
    using System.Globalization;

    public class ProtectedProviderSettings : ConfigurationElement
    {
        private ConfigurationPropertyCollection _properties;
        private readonly ConfigurationProperty _propProviders =
            new ConfigurationProperty(null, typeof(ProviderSettingsCollection), null, ConfigurationPropertyOptions.IsDefaultCollection);

        public ProtectedProviderSettings()
        {
            // Property initialization
            _properties = new ConfigurationPropertyCollection();
            _properties.Add(_propProviders);
        }

        protected internal override ConfigurationPropertyCollection Properties
        {
            get
            {
                return _properties;
            }
        }


        [ConfigurationProperty("", IsDefaultCollection = true, Options = ConfigurationPropertyOptions.IsDefaultCollection)]
        public ProviderSettingsCollection Providers
        {
            get
            {
                return (ProviderSettingsCollection)base[_propProviders];
            }
        }
    }
}
