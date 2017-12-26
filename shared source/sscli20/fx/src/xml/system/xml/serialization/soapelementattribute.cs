//------------------------------------------------------------------------------
// <copyright file="SoapElementAttribute.cs" company="Microsoft">
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

    /// <include file='doc\SoapElementAttribute.uex' path='docs/doc[@for="SoapElementAttribute"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Parameter | AttributeTargets.ReturnValue)]
    public class SoapElementAttribute : System.Attribute {
        string elementName;
        string dataType;
        bool nullable;
        
        /// <include file='doc\SoapElementAttribute.uex' path='docs/doc[@for="SoapElementAttribute.SoapElementAttribute"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public SoapElementAttribute() {
        }
        
        /// <include file='doc\SoapElementAttribute.uex' path='docs/doc[@for="SoapElementAttribute.SoapElementAttribute1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public SoapElementAttribute(string elementName) {
            this.elementName = elementName;
        }

        /// <include file='doc\SoapElementAttribute.uex' path='docs/doc[@for="SoapElementAttribute.ElementName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string ElementName {
            get { return elementName == null ? string.Empty : elementName; }
            set { elementName = value; }
        }

        /// <include file='doc\SoapElementAttribute.uex' path='docs/doc[@for="SoapElementAttribute.DataType"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string DataType {
            get { return dataType == null ? string.Empty : dataType; }
            set { dataType = value; }
        }

        /// <include file='doc\SoapElementAttribute.uex' path='docs/doc[@for="SoapElementAttribute.IsNullable"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool IsNullable {
            get { return nullable; }
            set { nullable = value; }
        }

    }
}
