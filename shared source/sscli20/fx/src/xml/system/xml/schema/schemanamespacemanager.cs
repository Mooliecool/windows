//------------------------------------------------------------------------------
// <copyright file="SchemaNamespaceManager.cs" company="Microsoft">
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
// <owner current="true" primary="true">priyal</owner>                                                                
//------------------------------------------------------------------------------

namespace System.Xml.Schema {
    using System;
    using System.Diagnostics;
    using System.Collections;

    internal class SchemaNamespaceManager : XmlNamespaceManager {
        XmlSchemaObject node;
    
        public SchemaNamespaceManager(XmlSchemaObject node) {
            this.node = node;
        }
        
        public override string LookupNamespace(string prefix) {
            if (prefix == "xml") { //Special case for the XML namespace
                return XmlReservedNs.NsXml;
            }
            Hashtable namespaces;
            for (XmlSchemaObject current = node; current != null; current = current.Parent) {
                namespaces = current.Namespaces.Namespaces;
                if (namespaces != null && namespaces.Count > 0) {
                    object uri = namespaces[prefix];
                    if (uri != null)
                        return (string)uri;
                }
            }
            return prefix.Length == 0 ? string.Empty : null;
        }

        public override string LookupPrefix(string ns) {
            if (ns == XmlReservedNs.NsXml) { //Special case for the XML namespace
                return "xml";
            }
            Hashtable namespaces;
            for (XmlSchemaObject current = node; current != null; current = current.Parent) {
                namespaces = current.Namespaces.Namespaces;
                if (namespaces != null && namespaces.Count > 0) {
                    foreach(DictionaryEntry entry in namespaces) {
                        if (entry.Value.Equals(ns)) {
                            return (string)entry.Key;
                        }
                    }
                }
            }
            return null;
        }

  }; //SchemaNamespaceManager
}
