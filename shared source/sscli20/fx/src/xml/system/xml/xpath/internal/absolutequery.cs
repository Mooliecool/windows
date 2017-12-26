//------------------------------------------------------------------------------
// <copyright file="AbsoluteQuery.cs" company="Microsoft">
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

    internal sealed class AbsoluteQuery : ContextQuery {
        public  AbsoluteQuery()                    : base() {}
        private AbsoluteQuery(AbsoluteQuery other) : base(other) {}

        public override object Evaluate(XPathNodeIterator context) {
            base.contextNode = context.Current.Clone();
            base.contextNode.MoveToRoot();
            count = 0;
            return this; 
        }

        public override XPathNavigator MatchNode(XPathNavigator context) {
            if (context != null && context.NodeType == XPathNodeType.Root) {
                return context;
            }
            return null;
        }

        public override XPathNodeIterator Clone() { return new AbsoluteQuery(this); }
    }
}
