//------------------------------------------------------------------------------
// <copyright file="DescendantQuery.cs" company="Microsoft">
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

    internal class DescendantQuery : DescendantBaseQuery {
        XPathNodeIterator nodeIterator;

        internal DescendantQuery(Query  qyParent, string Name, string Prefix, XPathNodeType Type, bool matchSelf, bool abbrAxis)
            : base(qyParent, Name, Prefix, Type, matchSelf, abbrAxis) {}

        public DescendantQuery(DescendantQuery other) : base(other) {
            this.nodeIterator = Clone(other.nodeIterator);
        }

        public override void Reset() {
            nodeIterator = null;
            base.Reset();
        }

        public override XPathNavigator Advance() {
            while (true) {
                if (nodeIterator == null) {
                    position = 0;
                    XPathNavigator nav = qyInput.Advance();
                    if (nav == null) {
                        return null;
                    }
                    if (NameTest) {
                        if (TypeTest == XPathNodeType.ProcessingInstruction) {
                            nodeIterator = new IteratorFilter(nav.SelectDescendants(TypeTest, matchSelf), Name);
                        } else {
                            nodeIterator = nav.SelectDescendants(Name, Namespace, matchSelf);
                        }
                    } else {
                        nodeIterator = nav.SelectDescendants(TypeTest, matchSelf);
                    }
                }

                if (nodeIterator.MoveNext()) {
                    position++;
                    currentNode = nodeIterator.Current;
                    return currentNode;
                } else {
                    nodeIterator = null;
                }
            }
        }
                    
        public override XPathNodeIterator Clone() { return new DescendantQuery(this); }         
    }
}
