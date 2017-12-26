//------------------------------------------------------------------------------
// <copyright file="SchemaImporterExtensionsSection.cs" company="Microsoft Corporation">
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
    using System.Configuration;
    using System.Collections;
    using System.Globalization;
    using System.Reflection;
    using System.Threading;
    using System.Xml.Serialization.Advanced;

    public sealed class SchemaImporterExtensionsSection : ConfigurationSection
    {
        public SchemaImporterExtensionsSection() 
        {
            this.properties.Add(this.schemaImporterExtensions);
        }


        protected override void InitializeDefault()
        {
        }

        protected override ConfigurationPropertyCollection Properties
        {
            get { return this.properties; }
        }

        [ConfigurationProperty("", IsDefaultCollection = true)]
        public SchemaImporterExtensionElementCollection SchemaImporterExtensions
        {
            get { return (SchemaImporterExtensionElementCollection)this[this.schemaImporterExtensions]; }
        }

        internal SchemaImporterExtensionCollection SchemaImporterExtensionsInternal {
            get {
                SchemaImporterExtensionCollection extensions = new SchemaImporterExtensionCollection();
                foreach(SchemaImporterExtensionElement elem in this.SchemaImporterExtensions) {
                    extensions.Add(elem.Name, elem.Type);
                }

                return extensions;
            }
        }

        ConfigurationPropertyCollection properties = new ConfigurationPropertyCollection();

        readonly ConfigurationProperty schemaImporterExtensions =
            new ConfigurationProperty(null, typeof(SchemaImporterExtensionElementCollection), null,
                    ConfigurationPropertyOptions.IsDefaultCollection);
    }
}
