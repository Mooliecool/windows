//------------------------------------------------------------------------------
// <copyright file="ChildrenQuery.cs" company="Microsoft">
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
    using System.Xml.Xsl;
    using System.Collections;

    internal  class ChildrenQuery : BaseAxisQuery {
        XPathNodeIterator iterator = XPathEmptyIterator.Instance;
        
        public ChildrenQuery(Query qyInput, string name, string prefix, XPathNodeType type) : base (qyInput, name, prefix, type) {}
        protected ChildrenQuery(ChildrenQuery other) : base(other) {
            this.iterator = Clone(other.iterator);
        }

        public override void Reset() {
            iterator = XPathEmptyIterator.Instance;
            base.Reset();
        }
        
        public override XPathNavigator Advance() {
            while (!iterator.MoveNext()) {
                XPathNavigator input = qyInput.Advance();
                if (input == null) {
                    return null;
                }
                if (NameTest) {
                    if (TypeTest == XPathNodeType.ProcessingInstruction) {
                        iterator = new IteratorFilter(input.SelectChildren(TypeTest), Name);
                    } else {
                        iterator = input.SelectChildren(Name, Namespace);
                    }
                } else {
                    iterator = input.SelectChildren(TypeTest);
                }
                position = 0;
            }
            position ++;
            currentNode = iterator.Current;
            return currentNode;
        } // Advance

        public sealed override XPathNavigator MatchNode(XPathNavigator context) {
            if (context != null) {
                if (matches(context)) {
                    XPathNavigator temp = context.Clone();
                    if (temp.NodeType != XPathNodeType.Attribute && temp.MoveToParent()) {
                        return qyInput.MatchNode(temp);
                    }
                    return null;
                }
            }
            return null;
        }

        public override XPathNodeIterator Clone() { return new ChildrenQuery(this); }
    }
}
