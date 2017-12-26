//------------------------------------------------------------------------------
// <copyright file="XmlIterators.cs" company="Microsoft">
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
using System.Xml;
using System.Xml.XPath;
using System.ComponentModel;

namespace System.Xml.Xsl.Runtime {

    /// <summary>
    /// Iterators that use containment to control a nested iterator return one of the following values from MoveNext().
    /// </summary>
    [EditorBrowsable(EditorBrowsableState.Never)]
    public enum IteratorResult {
        NoMoreNodes,                // Iteration is complete; there are no more nodes
        NeedInputNode,              // The next node needs to be fetched from the contained iterator before iteration can continue
        HaveCurrentNode,            // This iterator's Current property is set to the next node in the iteration
    };


    /// <summary>
    /// Tokenize a string containing IDREF values and deref the values in order to get a list of ID elements.
    /// </summary>
    [EditorBrowsable(EditorBrowsableState.Never)]
    public struct IdIterator {
        private XPathNavigator navCurrent;
        private string[] idrefs;
        private int idx;

        public void Create(XPathNavigator context, string value) {
            this.navCurrent = XmlQueryRuntime.SyncToNavigator(this.navCurrent, context);
            this.idrefs = XmlConvert.SplitString(value);
            this.idx = -1;
        }

        public bool MoveNext() {
            do {
                this.idx++;
                if (this.idx >= idrefs.Length)
                    return false;
            }
            while (!this.navCurrent.MoveToId(this.idrefs[this.idx]));

            return true;
        }

        /// <summary>
        /// Return the current result navigator.  This is only defined after MoveNext() has returned true.
        /// </summary>
        public XPathNavigator Current {
            get { return this.navCurrent; }
        }
    }
}
