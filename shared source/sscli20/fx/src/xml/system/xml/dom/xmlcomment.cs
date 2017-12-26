//------------------------------------------------------------------------------
// <copyright file="XmlComment.cs" company="Microsoft">
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
    using System.Xml.XPath;
    using System.Diagnostics;

    // Represents the content of an XML comment.
    public class XmlComment: XmlCharacterData {
        protected internal XmlComment( string comment, XmlDocument doc ): base( comment, doc ) {
        }

        // Gets the name of the node.
        public override String Name {
            get { return OwnerDocument.strCommentName;}
        }

        // Gets the name of the current node without the namespace prefix.
        public override String LocalName {
            get { return OwnerDocument.strCommentName;}
        }

        // Gets the type of the current node.
        public override XmlNodeType NodeType {
            get { return XmlNodeType.Comment;}
        }

        // Creates a duplicate of this node.
        public override XmlNode CloneNode(bool deep) {
            Debug.Assert( OwnerDocument != null );
            return OwnerDocument.CreateComment( Data );
        }

        // Saves the node to the specified XmlWriter.
        public override void WriteTo(XmlWriter w) {
            w.WriteComment( Data );
        }

        // Saves all the children of the node to the specified XmlWriter.
        public override void WriteContentTo(XmlWriter w) {
            // Intentionally do nothing
        }

        internal override XPathNodeType XPNodeType { get { return XPathNodeType.Comment; } }
    }
}

