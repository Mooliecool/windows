//------------------------------------------------------------------------------
// <copyright file="KeyValueConfigurationCollection.cs" company="Microsoft">
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
    using System.Xml;
    using System.Configuration;
    using System.Collections.Specialized;
    using System.Collections;
    using System.IO;
    using System.Text;

    [ConfigurationCollection(typeof(KeyValueConfigurationElement))]
    public class KeyValueConfigurationCollection : ConfigurationElementCollection {
        private static ConfigurationPropertyCollection _properties;

        static KeyValueConfigurationCollection() {
            // Property initialization
            _properties = new ConfigurationPropertyCollection();
        }

        protected internal override ConfigurationPropertyCollection Properties {
            get {
                return _properties;
            }
        }


        //
        // Constructor
        //
        public KeyValueConfigurationCollection() :
            base(StringComparer.OrdinalIgnoreCase) {
            internalAddToEnd = true;
        }

        //
        // Accessors
        //
        protected override bool ThrowOnDuplicate {
            get { 
                return false; 
            }
        }

        public new KeyValueConfigurationElement this[string key] {
            get {
                return (KeyValueConfigurationElement)BaseGet(key);
            }
        }

        public String[] AllKeys {
            get {
                return StringUtil.ObjectArrayToStringArray(BaseGetAllKeys());
            }
        }

        //
        // Methods
        //

        public void Add(KeyValueConfigurationElement keyValue) {
            // Need to initialize in order to get the key
            keyValue.Init();

            // the appsettings add works more like a namevalue collection add in that it appends values
            // when add is called and teh key already exists.
            KeyValueConfigurationElement oldValue = (KeyValueConfigurationElement)BaseGet(keyValue.Key);
            if (oldValue == null) {
                BaseAdd(keyValue);
            }
            else {
                oldValue.Value += "," + keyValue.Value;
                int index = BaseIndexOf(oldValue);
                BaseRemoveAt(index);
                BaseAdd(index, oldValue);
            }


        }

        public void Add(String key, String value) {
            KeyValueConfigurationElement element = new KeyValueConfigurationElement(key, value);
            Add(element);
        }

        
        public void Remove(string key) {
            BaseRemove(key);
        }
        
        public void Clear() {
            BaseClear();
        }
        
        protected override ConfigurationElement CreateNewElement() {
            return new KeyValueConfigurationElement();
        }
        
        protected override object GetElementKey(ConfigurationElement element) {
            return ((KeyValueConfigurationElement)element).Key;
        }
    }
}
