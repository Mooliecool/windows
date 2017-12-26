//------------------------------------------------------------------------------
// <copyright file="XmlILIndex.cs" company="Microsoft">
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
using System;
using System.Collections;
using System.Xml.XPath;
using System.ComponentModel;

namespace System.Xml.Xsl.Runtime {

    /// <summary>
    /// This class manages nodes from one input document, indexed by key value(s).
    /// </summary>
    [EditorBrowsable(EditorBrowsableState.Never)]
    public sealed class XmlILIndex {
        private Hashtable table;

        /// <summary>
        /// This constructor is internal so that external users cannot construct it (and therefore we do not have to test it separately).
        /// </summary>
        internal XmlILIndex() {
            this.table = new Hashtable();
        }

        /// <summary>
        /// Add a node indexed by the specified key value.
        /// </summary>
        public void Add(string key, XPathNavigator navigator) {
            XmlQueryNodeSequence seq = (XmlQueryNodeSequence) this.table[key];

            if (seq == null) {
                // Create a new sequence and add it to the index
                seq = new XmlQueryNodeSequence();
                seq.AddClone(navigator);
                this.table.Add(key, seq);
            }
            else {
                // Add node to existing sequence; don't add if it already there
                for (int i = 0; i < seq.Count; i++) {
                    if (navigator.IsSamePosition(seq[i]))
                        return;
                }

                seq.AddClone(navigator);
            }
        }

        /// <summary>
        /// Lookup a sequence of nodes that are indexed by the specified key value.
        /// Return a non-null empty sequence, if there are no nodes associated with the key.
        /// </summary>
        public XmlQueryNodeSequence Lookup(string key) {
            XmlQueryNodeSequence seq = (XmlQueryNodeSequence) this.table[key];

            if (seq == null)
                seq = new XmlQueryNodeSequence();

            return seq;
        }
    }
}
