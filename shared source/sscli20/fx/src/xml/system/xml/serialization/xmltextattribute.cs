//------------------------------------------------------------------------------
// <copyright file="XmlTextAttribute.cs" company="Microsoft">
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
    
    /// <include file='doc\XmlTextAttribute.uex' path='docs/doc[@for="XmlTextAttribute"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Parameter | AttributeTargets.ReturnValue)]
    public class XmlTextAttribute : System.Attribute {
        Type type;
        string dataType;

        /// <include file='doc\XmlTextAttribute.uex' path='docs/doc[@for="XmlTextAttribute.XmlTextAttribute"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlTextAttribute() {
        }
        
        /// <include file='doc\XmlTextAttribute.uex' path='docs/doc[@for="XmlTextAttribute.XmlTextAttribute1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlTextAttribute(Type type) {
            this.type = type;
        }
        
        /// <include file='doc\XmlTextAttribute.uex' path='docs/doc[@for="XmlTextAttribute.Type"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Type Type {
            get { return type; }
            set { type = value; }
        }

        /// <include file='doc\XmlTextAttribute.uex' path='docs/doc[@for="XmlTextAttribute.DataType"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string DataType {
            get { return dataType == null ? string.Empty : dataType; }
            set { dataType = value; }
        }
    }
}
