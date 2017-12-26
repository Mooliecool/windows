//------------------------------------------------------------------------------
// <copyright file="EmptyQuery.cs" company="Microsoft">
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

    internal sealed class EmptyQuery : Query {
        public override XPathNavigator Advance() { return null; }
        public override XPathNodeIterator Clone() { return this; }
        public override object Evaluate(XPathNodeIterator context) { return this; }
        public override int CurrentPosition { get { return 0; } }
        public override int Count { get { return 0; } }
        public override QueryProps Properties { get { return QueryProps.Merge | QueryProps.Cached | QueryProps.Position | QueryProps.Count; } }
        public override XPathResultType StaticType { get { return XPathResultType.NodeSet; } }
        public override void Reset() { }
        public override XPathNavigator Current { get { return null; } }
    }
}
