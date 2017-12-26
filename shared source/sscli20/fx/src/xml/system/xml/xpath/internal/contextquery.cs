//------------------------------------------------------------------------------
// <copyright file="ContextQuery.cs" company="Microsoft">
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

    internal class ContextQuery : Query {
        protected XPathNavigator contextNode;
        
        public ContextQuery() {
            this.count = 0;
        }
        protected ContextQuery(ContextQuery other) : base(other) {
            this.contextNode = other.contextNode;   // Don't need to clone here
        }
        public override void Reset() {
            count = 0;
        }

        public override XPathNavigator Current { get { return contextNode; } }

        public override object Evaluate(XPathNodeIterator context) {
            contextNode = context.Current; // We don't clone here. Because we never move it.
            count = 0;
            return this; 
        }

        public override XPathNavigator Advance() {
            if (count == 0) {
                count = 1;
                return contextNode;
            }
            return null;
        }

        public override XPathNavigator MatchNode(XPathNavigator current) {
            return current;
        }

        public override XPathNodeIterator Clone() { return new ContextQuery(this); }

        public override XPathResultType StaticType { get { return XPathResultType.NodeSet; } }
        public override int CurrentPosition   { get { return count; } }
        public override int Count             { get { return 1; } }
        public override QueryProps Properties { get { return QueryProps.Merge | QueryProps.Cached | QueryProps.Position | QueryProps.Count; } }
    }
}
