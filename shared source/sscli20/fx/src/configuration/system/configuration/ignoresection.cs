//------------------------------------------------------------------------------
// <copyright file="IgnoreSection.cs" company="Microsoft">
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
    using System.Xml;

    sealed public class IgnoreSection : ConfigurationSection {
        private static ConfigurationPropertyCollection  s_properties;
        
        string  _rawXml = string.Empty;
        bool    _isModified;

        private static ConfigurationPropertyCollection EnsureStaticPropertyBag() {
            if (s_properties == null) {
                ConfigurationPropertyCollection properties = new ConfigurationPropertyCollection();
                s_properties = properties;
            }

            return s_properties;
        }

        public IgnoreSection() {
            EnsureStaticPropertyBag();
        }

        protected internal override ConfigurationPropertyCollection Properties {
            get {
                return EnsureStaticPropertyBag();
            }
        }

        protected internal override bool IsModified() {
            return _isModified;
        }

        protected internal override void ResetModified() {
            _isModified = false;
        }

        protected internal override void Reset(ConfigurationElement parentSection) {
            _rawXml = string.Empty;
            _isModified = false;
        }

        protected internal override void DeserializeSection(XmlReader xmlReader) {
            if (!xmlReader.Read() || xmlReader.NodeType != XmlNodeType.Element) {
                throw new ConfigurationErrorsException(SR.GetString(SR.Config_base_expected_to_find_element), xmlReader);
            }
            _rawXml = xmlReader.ReadOuterXml();
            _isModified = true;
        }

        protected internal override string SerializeSection(ConfigurationElement parentSection, string name, ConfigurationSaveMode saveMode) {
            return _rawXml;
        }
    }
}

