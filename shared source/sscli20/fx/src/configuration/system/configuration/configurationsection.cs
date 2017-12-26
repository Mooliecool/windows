//------------------------------------------------------------------------------
// <copyright file="ConfigurationSection.cs" company="Microsoft">
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

namespace System.Configuration {
    using System;
    using System.Collections.Specialized;
    using System.Configuration.Internal;
    using System.IO;
    using System.Reflection;
    using System.Security;
    using System.Text;
    using System.Xml;
    using System.Globalization;

    public abstract class ConfigurationSection : ConfigurationElement {

        private SectionInformation _section;

        // Constructor
        //
        protected ConfigurationSection() {
            _section = new SectionInformation( this );
        }

        // SectionInformation property 
        //
        // Retrieve the class associated with the Section information
        //
        public SectionInformation SectionInformation {
            get {
                return _section;
            }
        }

        // GetRuntimeObject
        //
        // Return the Runtime Object for this Section
        //
        protected internal virtual object GetRuntimeObject() {
            return this;
        }


        protected internal override bool IsModified() {
            return ( SectionInformation.IsModifiedFlags() ||
                     base.IsModified() );
        }

        protected internal override void ResetModified() {
            SectionInformation.ResetModifiedFlags();
            base.ResetModified();
        }

        protected internal virtual void DeserializeSection(XmlReader reader) {
            if (!reader.Read() || reader.NodeType != XmlNodeType.Element) {
                throw new ConfigurationErrorsException(SR.GetString(SR.Config_base_expected_to_find_element), reader);
            }
            DeserializeElement(reader, false);
        }

        protected internal virtual string SerializeSection(ConfigurationElement parentElement, string name, ConfigurationSaveMode saveMode) {
            ValidateElement(this, null, true);

            ConfigurationElement TempElement = CreateElement(this.GetType());
            TempElement.Unmerge(this, parentElement, saveMode);

            StringWriter strWriter = new StringWriter(CultureInfo.InvariantCulture);
            XmlTextWriter writer = new XmlTextWriter(strWriter);
            writer.Formatting = Formatting.Indented;
            writer.Indentation = 4;
            writer.IndentChar = ' ';
            TempElement.DataToWriteInternal = (saveMode != ConfigurationSaveMode.Minimal);
            TempElement.SerializeToXmlElement(writer, name);
            writer.Flush();
            return strWriter.ToString();
        }
    }
}
