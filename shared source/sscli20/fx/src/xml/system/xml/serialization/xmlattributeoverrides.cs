//------------------------------------------------------------------------------
// <copyright file="XmlAttributeOverrides.cs" company="Microsoft">
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
// <owner current="true" primary="true">ElenaK</owner>                                                                
//------------------------------------------------------------------------------

namespace System.Xml.Serialization {
    using System.Reflection;
    using System.Collections;
    using System.IO;
    using System.Xml.Schema;
    using System;
    using System.ComponentModel;

    /// <include file='doc\XmlAttributeOverrides.uex' path='docs/doc[@for="XmlAttributeOverrides"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlAttributeOverrides {
        Hashtable types = new Hashtable();

        /// <include file='doc\XmlAttributeOverrides.uex' path='docs/doc[@for="XmlAttributeOverrides.Add"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Add(Type type, XmlAttributes attributes) {
            Add(type, string.Empty, attributes);
        }

        /// <include file='doc\XmlAttributeOverrides.uex' path='docs/doc[@for="XmlAttributeOverrides.Add1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void Add(Type type, string member, XmlAttributes attributes) {
            Hashtable members = (Hashtable)types[type];
            if (members == null) {
                members = new Hashtable();
                types.Add(type, members);
            }
            else if (members[member] != null) {
                throw new InvalidOperationException(Res.GetString(Res.XmlAttributeSetAgain, type.FullName, member));
            }
            members.Add(member, attributes);
        }

        /// <include file='doc\XmlAttributeOverrides.uex' path='docs/doc[@for="XmlAttributeOverrides.this"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlAttributes this[Type type] {
            get {
                return this[type, string.Empty];
            }
        }

        /// <include file='doc\XmlAttributeOverrides.uex' path='docs/doc[@for="XmlAttributeOverrides.this1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlAttributes this[Type type, string member] {
            get {
                Hashtable members = (Hashtable)types[type];
                if (members == null) return null;
                return (XmlAttributes)members[member];
            }
        }
    }
}

