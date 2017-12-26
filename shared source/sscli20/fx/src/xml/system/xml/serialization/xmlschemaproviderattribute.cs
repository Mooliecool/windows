
//------------------------------------------------------------------------------
// <copyright file="XmlSchemaProviderAttribute.cs" company="Microsoft">
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

    /// <include file='doc\XmlSchemaProviderAttribute.uex' path='docs/doc[@for="XmlSchemaProviderAttribute"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Interface | AttributeTargets.Struct)]
    public sealed class XmlSchemaProviderAttribute : System.Attribute {
        string methodName;
        bool any;
        
        /// <include file='doc\XmlSchemaProviderAttribute.uex' path='docs/doc[@for="XmlSchemaProviderAttribute.XmlSchemaProviderAttribute"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlSchemaProviderAttribute(string methodName) {
            this.methodName = methodName;
        }
        
        /// <include file='doc\XmlSchemaProviderAttribute.uex' path='docs/doc[@for="XmlSchemaProviderAttribute.MethodName"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public string MethodName {
            get { return methodName; }
        }

        /// <include file='doc\XmlSchemaProviderAttribute.uex' path='docs/doc[@for="XmlSchemaProviderAttribute.IsAny"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public bool IsAny {
            get { return any; }
            set {  any = value; }
        }
    }
}
