
//------------------------------------------------------------------------------
// <copyright file="XmlRootAttribute.cs" company="Microsoft">
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

    /// <include file='doc\XmlRootAttribute.uex' path='docs/doc[@for="XmlRootAttribute"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.ReturnValue | AttributeTargets.Class | AttributeTargets.Enum | AttributeTargets.Interface | AttributeTargets.Struct)]
    public class XmlRootAttribute : System.Attribute {
        string elementName;
        string ns;
        string dataType;
        bool nullable = true;
        bool nullableSpecified;
        
        /// <include file='doc\XmlRootAttribute.uex' path='docs/doc[@for="XmlRootAttribute.XmlRootAttribute"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlRootAttribute() {
        }
        
        /// <include file='doc\XmlRootAttribute.uex' path='docs/doc[@for="XmlRootAttribute.XmlRootAttribute1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlRootAttribute(string elementName) {
            this.elementName = elementName;
        }
        
        /// <include file='doc\XmlRootAttribute.uex' path='docs/doc[@for="XmlRootAttribute.ElementName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string ElementName {
            get { return elementName == null ? string.Empty : elementName; }
            set { elementName = value; }
        }
        
        /// <include file='doc\XmlRootAttribute.uex' path='docs/doc[@for="XmlRootAttribute.Namespace"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string Namespace {
            get { return ns; }
            set { ns = value; }
        }

        /// <include file='doc\XmlRootAttribute.uex' path='docs/doc[@for="XmlRootAttribute.DataType"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string DataType {
            get { return dataType == null ? string.Empty : dataType; }
            set { dataType = value; }
        }

        /// <include file='doc\XmlRootAttribute.uex' path='docs/doc[@for="XmlRootAttribute.IsNullable"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool IsNullable {
            get { return nullable; }
            set { 
                nullable = value; 
                nullableSpecified = true;
            }
        }

        internal bool IsNullableSpecified {
            get { return nullableSpecified; }
        }

        internal string Key {
            get { return (ns == null ? String.Empty : ns) + ":" + ElementName + ":" + nullable.ToString(); }
        }
    }
}
