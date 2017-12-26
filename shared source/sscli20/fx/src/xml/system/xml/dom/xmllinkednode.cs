//------------------------------------------------------------------------------
// <copyright file="XmlLinkedNode.cs" company="Microsoft">
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

    // Gets the node immediately preceeding or following this node.
    public abstract class XmlLinkedNode: XmlNode {
        internal XmlLinkedNode next;

        internal XmlLinkedNode(): base() {
            next = null;
        }
        internal XmlLinkedNode( XmlDocument doc ): base( doc ) {
            next = null;
        }

        // Gets the node immediately preceding this node.
        public override XmlNode PreviousSibling {
            get {
                XmlNode parent = ParentNode;
                if (parent != null) {
                    XmlNode node = parent.FirstChild;
                    while (node != null) {
                        XmlNode nextSibling = node.NextSibling; 
                        if (nextSibling == this) {
                            break;
                        }
                        node = nextSibling;
                    }
                    return node;
                }
                return null;
            }
        }

        // Gets the node immediately following this node.
        public override XmlNode NextSibling {
            get {
                XmlNode parent = ParentNode;
                if (parent != null) {
                    if (next != parent.FirstChild)
                        return next;
                }
                return null;
            }
        }
    }
}
