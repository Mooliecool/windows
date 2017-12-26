//------------------------------------------------------------------------------
// <copyright file="XmlAnyAttributeAttribute.cs" company="Microsoft">
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

    /// <include file='doc\XmlAnyAttributeAttribute.uex' path='docs/doc[@for="XmlAnyAttributeAttribute"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Parameter | AttributeTargets.ReturnValue, AllowMultiple=false)]
    public class XmlAnyAttributeAttribute : System.Attribute {

        /// <include file='doc\XmlAnyAttributeAttribute.uex' path='docs/doc[@for="XmlAnyAttributeAttribute.XmlAnyAttributeAttribute"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlAnyAttributeAttribute() {
        }
       
    }
    
}
