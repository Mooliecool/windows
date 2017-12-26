//------------------------------------------------------------------------------
// <copyright file="ConnectionStringsSection.cs" company="Microsoft">
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

    public sealed class ConnectionStringsSection : ConfigurationSection {
        private static ConfigurationPropertyCollection _properties;
        private static readonly ConfigurationProperty _propConnectionStrings =
            new ConfigurationProperty(null, typeof(ConnectionStringSettingsCollection), null, 
                                      ConfigurationPropertyOptions.IsDefaultCollection);

        static ConnectionStringsSection() {
            // Property initialization
            _properties = new ConfigurationPropertyCollection();
            _properties.Add(_propConnectionStrings);
        }

        public ConnectionStringsSection() {
        }

        protected internal override object GetRuntimeObject() {
            SetReadOnly();
            return this;            // return the read only object
        }

        protected internal override ConfigurationPropertyCollection Properties {
            get {
                return _properties;
            }
        }

        [ConfigurationProperty("", Options = ConfigurationPropertyOptions.IsDefaultCollection)]
        public ConnectionStringSettingsCollection ConnectionStrings {
            get {
                return (ConnectionStringSettingsCollection)base[_propConnectionStrings];
            }
        }
    }
}
