//------------------------------------------------------------------------------
// <copyright file="XPathSelfQuery.cs" company="Microsoft">
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

    internal sealed class XPathSelfQuery : BaseAxisQuery {
        public XPathSelfQuery(Query qyInput, string Name, string Prefix, XPathNodeType Type) : base(qyInput, Name, Prefix, Type) {}
        private XPathSelfQuery(XPathSelfQuery other) : base(other) { }
                
        public override XPathNavigator Advance() {
            while ((currentNode = qyInput.Advance()) != null) {
                if (matches(currentNode)) {
                    position = 1;
                    return currentNode;
                }
            }
            return null;
        }
                
        public override XPathNodeIterator Clone() { return new XPathSelfQuery(this); }
    }
}
