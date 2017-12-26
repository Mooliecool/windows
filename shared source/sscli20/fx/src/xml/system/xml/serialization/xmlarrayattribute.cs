//------------------------------------------------------------------------------
// <copyright file="XmlArrayAttribute.cs" company="Microsoft">
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
    
    /// <include file='doc\XmlArrayAttribute.uex' path='docs/doc[@for="XmlArrayAttribute"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Parameter | AttributeTargets.ReturnValue, AllowMultiple=false)]
    public class XmlArrayAttribute : System.Attribute {
        string elementName;
        string ns;
        bool nullable;
        XmlSchemaForm form = XmlSchemaForm.None;
        int order = -1;
        
        /// <include file='doc\XmlArrayAttribute.uex' path='docs/doc[@for="XmlArrayAttribute.XmlArrayAttribute"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlArrayAttribute() {
        }
        
        /// <include file='doc\XmlArrayAttribute.uex' path='docs/doc[@for="XmlArrayAttribute.XmlArrayAttribute1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlArrayAttribute(string elementName) {
            this.elementName = elementName;
        }
        
        /// <include file='doc\XmlArrayAttribute.uex' path='docs/doc[@for="XmlArrayAttribute.ElementName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string ElementName {
            get { return elementName == null ? string.Empty : elementName; }
            set { elementName = value; }
        }
    
        /// <include file='doc\XmlArrayAttribute.uex' path='docs/doc[@for="XmlArrayAttribute.Namespace"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string Namespace {
            get { return ns; }
            set { ns = value; }
        }

        /// <include file='doc\XmlArrayAttribute.uex' path='docs/doc[@for="XmlArrayAttribute.IsNullable"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool IsNullable {
            get { return nullable; }
            set { nullable = value; }
        }

        /// <include file='doc\XmlArrayAttribute.uex' path='docs/doc[@for="XmlArrayAttribute.Form"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlSchemaForm Form {
            get { return form; }
            set { form = value; }
        }

        /// <include file='doc\XmlArrayAttribute.uex' path='docs/doc[@for="XmlArrayAttribute.Order"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public int Order {
            get { return order; }
            set {
                if (value < 0)
                    throw new ArgumentException(Res.GetString(Res.XmlDisallowNegativeValues), "Order");
                order = value;
            }
        }
    }
 }
