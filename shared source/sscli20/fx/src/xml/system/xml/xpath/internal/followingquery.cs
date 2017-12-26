//------------------------------------------------------------------------------
// <copyright file="followingquery.cs" company="Microsoft">
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

    internal sealed class FollowingQuery : BaseAxisQuery {
        private XPathNavigator input;
        private XPathNodeIterator iterator;

        public FollowingQuery(Query qyInput, string name, string prefix, XPathNodeType typeTest) : base(qyInput, name, prefix, typeTest) {}
        private FollowingQuery(FollowingQuery other) : base(other) {
            this.input    = Clone(other.input);
            this.iterator = Clone(other.iterator);
        }

        public override void Reset() {
            iterator = null;
            base.Reset();
        }

        public override XPathNavigator Advance() {
            if (iterator == null) {
                input = qyInput.Advance();
                if (input == null) {
                    return null;
                }

                XPathNavigator prev;
                do {
                    prev = input.Clone();
                    input = qyInput.Advance();
                } while (prev.IsDescendant(input));
                input = prev;

                iterator = XPathEmptyIterator.Instance;
            }
 
            while (! iterator.MoveNext()) {
                bool matchSelf;
                if (input.NodeType == XPathNodeType.Attribute || input.NodeType == XPathNodeType.Namespace) {
                    input.MoveToParent();
                    matchSelf = false;
                } else {
                    while (! input.MoveToNext()) {
                        if (! input.MoveToParent()) {
                            return null;
                        }
                    }
                    matchSelf = true;
                }
                if (NameTest) {
                    iterator = input.SelectDescendants(Name, Namespace, matchSelf);
                } else {
                    iterator = input.SelectDescendants(TypeTest, matchSelf);
                }
            }
            position++;
            currentNode = iterator.Current;
            return currentNode;
        }

        public override XPathNodeIterator Clone() { return new FollowingQuery(this); }
    }
}
