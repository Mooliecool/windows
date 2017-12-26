//------------------------------------------------------------------------------
// <copyright file="XPathChildIterator.cs" company="Microsoft">
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

    internal class XPathChildIterator: XPathAxisIterator {
        public XPathChildIterator(XPathNavigator nav, XPathNodeType type) : base(nav, type, /*matchSelf:*/false) {}
        public XPathChildIterator(XPathNavigator nav, string name, string namespaceURI) : base(nav, name, namespaceURI, /*matchSelf:*/false) {}
        public XPathChildIterator(XPathChildIterator it) : base(it) {}

        public override XPathNodeIterator Clone() {
            return new XPathChildIterator(this);
        }

        public override bool MoveNext() {
            while ((first) ? nav.MoveToFirstChild() : nav.MoveToNext()) {
                first = false;
                if (Matches) {
                    position++;
                    return true;
                }
            }
            return false;
        }
    }    
}
