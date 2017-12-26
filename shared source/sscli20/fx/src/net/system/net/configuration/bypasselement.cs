//------------------------------------------------------------------------------
// <copyright file="BypassElement.cs" company="Microsoft Corporation">
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

    public sealed class BypassElement : ConfigurationElement
    {
        public BypassElement()
        {
            this.properties.Add(this.address);
        }

        public BypassElement(string address) : this()
        {
            this.Address = address;
        }

        protected override ConfigurationPropertyCollection Properties
        {
            get 
            {
                return this.properties;
            }
        }

        [ConfigurationProperty(ConfigurationStrings.Address, IsRequired=true, IsKey = true)]
        public string Address
        {
            get { return (string)this[this.address]; }
            set { this[this.address] = value; }
        }

        internal string Key
        {
            get { return this.Address; }
        }

        ConfigurationPropertyCollection properties = new ConfigurationPropertyCollection();

        readonly ConfigurationProperty address =
            new ConfigurationProperty(ConfigurationStrings.Address, typeof(string), null,
                    ConfigurationPropertyOptions.IsKey);

    }
}

