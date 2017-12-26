//------------------------------------------------------------------------------
// <copyright file="XmlNamedNodeMap.cs" company="Microsoft">
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

    // Represents a collection of nodes that can be accessed by name or index.
    public class XmlNamedNodeMap : IEnumerable {
        internal XmlNode parent;
        internal ArrayList nodes;

        internal XmlNamedNodeMap( XmlNode parent ) {
            this.parent = parent;
            this.nodes = null;
        }

        // Retrieves a XmlNode specified by name.
        public virtual XmlNode GetNamedItem(String name) {
            int offset = FindNodeOffset(name);
            if (offset >= 0)
                return(XmlNode) Nodes[offset];
            return null;
        }

        // Adds a XmlNode using its Name property
        public virtual XmlNode SetNamedItem(XmlNode node) {
            if ( node == null )
                return null;

            int offset = FindNodeOffset( node.LocalName, node.NamespaceURI );
            if (offset == -1) {
                AddNode( node );
                return null;
            }
            else {
                return ReplaceNodeAt( offset, node );
            }
        }

        // Removes the node specified by name.
        public virtual XmlNode RemoveNamedItem(String name) {
            int offset = FindNodeOffset(name);
            if (offset >= 0) {
                return RemoveNodeAt( offset );
            }
            return null;
        }

        // Gets the number of nodes in this XmlNamedNodeMap.
        public virtual int Count {
            get {
                if (nodes != null)
                    return nodes.Count;
                return 0;
            }
        }

        // Retrieves the node at the specified index in this XmlNamedNodeMap.
        public virtual XmlNode Item(int index) {
            if (index < 0 || index >= Nodes.Count)
                return null;
            try {
                return(XmlNode) Nodes[index];
            } catch ( ArgumentOutOfRangeException ) {
                throw new IndexOutOfRangeException(Res.GetString(Res.Xdom_IndexOutOfRange));
            }
        }

        //
        // DOM Level 2
        //

        // Retrieves a node specified by LocalName and NamespaceURI.
        public virtual XmlNode GetNamedItem(String localName, String namespaceURI) {
            int offset = FindNodeOffset( localName, namespaceURI );
            if (offset >= 0)
                return(XmlNode) Nodes[offset];
            return null;
        }

        // Removes a node specified by local name and namespace URI.
        public virtual XmlNode RemoveNamedItem(String localName, String namespaceURI) {
            int offset = FindNodeOffset( localName, namespaceURI );
            if (offset >= 0) {
                return RemoveNodeAt( offset );
            }
            return null;
        }

        internal ArrayList Nodes {
            get {
                if (nodes == null)
                    nodes = new ArrayList();

                return nodes;
            }
        }

        public virtual IEnumerator GetEnumerator() {
            if ( nodes == null ) {
                return XmlDocument.EmptyEnumerator;
            }
            else {
                return Nodes.GetEnumerator();
            }
        }

        internal int FindNodeOffset( string name ) {
            int c = this.Count;
            for (int i = 0; i < c; i++) {
                XmlNode node = (XmlNode) Nodes[i];

                if (name == node.Name)
                    return i;
            }

            return -1;
        }

        internal int FindNodeOffset( string localName, string namespaceURI ) {
            int c = this.Count;
            for (int i = 0; i < c; i++) {
                XmlNode node = (XmlNode) Nodes[i];

                if (node.LocalName == localName && node.NamespaceURI == namespaceURI)
                    return i;
            }

            return -1;
        }

        internal virtual XmlNode AddNode( XmlNode node ) {
            XmlNode oldParent;
            if ( node.NodeType == XmlNodeType.Attribute )
                oldParent = ((XmlAttribute)node).OwnerElement;
            else
                oldParent = node.ParentNode;
            string nodeValue = node.Value;
            XmlNodeChangedEventArgs args = parent.GetEventArgs( node, oldParent, parent, nodeValue, nodeValue, XmlNodeChangedAction.Insert );

            if (args != null)
                parent.BeforeEvent( args );

            Nodes.Add( node );
            node.SetParent( parent );

            if (args != null)
                parent.AfterEvent( args );

            return node;
        }

        internal virtual XmlNode AddNodeForLoad(XmlNode node, XmlDocument doc) {
            XmlNodeChangedEventArgs args = doc.GetInsertEventArgsForLoad(node, parent);
            if (args != null) {
                doc.BeforeEvent(args);
            }
            Nodes.Add(node);
            node.SetParent(parent);
            if (args != null) {
                doc.AfterEvent(args);
            }
            return node;
        }

        internal virtual XmlNode RemoveNodeAt( int i ) {
            XmlNode oldNode = (XmlNode)Nodes[i];

            string oldNodeValue = oldNode.Value;
            XmlNodeChangedEventArgs args = parent.GetEventArgs( oldNode, parent, null, oldNodeValue, oldNodeValue, XmlNodeChangedAction.Remove );

            if (args != null)
                parent.BeforeEvent( args );

            Nodes.RemoveAt(i);
            oldNode.SetParent( null );

            if (args != null)
                parent.AfterEvent( args );

            return oldNode;
        }

        internal XmlNode ReplaceNodeAt( int i, XmlNode node ) {
            XmlNode oldNode = RemoveNodeAt( i );
            InsertNodeAt( i, node );
            return oldNode;
        }

        internal virtual XmlNode InsertNodeAt( int i, XmlNode node ) {
            XmlNode oldParent;
            if ( node.NodeType == XmlNodeType.Attribute )
                oldParent = ((XmlAttribute)node).OwnerElement;
            else
                oldParent = node.ParentNode;

            string nodeValue = node.Value;
            XmlNodeChangedEventArgs args = parent.GetEventArgs( node, oldParent, parent, nodeValue, nodeValue, XmlNodeChangedAction.Insert );

            if (args != null)
                parent.BeforeEvent( args );

            Nodes.Insert( i, node );
            node.SetParent( parent );

            if (args != null)
                parent.AfterEvent( args );

            return node;
        }
    }
}
