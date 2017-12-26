//------------------------------------------------------------------------------
// <copyright file="SerializationSectionGroup.cs" company="Microsoft Corporation">
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

    /// <summary>
    /// Summary description for SerializationSectionGroup.
    /// </summary>
    public sealed class SerializationSectionGroup : ConfigurationSectionGroup
    {
        public SerializationSectionGroup() {}

        // public properties
        [ConfigurationProperty(ConfigurationStrings.SchemaImporterExtensionsSectionName)]
        public SchemaImporterExtensionsSection SchemaImporterExtensions
        {
            get { return (SchemaImporterExtensionsSection)Sections[ConfigurationStrings.SchemaImporterExtensionsSectionName]; }
        }

        [ConfigurationProperty(ConfigurationStrings.DateTimeSerializationSectionName)]
        public DateTimeSerializationSection DateTimeSerialization 
        {
            get { return (DateTimeSerializationSection)Sections[ConfigurationStrings.DateTimeSerializationSectionName]; }
        }
        
        public XmlSerializerSection XmlSerializer 
        {
            get { return (XmlSerializerSection)Sections[ConfigurationStrings.XmlSerializerSectionName]; }
        }
    }
}
