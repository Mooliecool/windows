//------------------------------------------------------------------------------
// <copyright file="XmlQualifiedNameTest.cs" company="Microsoft">
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

using System;
using System.Xml;
using System.Xml.Schema;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

namespace System.Xml.Xsl {

    /// <summary>
    /// XmlQualifiedName extends XmlQualifiedName to support wildcards and adds nametest functionality
    /// Following are the examples:
    ///     {A}:B     XmlQualifiedNameTest.New("B", "A")        Match QName with namespace A        and local name B
    ///     *         XmlQualifiedNameTest.New(null, null)      Match any QName
    ///     {A}:*     XmlQualifiedNameTest.New(null, "A")       Match QName with namespace A        and any local name
    ///               XmlQualifiedNameTest.New("A", false)
    ///     *:B       XmlQualifiedNameTest.New("B", null)       Match QName with any namespace      and local name B
    ///     ~{A}:*    XmlQualifiedNameTest.New("B", "A")        Match QName with namespace not A    and any local name
    ///     {~A}:B    only as a result of the intersection      Match QName with namespace not A    and local name B
    /// </summary>
    internal class XmlQualifiedNameTest : XmlQualifiedName {
        bool exclude;
        const string wildcard = "*";
        static XmlQualifiedNameTest wc = XmlQualifiedNameTest.New(wildcard, wildcard);

        /// <summary>
        /// Full wildcard
        /// </summary>
        public static XmlQualifiedNameTest Wildcard {
            get { return wc; }
        }

        /// <summary>
        /// Constructor
        /// </summary>
        private XmlQualifiedNameTest(string name, string ns, bool exclude) : base (name, ns) {
            this.exclude = exclude;
        }

        /// <summary>
        /// Construct new from name and namespace. Returns singleton Wildcard in case full wildcard
        /// </summary>
        public static XmlQualifiedNameTest New(string name, string ns) {
            if (ns == null && name == null) {
                return Wildcard;
            }
            else {
                return new XmlQualifiedNameTest(name == null ? wildcard : name, ns == null ? wildcard : ns, false);
            }
        }

        /// <summary>
        /// True if matches any name and any namespace
        /// </summary>
        public bool IsWildcard {
            get { return (object)this == (object)Wildcard; }
        }

        /// <summary>
        /// True if matches any name
        /// </summary>
        public bool IsNameWildcard {
            get { return (object)this.Name == (object)wildcard; }
        }

        /// <summary>
        /// True if matches any namespace
        /// </summary>
        public bool IsNamespaceWildcard {
            get { return (object)this.Namespace == (object)wildcard; }
        }

        private bool IsNameSubsetOf(XmlQualifiedNameTest other) {
            return other.IsNameWildcard || this.Name == other.Name;
        }

        private bool IsNamespaceSubsetOf(XmlQualifiedNameTest other) {
            return other.IsNamespaceWildcard
                || (this.exclude == other.exclude && this.Namespace == other.Namespace)
                || (other.exclude && !this.exclude && this.Namespace != other.Namespace);
        }

        /// <summary>
        /// True if this matches every QName other does
        /// </summary>
        public bool IsSubsetOf(XmlQualifiedNameTest other) {
            return IsNameSubsetOf(other) && IsNamespaceSubsetOf(other);
        }

        /// <summary>
        /// Return true if the result of intersection with other is not empty
        /// </summary>
        public bool HasIntersection(XmlQualifiedNameTest other) {
            return (IsNamespaceSubsetOf(other) || other.IsNamespaceSubsetOf(this)) && (IsNameSubsetOf(other) || other.IsNameSubsetOf(this));
        }

        /// <summary>
        /// String representation
        /// </summary>
        public override string ToString() {
            if ((object)this == (object)Wildcard) {
                return "*";
            }
            else {
                if (this.Namespace.Length == 0) {
                    return this.Name;
                }
                else if ((object)this.Namespace == (object)wildcard) {
                    return "*:" + this.Name;
                }
                else if (this.exclude) {
                    return  "{~" + this.Namespace + "}:" + this.Name;
                }
                else {
                    return  "{" + this.Namespace + "}:" + this.Name;
                }
            }
        }

    }
}
