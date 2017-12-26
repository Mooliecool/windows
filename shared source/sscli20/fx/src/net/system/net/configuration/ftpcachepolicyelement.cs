//------------------------------------------------------------------------------
// <copyright file="FtpCachePolicyElement.cs" company="Microsoft Corporation">
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
    using System.Net.Cache;
    using System.Xml;
    using System.Security.Permissions;

    public sealed class FtpCachePolicyElement : ConfigurationElement
    {
        public FtpCachePolicyElement()
        {
            this.properties.Add(this.policyLevel);
        }

        protected override ConfigurationPropertyCollection Properties
        {
            get
            {
                return this.properties;
            }
        }

        [ConfigurationProperty(ConfigurationStrings.PolicyLevel, DefaultValue = RequestCacheLevel.Default)]
        public RequestCacheLevel PolicyLevel
        {
            get { return (RequestCacheLevel)this[this.policyLevel]; }
            set { this[this.policyLevel] = value; }
        }


        protected override void DeserializeElement(XmlReader reader, bool serializeCollectionKey)
        {
            wasReadFromConfig = true;
            base.DeserializeElement(reader, serializeCollectionKey);
        }

        protected override void Reset(ConfigurationElement parentElement)
        {
            if (parentElement != null)
            {
                FtpCachePolicyElement http = (FtpCachePolicyElement)parentElement;
                this.wasReadFromConfig = http.wasReadFromConfig;
            }
            base.Reset(parentElement);
        }

        internal bool WasReadFromConfig
        {
            get { return this.wasReadFromConfig; }
        }

        bool wasReadFromConfig = false;
        ConfigurationPropertyCollection properties = new ConfigurationPropertyCollection();

        readonly ConfigurationProperty policyLevel =
            new ConfigurationProperty(ConfigurationStrings.PolicyLevel, typeof(RequestCacheLevel), RequestCacheLevel.Default, ConfigurationPropertyOptions.None);

    }

}

