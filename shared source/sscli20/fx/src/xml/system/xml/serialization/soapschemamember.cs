//------------------------------------------------------------------------------
// <copyright file="SoapSchemaMember.cs" company="Microsoft">
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

    using System;

    /// <include file='doc\SoapSchemaMember.uex' path='docs/doc[@for="SoapSchemaMember"]/*' />
    /// <internalonly/>
    public class SoapSchemaMember {
        string memberName;
        XmlQualifiedName type = XmlQualifiedName.Empty;

        /// <include file='doc\SoapSchemaMember.uex' path='docs/doc[@for="SoapSchemaMember.MemberType"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlQualifiedName MemberType {
            get { return type; }
            set { type = value; }
        }

        /// <include file='doc\SoapSchemaMember.uex' path='docs/doc[@for="SoapSchemaMember.MemberName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string MemberName {
            get { return memberName == null ? string.Empty : memberName; }
            set { memberName = value; }
        }
    }


}
