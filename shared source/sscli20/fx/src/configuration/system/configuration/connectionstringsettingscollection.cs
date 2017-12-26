//------------------------------------------------------------------------------
// <copyright file="ConnectionStringSettingsCollection.cs" company="Microsoft">
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

    // class ConnectionStringsSection

    [ConfigurationCollection(typeof(ConnectionStringSettings))]
    public sealed class ConnectionStringSettingsCollection : ConfigurationElementCollection {
        private static ConfigurationPropertyCollection _properties;

        static ConnectionStringSettingsCollection() {
            // Property initialization
            _properties = new ConfigurationPropertyCollection();
        }

        protected internal override ConfigurationPropertyCollection Properties {
            get {
                return _properties;
            }
        }

        public ConnectionStringSettingsCollection()
            : base(StringComparer.OrdinalIgnoreCase) {
        }
        public ConnectionStringSettings this[int index] {
            get {
                return (ConnectionStringSettings)BaseGet(index);
            }
            set {
                if (BaseGet(index) != null) {
                    BaseRemoveAt(index);
                }
                BaseAdd(index, value);
            }
        }

        new public ConnectionStringSettings this[string name] {
            get {
                return (ConnectionStringSettings)BaseGet(name);
            }
        }

        public int IndexOf(ConnectionStringSettings settings) {
            return BaseIndexOf(settings);
        }

        // the connection string behavior is strange in that is acts kind of like a
        // basic map and partially like a add remove clear collection
        // Overriding these methods allows for the specific behaviors to be
        // patterened
        protected override void BaseAdd(int index, ConfigurationElement element) {
            if (index == -1) {
                BaseAdd(element, false);
            }
            else {
                base.BaseAdd(index, element);
            }
        }
        
        public void Add(ConnectionStringSettings settings) {
            BaseAdd(settings);
        }
        
        public void Remove(ConnectionStringSettings settings) {
            if (BaseIndexOf(settings) >= 0) {
                BaseRemove(settings.Key);
            }
        }

        public void RemoveAt(int index) {
            BaseRemoveAt(index);
        }

        public void Remove(string name) {
            BaseRemove(name);
        }

        protected override ConfigurationElement CreateNewElement() {
            return new ConnectionStringSettings();
        }

        protected override Object GetElementKey(ConfigurationElement element) {
            return ((ConnectionStringSettings)element).Key;
        }
        
        public void Clear() {
            BaseClear();
        }
    }
}
