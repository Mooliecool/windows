//------------------------------------------------------------------------------
// <copyright file="XPathSingletonIterator.cs" company="Microsoft">
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

    internal class XPathSingletonIterator: ResetableIterator {
        private XPathNavigator nav;
        private int position;

        public XPathSingletonIterator(XPathNavigator nav) {
            Debug.Assert(nav != null);
            this.nav = nav;
        }

        public XPathSingletonIterator(XPathNavigator nav, bool moved) : this(nav) {
            if (moved) {
                position = 1;
            }
        }

        public XPathSingletonIterator(XPathSingletonIterator it) {
            this.nav      = it.nav.Clone();
            this.position = it.position;
        }

        public override XPathNodeIterator Clone() {
            return new XPathSingletonIterator(this);
        }

        public override XPathNavigator Current {
            get { return nav; }
        }

        public override int CurrentPosition {
            get { return position; }
        }

        public override int Count {
            get { return 1; }
        }

        public override bool MoveNext() {
            if(position == 0) {
                position = 1;
                return true;
            }
            return false;
        }

        public override void Reset() {
            position = 0;
        }
    }
}
