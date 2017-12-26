//------------------------------------------------------------------------------
// <copyright file="ModuleElement.cs" company="Microsoft Corporation">
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

    public sealed class ModuleElement : ConfigurationElement
    {
        public ModuleElement()
        {
            this.properties.Add(this.type);
        }

        protected override ConfigurationPropertyCollection Properties 
        {
            get 
            {
                return this.properties;
            }
        }

        [ConfigurationProperty(ConfigurationStrings.Type)]
        public string Type
        {
            get { return (string)this[this.type]; }
            set { this[this.type] = value; }
        }

        ConfigurationPropertyCollection properties = new ConfigurationPropertyCollection();

        readonly ConfigurationProperty type = 
            new ConfigurationProperty(ConfigurationStrings.Type, 
                                      typeof(string), 
                                      null, 
                                      ConfigurationPropertyOptions.None);

    }
}

