//------------------------------------------------------------------------------
// <copyright file="KeyValueInternalCollection.cs" company="Microsoft">
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

    // class AppSettingsSection

    class KeyValueInternalCollection : NameValueCollection {
        private AppSettingsSection _root = null;
        public KeyValueInternalCollection(AppSettingsSection root) {
            _root = root;
            foreach (KeyValueConfigurationElement element in _root.Settings) {
                base.Add(element.Key, element.Value);
            }
        }

        public override void Add(String key, String value) {
            _root.Settings.Add(new KeyValueConfigurationElement(key, value));
            base.Add(key, value);
        }

        public override void Clear() {
            _root.Settings.Clear();
            base.Clear();
        }

        public override void Remove(string key) {
            _root.Settings.Remove(key);
            base.Remove(key);
        }


    }
}
