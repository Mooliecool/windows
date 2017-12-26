//------------------------------------------------------------------------------
// <copyright file="SoapAttributeAttribute.cs" company="Microsoft">
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

    /// <include file='doc\SoapAttributeAttribute.uex' path='docs/doc[@for="SoapAttributeAttribute"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Parameter | AttributeTargets.ReturnValue)]
    public class SoapAttributeAttribute : System.Attribute {
        string attributeName;
        string ns;
        string dataType;
        
        /// <include file='doc\SoapAttributeAttribute.uex' path='docs/doc[@for="SoapAttributeAttribute.SoapAttributeAttribute"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public SoapAttributeAttribute() {
        }
        
        /// <include file='doc\SoapAttributeAttribute.uex' path='docs/doc[@for="SoapAttributeAttribute.SoapAttributeAttribute1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public SoapAttributeAttribute(string attributeName) {
            this.attributeName = attributeName;
        }

        /// <include file='doc\SoapAttributeAttribute.uex' path='docs/doc[@for="SoapAttributeAttribute.ElementName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string AttributeName {
            get { return attributeName == null ? string.Empty : attributeName; }
            set { attributeName = value; }
        }

        /// <include file='doc\SoapAttributeAttribute.uex' path='docs/doc[@for="SoapAttributeAttribute.Namespace"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string Namespace {
            get { return ns; }
            set { ns = value; }
        }

        /// <include file='doc\SoapAttributeAttribute.uex' path='docs/doc[@for="SoapAttributeAttribute.DataType"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string DataType {
            get { return dataType == null ? string.Empty : dataType; }
            set { dataType = value; }
        }
    }
}
