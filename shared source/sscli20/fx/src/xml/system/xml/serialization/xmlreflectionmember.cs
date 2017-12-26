//------------------------------------------------------------------------------
// <copyright file="XmlReflectionMember.cs" company="Microsoft">
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

    /// <include file='doc\XmlReflectionMember.uex' path='docs/doc[@for="XmlReflectionMember"]/*' />
    ///<internalonly/>
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlReflectionMember {
        string memberName;
        Type type;
        XmlAttributes xmlAttributes = new XmlAttributes();
        SoapAttributes soapAttributes = new SoapAttributes();
        bool isReturnValue;
        bool overrideIsNullable;

        /// <include file='doc\XmlReflectionMember.uex' path='docs/doc[@for="XmlReflectionMember.MemberType"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Type MemberType {
            get { return type; }
            set { type = value; }
        }

        /// <include file='doc\XmlReflectionMember.uex' path='docs/doc[@for="XmlReflectionMember.XmlAttributes"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlAttributes XmlAttributes {
            get { return xmlAttributes; }
            set { xmlAttributes = value; }
        }

        /// <include file='doc\XmlReflectionMember.uex' path='docs/doc[@for="XmlReflectionMember.SoapAttributes"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public SoapAttributes SoapAttributes {
            get { return soapAttributes; }
            set { soapAttributes = value; }
        }

        /// <include file='doc\XmlReflectionMember.uex' path='docs/doc[@for="XmlReflectionMember.MemberName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string MemberName {
            get { return memberName == null ? string.Empty : memberName; }
            set { memberName = value; }
        }

        /// <include file='doc\XmlReflectionMember.uex' path='docs/doc[@for="XmlReflectionMember.IsReturnValue"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool IsReturnValue {
            get { return isReturnValue; }
            set { isReturnValue = value; }
        }

        /// <include file='doc\XmlReflectionMember.uex' path='docs/doc[@for="XmlReflectionMember.OverrideIsNullable"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool OverrideIsNullable {
            get { return overrideIsNullable; }
            set { overrideIsNullable = value; }
        }
    }


}
