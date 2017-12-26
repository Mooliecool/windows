//------------------------------------------------------------------------------
// <copyright file="ValueQuery.cs" company="Microsoft">
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
    using System.Globalization;
    using System.Text;
    using System.Xml;
    using System.Xml.XPath;
    using System.Xml.Xsl;
    using System.Collections.Generic;
    using System.Diagnostics;

    [DebuggerDisplay("{this}")]
    internal abstract class ValueQuery : Query {
        public    ValueQuery() { }
        protected ValueQuery(ValueQuery other) : base(other) { }
        public sealed override void Reset() { }
        public sealed override XPathNavigator Current { get { throw XPathException.Create(Res.Xp_NodeSetExpected); } }
        public sealed override int CurrentPosition { get { throw XPathException.Create(Res.Xp_NodeSetExpected); } }
        public sealed override int Count { get { throw XPathException.Create(Res.Xp_NodeSetExpected); } }
        public sealed override XPathNavigator Advance() { throw XPathException.Create(Res.Xp_NodeSetExpected); }
    }
}
