//------------------------------------------------------------------------------
// <copyright file="AttributeQuery.cs" company="Microsoft">
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

namespace MS.Internal.Xml.XPath {
    using System;
    using System.Xml;
    using System.Xml.XPath;
    using System.Diagnostics;
    using System.Globalization;

    internal sealed class AttributeQuery : BaseAxisQuery {
        private bool onAttribute = false;

        public AttributeQuery(Query qyParent, string Name, string Prefix, XPathNodeType Type) : base(qyParent, Name, Prefix, Type) {}
        private AttributeQuery(AttributeQuery other) : base(other) {
            this.onAttribute = other.onAttribute;
        }
        public override void Reset() {
            onAttribute = false;
            base.Reset();
        }

        public override XPathNavigator Advance() {
            while (true) {
                if (! onAttribute) {
                    currentNode = qyInput.Advance();
                    if (currentNode == null) {
                        return null;
                    }
                    position = 0;
                    currentNode = currentNode.Clone();
                    onAttribute = currentNode.MoveToFirstAttribute();
                } else {
                    onAttribute = currentNode.MoveToNextAttribute();
                }

                if (onAttribute) {
                    Debug.Assert(! currentNode.NamespaceURI.Equals(XmlReservedNs.NsXmlNs));
                    if (matches(currentNode)) {
                        position++;
                        return currentNode;
                    }
                }
            } // while
        }

        public override XPathNavigator MatchNode(XPathNavigator context) {
            if (context != null) {
                if (context.NodeType == XPathNodeType.Attribute && matches(context)) {
                    XPathNavigator temp = context.Clone();
                    if (temp.MoveToParent()) {
                        return qyInput.MatchNode(temp);
                    }
                }
            }
            return null;
        }

        public override XPathNodeIterator Clone() { return new AttributeQuery(this); }
    }
}
