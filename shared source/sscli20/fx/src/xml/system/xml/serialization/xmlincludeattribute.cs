//------------------------------------------------------------------------------
// <copyright file="XmlIncludeAttribute.cs" company="Microsoft">
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

    /// <include file='doc\XmlIncludeAttribute.uex' path='docs/doc[@for="XmlIncludeAttribute"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Interface | AttributeTargets.Method, AllowMultiple=true)]
    public class XmlIncludeAttribute : System.Attribute {
        Type type;

        /// <include file='doc\XmlIncludeAttribute.uex' path='docs/doc[@for="XmlIncludeAttribute.XmlIncludeAttribute"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlIncludeAttribute(Type type) {
            this.type = type;
        }

        /// <include file='doc\XmlIncludeAttribute.uex' path='docs/doc[@for="XmlIncludeAttribute.Type"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public Type Type {
            get { return type; }
            set { type = value; }
        }
    }
}
