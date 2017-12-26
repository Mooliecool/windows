//------------------------------------------------------------------------------
// <copyright file="SwitchElementsCollection.cs" company="Microsoft Corporation">
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
using System.Configuration;
using System.Collections;
using System.Collections.Specialized;

namespace System.Diagnostics {
    [ConfigurationCollection(typeof(SwitchElement))]
    internal class SwitchElementsCollection : ConfigurationElementCollection {

        new public SwitchElement this[string name] {
            get {
                return (SwitchElement) BaseGet(name);
            }
        }
        
        public override ConfigurationElementCollectionType CollectionType {
            get {
                return ConfigurationElementCollectionType.AddRemoveClearMap;
            }
        }
    
        protected override ConfigurationElement CreateNewElement() {
            return new SwitchElement();
        }
    
        protected override Object GetElementKey(ConfigurationElement element) {
            return ((SwitchElement) element).Name;
        }
    }
    
    internal class SwitchElement : ConfigurationElement {
        private static readonly ConfigurationPropertyCollection _properties;
        private static readonly ConfigurationProperty _propName = new ConfigurationProperty("name", typeof(string), "", ConfigurationPropertyOptions.IsRequired | ConfigurationPropertyOptions.IsKey);
        private static readonly ConfigurationProperty _propValue = new ConfigurationProperty("value", typeof(string), null, ConfigurationPropertyOptions.IsRequired);

        private Hashtable _attributes;

        static SwitchElement(){
            _properties = new ConfigurationPropertyCollection();
            _properties.Add(_propName);
            _properties.Add(_propValue);
        }

        public Hashtable Attributes {
            get {
                if (_attributes == null)
                    _attributes = new Hashtable(StringComparer.OrdinalIgnoreCase);
                return _attributes;
            }
        }

        [ConfigurationProperty("name", DefaultValue = "", IsRequired = true, IsKey = true)]
        public string Name {
            get { 
                return (string) this[_propName]; 
            }
        }
        
        protected override ConfigurationPropertyCollection Properties {
            get {
                return _properties;
            }
        }

        [ConfigurationProperty("value", IsRequired = true)]
        public string Value {
            get { 
                return (string) this[_propValue]; 
            }
        }

        protected override bool OnDeserializeUnrecognizedAttribute(String name, String value)
        {
            ConfigurationProperty _propDynamic = new ConfigurationProperty(name, typeof(string), value);
            _properties.Add(_propDynamic);
            base[_propDynamic] = value; // Add them to the property bag
            Attributes.Add(name, value);
            return true;
        }

        internal void ResetProperties() 
        {
            // blow away any UnrecognizedAttributes that we have deserialized earlier 
            if (_attributes != null) {
                _attributes.Clear();
                _properties.Clear();
                _properties.Add(_propName);
                _properties.Add(_propValue);
            }
        }
    }
}

