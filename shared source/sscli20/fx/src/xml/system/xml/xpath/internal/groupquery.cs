//------------------------------------------------------------------------------
// <copyright file="GroupQuery.cs" company="Microsoft">
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

    internal sealed class GroupQuery : BaseAxisQuery {

        public GroupQuery(Query qy): base(qy) {}
        private GroupQuery(GroupQuery other) : base(other) { }

        public override XPathNavigator Advance() {
            currentNode = qyInput.Advance();
            if (currentNode != null) {
                position++;
            }
            return currentNode;
        }

        public override object Evaluate(XPathNodeIterator nodeIterator) {
            return qyInput.Evaluate(nodeIterator);
        }

        public override XPathNodeIterator Clone() { return new GroupQuery(this); }
        public override XPathResultType StaticType { get { return qyInput.StaticType; } }
        public override QueryProps Properties { get { return QueryProps.Position; } } // Doesn't have QueryProps.Merge
    }
}
