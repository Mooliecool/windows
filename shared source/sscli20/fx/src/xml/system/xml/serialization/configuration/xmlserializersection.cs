//------------------------------------------------------------------------------
// <copyright file="XmlSerializerSection.cs" company="Microsoft">
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
// <owner current="true" primary="true">ElenaK</owner>                                                                
//------------------------------------------------------------------------------

namespace System.Xml.Serialization.Configuration
{
    using System;
    using System.Configuration;
    using System.ComponentModel;
    using System.Globalization;
    using System.Reflection;

    public sealed class XmlSerializerSection : ConfigurationSection
    {
        public XmlSerializerSection()
        {
            this.properties.Add(this.checkDeserializeAdvances);
        }

        protected override ConfigurationPropertyCollection Properties 
        {
            get 
            {
                return this.properties;
            }
        }

        [ConfigurationProperty(ConfigurationStrings.CheckDeserializeAdvances, DefaultValue=false)]
        public bool CheckDeserializeAdvances
        {
            get { return (bool)this[this.checkDeserializeAdvances]; }
            set { this[this.checkDeserializeAdvances] = value; }
        }

        ConfigurationPropertyCollection properties = new ConfigurationPropertyCollection();

        // Supply a type converter, even though it's a plain type converter, to get around ConfigurationProperty's internal
        // Enum conversion routine.  The internal one is case-sensitive, we want this to be case-insensitive.
        readonly ConfigurationProperty checkDeserializeAdvances =
            new ConfigurationProperty(ConfigurationStrings.CheckDeserializeAdvances, typeof(bool), false,
                    ConfigurationPropertyOptions.None);

    }

}

