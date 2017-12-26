//------------------------------------------------------------------------------
// <copyright file="XmlChildNodes.cs" company="Microsoft">
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
namespace System.Xml {
    using System.Collections;

    internal class XmlChildNodes: XmlNodeList {
        private XmlNode container;

        public XmlChildNodes( XmlNode container ) {
            this.container = container;
        }

        public override XmlNode Item( int i ) {
            // Out of range indexes return a null XmlNode
            if (i < 0)
                return null;
            for (XmlNode n = container.FirstChild; n != null; n = n.NextSibling, i--) {
                if (i == 0)
                    return n;
            }
            return null;
        }

        public override int Count {
            get {
                int c = 0;
                for (XmlNode n = container.FirstChild; n != null; n = n.NextSibling) {
                    c++;
                }
                return c;
            }
        }

        public override IEnumerator GetEnumerator() {
            if ( container.FirstChild == null ) {
                return XmlDocument.EmptyEnumerator;
            }
            else {
                return new XmlChildEnumerator( container );
            }
        }
    }
}
