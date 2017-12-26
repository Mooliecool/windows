//------------------------------------------------------------------------------
// <copyright file="XmlNavigatorReader.cs" company="Microsoft">
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

using System.IO;
using System.Xml.Schema;
using System.Collections;
using System.Diagnostics;
using MS.Internal.Xml.Cache;

namespace System.Xml.XPath {
    internal class XPathNavigatorKeyComparer : IEqualityComparer {
        bool IEqualityComparer.Equals( Object obj1, Object obj2 ) {
            XPathNavigator nav1 = obj1 as XPathNavigator;
            XPathNavigator nav2 = obj2 as XPathNavigator;
            if( ( nav1 != null ) && ( nav2 != null ) ) {
                if( nav1.IsSamePosition( nav2 ) )
                    return true;
            }
            return false;
        }

        int IEqualityComparer.GetHashCode ( Object obj ) {
            int hashCode;
            XPathNavigator nav;
            XPathDocumentNavigator xpdocNav;

            if (obj == null) {
                throw new ArgumentNullException("obj");
            }
            else if ( null != (xpdocNav = obj as XPathDocumentNavigator) ) {
                hashCode = xpdocNav.GetPositionHashCode();
            }
            else if( null != (nav = obj as XPathNavigator) ) {
                Object underlyingObject = nav.UnderlyingObject;
                if (underlyingObject != null) {
                    hashCode = underlyingObject.GetHashCode();
                }
                else {
                    hashCode = (int)nav.NodeType;
                    hashCode ^= nav.LocalName.GetHashCode();
                    hashCode ^= nav.Prefix.GetHashCode();
                    hashCode ^= nav.NamespaceURI.GetHashCode();
                }
            } 
            else {
                hashCode = obj.GetHashCode();
            }
            return hashCode;
        }
    }
}
