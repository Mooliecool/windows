//------------------------------------------------------------------------------
// <copyright file="AuthenticationModuleElement.cs" company="Microsoft Corporation">
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

    public sealed class AuthenticationModuleElement : ConfigurationElement
    {
        public AuthenticationModuleElement()
        {
            this.properties.Add(this.type);
        }

        public AuthenticationModuleElement(string typeName) : this()
        {
            if (typeName != (string)this.type.DefaultValue)
            {
                this.Type = typeName;
            }
        }

        protected override ConfigurationPropertyCollection Properties
        {
            get 
            {
                return this.properties;
            }
        }

        [ConfigurationProperty(ConfigurationStrings.Type, IsRequired=true, IsKey = true)]
        public string Type
        {
            get { return (string)this[this.type]; }
            set { this[this.type] = value; }
        }

        internal string Key
        {
            get { return this.Type; }
        }
        
        ConfigurationPropertyCollection properties = new ConfigurationPropertyCollection();

        readonly ConfigurationProperty type = 
            new ConfigurationProperty(ConfigurationStrings.Type, 
                                      typeof(string), 
                                      null, 
                                      ConfigurationPropertyOptions.IsKey);
    }
}

