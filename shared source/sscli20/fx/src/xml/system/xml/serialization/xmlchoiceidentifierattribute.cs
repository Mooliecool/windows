//------------------------------------------------------------------------------
// <copyright file="XmlChoiceIdentifierAttribute.cs" company="Microsoft">
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
    using System.Xml.Schema;

    /// <include file='doc\XmlChoiceIdentifierAttribute.uex' path='docs/doc[@for="XmlChoiceIdentifierAttribute"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Parameter | AttributeTargets.ReturnValue, AllowMultiple=false)]
    public class XmlChoiceIdentifierAttribute : System.Attribute {
        string name;

        /// <include file='doc\XmlChoiceIdentifierAttribute.uex' path='docs/doc[@for="XmlChoiceIdentifierAttribute.XmlChoiceIdentifierAttribute"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlChoiceIdentifierAttribute() {
        }
        
        /// <include file='doc\XmlChoiceIdentifierAttribute.uex' path='docs/doc[@for="XmlChoiceIdentifierAttribute.XmlChoiceIdentifierAttribute1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlChoiceIdentifierAttribute(string name) {
            this.name = name;
        }

        /// <include file='doc\XmlChoiceIdentifierAttribute.uex' path='docs/doc[@for="XmlChoiceIdentifierAttribute.Name"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string MemberName {
            get { return name == null ? string.Empty : name; }
            set { name = value; }
        }
    }
}
