//------------------------------------------------------------------------------
// <copyright file="SourceElementsCollection .cs" company="Microsoft Corporation">
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
using System.Xml;

namespace System.Diagnostics {
    [ConfigurationCollection(typeof(SourceElement), AddItemName = "source",
     CollectionType = ConfigurationElementCollectionType.BasicMap)]
    internal class SourceElementsCollection  : ConfigurationElementCollection {

        new public SourceElement this[string name] {
            get {
                return (SourceElement) BaseGet(name);
            }
        }
        
        protected override string ElementName {
            get {
                return "source";
            }
        }

        public override ConfigurationElementCollectionType CollectionType {
            get {
                return ConfigurationElementCollectionType.BasicMap;
            }
        }

        protected override ConfigurationElement CreateNewElement() {
            SourceElement se = new SourceElement();
            se.Listeners.InitializeDefaultInternal();
            return se;
        }

        protected override Object GetElementKey(ConfigurationElement element) {
            return ((SourceElement) element).Name;
        }
    }


    internal class SourceElement : ConfigurationElement {
        private static readonly ConfigurationPropertyCollection _properties;
        private static readonly ConfigurationProperty _propName = new ConfigurationProperty("name", typeof(string), "", ConfigurationPropertyOptions.IsRequired);
        private static readonly ConfigurationProperty _propSwitchName = new ConfigurationProperty("switchName", typeof(string), null, ConfigurationPropertyOptions.None);
        private static readonly ConfigurationProperty _propSwitchValue = new ConfigurationProperty("switchValue", typeof(string), null, ConfigurationPropertyOptions.None);
        private static readonly ConfigurationProperty _propSwitchType = new ConfigurationProperty("switchType", typeof(string), null, ConfigurationPropertyOptions.None);
        private static readonly ConfigurationProperty _propListeners = new ConfigurationProperty("listeners", typeof(ListenerElementsCollection), new ListenerElementsCollection(), ConfigurationPropertyOptions.None);

        private Hashtable _attributes;

        static SourceElement() {
            _properties = new ConfigurationPropertyCollection();
            _properties.Add(_propName);
            _properties.Add(_propSwitchName);
            _properties.Add(_propSwitchValue);
            _properties.Add(_propSwitchType);
            _properties.Add(_propListeners);
        }

        public Hashtable Attributes {
            get {
                if (_attributes == null)
                    _attributes = new Hashtable(StringComparer.OrdinalIgnoreCase);
                return _attributes;
            }
        }

        [ConfigurationProperty("listeners")]
        public ListenerElementsCollection Listeners {
            get {
                return (ListenerElementsCollection) this[_propListeners];
            }
        }

        [ConfigurationProperty("name", IsRequired=true, DefaultValue="")]
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

        [ConfigurationProperty("switchName")]
        public string SwitchName {
            get { 
                return (string) this[_propSwitchName]; 
            }
        }

        [ConfigurationProperty("switchValue")]
        public string SwitchValue {
            get { 
                return (string) this[_propSwitchValue]; 
            }
        }

        [ConfigurationProperty("switchType")]
        public string SwitchType {
            get { 
                return (string) this[_propSwitchType];
            }
        }
                

        protected override void DeserializeElement(XmlReader reader, bool serializeCollectionKey)
        {
            base.DeserializeElement(reader, serializeCollectionKey);

            if (!String.IsNullOrEmpty(SwitchName) && !String.IsNullOrEmpty(SwitchValue))
                throw new ConfigurationErrorsException(SR.GetString(SR.Only_specify_one, Name));
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
                _properties.Add(_propSwitchName);
                _properties.Add(_propSwitchValue);
                _properties.Add(_propSwitchType);
                _properties.Add(_propListeners);
            }
        }
    }
        
}

